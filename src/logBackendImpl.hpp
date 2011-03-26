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
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/

///
/// \file logBackend.hpp
/// \brief header file for the logging backend implementation
///

#ifndef _LOG_BACKEND_IMPL_HPP_INCLUDED
#define _LOG_BACKEND_IMPL_HPP_INCLUDED

#include "appConfig.hpp"

#include "logBackendConsole.hpp"
#include "logBackendFile.hpp"

#include <string>
#include <fstream>
#include <sstream>

#if defined( _WIN32 )
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#endif // defined( _WIN32 )

namespace _Wolframe {
	namespace Logging {

#ifndef _WIN32
	class SyslogBackend
	{
	public:
		SyslogBackend( );

		~SyslogBackend( );

		void setLevel( const LogLevel::Level level );

		void setFacility( const SyslogFacility::Facility facility );

		void setIdent( const std::string ident );

		void log( const LogComponent component, const LogLevel::Level level, const std::string& msg );

		void reopen( );

	private:
		LogLevel::Level logLevel_;
		int facility_;
		std::string ident_;
	};
#endif // _WIN32

#ifdef _WIN32
	class EventlogBackend
	{
	public:
		EventlogBackend( );

		~EventlogBackend( );

		void setLevel( const LogLevel::Level level );

		void setLog( const std::string log );

		void setSource( const std::string source );

		void log( const LogComponent component, const LogLevel::Level level, const std::string& msg );

		void reopen( );

	private:
		LogLevel::Level logLevel_;
		HANDLE eventSource_;
		std::string log_;
		std::string source_;
		PSID sid_;

		void calculateSid( );		
	};
#endif // _WIN32

	class LogBackend::LogBackendImpl
	{
	public:
		LogBackendImpl( );

		~LogBackendImpl( );

		void setConsoleLevel( const LogLevel::Level level );
		
		void setLogfileLevel( const LogLevel::Level level );
		
		void setLogfileName( const std::string filename );

#ifndef _WIN32
		void setSyslogLevel( const LogLevel::Level level );

		void setSyslogFacility( const SyslogFacility::Facility facility );

		void setSyslogIdent( const std::string ident );
#endif // _WIN32

#ifdef _WIN32
		void setEventlogLevel( const LogLevel::Level level );

		void setEventlogLog( const std::string log );

		void setEventlogSource( const std::string source );
#endif // _WIN32

		void log( const LogComponent component, const LogLevel::Level level, const std::string& msg );

	private:
		ConsoleLogBackend consoleLogger_;
		LogfileBackend logfileLogger_;
#ifndef _WIN32
		SyslogBackend syslogLogger_;
#endif // _WIN32
#ifdef _WIN32
		EventlogBackend eventlogLogger_;
#endif // _WIN32
	};

	} // namespace Logging
} // namespace _Wolframe

#endif // _LOG_BACKEND_IMPL_HPP_INCLUDED
