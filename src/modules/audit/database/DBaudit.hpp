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
// database audit
//

#include "aaaa/auditUnit.hpp"
#include "module/configuredObjectConstructor.hpp"
#include "config/configurationTree.hpp"

#ifndef _DB_AUDIT_HPP_INCLUDED
#define _DB_AUDIT_HPP_INCLUDED

namespace _Wolframe {
namespace aaaa {

static const char* DB_AUDIT_CLASS_NAME = "DatabaseAudit";

class DBauditConfig : public config::NamedConfiguration
{
	friend class DBauditConstructor;
public:
	DBauditConfig( const char* cfgName, const char* logParent, const char* logName )
		: config::NamedConfiguration( cfgName, logParent, logName )
	{ m_required = true; }

	const char* className() const		{ return DB_AUDIT_CLASS_NAME; }

	/// methods
	bool parse( const config::ConfigurationNode& pt, const std::string& node,
		    const module::ModuleDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
private:
	bool			m_required;
	std::string		m_dbConfig;
	config::ConfigurationTree::Position m_config_pos;
};


class DBauditor : public AuditUnit
{
public:
	DBauditor( const std::string& dbLabel );
	~DBauditor();
	const char* className() const		{ return DB_AUDIT_CLASS_NAME; }

	bool resolveDB( const db::DatabaseProviderInterface& db );

	bool required()				{ return m_required; }

	bool audit( const Information& auditObject );
private:
	bool			m_required;
	std::string		m_dbLabel;
	const db::Database*	m_db;
};

class DBauditConstructor : public module::ConfiguredObjectConstructor< AuditUnit >
{
public:
	virtual ObjectConstructorBase::ObjectType objectType() const
						{ return AUDIT_OBJECT; }
	const char* objectClassName() const	{ return DB_AUDIT_CLASS_NAME; }
	DBauditor* object( const config::NamedConfiguration& conf ) const;
};

}} // namespace _Wolframe::aaaa

#endif // _DB_AUDIT_HPP_INCLUDED
