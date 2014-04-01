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
//\file vm/tdlTranslatorInterface.cpp
//\brief Implementation of the helpers for building a virtual machine for database transactions out of TDL
#include "vm/tdlTranslatorInterface.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::vm;

TdlTranslatorInterface::TdlTranslatorInterface( const types::keymap<Subroutine>* soubroutinemap_)
	:m_soubroutinemap(soubroutinemap_)
{
	m_sub_program.code
		( Op_GOTO_ABSOLUTE, 0 )		//... jump to start of main
	;
}

TdlTranslatorInterface::TdlTranslatorInterface( const TdlTranslatorInterface& o)
	:m_stateStack(o.m_stateStack)
	,m_soubroutinemap(o.m_soubroutinemap)
	,m_calledSubroutines(o.m_calledSubroutines)
	,m_sub_program(o.m_sub_program)
	,m_main_program(o.m_main_program)
{}

void TdlTranslatorInterface::begin_FOREACH( const std::string& selector)
{
	InstructionSet::ArgumentIndex idx;
	if (0!=(idx=m_main_program.resultnametab.getIndex( selector)))
	{
		m_main_program.code
			( Op_OPEN_ITER_KEPT_RESULT, idx )	// iterate on result named
			( Co_IF_COND, Op_GOTO_ABSOLUTE, 0)	// goto end of block if set empty
		;
	}
	else if (boost::algorithm::iequals( selector, "RESULT"))
	{
		//... selector is referencing the last result
		// Code generated:
		m_main_program.code
			( Op_OPEN_ITER_LAST_RESULT )		// iterate on last result
			( Co_IF_COND, Op_GOTO_ABSOLUTE, 0)	// goto end of block if set empty
		;
	}
	else if (boost::algorithm::iequals( selector, "PARAM"))
	{
		throw std::runtime_error("PARAM not allowed as argument of FOREACH");
	}
	else
	{
		//... selector is referencing a path expression on the input
		idx = m_main_program.pathset.add( selector);

		// Code generated:
		m_main_program.code
			( Op_OPEN_ITER_PATH, idx)		// iterate on input path
			( Co_IF_COND, Op_GOTO_ABSOLUTE, 0)	// goto end of block if set empty
		;
	}
	m_stateStack.push_back( State( State::OpenForeach, m_main_program.code.size()));
}

void TdlTranslatorInterface::end_FOREACH()
{
	if (m_stateStack.empty() || m_stateStack.back().id != State::OpenForeach) throw std::runtime_error( "illegal state: end of FOREACH");

	Instruction& forwardJumpInstr = m_main_program.code[ m_stateStack.back().value-1];
	if (forwardJumpInstr != instruction( Co_IF_COND, Op_GOTO_ABSOLUTE, 0))
	{
		throw std::runtime_error( "illegal state: forward patch reference not pointing to instruction expected");
	}
	// Code generated:
	m_main_program.code
		( Op_NEXT )
		( Co_IF_COND, Op_GOTO_ABSOLUTE, m_stateStack.back().value)
	;
	// Patch forward jump (if iterator set empty):
	forwardJumpInstr = InstructionSet::instruction( Co_IF_COND, Op_GOTO_ABSOLUTE, m_main_program.code.size());
	m_stateStack.pop_back();
}

void TdlTranslatorInterface::begin_DO_statement( const std::string& stm)
{
	// Code generated:
	m_main_program.code
		( Op_STM_START, m_main_program.statements.size() )
	;
	m_stateStack.push_back( State( State::OpenStatementCall, m_main_program.statements.size()));
	m_main_program.statements.push_back( stm);
}

void TdlTranslatorInterface::statement_HINT( const std::string& errorclass, const std::string& message)
{
	if (m_stateStack.empty()) throw std::runtime_error( "illegal state: HINT without statement");
	if (m_stateStack.back().id == State::OpenStatementCall)
	{
		InstructionSet::ArgumentIndex hintIdx = m_main_program.hinttab.startdef();
		m_stateStack.push_back( State( State::StatementHint, hintIdx));

		m_main_program.code
			( Op_STM_HINT, hintIdx )
		;
	}
	if (m_stateStack.back().id != State::StatementHint)
	{
		throw std::runtime_error( "illegal state: HINT without open statement");
	}
	m_main_program.hinttab.define( errorclass, message);
}

void TdlTranslatorInterface::end_DO_statement()
{
	if (m_stateStack.empty() || m_stateStack.back().id != State::OpenStatementCall) throw std::runtime_error( "illegal state: end of DO statement");
	// Code generated:
	m_main_program.code
		( Op_STM_EXEC, m_stateStack.back().value )
	;
	m_stateStack.pop_back();
}

void TdlTranslatorInterface::begin_INTO_block( const std::string& tag)
{
	// Code generated:
	m_main_program.code
		( Op_PRINT_OPEN, m_main_program.tagnametab.get( tag))
	;
	m_stateStack.push_back( State( State::OpenIntoBlock, 0));
}

void TdlTranslatorInterface::end_INTO_block()
{
	if (m_stateStack.empty() || m_stateStack.back().id != State::OpenIntoBlock) throw std::runtime_error( "illegal state: end of INTO");
	m_stateStack.pop_back();
}

static std::string mangledSubroutineName( const std::string& name, const std::vector<std::string>& templateParamValues)
{
	if (templateParamValues.empty())
	{
		return boost::to_lower_copy( name);
	}
	std::string rt = boost::to_lower_copy( name);
	std::vector<std::string>::const_iterator ti = templateParamValues.begin(), te = templateParamValues.end();
	for (;ti != te; ++ti)
	{
		rt.push_back( '$');
		rt.append( boost::to_upper_copy( *ti));
	}
	return rt;
}

void TdlTranslatorInterface::begin_DO_subroutine( const std::string& name, const std::vector<std::string>& templateParamValues)
{
	types::keymap<Subroutine>::const_iterator si = m_soubroutinemap->find( name), se = m_soubroutinemap->end();
	if (si == se)
	{
		if (templateParamValues.empty())
		{
			throw std::runtime_error( std::string("calling undefined subroutine '") + name + "'");
		}
		else
		{
			throw std::runtime_error( std::string("calling undefined subroutine template '") + name + "'");
		}
	}
	else
	{
		InstructionSet::ArgumentIndex subroutineIdx = 0;
		InstructionSet::ArgumentIndex frameIdx = 0;
		
		std::string mangledName = mangledSubroutineName( name, templateParamValues);
		std::vector<CalledSubroutineDef>::const_iterator ci = m_calledSubroutines.begin(), ce = m_calledSubroutines.end();
		for (; ci != ce && mangledName != ci->mangledName; ++ci,++subroutineIdx){}
		if (ci == ce)
		{
			subroutineIdx = m_calledSubroutines.size();
			CalledSubroutineDef sr( m_sub_program.code.size(), si->second, mangledName);
			if (sr.subroutine.templateParams().size() != templateParamValues.size())
			{
				throw std::runtime_error( std::string("calling subroutine template '") + name + "' with wrong number of template parameters");
			}
			if (templateParamValues.size())
			{
				sr.subroutine.substituteStatementTemplates( templateParamValues);
			}
			m_calledSubroutines.push_back( sr);
			m_sub_program.add( *si->second.program());
		}
		frameIdx = m_main_program.signatures.size();
		m_main_program.signatures.push_back( m_calledSubroutines.at(subroutineIdx).subroutine.params());

		// Code generated:
		m_main_program.code
			( Op_SUB_FRAME_OPEN, frameIdx )
		;
		m_stateStack.push_back( State( State::OpenSubroutineCall, subroutineIdx));
	}
}

void TdlTranslatorInterface::end_DO_subroutine()
{
	if (m_stateStack.empty() || m_stateStack.back().id != State::OpenSubroutineCall) throw std::runtime_error( "illegal state: end of DO");
	InstructionSet::ArgumentIndex subroutineIdx = m_stateStack.back().value;
	InstructionSet::ArgumentIndex address = m_calledSubroutines.at( subroutineIdx).address;
	std::size_t nofParams = m_calledSubroutines.at(subroutineIdx).subroutine.params().size();
	if (nofParams != m_stateStack.back().cnt)
	{
		throw std::runtime_error( "number of parameters in subroutine call does not match");
	}
	// Code generated:
	m_main_program.code
		( Op_SUB_FRAME_CLOSE )
		( Op_GOTO_ABSOLUTE, address )
	;
	m_stateStack.pop_back();
}

void TdlTranslatorInterface::push_ARGUMENT_PATH( const std::string& selector)
{
	if (m_stateStack.empty()) throw std::runtime_error( "illegal state: push paramter without context");
	if (m_stateStack.back().id == State::StatementHint)
	{
		m_stateStack.pop_back();
	}

	InstructionSet::ArgumentIndex idx = m_main_program.pathset.add( selector);
	if (m_stateStack.back().id == State::OpenSubroutineCall)
	{
		// Code generated:
		m_main_program.code
			( Op_SUB_ARG_PATH, idx )			// push element adressed by relative input path
		;
	}
	else if (m_stateStack.back().id == State::OpenStatementCall)
	{
		// Code generated:
		m_main_program.code
			( Op_STM_BIND_PATH, idx )			// push element adressed by relative input path
		;
	}
	else
	{
		throw std::runtime_error( "illegal state: expected statement or subroutine call context for push paramter");
	}
}

void TdlTranslatorInterface::push_ARGUMENT_CONST( const types::Variant& value)
{
	if (m_stateStack.empty()) throw std::runtime_error( "illegal state: push paramter without context");
	++m_stateStack.back().cnt;
	if (m_stateStack.back().id == State::StatementHint)
	{
		m_stateStack.pop_back();
	}

	InstructionSet::ArgumentIndex idx = m_main_program.constants.size();
	m_main_program.constants.push_back( value);
	if (m_stateStack.back().id == State::OpenSubroutineCall)
	{
		// Code generated:
		m_main_program.code
			( Op_SUB_ARG_CONST, idx )			// push constant element
		;
	}
	else if (m_stateStack.back().id == State::OpenStatementCall)
	{
		// Code generated:
		m_main_program.code
			( Op_STM_BIND_CONST, idx )			// push constant element
		;
	}
	else
	{
		throw std::runtime_error( "illegal state: expected statement or subroutine call context for push/bind paramter");
	}
}

void TdlTranslatorInterface::push_ARGUMENT_TUPLESET( const std::string& setname, unsigned int colidx)
{
	if (m_stateStack.empty()) throw std::runtime_error( "illegal state: push paramter without context");
	++m_stateStack.back().cnt;
	if (m_stateStack.back().id == State::StatementHint)
	{
		m_stateStack.pop_back();
	}
	if (m_stateStack.back().id == State::OpenSubroutineCall)
	{
		InstructionSet::ArgumentIndex idx;
		if (0!=(idx=m_main_program.resultnametab.getIndex( setname)))
		{
			m_main_program.code
				( Op_SELECT_KEPT_RESULT, idx )		// select named result
				( Op_SUB_ARG_SEL_IDX, colidx)		// push column by index
			;
		}
		else if (boost::algorithm::iequals( setname, "RESULT"))
		{
			//... selector is referencing the last result
			// Code generated:
			m_main_program.code
				( Op_SELECT_LAST_RESULT )		// select last result
				( Op_SUB_ARG_SEL_IDX, colidx)		// push column by index
			;
		}
		else if (boost::algorithm::iequals( setname, "PARAM"))
		{
			//... selector is referencing the last result
			// Code generated:
			m_main_program.code
				( Op_SELECT_PARAMETER )			// select parameter
				( Op_SUB_ARG_SEL_IDX, colidx)		// push column by index
			;
		}
		else
		{
			throw std::runtime_error( std::string( "selecting unknown name of set '") + setname + "'");
		}
	}
	else if (m_stateStack.back().id == State::OpenStatementCall)
	{
		InstructionSet::ArgumentIndex idx;
		if (0!=(idx=m_main_program.resultnametab.getIndex( setname)))
		{
			m_main_program.code
				( Op_SELECT_KEPT_RESULT, idx )		// select named result
				( Op_STM_BIND_SEL_IDX, colidx)		// bind column by index
			;
		}
		else if (boost::algorithm::iequals( setname, "RESULT"))
		{
			//... selector is referencing the last result
			// Code generated:
			m_main_program.code
				( Op_SELECT_LAST_RESULT )		// select last result
				( Op_STM_BIND_SEL_IDX, colidx)		// bind column by index
			;
		}
		else if (boost::algorithm::iequals( setname, "PARAM"))
		{
			//... selector is referencing the last result
			// Code generated:
			m_main_program.code
				( Op_SELECT_PARAMETER )			// select parameter
				( Op_STM_BIND_SEL_IDX, colidx)		// bind column by index
			;
		}
		else
		{
			throw std::runtime_error( std::string( "selecting unknown name of set '") + setname + "'");
		}
	}
	else
	{
		throw std::runtime_error( "illegal state: expected statement or subroutine call context for push/bind paramter");
	}
}

void TdlTranslatorInterface::push_ARGUMENT_TUPLESET( const std::string& setname, const std::string& colname)
{
	if (m_stateStack.empty()) throw std::runtime_error( "illegal state: push paramter without context");
	++m_stateStack.back().cnt;
	InstructionSet::ArgumentIndex colnameidx = m_main_program.colnametab.get( colname);

	if (m_stateStack.back().id == State::OpenSubroutineCall)
	{
		InstructionSet::ArgumentIndex idx;
		if (0!=(idx=m_main_program.resultnametab.getIndex( setname)))
		{
			m_main_program.code
				( Op_SELECT_KEPT_RESULT, idx )		// select named result
				( Op_SUB_ARG_SEL_NAM, colnameidx)	// push column by name
			;
		}
		else if (boost::algorithm::iequals( setname, "RESULT"))
		{
			//... selector is referencing the last result
			// Code generated:
			m_main_program.code
				( Op_SELECT_LAST_RESULT )		// select last result
				( Op_SUB_ARG_SEL_NAM, colnameidx)	// push column by name
			;
		}
		else if (boost::algorithm::iequals( setname, "PARAM"))
		{
			//... selector is referencing the last result
			// Code generated:
			m_main_program.code
				( Op_SELECT_PARAMETER )			// select parameter
				( Op_SUB_ARG_SEL_NAM, colnameidx)	// push column by name
			;
		}
		else
		{
			throw std::runtime_error( std::string( "selecting unknown name of set '") + setname + "'");
		}
	}
	else if (m_stateStack.back().id == State::OpenStatementCall)
	{
		InstructionSet::ArgumentIndex idx;
		if (0!=(idx=m_main_program.resultnametab.getIndex( setname)))
		{
			m_main_program.code
				( Op_SELECT_KEPT_RESULT, idx )		// select named result
				( Op_STM_BIND_SEL_NAM, colnameidx)	// bind column by name
			;
		}
		else if (boost::algorithm::iequals( setname, "RESULT"))
		{
			//... selector is referencing the last result
			// Code generated:
			m_main_program.code
				( Op_SELECT_LAST_RESULT )		// select last result
				( Op_STM_BIND_SEL_NAM, colnameidx)	// bind column by name
			;
		}
		else if (boost::algorithm::iequals( setname, "PARAM"))
		{
			//... selector is referencing the last result
			// Code generated:
			m_main_program.code
				( Op_SELECT_PARAMETER )			// select parameter
				( Op_STM_BIND_SEL_NAM, colnameidx)	// bind column by name
			;
		}
		else
		{
			throw std::runtime_error( std::string( "selecting unknown name of set '") + setname + "'");
		}
	}
	else
	{
		throw std::runtime_error( "illegal state: expected statement or subroutine call context for push/bind paramter");
	}
}

void TdlTranslatorInterface::print_ARGUMENT_PATH( const std::string& selector)
{
	if (m_stateStack.empty()) throw std::runtime_error( "illegal state: push paramter without context");

	InstructionSet::ArgumentIndex idx = m_main_program.pathset.add( selector);
	// Code generated:
	m_main_program.code
		( Op_PRINT_PATH, idx )				// print element adressed by relative input path
	;
}

void TdlTranslatorInterface::print_ARGUMENT_CONST( const types::Variant& value)
{
	if (m_stateStack.empty()) throw std::runtime_error( "illegal state: push paramter without context");

	InstructionSet::ArgumentIndex idx = m_main_program.constants.size();
	m_main_program.constants.push_back( value);
	// Code generated:
	m_main_program.code
		( Op_PRINT_CONST, idx )				// print element adressed by relative input path
	;
}

void TdlTranslatorInterface::print_ARGUMENT_TUPLESET( const std::string& setname, unsigned int colidx)
{
	InstructionSet::ArgumentIndex idx;
	if (0!=(idx=m_main_program.resultnametab.getIndex( setname)))
	{
		m_main_program.code
			( Op_SELECT_KEPT_RESULT, idx )		// select named result
			( Op_PRINT_SEL_IDX, colidx)		// print column by index
		;
	}
	else if (boost::algorithm::iequals( setname, "RESULT"))
	{
		//... selector is referencing the last result
		// Code generated:
		m_main_program.code
			( Op_SELECT_LAST_RESULT )		// select last result
			( Op_PRINT_SEL_IDX, colidx)		// print column by index
		;
	}
	else if (boost::algorithm::iequals( setname, "PARAM"))
	{
		//... selector is referencing the last result
		// Code generated:
		m_main_program.code
			( Op_SELECT_PARAMETER )			// select parameter
			( Op_PRINT_SEL_IDX, colidx)		// print column by index
		;
	}
	else
	{
		throw std::runtime_error( std::string( "selecting unknown name of set '") + setname + "'");
	}
}

void TdlTranslatorInterface::print_ARGUMENT_TUPLESET( const std::string& setname, const std::string& colname)
{
	InstructionSet::ArgumentIndex colnameidx = m_main_program.colnametab.get( colname);
	InstructionSet::ArgumentIndex idx;
	if (0!=(idx=m_main_program.resultnametab.getIndex( setname)))
	{
		m_main_program.code
			( Op_SELECT_KEPT_RESULT, idx )		// select named result
			( Op_PRINT_SEL_NAM, colnameidx)		// print column by name
		;
	}
	else if (boost::algorithm::iequals( setname, "RESULT"))
	{
		//... selector is referencing the last result
		// Code generated:
		m_main_program.code
			( Op_SELECT_LAST_RESULT )		// select last result
			( Op_PRINT_SEL_NAM, colnameidx)		// print column by name
		;
	}
	else if (boost::algorithm::iequals( setname, "PARAM"))
	{
		//... selector is referencing the last result
		// Code generated:
		m_main_program.code
			( Op_SELECT_PARAMETER )			// select parameter
			( Op_PRINT_SEL_NAM, colnameidx)		// print column by name
		;
	}
	else
	{
		throw std::runtime_error( std::string( "selecting unknown name of set '") + setname + "'");
	}
}


ProgramR TdlTranslatorInterface::createProgram() const
{
	ProgramR rt( new Program( m_sub_program));

	// Patch first instruction of the program, a GOTO that jumps to the start of main
	std::size_t ip_start_main = m_sub_program.code.size();
	if (rt->code.size() == 0 || *rt->code.at(0) != instruction( Op_GOTO_ABSOLUTE, 0))
	{
		throw std::runtime_error( "illegal state: jump to main instruction not found");
	}
	rt->code[ 0] = InstructionSet::instruction( Op_GOTO_ABSOLUTE, ip_start_main);

	rt->add( m_main_program, false/*do not patch GOTOs*/);
	std::size_t ip_end_main = rt->code.size();

	//Patch GOTO instructions that are not subroutine calls.
	//	Subroutine calls are already set correctly:
	OpCode lastOpCode = Op_NOP;
	for (std::size_t ip=ip_start_main; ip<ip_end_main; ++ip)
	{
		Instruction instr = *rt->code.at( ip);
		OpCode oc = opCode( instr);
		CondCode cc = condCode( instr);
		ArgumentType at = argumentType( oc);
		ArgumentIndex ai = argumentIndex( instr);
		if (at == At_Address && lastOpCode != Op_SUB_FRAME_CLOSE)
		{
			rt->code[ ip] = instruction( cc, oc, ai + ip_start_main);
		}
	}
	return rt;
}

