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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
/// \brief Processor provider implementation

#include "procProviderImpl.hpp"
#include "providerUtils.hpp"
#include "config/configurationObject.hpp"
#include "logger/logger-v1.hpp"
#include "utils/fileUtils.hpp"
#include "module/moduleDirectory.hpp"
#include "module/configuredObjectConstructor.hpp"
#include "module/simpleObjectConstructor.hpp"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <ostream>
#include <string>
#include <vector>

using namespace _Wolframe;
using namespace _Wolframe::proc;

class CombinedDoctypeDetector
	:public cmdbind::DoctypeDetector
{
public:
	CombinedDoctypeDetector( const std::vector<cmdbind::DoctypeDetectorTypeR>& dtlist)
		:m_doctypes(&dtlist),m_nof_finished(0)
	{
		std::vector<cmdbind::DoctypeDetectorTypeR>::const_iterator di = dtlist.begin(), de = dtlist.end();
		for (; di != de; ++di)
		{
			m_detectors.push_back( cmdbind::DoctypeDetectorR( (*di)->create()));
			m_finished.push_back( false);
		}
	}

	/// \brief Implement cmdbind::DoctypeDetector::putInput( const std::string&,std::size)
	virtual void putInput( const char* chunk, std::size_t chunksize)
	{
		std::vector<cmdbind::DoctypeDetectorR>::const_iterator di = m_detectors.begin(), de = m_detectors.end();
		std::size_t idx = 0;
		for (; di != de; ++di,++idx)
		{
			if (!m_finished.at(idx))
			{
				(*di)->putInput( chunk, chunksize);
			}
		}
	}

	/// \brief Implement cmdbind::DoctypeDetector::run()
	virtual bool run()
	{
		try
		{
			if (!m_lastError.empty()) return false;

			std::size_t idx = 0;
			std::vector<cmdbind::DoctypeDetectorR>::const_iterator di = m_detectors.begin(), de = m_detectors.end();
			for (; di != de; ++di,++idx)
			{
				if (!m_finished.at(idx))
				{
					if ((*di)->run())
					{
						if ((*di)->info().get())
						{
							//... we got a positive result
							LOG_DEBUG << "document type/format detection matches for '" << m_doctypes->at(idx)->name() << "'";
							m_info = (*di)->info();
							return true;
						}
						else
						{
							//... we got a negative result
							LOG_DEBUG << "document type/format detection for '" << m_doctypes->at(idx)->name() << "' returned negative result";
							++m_nof_finished;
							m_finished[ idx] = true;

							if (m_nof_finished == m_finished.size())
							{
								//... all results are negative
								m_lastError = "document type/format not recognized";
								return false;
							}
						}
					}
					else
					{
						const char* err = (*di)->lastError();
						if (err)
						{
							m_lastError.append( err);
							return false;
						}
						
					}
				}
			}
		}
		catch (const std::runtime_error& e)
		{
			m_lastError = std::string("exception while detecing document type: ") + e.what();
		}
		return false;
	}

	/// \brief Implement cmdbind::DoctypeDetector::lastError()
	virtual const char* lastError() const
	{
		return m_lastError.empty()?0:m_lastError.c_str();
	}

	/// \brief Implement cmdbind::DoctypeDetector::info()
	const types::DoctypeInfoR& info() const
	{
		return m_info;
	}

private:
	const std::vector<cmdbind::DoctypeDetectorTypeR>* m_doctypes;	///< list of sub document type detectors
	std::vector<cmdbind::DoctypeDetectorR> m_detectors;		///< list of all detector instances
	std::string m_lastError;					///< last error occurred
	std::vector<bool> m_finished;					///< bit field marking document type recognition termination
	std::size_t m_nof_finished;					///< count of finished document type detection processes (number of elements in m_nof_finished set to 'true')
	types::DoctypeInfoR m_info;					///< info object of the first positive match
};


ProcessorProviderImpl::ProcessorProviderImpl(
		const std::vector<config::ConfigurationObject*>& procConfig_,
		const std::vector<std::string>& programFiles_,
		const std::string& referencePath_,
		const module::ModuleDirectory* modules_)
	:m_programfiles(programFiles_)
	,m_referencePath(referencePath_)
{
	bool success = true;
	// Build the list of command handlers and runtime environments (configured objects)
	for ( std::vector<config::ConfigurationObject*>::const_iterator it = procConfig_.begin();
									it != procConfig_.end(); it++ )	{
		const module::ConfiguredObjectConstructor* constructor = modules_->getConfiguredObjectConstructor((*it)->className());
		if (constructor)
		{
			if (constructor->typeId() == module::ObjectDescription::CMD_HANDLER_OBJECT)
			{
				cmdbind::CommandHandlerUnit* obj = createConfiguredProviderObject<cmdbind::CommandHandlerUnit>( "processorProvider: ", constructor, *it);
				if (obj)
				{
					m_cmd.push_back( CommandHandlerDef( obj, *it));
				}
				else
				{
					success = false;
				}
			}
			else if (constructor->typeId() == module::ObjectDescription::RUNTIME_ENVIRONMENT_OBJECT)
			{
				langbind::RuntimeEnvironment* obj = createConfiguredProviderObject<langbind::RuntimeEnvironment>( "processorProvider: ", constructor, *it);
				if (obj)
				{
					langbind::RuntimeEnvironmentR objref( obj);
					m_programs.defineRuntimeEnvironment( objref);
				}
				else
				{
					success = false;
				}
			}
			else
			{
				LOG_ERROR << "internal: unexpected type of configured object in processor provider";
				success = false;
			}
		}
		else
		{
			throw std::logic_error( "internal: unexpected type of configured object in processor provider" );
		}
	}

	// Build the lists of objects without configuration
	std::vector<const module::SimpleObjectConstructor*> simpleObjectConstructorList = modules_->getSimpleObjectConstructorList();
	for ( std::vector<const module::SimpleObjectConstructor*>::const_iterator it = simpleObjectConstructorList.begin();
								it != simpleObjectConstructorList.end(); it++ )	{
		const module::SimpleObjectConstructor* constructor = *it;
		switch (constructor->typeId())
		{
			case module::ObjectDescription::PROTOCOL_HANDLER_OBJECT:
			{
				cmdbind::ProtocolHandlerUnit* obj = createSimpleProviderObject<cmdbind::ProtocolHandlerUnit>( "ProcessorProvider: ", constructor);
				if (obj)
				{
					cmdbind::ProtocolHandlerUnitR unit( obj);
					std::string name( unit->protocol());
					m_protocols.insert( name, unit);
					LOG_TRACE << "ProcessorProvider: registered protocol handler '" << constructor->className() << ")";
				}
				break;
			}
			case module::ObjectDescription::DOCTYPE_DETECTOR_OBJECT:
			{
				cmdbind::DoctypeDetectorType* obj = createSimpleProviderObject<cmdbind::DoctypeDetectorType>( "ProcessorProvider: ", constructor);
				if (obj)
				{
					boost::shared_ptr<cmdbind::DoctypeDetectorType> objref( obj);
					m_doctypes.push_back( objref);
					LOG_TRACE << "ProcessorProvider: registered document type/format detection '" << constructor->className() << ")";
				}
				break;
			}
			case module::ObjectDescription::FILTER_OBJECT:
			{
				langbind::FilterType* obj = createSimpleProviderObject<langbind::FilterType>( "ProcessorProvider: ", constructor);
				if (obj)
				{
					boost::shared_ptr<langbind::FilterType> objref( obj);
					m_programs.defineFilterType( obj->name(), objref);
					LOG_TRACE << "ProcessorProvider: registered filter type '" << constructor->className() << ")";
				}
				break;
			}

			case module::ObjectDescription::DDL_COMPILER_OBJECT:
			{
				langbind::DDLCompiler* obj = createSimpleProviderObject<langbind::DDLCompiler>( "ProcessorProvider: ", constructor);
				if (obj)
				{
					boost::shared_ptr<langbind::DDLCompiler> objref( obj);
					m_programs.defineFormDDL( objref);
					LOG_TRACE << "ProcessorProvider: registered DDL compiler '" << constructor->className() << ")";
				}
				break;
			}

			case module::ObjectDescription::PROGRAM_TYPE_OBJECT:
			{
				proc::Program* obj = createSimpleProviderObject<proc::Program>( "ProcessorProvider: ", constructor);
				if (obj)
				{
					boost::shared_ptr<proc::Program> objref( obj);
					m_programs.defineProgramType( objref);
					LOG_TRACE << "ProcessorProvider: registered program type '" << constructor->className() << ")";
				}
				break;
			}

			case module::ObjectDescription::FORM_FUNCTION_OBJECT:
			{
				serialize::CppFormFunction* obj = createSimpleProviderObject<serialize::CppFormFunction>( "ProcessorProvider: ", constructor);
				if (obj)
				{
					boost::shared_ptr<serialize::CppFormFunction> objref( obj);
					m_programs.defineFormFunction( obj->name(), objref);
					LOG_TRACE << "ProcessorProvider: registered C++ form function '" << constructor->className() << ")";
				}
				break;
			}

			case module::ObjectDescription::NORMALIZE_FUNCTION_OBJECT:
			{	// object is a normalize function constructor
				types::NormalizeFunctionType* obj = createSimpleProviderObject<types::NormalizeFunctionType>( "ProcessorProvider: ", constructor);
				if (obj)
				{
					boost::shared_ptr<types::NormalizeFunctionType> objref( obj);
					m_programs.defineNormalizeFunctionType( obj->name(), objref);
					LOG_TRACE << "ProcessorProvider: registered normalize function type '" << constructor->className() << ")";
				}
				break;
			}

			case module::ObjectDescription::CUSTOM_DATA_TYPE_OBJECT:
			{
				types::CustomDataType* obj = createSimpleProviderObject<types::CustomDataType>( "ProcessorProvider: ", constructor);
				if (obj)
				{
					boost::shared_ptr<types::CustomDataType> objref( obj);
					m_programs.defineCustomDataType( obj->name(), objref);
					LOG_TRACE << "ProcessorProvider: registered custom data type '" << constructor->className() << ")";
				}
				break;
			}

			case module::ObjectDescription::AUDIT_OBJECT:
			case module::ObjectDescription::AUTHENTICATION_OBJECT:
			case module::ObjectDescription::AUTHORIZATION_OBJECT:
			case module::ObjectDescription::JOB_SCHEDULE_OBJECT:
			case module::ObjectDescription::DATABASE_OBJECT:
			case module::ObjectDescription::CMD_HANDLER_OBJECT:
			case module::ObjectDescription::RUNTIME_ENVIRONMENT_OBJECT:
			case module::ObjectDescription::TEST_OBJECT:
				LOG_ERROR << "ProcessorProvider: object '" << constructor->className()
					  << "' is marked as '" << constructor->typeIdName()
					  << "' object but has a simple object constructor";
				success = false;
				break;
			default:
				LOG_ERROR << "ProcessorProvider: object '" << constructor->className()
					  << "' is of an unknown object type";
				success = false;
				break;
		}
	}
	if (!success)
	{
		throw std::runtime_error("failed to create processor provider context (see logs)");
	}
}


ProcessorProviderImpl::~ProcessorProviderImpl()
{}

bool ProcessorProviderImpl::loadPrograms()
{
	try
	{
		// load all globally defined programs:
		m_programs.loadPrograms( m_programfiles);

		// load command handler programs and register commands 
		std::vector<CommandHandlerDef>::const_iterator di = m_cmd.begin(), de = m_cmd.end();
		for (; di != de; ++di)
		{
			if (!di->unit->loadPrograms( this))
			{
				LOG_ERROR << "failed to load command handler programs";
			}
			const std::vector<std::string>& cmds = di->unit->commands();
			for (std::vector<std::string>::const_iterator cmdIt = cmds.begin(); cmdIt != cmds.end(); cmdIt++)
			{
				types::keymap<std::size_t>::const_iterator ci = m_cmdMap.find( *cmdIt);
				if (ci != m_cmdMap.end())
				{
					const std::string& c1 = m_cmd.at(ci->second).configuration->className();
					const std::string& c2 = di->configuration->className();
					LOG_ERROR << "duplicate definition of command '" << *cmdIt << "' (in '" << c1 << "' and in '" << c2 << "')";
					throw std::runtime_error( "duplicate command definition");
				}
				else
				{
					m_cmdMap[ *cmdIt] = m_cmd.size()-1;
				}
				LOG_TRACE << "Command '" << *cmdIt << "' registered for '" << di->configuration->className() << "' command handler";
			}
		}
		return true;
	}
	catch (const std::runtime_error& e)
	{
		LOG_ERROR << "failed to load programs: " << e.what();
		return false;
	}
}

void ProcessorProviderImpl::addDatabase( const db::Database* db_)
{
	m_programs.defineDatabase( db_);
}

const langbind::AuthorizationFunction* ProcessorProviderImpl::authorizationFunction( const std::string& name) const
{
	return m_programs.getAuthorizationFunction( name);
}

const langbind::AuditFunction* ProcessorProviderImpl::auditFunction( const std::string& name) const
{
	return m_programs.getAuditFunction( name);
}

const types::NormalizeFunction* ProcessorProviderImpl::normalizeFunction( const std::string& name) const
{
	return m_programs.getNormalizeFunction( name);
}

const types::NormalizeFunctionType* ProcessorProviderImpl::normalizeFunctionType( const std::string& name) const
{
	return m_programs.getNormalizeFunctionType( name);
}

const langbind::FormFunction* ProcessorProviderImpl::formFunction( const std::string& name) const
{
	LOG_TRACE << "[provider] get function '" << name << "'";
	return m_programs.getFormFunction( name);
}

const types::FormDescription* ProcessorProviderImpl::formDescription( const std::string& name) const
{
	LOG_TRACE << "[provider] get form description '" << name << "'";
	return m_programs.getFormDescription( name);
}

const langbind::FilterType* ProcessorProviderImpl::filterType( const std::string& name) const
{
	return m_programs.getFilterType( name);
}

const types::CustomDataType* ProcessorProviderImpl::customDataType( const std::string& name) const
{
	LOG_TRACE << "[provider] get custom data type '" << name << "'";
	return m_programs.getCustomDataType( name);
}

cmdbind::DoctypeDetector* ProcessorProviderImpl::doctypeDetector() const
{
	if (m_doctypes.empty()) return 0;
	return new CombinedDoctypeDetector( m_doctypes);
}

cmdbind::CommandHandler* ProcessorProviderImpl::cmdhandler( const std::string& command, const std::string& docformat) const
{
	types::keymap<std::size_t>::const_iterator ci = m_cmdMap.find( command);
	if (ci == m_cmdMap.end())
	{
		return 0;
	}
	cmdbind::CommandHandlerUnit* unit = m_cmd.at( ci->second).unit.get();
	return unit->createCommandHandler( command, docformat);
}

bool ProcessorProviderImpl::hasCommand( const std::string& command) const
{
	return m_cmdMap.find( command) != m_cmdMap.end();
}

cmdbind::ProtocolHandler* ProcessorProviderImpl::protocolHandler( const std::string& protocol) const
{
	types::keymap<cmdbind::ProtocolHandlerUnitR>::const_iterator pi = m_protocols.find( protocol);
	if (pi == m_protocols.end()) return 0;
	return pi->second->createProtocolHandler();
}

bool ProcessorProviderImpl::hasProtocol( const std::string& protocol) const
{
	return m_protocols.find( protocol) != m_protocols.end();
}

const db::Database* ProcessorProviderImpl::database( const std::string& name) const
{
	return m_programs.database( name);
}

const std::string& ProcessorProviderImpl::referencePath() const
{
	return m_referencePath;
}

