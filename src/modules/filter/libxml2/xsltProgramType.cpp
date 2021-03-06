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
///\file xsltProgramType.cpp
///\brief Function to create a program type object for xslt filters
#include "xsltProgramType.hpp"
#include "xsltMapper.hpp"
#include "inputfilterImpl.hpp"
#include "outputfilterImpl.hpp"
#include "processor/programLibrary.hpp"
#include "utils/fileUtils.hpp"
#include "logger/logger-v1.hpp"
#include <boost/shared_ptr.hpp>
#if WITH_LIBXSLT
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#endif
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

#if WITH_LIBXSLT

namespace {

struct XsltFilter :public Filter
{
	XsltFilter( const xsltStylesheetPtr stylesheet_, const std::vector<langbind::FilterArgument>& arg)
	{
		XsltMapper xsltmapper( stylesheet_, arg);
		InputFilterImpl impl( xsltmapper);
		m_inputfilter.reset( new InputFilterImpl( xsltmapper));
		m_outputfilter.reset( new OutputFilterImpl( xsltmapper, m_inputfilter->getMetaDataRef()));
	}
};


class XsltFilterType
	:public langbind::FilterType
{
public:
	XsltFilterType( const std::string& sourcefile_)
		:langbind::FilterType("xslt"),m_ptr(0)
	{
		m_ptr = xsltParseStylesheetFile( (const xmlChar *)sourcefile_.c_str());
		if (!m_ptr)
		{
			xmlError* err = xmlGetLastError();
			throw std::runtime_error( std::string("error loading XSLT program '") + sourcefile_ + "': '" + (err?err->message:"unspecified XSLT error") + "'");
		}
	}

	virtual ~XsltFilterType()
	{
		if (m_ptr) xsltFreeStylesheet( m_ptr);
	}

	virtual langbind::Filter* create( const std::vector<langbind::FilterArgument>& arg) const
	{
		return new XsltFilter( m_ptr, arg);
	}

private:
	XsltFilterType( const XsltFilterType& o)
		:FilterType(o)
	{
		throw std::logic_error( "non copyable XsltFilterConstructor");
	}
	xsltStylesheetPtr m_ptr;
};
}//anonymous namespace

XsltProgramType::~XsltProgramType()
{
	xsltCleanupGlobals();
}

bool XsltProgramType::is_mine( const std::string& filename) const
{
	boost::filesystem::path p( filename);
	return p.extension().string() == ".xslt";
}

void XsltProgramType::loadProgram( proc::ProgramLibrary& library, const std::string& filename)
{
	langbind::FilterTypeR fc( new XsltFilterType( filename));
	std::string filternme( utils::getFileStem(filename));
	library.defineFilterType( filternme, fc);
}

#else

XsltProgramType::~XsltProgramType(){}

bool XsltProgramType::is_mine( const std::string& filename) const
{
	return false;
}

void XsltProgramType::loadProgram( proc::ProgramLibrary&, const std::string&)
{
	throw std::runtime_error("XSLT support is not built-in (WITH_XSLT)");
}

#endif
