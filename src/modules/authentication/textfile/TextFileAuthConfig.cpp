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
// Text file authentication configuration
//

#include "logger-v1.hpp"
#include "config/valueParser.hpp"
#include "config/ConfigurationTree.hpp"

#include "TextFileAuth.hpp"

#include "boost/algorithm/string.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "utils/miscUtils.hpp"

using namespace _Wolframe::utils;

namespace _Wolframe {
namespace AAAA {

bool TextFileAuthConfig::parse( const config::ConfigurationTree& pt, const std::string& /*node*/,
				const module::ModulesDirectory* /*modules*/ )
{
	using namespace config;

	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "identifier" ))	{
			bool isDefined = ( !m_identifier.empty() );
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, m_identifier, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "file" )
			  || boost::algorithm::iequals( L1it->first, "filename" ))	{
			bool isDefined = ( !m_file.empty() );
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, m_file, &isDefined ))
				retVal = false;
		}
		else	{
			MOD_LOG_WARNING << logPrefix() << "unknown configuration option: '"
					<< L1it->first << "'";
		}
	}
	return retVal;
}


bool TextFileAuthConfig::check() const
{
	if ( m_file.empty() )	{
		MOD_LOG_ERROR << logPrefix() << "Authentication filename cannot be empty";
		return false;
	}
	return true;
}

void TextFileAuthConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );
	os << indStr << sectionName() << std::endl;
	os << indStr << "   Identifier: " << m_identifier << std::endl;
	os << indStr << "   File: " << m_file << std::endl;
}

void TextFileAuthConfig::setCanonicalPathes( const std::string& refPath )
{
	using namespace boost::filesystem;

	if ( ! m_file.empty() )	{
		std::string oldPath = m_file;
		m_file = utils::getCanonicalPath( m_file, refPath );
		if ( oldPath != m_file )
/*MBa ?!?*/		LOG_NOTICE << logPrefix() << "Using absolute filename '" << m_file
				   << "' instead of '" << oldPath << "' for the password file";
	}
}

}} // namespace _Wolframe::config
