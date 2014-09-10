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
/// \file config/configurationObject.hpp
/// \brief Base classes for the configuration structures

#ifndef _CONFIGURATION_OBJECT_HPP_INCLUDED
#define _CONFIGURATION_OBJECT_HPP_INCLUDED

#include "config/configurationObject.hpp"
#include "config/configurationObjectDescription.hpp"
#include "config/configurationTree.hpp"
#include <string>
#include <ostream>
#include <list>

// forward definition
namespace _Wolframe { namespace module {
	class ModuleDirectory;
}} // namespace _Wolframe::module

namespace _Wolframe {
namespace config {

/// \brief Base class for the configuration structures
class ConfigurationObject
	:public ConfigurationObjectDescription
{
public:
	/// Class constructor.
	/// \param[in] className_ 
	/// \param[in] configSection_ 
	/// \param[in] configKeyword_ 
	ConfigurationObject( const std::string& className_, const std::string& configSection_, const std::string& configKeyword_)
		:ConfigurationObjectDescription( className_, configSection_, configKeyword_)
	{}
	ConfigurationObject( const ConfigurationObjectDescription& d)
		:ConfigurationObjectDescription( d)
	{}

	virtual ~ConfigurationObject(){}

	/// \brief Parse the configuration section
	/// \param[in]	pt		property tree node
	/// \param[in]	node		the label of the node. It should be
	///				the same (case insensitive) as it->first
	virtual bool parse( const ConfigurationNode& cfgTree, const std::string& node,
			    const module::ModuleDirectory* modules ) = 0;

	/// Set the pathes in the configuration to absolute values
	/// \param[in]	refPath	use this path as reference when computing
	///			the absolute pathes
	virtual void setCanonicalPathes( const std::string& /* refPath */ ){}

	/// Check if the server configuration makes sense
	///
	/// Be aware that this function does NOT test if the configuration
	/// can be used. It only tests if it MAY be valid.
	/// This function will log errors / warnings
	/// \return	true if the configuration has no errors, false
	///		otherwise
	virtual bool check() const { return true; }

	/// Print the configuration
	/// This function is supposed to print the running configuration, this means
	/// all the configuration parameters, not only those that were set in the
	/// configuration file.
	/// \param[in]	os	stream to use for printing
	/// \param[in]	indent	print indented with this number of spaces
	virtual void print( std::ostream& os, size_t indent = 0 ) const = 0;
};

}}
#endif

