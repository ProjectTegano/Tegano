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
// file audit
//

#ifndef _FILE_AUDIT_HPP_INCLUDED
#define _FILE_AUDIT_HPP_INCLUDED

#include <string>

#include "config/configurationTree.hpp"
#include "config/configurationObject.hpp"
#include "aaaa/auditUnit.hpp"
#include "module/configuredObjectConstructor.hpp"

namespace _Wolframe {
namespace aaaa {

class TextFileAuditConfig : public config::ConfigurationObject
{
public:
	TextFileAuditConfig( const std::string& className_, const std::string& configSection_, const std::string& configKeyword_)
		:config::ConfigurationObject( className_, configSection_, configKeyword_)
		,m_required(true)
	{}

	bool parse( const config::ConfigurationNode& pt, const std::string& node,
		    const module::ModuleDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );

	bool required() const			{return m_required;}
	const std::string& file() const		{return m_file;}

private:
	bool		m_required;
	std::string	m_file;
	config::ConfigurationTree::Position m_config_pos;
};


class TextFileAuditor : public AuditUnit
{
public:
	TextFileAuditor( const TextFileAuditConfig* config);
	~TextFileAuditor();

	bool required()				{ return m_required; }
	bool audit( const Information& auditObject );
private:
	bool		m_required;
	std::string	m_file;
};

}}
#endif
