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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mbcs.h>

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
#endif

#ifdef __OS2__
	unsigned int codePage = 437;
#	ifdef M_I386
	{
		ULONG codePages[8];
		ULONG dataLength = 8;
		if (!DosQueryCp(sizeof(codePages), codePages, &dataLength))
		{
			codePage = codePages[0];
		}
	}
#	else
	{
		USHORT codePages[8];
		USHORT dataLength = 8;
		if (!DosGetCp(sizeof(codePages), codePages, &dataLength))
		{
			codePage = codePages[0];
		}
	}
#	endif
#endif

	while (i < argc)
	{
		const char* p = argv[i++];
		char input[1024];
		FILE* fp = fopen(p, "r");
		if (fp == NULL)
		{
			perror(p);
			return 1;
		}

		while (fgets(input, sizeof(input), fp))
		{
			wchar_t line[1024];
			int off = 0;
			int len = (int)strlen(input);
			int lineLen = 0;
			int outLen = 0;
			char output[1024];

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
