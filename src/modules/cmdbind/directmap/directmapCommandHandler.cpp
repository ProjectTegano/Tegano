/************************************************************************
Copyright (C) 2011 - 2014 Project Wolframe.
All rights reserved.

This file is part of Project Wolframe.

Commercial Usage
Licensees holding valid Project Wolframe Commercial licenses may
use this file in accordance with the Project Wolframe
Commercial License Agreement provided with the Software or,
alternatively, in accordance with the terms contained
in a written agreement between the licensee and Project Wolframe.

GNU General Public License Usage
Alternatively, you can redistribute this file and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Wolframe is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
#include "directmapCommandHandler.hpp"
#include "serialize/struct/structParser.hpp"
#include "serialize/struct/structSerializer.hpp"
#include "serialize/tostringUtils.hpp"
#include "types/docmetadata.hpp"
#include "types/variant.hpp"
#include "filter/typingfilter.hpp"
#include "filter/execContextInputFilter.hpp"
#include "filter/joinfilter.hpp"
#include "logger/logger-v1.hpp"
#include <stdexcept>
#include <cstddef>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace cmdbind;
using namespace langbind;

DirectmapCommandHandler::DirectmapCommandHandler( const langbind::DirectmapCommandDescription* cmddescr, const langbind::InputFilterR& inputfilter_, const langbind::OutputFilterR& outputfilter_)
	:m_cmd(cmddescr)
	,m_state(0)
	,m_outputprinter(true)
{
	setInputFilter( inputfilter_);
	setOutputFilter( outputfilter_);
}

void DirectmapCommandHandler::initcall()
{
	if (!execContext()) throw std::logic_error( "execution context is not defined");

	// Check if we are allowed to execute the command:
	if (!m_cmd->authfunction.empty())
	{
		std::string errmsg;
		if (!execContext()->checkAuthorization( m_cmd->authfunction, m_cmd->authresource, errmsg))
		{
			throw std::runtime_error( std::string( "not authorized to execute command '") + m_cmd->cmdname + "': " + errmsg);
		}
	}
	// Initialize input form for validation if defined:
	if (m_cmd->inputform)
	{
		m_inputform.reset( new types::Form( m_cmd->inputform));
	}
	// Initialize output form for validation if defined:
	if (m_cmd->outputform)
	{
		m_outputform.reset( new types::Form( m_cmd->outputform));
	}

	// Initialize filters:
	outputfilter()->inheritMetaData( inputfilter()->getMetaDataRef());
	outputfilter()->setMetaData( m_cmd->outputmetadata);

	m_inputfilter.reset( new langbind::TypingInputFilter( inputfilter()));
	m_outputfilter.reset( new langbind::TypingOutputFilter( outputfilter()));
}

IOFilterCommandHandler::CallResult DirectmapCommandHandler::call( const char*& err)
{
	try
	{
		for (;;) switch (m_state)
		{
			case 0:
				m_state = 1;
				initcall();
				/* no break here ! */
			case 1:
				if (m_inputform.get())
				{
					m_inputform_parser.reset( new serialize::DDLStructParser( m_inputform.get()));
					m_inputform_parser->init( m_inputfilter, serialize::ValidationFlags::All);
					m_state = 2;
					/* no break here ! */;
				}
				else
				{
					m_state = 3;
					continue;
				}
			case 2:
			{
				// Fill the form with the serialized input to validate it:
				if (!m_inputform_parser->call()) return IOFilterCommandHandler::Yield;
				m_inputfilter.reset( new serialize::DDLStructSerializer( m_inputform.get()));

				// Validate the input meta data depending on what is required in th filter:
				if (!inputfilter()->checkMetaData( m_cmd->inputform->metadata()))
				{
					LOG_ERROR << "error validating input form metadata: " << m_inputfilter->getError();
					throw std::runtime_error( "input validation failed");
				}
				m_state = 3;
				/* no break here ! */
			}
			case 3:
				// Join input with additional attributes (CONTEXT)
				if (m_cmd->execContextElements.size())
				{
					//... if we have execution context arguments, we have to join them with the input as new input structure:
					TypedInputFilterR ecinput( new langbind::ExecContextInputFilter( m_cmd->execContextElements, *execContext(), ""));
					TypedInputFilterR joinedinput( new langbind::JoinInputFilter( "dmapinput", ecinput, m_inputfilter));
					m_inputfilter = joinedinput;
				}
				// Initialize the function execution context:
				m_functionclosure.reset( m_cmd->function->createClosure());
				m_functionclosure->init( execContext(), m_inputfilter);
				m_state = 4;
				/* no break here ! */
			case 4:
			{
				// Call the function:
				if (!m_functionclosure->call()) return IOFilterCommandHandler::Yield;
				langbind::TypedInputFilterR res = m_functionclosure->result();

				if (!m_cmd->has_result)
				{
					// Validate that there is no result if the function did not declare it:
					langbind::InputFilter::ElementType typ;
					types::VariantConst element;

					if (res.get() && res->getNext( typ, element))
					{
						if (typ != langbind::FilterBase::CloseTag)
						{
							LOG_WARNING << "Function called is returning a result but no RETURN declared in command. The function result is ignored";
						}
					}
					m_state = 6;
					continue;
				}
				else if (m_outputform.get())
				{
					// Validate the output with form -> Fill the output form:
					serialize::DDLStructParser formparser( m_outputform.get());
					formparser.init( res, serialize::ValidationFlags::All);
					if (!formparser.call())
					{
						throw std::runtime_error( "internal: output form serialization is not complete");
					}
					// Pass serializer of the data in the output form ro printer:
					langbind::TypedInputFilterR outputform_serialize( new serialize::DDLStructSerializer( m_outputform.get()));
					m_outputprinter.init( outputform_serialize, m_outputfilter);
					m_state = 5;
				}
				else 
				{
					// SKIP output -> Pass function result iterator directly to printer:
					m_outputprinter.init( res, m_outputfilter);
					m_state = 5;
				}
				/* no break here ! */
			}
			case 5:
			{
				if (!m_outputprinter.call()) return IOFilterCommandHandler::Yield;
				m_state = 6;
				continue;
			}
			default:
				return IOFilterCommandHandler::Ok;
		}
	}
	catch (const std::runtime_error& e)
	{
		m_errormsg = e.what();
		err = m_errormsg.c_str();
		return IOFilterCommandHandler::Error;
	}
	catch (const std::logic_error& e)
	{
		LOG_FATAL << "logic error processing request: " << e.what();
		throw e;
	}
}


