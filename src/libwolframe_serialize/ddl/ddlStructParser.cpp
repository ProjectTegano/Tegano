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
///\file ddl/structParser.cpp

#include "serialize/ddl/ddlStructParser.hpp"
#include "filter/typedfilter.hpp"
#include "types/bignumber.hpp"
#include "types/datetime.hpp"
#include "types/customDataType.hpp"
#include "types/variantStruct.hpp"
#include "types/variantStructDescription.hpp"
#include "serializationErrorException.hpp"
#include "logger/logger-v1.hpp"
#include "utils/printFormats.hpp"
#include <cstring>
#include <sstream>

using namespace _Wolframe;
using namespace _Wolframe::serialize;

static bool emptycontent( const types::VariantConst& val)
{
	if (val.type() != types::Variant::String) return false;
	std::size_t ii = 0, nn = val.charsize();
	const char* cc = val.charptr();

	for (; ii<nn; ++ii)
	{
		if ((unsigned char)cc[ii]> 32) return false;
	}
	return true;
}

static std::string getElementPath( const DDLParseStateStack& stk)
{
	std::string rt;
	DDLParseStateStack::const_iterator itr=stk.begin(), end=stk.end();
	for (; itr != end; ++itr)
	{
		if (itr->name())
		{
			rt.append( "/");
			rt.append( itr->name());
		}
	}
	return rt;
}

// forward declarations
static bool parseObject( langbind::TypedInputFilter& inp, ValidationFlags::Enum vflags, std::vector<DDLParseState>& stk);

enum AtomicValueState
{
	TagValueOpen,
	TagValueParsed,
	AttributeValueOpen
};

static void setAtomValue( types::Variant& val, const types::VariantConst& element, const types::NormalizeFunction* normalizer)
{
	types::Variant::Type valueType = val.type();
	if (normalizer)
	{
		val = normalizer->execute( element);
	}
	else if (valueType == types::Variant::Null)
	{
		val = element;
	}
	else if (valueType == types::Variant::Custom)
	{
		val.customref()->assign( element);
	}
	else
	{
		val.assign( valueType, element);
	}
}

static bool parseAtom( types::Variant& val, langbind::TypedInputFilter& inp, ValidationFlags::Enum, std::vector<DDLParseState>& stk)
{
	langbind::InputFilter::ElementType typ;
	types::VariantConst element;

	if (!inp.getNext( typ, element))
	{
		if (inp.state() != langbind::InputFilter::Error) return false;
		throw SerializationErrorException( inp.getError(), element.tostring(), getElementPath( stk));
	}
	LOG_DATA << "[DDL structure serialization parse] atomic element " << langbind::InputFilter::elementTypeName( typ) << " '" << utils::getLogString( element) << "'";
	switch (typ)
	{
		case langbind::InputFilter::OpenTag:
		case langbind::InputFilter::OpenTagArray:
			throw SerializationErrorException( "atomic value expected instead of tag", element.tostring(), getElementPath( stk));

		case langbind::InputFilter::Attribute:
			throw SerializationErrorException( "atomic value expected instead of attribute", element.tostring(), getElementPath( stk));

		case langbind::InputFilter::Value:
			if (stk.back().state() == TagValueParsed)
			{
				throw SerializationErrorException( "two subsequent values for atomic value", element.tostring(), getElementPath( stk));
			}
			setAtomValue( val, element, stk.back().normalizer());

			if (stk.back().state() == AttributeValueOpen)
			{
				stk.pop_back();
			}
			else
			{
				stk.back().state( TagValueParsed);
			}
			return true;

		case langbind::InputFilter::CloseTag:
			if (stk.back().state() == AttributeValueOpen)
			{
				throw SerializationErrorException( "missing attribute value", element.tostring(), getElementPath( stk));
			}
			else if (stk.back().state() == TagValueOpen)
			{
				val = std::string();
			}
			stk.pop_back();
			return true;
	}
	throw SerializationErrorException( "illegal state in parse DDL form atomic value", getElementPath( stk));
}

static bool parseStruct( types::VariantStruct& st, langbind::TypedInputFilter& inp, ValidationFlags::Enum vflags, std::vector<DDLParseState>& stk)
{
	langbind::InputFilter::ElementType typ;
	types::VariantConst element;

	const types::VariantStructDescription* descr = st.description();
	if (!descr) throw SerializationErrorException( "structure expected", element.tostring(), getElementPath( stk));

	if (!inp.getNext( typ, element))
	{
		if (inp.state() != langbind::InputFilter::Error) return false;
		throw SerializationErrorException( inp.getError(), element.tostring(), getElementPath( stk));
	}
	LOG_DATA << "[DDL structure serialization parse] structure element " << langbind::InputFilter::elementTypeName( typ) << " '" << utils::getLogString( element) << "'";
	switch (typ)
	{
		case langbind::InputFilter::OpenTag:
		case langbind::InputFilter::OpenTagArray:
		{
			int idx;
			if (ValidationFlags::match( vflags, ValidationFlags::ValidateCase))
			{
				idx = descr->findidx( element.tostring());
			}
			else
			{
				idx = descr->findidx_cis( element.tostring());
			}
			if (idx < 0)
			{
				throw SerializationErrorException( "unknown tag ", element.tostring(), getElementPath( stk), std::string(".. candidates are {") + descr->names(", ") + "}");
			}
			types::VariantStructDescription::const_iterator
				ei = descr->begin() + idx;

			if (ei->attribute())
			{
				if (ValidationFlags::match( vflags, ValidationFlags::ValidateAttributes))
				{
					throw SerializationErrorException( "attribute element expected for ", element.tostring(), getElementPath( stk));
				}
			}
			types::VariantStruct* elem = st.at( idx);
			types::VariantStruct::Type type = elem->type();

			if (type != types::VariantStruct::Array && elem->initialized())
			{
				throw SerializationErrorException( "duplicate structure element definition", element.tostring(), getElementPath( stk));
			}
			if (ValidationFlags::match( vflags, ValidationFlags::ValidateArray))
			{
				if (typ == langbind::InputFilter::OpenTagArray)
				{
					if (type != types::VariantStruct::Array)
					{
						throw SerializationErrorException( "single element expected instead of array", element.tostring(), getElementPath( stk));
					}
				}
				else
				{
					if (type == types::VariantStruct::Array)
					{
						throw SerializationErrorException( "array expected instead of single element", element.tostring(), getElementPath( stk));
					}
				}
			}

			elem->setInitialized();
			stk.push_back( DDLParseState( ei->name, elem, ei->normalizer));
			if (elem->atomic())
			{
				stk.back().state( TagValueOpen);
			}
			return true;
		}

		case langbind::InputFilter::Attribute:
		{
			int idx;
			if (ValidationFlags::match( vflags, ValidationFlags::ValidateCase))
			{
				idx = descr->findidx( element.tostring());
			}
			else
			{
				idx = descr->findidx_cis( element.tostring());
			}
			if (idx < 0)
			{
				throw SerializationErrorException( "unknown attribute ", element.tostring(), getElementPath( stk));
			}
			types::VariantStructDescription::const_iterator ei = descr->begin() + idx;
			if (!ei->attribute())
			{
				if (ValidationFlags::match( vflags, ValidationFlags::ValidateAttributes))
				{
					throw SerializationErrorException( "content element expected", element.tostring(), getElementPath( stk));
				}
			}
			types::VariantStruct* elem = st.at( idx);
			if (!elem->atomic())
			{
				throw SerializationErrorException( "atomic element expected", element.tostring(), getElementPath( stk));
			}
			if (elem->initialized())
			{
				throw SerializationErrorException( "duplicate structure attribute definition", element.tostring(), getElementPath( stk));
			}
			elem->setInitialized();
			stk.push_back( DDLParseState( ei->name, elem, ei->normalizer));
			stk.back().state( AttributeValueOpen);
			return true;
		}

		case langbind::InputFilter::Value:
		{
			int idx = descr->findidx( "");
			if (idx < 0)
			{
				if (emptycontent( element)) return true;
				throw SerializationErrorException( "parsed untagged value, but no untagged value is defined for this structure", element.tostring(), getElementPath( stk));
			}
			types::VariantStructDescription::const_iterator
				ei = descr->begin() + idx;
			if (ei->attribute())
			{
				throw SerializationErrorException( "error in structure definition: defined untagged value as attribute in structure", element.tostring(), getElementPath( stk));
			}
			types::VariantStruct* elem = st.at( idx);
			elem->setInitialized();

			switch (elem->type())
			{
				case types::VariantStruct::Null:
				case types::VariantStruct::Bool:
				case types::VariantStruct::Double:
				case types::VariantStruct::Int:
				case types::VariantStruct::UInt:
				case types::VariantStruct::String:
				case types::VariantStruct::Custom:
				case types::VariantStruct::Timestamp:
				case types::VariantStruct::BigNumber:
					setAtomValue( *elem, element, stk.back().normalizer());
					return true;

				case types::VariantStruct::Struct:
				case types::VariantStruct::Indirection:
				case types::VariantStruct::Unresolved:
				{
					throw SerializationErrorException( "atomic element or vector of atomic elements expected for untagged value in structure", element.tostring(), getElementPath( stk));
				}
				case types::VariantStruct::Array:
					if (elem->prototype()->atomic())
					{
						elem->push();
						setAtomValue( elem->back(), element, stk.back().normalizer());
						elem->back().setInitialized();
						return true;
					}
					else
					{
						throw SerializationErrorException( "atomic element or vector of atomic elements expected for untagged value in structure", element.tostring(), getElementPath( stk));
					}
			}
		}

		case langbind::InputFilter::CloseTag:
		{
			types::VariantStruct::iterator itr = st.begin(), end = st.end();
			types::VariantStructDescription::const_iterator di = descr->begin();

			for (;itr != end; ++itr,++di)
			{
				if (di->mandatory() && !itr->initialized())
				{
					throw SerializationErrorException( "undefined mandatory structure element", di->name, getElementPath( stk));
				}
				if (ValidationFlags::match( vflags, ValidationFlags::ValidateInitialization))
				{
					if (!di->optional() && !di->array() && !itr->initialized())
					{
						throw SerializationErrorException( "schema validation failed: undefined non optional structure element", di->name, getElementPath( stk));
					}
				}
			}
			stk.pop_back();
			return true;
		}
	}
	throw SerializationErrorException( "illegal state in parse DDL form structure", getElementPath( stk));
}


static bool parseObject( langbind::TypedInputFilter& inp, ValidationFlags::Enum vflags, std::vector<DDLParseState>& stk)
{
	types::VariantStruct* elem = stk.back().value();
	switch (elem->type())
	{
		case types::VariantStruct::Null:
		case types::VariantStruct::Bool:
		case types::VariantStruct::Double:
		case types::VariantStruct::Int:
		case types::VariantStruct::UInt:
		case types::VariantStruct::String:
		case types::VariantStruct::Custom:
		case types::VariantStruct::Timestamp:
		case types::VariantStruct::BigNumber:
		{
			return parseAtom( *stk.back().value(), inp, vflags, stk);
		}
		case types::VariantStruct::Struct:
		{
			return parseStruct( *stk.back().value(), inp, vflags, stk);
		}
		case types::VariantStruct::Unresolved:
		{
			throw SerializationErrorException( "incomplete structure definition (unresolved indirection)", getElementPath( stk));
		}
		case types::VariantStruct::Indirection:
		{
			types::VariantStruct* st = stk.back().value();
			st->expandIndirection();
			if (st->type() == types::VariantStruct::Indirection)
			{
				throw SerializationErrorException( "indirection expanding to indirection", getElementPath( stk));
			}
			return parseObject( inp, vflags, stk);
		}
		case types::VariantStruct::Array:
		{
			stk.back().value()->push();
			types::VariantStruct* velem = &stk.back().value()->back();
			velem->setInitialized();
			const char* velemname = stk.back().name();
			stk.pop_back();
			stk.push_back( DDLParseState( velemname, velem, stk.back().normalizer()));
			return true;
		}
	}
	throw SerializationErrorException( "illegal state in parse DDL form object", getElementPath( stk));
}

DDLStructParser::DDLStructParser( types::VariantStruct* st)
	:m_st(st)
{
	m_stk.push_back( DDLParseState( 0, st, 0));
}

DDLStructParser::DDLStructParser( const DDLStructParser& o)
	:m_st(o.m_st)
	,m_validationFlags(o.m_validationFlags)
	,m_inp(o.m_inp)
	,m_stk(o.m_stk)
	{}

DDLStructParser& DDLStructParser::operator=( const DDLStructParser& o)
{
	m_st = o.m_st;
	m_validationFlags = o.m_validationFlags;
	m_inp = o.m_inp;
	m_stk = o.m_stk;
	return *this;
}

void DDLStructParser::init( const langbind::TypedInputFilterR& i, ValidationFlags::Enum vflags)
{
	m_inp = i;
	m_validationFlags = vflags;

	if (i->flag( langbind::FilterBase::PropagateNoCase))
	{
		ValidationFlags::unset( m_validationFlags, ValidationFlags::ValidateCase);
	}
	if (i->flag( langbind::FilterBase::PropagateNoAttr))
	{
		ValidationFlags::unset( m_validationFlags, ValidationFlags::ValidateAttributes);
	}
	if (i->flag( langbind::FilterBase::PropagateNoArray))
	{
		ValidationFlags::unset( m_validationFlags, ValidationFlags::ValidateArray);
	}
	m_stk.clear();
	m_stk.push_back( DDLParseState( 0, m_st, 0));
	LOG_DATA << "[DDL structure serialization parse] init structure" << "[" << (m_st->description()?m_st->description()->tostring():m_st->tostring()) << "]";
}

bool DDLStructParser::call()
{
	bool rt = true;
	if (!m_inp.get()) throw std::runtime_error( "no input for parse");

	while (rt && m_stk.size())
	{
		rt = parseObject( *m_inp, m_validationFlags, m_stk);
	}
	if (rt)
	{
		types::VariantStruct::iterator vi = m_st->begin(), ve = m_st->end();
		types::VariantStructDescription::const_iterator di = m_st->description()->begin();
		for (; vi != ve; ++vi,++di)
		{
			if (vi->initialized())
			{
				m_st->setInitialized();
				break;
			}
		}
	}
	return rt;
}

