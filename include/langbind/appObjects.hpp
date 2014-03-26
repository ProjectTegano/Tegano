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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
//\file langbind/appObjects.hpp
//\brief Interface to system objects for processor language bindings
#ifndef _Wolframe_langbind_APP_OBJECTS_HPP_INCLUDED
#define _Wolframe_langbind_APP_OBJECTS_HPP_INCLUDED
#include "filter/filter.hpp"
#include "langbind/cppFormFunction.hpp"
#include "processor/procProviderInterface.hpp"
#include "utils/typeSignature.hpp"
#include "types/form.hpp"
#include "types/variantStruct.hpp"
#include "types/variantStructDescription.hpp"
#include "serialize/struct/structSerializer.hpp"
#include "serialize/struct/structParser.hpp"
#include "serialize/ddl/ddlStructSerializer.hpp"
#include "serialize/ddl/ddlStructParser.hpp"
#include <stack>
#include <string>
#include <algorithm>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

//\class Logger
//\brief Logger as seen from scripting language binding
//\remark Empty object because it is represented as singleton in the system
struct Logger
{
	int _;			//< dummy element because some bindings (Lua) do not like empty structures (objects of size 1)
	Logger() :_(0){}
};


//\class Output
//\brief Output as seen from scripting language binding
class Output
	:public virtual utils::TypeSignature
{
public:
	//\brief Constructor
	Output()
		:utils::TypeSignature("langbind::Output", __LINE__)
		,m_state(0)
		,m_called(false){}
	//\brief Copy constructor
	//\param[in] o copied item
	Output( const Output& o)
		:utils::TypeSignature(o)
		,m_outputfilter(o.m_outputfilter)
		,m_state(o.m_state)
		,m_called(o.m_called){}
	//\brief Constructor by output filter
	//\param[in] flt output filter reference
	Output( const OutputFilterR& flt)
		:utils::TypeSignature("langbind::Output", __LINE__)
		,m_outputfilter(flt)
		,m_state(0)
		,m_called(false){}
	//\brief Destructor
	~Output(){}

	//\brief Print the next element
	//\param[in] e1 first element
	//\param[in] e1size first element size
	//\param[in] e2 second element
	//\param[in] e2size second element size
	//\return true, on success, false if we have to yield processing
	bool print( const char* e1, unsigned int e1size, const char* e2, unsigned int e2size);

	const OutputFilterR& outputfilter() const		{return m_outputfilter;}
	OutputFilterR& outputfilter()				{return m_outputfilter;}

	bool called() const					{return m_called;}
	void called( bool yes)					{m_called=yes;}

private:
	OutputFilterR m_outputfilter;				//< output filter reference
	unsigned int m_state;					//< current state for outputs with more than one elements
	bool m_called;						//< has already been called
};


//\class Input
//\brief Input as seen from the application processor program
class Input
	:public virtual utils::TypeSignature
{
public:
	//\brief Constructor
	Input()
		:utils::TypeSignature("langbind::Input", __LINE__)
		,m_used(false){}

	//\brief Copy constructor
	//\param[in] o copied item
	Input( const Input& o);

	//\brief Constructor by input filter
	//\param[in] inputfilter_ input filter reference
	//\param[in] docformat_ document format
	Input( const InputFilterR& inputfilter_, const std::string& docformat_);

	//\brief Constructor by content
	//\param[in] docformat_ document format
	//\param[in] content_ content string
	Input( const std::string& docformat_, const std::string& content_);

	//\brief Destructor
	~Input(){}

	//\brief Get the input filter attached to input
	//\return the input filter reference
	const InputFilterR& inputfilter() const		{return m_inputfilter;}

	//\brief Get the input filter attached to input
	//\return the input filter reference
	InputFilterR& inputfilter()			{return m_inputfilter;}

	//\brief Get the document format as recognized by the document type detection as string {"xml","json",...}
	//\return the document format as string
	const std::string& docformat() const		{return m_docformat;}

	//\brief Get the input filter attached to input and check for duplicate access
	//\return the input filter reference
	InputFilterR& getIterator();

	//\brief Eval if this represents a document
	//\return true, if yes
	bool isDocument() const				{return !m_isProcessorInput;}
	//\brief Get content source string if input is not from network
	//\return the content string pointer or NULL, if not defined
	const char* documentptr() const			{return m_isProcessorInput?0:m_content.get();}
	//\brief Get size of the content source string if input is not from network
	//\return the content string size or 0, if not defined
	std::size_t documentsize() const		{return m_isProcessorInput?0:m_contentsize;}

	//\brief Allocate a copy of the content to the input for a filter attached
	//\return the copy (managed by the input object)
	const void* allocContentCopy( const void* ptr, std::size_t size);

private:
	bool m_used;					//< only one iterator can be created from input. This is the guard for checking this.
	InputFilterR m_inputfilter;			//< input is defined by the associated input filter
	std::string m_docformat;			//< document format as recognized by the document type detection as string {"xml","json",...}
	boost::shared_ptr<char> m_content;		//< content source string is input is not from network
	std::size_t m_contentsize;			//< size of content in bytes
	bool m_isProcessorInput;			//< true, if this Input object represents a document and not the processor input
};


class DDLFormParser
	:public virtual utils::TypeSignature
	,public serialize::DDLStructParser
{
public:
	explicit DDLFormParser( const types::FormR& form_)
		:utils::TypeSignature("langbind::DDLFormParser", __LINE__)
		,DDLStructParser(form_.get())
		,m_form(form_){}

	DDLFormParser( const types::FormR& form_, types::VariantStruct* substructure)
		:utils::TypeSignature("langbind::DDLFormParser", __LINE__)
		,DDLStructParser(substructure)
		,m_form(form_){}

	DDLFormParser( const DDLFormParser& o)
		:utils::TypeSignature(o)
		,DDLStructParser(o)
		,m_form(o.m_form){}

	virtual ~DDLFormParser(){}

	DDLFormParser& operator=( const DDLFormParser& o)
	{
		utils::TypeSignature::operator=( o);
		DDLStructParser::operator=( o);
		m_form = o.m_form;
		return *this;
	}

	const types::FormR& form() const	{return m_form;}

private:
	types::FormR m_form;
};


class DDLFormSerializer
	:public virtual utils::TypeSignature
	,public serialize::DDLStructSerializer
{
public:
	DDLFormSerializer()
		:utils::TypeSignature("langbind::DDLFormSerializer", __LINE__){}

	explicit DDLFormSerializer( const types::FormR& form_)
		:utils::TypeSignature("langbind::DDLFormSerializer", __LINE__)
		,DDLStructSerializer(form_.get())
		,m_form(form_){}

	explicit DDLFormSerializer( const types::FormR& form_, const types::VariantStruct* substructure)
		:utils::TypeSignature("langbind::DDLFormSerializer", __LINE__)
		,DDLStructSerializer(substructure)
		,m_form(form_){}

	DDLFormSerializer( const DDLFormSerializer& o)
		:utils::TypeSignature(o)
		,DDLStructSerializer(o)
		,m_form(o.m_form){}
	virtual ~DDLFormSerializer(){}

	DDLFormSerializer& operator =( const DDLFormSerializer& o)
	{
		utils::TypeSignature::operator=( o);
		DDLStructSerializer::operator=( o);
		m_form = o.m_form;
		return *this;
	}

	const types::FormR& form() const	{return m_form;}

private:
	types::FormR m_form;
};


//\class RedirectFilterClosure
class RedirectFilterClosure
	:public virtual utils::TypeSignature
{
public:
	RedirectFilterClosure();
	RedirectFilterClosure( const TypedInputFilterR& i, const TypedOutputFilterR& o);
	RedirectFilterClosure( const RedirectFilterClosure& o);
	~RedirectFilterClosure(){}

	//\brief Calls the fetching of input and printing it to output until end or interruption
	//\return true when completed
	bool call();

	//\brief Initialization of call context for a new call
	//\param[in] i call input
	//\param[in] o call output
	void init( const TypedInputFilterR& i, const TypedOutputFilterR& o);

	const TypedInputFilterR& inputfilter() const		{return m_inputfilter;}
	const TypedOutputFilterR& outputfilter() const		{return m_outputfilter;}

private:
	int m_state;				//< current state of call
	int m_taglevel;				//< current balance of open/close tags
	TypedInputFilterR m_inputfilter;	//< input filter
	TypedOutputFilterR m_outputfilter;	//< output filter
	InputFilter::ElementType m_elemtype;	//< type of last element read from command result
	types::VariantConst m_elem;		//< last element read from command result
};


//\class ApiFormData
class ApiFormData
	:public virtual utils::TypeSignature
{
public:
	ApiFormData( const serialize::StructDescriptionBase* descr);
	~ApiFormData();

	void* get() const					{return m_data.get();}
	const serialize::StructDescriptionBase* descr() const	{return m_descr;}
	const boost::shared_ptr<void>& data() const		{return m_data;}
private:
	const serialize::StructDescriptionBase* m_descr;
	boost::shared_ptr<void> m_data;
};


//\class CppFormFunctionClosure
//\brief Closure with calling state of called CppFormFunction
class CppFormFunctionClosure
	:public virtual utils::TypeSignature
{
public:
	//\brief Constructor
	//\param[in] f function called
	CppFormFunctionClosure( const CppFormFunction& f);

	//\brief Copy constructor
	//\param[in] o copied item
	CppFormFunctionClosure( const CppFormFunctionClosure& o);

	//\brief Calls the form function with the input from the input filter specified
	//\return true when completed
	bool call();

	//\brief Initialization of call context for a new call
	//\param[in] i call input
	//\param[in] flags serialization flags depending on context (directmap "strict",lua relaxed)
	void init( const proc::ProcessorProviderInterface* provider, const TypedInputFilterR& i, serialize::Context::Flags flags);

	const serialize::StructSerializer& result() const	{return m_result;}

private:
	CppFormFunction m_func;
	int m_state;
	ApiFormData m_param_data;
	ApiFormData m_result_data;
	serialize::StructSerializer m_result;
	serialize::StructParser m_parser;
	const proc::ProcessorProviderInterface* m_provider;
};

}} //namespace
#endif

