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
///\file wolfilter/src/employee_assignment_print.hpp
///\brief Interface wolfilter test plugin function
#ifndef _Wolframe_TESTS_WOLFILTER_employee_assignment_print_HPP_INCLUDED
#define _Wolframe_TESTS_WOLFILTER_employee_assignment_print_HPP_INCLUDED
#include "serialize/struct/filtermapDescription.hpp"
#include <string>


namespace _Wolframe {
namespace test {

struct Task
{
	std::string title;
	std::string key;
	int customernumber;

	static const serialize::StructDescriptionBase* getStructDescription();
};

struct Employee
{
	std::string firstname;
	std::string surname;
	std::string phone;

	static const serialize::StructDescriptionBase* getStructDescription();
};

struct Assignment
{
	std::vector<Task> task;
	Employee employee;
	std::string issuedate;

	static const serialize::StructDescriptionBase* getStructDescription();
};

struct AssignmentList
{
	std::vector<Assignment> assignment;

	static const serialize::StructDescriptionBase* getStructDescription();
};

struct AssignmentListDoc
{
	AssignmentList assignmentlist;

	static const serialize::StructDescriptionBase* getStructDescription();
	static int convert( AssignmentListDoc& res, const AssignmentListDoc& param);
};

int convertAssignmentListDoc( void* res, const void* param);

}}
#endif

