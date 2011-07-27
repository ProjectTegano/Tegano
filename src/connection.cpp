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
//
// connection.cpp
//

#include "connectionBase.hpp"
#include "connection.hpp"
#include "logger.hpp"
#include "connectionHandler.hpp"

#ifdef WITH_SSL
#include "SSLcertificateInfo.hpp"
#endif

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

namespace _Wolframe {
namespace net {

static const char* REFUSE_MSG = "Server is busy. Please try again later.\n";


void GlobalConnectionList::addList( SocketConnectionList< connection_ptr >* lst )
{
	m_connList.push_back( lst );
	LOG_DATA << "Added unencrypted connection list, " << m_connList.size() << " list(s) for unencrypted connections";
}

#ifdef WITH_SSL
void GlobalConnectionList::addList( SocketConnectionList< SSLconnection_ptr >* lst )
{
	m_SSLconnList.push_back( lst );
	LOG_DATA << "Added SSL connection list, " << m_SSLconnList.size() << " list(s) for SSL connections";
}
#endif // WITH_SSL

bool GlobalConnectionList::isFull()
{
	std::size_t conns = 0;

	for ( std::list< SocketConnectionList< connection_ptr >* >::iterator it = m_connList.begin();
	      it != m_connList.end(); it++ )
		conns += (*it)->size();
#ifdef WITH_SSL
	for ( std::list< SocketConnectionList< SSLconnection_ptr >* >::iterator it = m_SSLconnList.begin();
	      it != m_SSLconnList.end(); it++ )
		conns += (*it)->size();
#endif // WITH_SSL
	if ( m_maxConn > 0 )	{
		LOG_DATA << "Global number of connections: " << conns << " of maximum " << m_maxConn;
		return( conns >= m_maxConn );
	}
	else	{
		LOG_DATA << "Global number of connections: " << conns << ", no maximum limit";
		return( false );
	}
}


connection::connection( boost::asio::io_service& IOservice,
			SocketConnectionList< connection_ptr >& connList,
			ConnectionHandler* handler ) :
	ConnectionBase< boost::asio::ip::tcp::socket >( IOservice, handler ),
	m_socket( IOservice ),
	m_connList( connList )
{
	LOG_TRACE << "New connection created";
}

connection::~connection()
{
	if ( m_ID.empty() )
		LOG_TRACE << "Connection (unconnected) destroyed";
	else
		LOG_TRACE << "Connection to " << m_ID <<" destroyed";
}


void connection::start()
{
	identifier( std::string( socket().remote_endpoint().address().to_string())
		    + ":" + boost::lexical_cast<std::string>( socket().remote_endpoint().port() ));
	LOG_TRACE << "Starting connection to " << identifier();

	// if the maximum number of connections has been reached refuse the connection
	if ( m_connList.isFull() )	{
		LOG_TRACE << "Refusing connection from " << identifier() << ". Too many connections.";
		boost::system::error_code ignored_ec;
		socket().write_some( boost::asio::buffer( REFUSE_MSG, strlen( REFUSE_MSG ) ));
		socket().lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
	}
	else	{
		m_connList.push( boost::static_pointer_cast< connection >( shared_from_this()));

		m_connHandler->setPeer( RemoteTCPendpoint( socket().remote_endpoint().address().to_string(),
							   socket().remote_endpoint().port()));
		nextOperation();
	}
}


#ifdef WITH_SSL

SSLconnection::SSLconnection( boost::asio::io_service& IOservice,
			      boost::asio::ssl::context& SSLcontext,
			      SocketConnectionList< SSLconnection_ptr >& connList,
			      ConnectionHandler *handler ) :
	ConnectionBase< ssl_socket >( IOservice, handler ),
	m_SSLsocket( IOservice, SSLcontext ),
	m_connList( connList )
{
	LOG_TRACE << "New SSL connection created";
}

SSLconnection::~SSLconnection()
{
	if ( m_ID.empty() )
		LOG_TRACE << "SSL connection (unconnected) destroyed";
	else
		LOG_TRACE << "Connection to " << m_ID <<" destroyed";
}


void SSLconnection::start()
{
	identifier( std::string( m_SSLsocket.lowest_layer().remote_endpoint().address().to_string())
		    + ":" + boost::lexical_cast<std::string>( m_SSLsocket.lowest_layer().remote_endpoint().port() )
		    + " (SSL)");
	LOG_TRACE << "Starting connection to " << identifier();

	m_SSLsocket.async_handshake( boost::asio::ssl::stream_base::server,
				     m_strand.wrap( boost::bind( &SSLconnection::handleHandshake,
								 boost::static_pointer_cast< SSLconnection >( shared_from_this() ),
								 boost::asio::placeholders::error )));
}


void SSLconnection::handleHandshake( const boost::system::error_code& e )
{
	if ( !e )	{
		LOG_DATA << "successful SSL handshake, peer " << identifier();

		// if the maximum number of connections has been reached refuse the connection
		if ( m_connList.isFull() )	{
			LOG_TRACE << "Refusing connection from " << identifier() << ". Too many connections.";
			boost::system::error_code ignored_ec;
			socket().write_some( boost::asio::buffer( REFUSE_MSG, strlen( REFUSE_MSG ) ));
			socket().lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
		}
		else	{
			SSL* ssl = m_SSLsocket.impl()->ssl;
			X509* peerCert = SSL_get_peer_certificate( ssl );
			SSLcertificateInfo* certInfo = NULL;

			if ( peerCert )	{
				certInfo = new SSLcertificateInfo( peerCert );
			}
			m_connHandler->setPeer( RemoteSSLendpoint( m_SSLsocket.lowest_layer().remote_endpoint().address().to_string(),
								   m_SSLsocket.lowest_layer().remote_endpoint().port(),
								   certInfo ));

			m_connList.push( boost::static_pointer_cast< SSLconnection >( shared_from_this()) );

			nextOperation();
		}
	}
	else	{
		LOG_DEBUG << e.message() << ", SSL handshake, peer " << identifier();
//		delete this;
		boost::system::error_code ignored_ec;
		socket().lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
	}
}

#endif // WITH_SSL

}} // namespace _Wolframe::net
