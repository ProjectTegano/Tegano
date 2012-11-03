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
///\file testDescription.cpp
///\brief Implements a test description loaded from a file
#include "testDescription.hpp"
#include "utils/miscUtils.hpp"
#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace wtest;

static const char* check_flag( const std::string& flag)
{
if (boost::iequals( flag, "DISABLED")) return "DISABLED ";
if (boost::starts_with( flag, "DISABLED "))
{
	unsigned int nargs=0;
	std::vector<std::string> platforms;
	utils::splitString( platforms, flag, "\n\t\r ");
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
#endif
#ifdef SUNOS
		if (boost::iequals( *ii, "SUNOS")) return "DISABLED ON PLATFORM SUNOS ";
#endif
#ifdef FREEBSD
		if (boost::iequals( *ii, "FREEBSD")) return "DISABLED ON PLATFORM FREEBSD ";
#endif
	}
	if (nargs == 0) return "DISABLED ";
}
	enum Platform {p_UNKNOWN,p_WIN32,p_LINUX,p_SUNOS,p_FREEBSD};
	Platform platform = p_UNKNOWN;
#ifdef _WIN32
	platform = p_WIN32;
#endif
#ifdef LINUX
	platform = p_LINUX;
#endif
#ifdef SUNOS
	platform = p_SUNOS;
#endif
#ifdef FREEBSD
	platform = p_FREEBSD;
#endif
	if (platform != p_WIN32 && boost::iequals( flag, "WIN32")) return "only on platform WINDOWS";
	if (platform != p_LINUX && boost::iequals( flag, "LINUX")) return "only on platform LINUX";
	if (platform != p_SUNOS && boost::iequals( flag, "SUNOS")) return "only on platform SUNOS";
	if (platform != p_FREEBSD && boost::iequals( flag, "FREEBSD")) return "only on platform FREEBSD";

#if !(WITH_LUA)
	if (boost::iequals( flag, "LUA")) return "WITH_LUA=1 ";
#endif
#if !(WITH_LIBHPDF)
	if (boost::iequals( flag, "LIBHPDF")) return "WITH_LIBHPDF=1 ";
#endif
#if !(WITH_SSL)
	if (boost::iequals( flag, "SSL")) return "WITH_SSL=1 ";
#endif
#if !(WITH_QT)
	if (boost::iequals( flag, "QT")) return "WITH_QT=1 ";
#endif
#if !(WITH_PAM)
	if (boost::iequals( flag, "PAM")) return "WITH_PAM=1 ";
#endif
#if !(WITH_SASL)
	if (boost::iequals( flag, "SASL")) return "WITH_SASL=1 ";
#endif
#if !(WITH_SQLITE3)
	if (boost::iequals( flag, "SQLITE3")) return "WITH_SQLITE3=1 ";
#endif
#if !(WITH_LOCAL_SQLITE3)
	if (boost::iequals( flag, "LOCAL_SQLITE3")) return "WITH_LOCAL_SQLITE3=1 ";
#endif
#if !(WITH_PGSQL)
	if (boost::iequals( flag, "PGSQL")) return "WITH_PGSQL=1 ";
#endif
#if !(WITH_LIBXML2)
	if (boost::iequals( flag, "LIBXML2")) return "WITH_LIBXML2=1 ";
#endif
#if !(WITH_LIBXSLT)
	if (boost::iequals( flag, "LIBXSLT")) return "WITH_LIBXSLT=1 ";
#endif
#if !(WITH_ICU)
	if (boost::iequals( flag, "ICU")) return "WITH_ICU=1 ";
#endif
	return "";
}

static void readFile( const std::string& pt, std::vector<std::string>& hdr, std::vector<std::string>& out, std::string& requires, std::vector<std::string>& outputfile)
{
	std::string element;
	char chb;
	std::fstream infh;
	infh.exceptions( std::ifstream::failbit | std::ifstream::badbit);
	infh.open( pt.c_str(), std::ios::in | std::ios::binary);
	std::string splitstr;
	std::string::const_iterator splititr;
	enum
	{
		PARSE_HDR,
		PARSE_OUT,
		PARSE_NOT
	}
	type = PARSE_HDR;

	while (infh.read( &chb, sizeof(chb)))
	{
		if (chb == '\r' || chb == '\n') break;
		splitstr.push_back( chb);
	}
	splititr = splitstr.begin();
	if (splititr == splitstr.end())
	{
		throw std::runtime_error( "illegal test definition file. no split tag defined at file start");
	}
	while (infh.read( &chb, sizeof(chb)))
	{
		if (chb != '\r' && chb != '\n') break;
	}
	do
	{
		if (chb != *splititr) break;
		++splititr;
	}
	while (splititr != splitstr.end() && infh.read( &chb, sizeof(chb)));

	if (splititr != splitstr.end())
	{
		throw std::runtime_error( "illegal test definition file. header expected after split tag definition");
	}
	splititr = splitstr.begin();

	while (infh.read( &chb, sizeof(chb)))
	{
		if (type == PARSE_HDR)
		{
			if (chb == '\n')
			{
				std::string tag( std::string( element.c_str(), element.size()));
				boost::trim( tag);
				if (boost::iequals( tag, "end"))
				{
					infh.close();
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
				element.push_back( chb);
			}
		}
		else if (type == PARSE_OUT)
		{
			element.push_back( chb);
			if (chb == *splititr)
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
			if (chb == *splititr)
			{
				++splititr;
				if (splititr == splitstr.end())
				{
					element.clear();
					type = PARSE_HDR;
					splititr = splitstr.begin();
				}
			}
			else if (chb > ' ' || chb < 0)
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

static void writeFile( const std::string& pt, const std::string& content)
{
	std::fstream ff( pt.c_str(), std::ios::out | std::ios::binary);
	ff.exceptions( std::ifstream::failbit | std::ifstream::badbit);
	ff.write( content.c_str(), content.size());
}


TestDescription::TestDescription( const std::string& pt, const char* argv0)
{
	static boost::filesystem::path testdir = boost::filesystem::system_complete( argv0).parent_path();

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

