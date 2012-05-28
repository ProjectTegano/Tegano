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
but WITHOUT ANY WARRANTY; without even the ied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file filter_token_filter.cpp
///\brief Filter implementation a filter for debug input/output

#include "filter/token_filter.hpp"
#include "filter/textwolf/charset.hpp"
#include "filter/textwolf/sourceiterator.hpp"
#include "filter/textwolf/textscanner.hpp"
#include "filter/textwolf/cstringiterator.hpp"
#include <cstring>
#include <cstddef>

using namespace _Wolframe;
using namespace langbind;

namespace {

static bool getElementType( InputFilter::ElementType& et, char ch)
{
	switch (ch)
	{
		case '>': et = InputFilter::OpenTag; return true;
		case '<': et = InputFilter::CloseTag; return true;
		case '@': et = InputFilter::Attribute; return true;
		case '=': et = InputFilter::Value; return true;
	}
	return false;
}

static char getElementTag( OutputFilter::ElementType tp)
{
	switch (tp)
	{
		case InputFilter::OpenTag: return '>';
		case InputFilter::CloseTag: return '<';
		case InputFilter::Attribute: return '@';
		case InputFilter::Value: return '=';
	}
	return '\0';
}


///\class OutputFilterImpl
template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct OutputFilterImpl :public OutputFilter
{
	///\brief Constructor
	OutputFilterImpl()
		:m_elemitr(0){}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	OutputFilterImpl( const OutputFilterImpl& o)
		:OutputFilter(o),m_elemitr(o.m_elemitr){}

	///\brief self copy
	///\return copy of this
	virtual OutputFilter* copy() const
	{
		return new OutputFilterImpl( *this);
	}

	///\brief print a value with EOL escaped
	///\param [in] src pointer to attribute value string to print
	///\param [in] srcsize size of src in bytes
	///\param [in,out] buf buffer to print to
	static void printToBufferEscEOL( const char* src, std::size_t srcsize, std::string& buf)
	{
		textwolf::CStringIterator itr( src, srcsize);
		textwolf::TextScanner<textwolf::CStringIterator,AppCharset> ts( itr);

		textwolf::UChar ch;
		while ((ch = ts.chr()) != 0)
		{
			if (ch == '\n')
			{
				IOCharset::print( ' ', buf);
			}
			else
			{
				IOCharset::print( ch, buf);
			}
			++ts;
		}
	}

	bool emptybuf()
	{
		std::size_t nn = m_elembuf.size() - m_elemitr;
		m_elemitr += write( m_elembuf.c_str() + m_elemitr, nn);
		if (m_elemitr == m_elembuf.size())
		{
			m_elembuf.clear();
			m_elemitr = 0;
			return true;
		}
		return false;
	}

	///\brief Implementation of OutputFilter::print(typename OutputFilter::ElementType,const void*,std::size_t)
	///\param [in] type type of the element to print
	///\param [in] element pointer to the element to print
	///\param [in] elementsize size of the element to print in bytes
	///\return true, if success, false else
	virtual bool print( typename OutputFilter::ElementType type, const void* element, std::size_t elementsize)
	{
		setState( Open);
		if (m_elemitr < m_elembuf.size())
		{
			// there is something to print left from last time
			if (!emptybuf())
			{
				setState( EndOfBuffer);
				return false;
			}
			// we finished the printing left
			return true;
		}
		IOCharset::print( getElementTag( type), m_elembuf);
		printToBufferEscEOL( (const char*)element, elementsize, m_elembuf);
		if (!emptybuf())
		{
			setState( EndOfBuffer);
			return false;
		}
		return true;
	}
private:
	std::string m_elembuf;				//< buffer for the currently printed element
	std::size_t m_elemitr;				//< iterator to pass it to output
};

///\class InputFilterImpl
template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct InputFilterImpl :public InputFilter
{
	typedef textwolf::TextScanner<textwolf::SrcIterator,IOCharset> TextScanner;

	///\brief Constructor
	InputFilterImpl()
		:m_tag(0)
		,m_src(0)
		,m_srcsize(0)
		,m_srcend(false){}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	InputFilterImpl( const InputFilterImpl& o)
		:InputFilter( o)
		,m_itr(o.m_itr)
		,m_tag(o.m_tag)
		,m_elembuf(o.m_elembuf)
		,m_src(o.m_src)
		,m_srcsize(o.m_srcsize)
		,m_srcend(o.m_srcend){}

	///\brief self copy
	///\return copy of this
	virtual InputFilter* copy() const
	{
		return new InputFilterImpl( *this);
	}

	///\brief implement interface member InputFilterImpl::putInput(const void*,std::size_t,bool)
	virtual void putInput( const void* ptr, std::size_t size, bool end)
	{
		m_src = (const char*)ptr;
		m_srcend = end;
		m_srcsize = size;
		m_itr.setSource( textwolf::SrcIterator( m_src, m_srcsize, m_srcend));
	}

	virtual void getRest( const void*& ptr, std::size_t& size, bool& end)
	{
		ptr = m_src + m_itr.getPosition();
		size = m_srcsize - m_itr.getPosition();
		end = m_srcend;
	}

	///\brief implement interface member InputFilter::getNext( typename InputFilter::ElementType&,const void*&,std::size_t&)
	virtual bool getNext( typename InputFilter::ElementType& type, const void*& element, std::size_t& elementsize)
	{
		if (state() == Open)
		{
			m_elembuf.clear();
		}
		else
		{
			setState( Open);
		}
		type = Value;
		try
		{
			if (m_tag)
			{
				if (!getElementType( type, m_tag))
				{
					setState( InputFilter::Error, "textwolf: Unknown token tag");
				}
			}
			else
			{
				char tg = m_itr.ascii();
				if (!getElementType( type, tg))
				{
					setState( InputFilter::Error, "textwolf: Unknown token tag");
				}
				m_tag = tg;
				++m_itr;
			}
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
					m_tag = '\0';
					return true;
				}
				else
				{
					AppCharset::print( ch, m_elembuf);
					++m_itr;
				}
			}
			if (m_elembuf.size() != 0)
			{
				element = m_elembuf.c_str();
				elementsize = m_elembuf.size();
				return true;
			}
		}
		catch (textwolf::SrcIterator::EoM)
		{
			setState( EndOfMessage);
		}
		return false;
	}
private:
	TextScanner m_itr;		//< src iterator
	char m_tag;			//< tag defining the currently parsed element type
	std::string m_elembuf;		//< buffer for current line => current token
	const char* m_src;		//< pointer to current chunk parsed
	std::size_t m_srcsize;		//< size of the current chunk parsed in bytes
	bool m_srcend;			//< true if end of message is in current chunk parsed
};

}//end anonymous namespace


class TokenFilter :public Filter
{
public:
	TokenFilter( const char* encoding=0)
	{
		if (!encoding)
		{
			m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF8>());
			m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF8>());
		}
		else
		{
			std::string enc;
			parseEncoding( enc, encoding);

			if ((enc.size() >= 8 && std::memcmp( enc.c_str(), "isolatin", enc.size())== 0)
			||  (enc.size() >= 7 && std::memcmp( enc.c_str(), "iso8859", enc.size()) == 0))
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::IsoLatin1>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::IsoLatin1>());
			}
			else if (enc.size() == 0 || enc == "utf8")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF8>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF8>());
			}
			else if (enc == "utf16" || enc == "utf16be")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF16BE>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF16BE>());
			}
			else if (enc == "utf16le")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UTF16LE>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UTF16LE>());
			}
			else if (enc == "ucs2" || enc == "ucs2be")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UCS2BE>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UCS2BE>());
			}
			else if (enc == "ucs2le")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UCS2LE>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UCS2LE>());
			}
			else if (enc == "ucs4" || enc == "ucs4be")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UCS4BE>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UCS4BE>());
			}
			else if (enc == "ucs4le")
			{
				m_inputfilter.reset( new InputFilterImpl<textwolf::charset::UCS4LE>());
				m_outputfilter.reset( new OutputFilterImpl<textwolf::charset::UCS4LE>());
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

Filter TokenFilterFactory::create( const char* encoding) const
{
	return TokenFilter( encoding);
}

