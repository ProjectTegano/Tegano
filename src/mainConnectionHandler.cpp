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
///\file mainConnectionHandler.cpp
#include "mainConnectionHandler.hpp"
#include "cmdbind/discardInputCommandHandlerEscDLF.hpp"
#include "cmdbind/authCommandHandler.hpp"
#include "handlerConfig.hpp"
#include "logger-v1.hpp"
#include <stdexcept>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::proc;

enum State
{
	Unauthorized,
	Authorization,
	Authorized
};

struct STM :public cmdbind::LineCommandHandlerSTMTemplate<CommandHandler>
{
	STM()
	{
		(*this)
			[Unauthorized]
				.cmd< &CommandHandler::doAuth >( "AUTH")
				.cmd< &CommandHandler::doQuit >( "QUIT")
				.cmd< &CommandHandler::doCapabilities >( "CAPABILITIES")
			[Authorization]
				.cmd< &CommandHandler::doMech >( "MECH")
				.cmd< &CommandHandler::doQuit >( "QUIT")
				.cmd< &CommandHandler::doCapabilities >( "CAPABILITIES")
			[Authorized]
				.cmd< &CommandHandler::doRequest >( "REQUEST")
				.cmd< &CommandHandler::doInterface >( "INTERFACE")
				.cmd< &CommandHandler::doAuth >( "AUTH")
				.cmd< &CommandHandler::doQuit >( "QUIT")
				.cmd< &CommandHandler::doCapabilities >( "CAPABILITIES")
		;
	}
};
static STM stm;

CommandHandler::CommandHandler()
	:cmdbind::LineCommandHandlerTemplate<CommandHandler>( &stm ){}

static unsigned long getNumber( const char* aa)
{
	unsigned long result = 0;
	for (int ii=0; aa[ii] <= '9' && aa[0] >= '0'; ++ii)
	{
		unsigned long xx = result * 10 + aa[0] - '0';
		if (xx < result) throw std::runtime_error( "number out of range");
		result = xx;
	}
	return result;

}

int CommandHandler::doCapabilities( int argc, const char**, std::ostream& out)
{
	if (argc != 0)
	{
		out << "ERR unexpected arguments" << endl();
		return stateidx();
	}
	else
	{
		out << "OK" << boost::algorithm::join( cmds(), " ") << endl();
		return stateidx();
	}
}

int CommandHandler::doQuit( int argc, const char**, std::ostream& out)
{
	if (argc != 0)
	{
		out << "ERR unexpected arguments" << endl();
		return stateidx();
	}
	else
	{
		out << "BYE" << endl();
		return -1;
	}
}

int CommandHandler::doAuth( int argc, const char**, std::ostream& out)
{
	if (argc != 0)
	{
		out << "ERR AUTH no arguments expected" << endl();
		return stateidx();
	}
	else
	{
		out << "MECHS " << boost::algorithm::join( m_authMechanisms.list(), " ") << "NONE" << endl();
		return Authorization;
	}
}

int CommandHandler::endMech( cmdbind::CommandHandler* ch, std::ostream& out)
{
	cmdbind::AuthCommandHandler* chnd = dynamic_cast<cmdbind::AuthCommandHandler*>( ch);
	cmdbind::CommandHandlerR chr( ch);
	const char* error = ch->lastError();
	if (error)
	{
		out << "ERR authorization " << error << endl();
		return -1;
	}
	else
	{
		out << "OK authorization" << endl();
		m_authtickets.push_back( chnd->ticket());
		return Authorized;
	}
}

int CommandHandler::doMech( int argc, const char** argv, std::ostream& out)
{
	if (argc == 0)
	{
		out << "ERR argument (mechanism identifier) expected for MECH" << endl();
		return stateidx();
	}
	if (argc >= 2)
	{
		out << "ERR to many arguments for MECH" << endl();
		return stateidx();
	}
	if (boost::iequals( std::string(argv[0]), "NONE"))
	{
		out << "OK authorization" << endl();
		m_authtickets.push_back( "none");
		return Authorized;
	}
	cmdbind::AuthCommandHandler* authch = m_authMechanisms.get( argv[0]);
	if (!authch)
	{
		out << "ERR no handler defined for authorization mechanism " << argv[0] << "'" << endl();
		return stateidx();
	}
	authch->setProcProvider( m_provider);
	delegateProcessing<&CommandHandler::endMech>( authch);
	return stateidx();
}

int CommandHandler::doInterface( int argc, const char** argv, std::ostream& out)
{
	int min_version = 0;
	int version = 0;
	if (argc == 1)
	{

		min_version = getNumber( argv[0]);
	}
	typedef std::map<std::string,std::string> UIForms;
	UIForms uiforms = m_provider->uiforms( m_authtickets.back(), min_version, version);
	UIForms::const_iterator fi = uiforms.begin(), fe = uiforms.end();
	out << "INTERFACE " << version << endl();
	for (; fi != fe; ++fi)
	{
		out << "UIFORM " << fi->first << endl();
		out << protocol::escapeStringDLF( fi->second);
		out << endl() << "." << endl();
	}
	out << "OK" << endl();
	return stateidx();
}

int CommandHandler::endRequest( cmdbind::CommandHandler* chnd, std::ostream& out)
{
	cmdbind::CommandHandlerR chr( chnd);
	int rt = stateidx();
	const char* error = chnd->lastError();
	if (error)
	{
		out << "ERR REQUEST " << error << endl();
		LOG_ERROR << "error in execution of REQUEST " << m_command << ":" << (error?error:"unspecified error");
	}
	else
	{
		out << "OK REQUEST " << m_command << endl();
	}
	return rt;
}

bool CommandHandler::redirectConsumedInput( cmdbind::DoctypeFilterCommandHandler* fromh, cmdbind::CommandHandler* toh, std::ostream& out)
{
	void* buf;
	std::size_t bufsize;
	fromh->getInputBuffer( buf, bufsize);
	return Parent::redirectInput( buf, bufsize, toh, out);
}

int CommandHandler::endErrDocumentType( cmdbind::CommandHandler* ch, std::ostream& out)
{
	cmdbind::CommandHandlerR chr( ch);
	const char* err = ch->lastError();
	out << "ERR " << (err?err:"document type")<< LineCommandHandler::endl();
	return stateidx();
}

int CommandHandler::endDoctypeDetection( cmdbind::CommandHandler* ch, std::ostream& out)
{
	cmdbind::DoctypeFilterCommandHandler* chnd = dynamic_cast<cmdbind::DoctypeFilterCommandHandler*>( ch);
	cmdbind::CommandHandlerR chr( ch);
	std::string doctype = chnd->doctypeid();
	const char* error = ch->lastError();
	if (error)
	{
		std::ostringstream msg;
		msg << "failed to retrieve document type (" << error << ")";
		cmdbind::CommandHandler* delegate_ch = (cmdbind::CommandHandler*)new cmdbind::DiscardInputCommandHandlerEscDLF( msg.str());
		if (m_commandtag.empty())
		{
			out << "ANSWER" << endl();
		}
		else
		{
			out << "ANSWER " << '&' << m_commandtag << endl();
		}
		if (redirectConsumedInput( chnd, delegate_ch, out))
		{
			delegateProcessing<&CommandHandler::endErrDocumentType>( delegate_ch);
		}
		else
		{
			out << "ERR doctype detection " << error << endl();
		}
	}
	if (!doctype.empty())
	{
		m_command.append(doctype);
	}
	cmdbind::CommandHandler* execch = m_provider->cmdhandler( m_command);
	if (!execch)
	{
		std::ostringstream msg;
		msg << "no command handler for '" << m_command << "'";
		execch = (cmdbind::CommandHandler*)new cmdbind::DiscardInputCommandHandlerEscDLF( msg.str());
		if (m_commandtag.empty())
		{
			out << "ANSWER" << endl();
		}
		else
		{
			out << "ANSWER " << '&' << m_commandtag << endl();
		}
		if (redirectConsumedInput( chnd, execch, out))
		{
			delegateProcessing<&CommandHandler::endErrDocumentType>( execch);
		}
		else
		{
			out << "ERR " << msg.str() << endl();
			delete execch;
		}
	}
	else
	{
		execch->passParameters( m_command, 0, 0);
		execch->setProcProvider( m_provider);
		if (m_commandtag.empty())
		{
			out << "ANSWER" << endl();
		}
		else
		{
			out << "ANSWER " << '&' << m_commandtag << endl();
		}
		if (redirectConsumedInput( chnd, execch, out))
		{
			delegateProcessing<&CommandHandler::endRequest>( execch);
		}
		else
		{
			if (execch->lastError())
			{
				out << "ERR REQUEST " << m_command << " " << execch->lastError() << endl();
			}
			else
			{
				out << "OK REQUEST " << m_command << endl();
			}
			delete execch;
		}
	}
	return stateidx();
}

int CommandHandler::doRequest( int argc, const char** argv, std::ostream& out)
{
	m_command.clear();
	m_commandtag.clear();
	if (argc)
	{
		bool has_commandtag = false;
		bool has_command = false;
		for (int ii=0; ii<argc; ++ii)
		{
			if (argv[ii][0] == '&')
			{
				if (has_commandtag)
				{
					out << "ERR more than one command tag" << endl();
					return stateidx();
				}
				has_commandtag = true;
				m_commandtag.append( argv[ii]+1);
			}
			else
			{
				if (has_command)
				{
					out << "ERR to many arguments" << endl();
					return stateidx();
				}
				has_command = true;
				m_command.append( argv[ii]);
			}
		}
	}
	CommandHandler* ch = (CommandHandler*)new cmdbind::DoctypeFilterCommandHandler();
	delegateProcessing<&CommandHandler::endDoctypeDetection>( ch);
	return stateidx();
}

void Connection::networkInput( const void* dt, std::size_t nofBytes)
{
	m_cmdhandler.putInput( dt, nofBytes);
}

void Connection::signalOccured( NetworkSignal)
{
	LOG_TRACE << "Got signal";
	m_terminated = true;
}

const net::NetworkOperation Connection::nextOperation()
{
	void* inpp;
	std::size_t inppsize;
	const void* outpp;
	std::size_t outppsize;
	if (m_terminated)
	{
		return net::CloseConnection();
	}
	switch(m_cmdhandler.nextOperation())
	{
		case cmdbind::CommandHandler::READ:
			m_cmdhandler.getInputBlock( inpp, inppsize);
			return net::ReadData( inpp, inppsize);

		case cmdbind::CommandHandler::WRITE:
			m_cmdhandler.getOutput( outpp, outppsize);
			return net::SendData( outpp, outppsize);

		case cmdbind::CommandHandler::CLOSE:
			return net::CloseConnection();
	}
	return net::CloseConnection();
}

Connection::Connection( const net::LocalEndpoint& local)
	:m_cmdhandler()
	,m_terminated(false)
{
	m_cmdhandler.setInputBuffer( m_input.ptr(), m_input.size());
	m_cmdhandler.setOutputBuffer( m_output.ptr(), m_output.size());
	LOG_TRACE << "Created connection handler for " << local.toString();
}

Connection::~Connection()
{
	LOG_TRACE << "Connection handler destroyed";
}

void Connection::setPeer( const net::RemoteEndpoint& remote)
{
	LOG_TRACE << "Peer set to " << remote.toString();
}


