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
//\brief Implements the list of all initially defined program types
//\file programLibrary.cpp

#include "types/customDataNormalizer.hpp"
#include "prgbind/programLibrary.hpp"
#include "prgbind/program.hpp"
#include "programLibraryImpl.hpp"
#include "ddlProgram.hpp"
#include "normalizeProgram.hpp"
#include "filter/null_filter.hpp"
#include "filter/filter.hpp"
#include "types/normalizeFunction.hpp"
#include "langbind/formFunction.hpp"
#include "logger-v1.hpp"
#include "utils/fileUtils.hpp"
#include <algorithm>

using namespace _Wolframe;
using namespace _Wolframe::prgbind;

ProgramLibraryImpl::~ProgramLibraryImpl(){}
ProgramLibraryImpl::ProgramLibraryImpl()
{
	m_programTypes.push_back( ProgramR( new NormalizeProgram()));
	m_filterTypeMap.insert( "null", langbind::FilterTypeR( new langbind::NullFilterType()));
}

ProgramLibraryImpl::ProgramLibraryImpl( const ProgramLibraryImpl& o)
	:m_authorizationFunctionMap(o.m_authorizationFunctionMap)
	,m_auditFunctionMap(o.m_auditFunctionMap)
	,m_normalizeFunctionTypeMap(o.m_normalizeFunctionTypeMap)
	,m_customDataTypeMap(o.m_customDataTypeMap)
	,m_normalizeFunctionMap(o.m_normalizeFunctionMap)
	,m_formFunctionMap(o.m_formFunctionMap)
	,m_runtimeEnvironmentList(o.m_runtimeEnvironmentList)
	,m_filterTypeMap(o.m_filterTypeMap)
	,m_formMap(o.m_formMap)
	,m_privateFormList(o.m_privateFormList)
	,m_programTypes(o.m_programTypes)
	,m_curfile(o.m_curfile)
	{}

void ProgramLibraryImpl::defineAuthorizationFunction( const std::string& name, const langbind::AuthorizationFunctionR& f)
{
	m_authorizationFunctionMap.insert( name, f);
}

void ProgramLibraryImpl::defineAuditFunction( const std::string& name, const langbind::AuditFunctionR& f)
{
	m_auditFunctionMap.insert( name, f);
}

void ProgramLibraryImpl::defineCppFormFunction( const std::string& name, const serialize::CppFormFunction& f)
{
	m_formFunctionMap.insert( name, langbind::FormFunctionR( new serialize::CppFormFunction( f)));
}

void ProgramLibraryImpl::defineFormFunction( const std::string& name, const langbind::FormFunctionR& f)
{
	m_formFunctionMap.insert( name, f);
}

void ProgramLibraryImpl::defineRuntimeEnvironment( const langbind::RuntimeEnvironmentR& env)
{
	m_runtimeEnvironmentList.push_back( env);
}

void ProgramLibraryImpl::defineNormalizeFunction( const std::string& name, const types::NormalizeFunctionR& f)
{
	m_normalizeFunctionMap.define( name, f);
}

void ProgramLibraryImpl::definePrivateForm( const types::FormDescriptionR& f)
{
	m_privateFormList.push_back( f);
}

void ProgramLibraryImpl::defineForm( const std::string& name, const types::FormDescriptionR& f)
{
	m_formMap.insert( name, f);
}

void ProgramLibraryImpl::defineFormDDL( const langbind::DDLCompilerR& c)
{
	DDLProgram* prg = new DDLProgram( c);
	m_programTypes.push_back( ProgramR( prg));
}

void ProgramLibraryImpl::defineNormalizeFunctionType( const std::string& name, const types::NormalizeFunctionType& f)
{
	m_normalizeFunctionTypeMap.insert( name, f);
}

void ProgramLibraryImpl::defineCustomDataType( const std::string& name, const types::CustomDataTypeR& t)
{
	m_customDataTypeMap.insert( name, t);
}

void ProgramLibraryImpl::defineProgramType( const ProgramR& prg)
{
	m_programTypes.push_back( prg);
}

void ProgramLibraryImpl::defineFilterType( const std::string& name, const langbind::FilterTypeR& f)
{
	m_filterTypeMap.insert( name, f);
}

const types::FormDescription* ProgramLibraryImpl::getFormDescription( const std::string& name) const
{
	types::keymap<types::FormDescriptionR>::const_iterator fi = m_formMap.find( name);
	if (fi == m_formMap.end()) return 0;
	return fi->second.get();
}

std::vector<std::string> ProgramLibraryImpl::getFormNames() const
{
	return m_formMap.getkeys< std::vector<std::string> >();
}

const langbind::AuthorizationFunction* ProgramLibraryImpl::getAuthorizationFunction( const std::string& name) const
{
	types::keymap<langbind::AuthorizationFunctionR>::const_iterator fi = m_authorizationFunctionMap.find( name);
	if (fi == m_authorizationFunctionMap.end()) return 0;
	return fi->second.get();
}

const langbind::AuditFunction* ProgramLibraryImpl::getAuditFunction( const std::string& name) const
{
	types::keymap<langbind::AuditFunctionR>::const_iterator fi = m_auditFunctionMap.find( name);
	if (fi == m_auditFunctionMap.end()) return 0;
	return fi->second.get();
}

const langbind::FormFunction* ProgramLibraryImpl::getFormFunction( const std::string& name) const
{
	types::keymap<langbind::FormFunctionR>::const_iterator fi = m_formFunctionMap.find( name);
	if (fi == m_formFunctionMap.end()) return 0;
	return fi->second.get();
}

const types::CustomDataType* ProgramLibraryImpl::getCustomDataType( const std::string& name) const
{
	types::keymap<types::CustomDataTypeR>::const_iterator ti = m_customDataTypeMap.find( name);
	if (ti == m_customDataTypeMap.end()) return 0;
	return ti->second.get();
}

const types::NormalizeFunction* ProgramLibraryImpl::getNormalizeFunction( const std::string& name) const
{
	return m_normalizeFunctionMap.get( name);
}

const types::NormalizeFunctionType* ProgramLibraryImpl::getNormalizeFunctionType( const std::string& name) const
{
	types::keymap<types::NormalizeFunctionType>::const_iterator fi = m_normalizeFunctionTypeMap.find( name);
	if  (fi == m_normalizeFunctionTypeMap.end()) return 0;
	return &fi->second;
}

const types::NormalizeFunctionMap* ProgramLibraryImpl::formtypemap() const
{
	return &m_normalizeFunctionMap;
}

const langbind::FilterType* ProgramLibraryImpl::getFilterType( const std::string& name) const
{
	types::keymap<langbind::FilterTypeR>::const_iterator fi = m_filterTypeMap.find( name);
	return (fi == m_filterTypeMap.end())?0:fi->second.get();
}

static bool programOrderAsc( std::pair<Program*, std::string> const& a, std::pair<Program*, std::string> const& b)
{
	return ((int)a.first->category()) < ((int)b.first->category());
}

void ProgramLibraryImpl::loadPrograms( db::Database* transactionDB, const std::vector<std::string>& filenames)
{
	LOG_DEBUG << "Loading programs";

	// Loading programs enclosed in a runtime environment:
	std::vector<langbind::RuntimeEnvironmentR>::const_iterator ri = m_runtimeEnvironmentList.begin(), re = m_runtimeEnvironmentList.end();
	for (; ri != re; ++ri)
	{
		std::vector<std::string> functions = (*ri)->functions();
		std::vector<std::string>::const_iterator fi = functions.begin(), fe = functions.end();
		for (; fi != fe; ++fi)
		{
			LOG_TRACE << "Function '" << *fi << "' registered in '" << (*ri)->name() << "' environment";
			m_formFunctionMap.insert( *fi, langbind::FormFunctionR( new langbind::RuntimeEnvironmentFormFunction( *fi, ri->get())));
		}
	}

	// Locate all programs to load:
	std::vector< std::pair<Program*, std::string> > typed_filenames;

	std::vector<std::string>::const_iterator fi = filenames.begin(), fe = filenames.end();
	for (; fi != fe; ++fi)
	{
		std::vector<ProgramR>::const_iterator pi = m_programTypes.begin(), pe = m_programTypes.end();
		for (; pi != pe; ++pi)
		{
			if ((*pi)->is_mine( *fi))
			{
				typed_filenames.push_back( std::pair<Program*, std::string>(pi->get(), *fi));
				break;
			}
		}
		if (pi == pe)
		{
			throw std::runtime_error( std::string("unknown type of program '") + *fi + "'");
		}
	}
	// Load programs in the order of their dependency level:
	std::sort( typed_filenames.begin(), typed_filenames.end(), programOrderAsc);

	std::vector< std::pair<Program*, std::string> >::const_iterator ti = typed_filenames.begin(), te = typed_filenames.end();
	for (; ti != te; ++ti)
	{
		LOG_DEBUG << "Loading program '" << ti->second << "'";
		ti->first->loadProgram( *this, transactionDB, m_curfile=ti->second);
	}

	// Initialize context of form functions:
	types::keymap<langbind::FormFunctionR>::const_iterator gi = m_formFunctionMap.begin(), ge = m_formFunctionMap.end();
	for (; gi != ge; ++gi)
	{
		if (!gi->second->initializeContext())
		{
			LOG_ERROR << "Initializing context of form function '" << gi->first << "' failed";
		}
	}
}

