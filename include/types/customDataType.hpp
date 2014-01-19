/************************************************************************
Copyright (C) 2011 - 2013 Project Wolframe.
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
///\file types/customDataType.hpp
///\brief Custom data type interface for variant

#ifndef _Wolframe_TYPES_CUSTOM_DATA_TYPE_HPP_INCLUDED
#define _Wolframe_TYPES_CUSTOM_DATA_TYPE_HPP_INCLUDED
#include <string>
#include <cstring>
#include "types/countedReference.hpp"
#include "types/normalizeFunction.hpp"

namespace _Wolframe {
namespace proc
{
	//\brief Forward declaration
	class ProcessorProvider;
}
namespace types {

//\brief Forward declaration
class Variant;
//\brief Forward declaration
class CustomDataType;
//\brief Forward declaration
class CustomDataInitializer;


class CustomDataValue
{
public:
	CustomDataValue()
		:m_type(0),m_initializer(0){}
	virtual ~CustomDataValue(){};

	const CustomDataType* type() const			{return m_type;}
	const CustomDataInitializer* initializer() const	{return m_initializer;}

	virtual int compare( const CustomDataValue& o) const=0;
	virtual std::string tostring() const=0;
	virtual void assign( const Variant& o)=0;

private:
	friend class CustomDataType;
	const CustomDataType* m_type;
	const CustomDataInitializer* m_initializer;
};

typedef types::CountedReference<CustomDataValue> CustomDataValueR;


class CustomDataInitializer
{
public:
	CustomDataInitializer(){}
	virtual ~CustomDataInitializer(){};
};

typedef types::CountedReference<CustomDataInitializer> CustomDataInitializerR;


typedef CustomDataInitializer* (*CreateCustomDataInitializer)( const std::string& description);
typedef CustomDataValue* (*CustomDataValueConstructor)( const CustomDataInitializer* initializer);
typedef CustomDataValue* (*CustomDataValueCopyConstructor)( const CustomDataValue* o);


class CustomDataType
{
public:
	typedef unsigned int ID;
	enum {NofUnaryOperators=3};
	enum UnaryOperatorType {Increment,Decrement,Negation};
	enum {NofBinaryOperators=6};
	enum BinaryOperatorType {Add,Subtract,Multiply,Divide,Power,Concat};
	enum {NofDimensionOperators=1};
	enum DimensionOperatorType {Length};

	typedef types::Variant (*UnaryOperator)( const CustomDataValue& operand);
	typedef types::Variant (*BinaryOperator)( const CustomDataValue& operand, const Variant& arg);
	typedef std::size_t (*DimensionOperator)( const CustomDataValue& arg);

public:
	CustomDataType()
		:m_name("null"),m_id(0)
	{
		std::memset( &m_vmt, 0, sizeof( m_vmt));
	}

	CustomDataType( const std::string& name_,
				CustomDataValueConstructor constructor_,
				CustomDataValueCopyConstructor copyconstructor_,
				CreateCustomDataInitializer initializerconstructor_);

	CustomDataType( const CustomDataType& o);

	void define( UnaryOperatorType type, UnaryOperator op);
	void define( BinaryOperatorType type, BinaryOperator op);
	void define( DimensionOperatorType type, DimensionOperator op);

	UnaryOperator getOperator( UnaryOperatorType type) const;
	BinaryOperator getOperator( BinaryOperatorType type) const;
	DimensionOperator getOperator( DimensionOperatorType type) const;

	CustomDataInitializer* createInitializer( const std::string& d) const;
	CustomDataValue* createValue( const CustomDataInitializer* i=0) const;
	CustomDataValue* copyValue( const CustomDataValue& o) const;

	const ID& id() const			{return m_id;}
	const std::string& name() const		{return m_name;}

private:
	std::string m_name;
	friend class proc::ProcessorProvider;
	ID m_id;

	struct
	{
		UnaryOperator opUnary[ NofUnaryOperators];
		BinaryOperator opBinary[ NofBinaryOperators];
		DimensionOperator opDimension[ NofDimensionOperators];
		CreateCustomDataInitializer opInitializerConstructor;
		CustomDataValueConstructor opConstructor;
		CustomDataValueCopyConstructor opCopyConstructor;
	}
	m_vmt;
};

typedef types::CountedReference<CustomDataType> CustomDataTypeR;


typedef CustomDataType (*CreateCustomDataType)( const std::string& name);



class CustomDataNormalizer
	:public types::NormalizeFunction
{
public:
	CustomDataNormalizer( const std::string& name_, const CustomDataType* type_, const CustomDataInitializer* initializer_)
		:m_name(name_)
		,m_type(type_)
		,m_initializer(initializer_){}

	virtual ~CustomDataNormalizer(){}

	virtual const char* name() const
	{
		return m_name.c_str();
	}

	virtual Variant execute( const Variant& i) const;

private:
	std::string m_name;
	const CustomDataType* m_type;
	const CustomDataInitializer* m_initializer;
};

}}//namespace
#endif