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
/// \file serverEndpoint.hpp
/// \brief Server endpoint classes

#ifndef _SERVER_ENDPOINT_HPP_INCLUDED
#define _SERVER_ENDPOINT_HPP_INCLUDED

#include <string>
#include "system/connectionEndpoint.hpp"
#include "types/addressRestriction.hpp"

namespace _Wolframe	{
namespace net	{

	/// No encryption server endpoint
	class ServerTCPendpoint : public LocalTCPendpoint
	{
	public:
		ServerTCPendpoint( const std::string& Host, unsigned short Port,
				   unsigned short maxConn,
				   const LocalEndpointConfig& Config,
				   const types::AddressRestriction& AddressRestriction)
			: LocalTCPendpoint( Host, Port, Config ),
			  m_maxConn( maxConn ),
			  m_addressRestriction( AddressRestriction )		{}

		unsigned short maxConnections() const				{ return m_maxConn; }
		const types::AddressRestriction& addressRestriction() const	{ return m_addressRestriction; }

	private:
		const std::string	m_identifier;
		const unsigned short	m_maxConn;
		types::AddressRestriction	m_addressRestriction;
	};


#ifdef WITH_SSL
	/// SSL connection server endpoint
	class ServerSSLendpoint : public LocalSSLendpoint
	{
		friend class server;
	public:
		ServerSSLendpoint( const std::string& Host, unsigned short Port,
				   unsigned short maxConn,
				   const LocalEndpointConfig& Config,
				   const types::AddressRestriction& addressRestriction_,
				   const std::string& Certificate, const std::string& Key,
				   bool verify, const std::string& CAdir, const std::string& CAchainFile )
			: LocalSSLendpoint( Host, Port, Config ),
			  m_maxConn( maxConn ),
			  m_addressRestriction( addressRestriction_ )
		{
			m_cert = Certificate;
			m_key = Key;
			m_verify = verify;
			m_CAdir = CAdir;
			m_CAchain = CAchainFile;
		}

		const types::AddressRestriction& addressRestriction() const	{ return m_addressRestriction; }
		unsigned short maxConnections() const	{ return m_maxConn; }
		const std::string& certificate() const	{ return m_cert; }
		const std::string& key() const		{ return m_key; }
		const std::string& CAdirectory() const	{ return m_CAdir; }
		const std::string& CAchain() const	{ return m_CAchain; }
		bool verifyClientCert() const		{ return m_verify; }

		void setAbsolutePath( const std::string& referencePath );

	private:
		const std::string	m_identifier;
		const unsigned short	m_maxConn;
		types::AddressRestriction	m_addressRestriction;
		std::string		m_cert;
		std::string		m_key;
		std::string		m_CAdir;
		std::string		m_CAchain;
		bool			m_verify;
	};
#endif // WITH_SSL

}} // namespace _Wolframe::net

#endif // _SERVER_ENDPOINT_HPP_INCLUDED
