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
// wolfpasswd - text file password utility
//

#include <boost/program_options.hpp>
#include <string>
#include <iostream>

#include "getPassword.hpp"
#include "passwdFile.hpp"

namespace PO = boost::program_options;
namespace WA = _Wolframe::AAAA;

int main( int argc, char* argv[] )
{
	PO::options_description desc( "Usage:\n"
				      "  wolfpasswd [-cD] passwordfile username\n"
				      "  wolfpasswd -b[c] passwordfile username password\n"
				      "  wolfpasswd -n username\n"
				      "  wolfpasswd -nb username password\n"
				      "  wolfpasswd -h\n"
				      "Options" );
	desc.add_options()
			( "help,h", "Display this help message." )
			( "create,c", "Create the file if it doesn't exist." )
			( "display-only,n", "Don't update the password file; display results on stdout." )
			( "batch,b", "Use the password from the command line instead of prompting for it.")
			( "delete,D", "Delete the specified user." )
			;

	PO::options_description cmdArgs( "Arguments" );
	cmdArgs.add_options()
			( "posArgs", PO::value< std::vector<std::string> >(), "positional arguments" )
			;
	cmdArgs.add( desc );

	PO::positional_options_description posArgs;
	posArgs.add( "posArgs", -1 );

	PO::variables_map vm;
	try	{
		PO::store( PO::command_line_parser( argc, argv ).
			   options( cmdArgs ).positional( posArgs ).run(), vm );
		PO::notify( vm );
	}
	catch( std::exception& e )	{
		std::cerr << "\nERROR: " << e.what() << "\n\n";
		std::cout << desc << "\n";
		return 2;
	}

	bool	createFile = false;
	bool	displayOnly = false;
	bool	batchPwd = false;
	bool	delUser = false;

	if ( vm.count( "create" ))
		createFile = true;
	if ( vm.count( "display-only" ))
		displayOnly = true;
	if ( vm.count( "batch" ))
		batchPwd = true;
	if ( vm.count( "delete" ))
		delUser = true;

	// deal with help
	if ( vm.count( "help" ))	{
		if ( createFile || displayOnly|| batchPwd || delUser
				|| vm.count( "posArgs" ))
			std::cout << "\nWARNING: --help ignores all other flags and arguments.\n\n";
		std::cout << desc << "\n";
		return 1;
	}

	if ( !vm.count( "posArgs" ))	{
		std::cerr << "\nERROR: no arguments given.\n\n";
		std::cout << desc << "\n";
		return 2;
	}
	const std::vector<std::string>& args = vm["posArgs"].as< std::vector<std::string> >();

	// display only
	if ( displayOnly )	{
		bool	wrong = false;
		if ( createFile )	{
			std::cerr << "\nERROR: --create cannot be used with --display-only.";
			wrong = true;
		}
		if ( delUser )	{
			std::cerr << "\nERROR: --delete cannot be used with --display-only.";
			wrong = true;
		}
		if ( batchPwd )	{
			if ( args.size() < 2 )	{
				std::cerr << "\nERROR: too few arguments.";
				wrong = true;
			}
			else if ( args.size() > 2 )	{
				std::cerr << "\nERROR: too many arguments.";
				wrong = true;
			}
		}
		else	{
			if ( args.size() > 1 )	{
				std::cerr << "\nERROR: too many arguments.";
				wrong = true;
			}
		}
		if ( wrong )	{
			std::cout << "\n\n" << desc << "\n";
			return 2;
		}

		// All parameters are OK
		std::string passwd;
		if ( ! batchPwd )
			passwd = WA::getPassword();
		else
			passwd = args[1];
		std::cout << "Display only user '" << args[0] << "' with password '"
			  << passwd << "'";
		// do the job
	}
	// delete user
	else if ( delUser )	{
		bool	wrong = false;
		if ( createFile )	{
			std::cerr << "\nERROR: --create cannot be used with --delete.";
			wrong = true;
		}
		if ( args.size() < 2 )	{
			std::cerr << "\nERROR: too few arguments.";
			wrong = true;
		}
		else if ( args.size() > 2 )	{
			std::cerr << "\nERROR: too many arguments.";
			wrong = true;
		}
		if ( wrong )	{
			std::cout << "\n\n" << desc << "\n";
			return 2;
		}

		// All parameters are OK
		WA::PasswordFile pwdFile( args[0] );
		try	{
			if ( pwdFile.delUser( args[1] ))
				std::cout << "User '" << args[1] << "' removed from password file '"
					  << args[0] << "'";
			else
				std::cout << "User '" << args[1] << "' not found in password file '"
					  << args[0] << "'";
		}
		catch( std::exception& e )	{
			std::cerr << "Error removing user '" << args[1] << "' from password file '"
				  << args[0] << ": " << e.what();
			return 1;
		}
	}
	// normal operation
	else	{
		bool	wrong = false;
		if ( batchPwd )	{
			if ( args.size() < 3 )	{
				std::cerr << "\nERROR: too few arguments.";
				wrong = true;
			}
			else if ( args.size() > 3 )	{
				std::cerr << "\nERROR: too many arguments.";
				wrong = true;
			}
		}
		else	{
			if ( args.size() < 2 )	{
				std::cerr << "\nERROR: too few arguments.";
				wrong = true;
			}
			if ( args.size() > 2 )	{
				std::cerr << "\nERROR: too many arguments.";
				wrong = true;
			}
		}
		if ( wrong )	{
			std::cout << "\n\n" << desc << "\n";
			return 2;
		}

		// Al parameters are OK
		if ( createFile )
			std::cout << "Create password file '" << args[0] << "'\n";
		std::string passwd;
		if ( ! batchPwd )
			passwd = WA::getPassword();
		else
			passwd = args[2];

		std::cout << "Change / add user '" << args[1] << "', password file '"
			  << args[0] << "', password '" << passwd << "'";
		// do the job
	}
	std::cout << "\nDone.\n\n";
	return 0;
}
