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
///\file contentOnlyProtocolHandler.hpp

#ifndef _Wolframe_CONTENT_ONLY_PROTOCOL_HANDLER_HPP_INCLUDED
#define _Wolframe_CONTENT_ONLY_PROTOCOL_HANDLER_HPP_INCLUDED
#include "cmdbind/protocolHandler.hpp"
#include "cmdbind/doctypeDetector.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace cmdbind {

class ContentOnlyProtocolHandler
	:public ProtocolHandler
{
public:
	typedef cmdbind::ProtocolHandler Parent;

	ContentOnlyProtocolHandler();
	virtual ~ContentOnlyProtocolHandler(){}

	void setPeer( const net::RemoteEndpointR& remote);
	void setLocalEndPoint( const net::LocalEndpointR& local);

	virtual void setInputBuffer( void* buf, std::size_t allocsize);

	virtual void setOutputBuffer( void* buf, std::size_t size, std::size_t pos);

	virtual Operation nextOperation();

	virtual void putInput( const void* chunk, std::size_t chunksize);
	virtual void putEOF();

	virtual void getInputBlock( void*& begin, std::size_t& maxBlockSize);

	virtual void getOutput( const void*& begin, std::size_t& bytesToTransfer);

	virtual void getDataLeft( const void*& begin, std::size_t& nofBytes);

	virtual void setArgumentString( const std::string&);

private:
	enum State
	{
		Init,
		DoctypeDetection,
		InitProcessing,
		Processing,
		FlushingOutput,
		DiscardInput,
		Done
	};
	static const char* stateName( State s)
	{
		static const char* ar[] = {"Init","DoctypeDetection","InitProcessing","Processing","FlushingOutput","DiscardInput","Done"};
		return ar[s];
	}
	net::RemoteEndpointR m_remoteEndpoint;		///< remote end point
	net::LocalEndpointR m_localEndpoint;		///< connection endpoint
	cmdbind::DoctypeDetectorR m_doctypeDetector;	///< document type detection
	std::string m_buffer;				///< buffer for input during document type detection
	std::size_t m_bufferpos;			///< content processed position in m_buffer 
	std::string m_cmdname;				///< name of command to execute -> command handler
	cmdbind::CommandHandlerR m_commandHandler;	///< command handler for processing
	char* m_input;					///< buffer for read messages
	std::size_t m_inputsize;			///< allocation size of m_input in bytes
	char* m_output;					///< buffer for write messages
	std::size_t m_outputsize;			///< allocation size of m_output in bytes
	std::size_t m_outputpos;			///< position in m_output (chunk before belongs to caller)
	const char* m_writeblock;			///< current block written
	std::size_t m_writesize;			///< allocation size of m_writeblock
	std::size_t m_writepos;				///< write position in m_writeblock
	types::DoctypeInfoR m_doctypeInfo;		///< type/format attributes of processed document
	State m_state;					///< processing state
	bool m_eod;					///< got end of data marker
};


/// \class ContentOnlyProtocolHandlerUnit
/// \brief Class that defines the Wolframe standard protocol handler 
class ContentOnlyProtocolHandlerUnit
	:public ProtocolHandlerUnit
{
public:
	/// \brief Get the name of the Wolframe standard protocol handler
	virtual const char* protocol() const
	{
		return "wolframe";
	}

	/// \brief Create an instance of the Wolframe standard protocol handler
	virtual ProtocolHandler* createProtocolHandler()
	{
		return new ContentOnlyProtocolHandler();
	}
};

}}//namespace
#endif
