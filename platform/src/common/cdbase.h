/*  Copyright 2004-2005 Theo Berkau
    Copyright 2005 Joost Peters
    Copyright 2006 Guillaume Duhamel

    This file is part of Yabause.

    Yabause is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Yabause is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Yabause; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifndef CDBASE_H
#define CDBASE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#ifdef BUILDING_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif //BUILDING_DLL
#else //!WIN32
#define DLL_EXPORT
#endif //WIN32

#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
typedef int8_t s8;
typedef uint8_t u8;
typedef int32_t s32;
typedef uint32_t u32;
#define UNUSED

#define CDCORE_DEFAULT -1
#define CDCORE_DUMMY    0
#define CDCORE_ISO      1
#define CDCORE_ARCH     2
#if 0
typedef struct
{
        int id;
        const char *Name;
        int (*Init)(const char *);
        int (*DeInit)();
        int (*GetStatus)();
        s32 (*ReadTOC)(u32 *TOC);
        int (*ReadSectorFAD)(u32 FAD, void *buffer);
} CDInterface;

extern CDInterface DummyCD;

extern CDInterface ISOCD;

extern CDInterface ArchCD;
#endif

DLL_EXPORT int GetPhysicalStatus();
DLL_EXPORT int32_t ReadToc(uint32_t* toc);
DLL_EXPORT int ReadSectorAtFad(uint32_t fad, void *buffer);
DLL_EXPORT int CdInit(const char* filename);

#ifdef __cplusplus
}
#endif

#endif
