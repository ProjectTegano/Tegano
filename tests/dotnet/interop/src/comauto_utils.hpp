#ifndef _Wolframe_COM_AUTOMATION_UTILS_INCLUDED
#define _Wolframe_COM_AUTOMATION_UTILS_INCLUDED
#include "filter/typedfilter.hpp"
#include <cstring>
#include <sstream>
#include <iostream>
#include <string>
#include <stdexcept>
#include <oaidl.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>

namespace _Wolframe {
namespace comauto {

void* allocMem( std::size_t size);
void freeMem( void* ptr);

std::wstring utf16string( const std::string& utf8str);
std::wstring utf16string( const char* utf8ptr, std::size_t utf8size);
std::string utf8string( const std::wstring& utf16str);
std::string utf8string( LPCSTR str);
std::string utf8string( LPCWSTR str);
char* createLPSTR( const std::string& str);
char* createLPSTR( LPCSTR str);
wchar_t* createLPWSTR( const std::wstring& str);
wchar_t* createLPWSTR( LPCWSTR str);
BSTR createBSTR( LPCSTR str);
BSTR createBSTR( LPCWSTR str);
std::string utf8string( const BSTR& str);
std::wstring tostring( VARIANT* v);
std::wstring tostring( const _com_error& err);
std::string typestr( VARTYPE vt);
std::string typestr( ITypeInfo* typeinfo, const TYPEDESC* ed);
std::string structstring( ITypeInfo* typeinfo);
std::string variablename( ITypeInfo* typeinfo, VARDESC* vardesc);
std::string variabletype( ITypeInfo* typeinfo, VARDESC* vardesc);

bool isCOMInterfaceMethod( const std::string& name);

VARIANT createVariantType( bool val);
VARIANT createVariantType( int val);
VARIANT createVariantType( unsigned int val);
VARIANT createVariantType( short val);
VARIANT createVariantType( unsigned short val);
VARIANT createVariantType( long val);
VARIANT createVariantType( unsigned long val);
VARIANT createVariantType( float val);
VARIANT createVariantType( double val);
VARIANT createVariantType( const std::string& val);
VARIANT createVariantType( const char* val, std::size_t valsize, VARTYPE stringtype=VT_BSTR);
VARIANT createVariantType( const std::wstring& val);
VARIANT createVariantType( const langbind::TypedInputFilter::Element& val);
VARIANT createVariantType( const langbind::TypedInputFilter::Element& val, VARTYPE dsttype);
void copyVariantType( VARTYPE dsttype, void* dstfield, const langbind::TypedInputFilter::Element& val);
HRESULT wrapVariantCopy( VARIANT* pvargDest, const VARIANT* pvargSrc);
HRESULT wrapVariantCopyInd( VARIANT* pvargDest, const VARIANT* pvargSrc);
HRESULT wrapVariantClear( VARIANT* pvarg);
HRESULT wrapVariantChangeType( VARIANT* pvargDest, const VARIANT* pvargSrc, unsigned short wFlags, VARTYPE vt);

unsigned char sizeofAtomicType( int vt);
bool isAtomicType( int vt);
bool isStringType( int vt);
const void* arithmeticTypeAddress( const VARIANT* val);
void* arithmeticTypeAddress( VARIANT* val);

langbind::TypedInputFilter::Element getAtomicElement( VARTYPE vt, const void* ref, std::string& elembuf);
langbind::TypedInputFilter::Element getAtomicElement( const VARIANT& val, std::string& elembuf);

#define WRAP(SYSCALL){\
	static const char* call = "" #SYSCALL;\
	HRESULT hr = SYSCALL;\
	if (hr != S_OK)\
	{\
		const char* endcallname = strchr( call, '(');\
		std::string callname( call, endcallname?(endcallname-call):strlen(call));\
		_com_error error(hr);\
		std::ostringstream errcode; errcode << std::hex << "0x" << hr; \
		throw std::runtime_error( std::string("Error calling ") + callname + ": '" + comauto::utf8string(error.ErrorMessage()) + "' [" + errcode.str() + "]");\
	}}

}}
#endif
