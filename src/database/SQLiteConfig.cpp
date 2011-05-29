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
//
// SQLite configuration parser
//

#include "database/SQLite.hpp"
#include "config/valueParser.hpp"
#include "config/configurationParser.hpp"

#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

namespace _Wolframe {
namespace config {

/// Specialization of the ConfigurationParser::parse for the SQLite configuration
template<>
bool ConfigurationParser::parse( db::SQLiteConfig& cfg,
				 const boost::property_tree::ptree& pt, const std::string& /*node*/ )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "identifier" ))	{
			bool isDefined = ( !cfg.ID().empty() );
			std::string id;
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, id, &isDefined ))
				retVal = false;
			else
				cfg.ID( id );
		}
		else if ( boost::algorithm::iequals( L1it->first, "file" ) ||
			  boost::algorithm::iequals( L1it->first, "filename" ))	{
			bool isDefined = ( !cfg.filename.empty() );
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.filename, &isDefined ))
				retVal = false;
			else	{
				if ( ! boost::filesystem::path( cfg.filename ).is_absolute() )
					LOG_WARNING << cfg.logPrefix() << "database file path is not absolute: "
						    << cfg.filename;
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "flag" ))	{
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.flag, Parser::BoolDomain() ))
				retVal = false;
		}
		else	{
			LOG_WARNING << cfg.logPrefix() << "unknown configuration option: '"
				    << L1it->first << "'";
		}
	}
	return retVal;
}

}} // namespace _Wolframe::config
