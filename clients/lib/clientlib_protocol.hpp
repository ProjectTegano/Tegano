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
///\file clientlib_protocol.hpp
///\brief C client library interface

#ifndef _WOLFRAME_CLIENTLIB_PROTOCOL_HPP_INCLUDED
#define _WOLFRAME_CLIENTLIB_PROTOCOL_HPP_INCLUDED
#include <string>
#include <cstdlib>


namespace _Wolframe {
namespace client {

///\defgroup protocol
///\brief Interface to the client protocol handling.
// Provides an abstraction of the wolframe client protocol statemachine
// to process complete data items.
///@{

///\brief Protocol handler
class Protocol
{
public:
	///\struct Event
	///\brief Protocl event structure
	struct Event
	{
		///\struct Type
		///\brief Enumeration of protocol event types
		enum Type
		{
			SEND=	1,		//< data with messages from the protocol to be sent to the server
			UIFORM=	2,		//< UI form sent from server to client in the initialization phase
			ANSWER=	3,		//< answer of a requset from the server to the client in a session
			STATE=	4,		//< selected state info for the client
			REQERR=	5,		//< error in request (negative answer)
			ERROR=	6		//< error reported by the server
		};

		Event( Type t, const char* i, const char* c, std::size_t s)
			:m_type(t),m_id(i),m_content(c),m_contentsize(s){}
		Event( const Event& o)
			:m_type(o.m_type),m_id(o.m_id),m_content(o.m_content),m_contentsize(o.m_contentsize){}

		Type type() const			{return m_type;}
		const char* id() const			{return m_id;}
		const char* content() const		{return m_content;}
		std::size_t contentsize() const		{return m_contentsize;}

	private:
		Type m_type;			//< type of the protocol event
		const char* m_id;		//< identifier of the protocol event
		const char* m_content;		//< data of the protocol event
		std::size_t m_contentsize;	//< size of the protocol event data in bytes
	};

	typedef void (*Callback)( void* clientobject, const Protocol::Event& event);

	///\brief Constructor
	///\param[in] notifier_ callback for event notification
	///\param[in] clientobject_ first parameter of notifier_
	Protocol( Callback notifier_, void* clientobject_);
	virtual ~Protocol();

	///\brief Push data to be processe received from the server
	///\param[in] data pointer to data to push (decrypted plain data)
	///\param[in] datasize size of data in bytes
	void pushData( const char* data, std::size_t datasize);

	///\brief Push a request that will be served when the session is established
	///\param[in] data request data (decrypted plain data)
	///\param[in] datasize size of data in bytes
	void pushRequest( const char* data, std::size_t datasize);

	bool isOpen() const;

	void doQuit();

	typedef enum
	{
		CALL_DATA,	//< the protocol event handler is requesting more data. all input (complete lines have been consumed)
		CALL_IDLE,	//< the protocol event handler is in an established session state and has processed all requests until now.
		CALL_ERROR,	//< an internal error (memory allocation error) has occurred
		CALL_CLOSED	//< the client has to terminate becaues it cannot work anymore or the server will close the connection
	} CallResult;

	///\brief Call the protocol handler state machine
	CallResult run();

private:
	Protocol( const Protocol&){}	//non copyable

	///\class Impl
	///\brief PIMPL for internal protocol handler data structure
	struct Impl;
	Impl* m_impl;
};

///@} end group protocol
}}//namespace
#endif

