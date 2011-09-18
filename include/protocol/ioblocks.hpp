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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
#ifndef _Wolframe_PROTOCOL_IO_BLOCKS_HPP_INCLUDED
#define _Wolframe_PROTOCOL_IO_BLOCKS_HPP_INCLUDED
///\file protocol/ioblocks.hpp
///\brief Network message blocks as seen from the protocol with the input iterator classes and print functions

#include "iterators.hpp"
#include "buffers.hpp"
#include <stdexcept>
#include <cstring>
#include <cstddef>

namespace _Wolframe {
namespace protocol {

///
///\class MemBlock
///\brief Memory block base structure for input/output network messages of the protocol
///
class MemBlock
{
public:
	//element typedefs
	typedef char value_type;		///< basic STL vector typedefs

	///\brief Constructor
	MemBlock();
	///\brief Constructor
	///\param [in] p_size size of the memory block to allocate
	MemBlock( std::size_t p_size);
	///\param [in] p_ptr pointer to the memory block (not allocated on your own)
	///\param [in] p_size size of the memory block p_ptr
	MemBlock( void* p_ptr, std::size_t p_size);
	///\brief Copy constructor
	///\param [in] o MemBlock to copy
	MemBlock( const MemBlock& o);
	///\brief Destructor
	~MemBlock();

	///\brief Copy assignement operator
	///\param [in] o MemBlock to copy
	MemBlock& operator=( const MemBlock& o);

	///\brief Set the current cursor byte position
	///\param [in] p_pos new cursor position in bytes from the memory block start
	void setPos( std::size_t p_pos=0)				{m_pos = p_pos;}

	///\brief Set the buffer to be used by this class (as non allocated by this class)
	///\param [in] p_ptr new buffer to use
	///\param [in] p_size size of buffer to use
	void set( void* p_ptr, std::size_t p_size);

	///\brief Constant void pointer to start of buffer
	void* ptr()						{return m_ptr;}

	///\brief Constant void pointer to start of buffer
	const void* ptr() const					{return m_ptr;}

	///\brief Constant char pointer to start of buffer
	char* charptr()						{return (char*)m_ptr;}

	///\brief Constant char pointer to start of buffer
	const char* charptr() const				{return (const char*)m_ptr;}

	///\brief Allocation size of the buffer in bytes
	std::size_t size() const				{return m_size;}

	///\brief Get the size of the buffer left
	///\return the size of the buffer left
	std::size_t restsize() const				{return (m_pos<m_size)?(m_size-m_pos):0;}

	///\brief Current byte position of the cursor (input or output)
	std::size_t pos() const					{return m_pos;}

	///\brief Shift current cursor poition by some bytes
	///\param [in] n number of bytes to shift
	void incr( std::size_t n)				{if ((m_pos+n)>=m_size) m_pos=m_size; else m_pos+=n;}

	///\exception ArrayBoundReadError
	///\brief Protocol memory block access violation exception
	struct ArrayBoundReadError				:public std::logic_error {ArrayBoundReadError():std::logic_error("ABR"){}};

	///\brief random access operator on const
	///\param [in] idx index of element to access
	char operator[]( std::size_t idx) const			{if (idx>=m_pos) throw ArrayBoundReadError(); return charptr()[idx];}

	///\brief random access operator on reference type
	///\param [in] idx index of element to access
	char& operator[]( std::size_t idx)			{if (idx>=m_pos) throw ArrayBoundReadError(); return charptr()[idx];}

private:
	void* m_ptr;		///< pointer to buffer
	std::size_t m_size;	///< allocation size in bytes of this buffer
	std::size_t m_pos;	///< current byte position of the cursor
	bool m_allocated;	///< true, if the memory of the buffer \c m_ptr is owned by this class and has to be freed again in the destructor
};



///
///\class InputBlock
///\brief Input memory block of the protocol for network input with iterators for the processing of the data
///
class InputBlock :public MemBlock
{
public:
	///
	///\class EoD
	///\brief States of the automaton recognizing end of data and unescaping escape sequences in data
	///
	/// See method getEoD(iterator)
	///
	/// The end of data is marked with a sequence "\n.\r" or "\n.\n".
	/// If such a sequence appears by accident in the content it has to be escaped with a dot as "\n..\r" or "\n..\r".
	/// The additional dot after the end of line escapes any other character sequence too.
	/// As consequence every line starting with a dot has to be escaped.
	///
	struct EoD
	{
		///\enum State
		///\brief enumeration of states for handling eoD
		enum State
		{
			SRC,			///< parsing content
			CR,			///< detected an CarriageReturn in state SRC, belonging either to a CRLF or a LF sequence
			CR_LF,			///< detected an LineFeed in state SRC or CR, belonging either to a CRLF or a LF sequence
			CR_LF_DOT,		///< detected a '.' (dot) in state CR_LF
			CR_LF_DOT_CR,		///< detected a CarriageReturn CR in state CR_LF_DOT. Here we assume that end of data is reached
			CR_LF_DOT_CR_LF		///< detected a LineFeed LF in state CR_LF_DOT or CR_LF_DOT_CR. Here we assume at last that end of data is reached
		};
	};

	///\brief Constructor
	InputBlock()						:m_eodState(EoD::SRC){}

	///\brief Constructor
	///\param [in] p_size size of the memory block in bytes to allocate
	InputBlock( std::size_t p_size)				:MemBlock(p_size),m_eodState(EoD::SRC){}

	///\brief Constructor
	///\param [in] p_ptr pointer to the memory block to use
	///\param [in] p_size size of the memory block in bytes
	InputBlock( void* p_ptr, std::size_t p_size)		:MemBlock(p_ptr,p_size),m_eodState(EoD::SRC){}

	///\brief Copy constructor
	///\param [in] o InputBlock to copy
	InputBlock( const InputBlock& o)			:MemBlock(o),m_eodState(o.m_eodState){}

	InputBlock& operator=( const InputBlock& o)		{MemBlock::operator=(o); m_eodState=o.m_eodState; return *this;}

	///\brief Random access const iterator
	typedef array::iterator_t<const InputBlock,std::size_t,char,char,const char*> const_iterator;

	///\brief Random access iterator
	typedef array::iterator_t<InputBlock,std::size_t,char,char&,char*> iterator;

	///\brief Get the starting const iterator
	const_iterator begin() const				{const_iterator rt(this); return rt;}

	///\brief Get the starting iterator
	iterator begin()					{iterator rt(this); return rt;}

	///\brief Random access to a const iterator
	///\param [in] pos_ position from where to get the iterator
	const_iterator at( std::size_t pos_) const		{const_iterator rt(this); return rt+pos_;}

	///\brief Random access to an iterator
	///\param [in] pos_ position from where to get the iterator
	iterator at( std::size_t pos_)				{iterator rt(this); return rt+pos_;}

	///\brief Get the end of block const iterator
	const_iterator end() const				{return const_iterator(this)+pos();}

	///\brief Get the end of block iterator
	iterator end()						{return iterator(this)+pos();}

	///\brief End of data calculation and unescaping of LF_DOT sequences
	///\param [in] start where to start searching for the end of data marker
	iterator getEoD( iterator start)
	{
		int eodpos = getEoDpos( start-begin());
		return (eodpos>=0)?(start+eodpos):end();
	}

	///\brief Reset end of data state machine
	void resetEoD()						{m_eodState=EoD::SRC;}

	///\brief Return true if end of data was recognized with the last call of getEoD(iterator)
	bool gotEoD() const					{return m_eodState>=EoD::CR_LF_DOT_CR_LF;}

	///\brief Get the network message pointer and size for network input considering the EoD state
	///\param[out] pp pointer to buffer
	///\param[out] ppsize size of buffer in bytes
	///\return false, if the buffer is too small to recognize EoD properly
	bool getNetworkMessageRead( void*& pp, std::size_t& ppsize)
	{
		if (size() <= m_eodcharbuf.size()) return false;
		std::memcpy( ptr(), m_eodcharbuf.ptr(), m_eodcharbuf.size());
		setPos( m_eodcharbuf.size());
		pp = charptr() + m_eodcharbuf.size();
		ppsize = size() - m_eodcharbuf.size();
		m_eodcharbuf.clear();
		return true;
	}

private:
	///\brief Implementation of the end of data recognition and linefeed,dot escaping state machine
	int getEoDpos( std::size_t offset);

	///\brief State of end of data recognition
	EoD::State m_eodState;

	///\brief Buffer for unprocessed EoD characters
	Buffer<8> m_eodcharbuf;
};




///\class OutputBlock
///\brief Protocol output buffer
///
/// Print as buffer is available and then order to "ship" what you printed.
///
class OutputBlock :public MemBlock
{
public:
	///\brief Constructor
	///\param [in] p_size size of the memory block in bytes to allocate
	OutputBlock( std::size_t p_size=0)			:MemBlock(p_size) {}
	///\brief Constructor
	///\param [in] p_ptr pointer to the memory block to use
	///\param [in] p_size size of the memory block in bytes
	OutputBlock( void* p_ptr, std::size_t p_size)		:MemBlock(p_ptr,p_size) {}
	///\brief Copy constructor
	///\param [in] o OutputBlock to copy
	OutputBlock( const OutputBlock& o)			:MemBlock(o) {}
	OutputBlock& operator=( const OutputBlock& o)		{MemBlock::operator=(o); return *this;}

	///\brief Return true if the buffer is empty
	bool empty() const
	{
		return (pos()==0);
	}

	///\brief Print one character to the output
	///\param [in] ch the character to print
	bool print( char ch)
	{
		if (pos() == size()) return false;
		charptr()[ pos()] = ch;
		setPos( pos() + 1);
		return true;
	}

	///\brief Print one character to the output
	///\param [in] ch the character to print
	bool print( const void* p, std::size_t n)
	{
		if (n >= restsize()) return false;
		std::memmove( charptr()+pos(), p, n);
		setPos( pos() + n);
		return true;
	}

	///\brief Skip the output buffer cursor by some bytes
	///\param [in] nn the number of bytes to skip
	bool incPos( std::size_t nn)
	{
		if (pos()+nn > size()) return false;
		setPos( pos() + nn);
		return true;
	}

	///\brief Returns a const pointer to the rest of the output buffer block
	const char* rest() const				{return charptr() + pos();}
	///\brief Returns a pointer to the rest of the output buffer block
	char* rest()						{return charptr() + pos();}

	///\brief Returns the size of the rest of the output buffer (how many characters can be written)
	std::size_t restsize() const				{return size()-pos();}

	///\brief Release a written memory block (reset cursor position)
	void release()						{setPos(0);}
};

///\class EscapingBuffer
///\brief Back insertion sequence for content data where LF '.' sequences have to be escaped for the prococol
///\tparam Inherited back insertion sequence base
template <class BufferType>
class EscapingBuffer :public BufferType
{
public:
	///\enum State
	///\brief enumeration of states for escaping LF '.' sequences to LF '.' '.'
	enum State
	{
		SRC,			///< parsing content
		LF,			///< detected an LineFeed in state SRC
		LF_DOT			///< LF and additional '.' not printed yet
	};

	///\brief Constructor
	///\param [in] p_ptr pointer to the memory block to use
	///\param [in] p_size size of the memory block in bytes
	EscapingBuffer( char* p_ptr, std::size_t p_size, State p_state)
		:BufferType(p_ptr,p_size),m_state(p_state) {}

	///\brief get the current state of escaping LF DOT
	State state() const
	{
		return m_state;
	}

	///\brief redirect to BufferType::push_back(char) with escaping of LF DOT as LF DOT DOT
	void push_back( char ch)
	{
		BufferType::push_back( ch);
		if (ch == '\n')
		{
			m_state = LF;
		}
		else if (ch == '.')
		{
			if (m_state == LF)
			{
				m_state = LF_DOT;
				BufferType::push_back( ch);

			}
			m_state = SRC;
		}
		else
		{
			m_state = SRC;
		}
	}
private:
	State m_state;		///< the current state of escaping LF DOT as LF DOT DOT
};

} // namespace protocol
} // namespace _Wolframe

#endif // _Wolframe_PROTOCOL_IO_BLOCKS_HPP_INCLUDED

