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
///\file testDescription.cpp
///\brief Implements a test description loaded from a file

//... for boost usage in splitString
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4996)
#endif

#include "wtest/testDescription.hpp"
#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#ifdef WITH_PGSQL
#include <libpq-fe.h>
#endif
#include <boost/lexical_cast.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

static void splitString( std::vector<std::string>& res, const std::string& inp, const char* splitchrs)
{
	res.clear();
	std::vector<std::string> imm;
	boost::split( imm, inp, boost::is_any_of(splitchrs));
	std::vector<std::string>::const_iterator vi=imm.begin(), ve=imm.end();
	for (; vi != ve; ++vi) if (!vi->empty()) res.push_back( *vi);
}

using namespace _Wolframe;
using namespace wtest;

#ifdef WITH_PGSQL
static bool check_PGSQL_RUNNING()
{
	enum PGSQLState {Init,ConnSuccess,ConnFail};
	static PGSQLState state = Init;
	if (state == Init)
	{
		static const char *connstr = "host='localhost' user='wolfusr' password='wolfpwd' dbname='wolframe'";
		PGconn* conn = PQconnectdb( connstr);
		if (conn)
		{
			ConnStatusType stat = PQstatus( conn);
			if (stat == CONNECTION_OK)
			{
				state = ConnSuccess;
			}
			else
			{
				state = ConnFail;
			}
		}
		else
		{
			state = ConnFail;
		}
		PQfinish( conn );
	}
	return (state == ConnSuccess);
}
#endif

static const char* check_flag( const std::string& flag)
{
if (boost::iequals( flag, "DISABLED")) return "DISABLED ";
if (boost::starts_with( flag, "DISABLED "))
{
	unsigned int nargs=0;
	std::vector<std::string> platforms;
	splitString( platforms, flag, "\n\t\r ");
	std::vector<std::string>::const_iterator ii = platforms.begin(),ee = platforms.end();
	for (++ii; ii != ee; ++ii)
	{
		if (ii->size() == 0) continue;
		++nargs;
#ifdef _WIN32
		if (boost::iequals( *ii, "WIN32")) return "DISABLED ON PLATFORM WINDOWS ";
#endif
#ifdef LINUX
		if (boost::iequals( *ii, "LINUX")) return "DISABLED ON PLATFORM LINUX ";
#ifdef LINUX_DIST_ARCH
		if (boost::iequals( *ii, "ARCH_LINUX")) return "DISABLED ON LINUX DISTRIBUTION ArchLinux";
#endif
#ifdef LINUX_DIST_DEBIAN
		if (boost::iequals( *ii, "DEBIAN_LINUX")) return "DISABLED ON LINUX DISTRIBUTION Debian";
#endif
#ifdef LINUX_DIST_REDHAT
		if (boost::iequals( *ii, "REDHAT_LINUX")) return "DISABLED ON LINUX DISTRIBUTION Redhat/Fedora";
#endif
#ifdef LINUX_DIST_SLACKWARE
		if (boost::iequals( *ii, "SLACKWARE_LINUX")) return "DISABLED ON LINUX DISTRIBUTION Slackware";
#endif
#ifdef LINUX_DIST_SLES
		if (boost::iequals( *ii, "SLES_LINUX")) return "DISABLED ON LINUX DISTRIBUTION SuSE Enterprise";
#endif
#ifdef LINUX_DIST_SUSE
		if (boost::iequals( *ii, "SUSE_LINUX")) return "DISABLED ON LINUX DISTRIBUTION OpenSuSE";
#endif
#ifdef LINUX_DIST_UBUNTU
		if (boost::iequals( *ii, "UBUNTU_LINUX")) return "DISABLED ON LINUX DISTRIBUTION Ubuntu";
#endif
#endif
	}
	if (nargs == 0) return "DISABLED ";
}
	enum Platform {p_UNKNOWN,p_WIN32,p_LINUX};
	enum LinuxDistro {d_UNKNOWN,d_ARCH,d_DEBIAN,d_REDHAT,d_SLACKWARE,d_SLES,d_SUSE,d_UBUNTU};

	Platform platform = p_UNKNOWN;
#ifdef _WIN32
	platform = p_WIN32;
#endif
#ifdef LINUX
	platform = p_LINUX;
#endif
	if (platform != p_WIN32 && boost::iequals( flag, "WIN32")) return "only on platform WINDOWS";
	if (platform != p_LINUX && boost::iequals( flag, "LINUX")) return "only on platform LINUX";

#if !(WITH_LUA)
	if (boost::iequals( flag, "LUA")) return "WITH_LUA=1 ";
#endif
#if !(WITH_LIBHPDF)
	if (boost::iequals( flag, "LIBHPDF")) return "WITH_LIBHPDF=1 ";
#endif
#if !(WITH_SSL)
	if (boost::iequals( flag, "SSL")) return "WITH_SSL=1 ";
#endif
#if !(WITH_SQLITE3)
	if (boost::iequals( flag, "SQLITE3")) return "WITH_SQLITE3=1 ";
#endif
#if !(WITH_LOCAL_SQLITE3)
	if (boost::iequals( flag, "LOCAL_SQLITE3")) return "WITH_LOCAL_SQLITE3=1 ";
#endif
#if !(WITH_PGSQL)
	if (boost::iequals( flag, "PGSQL")) return "WITH_PGSQL=1 ";
#else
	if (boost::iequals( flag, "PGSQL") && !check_PGSQL_RUNNING()) return "RUNNING Postgresql Database ";
#endif
#if !(WITH_LIBXML2)
	if (boost::iequals( flag, "LIBXML2")) return "WITH_LIBXML2=1 ";
#endif
#if !(WITH_LIBXSLT)
	if (boost::iequals( flag, "LIBXSLT")) return "WITH_LIBXSLT=1 ";
#endif
#if !(WITH_CJSON)
	if (boost::iequals( flag, "CJSON")) return "WITH_CJSON=1 ";
#endif
#if !(WITH_TEXTWOLF)
	if (boost::iequals( flag, "TEXTWOLF")) return "WITH_TEXTWOLF=1 ";
#endif
#if !(WITH_ICU)
	if (boost::iequals( flag, "ICU")) return "WITH_ICU=1 ";
#endif
	return "";
}

static void readFileContent( const std::string& filename, std::string& res)
{
	unsigned char ch;
	FILE* fh = fopen( filename.c_str(), "rb");
	if (!fh)
	{
		throw std::runtime_error( std::string( "failed (errno " + boost::lexical_cast<std::string>(errno) + ") to open file ") + filename + "' for reading");
	}
	boost::shared_ptr<FILE> fhr( fh, fclose);
	while (1 == fread( &ch, 1, 1, fh))
	{
		res.push_back( ch);
	}
	if (!feof( fh))
	{
		int ec = ferror( fh);
		if (ec) throw std::runtime_error( std::string( "failed to read (errno " + boost::lexical_cast<std::string>(ec) + ") from file ") + filename + "'");
	}
}

static void readFile( const std::string& pt, std::vector<std::string>& hdr, std::vector<std::string>& out, std::string& requires, std::vector<std::string>& outputfile)
{
	std::string element;
	std::string content;
	readFileContent( pt, content);
	std::string::const_iterator ci = content.begin(), ce = content.end();

	std::size_t linecnt = 0;
	std::size_t colcnt = 0;
	std::string splitstr;
	std::string::const_iterator splititr;
	enum
	{
		PARSE_HDR,
		PARSE_OUT,
		PARSE_NOT
	}
	type = PARSE_HDR;

	for (; ci != ce; ++ci)
	{
		if (*ci == '\r' || *ci == '\n') break;
		++linecnt;
		splitstr.push_back( *ci);
	}
	if (ci != ce && *ci == '\r') ++ci;
	if (ci != ce && *ci == '\n') ++ci;

	splititr = splitstr.begin();
	if (splititr == splitstr.end())
	{
		throw std::runtime_error( "illegal test definition file. no split tag defined at file start");
	}
	for (; ci != ce && splititr != splitstr.end(); ++ci)
	{
		if (*ci != *splititr) break;
		++splititr;
	};
	if (splititr != splitstr.end())
	{
		throw std::runtime_error( "illegal test definition file. header expected after split tag definition");
	}
	splititr = splitstr.begin();

	try
	{
		for (; ci != ce; ++ci)
		{
			if (*ci == '\n')
			{
				++linecnt;
				colcnt = 0;
			}
			else
			{
				++colcnt;
			}
			if (type == PARSE_HDR)
			{
				if (*ci == '\n')
				{
					std::string tag( std::string( element.c_str(), element.size()));
					boost::trim( tag);
					if (boost::iequals( tag, "end"))
					{
						return;
					}
					else if (boost::starts_with( tag, "outputfile:"))
					{
						std::size_t nn = std::strlen("outputfile:");
						std::string filename( std::string( tag.c_str()+nn, tag.size()-nn));
						boost::trim( filename);
						outputfile.push_back( filename);
						type = PARSE_NOT;
						splititr = splitstr.begin();
					}
					else if (boost::starts_with( tag, "requires:"))
					{
						std::size_t nn = std::strlen("requires:");
						std::string flagname( std::string( tag.c_str()+nn, tag.size()-nn));
						boost::trim( flagname);
						requires.append( check_flag( flagname));
						type = PARSE_NOT;
						splititr = splitstr.begin();
					}
					else
					{
						hdr.push_back( tag);
						element.clear();
						type = PARSE_OUT;
					}
				}
				else
				{
					element.push_back( *ci);
				}
			}
			else if (type == PARSE_OUT)
			{
				element.push_back( *ci);
				if (*ci == *splititr)
				{
					++splititr;
					if (splititr == splitstr.end())
					{
						std::string outelem( element.c_str(), element.size() - splitstr.size());
						out.push_back( outelem);
						element.clear();
						type = PARSE_HDR;
						splititr = splitstr.begin();
					}
				}
				else
				{
					splititr = splitstr.begin();
				}
			}
			else if (type == PARSE_NOT)
			{
				if (*ci == *splititr)
				{
					++splititr;
					if (splititr == splitstr.end())
					{
						element.clear();
						type = PARSE_HDR;
						splititr = splitstr.begin();
					}
				}
				else if ((unsigned char)*ci > 32)
				{
					throw std::runtime_error( "illegal test definition file. tag definition expected");
				}
				else
				{
					splititr = splitstr.begin();
				}
			}
		}
		throw std::runtime_error( "no end tag at end of file");
	}
	catch (const std::runtime_error& e)
	{
		std::ostringstream errmsg;
		std::string filename = boost::filesystem::path(pt).stem().string();

		errmsg << "error in test description file '" << filename << "'"
			<< " at line " << (linecnt+1) << " column " << (colcnt+1)
			<< ": " << e.what();

		throw std::runtime_error( errmsg.str());
	}
}

static void writeFile( const std::string& pt, const std::string& content)
{
	try
	{
		std::fstream ff( pt.c_str(), std::ios::out | std::ios::binary);
		ff.exceptions( std::ifstream::failbit | std::ifstream::badbit);
		ff.write( content.c_str(), content.size());
	}
	catch (std::exception& e)
	{
		std::runtime_error( std::string( "failed to write file '") + pt + "':" + e.what());
	}
}


TestDescription::TestDescription( const std::string& pt, const std::string& testdirstr)
{
	boost::filesystem::path testdir( testdirstr);

	std::vector<std::string> header;
	std::vector<std::string> content;
	readFile( pt, header, content, requires, outputfile);

	std::vector<std::string>::const_iterator hi=header.begin();
	std::vector<std::string>::const_iterator itr=content.begin(),end=content.end();

	for (;itr != end; ++itr,++hi)
	{
		if (boost::iequals( *hi, "input"))
		{
			input.append( *itr);
		}
		else if (boost::iequals( *hi, "output"))
		{
			expected.append( *itr);
		}
		else if (boost::iequals( *hi, "exception"))
		{
			exception.append( *itr);
			std::size_t nn = exception.size()-1;
			if (exception.at(nn) == '\n') --nn;
			if (exception.at(nn) == '\r') --nn;
			exception.resize( nn+1);
		}
		else if (boost::iequals( *hi, "config"))
		{
			config.append( *itr);
			boost::filesystem::path fn( testdir / "temp" / "test.cfg");
			writeFile( fn.string(), config);
		}
		else if (boost::starts_with( *hi, "file:"))
		{
			std::string filename( hi->c_str()+std::strlen("file:"));
			boost::trim( filename);
			boost::filesystem::path fn( testdir / "temp" / filename);
			writeFile( fn.string(), *itr);
		}
	}
}

std::string TestDescription::normalizeOutputCRLF( const std::string& output, const std::string& expected_)
{
	std::string::const_iterator ei = expected_.begin(), ee = expected_.end();
	std::string::const_iterator oi = output.begin(), oe = output.end();
	std::string resultstr;

	for (; ei != ee && oi != oe; ++oi,++ei)
	{
		if (*ei == *oi)
		{
			resultstr.push_back( *ei);
		}
		else if (*ei == '\r' && *oi == '\n')
		{
			++ei;
			if (*ei == '\n')
			{
				resultstr.push_back( '\r');
				resultstr.push_back( '\n');
			}
			else
			{
				break;
			}
		}
		else if (*ei == '\n' && *oi == '\r')
		{
			++oi;
			if (*oi == '\n')
			{
				resultstr.push_back( '\n');
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	if (oi == oe && ei == ee)
	{
		return resultstr;
	}
	else
	{
		return output;
	}
}

std::string TestDescription::normalizeOutputCRLF( const std::string& output)
{
	return normalizeOutputCRLF( output, expected);
}

#ifdef _WIN32
#pragma warning(pop)
#endif
