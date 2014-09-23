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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file libxml2_filter.cpp
#include "libxml2_filter.hpp"
#include "inputfilterImpl.hpp"
#include "outputfilterImpl.hpp"
#include "xsltMapper.hpp"
#include "types/docmetadata.hpp"
#include "utils/parseUtils.hpp"
#include <cstddef>
#include <cstring>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/encoding.h"
#include "libxml/xmlwriter.h"
#include "libxml/xmlsave.h"
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

namespace {

struct LibXml2Init
{
	LibXml2Init()
	{
		LIBXML_TEST_VERSION;
	}

	~LibXml2Init()
	{
		xmlCleanupParser();
	}
};
static LibXml2Init libXml2Init;
}//end anonymous namespace

struct Libxml2Filter :public Filter
{
	Libxml2Filter( const char* encoding, bool withEmpty)
	{
		m_inputfilter.reset( new InputFilterImpl( withEmpty));
		OutputFilterImpl* oo = new OutputFilterImpl( m_inputfilter->getMetaDataRef());
		m_outputfilter.reset( oo);
		if (encoding)
		{
			m_outputfilter->setAttribute( "encoding", encoding);
		}
	}
};

static void getArguments( const std::vector<FilterArgument>& arg, const char*& encoding, bool& withEmpty)
{
	encoding = 0;
	withEmpty = true; //... W3C standard

	std::vector<FilterArgument>::const_iterator ai = arg.begin(), ae = arg.end();
	for (; ai != ae; ++ai)
	{
		if (ai->first.empty() || boost::algorithm::iequals( ai->first, "encoding"))
		{
			if (encoding)
			{
				if (ai->first.empty())
				{
					throw std::runtime_error( "too many filter arguments");
				}
				else
				{
					throw std::runtime_error( "duplicate filter argument 'encoding'");
				}
			}
			encoding = ai->second.c_str();
			break;
		}
		if (boost::algorithm::iequals( ai->first, "empty"))
		{
			withEmpty = utils::getBooleanTokenValue( ai->second);
		}
		else
		{
			throw std::runtime_error( std::string( "unknown filter argument '") + ai->first + "'");
		}
	}
}

Filter* Libxml2FilterType::create( const std::vector<FilterArgument>& arg) const
{
	const char* encoding = 0;
	bool withEmpty = true;
	getArguments( arg, encoding, withEmpty);
	return new Libxml2Filter( encoding, withEmpty);
}


