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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\brief Alternative malloc that checks for memory corruption (only for testing)
///\file malloc.cpp
#include "types/malloc.hpp"
#include <stdexcept>
#include <cstdlib>
#include <cstring>

#undef WOLFRAME_DEBUG_MALLOC
#ifdef WOLFRAME_DEBUG_MALLOC
#ifdef __GLIBC__
#include <malloc.h>
#include <dlfcn.h>
///\link http://www.gnu.org/software/libc/manual/html_node/Hooks-for-Malloc.html
///\link http://stackoverflow.com/questions/6083337/overriding-malloc-using-the-ld-preload-mechanism

typedef void* (*real_malloc_f)(size_t);
static void* (*real_malloc)(size_t)=0;
typedef void* (*real_realloc_f)(void*,size_t);
static void* (*real_realloc)(void*,size_t)=0;
typedef void (*real_free_f)(void*);
static void (*real_free)(void*)=0;

extern "C" void* __libc_calloc( size_t, size_t);
extern "C" void* __libc_malloc( size_t);
extern "C" void* __libc_realloc( void*, size_t);
extern "C" void __libc_free( void*);

static void real_malloc_initialize()
{
	real_malloc = (real_malloc_f)(dlsym( RTLD_NEXT, "malloc"));
	if (!real_malloc)
	{
		throw std::runtime_error( std::string("error in dlsym:") + dlerror());
		return;
	}
	real_realloc = (real_realloc_f)dlsym( RTLD_NEXT, "realloc");
	if (!real_realloc)
	{
		throw std::runtime_error( std::string("error in dlsym:") + dlerror());
		return;
	}
	real_free = (real_free_f)dlsym( RTLD_NEXT, "free");
	if (!real_free)
	{
		throw std::runtime_error( std::string("error in dlsym:") + dlerror());
		return;
	}
}

static void* (*system_malloc)( size_t size, const void *caller) = 0;
static void  (*system_free)( void *ptr, const void *caller) = 0;
static void* (*system_realloc)( void *ptr, size_t size, const void *caller) = 0;
static void  (*system_initialize)() = 0;

#define SYSTEM_MALLOC(N)	(*real_malloc)(N)
#define SYSTEM_REALLOC(P,N)	(*real_realloc)(P,N)
#define SYSTEM_FREE(P)		(*real_free)(P)

static void* this_malloc( size_t size, const void*)
{
	return wolframe_malloc( size);
}
static void* this_realloc( void *ptr, size_t size, const void*)
{
	return wolframe_realloc( ptr, size);
}
static void this_free( void *ptr, const void*)
{
	wolframe_free( ptr);
}
static void this_initialize()
{
	system_malloc = __malloc_hook;
	system_realloc = __realloc_hook;
	system_free = __free_hook;

	__malloc_hook = this_malloc;
	__realloc_hook = this_realloc;
	__free_hook = this_free;

	real_malloc_initialize();
}
///\brief Installs the malloc library hooks
void (*volatile __malloc_initialize_hook)() = &this_initialize;

#else
#define SYSTEM_MALLOC(size)		std::malloc(size)
#define SYSTEM_FREE(ptr)		std::free(ptr)
#define SYSTEM_REALLOC(ptr,size)	std::realloc(ptr,size)
#endif
#else
#define SYSTEM_MALLOC(size)		std::malloc(size)
#define SYSTEM_FREE(ptr)		std::free(ptr)
#define SYSTEM_REALLOC(ptr,size)	std::realloc(ptr,size)
#endif

struct MemChunkHeader
{
	size_t size;
	size_t ref;
	size_t chk;
};

void* wolframe_malloc( size_t size)
{
	if (!size) return 0;
	MemChunkHeader* hdr;
	hdr = (MemChunkHeader*)SYSTEM_MALLOC( size + sizeof(MemChunkHeader));
	if (!hdr) return 0;
	hdr->size = size;
	hdr->ref = 1;
	hdr->chk = (size + sizeof(MemChunkHeader)) * 2654435761U/*knuth's integer hash*/;
	return (void*)(hdr+1);
}

void* wolframe_calloc( size_t size, size_t esize)
{
	if (!size) return 0;
	std::size_t mm = size * esize;
	if (mm < size || mm < esize) return 0;
	void* rt = wolframe_malloc( mm);
	std::memset( rt, 0, mm);
	return rt;
}

static void wolframe_deref_memhdr( MemChunkHeader* hdr)
{
	if (--hdr->ref != 0)
	{
		throw std::runtime_error( "free: invalid or double free detected");
	}
	if (hdr->chk != (hdr->size + sizeof(MemChunkHeader)) * 2654435761U/*knuth's integer hash*/)
	{
		throw std::runtime_error( "free: check of memory block checksum failed");
	}
	hdr->chk--;
}

void wolframe_free( void* ptr)
{
	if (!ptr) return;
	MemChunkHeader* hdr = (MemChunkHeader*)ptr - 1;
	wolframe_deref_memhdr( hdr);
	SYSTEM_FREE( hdr);
}

void* wolframe_realloc( void* oldptr, size_t size)
{
	MemChunkHeader* hdr = (MemChunkHeader*)SYSTEM_MALLOC( size + sizeof(MemChunkHeader));
	if (!hdr) return 0;
	hdr->size = size;
	hdr->ref = 1;
	hdr->chk = (size + sizeof(MemChunkHeader)) * 2654435761U/*knuth's integer hash*/;
	void* ptr = (void*)(hdr+1);
	if (oldptr)
	{
		MemChunkHeader* oldhdr = (MemChunkHeader*)oldptr - 1;
		std::size_t cpsize = (oldhdr->size > hdr->size)?hdr->size:oldhdr->size;
		std::memmove( ptr, oldptr, cpsize);
		wolframe_deref_memhdr( oldhdr);
		SYSTEM_FREE( oldhdr);
	}
	return ptr;
}

