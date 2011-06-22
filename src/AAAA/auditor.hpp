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
// auditing objects
//

#ifndef _AUDITOR_HPP_INCLUDED
#define _AUDITOR_HPP_INCLUDED

#include "config/configurationBase.hpp"
#include "database/database.hpp"

#include <list>

namespace _Wolframe {
namespace AAAA {

enum AuditType	{
	AUDIT_FILE,		///< Standard text file auditing (list of entries)
	AUDIT_DATABASE		///< Database audit entries
};


class AuditConfigurationBase : public config::ConfigurationBase
{
public:
	/// constructor
	AuditConfigurationBase( const char* name, const char* logParent, const char* logName )
		: config::ConfigurationBase( name, logParent, logName )		{}

	virtual ~AuditConfigurationBase()	{}

	virtual AuditType type() const = 0;
};


class AuditConfiguration : public config::ConfigurationBase
{
	friend class AAAAprovider;
	friend class config::ConfigurationParser;
public:
	/// constructor
	AuditConfiguration( const char* cfgName, const char* logParent, const char* logName )
		: ConfigurationBase( cfgName, logParent, logName )	{}
	~AuditConfiguration();

	/// methods
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );

	// bool test() const;	// Not implemented yet, inherited from base
private:
	std::list<AuditConfigurationBase*>	m_config;
};


class AuditorBase
{
public:
	virtual ~AuditorBase()					{}
	virtual bool resolveDB( const db::DatabaseProvider& /*db*/ )	{ return true; }
};

}} // namespace _Wolframe::AAAA

#endif // _AUDITOR_HPP_INCLUDED
