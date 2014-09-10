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
/// \brief Base class for object properties used for identifying an object
/// \file module/objectDescription.hpp

#ifndef _CONFIGURATION_OBJECT_DESCRIPTION_HPP_INCLUDED
#define _CONFIGURATION_OBJECT_DESCRIPTION_HPP_INCLUDED
#include <string>

namespace _Wolframe {
namespace config {

/// \class ConfigurationObjectDescription
/// \brief Description of a configuration object
class ConfigurationObjectDescription
{
public:
	ConfigurationObjectDescription( const std::string& className_, const std::string& configSection_, const std::string& configKeyword_)
		:m_className(className_)
		,m_configKeyword(configKeyword_)
		,m_configSection(configSection_)
		,m_logPrefix(configSection_ + "/" + configKeyword_)
	{}

	ConfigurationObjectDescription( const ConfigurationObjectDescription& o)
		:m_className(o.m_className)
		,m_configKeyword(o.m_configKeyword)
		,m_configSection(o.m_configSection)
		,m_logPrefix(o.m_logPrefix)
	{}

	~ConfigurationObjectDescription(){}

	const std::string& className() const
	{
		return m_className;
	}

	const std::string& logPrefix() const
	{
		return m_logPrefix;
	}

	const std::string& configSection() const
	{
		return m_configSection;
	}

	const std::string& configKeyword() const
	{
		return m_configKeyword;
	}

private:
	std::string m_className;
	std::string m_configKeyword;
	std::string m_configSection;
	std::string m_logPrefix;
};

}}//namespace
#endif


