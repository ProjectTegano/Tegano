/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
///\file langbind_appConfig_env.cpp
///\brief Part of langbind_appConfig.cpp
#include "langbind/appConfig_struct.hpp"
#include "config/description.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;

const config::DescriptionBase* EnvironmentConfigStruct::description()
{
	struct ThisDescription :public config::Description<EnvironmentConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "DDL",		&EnvironmentConfigStruct::DDL)
			( "form",		&EnvironmentConfigStruct::form)
			( "filter",		&EnvironmentConfigStruct::filter)
			( "transaction",	&EnvironmentConfigStruct::transaction)
			( "plugin",		&EnvironmentConfigStruct::plugin)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

