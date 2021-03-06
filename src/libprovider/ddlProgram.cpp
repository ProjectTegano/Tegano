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
/// \brief Implementation of programs for definition of forms in a DDL
/// \file ddlProgram.cpp

#include "ddlProgram.hpp"
#include "processor/programLibrary.hpp"
#include "utils/fileUtils.hpp"
#include "logger/logger-v1.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::proc;


bool DDLProgram::is_mine( const std::string& filename) const
{
	std::string ext = utils::getFileExtension( filename);
	if (ext.empty()) return false;
	if (boost::iequals( m_constructor->ddlname(), ext.c_str()+1))
	{
		LOG_WARNING << "Using deprecated file extension for program '" << ext << "' instead of '." << m_constructor->fileext() << "'";
		return true;
	}
	if (boost::iequals( m_constructor->fileext(), ext.c_str()+1)) return true;
	return false;
}

void DDLProgram::loadProgram( ProgramLibrary& library, const std::string& filename)
{
	const types::NormalizeFunctionMap* typemap = library.formtypemap();
	std::vector<types::FormDescriptionR> forms = m_constructor->compile( filename, typemap);
	std::vector<types::FormDescriptionR>::const_iterator fi = forms.begin(), fe = forms.end();
	for (; fi != fe; ++fi)
	{
		if ((*fi)->name().empty())
		{
			library.definePrivateForm( *fi);
		}
		else
		{
			library.defineForm( (*fi)->name(), *fi);
		}
	}
}


