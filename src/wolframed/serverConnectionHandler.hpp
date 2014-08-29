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
/// \file serverConnectionHandler.hpp

#ifndef _Wolframe_SERVER_CONNECTION_HANDLER_HPP_INCLUDED
#define _Wolframe_SERVER_CONNECTION_HANDLER_HPP_INCLUDED

#include "system/connectionHandler.hpp"
#include "processor/execContext.hpp"
#include "cmdbind/protocolHandler.hpp"
#include "database/database.hpp"
#include "AAAA/AAAAprovider.hpp"
#include "libprovider/procProviderImpl.hpp"
#include "processor/execContext.hpp"

namespace _Wolframe {

class ServerHandler;

/// The connection handler
class ServerConnectionHandler : public net::ConnectionHandler
{
public:
	ServerConnectionHandler( const ServerHandler* context, const net::LocalEndpointR& local );
	~ServerConnectionHandler();

	void setPeer( const net::RemoteEndpointR& remote );

	/// Parse / get the incoming data.
	void networkInput( const void *begin, std::size_t bytesTransferred );

	void signalOccured( NetworkSignal );

	/// Handle a request and produce a reply.
	const net::NetworkOperation nextOperation();

private:
	///\enum States
	///\brief Enumeration of processor states
	enum FSMstate	{
		NEW_CONNECTION,			///<
		SEND_HELLO,			///<
		COMMAND_HANDLER,		//< redirecting to sub state machine of command handler
		TIMEOUT_OCCURED,		///<
		SIGNALLED,			///<
		FORBIDDEN,			///<
		TERMINATING,			///<
		FINISHED			///<
	};

	///\brief Returns the state as string for logging etc.
	///\param [in] state state to get as string
	static const char* stateName( FSMstate state )
	{
		const char* names[] = { "NEW", "HELLO SENT", "COMMAND_HANDLER",
					"TIMEOUT", "SIGNALLED", "FORBIDDEN", "TERMINATE",
					"FINISHED" };
		return names[ state ];
	}

	/// Back link to global context
	const ServerHandler*		m_globalCtx;

	AAAA::Authenticator*		m_authentication;
	AAAA::Authorizer*		m_authorization;
	AAAA::Auditor*			m_audit;

	net::LocalEndpointR		m_localEP;		///< local endpoint
	net::RemoteEndpointR		m_remoteEP;		///< remote endpoint

	FSMstate			m_state;		///< top processor FSM state

	void* m_input;						///< buffer for read messages
	std::size_t m_inputsize;				///< allocation size of m_input in bytes
//	std::size_t m_inputpos;
	void* m_output;						///< buffer for write messages
	std::size_t m_outputsize;				///< allocation size of m_output in bytes

	char*				m_dataStart;
	std::size_t			m_dataSize;

	std::string			m_outMsg;		///< output buffer for initial handshake messages
	std::string			m_endDataSessionMarker;	///< buffer for message to interrupt data session for the client

	proc::ExecContext		m_execContext;		///< execution context of the connection
	cmdbind::ProtocolHandlerR	m_protocolHandler;	///< protocol handler
};

} // namespace _Wolframe
#endif
