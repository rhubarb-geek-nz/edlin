/************************************
 * Copyright (c) 2024 Roger Brown.
 * Licensed under the MIT License.
 */

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <locale.h>
#ifdef HAVE_LANGINFO_H
#	include <langinfo.h>
#endif
#include "mbcs.h"

#ifdef HAVE_LANGINFO_H
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

unsigned int mbcsCodePage(void)
{
	unsigned int codePage = 646;
#ifdef HAVE_LANGINFO_H
	const char *p;
#endif

	setlocale(LC_ALL, "");

#ifdef HAVE_LANGINFO_H
	p = nl_langinfo(CODESET);

	if (p)
	{
		int i = sizeof(codePageMap) / sizeof(codePageMap[0]);
		int k = strlen(p);
		char buf[32];

		if ((k > 4) && (k < sizeof(buf)) && !memcmp(p, "ISO-", 4))
		{
			memcpy(buf, p, 3);
			memcpy(buf+3, p + 4, k - 3);
			p = buf;
		}

		while (i--)
		{
			if (!strcmp(p, codePageMap[i].name))
			{
				codePage = codePageMap[i].codePage;
				break;
			}
		}
	}
#endif

	return codePage;
}
