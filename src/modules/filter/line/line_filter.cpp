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
/// \file line_filter.cpp
/// \brief Implementation of a filter reading/writing line by line

#include "line_filter.hpp"
#include "textwolf/charset.hpp"
#include "textwolf/sourceiterator.hpp"
#include "textwolf/textscanner.hpp"
#include "textwolf/cstringiterator.hpp"
#include <cstring>
#include <cstddef>
#include <algorithm>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace langbind;

namespace {

/// \class OutputFilterImpl
template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct OutputFilterImpl :public OutputFilter
{
	/// \brief Constructor
	OutputFilterImpl( const types::DocMetaDataR& inheritedMetaData, const IOCharset& iocharset_=IOCharset())
		:OutputFilter("line", inheritedMetaData)
		,m_elemitr(0)
		,m_output(iocharset_)
	{
		setFlags( langbind::FilterBase::PropagateNoArray);
		setFlags( langbind::FilterBase::PropagateNoAttr);
	}

	/// \brief Copy constructor
	/// \param [in] o output filter to copy
	OutputFilterImpl( const OutputFilterImpl& o)
		:OutputFilter(o)
		,m_elembuf(o.m_elembuf)
		,m_elemitr(o.m_elemitr)
		,m_output(o.m_output)
	{
		setFlags( langbind::FilterBase::PropagateNoArray);
		setFlags( langbind::FilterBase::PropagateNoAttr);
	}

	/// \brief self copy
	/// \return copy of this
	virtual OutputFilter* copy() const
	{
		return new OutputFilterImpl( *this);
	}

	/// \brief Prints a character string to an STL back insertion sequence buffer in the IO character set encoding
	/// \param [in] src pointer to string to print
	/// \param [in] srcsize size of src in bytes
	void printToBuffer( const char* src, std::size_t srcsize, std::string& buf) const
	{
		textwolf::CStringIterator itr( src, srcsize);
		textwolf::TextScanner<textwolf::CStringIterator,AppCharset> ts( itr);

		textwolf::UChar ch;
		while ((ch = ts.chr()) != 0)
		{
			m_output.print( ch, buf);
			++ts;
		}
	}

	/// \brief Implementation of OutputFilter::print(typename OutputFilter::ElementType,const void*,std::size_t)
	/// \param [in] type type of the element to print
	/// \param [in] element pointer to the element to print
	/// \param [in] elementsize size of the element to print in bytes
	/// \return true, if success, false else
	virtual bool print( typename OutputFilter::ElementType type, const void* element, std::size_t elementsize)
	{
		if (m_elembuf.size() > outputChunkSize() && outputChunkSize())
		{
			if (m_elemitr == m_elembuf.size())
			{
				m_elembuf.clear();
				m_elemitr = 0;
			}
			else
			{
				setState( EndOfBuffer);
				return false;
			}
		}
		setState( Open);
		if (type == Value)
		{
			printToBuffer( (const char*)element, elementsize, m_elembuf);
			m_output.print( '\n', m_elembuf);
		}
		return true;
	}

	/// \brief Implementation of OutputFilter::getOutput( const void*&,std::size_t&)
	virtual void getOutput( const void*& buf, std::size_t& bufsize)
	{
		buf = (const void*)(m_elembuf.c_str() + m_elemitr);
		bufsize = m_elembuf.size() - m_elemitr;
		m_elemitr = m_elembuf.size();
	}

	/// \brief Implementation of OutputFilter::close()
	virtual bool close(){return true;}

private:
	std::string m_elembuf;				///< buffer for the currently printed element
	std::size_t m_elemitr;				///< iterator to pass it to output
	IOCharset m_output;
};

/// \class InputFilterImpl
template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct InputFilterImpl :public InputFilter
{
	typedef textwolf::TextScanner<textwolf::SrcIterator,IOCharset> TextScanner;

	/// \brief Constructor
	InputFilterImpl( const char* encoding_, const IOCharset& iocharset_=IOCharset())
		:InputFilter("line")
		,m_charset(iocharset_)
		,m_itr(iocharset_)
		,m_output(AppCharset())
		,m_src(0)
		,m_srcsize(0)
		,m_srcend(false)
		,m_srcclosed(false)
		,m_linecomplete(false)
	{
		setAttribute( "encoding", encoding_?encoding_:"UTF-8");
		setState( Open);
		m_itr.setSource( textwolf::SrcIterator( m_src, m_srcsize, &m_eom));
	}

	/// \brief Constructor
	InputFilterImpl( const types::DocMetaData& md, const IOCharset& iocharset_=IOCharset())
		:InputFilter("line", md)
		,m_charset(iocharset_)
		,m_itr(iocharset_)
		,m_output(AppCharset())
		,m_src(0)
		,m_srcsize(0)
		,m_srcend(false)
		,m_srcclosed(false)
		,m_linecomplete(false)
	{
		setState( Open);
		m_itr.setSource( textwolf::SrcIterator( m_src, m_srcsize, &m_eom));
	}

	/// \brief Copy constructor
	/// \param [in] o output filter to copy
	InputFilterImpl( const InputFilterImpl& o)
		:InputFilter( o)
		,m_charset(o.m_charset)
		,m_itr(o.m_itr)
		,m_output(o.m_output)
		,m_elembuf(o.m_elembuf)
		,m_src(o.m_src)
		,m_srcsize(o.m_srcsize)
		,m_srcend(o.m_srcend)
		,m_srcclosed(o.m_srcclosed)
		,m_linecomplete(o.m_linecomplete)
		{
			m_itr.setSource( textwolf::SrcIterator( m_src, m_srcsize, &m_eom));
		}

	/// \brief Implement InputFilter::copy()
	virtual InputFilter* copy() const
	{
		return new InputFilterImpl( *this);
	}

	/// \brief implement interface member InputFilter::putInput(const void*,std::size_t,bool)
	virtual void putInput( const void* ptr, std::size_t size, bool end)
	{
		m_src = (const char*)ptr;
		m_srcend = end;
		m_srcsize = size;
		m_itr.setSource( textwolf::SrcIterator( m_src, m_srcsize, end?0:&m_eom));
		setState( Open);
	}

	/// \brief implement interface member InputFilter::getNext( typename InputFilter::ElementType&,const void*&,std::size_t&)
	virtual bool getNext( typename InputFilter::ElementType& type, const void*& element, std::size_t& elementsize)
	{
		if (m_linecomplete)
		{
			m_elembuf.clear();
			m_linecomplete = false;
		}
		setState( Open);
		if (setjmp(m_eom) != 0)
		{
			setState( EndOfMessage);
			return 0;
		}
		type = Value;

		textwolf::UChar ch;
		while ((ch = *m_itr) != 0)
		{
			if (ch == '\r')
			{
				++m_itr;
				continue;
			}
			if (ch == '\n')
			{
				element = m_elembuf.c_str();
				elementsize = m_elembuf.size();
				++m_itr;
				m_linecomplete = true;
				return true;
			}
			else
			{
				m_output.print( ch, m_elembuf);
				++m_itr;
			}
		}
		if (m_elembuf.size() != 0)
		{
			element = m_elembuf.c_str();
			elementsize = m_elembuf.size();
			m_linecomplete = true;
			return true;
		}
		else if (!m_srcclosed)
		{
			type = InputFilter::CloseTag;
			element = 0;
			elementsize = 0;
			m_srcclosed = true;
			return true;
		}
		return false;
	}

	virtual const types::DocMetaData* getMetaData()
	{
		return getMetaDataRef().get();
	}

private:
	IOCharset m_charset;			///< character set encoding
	TextScanner m_itr;			///< iterator on source
	jmp_buf m_eom;				///< end of message trigger
	AppCharset m_output;			///< output
	std::string m_elembuf;			///< buffer for current line
	const char* m_src;			///< pointer to current chunk parsed
	std::size_t m_srcsize;			///< size of the current chunk parsed in bytes
	bool m_srcend;				///< true if end of message is in current chunk parsed
	bool m_srcclosed;			///< true if the finishing close tag has been returned
	bool m_linecomplete;			///< true if the last getNext could complete a line
};

}//end anonymous namespace


class LineFilter :public Filter
{
public:
	LineFilter( const char *encoding=0)
	{
		if (!encoding)
		{
			m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF8>( encoding));
			m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF8>( m_inputfilter->getMetaDataRef()));
		}
		else
		{
			std::string enc;
			parseEncoding( enc, encoding);

			if ((enc.size() >= 8 && std::memcmp( enc.c_str(), "isolatin", 8)== 0)
			||  (enc.size() >= 7 && std::memcmp( enc.c_str(), "iso8859", 7) == 0))
			{
				const char* codepage = enc.c_str() + ((enc.c_str()[4] == 'l')?8:7);
				if (std::strlen( codepage) > 1 || codepage[0] < '0' || codepage[0] > '9')
				{
					throw std::runtime_error( "unknown iso-latin code page index");
				}
				if (codepage[0] == '1')
				{
					m_inputfilter.reset( new InputFilterImpl<textwolf::charset::IsoLatin>( encoding));
					m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::IsoLatin>( m_inputfilter->getMetaDataRef()));
				}
				else
				{
					m_inputfilter.reset( new InputFilterImpl<textwolf::charset::IsoLatin>( encoding, textwolf::charset::IsoLatin( codepage[0] - '0')));
					m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::IsoLatin>( m_inputfilter->getMetaDataRef(), textwolf::charset::IsoLatin( codepage[0] - '0')));
				}
			}
			else if (enc.size() == 0 || enc == "utf8")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF8>( encoding));
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF8>( m_inputfilter->getMetaDataRef()));
			}
			else if (enc == "utf16" || enc == "utf16be")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF16BE>( encoding));
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF16BE>( m_inputfilter->getMetaDataRef()));
			}
			else if (enc == "utf16le")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF16LE>( encoding));
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF16LE>( m_inputfilter->getMetaDataRef()));
			}
			else if (enc == "ucs2" || enc == "ucs2be")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UCS2BE>( encoding));
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UCS2BE>( m_inputfilter->getMetaDataRef()));
			}
			else if (enc == "ucs2le")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UCS2LE>( encoding));
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UCS2LE>( m_inputfilter->getMetaDataRef()));
			}
			else if (enc == "utf32" || enc == "ucs4" || enc == "utf32be" || enc == "ucs4be")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UCS4BE>( encoding));
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UCS4BE>( m_inputfilter->getMetaDataRef()));
			}
			else if (enc == "utf32le" || enc == "ucs4le")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UCS4LE>( encoding));
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UCS4LE>( m_inputfilter->getMetaDataRef()));
			}
			else
			{
				throw std::runtime_error( "unknown character set encoding");
			}
		}
	}

	static void parseEncoding( std::string& dest, const std::string& src)
	{
		dest.clear();
		std::string::const_iterator cc=src.begin();
		for (; cc != src.end(); ++cc)
		{
			if (*cc <= ' ') continue;
			if (*cc == '-') continue;
			if (*cc == ' ') continue;
			dest.push_back( ::tolower( *cc));
		}
	}
};


static const char* getArgumentEncoding( const std::vector<FilterArgument>& arg)
{
	const char* encoding = 0;
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
		else
		{
			throw std::runtime_error( std::string( "unknown filter argument '") + ai->first + "'");
		}
	}
	return encoding;
}

Filter* LineFilterType::create( const std::vector<FilterArgument>& arg) const
{
	return new LineFilter( getArgumentEncoding( arg));
}





