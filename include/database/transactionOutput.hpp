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
///\brief Definition of transaction output
///\file database/transactionOutput.hpp
#ifndef _DATABASE_TRANSACTION_OUTPUT_HPP_INCLUDED
#define _DATABASE_TRANSACTION_OUTPUT_HPP_INCLUDED
#include <string>
#include <vector>
#include <cstdlib>

namespace _Wolframe {
namespace db {

class TransactionOutput
{
public:
	TransactionOutput()
		:m_strings("\0\0",2){}
	TransactionOutput( const TransactionOutput& o)
		:m_result(o.m_result)
		,m_strings(o.m_strings){}

	class CommandResultBuilder;

	class CommandResult
	{
	public:
		typedef std::vector< std::size_t> Row;

	public:
		explicit CommandResult( std::size_t functionidx_)
			:m_functionidx(functionidx_){}
		CommandResult( const CommandResult& o)
			:m_functionidx(o.m_functionidx)
			,m_columnName(o.m_columnName)
			,m_row(o.m_row){}

		std::vector<Row>::const_iterator begin() const		{return m_row.begin();}
		std::vector<Row>::const_iterator end() const		{return m_row.end();}

		std::size_t functionidx() const				{return m_functionidx;}
		std::size_t nofColumns() const				{return m_columnName.size();}
		const std::string& columnName( std::size_t i) const	{return m_columnName[i];}

	private:
		friend class TransactionOutput;
		friend class CommandResultBuilder;
		std::size_t m_functionidx;
		std::vector<std::string> m_columnName;
		std::vector<Row> m_row;
	};

	class CommandResultBuilder
		:public CommandResult
	{
	public:
		CommandResultBuilder( TransactionOutput* ref_, std::size_t functionidx_)
			:CommandResult(functionidx_)
			,m_ref(ref_){}

	public:
		void addColumn( const std::string& name)		{m_columnName.push_back( name);}
		void openRow()						{m_row.push_back( CommandResult::Row());}
		void addValue( const std::string& value_)		{m_row.back().push_back( m_ref->getValueIdx( value_));}
		void addNull()						{m_row.back().push_back( 0);}

	private:
		TransactionOutput* m_ref;
	};

	class row_iterator
	{
	public:
		row_iterator(){}
		row_iterator( const TransactionOutput* ref_, const std::vector<CommandResult::Row>::const_iterator& itr_)
			:m_content(ref_,itr_){}

		bool compare( const row_iterator& o) const	{return m_content.m_itr==o.m_content.m_itr;}
		bool operator==( const row_iterator& o) const	{return compare(o);}
		bool operator!=( const row_iterator& o) const	{return !compare(o);}

		row_iterator& operator++()			{++m_content.m_itr; return *this;}

		struct Content
		{
			Content()
				:m_ref(0){}
			Content( const TransactionOutput* ref_, const std::vector<CommandResult::Row>::const_iterator& itr_)
				:m_ref(ref_)
				,m_itr(itr_){}

			const char* operator[]( std::size_t i) const		{return (i >= m_itr->size())?0:m_ref->value((*m_itr)[i]);}
			std::size_t size() const				{return m_itr->size();}

			const TransactionOutput* m_ref;
			std::vector<CommandResult::Row>::const_iterator m_itr;
		};
		const Content& operator*() const				{return m_content;}
		const Content* operator->() const				{return &m_content;}

	private:
		Content m_content;
	};

	class result_iterator
	{
	public:
		result_iterator(){}
		result_iterator( const TransactionOutput* ref_, const std::vector<CommandResult>::const_iterator& itr_)
			:m_content(ref_,itr_){}

		bool compare( const result_iterator& o) const		{return m_content.m_itr==o.m_content.m_itr;}
		bool operator==( const result_iterator& o) const	{return compare(o);}
		bool operator!=( const result_iterator& o) const	{return !compare(o);}

		result_iterator& operator++()				{++m_content.m_itr; return *this;}

		struct Content
		{
			Content()
				:m_ref(0){}
			Content( const TransactionOutput* ref_, const std::vector<CommandResult>::const_iterator& itr_)
				:m_ref(ref_)
				,m_itr(itr_){}

			row_iterator begin() const				{return row_iterator(m_ref, m_itr->begin());}
			row_iterator end() const				{return row_iterator(m_ref, m_itr->end());}

			const std::string& columnName( std::size_t i) const	{return m_itr->columnName(i);}
			std::size_t nofColumns() const				{return m_itr->nofColumns();}
			std::size_t functionidx() const				{return m_itr->functionidx();}

			const TransactionOutput* m_ref;
			std::vector<CommandResult>::const_iterator m_itr;
		};

		const Content& operator*() const				{return m_content;}
		const Content* operator->() const				{return &m_content;}

	private:
		Content m_content;
	};

	result_iterator begin() const						{return result_iterator(this,m_result.begin());}
	result_iterator end() const						{return result_iterator(this,m_result.end());}
	result_iterator last() const						{return m_result.empty()?end():result_iterator(this,m_result.begin() + m_result.size() -1);}

	const char* value( std::size_t idx) const				{return (idx)?(m_strings.c_str()+idx):0;}

	std::size_t getValueIdx( const std::string& v)
	{
		if (v.empty()) return 1;
		std::size_t rt = m_strings.size();
		m_strings.append( v);
		m_strings.push_back('\0');
		return rt;
	}

	std::size_t size() const						{return m_result.size();}

	std::string tostring() const;

public:
	// interface for constructing the result:
	void openCommandResult( std::size_t functionidx)			{m_result.push_back( CommandResult( functionidx));}
	void addCommandResult( const CommandResult& r)				{m_result.push_back( r);}
	void addColumn( const std::string& name)				{m_result.back().m_columnName.push_back( name);}
	void openRow()								{m_result.back().m_row.push_back( CommandResult::Row());}
	void addValue( const std::string& value_)				{m_result.back().m_row.back().push_back( getValueIdx( value_));}
	void addNull()								{m_result.back().m_row.back().push_back( 0);}

private:
	std::vector<CommandResult> m_result;
	std::string m_strings;
};

}}
#endif


