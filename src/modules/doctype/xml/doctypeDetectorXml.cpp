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
/// \file doctypeDetectorXml.cpp
/// \brief Implementation of document type and format recognition for XML
#include "doctypeDetectorXml.hpp"
#include "utils/parseUtils.hpp"
#include "utils/fileUtils.hpp"
#include "logger/logger-v1.hpp"
#include <cstring>
#include <stdexcept>

#undef _Wolframe_LOWLEVEL_DEBUG

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;

static bool isSpace( char ch)
{
	return ((unsigned char)ch <= 32);
}

static std::string getIdFromXMLDoctype( const std::string& doctype)
{
	std::string rootid;
	std::string publicid;
	std::string systemid;
	std::string::const_iterator itr=doctype.begin(), end=doctype.end();

	if (utils::parseNextToken( rootid, itr, end))
	{
		if (utils::parseNextToken( publicid, itr, end))
		{
			if (publicid == "PUBLIC")
			{
				if (!utils::parseNextToken( publicid, itr, end)
				||  !utils::parseNextToken( systemid, itr, end))
				{
					throw std::runtime_error( "illegal doctype definition (public id)");
				}
				return utils::getFileStem( systemid);
			}
			else if (publicid == "SYSTEM")
			{
				if (!utils::parseNextToken( systemid, itr, end))
				{
					throw std::runtime_error( "illegal doctype definition (system id)");
				}
				return utils::getFileStem( systemid);
			}
			else if (utils::parseNextToken( systemid, itr, end))
			{
				if (itr != end)
				{
					throw std::runtime_error( "illegal doctype definition (not terminated)");
				}
				return utils::getFileStem( systemid);
			}
			else
			{
				return utils::getFileStem( publicid);
			}
		}
		else
		{
			return rootid;
		}
	}
	else
	{
		return "";
	}
}

bool DoctypeDetectorXml::run()
{
	unsigned char ch = m_charparser.getNext();
	for (; ch != 0 && m_state != Done; ch = m_charparser.getNext())
	{
		if (ch == '.' && m_lastchar == '\n')
		{
			setState( Done);
			break;
		}
		m_lastchar = ch;

#ifdef _Wolframe_LOWLEVEL_DEBUG
		LOG_DEBUG << "[docformat detection] state " << stateName( m_state) << " char '" << ((ch < 32)?'_':(char)ch) << " [" << std::hex << (unsigned int)ch << std::dec << "]";
#endif
		switch (m_state)
		{
			case ParseStart:
				if (ch == '<')
				{
					setState( ParseXMLHeader0);
				}
				else if (isSpace( ch))
				{
					continue;
				}
				else
				{
					setState( Done);
				}
				break;

			case ParseXMLHeader0:
				if (ch == '?')
				{
					setState( ParseXMLHeader);
				}
				else if (ch == '!')
				{
					setState( ParseXMLDoctype1);
				}
				else if (ch == '>')
				{
					setState( Done);
				}
				else if (ch > 32)
				{
					setState( ParseXMLRootName);
					m_itembuf.clear();
					m_itembuf.push_back(ch);
				}
				else
				{
					m_itembuf.clear();
					setState( SearchXMLRootName);
				}
				break;

			case ParseXMLHeader:
				if (ch == '>')
				{
					const char* cc = std::strstr( m_itembuf.c_str(), "standalone");
					if (cc)
					{
						cc = std::strchr( cc, '=');
						if (cc) cc = std::strstr( cc, "yes");
						if (cc)
						{
							m_info.reset( new types::DoctypeInfo("XML",""));
							setState( Done);
							break;
						}
					}
					setState( SearchXMLDoctypeTag);
					m_itembuf.clear();
				}
				else
				{
					m_itembuf.push_back( ch);
					if (m_itembuf.size() > 128)
					{
						m_lasterror = "XML header not terminated";
						setState( Done);
					}
				}
				break;

			case SearchXMLDoctypeTag:
				if (ch == '<')
				{
					setState( ParseXMLDoctype0);
				}
				else if (ch > 32)
				{
					m_lasterror = "expected XML document type or root element";
					setState( Done);
				}
				break;

			case ParseXMLDoctype0:
				if (ch == '!')
				{
					setState( ParseXMLDoctype1);
				}
				else if (ch == '>')
				{
					setState( Done);
				}
				else if (ch > 32)
				{
					setState( ParseXMLRootName);
					m_itembuf.clear();
					m_itembuf.push_back(ch);
				}
				else
				{
					m_itembuf.clear();
					setState( SearchXMLRootName);
				}
				break;

			case ParseXMLDoctype1:
				if (ch == '-')
				{
					setState( SkipXMLComment);
				}
				else if (ch == 'D')
				{
					m_itembuf.push_back( ch);
					setState( ParseXMLDoctype2);
				}
				else
				{
					m_lasterror = "expected '<!DOCTYPE' or <!--";
					setState( Done);
				}
				break;

			case SkipXMLComment:
				if (ch == '>')
				{
					setState( SearchXMLDoctypeTag);
				}
				break;

			case ParseXMLDoctype2:
				if (ch <= ' ' && ch > 0)
				{
					if (m_itembuf != "DOCTYPE")
					{
						m_lasterror = "expected '<!DOCTYPE'";
						setState( Done);
					}
					setState( ParseXMLDoctype);
					m_itembuf.clear();
				}
				else
				{
					m_itembuf.push_back( ch);
					if (m_itembuf.size() > 8)
					{
						m_lasterror = "expected '<!DOCTYPE'";
						setState( Done);
					}
				}
				break;

			case ParseXMLDoctype:
				if (ch <= ' ' && ch > 0)
				{
					m_itembuf.push_back( ' ');
				}
				else if (ch == '>')
				{
					try
					{
						std::string doctype = getIdFromXMLDoctype( m_itembuf);
						m_info.reset( new types::DoctypeInfo( "XML", doctype));
					}
					catch (const std::runtime_error& e)
					{
						m_lasterror = e.what();
					}
					setState( Done);
				}
				else
				{
					m_itembuf.push_back( ch);
				}
				break;

			case SearchXMLRootName:
				if (ch == '>')
				{
					m_lasterror = "invalid XML root element";
					setState( Done);
				}
				if (ch > 32)
				{
					setState( ParseXMLRootName);
					m_itembuf.clear();
					m_itembuf.push_back(ch);
				}
				break;

			case ParseXMLRootName:
				if (ch != '>' && ch > ' ')
				{
					m_itembuf.push_back( ch);
					if (m_itembuf.size() > 128)
					{
						m_lasterror = "XML root element name too big";
						setState( Done);
					}
				}
				else
				{
					if (ch == '>')
					{
						m_lasterror = "No document type or schema definition found and document is not standalone";
						setState( Done);
						break;
					}
					else
					{
						setState( SearchXMLRootAttrib);
					}
				}
				break;

			case SearchXMLRootAttrib:
				if (ch == '>')
				{
					m_lasterror = "No document type or schema definition found and document is not standalone";
					setState( Done);
				}
				if (ch > ' ')
				{
					setState( ParseXMLRootAttrib);
					m_itembuf.clear();
					m_itembuf.push_back(ch);
				}
				break;

			case ParseXMLRootAttrib:
				if (ch != '>' && ch != '=')
				{
					if (ch == ':')
					{
						m_itembuf.clear();
					}
					else
					{
						m_itembuf.push_back( ch);
						if (m_itembuf.size() > 128)
						{
							m_lasterror = "XML root attribute name too big";
							setState( Done);
						}
					}
				}
				else
				{
					if (ch == '>')
					{
						m_lasterror = "No document type or schema definition found and document is not standalone";
						setState( Done);
					}
					else
					{
						m_isDoctypeAttrib = (m_itembuf == "schemaLocation" || m_itembuf == "noNamespaceSchemaLocation");
#ifdef _Wolframe_LOWLEVEL_DEBUG
						LOG_DEBUG << "[docformat detection] check document type attribute '" << m_itembuf << "' " << ((m_isDoctypeAttrib?"YES":"NO"));
#endif
						setState( ch <= '='?SearchXMLRootAttribQuote:SearchXMLRootAttribAssign);
					}
				}
				break;

			case SearchXMLRootAttribAssign:
				if (ch == '=')
				{
					setState( SearchXMLRootAttribQuote);
					m_itembuf.clear();
				}
				else if (ch > ' ')
				{
					m_lasterror = "invalid XML root attribute definition (missing value assingment)";
					setState( Done);
				}
				break;

			case SearchXMLRootAttribQuote:
				if (ch == '\'')
				{
					setState( ParseXMLRootAttribValueSQ);
				}
				else if (ch == '\"')
				{
					setState( ParseXMLRootAttribValueDQ);
				}
				else if (ch > ' ')
				{
					m_lasterror = "invalid XML root attribute definition (missing value assingment)";
					setState( Done);
				}
				break;

			case ParseXMLRootAttribValueSQ:
				if (ch == '\'')
				{
					if (m_isDoctypeAttrib)
					{
						m_info.reset( new types::DoctypeInfo("XML", utils::getFileStem( m_itembuf)));
						setState( Done);
					}
					else
					{
						setState( SearchXMLRootAttrib);
					}
				}
				else
				{
					if (ch <= ' ')
					{
						m_itembuf.clear();
					}
					else
					{
						m_itembuf.push_back( ch);
						if (m_itembuf.size() > 256)
						{
							m_lasterror = "XML schemaLocation attribute (schema definition) is too big";
							setState( Done);
						}
					}
				}
				break;

			case ParseXMLRootAttribValueDQ:
				if (ch == '\"')
				{
					if (m_isDoctypeAttrib)
					{
						m_info.reset( new types::DoctypeInfo("XML", utils::getFileStem( m_itembuf)));
						setState( Done);
					}
					else
					{
						setState( SearchXMLRootAttrib);
					}
				}
				else
				{
					if (ch <= ' ')
					{
						m_itembuf.clear();
					}
					else
					{
						m_itembuf.push_back( ch);
						if (m_itembuf.size() > 256)
						{
							m_lasterror = "XML schemaLocation attribute (schema definition) is too big";
							setState( Done);
						}
					}
				}
				break;

			case Done:
				break;
		}
	}
	if (ch == 0 && m_state != Done)
	{
		if (m_charparser.lastError())
		{
			m_lasterror = m_charparser.lastError();
		}
		return false;
	}
	return m_lasterror.empty();
}

void DoctypeDetectorXml::setState( State state_)
{
	m_state = state_;
	LOG_DATA << "STATE DoctypeDetectorXml " << stateName( m_state);
}





