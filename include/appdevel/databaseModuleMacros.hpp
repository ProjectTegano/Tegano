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
/// \file appdevel/databaseModuleMacros.hpp
/// \brief Macros for a module for defining a database interface
#include "module/moduleInterface.hpp"
#include "module/configuredObjectConstructor.hpp"
#include "config/configurationObject.hpp"
#include "logger/logger-v1.hpp"
#include <boost/lexical_cast.hpp>

/// \brief Defines a simple database (one database object per unit) interface
#define WF_DATABASE(NAME,DBCLASS,CONFIGCLASS) \
{\
	class Unit :public _Wolframe::db::DatabaseUnit \
	{\
	public:\
		Unit( const CONFIGCLASS* cfg)\
			:m_id(cfg->id()),m_db(cfg){}\
		virtual const std::string& id() const\
		{\
			return m_id;\
		}\
		virtual DBCLASS* database()\
		{\
			return &m_db;\
		}\
	private:\
		std::string m_id;\
		DBCLASS m_db;\
	};\
	class ModuleObjectEnvelope \
		:public Unit \
		,public _Wolframe::module::BaseObject\
	{\
	public:\
		ModuleObjectEnvelope( const CONFIGCLASS* cfg) :Unit(cfg){}\
	};\
	class Constructor\
		:public _Wolframe::module::ConfiguredObjectConstructor\
	{\
	public:\
		Constructor()\
			:_Wolframe::module::ConfiguredObjectConstructor( _Wolframe::module::ObjectDescription::DATABASE_OBJECT, NAME "Database", "database", NAME){}\
		virtual _Wolframe::module::BaseObject* object( const _Wolframe::config::ConfigurationObject& cfgi) const\
		{\
			const CONFIGCLASS* cfg = dynamic_cast<const CONFIGCLASS*>(&cfgi);\
			if (!cfg)\
			{\
				LOG_ERROR << "internal: wrong configuration interface passed to " NAME " DatabaseUnit constructor";\
				return 0;\
			}\
			return new ModuleObjectEnvelope( cfg);\
		}\
		virtual _Wolframe::config::ConfigurationObject* configuration() const\
		{\
			return new CONFIGCLASS( NAME "Database", "Database", NAME);\
		}\
		static const _Wolframe::module::ObjectConstructor* impl()\
		{\
			static const Constructor rt;\
			return &rt;\
		}\
	};\
}


/// \brief Defines a test database interface (simple database with initializer class with configuration as constructor argument that is constructed before the database and destructed after)
#define WF_TEST_DATABASE(NAME,DBCLASS,CONFIGCLASS,DBINITCLASS) \
{\
	class Unit :public _Wolframe::db::DatabaseUnit \
	{\
	public:\
		Unit( const CONFIGCLASS* cfg)\
			:m_id(cfg->id()),m_dbinit(cfg),m_db(cfg){}\
		virtual const std::string& id() const\
		{\
			return m_id;\
		}\
		virtual DBCLASS* database()\
		{\
			return &m_db;\
		}\
	private:\
		std::string m_id;\
		DBINITCLASS m_dbinit;\
		DBCLASS m_db;\
	};\
	class ModuleObjectEnvelope \
		:public Unit \
		,public _Wolframe::module::BaseObject\
	{\
	public:\
		ModuleObjectEnvelope( const CONFIGCLASS* cfg) :Unit(cfg){}\
	};\
	class Constructor\
		:public _Wolframe::module::ConfiguredObjectConstructor\
	{\
	public:\
		Constructor()\
			:_Wolframe::module::ConfiguredObjectConstructor( _Wolframe::module::ObjectDescription::DATABASE_OBJECT, NAME "Database", "database", NAME){}\
		virtual _Wolframe::module::BaseObject* object( const _Wolframe::config::ConfigurationObject& cfgi) const\
		{\
			const CONFIGCLASS* cfg = dynamic_cast<const CONFIGCLASS*>(&cfgi);\
			if (!cfg)\
			{\
				LOG_ERROR << "internal: wrong configuration interface passed to " NAME " DatabaseUnit constructor";\
				return 0;\
			}\
			return new ModuleObjectEnvelope( cfg);\
		}\
		virtual _Wolframe::config::ConfigurationObject* configuration() const\
		{\
			return new CONFIGCLASS( NAME "Database");\
		}\
		static const _Wolframe::module::ObjectConstructor* impl()\
		{\
			static const Constructor rt;\
			return &rt;\
		}\
	};\
}

