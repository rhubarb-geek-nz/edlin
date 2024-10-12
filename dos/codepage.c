/************************************
 * Copyright (c) 2024 Roger Brown.
 * Licensed under the MIT License.
 */

#ifdef _WIN32
#	include <windows.h>
#else
#	ifdef __OS2__
#		define INCL_DOSNLS
#		define INCL_DOSMISC
#		include <os2.h>
#	else
#		include <dos.h>
#	endif
#endif

#include <stdlib.h>
#include <string.h>
#include "mbcs.h"

unsigned int mbcsCodePage(void)
{
#ifdef _WIN32
	return GetACP();
#else
	unsigned int codePage = 437;
#	ifdef __OS2__
#		ifdef M_I386
	{
		ULONG codePages[8];
		ULONG dataLength = sizeof(codePages)/sizeof(codePages[0]);
		if (!DosQueryCp(sizeof(codePages), codePages, &dataLength))
		{
			codePage = codePages[0];
		}
	}
#		else
	{
		USHORT codePages[8];
		USHORT dataLength = sizeof(codePages)/sizeof(codePages[0]);
		if (!DosGetCp(sizeof(codePages), codePages, &dataLength))
		{
			codePage = codePages[0];
		}
	}
#		endif
#	else
	union REGS in_regs, out_regs;
	memset(&in_regs, 0, sizeof(in_regs));
	memset(&out_regs, 0, sizeof(out_regs));
	in_regs.h.ah = 0x66;
	in_regs.h.al = 1;
	intdos(&in_regs, &out_regs);
	if (out_regs.w.bx && !out_regs.w.cflag)
	{
		codePage = out_regs.w.bx;
	}
#	endif
	return codePage;
#endif
}
