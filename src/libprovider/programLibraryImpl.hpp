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
/// \brief Program library implementation
/// \file libprovider/programLibraryImpl.hpp

#ifndef _Wolframe_proc_PROGRAM_LIBRARY_IMPLEMENTATION_HPP_INCLUDED
#define _Wolframe_proc_PROGRAM_LIBRARY_IMPLEMENTATION_HPP_INCLUDED
#include "filter/filter.hpp"
#include "serialize/cppFormFunction.hpp"
#include "langbind/ddlCompilerInterface.hpp"
#include "langbind/formFunction.hpp"
#include "langbind/runtimeEnvironment.hpp"
#include "langbind/authorizationFunction.hpp"
#include "langbind/auditFunction.hpp"
#include "types/form.hpp"
#include "types/normalizeFunction.hpp"
#include "types/customDataType.hpp"
#include "processor/programLibrary.hpp"
#include "processor/program.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace db {
	class Database;
}
namespace proc {

/// \class ProgramLibraryImpl
/// \brief Class implementing the program library with all programs loaded
class ProgramLibraryImpl
	:public ProgramLibrary
{
public:
	/// \brief Default constructor
	ProgramLibraryImpl();
	/// \brief Copy constructor
	ProgramLibraryImpl( const ProgramLibraryImpl& o);

	/// \brief Destructor
	virtual ~ProgramLibraryImpl();

	/// \brief Define an authorization function
	virtual void defineAuthorizationFunction( const std::string& name, const langbind::AuthorizationFunctionR& f);
	/// \brief Define an audit function
	virtual void defineAuditFunction( const std::string& name, const langbind::AuditFunctionR& f);
	/// \brief Define an ordinary form function
	virtual void defineFormFunction( const std::string& name, const langbind::FormFunctionR& f);
	/// \brief Define a runtime environment
	virtual void defineRuntimeEnvironment( const langbind::RuntimeEnvironmentR& env);
	/// \brief Define a form structure that is referenced as unexpanded indirection
	virtual void definePrivateForm( const types::FormDescriptionR& f);
	/// \brief Define a form
	virtual void defineForm( const std::string& name, const types::FormDescriptionR& f);
	/// \brief Define a normalization function
	virtual void defineNormalizeFunction( const std::string& name, const types::NormalizeFunctionR& f);
	/// \brief Define a normalization function type
	virtual void defineNormalizeFunctionType( const std::string& name, const types::NormalizeFunctionTypeR& ftype);
	/// \brief Define a custom data type
	virtual void defineCustomDataType( const std::string& name, const types::CustomDataTypeR& t);
	/// \brief Define a data definition language
	virtual void defineFormDDL( const langbind::DDLCompilerR& c);
	/// \brief Define a filter type
	virtual void defineFilterType( const std::string& name, const langbind::FilterTypeR& f);
	/// \brief Define a program type
	virtual void defineProgramType( const ProgramR& prg);

	/// \brief Get the map for DDLs to map types to normalizer call sequences
	virtual const types::NormalizeFunctionMap* formtypemap() const;
	/// \brief Get a custom data type defined by name
	virtual const types::CustomDataType* getCustomDataType( const std::string& name) const;
	/// \brief Get a normalization function type defined by name
	virtual const types::NormalizeFunctionType* getNormalizeFunctionType( const std::string& name) const;

	/// \brief Get a form description
	virtual const types::FormDescription* getFormDescription( const std::string& name) const;
	/// \brief Get the list of all forms defined
	virtual std::vector<std::string> getFormNames() const;

	/// \brief Get an authorization function by name
	virtual const langbind::AuthorizationFunction* getAuthorizationFunction( const std::string& name) const;
	/// \brief Get an audit function by name
	virtual const langbind::AuditFunction* getAuditFunction( const std::string& name) const;
	/// \brief Get a form function by name
	virtual const langbind::FormFunction* getFormFunction( const std::string& name) const;
	/// \brief Get a normalizer function by name
	virtual const types::NormalizeFunction* getNormalizeFunction( const std::string& name) const;
	/// \brief Get a filter type by name
	virtual const langbind::FilterType* getFilterType( const std::string& name) const;

	/// \brief Define a database
	virtual void defineDatabase( const db::Database* database_);
	/// \brief Get a database by name
	virtual const db::Database* database( const std::string& id) const;
	/// \brief Get the list of databases (first=id, second=type)
	virtual std::vector<const db::Database*> databaseList() const;

	/// \brief Load all programs passed in 'filenames'
	virtual void loadPrograms( const std::vector<std::string>& filenames);

private:
	class NormalizeFunctionMap
		:public types::NormalizeFunctionMap
	{
	public:
		virtual ~NormalizeFunctionMap(){}
		virtual const types::NormalizeFunction* get( const std::string& name) const
		{
			types::keymap<types::NormalizeFunctionR>::const_iterator fi = m_impl.find( name);
			if (fi == m_impl.end()) return 0;
			return fi->second.get();
		}
		void define( const std::string& name, const types::NormalizeFunctionR& f)
		{
			m_impl.insert( name, f);
		}
	private:
		types::keymap<types::NormalizeFunctionR> m_impl;
	};
	
private:
	types::keymap<langbind::AuthorizationFunctionR> m_authorizationFunctionMap;
	types::keymap<langbind::AuditFunctionR> m_auditFunctionMap;
	types::keymap<types::NormalizeFunctionTypeR> m_normalizeFunctionTypeMap;
	types::keymap<types::CustomDataTypeR> m_customDataTypeMap;
	NormalizeFunctionMap m_normalizeFunctionMap;
	types::keymap<langbind::FormFunctionR> m_formFunctionMap;
	std::vector<langbind::RuntimeEnvironmentR> m_runtimeEnvironmentList;
	types::keymap<langbind::FilterTypeR> m_filterTypeMap;
	types::keymap<types::FormDescriptionR> m_formMap;
	std::vector<types::FormDescriptionR> m_privateFormList;
	std::vector<ProgramR> m_programTypes;
	std::string m_curfile;
	std::vector<const db::Database*> m_databases;
};

}} //namespace
#endif

