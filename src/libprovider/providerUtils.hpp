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
/// \brief Utility inline helper functions for providers

#ifndef _Wolframe_PROVIDER_UITILITIES_HPP_INCLUDED
#define _Wolframe_PROVIDER_UITILITIES_HPP_INCLUDED

#include "config/configurationObject.hpp"
#include "module/moduleDirectory.hpp"
#include "module/configuredObjectConstructor.hpp"
#include "module/simpleObjectConstructor.hpp"
#include "module/objectDescription.hpp"
#include "logger/logger-v1.hpp"
#include <string>
#include <vector>

namespace {

template <class OBJECT>
static OBJECT* createSimpleProviderObject( const std::string& logPrefix, const _Wolframe::module::SimpleObjectConstructor* constructor)
{
	using namespace _Wolframe;
	module::BaseObject* baseobj = constructor->object();
	OBJECT* obj = dynamic_cast<OBJECT*>( baseobj);
	if (!baseobj)
	{
		LOG_ERROR << logPrefix << "failed to create object '" << constructor->className() << "' (without configuration)";
		return 0;
	}
	else if (!obj)
	{
		LOG_ERROR << logPrefix << "internal: object '" << constructor->className() << "' created (without configuration) is not of expected type";
		delete baseobj;
		return 0;
	}
	else
	{
		return obj;
	}
}

template <class OBJECT>
static OBJECT* createConfiguredProviderObject( const std::string& logPrefix, const _Wolframe::module::ConfiguredObjectConstructor* constructor, const _Wolframe::config::ConfigurationObject* configuration)
{
	using namespace _Wolframe;
	module::BaseObject* baseobj = constructor->object( *configuration);
	OBJECT* obj = dynamic_cast<OBJECT*>( baseobj);
	if (!baseobj)
	{
		LOG_ERROR << logPrefix << "failed to create object '" << constructor->className() << "' (from configuration " << configuration->configSection() << "/" << configuration->configKeyword() << ")";
		return 0;
	}
	else if (!obj)
	{
		LOG_ERROR << logPrefix << "internal: object '" << constructor->className() << "' created (from configuration " << configuration->configSection() << "/" << configuration->configKeyword() << ") is not of expected type";
		delete baseobj;
		return 0;
	}
	else
	{
		return obj;
	}
}


template <class OBJECT>
static bool createConfiguredProviderObjects( const std::string& logPrefix, std::vector<OBJECT*>& res, const std::vector<_Wolframe::config::ConfigurationObject*>& cfgobjs, const _Wolframe::module::ModuleDirectory* modules)
{
	using namespace _Wolframe;
	bool rt = true;
	std::vector<config::ConfigurationObject*>::const_iterator oi = cfgobjs.begin(), oe = cfgobjs.end();
	for (; oi != oe; ++oi)
	{
		const module::ConfiguredObjectConstructor* constructor = modules->getConfiguredObjectConstructor((*oi)->className());
		if (constructor)
		{
			OBJECT* obj = createConfiguredProviderObject<OBJECT>( logPrefix, constructor, *oi);
			if (obj)
			{
				res.push_back( obj);
			}
			else
			{
				rt = false;
			}
		}
		else
		{
			LOG_ALERT << logPrefix << "unknown object '" << (*oi)->className() << "'";
			rt = false;
		}
	}
	return rt;
}

}//anonymous namespace
#endif

