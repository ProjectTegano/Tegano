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
/// \brief Base class for object properties used for identifying an object
/// \file module/objectDescription.hpp

#ifndef _MODULE_OBJECT_DESCRIPTION_HPP_INCLUDED
#define _MODULE_OBJECT_DESCRIPTION_HPP_INCLUDED
#include <string>

namespace _Wolframe {
namespace module {

/// \class ObjectDescription
/// \brief Description of an object in a module
class ObjectDescription
{
public:
	enum Category
	{
		ConfigurableObject,
		SimpleObject
	};

	enum TypeId
	{
		AUTHENTICATION_OBJECT		=0x0010,
		AUTHORIZATION_OBJECT		=0x0020,
		AUDIT_OBJECT			=0x0110,
		DATABASE_OBJECT			=0x0210,
		FILTER_OBJECT			=0x0310,
		FORM_FUNCTION_OBJECT		=0x0410,
		NORMALIZE_FUNCTION_OBJECT	=0x0420,
		CUSTOM_DATA_TYPE_OBJECT		=0x0430,
		DOCTYPE_DETECTOR_OBJECT		=0x0510,
		CMD_HANDLER_OBJECT		=0x0610,
		PROTOCOL_HANDLER_OBJECT		=0x0620,
		PROGRAM_TYPE_OBJECT		=0x0710,
		DDL_COMPILER_OBJECT		=0x0720,
		RUNTIME_ENVIRONMENT_OBJECT	=0x0730,
		TEST_OBJECT			=0x9990
	};

	static const char* typeIdName( TypeId tp)
	{
		switch (tp)
		{
			case AUTHENTICATION_OBJECT: return "Authentication";
			case AUTHORIZATION_OBJECT: return "Authorization";
			case AUDIT_OBJECT: return "Audit";
			case DATABASE_OBJECT: return "Database";
			case FILTER_OBJECT: return "Filter";
			case FORM_FUNCTION_OBJECT: return "Form Function";
			case NORMALIZE_FUNCTION_OBJECT: return "Normalize Function";
			case CUSTOM_DATA_TYPE_OBJECT: return "Custom Data Type";
			case DOCTYPE_DETECTOR_OBJECT: return "Document Type/Format Detector";
			case CMD_HANDLER_OBJECT: return "Command Handler";
			case PROTOCOL_HANDLER_OBJECT: return "Protocol Handler";
			case PROGRAM_TYPE_OBJECT: return "Program Type";
			case DDL_COMPILER_OBJECT: return "DLL Compiler";
			case RUNTIME_ENVIRONMENT_OBJECT: return "Runtime Environment";
			case TEST_OBJECT: return "#Test#";
		}
		return "Unknown module type";
	}

	const char* typeIdName() const
	{
		return typeIdName( typeId());
	}

public:
	ObjectDescription( const TypeId& typeId_, const std::string& className_, const std::string& configSection_, const std::string& configKeyword_, const std::string& id_="")
		:m_category(ConfigurableObject)
		,m_typeId(typeId_)
		,m_className(className_)
		,m_configKeyword(configKeyword_)
		,m_configSection(configSection_)
		,m_id(id_)
	{
		update();
	}

	ObjectDescription( const TypeId& typeId_, const std::string& className_)
		:m_category(SimpleObject)
		,m_typeId(typeId_)
		,m_className(className_)
	{
		update();
	}

	ObjectDescription( const ObjectDescription& o)
		:m_category(o.m_category)
		,m_typeId(o.m_typeId)
		,m_className(o.m_className)
		,m_configKeyword(o.m_configKeyword)
		,m_configSection(o.m_configSection)
		,m_id(o.m_id)
		,m_logPrefix(o.m_logPrefix)
	{
		update();
	}

	~ObjectDescription(){}

	const std::string& className() const
	{
		return m_className;
	}

	const std::string& logPrefix() const
	{
		return m_logPrefix;
	}

	const std::string& configSection() const
	{
		return m_configSection;
	}

	const std::string& configKeyword() const
	{
		return m_configKeyword;
	}

	const std::string& id() const
	{
		return m_id;
	}

	void setId( const std::string& id_)
	{
		m_id = id_;
		update();
	}

	Category category() const
	{
		return m_category;
	}

	bool hasConfiguration() const
	{
		return category() == ConfigurableObject;
	}

	TypeId typeId() const
	{
		return m_typeId;
	}

private:
	void update()
	{
		switch (m_category)
		{
			case ConfigurableObject:
				m_logPrefix = m_configSection + "/" + m_configKeyword;
				if (m_id.size())
				{
					m_logPrefix.append( "(");
					m_logPrefix.append( m_id);
					m_logPrefix.append( ")");
				}
				break;
			case SimpleObject:
				m_logPrefix = m_className;
				break;
		}
		
	}

private:
	Category m_category;
	TypeId m_typeId;
	std::string m_className;
	std::string m_configKeyword;
	std::string m_configSection;
	std::string m_id;
	std::string m_logPrefix;
};

}}//namespace
#endif


