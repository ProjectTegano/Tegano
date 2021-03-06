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
#include "comauto/utils.hpp"
#include "types/numberBaseConversion.hpp"
#include <iostream>
#include <sstream>
#pragma warning(disable:4996)

using namespace _Wolframe;

const char*& comauto::impl_V_LPSTR( const VARIANT* v)		{ return *(const char**)comauto::arithmeticTypeAddress(v); }
char*& comauto::impl_V_LPSTR( VARIANT* v)			{ return *(char**)comauto::arithmeticTypeAddress(v); }
const wchar_t*& comauto::impl_V_LPWSTR( const VARIANT* v)	{ return *(const wchar_t**)comauto::arithmeticTypeAddress(v); }
wchar_t*& comauto::impl_V_LPWSTR( VARIANT* v)			{ return *(wchar_t**)comauto::arithmeticTypeAddress(v); }
const BSTR& comauto::impl_V_BSTR( const VARIANT* v)		{ return *(const BSTR*)&v->bstrVal;}
BSTR& comauto::impl_V_BSTR( VARIANT* v)				{ return *(BSTR*)&v->bstrVal;}

void* comauto::allocMem( std::size_t size)
{
	return ::CoTaskMemAlloc( size + 1000);
}

void comauto::freeMem( void* ptr)
{
	::CoTaskMemFree( ptr);
}

bool comauto::isCOMInterfaceMethod( const std::string& name)
{
	if (std::strcmp( name.c_str(), "AddRef") == 0) return true;
	if (std::strcmp( name.c_str(), "Release") == 0) return true;
	if (std::strcmp( name.c_str(), "GetTypeInfo") == 0) return true;
	if (std::strcmp( name.c_str(), "GetTypeInfoCount") == 0) return true;
	if (std::strcmp( name.c_str(), "GetIDsOfNames") == 0) return true;
	if (std::strcmp( name.c_str(), "Invoke") == 0) return true;
	if (std::strcmp( name.c_str(), "QueryInterface") == 0) return true;
	if (std::strcmp( name.c_str(), "ToString") == 0) return true;
	if (std::strcmp( name.c_str(), "Equals") == 0) return true;
	if (std::strcmp( name.c_str(), "GetType") == 0) return true;
	if (std::strcmp( name.c_str(), "GetHashCode") == 0) return true;
	return false;
}

std::wstring comauto::utf16string( const char* utf8ptr, std::size_t utf8size)
{
	std::wstring rt;
	int len = ::MultiByteToWideChar( CP_UTF8, 0, utf8ptr, utf8size, NULL, 0);
	if (len>0)
	{
		rt.resize( len);
		::MultiByteToWideChar( CP_UTF8, 0, utf8ptr, utf8size, (LPWSTR)(void*)rt.c_str(), len);
	}
	return rt;
}

std::wstring comauto::utf16string( const std::string& utf8str)
{
	return comauto::utf16string( utf8str.c_str(), utf8str.size());
}

std::string comauto::utf8string( const std::wstring& utf16str)
{
	std::string rt;
	int len = ::WideCharToMultiByte( CP_UTF8, 0, utf16str.c_str(), utf16str.size(), NULL, 0, NULL, NULL);
	if (len>0)
	{
		rt.resize( len);
		::WideCharToMultiByte( CP_UTF8, 0, utf16str.c_str(), utf16str.size(), (LPSTR)(void*)rt.c_str(), len, NULL, NULL);
	}
	return rt;
}

std::string comauto::utf8string( LPCSTR str)
{
	std::wstring rt;
	std::size_t ii = 0;
	for (; str[ii]; ++ii) rt.push_back( (wchar_t)str[ii]);
	return comauto::utf8string( rt);
}

std::string comauto::utf8string( LPCWSTR str)
{
	return comauto::utf8string( std::wstring( str));
}

std::string comauto::utf8string( const BSTR& str)
{
	std::string rt;
	int strl = ::SysStringLen( str);
	int len = ::WideCharToMultiByte( CP_UTF8, 0, str, strl, NULL, 0, NULL, NULL);
	if (len>0)
	{
		rt.resize( len);
		::WideCharToMultiByte( CP_UTF8, 0, str, strl, (LPSTR)(void*)rt.c_str(), len, NULL, NULL);
	}
	return rt;
}

char* comauto::createLPSTR( const std::string& str)
{
	LPSTR rt;
	rt = (LPSTR)comauto::allocMem( str.size() + sizeof(char));
	if (rt == NULL) throw std::bad_alloc();
	::strcpy( rt, str.c_str());
	return rt;
}

char* comauto::createLPSTR( LPCSTR str)
{
	if (!str) return NULL;
	char* rt = (char*)comauto::allocMem( std::strlen(str) + sizeof(char));
	if (rt == NULL) throw std::bad_alloc();
	::strcpy( rt, str);
	return rt;
}

wchar_t* comauto::createLPWSTR( const std::wstring& str)
{
	wchar_t* rt = (wchar_t*)comauto::allocMem( (str.size()+1) * sizeof(wchar_t));
	if (rt == NULL) throw std::bad_alloc();
	::wcscpy( rt, str.c_str());
	return rt;
}

wchar_t* comauto::createLPWSTR( const wchar_t* str)
{
	if (!str) return NULL;
	wchar_t* rt = (wchar_t*)comauto::allocMem( (std::wcslen( str)+1) * sizeof(wchar_t));
	if (rt == NULL) throw std::bad_alloc();
	::wcscpy( rt, str);
	return rt;
}

BSTR comauto::createBSTR( LPCSTR str)
{
	std::wstring wstr = str?comauto::utf16string( str):L"";
	return ::SysAllocStringLen( wstr.c_str(), wstr.size());
}

BSTR comauto::createBSTR( LPCWSTR str)
{
	return ::SysAllocString( str?str:L"");
}

std::wstring comauto::tostring( VARIANT* v)
{
	VARIANT vcp;
	vcp.vt = VT_EMPTY;
	WRAP( comauto::wrapVariantCopy( &vcp, v));
	if (v->vt != VT_BSTR)
	{
		WRAP( comauto::wrapVariantChangeType( &vcp, &vcp, 0, VT_BSTR));
	}
	std::wstring rt( vcp.bstrVal, ::SysStringLen( vcp.bstrVal));
	return rt;
}

std::wstring comauto::tostring( const _com_error& err)
{
	std::wstring rt;
	LPTSTR errstr = NULL;

	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, err.Error(), MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&errstr, 0, NULL);

	if (errstr)
	{
		rt.append( (wchar_t*)errstr);
		::LocalFree( errstr);
	}
	else
	{
		const TCHAR* ee = err.ErrorMessage();
		for (int ii=0; ee[ii]; ++ii) rt.push_back( ee[ii]);
	}
	return rt;
}

std::string comauto::tostring( const EXCEPINFO& einfo)
{
	std::ostringstream rt;
	rt << "[" << std::hex << einfo.wCode << "/" << einfo.scode << "]: " << std::dec << comauto::utf8string( einfo.bstrDescription);
	return rt.str();
}

std::string comauto::typestr( VARTYPE vt)
{
	std::string rt;
	switch (vt)
	{
		case VT_EMPTY:	rt.append( "VT_EMPTY"); break;
		case VT_VOID:	rt.append( "VT_VOID"); break;
		case VT_NULL:	rt.append( "VT_NULL"); break;
		case VT_INT:	rt.append( "VT_INT"); break;
		case VT_I1:	rt.append( "VT_I1"); break;
		case VT_I2:	rt.append( "VT_I2"); break;
		case VT_I4:	rt.append( "VT_I4"); break;
		case VT_UINT:	rt.append( "VT_UINT"); break;
		case VT_UI1:	rt.append( "VT_UI1"); break;
		case VT_UI2:	rt.append( "VT_UI2"); break;
		case VT_UI4:	rt.append( "VT_UI4"); break;
		case VT_R4:	rt.append( "VT_R4"); break;
		case VT_R8:	rt.append( "VT_R8"); break;
		case VT_CY:	rt.append( "VT_CY"); break;
		case VT_DATE:	rt.append( "VT_DATE"); break;
		case VT_BSTR:	rt.append( "VT_BSTR"); break;
		case VT_DISPATCH:rt.append( "VT_DISPATCH"); break;
		case VT_ERROR:	rt.append( "VT_ERROR"); break;
		case VT_BOOL:	rt.append( "VT_BOOL"); break;
		case VT_VARIANT:rt.append( "VT_VARIANT"); break;
		case VT_DECIMAL:rt.append( "VT_DECIMAL"); break;
		case VT_RECORD:	rt.append( "VT_RECORD"); break;
		case VT_UNKNOWN:rt.append( "VT_UNKNOWN"); break;
		case VT_HRESULT:rt.append( "VT_HRESULT"); break;
		case VT_CARRAY:	rt.append( "VT_CARRAY"); break;
		case VT_LPSTR:	rt.append( "VT_LPSTR"); break;
		case VT_LPWSTR:	rt.append( "VT_LPWSTR"); break;
		case VT_BLOB:	rt.append( "VT_BLOB"); break;
		case VT_STREAM:	rt.append( "VT_STREAM"); break;
		case VT_STORAGE:rt.append( "VT_STORAGE"); break;
		case VT_STREAMED_OBJECT:rt.append( "VT_STREAMED_OBJECT"); break;
		case VT_STORED_OBJECT:rt.append( "VT_STORED_OBJECT"); break;
		case VT_BLOB_OBJECT:rt.append( "VT_BLOB_OBJECT"); break;
		case VT_CF:	rt.append( "VT_CF"); break;
		case VT_CLSID:	rt.append( "VT_CLSID"); break;
		default:	rt.append( "[Unknown]"); break;
	}
	return rt;
}

std::string comauto::typestr( const ITypeInfo* typeinfo, const TYPEDESC* typedesc)
{
	std::string rt;
	VARTYPE vt = typedesc->vt;
	if ((vt & VT_BYREF) == VT_BYREF)
	{
		rt.append( "-> ");
		vt -= VT_BYREF;
	}
	if ((vt & VT_ARRAY) == VT_ARRAY)
	{
		rt.append( "[] ");
		vt -= VT_ARRAY;
	}
	if ((vt & VT_VECTOR) == VT_VECTOR)
	{
		rt.append( "[] ");
		vt -= VT_VECTOR;
	}
	if (vt == VT_PTR)
	{
		rt.append( "^");
		rt.append( typestr( typeinfo, typedesc->lptdesc));
		return rt;
	}
	if (vt == VT_SAFEARRAY)
	{
		rt.append( "[] ");
		rt.append( typestr( typeinfo, typedesc->lptdesc));
		return rt;
	}
	if (vt == VT_USERDEFINED)
	{
		ITypeInfo* rectypeinfo = 0;
		TYPEATTR *recattr = 0;
		try
		{
			WRAP( const_cast<ITypeInfo*>(typeinfo)->GetRefTypeInfo( typedesc->hreftype, &rectypeinfo))
			WRAP( rectypeinfo->GetTypeAttr( &recattr))
			if (recattr->typekind == TKIND_RECORD)
			{
				rt.append( "{");
				rt.append( structstring( rectypeinfo));
				rt.append( "}");
			}
			else if (recattr->typekind == TKIND_DISPATCH)
			{
				rt.append( "[IDispatch]");
			}
			else if (recattr->typekind == TKIND_INTERFACE)
			{
				rt.append( "[Interface]");
			}
			else
			{
				throw std::runtime_error("Can only handle VT_USERDEFINED type of kind VT_RECORD (a structure with no methods) or TKIND_DISPATCH (processor provider context)");
			}
		}
		catch (const std::runtime_error& e)
		{
			if (rectypeinfo && recattr) rectypeinfo->ReleaseTypeAttr( recattr);
			if (rectypeinfo) rectypeinfo->Release();
			throw e;
		}
		rectypeinfo->ReleaseTypeAttr( recattr);
		rectypeinfo->Release();
		return rt;
	}
	rt.append( comauto::typestr( vt));
	return rt;
}

std::string comauto::typestr( const ITypeInfo* typeinfo)
{
	CComBSTR typeName;
	WRAP( const_cast<ITypeInfo*>(typeinfo)->GetDocumentation( MEMBERID_NIL, &typeName, NULL, NULL, NULL))
	std::string rt( comauto::utf8string( typeName));
	::SysFreeString( typeName);
	return rt;
}

std::string comauto::structstring( const ITypeInfo* typeinfo)
{
	VARDESC* var = 0;
	TYPEATTR* typeattr = 0;
	BSTR varname = NULL;
	std::ostringstream out;

	try
	{
		WRAP( const_cast<ITypeInfo*>(typeinfo)->GetTypeAttr( &typeattr));
		for (unsigned short ii = 0; ii < typeattr->cVars; ++ii)
		{
			WRAP( const_cast<ITypeInfo*>(typeinfo)->GetVarDesc( ii, &var))
			BSTR varname;
			UINT nn;
			WRAP( const_cast<ITypeInfo*>(typeinfo)->GetNames( var->memid, &varname, 1, &nn))
			ELEMDESC ed = var->elemdescVar;
			if (ii > 0) out << ";";
			out << comauto::utf8string(varname) << ":" << comauto::typestr( typeinfo, &ed.tdesc);
			const_cast<ITypeInfo*>(typeinfo)->ReleaseVarDesc( var);
			var = 0;
			::SysFreeString( varname);
			varname = NULL;
		}
		const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
		return out.str();
	}
	catch (const std::runtime_error& e)
	{
		if (var) const_cast<ITypeInfo*>(typeinfo)->ReleaseVarDesc( var);
		if (varname) ::SysFreeString( varname);
		if (typeattr) const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
		throw e;
	}
}

VARIANT comauto::createVariantType( bool val)
{
	VARIANT rt;
	rt.vt = VT_BOOL;
	rt.boolVal = val ? VARIANT_TRUE : VARIANT_FALSE;
	return rt;
}

namespace {
template <int IntTypeSize>
struct IntegerConstructor
{
	template <typename Type>
	static VARIANT createUnsigned( Type val)
	{
		VARIANT rt;
		rt.vt = VT_UINT;
		rt.uintVal = val;
		return rt;
	}
	template <typename Type>
	static VARIANT createInteger( Type val)
	{
		VARIANT rt;
		rt.vt = VT_INT;
		rt.intVal = val;
		return rt;
	}
};

template <>
struct IntegerConstructor<2>
{
	template <typename Type>
	static VARIANT createUnsigned( Type val)
	{
		VARIANT rt;
		rt.vt = VT_UI2;
		rt.uiVal = val;
		return rt;
	}
	template <typename Type>
	static VARIANT createInteger( Type val)
	{
		VARIANT rt;
		rt.vt = VT_I2;
		rt.iVal = val;
		return rt;
	}
};

template <>
struct IntegerConstructor<4>
{
	template <typename Type>
	static VARIANT createUnsigned( Type val)
	{
		VARIANT rt;
		rt.vt = VT_I4;
		rt.lVal = val;
		return rt;
	}
	template <typename Type>
	static VARIANT createInteger( Type val)
	{
		VARIANT rt;
		rt.vt = VT_I4;
		rt.lVal = val;
		return rt;
	}
};

template <>
struct IntegerConstructor<8>
{
	template <typename Type>
	static VARIANT createUnsigned( Type val)
	{
		VARIANT rt;
		rt.vt = VT_I8;
		rt.hVal.QuadPart = val;
		return rt;
	}
	template <typename Type>
	static VARIANT createInteger( Type val)
	{
		VARIANT rt;
		rt.vt = VT_I8;
		rt.uhVal.QuadPart = val;
		return rt;
	}
};
}//anonymous namespace

VARIANT comauto::createVariantType( int val)
{
	return IntegerConstructor<sizeof(val)>::createInteger<int>( val);
}

VARIANT comauto::createVariantType()
{
	VARIANT rt;
	rt.vt = VT_EMPTY;
	return rt;
}

VARIANT comauto::createVariantType( unsigned int val)
{
	VARIANT rt;
	rt.vt = VT_UINT;
	rt.uintVal = val;
	return rt;
}

VARIANT comauto::createVariantType( short val)
{
	VARIANT rt;
	rt.vt = VT_I2;
	rt.iVal = val;
	return rt;
}

VARIANT comauto::createVariantType( unsigned short val)
{
	VARIANT rt;
	rt.vt = VT_UI2;
	rt.uiVal = val;
	return rt;
}

VARIANT comauto::createVariantType( signed __int64 val)
{
	VARIANT rt;
	rt.vt = VT_I8;
	rt.llVal = val;
	return rt;
}

VARIANT comauto::createVariantType( unsigned __int64 val)
{
	VARIANT rt;
	rt.vt = VT_UI8;
	rt.ullVal = val;
	return rt;
}

VARIANT comauto::createVariantType( float val)
{
	VARIANT rt;
	rt.vt = VT_R4;
	rt.fltVal = val;
	return rt;
}

VARIANT comauto::createVariantType( double val)
{
	VARIANT rt;
	rt.vt = VT_R8;
	rt.dblVal = val;
	return rt;
}

VARIANT comauto::createVariantType( const std::wstring& val)
{
	VARIANT rt;
	rt.vt = VT_BSTR;
	rt.bstrVal = ::SysAllocString( bstr_t( val.c_str()));
	if (rt.bstrVal == NULL) throw std::bad_alloc();
	return rt;
}

VARIANT comauto::createVariantType( const char* val, std::size_t valsize, VARTYPE stringtype)
{
	VARIANT rt;
	rt.vt = stringtype;
	switch (stringtype)
	{
		case VT_BSTR:
		{
			std::wstring wstr( comauto::utf16string( val, valsize));
			rt.bstrVal = ::SysAllocString( bstr_t( wstr.c_str()));
			if (rt.bstrVal == NULL) throw std::bad_alloc();
			break;
		}
		case VT_LPSTR:
		{
			rt.pcVal = (LPSTR)comauto::allocMem( valsize+1);
			if (rt.pcVal == NULL) throw std::bad_alloc();
			::memcpy( rt.pcVal, val, valsize+1);
			break;
		}
		case VT_LPWSTR:
		{
			std::wstring wstr( comauto::utf16string( val, valsize));
			std::size_t mm = (wstr.size()+1) * sizeof(wchar_t);
			rt.pcVal = (char*)(LPWSTR)comauto::allocMem( mm);
			if (rt.pcVal == NULL) throw std::bad_alloc();
			::memcpy( rt.pcVal, wstr.c_str(), mm);
			break;
		}
		default:
			throw std::logic_error( "selected unkown string type");
	}
	return rt;
}

VARIANT comauto::createVariantType( const types::DateTime& dt)
{
	VARIANT rt;
	rt.vt = VT_DATE;
	rt.date = dt.toMSDNtimestamp();
	return rt;
}

VARIANT comauto::createVariantType( const types::BigNumber& dt)
{
	VARIANT rt;
	if (dt.scale() < 0 || dt.scale() > 28 || dt.size() > 28)
	{
		throw std::runtime_error("cannot convert bignumber value to MSDN decimal variant value (scale out of range)");
	}
	struct
	{
		ULONG numhi;
		ULONGLONG numlo;
	} value;
	
	types::convertBCDtoBigEndianUint( dt.digits(), dt.size(), value);
	types::Endian::reorder( value.numhi);
	types::Endian::reorder( value.numlo);

	rt.vt = VT_DECIMAL;
	rt.decVal.scale = (BYTE)dt.scale();
	rt.decVal.sign = dt.sign()?DECIMAL_NEG:0;
	rt.decVal.Hi32 = value.numhi;
	rt.decVal.Lo64 = value.numlo;
	return rt;
}

static types::BigNumber getBigNumber( const VARIANT& val)
{
	if (val.vt != VT_DECIMAL) std::logic_error("assertion failed: getBigNumber called with non DECIMAL variant type");

	enum {MaxNofDigits=40/*more than 28*/};
	unsigned char digits[ MaxNofDigits];
	struct
	{
		ULONG numhi;
		ULONGLONG numlo;
	} value;
	value.numhi = val.decVal.Hi32;
	value.numlo = val.decVal.Lo64;
	types::Endian::reorder( value.numhi);
	types::Endian::reorder( value.numlo);

	unsigned short nofdigits = (unsigned short)types::convertBigEndianUintToBCD( value, digits, MaxNofDigits);

	bool sign = (val.decVal.sign == DECIMAL_NEG);
	signed short scale = val.decVal.scale;
	return types::BigNumber( sign, nofdigits, scale, digits);
}

VARIANT comauto::createVariantType( const std::string& val, VARTYPE stringtype)
{
	return comauto::createVariantType( val.c_str(), val.size(), stringtype);
}

VARIANT comauto::createVariantType( const types::Variant& val)
{
	VARIANT rt;
	rt.vt = VT_EMPTY;
	switch (val.type())
	{
		case types::Variant::Null:	rt = comauto::createVariantType(); break;
		case types::Variant::Bool:	rt = comauto::createVariantType( val.data().value.Bool); break;
		case types::Variant::Double:	rt = comauto::createVariantType( val.data().value.Double); break;
		case types::Variant::Int:	rt = comauto::createVariantType( val.data().value.Int); break;
		case types::Variant::UInt:	rt = comauto::createVariantType( val.data().value.UInt); break;
		case types::Variant::String:	rt = comauto::createVariantType( val.charptr(), val.charsize()); break;
		case types::Variant::Custom:	rt = comauto::createVariantType( val.tostring()); break;
		case types::Variant::Timestamp:	rt = comauto::createVariantType( types::DateTime( val.totimestamp())); break;
		case types::Variant::BigNumber:	rt = comauto::createVariantType( *val.bignumref()); break;
	}
	return rt;
}

VARIANT comauto::createVariantType( const types::Variant& val, VARTYPE dsttype)
{
	VARIANT rt;
	rt.vt = VT_EMPTY;
	switch (val.type())
	{
		case types::Variant::Null:	rt = comauto::createVariantType(); break;
		case types::Variant::Bool:	rt = comauto::createVariantType( val.data().value.Bool); break;
		case types::Variant::Double:	rt = comauto::createVariantType( val.data().value.Double); break;
		case types::Variant::Int:	rt = comauto::createVariantType( val.data().value.Int); break;
		case types::Variant::UInt:	rt = comauto::createVariantType( val.data().value.UInt); break;
		case types::Variant::String:	rt = comauto::createVariantType( val.charptr(), val.charsize(), (dsttype == VT_LPWSTR || dsttype == VT_LPSTR)?dsttype:VT_BSTR); break;
		case types::Variant::Custom:	rt = comauto::createVariantType( val.tostring(), (dsttype == VT_LPWSTR || dsttype == VT_LPSTR)?dsttype:VT_BSTR); break;
		case types::Variant::Timestamp:	rt = comauto::createVariantType( types::DateTime( val.totimestamp())); break;
		case types::Variant::BigNumber:	rt = comauto::createVariantType( *val.bignumref()); break;
	}
	if (rt.vt != dsttype)
	{
		WRAP( comauto::wrapVariantChangeType( &rt, &rt, 0, dsttype))
	}
	return rt;
}

VARIANT comauto::createVariantArray( VARTYPE vt, const IRecordInfo* recinfo, const std::vector<VARIANT>& ar)
{
	VARIANT rt;
	rt.vt = VT_EMPTY;
	SAFEARRAY* pa = NULL;
	try
	{
		LONG ii,nn;
		SAFEARRAYBOUND bd;
		bd.cElements = ar.size();
		bd.lLbound = 0;

		if (comauto::isAtomicType( vt))
		{
			pa = ::SafeArrayCreate( vt, 1, &bd);
			if (!pa) throw std::bad_alloc();

			for (ii=0,nn=ar.size(); ii<nn; ++ii)
			{
				if (vt != ar[ii].vt) throw std::logic_error( "internal: try to create mixed type array");
				WRAP( ::SafeArrayPutElement( pa, &ii, const_cast<void*>(comauto::arithmeticTypeAddress( &ar[ii]))))
			}
			rt.vt = VT_ARRAY | vt;
			rt.parray = pa;
		}
		else if (comauto::isStringType( vt))
		{
			pa = ::SafeArrayCreate( vt, 1, &bd);
			if (!pa) throw std::bad_alloc();

			switch (vt)
			{
				case VT_LPSTR:
					for (ii=0,nn=ar.size(); ii<nn; ++ii)
					{
						if (vt != ar[ii].vt) throw std::logic_error( "internal: try to create mixed type array");
						LPSTR* bs = &V_LPSTR( const_cast<VARIANT*>(&ar[ii]));
						WRAP( ::SafeArrayPutElement( pa, &ii, bs))
					}
					break;
				case VT_LPWSTR:
					for (ii=0,nn=ar.size(); ii<nn; ++ii)
					{
						if (vt != ar[ii].vt) throw std::logic_error( "internal: try to create mixed type array");
						LPWSTR* bs = &V_LPWSTR( const_cast<VARIANT*>(&ar[ii]));
						WRAP( ::SafeArrayPutElement( pa, &ii, bs))
					}
					break;
				case VT_BSTR:
					for (ii=0,nn=ar.size(); ii<nn; ++ii)
					{
						if (vt != ar[ii].vt) throw std::logic_error( "internal: try to create mixed type array");
						BSTR bs = V_BSTR( const_cast<VARIANT*>(&ar[ii]));
						WRAP( ::SafeArrayPutElement( pa, &ii, bs))
					}
					break;
				default:
					throw std::logic_error("internal: unknown string type");
			}
			rt.vt = VT_ARRAY | vt;
			rt.parray = pa;
		}
		else if (vt == VT_RECORD)
		{
			pa = ::SafeArrayCreateEx( VT_RECORD, 1, &bd, const_cast<IRecordInfo*>(recinfo));
			if (!pa) throw std::bad_alloc();

			for (ii=0,nn=ar.size(); ii<nn; ++ii)
			{
				if (vt != ar[ii].vt) throw std::logic_error( "internal: try to create mixed type array");
				WRAP( ::SafeArrayPutElement( pa, &ii, const_cast<void*>(ar[ii].pvRecord)))
			}
			rt.vt = VT_ARRAY | VT_RECORD;
			rt.parray = pa;
		}
		else
		{
			throw std::runtime_error( "cannot create array of this type");
		}
		return rt;
	}
	catch (const std::runtime_error& e)
	{
		if (pa) ::SafeArrayDestroy( pa);
		throw e;
	}
}


HRESULT comauto::wrapVariantCopy( VARIANT* pvargDest, const VARIANT* pvargSrc)
{
	std::size_t size;

	switch (pvargSrc->vt)
	{
		case VT_LPSTR:
			comauto::wrapVariantClear( pvargDest);
			size = std::strlen(V_LPSTR( pvargSrc)) + 1;
			V_LPSTR( pvargDest) = (LPSTR)comauto::allocMem( size);
			if (V_LPSTR( pvargDest) == NULL) throw std::bad_alloc();
			::memcpy( V_LPSTR( pvargDest), V_LPSTR( pvargSrc), size);
			return S_OK;

		case VT_LPWSTR:
			comauto::wrapVariantClear( pvargDest);
			size = std::wcslen( V_LPWSTR( pvargSrc))+sizeof(wchar_t);
			V_LPWSTR( pvargDest) = (LPWSTR)comauto::allocMem( size);
			if (V_LPWSTR( pvargDest) == NULL) throw std::bad_alloc();
			::memcpy( V_LPWSTR( pvargDest), V_LPWSTR( pvargSrc), size);
			return S_OK;

		default:
			return ::VariantCopy( pvargDest, pvargSrc);
	}
}

HRESULT comauto::wrapVariantClear( VARIANT* pvarg)
{
	HRESULT rt = S_OK;
	switch (pvarg->vt)
	{
		case VT_LPSTR:
			if (V_LPSTR( pvarg) != NULL) comauto::freeMem( V_LPSTR( pvarg));
			V_LPSTR( pvarg) = NULL;
			pvarg->vt = VT_EMPTY;
			rt = S_OK;
			break;

		case VT_LPWSTR:
			if (V_LPWSTR( pvarg) != NULL) comauto::freeMem( V_LPWSTR( pvarg));
			V_LPWSTR( pvarg) = NULL;
			pvarg->vt = VT_EMPTY;
			rt = S_OK;
			break;

		default:
			rt = ::VariantClear( pvarg);
			pvarg->vt = VT_EMPTY;
			break;
	}
	return rt;
}

HRESULT comauto::wrapVariantChangeType( VARIANT* pvargDest, const VARIANT* pvargSrc, unsigned short wFlags, VARTYPE vt)
{
	enum Conversion
	{
		C_COPY,
		C_LPSTR_LPWSTR,
		C_LPWSTR_LPSTR,
		C_BSTR_LPWSTR,
		C_BSTR_LPSTR,
		C_LPSTR_BSTR,
		C_LPWSTR_BSTR,
		C_OTHER
	};
	Conversion conversion = C_OTHER;
	switch (vt)
	{
		case VT_LPSTR:
			switch (pvargSrc->vt)
			{
				case VT_LPSTR:		conversion = C_COPY; break;
				case VT_LPWSTR:		conversion = C_LPSTR_LPWSTR; break;
				default:		conversion = C_LPSTR_BSTR; break;
			}
		case VT_LPWSTR:
			switch (pvargSrc->vt)
			{
				case VT_LPSTR:		conversion = C_LPWSTR_LPSTR; break;
				case VT_LPWSTR:		conversion = C_COPY; break;
				default:		conversion = C_LPWSTR_BSTR; break;
			}
		default:
			switch (pvargSrc->vt)
			{
				case VT_LPSTR:		conversion = C_BSTR_LPSTR; break;
				case VT_LPWSTR:		conversion = C_BSTR_LPWSTR; break;
				default:		conversion = C_OTHER; break;
			}
	}
	switch (conversion)
	{
		case C_COPY:
			if (pvargDest != pvargSrc)
			{
				comauto::wrapVariantClear( pvargDest);
				comauto::wrapVariantCopy( pvargDest, pvargSrc);
			}
			break;
		case C_LPSTR_LPWSTR:
		{
			LPSTR dest = comauto::createLPSTR( comauto::utf8string( V_LPWSTR( pvargSrc)));
			comauto::wrapVariantClear( pvargDest);
			V_LPSTR( pvargDest) = dest;
			pvargDest->vt = VT_LPSTR;
			break;
		}
		case C_LPWSTR_LPSTR:
		{
			LPWSTR dest = comauto::createLPWSTR( comauto::utf16string( V_LPSTR( pvargSrc)));
			comauto::wrapVariantClear( pvargDest);
			V_LPWSTR( pvargDest) = dest;
			pvargDest->vt = VT_LPWSTR;
			break;
		}
		case C_BSTR_LPWSTR:
		{
			BSTR dest = comauto::createBSTR( V_LPWSTR( pvargSrc));
			comauto::wrapVariantClear( pvargDest);
			V_BSTR( pvargDest) = dest;
			pvargDest->vt = VT_BSTR;
			break;
		}
		case C_BSTR_LPSTR:
		{
			BSTR dest = comauto::createBSTR( V_LPSTR( pvargSrc));
			comauto::wrapVariantClear( pvargDest);
			V_BSTR( pvargDest) = dest;
			pvargDest->vt = VT_BSTR;
			break;
		}
		case C_LPSTR_BSTR:
		{
			LPSTR dest = comauto::createLPSTR( comauto::utf8string( V_BSTR( pvargSrc)));
			comauto::wrapVariantClear( pvargDest);
			V_LPSTR( pvargDest) = dest;
			pvargDest->vt = VT_LPSTR;
			break;
		}
		case C_LPWSTR_BSTR:
		{
			if (pvargDest != pvargSrc)
			{
				comauto::wrapVariantClear( pvargDest);
				pvargDest->vt = VT_LPWSTR;
				V_LPWSTR(pvargDest) = comauto::createLPWSTR( V_LPWSTR( pvargSrc));
			}
			else
			{
				pvargDest->vt = VT_LPWSTR;
				V_LPWSTR(pvargDest) = (LPWSTR)V_BSTR(pvargDest);
			}
			break;
		}
		case C_OTHER:
			break;
	}
	if (vt != pvargDest->vt)
	{
		return ::VariantChangeType( pvargDest, pvargDest, wFlags, vt);
	}
	return S_OK;
}
 
HRESULT comauto::wrapVariantCopyInd( VARIANT* pvargDest, const VARIANT* pvargSrc)
{
	if ((pvargSrc->vt & VT_BYREF) == VT_BYREF)
	{
		VARTYPE vt = pvargSrc->vt - VT_BYREF;
		VARIANT vargSrc;
		vargSrc.vt = VT_EMPTY;

		switch (vt)
		{
			case VT_LPSTR:
				V_LPSTR( &vargSrc) = (LPSTR)pvargSrc->byref;
				return comauto::wrapVariantCopy( pvargDest, &vargSrc);
			case VT_LPWSTR:
				V_LPWSTR( &vargSrc) = (LPWSTR)pvargSrc->byref;
				return comauto::wrapVariantCopy( pvargDest, &vargSrc);
			default:
				return ::VariantCopyInd( pvargDest, pvargSrc);
		}
	}
	return ::VariantCopyInd( pvargDest, pvargSrc);
}


void comauto::copyVariantType( VARTYPE dsttype, void* dstfield, const types::Variant& val)
{
	if (dsttype == VT_BSTR)
	{
		VARIANT dstcp = comauto::createVariantType( val, dsttype);
		if (*((BSTR*)dstfield) != NULL) ::SysFreeString( *((BSTR*)dstfield));
		*((BSTR*)dstfield) = dstcp.bstrVal;
		dstcp.vt = VT_EMPTY;
	}
	else if (dsttype == VT_LPSTR)
	{
		VARIANT dstcp = comauto::createVariantType( val, dsttype);
		if (*((LPSTR*)dstfield) != NULL) comauto::freeMem( *((LPSTR*)dstfield));
		*((LPSTR*)dstfield) = V_LPSTR(&dstcp);
		dstcp.vt = VT_EMPTY;
	}
	else if (dsttype == VT_LPWSTR)
	{
		VARIANT dstcp = comauto::createVariantType( val, dsttype);
		if (*((LPWSTR*)dstfield) != NULL) comauto::freeMem( *((LPWSTR*)dstfield));
		*((LPWSTR*)dstfield) = V_LPWSTR(&dstcp);
		dstcp.vt = VT_EMPTY;
	}
	else if (comauto::isAtomicType( dsttype))
	{
		VARIANT dstcp = comauto::createVariantType( val, dsttype);
		::memcpy( dstfield, comauto::arithmeticTypeAddress( &dstcp), comauto::sizeofAtomicType( dsttype));
	}
	else
	{
		throw std::runtime_error( "cannot copy this type");
	}
}

unsigned char comauto::sizeofAtomicType( int vt)
{
	struct AtomicTypes
	{
		unsigned char ar[80];
		AtomicTypes()
		{
			std::memset( ar, 0, sizeof(ar));
			ar[ VT_I2] = 2;
			ar[ VT_I4] = 4;
			ar[ VT_R4] = 4;
			ar[ VT_R8] = 8;
			ar[ VT_CY] = sizeof(CY);
			ar[ VT_DATE] = sizeof(DATE);
			ar[ VT_BOOL] = sizeof(BOOL);
			ar[ VT_DECIMAL] = sizeof(DECIMAL);
			ar[ VT_I1] = 1;
			ar[ VT_UI1] = 1;
			ar[ VT_UI2] = 2;
			ar[ VT_UI4] = 4;
			ar[ VT_I8] = 8;
			ar[ VT_UI8] = 8;
			ar[ VT_INT] = sizeof(INT);
			ar[ VT_UINT] = sizeof(UINT);
			ar[ VT_HRESULT] = sizeof(HRESULT);
		}

		char operator[]( int i) const
		{
			if (i >= 0 && i <= sizeof(ar)) return ar[i];
			return 0;
		}
	};
	static AtomicTypes at;
	return at[vt];
}

bool comauto::isAtomicType( int vt)
{
	return comauto::sizeofAtomicType(vt)!=0;
}

bool comauto::isStringType( int vt)
{
	return vt == VT_BSTR || vt == VT_LPSTR || vt == VT_LPWSTR;
}

void* comauto::arithmeticTypeAddress( VARIANT* val)
{
	if (val->vt == VT_DECIMAL) return (void*)val;
	return (void*)((char*)val + 4*sizeof(short));
}

const void* comauto::arithmeticTypeAddress( const VARIANT* val)
{
	if (val->vt == VT_DECIMAL) return (const void*)val;
	return (const void*)((const char*)val + 4*sizeof(short));
}

std::string comauto::variablename( const ITypeInfo* typeinfo, VARDESC* vardesc)
{
	std::string rt;
	BSTR vv;
	UINT nn;
	WRAP( const_cast<ITypeInfo*>(typeinfo)->GetNames( vardesc->memid, &vv, 1, &nn))
	rt = comauto::utf8string(vv);
	::SysFreeString( vv);
	return rt;
}

std::wstring comauto::variablename_utf16( const ITypeInfo* typeinfo, VARDESC* vardesc)
{
	std::wstring rt;
	BSTR vv;
	UINT nn;
	WRAP( const_cast<ITypeInfo*>(typeinfo)->GetNames( vardesc->memid, &vv, 1, &nn))
	rt = std::wstring(vv?vv:L"");
	::SysFreeString( vv);
	return rt;
}

std::string comauto::variabletype( const ITypeInfo* typeinfo, VARDESC* vardesc)
{
	std::string rt;
	ELEMDESC ed = vardesc->elemdescVar;
	rt = comauto::typestr( typeinfo, &ed.tdesc);
	return rt;
}

types::Variant comauto::getAtomicElement( VARTYPE vt, const void* ref)
{
	switch (vt)
	{
		case VT_I1:  return types::Variant( (types::Variant::Data::Int)(*(const SHORT*)ref)); 
		case VT_I2:  return types::Variant( (types::Variant::Data::Int)(*(const SHORT*)ref));
		case VT_I4:  return types::Variant( (types::Variant::Data::Int)(*(const LONG*)ref));
		case VT_I8:  return types::Variant( (types::Variant::Data::Int)(*(const LONGLONG*)ref));
		case VT_INT: return types::Variant( (types::Variant::Data::Int)(*(const INT*)ref));
		case VT_UI1:  return types::Variant( (types::Variant::Data::UInt)(*(const USHORT*)ref));
		case VT_UI2:  return types::Variant( (types::Variant::Data::UInt)(*(const USHORT*)ref));
		case VT_UI4:  return types::Variant( (types::Variant::Data::UInt)(*(const ULONG*)ref));
		case VT_UI8:  return types::Variant( (types::Variant::Data::UInt)(*(const ULONGLONG*)ref));
		case VT_UINT: return types::Variant( (types::Variant::Data::UInt)(*(const UINT*)ref));
		case VT_R4:  return types::Variant( (double)(*(const FLOAT*)ref));
		case VT_R8:  return types::Variant( (double)(*(const DOUBLE*)ref));
		case VT_BOOL: return types::Variant( (bool)(*(const VARIANT_BOOL*)ref != VARIANT_FALSE));
		case VT_BSTR: return types::Variant( comauto::utf8string( *(const BSTR*)ref));
		case VT_LPSTR: return types::Variant( comauto::utf8string( *(LPCSTR*)ref));
		case VT_LPWSTR: return types::Variant( comauto::utf8string( *(LPCWSTR*)ref));
		case VT_DATE: return types::Variant( types::DateTime::fromMSDNtimestamp( *(DATE*)ref).timestamp());
		default:
		{
			VARIANT elemorig;
			VARIANT elemcopy;
			try
			{
				elemorig.byref = const_cast<void*>(ref);
				elemorig.vt = vt | VT_BYREF;
				WRAP( comauto::wrapVariantCopyInd( &elemcopy, &elemorig))
				WRAP( comauto::wrapVariantChangeType( &elemcopy, &elemcopy, 0, VT_BSTR))
				return types::Variant( comauto::utf8string( elemcopy.bstrVal));
			}
			catch (const std::runtime_error& e)
			{
				comauto::wrapVariantClear( &elemorig);
				comauto::wrapVariantClear( &elemcopy);
				throw e;
			}
		}
	}
}

types::Variant comauto::getAtomicElement( const VARIANT& val)
{
	if (val.vt == VT_DECIMAL)
	{
		return types::Variant( getBigNumber( val));
	}
	else
	{
		return getAtomicElement( val.vt, comauto::arithmeticTypeAddress( &val));
	}
}

