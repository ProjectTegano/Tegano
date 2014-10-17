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
/// \brief Processor Provider configuration
/// \file processor/procProviderConfig.hpp

#ifndef _PROCESSOR_PROVIDER_CONFIG_HPP_INCLUDED
#define _PROCESSOR_PROVIDER_CONFIG_HPP_INCLUDED
#include "config/configurationObject.hpp"
#include "moduleDirectoryImpl.hpp"
#include "types/keymap.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace config {

/// \brief Processor provider configuration
class ProcProviderConfiguration
	:public config::ConfigurationObject
{
public:
	/// constructor & destructor
	ProcProviderConfiguration()
		: ConfigurationObject( "ProcProvider", "Processor", "") {}
	~ProcProviderConfiguration();

	/// methods
	bool parse( const config::ConfigurationNode& pt, const std::string& node,
		    const module::ModuleDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );

	const std::vector<std::string>& databaseIds() const			{return m_databaseIds;}
	const std::vector<config::ConfigurationObject*>& procConfig() const	{return m_procConfig;}
	const std::vector<std::string> programFiles() const			{return m_programFiles;}
	const std::string& referencePath() const				{return m_referencePath;}

private:
	std::vector<std::string>			m_databaseIds;
	std::vector<config::ConfigurationObject*>	m_procConfig;
	std::vector<std::string>			m_programFiles;
	std::string					m_referencePath;
};

}}//namespace
#endif

