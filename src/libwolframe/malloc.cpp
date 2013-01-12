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
///\brief Alternative malloc that checks for memory corruption (only for testing)
///\file malloc.cpp
#include "types/malloc.hpp"
#ifndef _WIN32
#include <features.h>
#include <string.h>
#endif
#ifdef __GLIBC__
#include <glib.h>

///\brief Installs the malloc wrapper
///\remark Must be created before any malloc call
void init_wolframe_malloc()
{
	static GMemVTable memvtable;
	memset( &memvtable, 0, sizeof(memvtable));
	memvtable.malloc = &wolframe_malloc;
	memvtable.free = &wolframe_free;
	memvtable.realloc = &wolframe_realloc;
	g_mem_set_vtable( &memvtable);
}
#else
#error Alternative malloc not supported on this platform (only with gcc/glibc)
#endif
#include <stdexcept>

struct MemChunkHeader
{
	size_t size;
	size_t ref;
	size_t chk;
};

void* wolframe_malloc( size_t size)
{
	MemChunkHeader* hdr;
	hdr = (MemChunkHeader*)g_malloc_n( size + sizeof(MemChunkHeader), 1);
	hdr->size = size;
	hdr->ref = 1;
	hdr->chk = (size + sizeof(MemChunkHeader)) * 2654435761/*knuth's integer hash*/;
	return (void*)(hdr+1);
}

static void wolframe_deref_memhdr( MemChunkHeader* hdr)
{
	if (--hdr->ref != 0)
	{
		throw std::runtime_error( "free: invalid or double free detected");
	}
	if (hdr->chk != (hdr->size + sizeof(MemChunkHeader)) * 2654435761/*knuth's integer hash*/)
	{
		throw std::runtime_error( "free: check of memory block checksum failed");
	}
	hdr->chk--;
}

void wolframe_free( void* ptr)
{
	MemChunkHeader* hdr = (MemChunkHeader*)ptr - 1;
	wolframe_deref_memhdr( hdr);
	g_free( hdr);
}

void* wolframe_realloc( void* oldptr, size_t size)
{
	MemChunkHeader* hdr = (MemChunkHeader*)g_malloc_n( size + sizeof(MemChunkHeader), 1);
	hdr->size = size;
	hdr->ref = 1;
	hdr->chk = (size + sizeof(MemChunkHeader)) * 2654435761/*knuth's integer hash*/;
	void* ptr = (void*)(hdr+1);
	if (!oldptr)
	{
		MemChunkHeader* oldhdr = (MemChunkHeader*)ptr - 1;
		wolframe_deref_memhdr( oldhdr);
		if (oldhdr->size > hdr->size)
		{
			oldhdr->size = hdr->size;
		}
		g_memmove( ptr, oldptr, oldhdr->size);
		g_free( oldhdr);
	}
	return ptr;
}

