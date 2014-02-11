/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
///\brief Program library interface
///\file programLibrary.hpp

#ifndef _PRGBIND_PROGRAM_LIBRARY_HPP_INCLUDED
#define _PRGBIND_PROGRAM_LIBRARY_HPP_INCLUDED
#include "filter/filter.hpp"
#include "database/database.hpp"
#include "langbind/cppFormFunction.hpp"
#include "langbind/ddlCompilerInterface.hpp"
#include "langbind/formFunction.hpp"
#include "langbind/runtimeEnvironment.hpp"
#include "types/form.hpp"
#include "types/normalizeFunction.hpp"
#include "types/customDataType.hpp"
#include "prgbind/program.hpp"
#include <string>
#include <list>


namespace _Wolframe {
namespace prgbind {

//\class ProgramLibrary
//\brief Class representing the program library with all programs loaded
class ProgramLibrary
{
public:
	ProgramLibrary();
	ProgramLibrary( const ProgramLibrary& o);

	virtual ~ProgramLibrary();
	virtual void defineCppFormFunction( const std::string& name, const langbind::CppFormFunction& f);
	virtual void defineFormFunction( const std::string& name, const langbind::FormFunctionR f);
	virtual void defineRuntimeEnvironment( const langbind::RuntimeEnvironmentR& env);
	virtual void definePrivateForm( const types::FormDescriptionR& f);
	virtual void defineForm( const std::string& name, const types::FormDescriptionR& f);
	virtual void defineDDLTypeNormalizer( const std::string& name, const types::NormalizeFunctionR& f) const;
	virtual void defineNormalizeFunctionType( const std::string& name, const types::NormalizeFunctionType& ftype);
	virtual void defineCustomDataType( const std::string& name, const types::CustomDataTypeR& t);
	virtual void defineFormDDL( const langbind::DDLCompilerR& c);
	virtual void defineFilterType( const std::string& name, const langbind::FilterTypeR& f);
	virtual void defineProgramType( const ProgramR& prg);

	virtual const types::NormalizeFunctionMap* formtypemap() const;
	virtual const types::CustomDataType* getCustomDataType( const std::string& name) const;
	virtual const types::NormalizeFunctionType* getNormalizeFunctionType( const std::string& name) const;

	virtual const types::FormDescription* getFormDescription( const std::string& name) const;
	virtual std::vector<std::string> getFormNames() const;

	virtual const langbind::FormFunction* getFormFunction( const std::string& name) const;
	virtual const types::NormalizeFunction* getDDLTypeNormalizer( const std::string& name) const;
	virtual const langbind::FilterType* getFilterType( const std::string& name) const;

	virtual void loadPrograms( db::Database* transactionDB, const std::list<std::string>& filenames);

private:
	class Impl;
	Impl* m_impl;
};

}} //namespace
#endif

