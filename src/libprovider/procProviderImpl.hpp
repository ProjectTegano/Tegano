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
/// \file procProviderImpl.hpp
/// \brief Processor provider implementation for language bindings

#ifndef _PROCESSOR_PROVIDER_IMPLEMENTATION_HPP_INCLUDED
#define _PROCESSOR_PROVIDER_IMPLEMENTATION_HPP_INCLUDED
#include "processor/procProviderInterface.hpp"
#include "config/configurationObject.hpp"
#include "programLibraryImpl.hpp"
#include "database/database.hpp"
#include "cmdbind/commandHandler.hpp"
#include "cmdbind/doctypeDetector.hpp"
#include "cmdbind/protocolHandler.hpp"
#include "types/keymap.hpp"
#include <vector>
#include <map>
#include <boost/noncopyable.hpp>

namespace _Wolframe {
namespace db {
/// \brief Forward declaration
class Database;
}
namespace proc {

/// \class ProcessorProviderImpl
/// \brief Processor provider, the class that provides access to configured global objects to processors
class ProcessorProviderImpl
	:public ProcessorProviderInterface
	,private boost::noncopyable
{
public:
	/// \brief Constructor
	ProcessorProviderImpl( const std::vector<config::ConfigurationObject*>& procConfig_,
				const std::vector<std::string>& programFiles_,
				const std::string& referencePath_,
				const module::ModuleDirectory* modules_);
	/// \brief Destructor
	virtual ~ProcessorProviderImpl();

	/// \brief Define a database to be used for transactions
	/// \note The order of definition defines the priority
	void addDatabase( const db::Database* database);

	/// \brief Load all configured programs
	bool loadPrograms();

	/// \brief Create a new command handler for a command and for a document format (e.g. XML,JSON,...)
	virtual cmdbind::CommandHandler* cmdhandler( const std::string& command, const std::string& docformat) const;

	/// \brief Create a new protocol handler
	virtual cmdbind::ProtocolHandler* protocolHandler( const std::string& protocol) const;
	/// \brief Find out if a protocol with a specific name exists
	virtual bool hasProtocol( const std::string& protocol) const;
	/// \brief Find out if a command with a specific name exists
	virtual bool hasCommand( const std::string& command) const;

	/// \brief Get a database for transactions
	virtual const db::Database* database( const std::string& name) const;

	/// \brief Get a reference to an authorization function identified by name
	virtual const langbind::AuthorizationFunction* authorizationFunction( const std::string& name) const;

	/// \brief Get a reference to an audit function identified by name
	virtual const langbind::AuditFunction* auditFunction( const std::string& name) const;

	/// \brief Get a reference to a normalization function identified by name
	virtual const types::NormalizeFunction* normalizeFunction( const std::string& name) const;

	/// \brief Get a reference to a normalization function type identified by name
	virtual const types::NormalizeFunctionType* normalizeFunctionType( const std::string& name) const;

	/// \brief Get a reference to a form function type identified by name
	virtual const langbind::FormFunction* formFunction( const std::string& name) const;

	/// \brief Get a reference to a form description identified by name
	virtual const types::FormDescription* formDescription( const std::string& name) const;

	/// \brief Get a reference to a filter type identified by name
	virtual const langbind::FilterType* filterType( const std::string& name) const;

	/// \brief Get a reference to a custom data type identified by name
	virtual const types::CustomDataType* customDataType( const std::string& name) const;

	/// \brief Create a new document type and format detector (defined in modules)
	/// \return a document type and format detector reference allocated (owned and deleted by the caller)
	virtual cmdbind::DoctypeDetector* doctypeDetector() const;

	/// \brief Get the application reference path for local path expansion
	virtual const std::string& referencePath() const;

private:
	/// \class CommandHandlerDef
	/// \brief Definition of a command handler with its configuration
	class CommandHandlerDef
	{
	public:
		/// \brief Default constructor
		CommandHandlerDef()
			:configuration(0){}
		/// \brief Copy constructor
		CommandHandlerDef( const CommandHandlerDef& o)
			:unit(o.unit),configuration(o.configuration){}
		/// \brief Constructor
		CommandHandlerDef( cmdbind::CommandHandlerUnit* unit_, const config::ConfigurationObject* configuration_)
			:unit(unit_),configuration(configuration_){}
		/// \brief Destructor
		~CommandHandlerDef(){}

	public:
		cmdbind::CommandHandlerUnitR unit;			///< command handler unit to instantiate new command handlers
		const config::ConfigurationObject* configuration;	///< command handler configuration
	};

	std::vector<CommandHandlerDef> m_cmd;				///< list of defined command handlers
	types::keymap<std::size_t> m_cmdMap;				///< map of command names to indices in 'm_cmd'

	types::keymap<cmdbind::ProtocolHandlerUnitR> m_protocols;	///< map protocol identifiers to handler units
	std::vector<cmdbind::DoctypeDetectorTypeR> m_doctypes;		///< list of document type detectors loaded from modules
	std::vector<std::string> m_programfiles;			///< list of all programs to load
	proc::ProgramLibraryImpl m_programs;				///< program library
	std::string m_referencePath;					///< application reference path
};

}} // namespace _Wolframe::proc

#endif // _PROCESSOR_PROVIDER_HPP_INCLUDED
