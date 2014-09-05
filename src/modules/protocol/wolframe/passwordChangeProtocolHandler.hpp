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
/// \file passwordChangeProtocolHandler.hpp
/// \brief Interface of the password change command handler

#ifndef _Wolframe_PASSWORD_CHANGE_PROTOCOL_HANDLER_HPP_INCLUDED
#define _Wolframe_PASSWORD_CHANGE_PROTOCOL_HANDLER_HPP_INCLUDED

#include "baseCryptoProtocolHandler.hpp"
#include "aaaa/passwordChanger.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace cmdbind {

/// \class PasswordChangeProtocolHandler
/// \brief Command handler for the sub protocol for password change
class PasswordChangeProtocolHandler
	:public cmdbind::BaseCryptoProtocolHandler
{
public:
	explicit PasswordChangeProtocolHandler( const boost::shared_ptr<aaaa::PasswordChanger>& passwordChanger_);
	virtual ~PasswordChangeProtocolHandler();

private:
	/// \brief See ProtocolHandler::nextOperation()
	virtual Operation nextOperation();
	/// \brief See BaseCryptoProtocolHandler::processMessage(const std::string&)
	virtual void processMessage( const std::string& msg);

private:
	boost::shared_ptr<aaaa::PasswordChanger> m_passwordChanger;	///< password changer object reference
};

}} //namespace

#endif

