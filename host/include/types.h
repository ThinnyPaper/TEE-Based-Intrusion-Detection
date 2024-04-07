/* types.h - some common typedefs
 *	Copyright (C) 1998 Free Software Foundation, Inc.
 *
 * This file was part of GNUPG.
 *
 * GNUPG is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GNUPG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifndef _TYPES_H_INCLUDED
#define _TYPES_H_INCLUDED

#include <sys/types.h>


  typedef unsigned char byte;

/*
#ifndef HAVE_USHORT
#  undef ushort    
  typedef unsigned short ushort;
#  define HAVE_USHORT
#endif
*/
/*
#ifndef HAVE_ULONG
#  undef ulong	   
  typedef unsigned long ulong;
#  define HAVE_ULONG
#endif
*/

typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef union {
    int a;
    short b;
    char c[1];
    long d;
    u64 e;
    float f;
    double g;
} PROPERLY_ALIGNED_TYPE;

typedef struct string_list {
    struct string_list *next;
    unsigned int flags;
    char d[1];
} *STRLIST;


#endif /*_INCLUDED_TYPES_H*/
