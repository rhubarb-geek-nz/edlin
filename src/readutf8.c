/************************************
 * Copyright (c) 2024 Roger Brown.
 * Licensed under the MIT License.
 */

#ifdef _WIN32
#	include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <mbcs.h>

int main(int argc, char **argv)
{
	int i = 1;
	unsigned int codePage = mbcsCodePage();

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
