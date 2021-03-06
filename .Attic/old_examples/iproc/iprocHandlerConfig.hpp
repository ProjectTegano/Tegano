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
///\file iprocHandlerConfig.hpp
///\brief iproc handler configuration
#ifndef _Wolframe_IPROC_HANDLER_CONFIGURATION_HPP_INCLUDED
#define _Wolframe_IPROC_HANDLER_CONFIGURATION_HPP_INCLUDED
#include <vector>
#include <string>
#include "config/configurationObject.hpp"
#include "standardConfigs.hpp"

namespace _Wolframe {
namespace iproc {

///\class Configuration
///\brief Configuration description of the 'iproc' example
class Configuration :public config::ConfigurationObject
{
public:
	struct Command
	{
		std::string m_cmdname;
		std::string m_procname;

		Command( const Command& o)				:m_cmdname(o.m_cmdname),m_procname(o.m_procname){}
		Command( const std::string& c, const std::string& p)	:m_cmdname(c),m_procname(p){}
		Command( const std::string& c)				:m_cmdname(c),m_procname(c){}
	};
public:
	Configuration()
		:config::ConfigurationObject( "iproc", 0, "iproc")
		,m_input_bufsize(1024)
		,m_output_bufsize(1024) {}

	Configuration( const std::vector<Command>& cc, std::size_t ib, std::size_t ob)
		:config::ConfigurationObject("iproc", 0, "iproc")
		,m_commands(cc)
		,m_input_bufsize(ib)
		,m_output_bufsize(ob){}

	Configuration( const Configuration& o)
		:config::ConfigurationObject(o)
		,m_commands(o.m_commands)
		,m_input_bufsize(o.m_input_bufsize)
		,m_output_bufsize(o.m_output_bufsize){}

	bool parse( const config::ConfigurationNode& pt, const std::string& node, const module::ModuleDirectory* modules );

	///\brief interface implementation of ConfigurationObject::test() const
	virtual bool test() const;

	///\brief interface implementation of ConfigurationObject::check() const
	virtual bool check() const;

	///\brief interface implementation of ConfigurationObject::print(std::ostream& os, size_t indent) const
	virtual void print( std::ostream&, size_t indent=0) const;

	///\brief return size of the buffer used for input network messages in bytes
	std::size_t input_bufsize() const		{return m_input_bufsize;}
	///\brief return size of the buffer used for output network messages in bytes
	std::size_t output_bufsize() const		{return m_output_bufsize;}

	///\brief return all currently available commands
	const std::vector<Command>& commands() const	{return m_commands;}

private:
	std::vector<Command> m_commands;		//< command definitions
	std::size_t m_input_bufsize;			//< size of input network message buffers in bytes (not configured, but hardcoded by the messaging layer)
	std::size_t m_output_bufsize;			//< size of output network message buffers in bytes (not configured, but hardcoded by the messaging layer)

public:
	///\brief Set the buffer sizes for tests
	///\remark Should only be called in tests or by the core
	///\param[in] ib size of input buffer in bytes
	///\param[in] ob size of output buffer in bytes
	void setBuffers( std::size_t ib, std::size_t ob)
	{
		m_input_bufsize = ib;
		m_output_bufsize = ob;
	}
};
}}//namespace
#endif


