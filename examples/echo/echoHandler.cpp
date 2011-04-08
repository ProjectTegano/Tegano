//
// connectionHandler.cpp
//

#include "echoHandler.hpp"
#include "logger.hpp"
#include "SSLcertificateInfo.hpp"

#include <string>
#include <cstring>

#include "boost/date_time/posix_time/posix_time.hpp"		// to print time_t structures

namespace _Wolframe {

echoConnection::echoConnection( const net::LocalEndpoint& local, unsigned short timeout )
{
	_Wolframe::net::ConnectionEndpoint::ConnectionType type = local.type();

	switch ( type )	{
	case _Wolframe::net::ConnectionEndpoint::TCP_CONNECTION:	{
		const _Wolframe::net::LocalTCPendpoint& lcl = static_cast<const _Wolframe::net::LocalTCPendpoint&>( local );
		LOG_TRACE << "Created connection handler for " << lcl.toString();
		break;
	}
#ifdef WITH_SSL
	case _Wolframe::net::ConnectionEndpoint::SSL_CONNECTION:	{
		const _Wolframe::net::LocalSSLendpoint& lcl = static_cast<const _Wolframe::net::LocalSSLendpoint&>( local );
		LOG_TRACE << "Created connection handler (SSL) for " << lcl.toString();
		break;
	}
#else
	case _Wolframe::net::ConnectionEndpoint::SSL_CONNECTION:
#endif // WITH_SSL
	default:
		LOG_FATAL << "Impossible local connection type !";
		abort();
	}


	state_ = NEW;
	dataStart_ = NULL;
	dataSize_ = 0;
	idleTimeout_ = timeout;
}


echoConnection::~echoConnection()
{
	LOG_TRACE << "Connection handler destroyed";
}


void echoConnection::setPeer( const net::RemoteEndpoint& remote )
{
	_Wolframe::net::ConnectionEndpoint::ConnectionType type = remote.type();

	switch ( type )	{
	case _Wolframe::net::ConnectionEndpoint::TCP_CONNECTION:	{
		const _Wolframe::net::RemoteTCPendpoint& rmt = static_cast<const _Wolframe::net::RemoteTCPendpoint&>( remote );
		LOG_TRACE << "Peer set to " << rmt.toString() << ", connected at " << rmt.connectionTime();
		break;
	}
#ifdef WITH_SSL
	case _Wolframe::net::ConnectionEndpoint::SSL_CONNECTION:	{
		const _Wolframe::net::RemoteSSLendpoint& rmt = static_cast<const _Wolframe::net::RemoteSSLendpoint&>( remote );
		LOG_TRACE << "Peer set to " << rmt.toString() << ", connected at " << boost::posix_time::from_time_t( rmt.connectionTime());
		if ( rmt.SSLcertInfo() )	{
			LOG_TRACE << "Peer SSL certificate serial number " << rmt.SSLcertInfo()->serialNumber()
				  << ", issued by: " << rmt.SSLcertInfo()->issuer();
			LOG_TRACE << "Peer SSL certificate valid from " << boost::posix_time::from_time_t( rmt.SSLcertInfo()->notBefore())
				  << " to " <<  boost::posix_time::from_time_t( rmt.SSLcertInfo()->notAfter());
			LOG_TRACE << "Peer SSL certificate subject: " << rmt.SSLcertInfo()->subject();
			LOG_TRACE << "Peer SSL certificate Common Name: " << rmt.SSLcertInfo()->commonName();
		}
		break;
	}
#else
	case _Wolframe::net::ConnectionEndpoint::SSL_CONNECTION:
#endif // WITH_SSL
	default:
		LOG_FATAL << "Impossible remote connection type !";
		abort();
	}
}


/// Handle a request and produce a reply.
const net::NetworkOperation echoConnection::nextOperation()
{
	switch( state_ )	{
	case NEW:	{
		state_ = HELLO_SENT;
		return net::NetworkOperation( net::SendString( "Welcome to Wolframe.\n" ));
	}

	case HELLO_SENT:	{
		state_ = READ_INPUT;
		return net::NetworkOperation( net::ReadData( readBuf_, ReadBufSize, idleTimeout_ ));
	}

	case READ_INPUT:
		dataStart_ = readBuf_;
		// Yes, it continues with OUTPUT_MSG, sneaky, sneaky, sneaky :P

	case OUTPUT_MSG:
		if ( !strncmp( "quit", dataStart_, 4 ))	{
			state_ = TERMINATE;
			return net::NetworkOperation( net::SendString( "Thanks for using Wolframe.\n" ));
		}
		else	{
			char *s = dataStart_;
			for ( std::size_t i = 0; i < dataSize_; i++ )	{
				if ( *s == '\n' )	{
					s++;
					outMsg_ = std::string( dataStart_, s - dataStart_ );
					dataSize_ -= s - dataStart_;
					dataStart_ = s;
					state_ = OUTPUT_MSG;
					return net::NetworkOperation( net::SendString( outMsg_ ));
				}
				s++;
			}
			// If we got here, no \n was found, we need to read more
			// or close the connection if the buffer is full
			if ( dataSize_ >= ReadBufSize )	{
				state_ = TERMINATE;
				return net::NetworkOperation( net::SendString( "Line too long. Bye.\n" ));
			}
			else {
				memmove( readBuf_, dataStart_, dataSize_ );
				state_ = READ_INPUT;
				return net::NetworkOperation( net::ReadData( readBuf_ + dataSize_,
									     ReadBufSize - dataSize_,
									     idleTimeout_ ));
			}
		}

	case TIMEOUT:	{
		state_ = TERMINATE;
		return net::NetworkOperation( net::SendString( "Timeout. :P\n" ));
	}

	case SIGNALLED:	{
		state_ = TERMINATE;
		return net::NetworkOperation( net::SendString( "Server is shutting down. :P\n" ));
	}

	case TERMINATE:	{
		state_ = FINISHED;
		return net::NetworkOperation( net::CloseConnection() );
	}

	case FINISHED:
		LOG_DEBUG << "Processor in FINISHED state";
		break;

	} /* switch( state_ ) */
	return net::NetworkOperation( net::CloseConnection() );
}


/// Parse incoming data. The return value indicates how much of the
/// input has been consumed.
void echoConnection::networkInput( const void*, std::size_t bytesTransferred )
{
	dataSize_ += bytesTransferred;
}

void echoConnection::timeoutOccured()
{
	state_ = TIMEOUT;
	LOG_TRACE << "Processor received timeout";
}

void echoConnection::signalOccured()
{
	state_ = SIGNALLED;
	LOG_TRACE << "Processor received signal";
}

void echoConnection::errorOccured( NetworkSignal signal )
{
	switch( signal )	{
	case END_OF_FILE:
		LOG_TRACE << "Processor received EOF (read on closed connection)";
		break;

	case BROKEN_PIPE:
		LOG_TRACE << "Processor received BROKEN PIPE (write on closed connection)";
		break;

	case OPERATION_CANCELLED:
		LOG_TRACE << "Processor received OPERATION_CANCELED (should have been requested by us)";
		break;

	case UNKNOWN_ERROR:
		LOG_TRACE << "Processor received an UNKNOWN error from the framework";
		break;
	}
	state_ = TERMINATE;
}


/// ServerHandler PIMPL
net::connectionHandler* ServerHandler::ServerHandlerImpl::newConnection( const net::LocalEndpoint& local )
{
	return new echoConnection( local, timeout );
}

/// Outside face of the PIMPL
ServerHandler::ServerHandler( const HandlerConfiguration *config ) : impl_( new ServerHandlerImpl( config ) )	{}

ServerHandler::~ServerHandler()	{ delete impl_; }

net::connectionHandler* ServerHandler::newConnection( const net::LocalEndpoint& local )
{
	return impl_->newConnection( local );
}

} // namespace _Wolframe
