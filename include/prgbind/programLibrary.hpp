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
/// \brief Program library interface
/// \file programLibrary.hpp

#ifndef _PRGBIND_PROGRAM_LIBRARY_INTERFACE_HPP_INCLUDED
#define _PRGBIND_PROGRAM_LIBRARY_INTERFACE_HPP_INCLUDED
#include "filter/filter.hpp"
#include "database/database.hpp"
#include "serialize/cppFormFunction.hpp"
#include "langbind/ddlCompilerInterface.hpp"
#include "langbind/formFunction.hpp"
#include "langbind/runtimeEnvironment.hpp"
#include "langbind/authorizationFunction.hpp"
#include "langbind/auditFunction.hpp"
#include "types/form.hpp"
#include "types/normalizeFunction.hpp"
#include "types/customDataType.hpp"
#include "prgbind/program.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace prgbind {

/// \class ProgramLibrary
/// \brief Interface for the program library with all programs loaded
class ProgramLibrary
{
public:
	/// \brief Destructor
	virtual ~ProgramLibrary(){}

	/// \brief Define an authorization function
	virtual void defineAuthorizationFunction( const std::string& name, const langbind::AuthorizationFunctionR& f)=0;
	/// \brief Define an audit function
	virtual void defineAuditFunction( const std::string& name, const langbind::AuditFunctionR& f)=0;
	/// \brief Define a C++ form function
	virtual void defineCppFormFunction( const std::string& name, const serialize::CppFormFunction& f)=0;
	/// \brief Define an ordinary form function
	virtual void defineFormFunction( const std::string& name, const langbind::FormFunctionR& f)=0;
	/// \brief Define a runtime environment
	virtual void defineRuntimeEnvironment( const langbind::RuntimeEnvironmentR& env)=0;
	/// \brief Define a form structure that is referenced as unexpanded indirection
	virtual void definePrivateForm( const types::FormDescriptionR& f)=0;
	/// \brief Define a form
	virtual void defineForm( const std::string& name, const types::FormDescriptionR& f)=0;
	/// \brief Define a normalization function
	virtual void defineNormalizeFunction( const std::string& name, const types::NormalizeFunctionR& f)=0;
	/// \brief Define a normalization function type
	virtual void defineNormalizeFunctionType( const std::string& name, const types::NormalizeFunctionType& ftype)=0;
	/// \brief Define a custom data type
	virtual void defineCustomDataType( const std::string& name, const types::CustomDataTypeR& t)=0;
	/// \brief Define a data definition language
	virtual void defineFormDDL( const langbind::DDLCompilerR& c)=0;
	/// \brief Define a filter type
	virtual void defineFilterType( const std::string& name, const langbind::FilterTypeR& f)=0;
	/// \brief Define a program type
	virtual void defineProgramType( const ProgramR& prg)=0;

	/// \brief Get the map for DDLs to map types to normalizer call sequences
	virtual const types::NormalizeFunctionMap* formtypemap() const=0;
	/// \brief Get a custom data type defined by name
	virtual const types::CustomDataType* getCustomDataType( const std::string& name) const=0;
	/// \brief Get a normalization function type defined by name
	virtual const types::NormalizeFunctionType* getNormalizeFunctionType( const std::string& name) const=0;

	/// \brief Get a form description
	virtual const types::FormDescription* getFormDescription( const std::string& name) const=0;
	/// \brief Get the list of all forms defined
	virtual std::vector<std::string> getFormNames() const=0;

	/// \brief Get an authorization function by name
	virtual const langbind::AuthorizationFunction* getAuthorizationFunction( const std::string& name) const=0;
	/// \brief Get an audit function by name
	virtual const langbind::AuditFunction* getAuditFunction( const std::string& name) const=0;
	/// \brief Get a form function by name
	virtual const langbind::FormFunction* getFormFunction( const std::string& name) const=0;
	/// \brief Get a normalizer function by name
	virtual const types::NormalizeFunction* getNormalizeFunction( const std::string& name) const=0;
	/// \brief Get a filter type by name
	virtual const langbind::FilterType* getFilterType( const std::string& name) const=0;

	/// \brief Load all programs passed in 'filenames'
	virtual void loadPrograms( db::Database* transactionDB, const std::vector<std::string>& filenames)=0;
};

}} //namespace
#endif

