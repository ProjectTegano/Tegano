/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
// moduleDirectory.cpp
//

#include <boost/algorithm/string.hpp>
#include "logger-v1.hpp"
#include "moduleInterface.hpp"

namespace _Wolframe {
namespace module {

ModulesDirectory::~ModulesDirectory()
{
	while ( ! m_builder.empty() )	{
		assert( m_builder.front() != NULL );
		delete m_builder.front();
		m_builder.pop_front();
	}
}


bool ModulesDirectory::addBuilder( ConfiguredBuilder* container )
{
	for ( std::list< ConfiguredBuilder* >::const_iterator it = m_cfgdBuilder.begin();
							it != m_cfgdBuilder.end(); it++ )	{
		if ( boost::algorithm::iequals( (*it)->m_section, container->m_section ) &&
				boost::algorithm::iequals( (*it)->m_keyword, container->m_keyword ))	{
			LOG_ALERT << "A configuration module for section '" << container->m_section
				  << "' keyword '" << container->m_keyword << "' already exists";
			return false;
		}
		if ( boost::algorithm::iequals( (*it)->m_identifier, container->m_identifier ))	{
			LOG_ALERT << "A module container named '" << container->m_identifier
				  << "' already exists";
			return false;
		}
	}
	m_cfgdBuilder.push_back( container );
	LOG_DEBUG << "Module '" << container->m_identifier << "' registered for section '"
		  << container->m_section << "' keyword '" << container->m_keyword << "'";
	return true;
}

bool ModulesDirectory::addBuilder( SimpleBuilder* container )
{
	for ( std::list< SimpleBuilder* >::const_iterator it = m_builder.begin();
							it != m_builder.end(); it++ )	{
		if ( boost::algorithm::iequals( (*it)->m_identifier, container->m_identifier ))	{
			LOG_ALERT << "A module object named '" << container->m_identifier
				  << "' already exists";
			return false;
		}
	}
	m_builder.push_back( container );
	LOG_DEBUG << "Module object '" << container->m_identifier << "' registered";
	return true;
}

ConfiguredBuilder* ModulesDirectory::getBuilder( const std::string& section,
						 const std::string& keyword ) const
{
	for ( std::list< ConfiguredBuilder* >::const_iterator it = m_cfgdBuilder.begin();
							it != m_cfgdBuilder.end(); it++ )	{
		if ( boost::algorithm::iequals( (*it)->m_keyword, keyword ) &&
				boost::algorithm::iequals( (*it)->m_section, section ))
			return *it;
	}
	return NULL;
}

ConfiguredBuilder* ModulesDirectory::getBuilder( const std::string& identifier ) const
{
	for ( std::list< ConfiguredBuilder* >::const_iterator it = m_cfgdBuilder.begin();
							it != m_cfgdBuilder.end(); it++ )	{
		if ( boost::algorithm::iequals( (*it)->m_identifier, identifier ))
			return *it;
	}
	return NULL;
}

SimpleBuilder* ModulesDirectory::getObject( const std::string& identifier ) const
{
	for ( std::list< SimpleBuilder* >::const_iterator it = m_builder.begin();
							it != m_builder.end(); it++ )	{
		if ( boost::algorithm::iequals( (*it)->m_identifier, identifier ))
			return *it;
	}
	return NULL;
}

}} // namespace _Wolframe::module
