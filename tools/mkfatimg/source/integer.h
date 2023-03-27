/*------------------------------------------------------------------------/
/  Integer type definitions for FatFs module                              /
/-------------------------------------------------------------------------/
/
/ Copyright (C) 2019, ChaN, all right reserved.
/
/ FatFs module is an open source software. Redistribution and use of FatFs in
/ source and binary forms, with or without modification, are permitted provided
/ that the following condition is met:
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/
/----------------------------------------------------------------------------*/

#ifndef FF_INTEGER
#define FF_INTEGER

#ifdef _WIN32	/* FatFs development platform */

#include <windows.h>
#include <tchar.h>
typedef unsigned __int64 QWORD;


#else			/* Embedded platform */

/* These types MUST be 16-bit or 32-bit */
typedef int				INT;
typedef unsigned int	UINT;

/* This type MUST be 8-bit */
typedef unsigned char	BYTE;

/* These types MUST be 16-bit */
typedef short			SHORT;
typedef unsigned short	WORD;
typedef unsigned short	WCHAR;

/* These types MUST be 32-bit */
typedef long			LONG;
typedef unsigned long	DWORD;

/* This type MUST be 64-bit (Remove this for ANSI C (C89) compatibility) */
typedef unsigned long long QWORD;

#endif

#endif
