/************************************
 * Copyright (c) 2024 Roger Brown.
 * Licensed under the MIT License.
 */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "mbcs.h"

int mbcsLen(unsigned int cp, const char* p, size_t avail)
{
	if (avail)
	{
		size_t len = 1;

		if (cp == CP_UTF8)
		{
			unsigned char c = *p;

			if (c < 0x80)
			{
				return 1;
			}

			if (c < 0xC0)
			{
				return -1;
			}

			if (c < 0xE0)
			{
				len = 2;
			}
			else
			{
				if (len < 0xF0)
				{
					len = 3;
				}
				else
				{
					len = 4;
				}
			}

			if (avail >= len)
			{
				size_t i = len;

				while (--i)
				{
					if (0x80 != (0xC0 & *++p))
					{
						return -(int)len;
					}
				}
			}
		}

		return (int)len;
	}

	return 0;
}
