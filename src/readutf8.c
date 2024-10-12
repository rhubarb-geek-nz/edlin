/************************************
 * Copyright (c) 2024 Roger Brown.
 * Licensed under the MIT License.
 */

#ifdef _WIN32
#	include <windows.h>
#endif

#ifdef __OS2__
#	define INCL_DOSNLS
#	include <os2.h>
#endif

#ifdef __DOS__
#	include <dos.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if !defined(_WIN32) && !defined(__OS2__) && !defined(__DOS__)
#	include <locale.h>
#	include <langinfo.h>
#endif

#include <mbcs.h>

#if !defined(_WIN32) && !defined(__OS2__) && !defined(__DOS__)
static struct
{
	unsigned int codePage;
	const char *name;
} codePageMap[] = {
	{ CP_UTF8,"UTF-8" },
	{ 850,"ISO8859-1" },
	{ 28605,"ISO8859-15" }
};
#endif

int main(int argc, char **argv)
{
	int i = 1;
#ifdef _WIN32
	UINT codePage = GetACP();
	DWORD mode;
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	if (GetConsoleMode(hStdOut, &mode))
	{
		codePage = GetConsoleOutputCP();

		if (codePage == CP_ACP)
		{
			codePage = GetACP();
		}
	}
#else
#	if defined(__OS2__) || defined(__DOS__)
	unsigned int codePage = 437;
#		ifdef __OS2__
#			ifdef M_I386
	{
		ULONG codePages[8];
		ULONG dataLength = 8;
		if (!DosQueryCp(sizeof(codePages), codePages, &dataLength))
		{
			codePage = codePages[0];
		}
	}
#			else
	{
		USHORT codePages[8];
		USHORT dataLength = 8;
		if (!DosGetCp(sizeof(codePages), codePages, &dataLength))
		{
			codePage = codePages[0];
		}
	}
#			endif
#		else
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
#		endif
#	else
	const char *cp;
	unsigned int codePage = 646;
	setlocale(LC_ALL, "");

	cp = nl_langinfo(CODESET);

	if (cp)
	{
		int i = sizeof(codePageMap) / sizeof(codePageMap[0]);
		int k = strlen(cp);
		char buf[32];

		if ((k > 4) && (k < sizeof(buf)) && !memcmp(cp, "ISO-", 4))
		{
			memcpy(buf, cp, 3);
			memcpy(buf+3, cp + 4, k - 3);
			cp = buf;
		}

		while (i--)
		{
			if (!strcmp(cp,codePageMap[i].name))
			{
				codePage = codePageMap[i].codePage;
				break;
			}
		}
	}
#	endif
#endif

	while (i < argc)
	{
		const char *p = argv[i++];
		static unsigned char input[1024];
		FILE* fp = fopen(p, "r");
		if (fp == NULL)
		{
			perror(p);
			return 1;
		}

		while (fgets((char *)input, sizeof(input), fp))
		{
			static wchar_t line[1024];
			int off = 0;
			int len = (int)strlen((char *)input);
			int lineLen = 0;
			int outLen = 0;
			static unsigned char output[1024];

			while ((off < len) && (input[off] != '\n'))
			{
				int remaining = len - off;
				int i = mbcsLen(CP_UTF8, input + off, remaining);

				if (i > remaining)
				{
					break;
				}

				line[lineLen++] = mbcsToChar(CP_UTF8, input + off, i);

				off += i;
			}

			off = 0;

			while (off < lineLen)
			{
				int i = mbcsFromChar(codePage, line[off++], output + outLen);
				outLen += i;
			}

			output[outLen] = 0;

			printf("%s\n", output);
		}

		fclose(fp);
	}

	return 0;
}
