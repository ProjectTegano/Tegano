/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
///\file wolfilterCommandLine.hpp
///\brief Interface for the wolfilter call command line
#ifndef _Wolframe_WOLFILTER_COMMANDLINE_HPP_INCLUDED
#define _Wolframe_WOLFILTER_COMMANDLINE_HPP_INCLUDED
#include <string>
#include <vector>
#include <iostream>
#include "moduleInterface.hpp"
#include "processor/procProvider.hpp"
#include "langbind/appConfig_struct.hpp"
#include "langbind/appConfig_option.hpp"

namespace _Wolframe {
namespace config {

class WolfilterCommandLine
{
public:
	WolfilterCommandLine( int argc, char **argv, const std::string& referencePath);
	~WolfilterCommandLine(){}

	bool printhelp() const						{return m_printhelp;}
	bool printversion() const					{return m_printversion;}
	const std::string& inputfile() const				{return m_inputfile;}
	const std::vector<std::string>& modules() const			{return m_modules;}
	const langbind::EnvironmentConfigStruct& envconfig() const	{return m_envconfig;}
	const std::string& cmd() const					{return m_cmd;}
	const std::string& inputfilter() const				{return m_inputfilter;}
	const std::string& outputfilter() const				{return m_outputfilter;}
	std::size_t inbufsize() const					{return m_inbufsize;}
	std::size_t outbufsize() const					{return m_outbufsize;}
	const proc::ProcProviderConfig& providerConfig() const		{return m_providerConfig;}
	const module::ModulesDirectory& modulesDirectory() const	{return m_modulesDirectory;}

	void print(std::ostream &) const;

	///\brief loads the command line objects into the global context
	void loadGlobalContext() const;

private:
	bool m_printhelp;
	bool m_printversion;
	std::string m_inputfile;
	std::vector<std::string> m_modules;
	langbind::EnvironmentConfigStruct m_envconfig;
	std::string m_cmd;
	std::string m_inputfilter;
	std::string m_outputfilter;
	std::string m_helpstring;
	std::size_t m_inbufsize;
	std::size_t m_outbufsize;
	proc::ProcProviderConfig m_providerConfig;
	module::ModulesDirectory m_modulesDirectory;
	std::string m_referencePath;
};

}}//namespace
#endif

