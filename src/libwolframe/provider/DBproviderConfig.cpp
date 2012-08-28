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
// database provider configuration functions
//

#include "DBproviderImpl.hpp"

#include "config/valueParser.hpp"
#include "config/ConfigurationTree.hpp"
#include "moduleInterface.hpp"
#include "logger-v1.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "utils/miscUtils.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>
#include <string>
#include <cassert>
#include <list>

namespace _Wolframe {
namespace db {

bool DBproviderConfig::parse( const config::ConfigurationTree& pt,
			      const std::string& /*node*/,
			      const module::ModulesDirectory* modules )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( modules )	{
			module::ConfiguredBuilder* builder = modules->getBuilder( "database", L1it->first );
			if ( builder )	{
				config::NamedConfiguration* conf = builder->configuration( logPrefix().c_str());
				if ( conf->parse( L1it->second, L1it->first, modules ))
					m_config.push_back( conf );
				else	{
					delete conf;
					retVal = false;
				}
			}
			else
				LOG_WARNING << logPrefix() << "unknown configuration option: '"
					    << L1it->first << "'";
		}
		else
			LOG_WARNING << logPrefix() << "unknown configuration option: '"
				    << L1it->first << "'";
	}
	return retVal;
}

DBproviderConfig::~DBproviderConfig()
{
	for ( std::list< config::NamedConfiguration* >::const_iterator it = m_config.begin();
								it != m_config.end(); it++ )
		delete *it;
}

void DBproviderConfig::print( std::ostream& os, size_t /* indent */ ) const
{
	os << sectionName() << std::endl;
	if ( m_config.size() > 0 )	{
		for ( std::list< config::NamedConfiguration* >::const_iterator it = m_config.begin();
								it != m_config.end(); it++ )	{
			(*it)->print( os, 3 );
		}
	}
	else
		os << "   None configured" << std::endl;
}


/// Check if the database configuration makes sense
bool DBproviderConfig::check() const
{
	bool correct = true;
	for ( std::list< config::NamedConfiguration* >::const_iterator it = m_config.begin();
								it != m_config.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}
	return correct;
}

void DBproviderConfig::setCanonicalPathes( const std::string& refPath )
{
	for ( std::list< config::NamedConfiguration* >::const_iterator it = m_config.begin();
								it != m_config.end(); it++ )	{
		(*it)->setCanonicalPathes( refPath );
	}
}

}} // namespace _Wolframe::db
