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
///\file inputfilterImpl.cpp
///\brief Implementation of input filter abstraction for the cjson library
#include "inputfilterImpl.hpp"
#include "utils/parseUtils.hpp"
#include "utils/sourceLineInfo.hpp"
#include "types/string.hpp"
#include "logger/logger-v1.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <sstream>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

bool InputFilterImpl::getValue( const char* id, std::string& val) const
{
	return Parent::getValue( id, val);
}

const types::DocMetaData* InputFilterImpl::getMetaData()
{
	if (!m_root.get())
	{
		setState( EndOfMessage);
		return 0;
	}
	return getMetaDataRef().get();
}

bool InputFilterImpl::setValue( const char* id, const std::string& value)
{
	return Parent::setValue( id, value);
}

static types::String::Encoding guessCharsetEncoding( const void* content, std::size_t contentsize)
{
	types::String::EncodingClass::Id cl = types::String::guessEncoding( (const char*)content, contentsize);
	switch (cl)
	{
		case types::String::EncodingClass::FAIL:
			throw std::runtime_error( "failed to guess charset encoding for JSON filter");
		case types::String::EncodingClass::NONE:
			throw std::runtime_error( "cannot guess charset encoding for JSON filter");
		case types::String::EncodingClass::UCS1:
			return types::String::UTF8;
		case types::String::EncodingClass::UCS2BE:
			return types::String::UTF16BE;
		case types::String::EncodingClass::UCS2LE:
			return types::String::UTF16LE;
		case types::String::EncodingClass::UCS4BE:
			return types::String::UCS4BE;
		case types::String::EncodingClass::UCS4LE:
			return types::String::UCS4LE;
	}
	throw std::runtime_error( "cannot guess charset encoding for JSON filter");
}

boost::shared_ptr<cJSON> InputFilterImpl::parse( const std::string& content)
{
	if (m_encattr.encoding == types::String::UTF8)
	{
		m_content = content;
	}
	else
	{
		m_content = types::StringConst( content.c_str(), content.size(), m_encattr.encoding, m_encattr.codepage).tostring();
	}
	cJSON_Context ctx;
	cJSON* pp = cJSON_Parse( &ctx, m_content.c_str());
	if (!pp)
	{
		if (!ctx.errorptr) throw std::bad_alloc();
		utils::SourceLineInfo pos = utils::getSourceLineInfo( m_content.begin(), m_content.begin() + (ctx.errorptr - m_content.c_str()));

		std::string err( ctx.errorptr);
		if (err.size() > 80)
		{
			err.resize( 80);
			err.append( "...");
		}
		throw std::runtime_error( std::string( "error in JSON content at line ") + boost::lexical_cast<std::string>(pos.line()) + " column " + boost::lexical_cast<std::string>(pos.column()) + " at '" + err + "'");
	}
	return boost::shared_ptr<cJSON>( pp, cJSON_Delete);
}

void InputFilterImpl::putInput( const void* content, std::size_t contentsize, bool end)
{
	try
	{
		setState( Open);
		m_content.append( (const char*)content, contentsize);
		if (end)
		{
			std::string origcontent( m_content);
			if (m_root.get()) throw std::logic_error( "bad operation on JSON input filter: put input after end");
			m_encattr.encoding = guessCharsetEncoding( m_content.c_str(), m_content.size());
			m_encattr.codepage = 0;
			if (m_encattr.encoding != types::String::UTF8)
			{
				setAttribute( "encoding", m_encattr.encodingName());
			}
			m_root = parse( origcontent);
			m_firstnode = m_root.get();
			int nof_docattributes = 0;
			bool encodingParsed = false;
			bool doctypeParsed = false;
	
			if (!m_firstnode->string && !m_firstnode->valuestring && !m_firstnode->next && m_firstnode->type == cJSON_Object)
			{
				//CJSON creates a toplevel object for multiple root nodes:
				m_firstnode = m_firstnode->child;
			}

			while (m_firstnode)
			{
				if (m_firstnode->string && m_firstnode->valuestring)
				{
					if (boost::iequals("-doctype",m_firstnode->string))
					{
						++nof_docattributes;
						if (doctypeParsed) throw std::runtime_error("duplicate 'doctype' definition");
						setDoctype( m_firstnode->valuestring);
						setAttribute( "doctype", m_firstnode->valuestring);
						m_firstnode = m_firstnode->next;
						continue;
					}
					else if (boost::iequals("-encoding", m_firstnode->string))
					{
						++nof_docattributes;
						types::String::EncodingAttrib ea = types::String::getEncodingFromName( m_firstnode->valuestring);
						setAttribute( "encoding", m_firstnode->valuestring);

						if (m_encattr.encoding != ea.encoding || ea.codepage != 0)
						{
							// ... encoding different than guessed. Parse again
							if (encodingParsed) throw std::runtime_error( "duplicate 'encoding' definition");
							encodingParsed = true;
							m_encattr = ea;
							m_root = parse( origcontent);
							m_firstnode = m_root.get();
							while (m_firstnode && nof_docattributes--) m_firstnode = m_firstnode->next;
						}
						else
						{
							m_firstnode = m_firstnode->next;
						}
						continue;
					}
				}
				break;
			}
			if (m_firstnode)
			{
				m_stk.push_back( StackElement( m_firstnode));
			}
			LOG_DEBUG << "[cjson input] document meta data: {" << getMetaDataRef()->tostring() << "}";
			setState( Open);
		}
	}
	catch (const std::runtime_error& err)
	{
		setState( InputFilter::Error, err.what());
		return;
	}
	catch (const std::bad_alloc& err)
	{
		setState( InputFilter::Error, "out of memory");
		return;
	}
	catch (const std::logic_error& err)
	{
		LOG_FATAL << "logic error in JSON filer: " << err.what();
		setState( InputFilter::Error, "logic error in libxml2 filer. See logs");
		return;
	}
}

bool InputFilterImpl::getNodeData( const cJSON* nd, const char*& eename, InputFilter::ElementType& eetype, const void*& ee, std::size_t& eesize) const
{
	if (!nd) return false;

	if (nd->string)
	{
		if (nd->string[0] == '-')
		{
			eetype = InputFilter::Attribute;
			eename = nd->string+1;
		}
		else if (nd->string[0] == '#')
		{
			if (std::strcmp( nd->string, "#text") == 0)
			{
				eetype = InputFilter::Value;
				eename = 0;
			}
			else
			{
				return false;
			}
		}
		else
		{
			eename = nd->string;
			if (nd->child)
			{
				int ndtype = nd->type & 0x7F;
				if (ndtype == cJSON_Object)
				{
					eetype = InputFilter::OpenTag;
					ee = eename;
					eesize = std::strlen(eename);
					eename = 0;
					return true;
				}
				else if (ndtype == cJSON_Array)
				{
					eetype = InputFilter::OpenTagArray;
					ee = eename;
					eesize = std::strlen(eename);
					eename = 0;
					return true;
				}
				else
				{
					eetype = InputFilter::Value;
				}
			}
			else
			{
				eetype = InputFilter::Value;
			}
		}
	}
	else
	{
		eetype = InputFilter::Value;
		eename = 0;
	}
	switch (nd->type & 0x7F)
	{
		case cJSON_False:
			ee = "false";
			eesize = 5;
			return true;
		case cJSON_True:
			ee = "true";
			eesize = 4;
			return true;
		case cJSON_NULL:
			return false;

		case cJSON_String:
		case cJSON_Number:
			if (!nd->valuestring)
			{
				throw std::runtime_error( "internal: value node without string value found in JSON structure");
				return false;
			}
			ee = nd->valuestring;
			eesize = nd->valuestring?std::strlen( nd->valuestring):0;
			return true;
		case cJSON_Array:
		case cJSON_Object:
			return false;
		default:
			throw std::runtime_error( "internal: memory corruption found in JSON structure");
	}
}

std::string InputFilterImpl::stackdump() const
{
	std::ostringstream rt;
	unsigned int ii = m_stk.size();
	const char* eename;
	InputFilter::ElementType eetype;
	const void* ee;
	std::size_t eesize;

	std::vector<StackElement>::const_iterator si=m_stk.begin(), se=m_stk.end();
	for (; si != se; ++si,--ii)
	{
		rt << "[" << ii << "] " << StackElement::stateName( si->m_state);
		if (si->m_tag)
		{
			rt << " #" << si->m_tag;
		}
		if (getNodeData( si->m_node, eename, eetype, ee, eesize))
		{
			rt << " " << InputFilter::elementTypeName( eetype);
			if (eename) rt << " " << eename;
			if (ee) rt << " '" << std::string( (const char*)ee, eesize) << "'";
		}
		rt << " ";
	}
	return rt.str();
}

bool InputFilterImpl::getNext( InputFilter::ElementType& type, const void*& element, std::size_t& elementsize)
{
try
{
	const char* eename = 0;

	if (!m_root.get())
	{
		if (state() != Error)
		{
			setState( EndOfMessage);
		}
		return false;
	}
	setState( Open);
	while (!m_stk.empty())
	{
		const cJSON* nd = m_stk.back().m_node;
		if (!nd)
		{
			setState( Error, "internal: invalid node in JSON structure");
			return false;
		}
		for (;;)
		{
			LOG_DATA << "[json input filter] State " << StackElement::stateName( m_stk.back().m_state);
			LOG_DATA2<< "[json input filter] Stack " << stackdump();

			switch (m_stk.back().m_state)
			{
				case StackElement::StateValue:
					if (getNodeData( nd, eename, type, element, elementsize))
					{
						switch (type)
						{
							case InputFilter::Attribute:
								m_stk.back().m_state = StackElement::StateAttributeValue;
								m_buffered_element = element;
								m_buffered_element_size = elementsize;
								element = eename;
								elementsize = std::strlen(eename);

								LOG_DATA << "[json input filter] get next " << FilterBase::elementTypeName( type) << " '" << std::string( (const char*)element, elementsize) << "'";
								return true;

							case InputFilter::Value:
								if (eename)
								{
									m_stk.back().m_state = StackElement::StateContentValue;
									m_buffered_element = element;
									m_buffered_element_size = elementsize;
									element = eename;
									elementsize = std::strlen(eename);
									type = InputFilter::OpenTag;
								}
								else
								{
									m_stk.back().m_state = StackElement::StateNext;
								}
								LOG_DATA << "[json input filter] get next " << FilterBase::elementTypeName( type) << " '" << std::string( (const char*)element, elementsize) << "'";
								return true;

							case InputFilter::OpenTagArray:
								m_stk.back().m_state = StackElement::StateNext;
								if (nd->child)
								{
									m_stk.push_back( StackElement( nd->child, (const char*)element));
									m_stk.back().m_state = StackElement::StateValue;
									nd = m_stk.back().m_node;
								}

								LOG_DATA << "[json input filter] get next " << FilterBase::elementTypeName( type) << " '" << std::string( (const char*)element, elementsize) << "'";
								return true;

							case InputFilter::OpenTag:
								m_stk.back().m_state = StackElement::StateNext;
								if (nd->child)
								{
									bool withTags = ((const char*)element != m_stk.back().m_tag);
									m_stk.push_back( StackElement( nd->child));
									m_stk.back().m_state = StackElement::StateValue;
									nd = m_stk.back().m_node;
									if (!withTags)
									{
										m_stk.back().m_visible = false;
										continue;
									}
								}
								LOG_DATA << "[json input filter] get next " << FilterBase::elementTypeName( type) << " '" << std::string( (const char*)element, elementsize) << "'";
								return true;

							case InputFilter::CloseTag:
								throw std::logic_error("illegal return type value returned by getNodeData");
						}
					}
					else
					{
						m_stk.back().m_state = StackElement::StateNext;
						if (nd->child)
						{
							m_stk.push_back( StackElement( nd->child));
							m_stk.back().m_visible = false;
							nd = m_stk.back().m_node;
						}
					}
					continue;

				case StackElement::StateAttributeValue:
					m_stk.back().m_state = StackElement::StateNext;

					element = m_buffered_element;
					m_buffered_element = 0;
					elementsize = m_buffered_element_size;
					m_buffered_element_size = 0;
					type = InputFilter::Value;
					LOG_DATA << "[json input filter] get next " << FilterBase::elementTypeName( type) << " '" << std::string( (const char*)element, elementsize) << "'";
					return true;

				case StackElement::StateContentValue:
					m_stk.back().m_state = StackElement::StateContentClose;
					
					element = m_buffered_element;
					m_buffered_element = 0;
					elementsize = m_buffered_element_size;
					m_buffered_element_size = 0;
					type = InputFilter::Value;
					LOG_DATA << "[json input filter] get next " << FilterBase::elementTypeName( type) << " '" << std::string( (const char*)element, elementsize) << "'";
					return true;

				case StackElement::StateContentClose:
					m_stk.back().m_state = StackElement::StateNext;
					type = InputFilter::CloseTag;
					element = 0;
					elementsize = 0;
					LOG_DATA << "[json input filter] get next " << FilterBase::elementTypeName( type);
					return true;

				case StackElement::StateNext:
					if (nd->next)
					{
						if (m_stk.back().m_tag)
						{
							nd = m_stk.back().m_node = nd->next;
							m_stk.back().m_state = StackElement::StateReopenArray;
							if (m_stk.back().m_visible)
							{
								type = InputFilter::CloseTag;
								element = 0;
								elementsize = 0;
								LOG_DATA << "[json input filter] get next " << FilterBase::elementTypeName( type);
								return true;
							}
							else
							{
								continue;
							}
						}
						else
						{
							nd = m_stk.back().m_node = nd->next;
							m_stk.back().m_state = StackElement::StateValue;
							continue;
						}
					}
					else
					{
						bool doClose = m_stk.back().m_visible;
						m_stk.pop_back();

						if (doClose)
						{
							type = InputFilter::CloseTag;
							element = 0;
							elementsize = 0;
							LOG_DATA << "[json input filter] get next " << FilterBase::elementTypeName( type) << " '" << std::string( (const char*)element, elementsize) << "'";
							return true;
						}
						else
						{
							nd = m_stk.back().m_node;
							continue;
						}
					}

				case StackElement::StateReopenArray:
					m_stk.back().m_state = StackElement::StateValue;
					type = InputFilter::OpenTagArray;
					element = m_stk.back().m_tag;
					elementsize = std::strlen( (const char*)element);
					return true;
			}
		}
	}
	if (!m_done)
	{
		//... emit final close
		type = InputFilter::CloseTag;
		element = 0;
		elementsize = 0;
		m_done = true;
		return true;
	}
	return false;
}
catch (const std::runtime_error& err)
{
	setState( InputFilter::Error, err.what());
	return false;
}
catch (const std::bad_alloc& err)
{
	setState( InputFilter::Error, "out of memory");
	return false;
}
catch (const std::logic_error& err)
{
	LOG_FATAL << "logic error in JSON filer: " << err.what();
	setState( InputFilter::Error, "logic error in libxml2 filer. See logs");
	return false;
}
}

