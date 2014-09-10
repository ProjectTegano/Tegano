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
//
/// \file config/bannerConfiguration.cpp
//

#include "bannerConfiguration.hpp"
#include "config/valueParser.hpp"
#include "logger/logger-v1.hpp"
#include "applicationInfo.hpp"
#include "system/platform.hpp"

#include <string>
#include <stdexcept>
#include <boost/algorithm/string.hpp>

namespace _Wolframe {
namespace config {


static BannerConfiguration::SignatureTokens strToToken( std::string& str )
{
	if ( boost::algorithm::iequals( str, "ProductOnly" ))
		return BannerConfiguration::PRODUCT_NAME;
	else if ( boost::algorithm::iequals( str, "Major" ))
		return BannerConfiguration::VERSION_MAJOR;
	else if ( boost::algorithm::iequals( str, "Minor" ))
		return BannerConfiguration::VERSION_MINOR;
	else if ( boost::algorithm::iequals( str, "Revision" ))
		return BannerConfiguration::VERSION_REVISION;
	else if ( boost::algorithm::iequals( str, "Build" ))
		return BannerConfiguration::VERSION_BUILD;
	else if ( boost::algorithm::iequals( str, "OS" ))
		return BannerConfiguration::PRODUCT_OS;
	else if ( boost::algorithm::iequals( str, "None" ))
		return BannerConfiguration::NONE;
	return BannerConfiguration::UNDEFINED;
}


/// Service signature parser
bool BannerConfiguration::parse( const config::ConfigurationNode& pt, const std::string& node,
			   const module::ModuleDirectory* /*modules*/ )
{
	if ( boost::algorithm::iequals( node, "ServerTokens" ))	{
		bool tokensDefined = ( m_tokens != BannerConfiguration::UNDEFINED );
		std::string	val;
		if ( !Parser::getValue( logPrefix().c_str(), node.c_str(), pt.data(),
					val, &tokensDefined ))
			return false;
		m_tokens = strToToken( val );
		if ( m_tokens == BannerConfiguration::UNDEFINED )	{
			LOG_ERROR << logPrefix() << "Unknown option '" << val << "' for " << node;
			return false;
		}
	}
	else if ( boost::algorithm::iequals( node, "ServerSignature" ))	{
		bool isDefined = ( !m_serverSignature.empty());
		if ( !Parser::getValue( logPrefix().c_str(), node.c_str(), pt.data(),
					m_serverSignature, &isDefined ))
			return false;
	}
	else	{
		LOG_FATAL << logPrefix() << "called with unknown configuration option: '"
			  << node << "'";
		return false;
	}

	return true;
}


bool BannerConfiguration::check() const
{
	switch ( m_tokens )	{
	case PRODUCT_NAME:
	case VERSION_MAJOR:
	case VERSION_MINOR:
	case VERSION_REVISION:
	case VERSION_BUILD:
	case PRODUCT_OS:
	case NONE:
	case UNDEFINED:
		return true;
	default:
		LOG_ERROR << "Unknown value for BannerConfiguration::tokens: " << (int)m_tokens;
		return false;
	}
	// for stupid compilers
}


void BannerConfiguration::print( std::ostream& os, size_t /* indent */ ) const
{
	os << configSection() << std::endl;
	os << "   Service banner: ";
	switch ( m_tokens )	{
	case PRODUCT_NAME:	os << "product name only"; break;
	case VERSION_MAJOR:	os << "product name and major version"; break;
	case VERSION_MINOR:	os << "product name and minor version"; break;
	case VERSION_REVISION:	os << "product name and revision"; break;
	case VERSION_BUILD:	os << "product name and build"; break;
	case PRODUCT_OS:	os << "product name, revision and OS"; break;
	case NONE:		os << "none"; break;
	case UNDEFINED:
	default:		os << "NOT DEFINED !"; break;
	}
	os << std::endl;
	os << "   Print service name: " << ( m_serverSignature.empty() ? m_serverSignature : "(none)" ) << std::endl;
}


std::string BannerConfiguration::toString() const
{
	std::string	banner;

	if ( m_tokens == UNDEFINED )	{

	}

	switch ( m_tokens )	{
		case PRODUCT_NAME:
			banner = "Wolframe";
			break;
		case VERSION_MAJOR:
			banner = "Wolframe " + _Wolframe::ApplicationInfo::instance().version().toString( "version %M" );
			break;
		case VERSION_MINOR:
			banner = "Wolframe " + _Wolframe::ApplicationInfo::instance().version().toString( "version %M.%m" );
			break;
		case VERSION_REVISION:
			banner = "Wolframe " + _Wolframe::ApplicationInfo::instance().version().toString( "version %M.%m.%r" );
			break;
		case VERSION_BUILD:
			banner = "Wolframe " + _Wolframe::ApplicationInfo::instance().version().toString( "version %M.%m.%r.%b" );
			break;
		case PRODUCT_OS: {
			system::Platform p = system::Platform::makePlatform( );
			banner = "Wolframe " + _Wolframe::ApplicationInfo::instance().version().toString( "version %M.%m.%r.%b" ) + " " + p.toString( );
			}
			break;
		case NONE:
			break;
		case UNDEFINED:
			LOG_INFO << "Undefined ServerTokens, using ProductOnly";
			banner = "Wolframe";
			break;
		default:
			throw std::domain_error( "ServiceBanner: unknown ServerTokens value" );
	}
	if ( ! m_serverSignature.empty() )
		banner = m_serverSignature + " (" + banner + ")";

	return banner;
}

}} // namespace _Wolframe::Configuration
