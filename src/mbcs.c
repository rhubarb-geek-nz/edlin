/************************************
 * Copyright (c) 2024 Roger Brown.
 * Licensed under the MIT License.
 */

#ifdef _WIN32
#	include <windows.h>
#else
#endif
#include <stdio.h>
#include <stdlib.h>
#include "mbcs.h"

#ifdef MBCS_LOOKUP
static const struct
{
	const int codePage;
	const wchar_t table[0x80];
} codePageList[] = { { 437,{
0x00C7,0x00FC,0x00E9,0x00E2,0x00E4,0x00E0,0x00E5,0x00E7,0x00EA,0x00EB,0x00E8,0x00EF,0x00EE,0x00EC,0x00C4,0x00C5,
0x00C9,0x00E6,0x00C6,0x00F4,0x00F6,0x00F2,0x00FB,0x00F9,0x00FF,0x00D6,0x00DC,0x00A2,0x00A3,0x00A5,0x20A7,0x0192,
0x00E1,0x00ED,0x00F3,0x00FA,0x00F1,0x00D1,0x00AA,0x00BA,0x00BF,0x2310,0x00AC,0x00BD,0x00BC,0x00A1,0x00AB,0x00BB,
0x2591,0x2592,0x2593,0x2502,0x2524,0x2561,0x2562,0x2556,0x2555,0x2563,0x2551,0x2557,0x255D,0x255C,0x255B,0x2510,
0x2514,0x2534,0x252C,0x251C,0x2500,0x253C,0x255E,0x255F,0x255A,0x2554,0x2569,0x2566,0x2560,0x2550,0x256C,0x2567,
0x2568,0x2564,0x2565,0x2559,0x2558,0x2552,0x2553,0x256B,0x256A,0x2518,0x250C,0x2588,0x2584,0x258C,0x2590,0x2580,
0x03B1,0x00DF,0x0393,0x03C0,0x03A3,0x03C3,0x00B5,0x03C4,0x03A6,0x0398,0x03A9,0x03B4,0x221E,0x03C6,0x03B5,0x2229,
0x2261,0x00B1,0x2265,0x2264,0x2320,0x2321,0x00F7,0x2248,0x00B0,0x2219,0x00B7,0x221A,0x207F,0x00B2,0x25A0,0x00A0} },
{ 850,{
0x00C7,0x00FC,0x00E9,0x00E2,0x00E4,0x00E0,0x00E5,0x00E7,0x00EA,0x00EB,0x00E8,0x00EF,0x00EE,0x00EC,0x00C4,0x00C5,
0x00C9,0x00E6,0x00C6,0x00F4,0x00F6,0x00F2,0x00FB,0x00F9,0x00FF,0x00D6,0x00DC,0x00F8,0x00A3,0x00D8,0x00D7,0x0192,
0x00E1,0x00ED,0x00F3,0x00FA,0x00F1,0x00D1,0x00AA,0x00BA,0x00BF,0x00AE,0x00AC,0x00BD,0x00BC,0x00A1,0x00AB,0x00BB,
0x2591,0x2592,0x2593,0x2502,0x2524,0x00C1,0x00C2,0x00C0,0x00A9,0x2563,0x2551,0x2557,0x255D,0x00A2,0x00A5,0x2510,
0x2514,0x2534,0x252C,0x251C,0x2500,0x253C,0x00E3,0x00C3,0x255A,0x2554,0x2569,0x2566,0x2560,0x2550,0x256C,0x00A4,
0x00F0,0x00D0,0x00CA,0x00CB,0x00C8,0x0131,0x00CD,0x00CE,0x00CF,0x2518,0x250C,0x2588,0x2584,0x00A6,0x00CC,0x2580,
0x00D3,0x00DF,0x00D4,0x00D2,0x00F5,0x00D5,0x00B5,0x00FE,0x00DE,0x00DA,0x00DB,0x00D9,0x00FD,0x00DD,0x00AF,0x00B4,
0x00AD,0x00B1,0x2017,0x00BE,0x00B6,0x00A7,0x00F7,0x00B8,0x00B0,0x00A8,0x00B7,0x00B9,0x00B3,0x00B2,0x25A0,0x00A0} },
{ 863,{
0x00C7,0x00FC,0x00E9,0x00E2,0x00C2,0x00E0,0x00B6,0x00E7,0x00EA,0x00EB,0x00E8,0x00EF,0x00EE,0x2017,0x00C0,0x00A7,
0x00C9,0x00C8,0x00CA,0x00F4,0x00CB,0x00CF,0x00FB,0x00F9,0x00A4,0x00D4,0x00DC,0x00A2,0x00A3,0x00D9,0x00DB,0x0192,
0x00A6,0x00B4,0x00F3,0x00FA,0x00A8,0x00B8,0x00B3,0x00AF,0x00CE,0x2310,0x00AC,0x00BD,0x00BC,0x00BE,0x00AB,0x00BB,
0x2591,0x2592,0x2593,0x2502,0x2524,0x2561,0x2562,0x2556,0x2555,0x2563,0x2551,0x2557,0x255D,0x255C,0x255B,0x2510,
0x2514,0x2534,0x252C,0x251C,0x2500,0x253C,0x255E,0x255F,0x255A,0x2554,0x2569,0x2566,0x2560,0x2550,0x256C,0x2567,
0x2568,0x2564,0x2565,0x2559,0x2558,0x2552,0x2553,0x256B,0x256A,0x2518,0x250C,0x2588,0x2584,0x258C,0x2590,0x2580,
0x03B1,0x00DF,0x0393,0x03C0,0x03A3,0x03C3,0x00B5,0x03C4,0x03A6,0x0398,0x03A9,0x03B4,0x221E,0x03C6,0x03B5,0x2229,
0x2261,0x00B1,0x2265,0x2264,0x2320,0x2321,0x00F7,0x2248,0x00B0,0x2219,0x00B7,0x221A,0x207F,0x00B2,0x25A0,0x00A0} },
{ 865,{
0x00C7,0x00FC,0x00E9,0x00E2,0x00E4,0x00E0,0x00E5,0x00E7,0x00EA,0x00EB,0x00E8,0x00EF,0x00EE,0x00EC,0x00C4,0x00C5,
0x00C9,0x00E6,0x00C6,0x00F4,0x00F6,0x00F2,0x00FB,0x00F9,0x00FF,0x00D6,0x00DC,0x00F8,0x00A3,0x00D8,0x20A7,0x0192,
0x00E1,0x00ED,0x00F3,0x00FA,0x00F1,0x00D1,0x00AA,0x00BA,0x00BF,0x2310,0x00AC,0x00BD,0x00BC,0x00A1,0x00AB,0x00A4,
0x2591,0x2592,0x2593,0x2502,0x2524,0x2561,0x2562,0x2556,0x2555,0x2563,0x2551,0x2557,0x255D,0x255C,0x255B,0x2510,
0x2514,0x2534,0x252C,0x251C,0x2500,0x253C,0x255E,0x255F,0x255A,0x2554,0x2569,0x2566,0x2560,0x2550,0x256C,0x2567,
0x2568,0x2564,0x2565,0x2559,0x2558,0x2552,0x2553,0x256B,0x256A,0x2518,0x250C,0x2588,0x2584,0x258C,0x2590,0x2580,
0x03B1,0x00DF,0x0393,0x03C0,0x03A3,0x03C3,0x00B5,0x03C4,0x03A6,0x0398,0x03A9,0x03B4,0x221E,0x03C6,0x03B5,0x2229,
0x2261,0x00B1,0x2265,0x2264,0x2320,0x2321,0x00F7,0x2248,0x00B0,0x2219,0x00B7,0x221A,0x207F,0x00B2,0x25A0,0x00A0} },
{ 1252,{
0x20AC,0x0081,0x201A,0x0192,0x201E,0x2026,0x2020,0x2021,0x02C6,0x2030,0x0160,0x2039,0x0152,0x008D,0x017D,0x008F,
0x0090,0x2018,0x2019,0x201C,0x201D,0x2022,0x2013,0x2014,0x02DC,0x2122,0x0161,0x203A,0x0153,0x009D,0x017E,0x0178,
0x00A0,0x00A1,0x00A2,0x00A3,0x00A4,0x00A5,0x00A6,0x00A7,0x00A8,0x00A9,0x00AA,0x00AB,0x00AC,0x00AD,0x00AE,0x00AF,
0x00B0,0x00B1,0x00B2,0x00B3,0x00B4,0x00B5,0x00B6,0x00B7,0x00B8,0x00B9,0x00BA,0x00BB,0x00BC,0x00BD,0x00BE,0x00BF,
0x00C0,0x00C1,0x00C2,0x00C3,0x00C4,0x00C5,0x00C6,0x00C7,0x00C8,0x00C9,0x00CA,0x00CB,0x00CC,0x00CD,0x00CE,0x00CF,
0x00D0,0x00D1,0x00D2,0x00D3,0x00D4,0x00D5,0x00D6,0x00D7,0x00D8,0x00D9,0x00DA,0x00DB,0x00DC,0x00DD,0x00DE,0x00DF,
0x00E0,0x00E1,0x00E2,0x00E3,0x00E4,0x00E5,0x00E6,0x00E7,0x00E8,0x00E9,0x00EA,0x00EB,0x00EC,0x00ED,0x00EE,0x00EF,
0x00F0,0x00F1,0x00F2,0x00F3,0x00F4,0x00F5,0x00F6,0x00F7,0x00F8,0x00F9,0x00FA,0x00FB,0x00FC,0x00FD,0x00FE,0x00FF} } };
#endif

int mbcsLen(unsigned int cp, const unsigned char* p, size_t avail)
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

wchar_t mbcsToChar(unsigned int cp, const unsigned char* p, int len)
{
	const unsigned char c = p[0];

	if (c < 0x80) return c;

	if (cp == CP_UTF8)
	{
		if (c < 0xC0) return ~0;
		if (c < 0xE0) return ((c & 0x1f) << 6) | (p[1] & 0x3F);
#ifdef __WATCOMC__
		return ((c & 0xf) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F);
#else
		if (c < 0xF0) return ((c & 0xf) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F);
		return ((c & 0x7) << 18) | ((p[1] & 0x3F) << 12) | ((p[2] & 0x3F) << 6) | (p[3] & 0x3F);
#endif
	}
#ifdef MBCS_LOOKUP
	else
	{
		int k = sizeof(codePageList) / sizeof(codePageList[0]);

		while (k--)
		{
			if (codePageList[k].codePage == cp)
			{
				return codePageList[k].table[c - 0x80];
			}
		}
	}
#endif

	return c;
}

int mbcsFromChar(unsigned int cp, wchar_t ch, unsigned char* p)
{
	if (ch < 0x80)
	{
		*p = (char)ch;
		return 1;
	}

	if (cp == CP_UTF8)
	{
		if (ch < 0x800)
		{
			p[0] = 0xC0 | (ch >> 6);
			p[1] = 0x80 | (ch & 0x3F);

			return 2;
		}

#ifdef __WATCOMC__
		p[0] = 0xE0 | (ch >> 12);
		p[1] = 0x80 | ((ch >> 6) & 0x3F);
		p[2] = 0x80 | (ch & 0x3F);

		return 3;
#else
		if (ch < 0x40000)
		{
			p[0] = 0xE0 | (ch >> 12);
			p[1] = 0x80 | ((ch >> 6) & 0x3F);
			p[2] = 0x80 | (ch & 0x3F);

			return 3;
		}

		p[0] = 0xF0 | (((unsigned int)ch) >> 18);
		p[1] = 0x80 | ((ch >> 12) & 0x3F);
		p[2] = 0x80 | ((ch >> 6) & 0x3F);
		p[3] = 0x80 | (ch & 0x3F);

		return 4;
#endif
	}
#ifdef MBCS_LOOKUP
	else
	{
		int k = sizeof(codePageList) / sizeof(codePageList[0]);

		while (k--)
		{
			if (codePageList[k].codePage == cp)
			{
				const wchar_t* table = codePageList[k].table;
				int i = 0;

				while (i < 0x80)
				{
					if (table[i] == ch)
					{
						ch = i + 0x80;

						break;
					}

					i++;
				}

				if (i == 0x80)
				{
					ch = 0xFFFD;
				}

				break;
			}
		}

		if ((k < 0) && (ch >= 0x80))
		{
			ch = 0xFFFD;
		}
	}
#endif

	p[0] = (ch == 0xFFFD) ? '?' : (char)ch;

	return 1;
}
