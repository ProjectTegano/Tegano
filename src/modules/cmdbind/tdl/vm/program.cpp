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
//\file vm/program.cpp
//\brief Implementation of the virtual machine program
#include "vm/program.hpp"
#include "vm/patchArgumentMap.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::vm;

template <class Element>
static PatchArgumentMapR joinVectors( std::vector<Element>& dst, const std::vector<Element>& ar)
{
	std::size_t ofs = dst.size();
	dst.insert( dst.end(), ar.begin(), ar.end());
	return PatchArgumentMapR( new PatchArgumentMap_Offset( ofs));
}

static PatchArgumentMapR joinProgramCode( ProgramCode& ths, const ProgramCode& oth)
{
	std::size_t ofs = ths.size();
	ths.append( oth);
	return PatchArgumentMapR( new PatchArgumentMap_Offset( ofs));
}

static void patchCode( ProgramCode& code, InstructionSet::ArgumentType argtype, std::size_t startidx, const PatchArgumentMap& pamap)
{
	if (startidx > code.size())
	{
		throw std::logic_error("array bound access");
	}
	if (startidx == code.size())
	{
		return;
	}
	ProgramCode::iterator ci = code.begin() + startidx, ce = code.end();
	for (; ci != ce; ++ci)
	{
		InstructionSet::OpCode oc = InstructionSet::opCode( *ci);
		if (argtype == InstructionSet::argumentType( oc))
		{
			InstructionSet::CondCode cc = InstructionSet::condCode( *ci);
			InstructionSet::ArgumentIndex ai = InstructionSet::argumentIndex( *ci);
			*ci = InstructionSet::instruction( cc, oc, pamap[ ai]);
		}
	}
}

void vm::Program::add( const vm::Program& oth, bool doPatchGOTOs)
{
	std::size_t startidx = code.size();

	// Patch GOT0 addresses if required (doPatchGOTOs == true):
	PatchArgumentMapR pmap;
	if (doPatchGOTOs)
	{
		pmap = joinProgramCode( code, oth.code);
		patchCode( code, At_Address, startidx, *pmap);
	}
	else
	{
		code.append( oth.code);
	}

	// Update variant constant references:
	pmap = joinVectors( constants, oth.constants);
	patchCode( code, At_Constant, startidx, *pmap);

	// Update tag name references:
	pmap = tagnametab.join( oth.tagnametab);
	patchCode( code, At_TagName, startidx, *pmap);

	// Update result name references:
	pmap = resultnametab.join( oth.resultnametab);
	patchCode( code, At_ResultName, startidx, *pmap);

	// Update column name references:
	pmap = colnametab.join( oth.colnametab);
	patchCode( code, At_ColumnName, startidx, *pmap);

	// Update statement references:
	pmap = joinVectors( statements, oth.statements);
	patchCode( code, At_Statement, startidx, *pmap);
	
	// Update error hint references:
	pmap = hinttab.join( oth.hinttab);
	patchCode( code, At_Hint, startidx, *pmap);

	// Update signature references:
	pmap = joinVectors( signatures, oth.signatures);
	patchCode( code, At_SubroutineSignature, startidx, *pmap);

	// Update tuple set references:
	pmap = joinVectors( tuplesets, oth.tuplesets);
	patchCode( code, At_TupleSet, startidx, *pmap);

	// Update path references:
	pmap = pathset.join( oth.pathset);
	patchCode( code, At_Path, startidx, *pmap);

	// Update TDL position references:
	std::size_t startidx_tdlpos = tdlpositions.size();
	tdlpositions.insert( tdlpositions.end(), oth.tdlpositions.begin(), oth.tdlpositions.end());
	std::vector<InstructionPos>::iterator di = tdlpositions.begin() + startidx_tdlpos, de = tdlpositions.end();
	for (; di != de; ++di)
	{
		di->ip += startidx;
	}
}

void vm::Program::setCurrentSourceReference( const utils::FileLineInfo& posinfo)
{
	tdlpositions.push_back( InstructionPos( code.size(), posinfo));
}

bool vm::Program::getSourceReference( std::size_t ip, utils::FileLineInfo& posinfo) const
{
	std::vector<InstructionPos>::const_iterator prev = tdlpositions.begin(), di = tdlpositions.begin(), de = tdlpositions.end();
	for (; di != de && ip >= di->ip; prev=di++) {}
	if (di == tdlpositions.begin()) return false;
	posinfo = prev->pos;
	return true;
}

std::string vm::Program::instructionString( const Instruction& instr) const
{
	std::ostringstream out;
	printInstruction( out, instr);
	return out.str();
}

std::string vm::Program::instructionStringAt( const std::size_t& ip) const
{
	std::ostringstream out;
	printInstructionAt( out, ip);
	return out.str();
}

void vm::Program::printInstructionAt( std::ostream& out, const std::size_t& ip) const
{
	printInstruction( out, *code.at( ip));
}

void vm::Program::printInstruction( std::ostream& out, const Instruction& instr) const
{
	CondCode cc = condCode( instr);
	const char* ccnam = condCodeName( cc);
	OpCode oc = opCode( instr);
	ArgumentType at = argumentType( oc);
	ArgumentIndex ai = argumentIndex( instr);
	if (ccnam[0])
	{
		out << ccnam << " ";

	}
	out << opCodeName( oc);
	switch (at)
	{
		case At_None:
			break;
		case At_Address:
			out << " @" << ai;
			break;
		case At_Path:
			out << " PATH ";
			pathset.getPath( ai).print( out, pathset.tagtab());
			break;
		case At_Constant:
			if (ai >= constants.size()) throw std::runtime_error("array bound read");
			out << " CONST \'" << constants.at(ai).tostring() << '\'';
			break;
		case At_ColumnName:
			out << " COLUMN " << colnametab.getName( ai);
			break;
		case At_TagName:
			out << " TAG " << tagnametab.getName( ai);
			break;
		case At_ResultName:
			out << " RESULT " << resultnametab.getName( ai);
			break;
		case At_Statement:
			if (ai >= statements.size()) throw std::runtime_error("array bound read");
			out << " STM (" << statements.at( ai) << ")";
			break;
		case At_Hint:
			out << " ERROR HINTS " << ai;
			break;
		case At_SubroutineSignature:
			out << " SIGNATURE " << ai;
			break;
		case At_TupleSet:
			out << " TUPLESET " << ai;
			break;
		case At_SelectedColumnIdx:
			out << " COLIDX " << ai;
			break;
		case At_IteratorColumnIdx:
			out << " COLIDX " << ai;
			break;
	}
}

void vm::Program::print( std::ostream& out) const
{
	out << "Code:" << std::endl;
	std::vector<Instruction>::const_iterator pi = code.begin(), pe = code.end();
	unsigned int adr = 0;
	for (; pi != pe; ++pi,++adr)
	{
		out << "[" << adr << "] ";
		printInstruction( out, *pi);
		out << std::endl;
	}

	std::vector<ValueTupleSetR>::const_iterator vi = tuplesets.begin(), ve = tuplesets.end();
	if (vi != ve)
	{
		out << "Input Data:" << std::endl;
		for (std::size_t idx=0; vi != ve; ++vi,++idx)
		{
			out << "SET " << idx << ": ";
			std::size_t ii=1,nn = (*vi)->nofColumns();
			for (; ii<=nn;++ii)
			{
				if (ii>1) out << ", ";
				std::string colnam = (*vi)->columnName( ii);
				if (colnam.empty())
				{
					out << "<no name>";
				}
				else
				{
					out << colnam;
				}
			}
			out << std::endl;
			ValueTupleSet::const_iterator si = (*vi)->begin(), se = (*vi)->end();
			for (; si != se; ++si)
			{
				out << "  ";
				for (std::size_t colidx=1; colidx <= nn; ++colidx)
				{
					if (colidx>1) out << ", ";
					const types::Variant& colval = si->column(colidx);
					if (colval.defined())
					{
						out << "'" << si->column(colidx).tostring() << "'";
					}
					else
					{
						out << "NULL";
					}
				}
				out << std::endl;
			}
		}
	}
	std::vector<SubroutineSignature>::const_iterator si = signatures.begin(), se = signatures.end();
	if (si != se)
	{
		out << "Subroutine Signatures:" << std::endl;
		for (std::size_t idx=0; si != se; ++si,++idx)
		{
			out << "SUB " << idx << " ( ";
			SubroutineSignature::const_iterator gi = si->begin(), ge = si->end();
			for (std::size_t paramidx=0; gi != ge; ++gi,++paramidx)
			{
				if (paramidx > 0) out << ", ";
				out << *gi;
			}
			out << " )" << std::endl;
		}
	}

	std::vector<ErrorHintTable::HintList>::const_iterator hi = hinttab.begin(), he = hinttab.end();
	if (hi != he)
	{
		out << "Error Hints:" << std::endl;
		for (std::size_t idx=0; hi != he; ++hi,++idx)
		{
			out << "HINT " << idx << ":" << std::endl;
			ErrorHintTable::HintList::const_iterator ei = hi->begin(), ee = hi->end();
			for (; ei != ee; ++ei)
			{
				out << "  " << ei->errorclass << ": " << ei->message << std::endl;
			}
			out << std::endl;
		}
	}
}

ProgramImage vm::Program::image() const
{
	ProgramImage rt;
	{
		rt.code = code;
		rt.constants = constants;
		rt.colnames = colnametab.array();
		rt.tagnames = tagnametab.array();
		rt.resultnames = resultnametab.array();
		rt.statements = statements;
		ErrorHintTable::const_iterator hi = hinttab.begin(), he = hinttab.end();
		for (; hi != he; ++hi)
		{
			rt.errorhints.push_back( ProgramImage::ErrorHintList());
	
			ErrorHintTable::HintList::const_iterator ei = hi->begin(), ee = hi->end();
			for (; ei != ee; ++ei)
			{
				rt.errorhints.back().push_back( ProgramImage::ErrorHint( ei->errorclass, ei->message));
			}
		}
		rt.signatures = signatures;
		rt.tuplesets = tuplesets;
	}
	return rt;
}

