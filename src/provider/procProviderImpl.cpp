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
//
// Wolframe processor provider
//

#include "procProviderImpl.hpp"
#include "database/DBprovider.hpp"
#include "appdevel/module/runtimeEnvironmentConstructor.hpp"
#include "appdevel/module/filterBuilder.hpp"
#include "appdevel/module/ddlcompilerBuilder.hpp"
#include "appdevel/module/programTypeBuilder.hpp"
#include "appdevel/module/cppFormFunctionBuilder.hpp"
#include "appdevel/module/normalizeFunctionBuilder.hpp"
#include "appdevel/module/customDataTypeBuilder.hpp"
#include "appdevel/module/doctypeDetectorBuilder.hpp"
#include "logger-v1.hpp"
#include "utils/fileUtils.hpp"
#include "module/moduleDirectory.hpp"

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
	CombinedDoctypeDetector( const std::vector<cmdbind::DoctypeDetectorType>& dtlist)
		:m_doctypes(&dtlist),m_nof_finished(0)
	{
		std::vector<cmdbind::DoctypeDetectorType>::const_iterator di = dtlist.begin(), de = dtlist.end();
		for (; di != de; ++di)
		{
			m_detectors.push_back( cmdbind::DoctypeDetectorR( di->create()));
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
							LOG_DEBUG << "document type/format detection matches for '" << m_doctypes->at(idx).name() << "'";
							m_info = (*di)->info();
							return true;
						}
						else
						{
							//... we got a negative result
							LOG_DEBUG << "document type/format detection for '" << m_doctypes->at(idx).name() << "' returned negative result";
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
	const std::vector<cmdbind::DoctypeDetectorType>* m_doctypes;	///< list of sub document type detectors
	std::vector<cmdbind::DoctypeDetectorR> m_detectors;		///< list of all detector instances
	std::string m_lastError;					///< last error occurred
	std::vector<bool> m_finished;					///< bit field marking document type recognition termination
	std::size_t m_nof_finished;					///< count of finished document type detection processes (number of elements in m_nof_finished set to 'true')
	types::DoctypeInfoR m_info;					///< info object of the first positive match
};


ProcessorProviderImpl::ProcessorProviderImpl( const std::string& dbLabel_,
					const std::vector<config::NamedConfiguration*>& procConfig_,
					const std::vector<std::string>& programFiles_,
					const std::string& referencePath_,
					const module::ModulesDirectory* modules_)
	:m_dbLabel(dbLabel_)
	,m_db(0)
	,m_dbProvider(0)
	,m_programfiles(programFiles_)
	,m_referencePath(referencePath_)
{
	m_programfiles.insert( m_programfiles.end(), programFiles_.begin(), programFiles_.end());

	// Build the list of command handlers and runtime environments (configured objects)
	for ( std::vector< config::NamedConfiguration* >::const_iterator it = procConfig_.begin();
									it != procConfig_.end(); it++ )	{
		const module::ConfiguredBuilder* builder = modules_->getConfiguredBuilder((*it)->className());
		if ( builder )
		{
			if (builder->objectType() == ObjectConstructorBase::CMD_HANDLER_OBJECT)
			{
				ObjectConstructorBaseR constructorRef( builder->constructor());
				typedef ConfiguredObjectConstructor<cmdbind::CommandHandlerUnit> ThisConstructor;
				ThisConstructor* constructor = dynamic_cast<ThisConstructor*>( constructorRef.get());

				if (!constructor)	{
					LOG_ALERT << "Wolframe Processor Provider: '" << builder->objectClassName()
						  << "' is not a command handler";
					throw std::logic_error( "Object is not a commandHandler. See log." );
				}
				else	{
					m_cmd.push_back( CommandHandlerDef( constructor->object( **it), *it));
				}
			}
			else if (builder->objectType() == ObjectConstructorBase::RUNTIME_ENVIRONMENT_OBJECT)
			{
				ObjectConstructorBaseR constructorRef( builder->constructor());
				module::RuntimeEnvironmentConstructor* constructor = dynamic_cast<module::RuntimeEnvironmentConstructor*>( constructorRef.get());
				if (!constructor)
				{
					LOG_ALERT << "Wolframe Processor Provider: '" << builder->objectClassName()
						  << "' is not a runtime environment constructor";
					throw std::logic_error( "Object is not a runtime environment constructor. See log." );
				}
				else
				{
					langbind::RuntimeEnvironmentR env( constructor->object( **it));
					m_programs.defineRuntimeEnvironment( env);

					LOG_TRACE << "Registered runtime environment '" << env->name() << "'";
				}
			}
			else	{
				LOG_ALERT << "Wolframe Processor Provider: unknown processor type '" << (*it)->className() << "'";
				throw std::domain_error( "Unknown command handler type constructor. See log." );
			}
		}
		else	{
			LOG_ALERT << "Wolframe Processor Provider: processor provider configuration can not handle objects of this type '" << (*it)->className() << "'";
			throw std::domain_error( "Unknown configurable object for processor provider. See log." );
		}
	}

	// Build the lists of objects without configuration
	std::vector<const module::SimpleBuilder*> simpleBuilderList = modules_->getSimpleBuilderList();
	for ( std::vector<const module::SimpleBuilder*>::const_iterator it = simpleBuilderList.begin();
								it != simpleBuilderList.end(); it++ )	{
		switch( (*it)->objectType() )	{
			case ObjectConstructorBase::PROTOCOL_HANDLER_OBJECT:
			{
				ObjectConstructorBaseR constructorRef( (*it)->constructor());
				typedef SimpleObjectConstructor<cmdbind::ProtocolHandlerUnit> ThisConstructor;
				ThisConstructor* constructor = dynamic_cast<ThisConstructor*>( constructorRef.get());

				if (!constructor)	{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "' is not a protocol handler";
					throw std::logic_error( "Object is not a protocolHandler. See log." );
				}
				else
				{
					cmdbind::ProtocolHandlerUnitR unit( constructor->object());
					std::string name( unit->protocol());
					m_protocols.insert( name, unit);
				}
				break;
			}
			case ObjectConstructorBase::DOCTYPE_DETECTOR_OBJECT:
			{
				// object defines a document type/format detector
				const module::DoctypeDetectorBuilder* bld = dynamic_cast<const module::DoctypeDetectorBuilder*>( (*it));
				ObjectConstructorBaseR constructorRef( (*it)->constructor());
				module::DoctypeDetectorConstructor* dtcr = dynamic_cast<module::DoctypeDetectorConstructor*>(constructorRef.get());
				if (!dtcr || !bld)
				{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "' is not a document type/format detector";
					throw std::logic_error( "Object is not a document type/format detector. See log." );
				}
				else
				{
					try
					{
						boost::shared_ptr<cmdbind::DoctypeDetectorType> type( dtcr->object());
						m_doctypes.push_back( *type);
						LOG_TRACE << "registered document type/format detection for '" << bld->name() << "' (" << (*it)->objectClassName() << ")";
					}
					catch (const std::runtime_error& e)
					{
						LOG_ERROR << "error loading document type/format detection object: " << e.what();
					}
				}
				break;
			}
			case ObjectConstructorBase::FILTER_OBJECT:	{	// object is a filter
				ObjectConstructorBaseR constructorRef( (*it)->constructor());
				module::FilterConstructor* fltr = dynamic_cast< module::FilterConstructor* >(constructorRef.get());
				if (!fltr)	{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "' is not a filter";
					throw std::logic_error( "Object is not a filter. See log." );
				}
				else	{
					try
					{
						langbind::FilterTypeR filtertype( fltr->object());
						m_programs.defineFilterType( fltr->name(), filtertype);
						LOG_TRACE << "registered filter '" << fltr->name() << "' (" << fltr->objectClassName() << ")";
					}
					catch (const std::runtime_error& e)
					{
						LOG_ERROR << "error loading filter object module: " << e.what();
					}
				}
				break;
			}

			case ObjectConstructorBase::DDL_COMPILER_OBJECT:
			{	// object is a DDL compiler
				ObjectConstructorBaseR constructorRef( (*it)->constructor());
				module::DDLCompilerConstructor* ffo = dynamic_cast< module::DDLCompilerConstructor* >(constructorRef.get());
				if (!ffo)	{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "' is not a DDL compiler";
					throw std::logic_error( "Object is not a form function. See log." );
				}
				else {
					try
					{
						langbind::DDLCompilerR constructor( ffo->object());
						m_programs.defineFormDDL( constructor);
						LOG_TRACE << "registered '" << constructor->ddlname() << "' DDL compiler";
					}
					catch (const std::runtime_error& e)
					{
						LOG_FATAL << "Error loading DDL compiler '" << ffo->name() << "':" << e.what();
					}
				}
				break;
			}

			case ObjectConstructorBase::PROGRAM_TYPE_OBJECT:
			{	// object is a form function program type
				ObjectConstructorBaseR constructorRef( (*it)->constructor());
				module::ProgramTypeConstructor* ffo = dynamic_cast< module::ProgramTypeConstructor* >( constructorRef.get());
				if (!ffo)	{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "' is not a program type";
					throw std::logic_error( "Object is not a program type. See log." );
				}
				else {
					try
					{
						prgbind::ProgramR prgtype( ffo->object());
						m_programs.defineProgramType( prgtype);
						LOG_TRACE << "registered '" << ffo->name() << "' program type";
					}
					catch (const std::runtime_error& e)
					{
						LOG_FATAL << "Error loading program type '" << ffo->name() << "':" << e.what();
					}
				}
				break;
			}

			case ObjectConstructorBase::FORM_FUNCTION_OBJECT:
			{	// object is a form function
				ObjectConstructorBaseR constructorRef( (*it)->constructor());
				module::CppFormFunctionConstructor* ffo = dynamic_cast< module::CppFormFunctionConstructor* >(constructorRef.get());
				if (!ffo)	{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "' is not a form function";
					throw std::logic_error( "Object is not a form function. See log." );
				}
				else
				{
					try
					{
						m_programs.defineCppFormFunction( ffo->identifier(), ffo->function());
						LOG_TRACE << "registered C++ form function '" << ffo->identifier() << "'";
					}
					catch (const std::runtime_error& e)
					{
						LOG_FATAL << "Error loading form function object '" << ffo->objectClassName() << "':" << e.what();
					}
				}
				break;
			}

			case ObjectConstructorBase::NORMALIZE_FUNCTION_OBJECT:
			{	// object is a normalize function constructor
				ObjectConstructorBaseR constructorRef( (*it)->constructor());
				module::NormalizeFunctionConstructor* constructor = dynamic_cast< module::NormalizeFunctionConstructor* >(constructorRef.get());
				if (!constructor)
				{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "' is not a normalize function constructor";
					throw std::logic_error( "Object is not a normalize function constructor. See log." );
				}
				else
				{
					try {
						m_programs.defineNormalizeFunctionType( constructor->identifier(), constructor->function());
						LOG_TRACE << "registered '" << constructor->objectClassName() << "' normalize function '" << constructor->identifier() << "'";
					}
					catch (const std::runtime_error& e)
					{
						LOG_FATAL << "Error loading normalize function object '" << constructor->objectClassName() << "':" << e.what();
					}
				}
				break;
			}

			case ObjectConstructorBase::CUSTOM_DATA_TYPE_OBJECT:
			{
				ObjectConstructorBaseR constructorRef( (*it)->constructor());
				module::CustomDataTypeConstructor* constructor = dynamic_cast< module::CustomDataTypeConstructor* >(constructorRef.get());
				if (!constructor)
				{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "' is not a custom data type constructor";
					throw std::logic_error( "Object is not a custom data type constructor. See log." );
				}
				else
				{
					try {
						types::CustomDataTypeR dt( constructor->object());
						m_programs.defineCustomDataType( constructor->identifier(), dt);
						LOG_TRACE << "registered '" << constructor->objectClassName() << "' custom data type '" << constructor->identifier() << "'";
					}
					catch (const std::runtime_error& e)
					{
						LOG_FATAL << "Error loading custom data type '" << constructor->objectClassName() << "':" << e.what();
					}
				}
				break;
			}

			case ObjectConstructorBase::AUDIT_OBJECT:
			case ObjectConstructorBase::AUTHENTICATION_OBJECT:
			case ObjectConstructorBase::AUTHORIZATION_OBJECT:
			case ObjectConstructorBase::JOB_SCHEDULE_OBJECT:
			case ObjectConstructorBase::DATABASE_OBJECT:
			case ObjectConstructorBase::CMD_HANDLER_OBJECT:
			case ObjectConstructorBase::RUNTIME_ENVIRONMENT_OBJECT:
			case ObjectConstructorBase::TEST_OBJECT:
				LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
					  << "' is marked as '" << ObjectConstructorBase::objectTypeName( (*it)->objectType())
					  << "' object but has a simple object constructor";
				throw std::logic_error( "Object is not a valid simple object. See log." );
				break;
			default:
				LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
					  << "' is of an unknown object type";
				throw std::logic_error( "Object is not a valid simple object. See log." );
		}
	}
}


ProcessorProviderImpl::~ProcessorProviderImpl()
{}

bool ProcessorProviderImpl::loadPrograms()
{
	try
	{
		// load all globally defined programs:
		m_programs.loadPrograms( m_db, m_programfiles);

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
					const char* c1 = m_cmd.at(ci->second).configuration->className();
					const char* c2 = di->configuration->className();
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

bool ProcessorProviderImpl::resolveDB( const db::DatabaseProvider& db )
{
	bool rt = true;
	if ( m_db == NULL && ! m_dbLabel.empty() )	{
		m_db = db.database( m_dbLabel );
		if ( m_db )	{
			LOG_DEBUG << "Processor database: database reference '" << m_dbLabel << "' resolved";
		}
		else	{
			LOG_ALERT << "Processor database: database labeled '" << m_dbLabel << "' not found !";
			return false;
		}
	}
	m_dbProvider = &db;
	return rt;
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


db::Database* ProcessorProviderImpl::transactionDatabase() const
{
	if ( ! m_db )	{
		LOG_ALERT << "No database defined for the processor provider";
	}
	return m_db;
}

db::Transaction* ProcessorProviderImpl::transaction( const std::string& name ) const
{
	if ( m_db )
	{
		LOG_TRACE << "[provider] get transaction '" << name << "'";
		return m_db->transaction( name );
	} else	{
		LOG_ALERT << "No database defined for the processor provider";
		return NULL;
	}
}

db::Transaction* ProcessorProviderImpl::transaction( const std::string& dbname, const std::string& name) const
{
	db::Database* altdb = m_dbProvider->database( dbname);
	if (!altdb)
	{
		LOG_ERROR << "No database defined with name '" << dbname << "'";
		return 0;
	}
	LOG_TRACE << "[provider] get transaction '" << name << "' on database '" << dbname << "'";
	return altdb->transaction( name);
}

const std::string& ProcessorProviderImpl::referencePath() const
{
	return m_referencePath;
}

