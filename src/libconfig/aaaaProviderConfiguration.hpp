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
/// \brief AAAA provider configuration

#ifndef _CONFIG_AAAA_PROVIDER_CONFIGURATION_HPP_INCLUDED
#define _CONFIG_AAAA_PROVIDER_CONFIGURATION_HPP_INCLUDED

#include "config/configurationBase.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace config {

/// \brief Configuration for AAAA (Authentication, Authorization, Audit, Accounting)
class AaaaProviderConfiguration
	:public config::ConfigurationBase
{
public:
	/// x-structor
	AaaaProviderConfiguration();
	~AaaaProviderConfiguration();

	/// methods
	bool parse( const config::ConfigurationNode& pt, const std::string& node,
		    const module::ModuleDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );

	const std::string& randomDevice() const					{return m_randomDevice;}
	const std::vector<config::NamedConfiguration*>& authConfig() const	{return m_authConfig;}
	bool authzDefault() const						{return m_authzDefault;}
	const std::vector<config::NamedConfiguration*>& authzConfig() const	{return m_authzConfig;}
	bool mandatoryAudit() const						{return m_mandatoryAudit;}
	const std::vector<config::NamedConfiguration*>&	auditConfig() const	{return m_auditConfig;}
	
private:
	std::string					m_randomDevice;
	std::vector< config::NamedConfiguration* >	m_authConfig;
	bool						m_authzDefault;
	std::vector< config::NamedConfiguration* >	m_authzConfig;
	bool						m_mandatoryAudit;
	std::vector< config::NamedConfiguration* >	m_auditConfig;
};

}}//namespace
#endif

