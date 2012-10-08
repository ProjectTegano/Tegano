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
///\file modules/database/testtrace/testtraceDatabase.cpp
///\brief Fake database implementation for testing database processing engines
#include "logger-v1.hpp"
#include "testtraceDatabase.hpp"
#include "testtraceTransaction.hpp"
#include "serialize/struct/filtermapDescription.hpp"
#include "config/structSerialize.hpp"
#include "utils/miscUtils.hpp"
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

const serialize::StructDescriptionBase* TesttraceDatabaseConfig::Data::getStructDescription()
{
	typedef TesttraceDatabaseConfig::Data This;
	struct ThisDescription :public serialize::StructDescription<This>
	{
		ThisDescription()
		{
			(*this)
			( "id",		&This::id)
			( "program",	&This::programfilename)
			( "file",	&This::resultfilename)
			( "outfile",	&This::outfilename)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

bool TesttraceDatabaseConfig::parse( const config::ConfigurationTree& pt, const std::string&, const module::ModulesDirectory*)
{
	try
	{
		config::parseConfigStructure( m_data, (const boost::property_tree::ptree&)pt);
		return true;
	}
	catch (std::exception& e)
	{
		LOG_ERROR << e.what();
		return false;
	}
}

bool TesttraceDatabaseConfig::check() const
{
	if (!m_data.programfilename.empty() && !utils::fileExists( m_data.programfilename))
	{
		LOG_ERROR << "Configured program file '" << m_data.programfilename << "' does not exist";
		return false;
	}
	if (!m_data.resultfilename.empty() && !utils::fileExists( m_data.resultfilename))
	{
		LOG_ERROR << "Configured result file '" << m_data.resultfilename << "' does not exist";
		return false;
	}
	if (m_data.outfilename.empty())
	{
		LOG_ERROR << "Output file of test trace DB is not configured";
		return false;
	}
	return true;
}

void TesttraceDatabaseConfig::print( std::ostream& os, size_t indent) const
{
	std::string indentstr( indent+1, '\t');
	indentstr[0] = '\n';
	std::string rt( config::structureToString( m_data));
	boost::replace_all( rt, "\n", indentstr);
	os << rt;
}

void TesttraceDatabaseConfig::setCanonicalPathes( const std::string& referencePath)
{
	if (!m_data.programfilename.empty())
	{
		m_data.programfilename = utils::getCanonicalPath( m_data.programfilename, referencePath);
	}
	if (!m_data.resultfilename.empty())
	{
		m_data.resultfilename = utils::getCanonicalPath( m_data.resultfilename, referencePath);
	}
	if (!m_data.outfilename.empty())
	{
		m_data.outfilename = utils::getCanonicalPath( m_data.outfilename, referencePath);
	}
}


TesttraceDatabase::TesttraceDatabase( const std::string& id_, const std::string& programfilename_, const std::string& resultfilename_, const std::string& outfilename_, unsigned short, bool)
	:m_id(id_)
	,m_outfilename(outfilename_)
{
	if (!resultfilename_.empty())
	{
		m_result = utils::readSourceFileLines( resultfilename_);
	}
	if (!programfilename_.empty())
	{
		m_programsrc = utils::readSourceFileContent( programfilename_);
	}
}

Transaction* TesttraceDatabase::transaction( const std::string& /*name*/ )
{
	return new TesttraceTransaction( this, m_result);
}

bool TesttraceDatabase::loadProgram()
{
	try
	{
		m_program.load( m_programsrc);
		return true;
	}
	catch (std::runtime_error& e)
	{
		LOG_ERROR << "failed to load database program: " << e.what();
		return false;
	}
}

