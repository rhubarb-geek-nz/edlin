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
#include "edlin.h"
#include "readline.h"
#include "mbcs.h"
#include "edlmes.h"

static unsigned int edlinMeasure(struct edlinReadline* line, unsigned int len)
{
	unsigned int column = line->leftColumn;
	unsigned int remaining = line->lineLen;
	unsigned char* p = line->line;

	while (len)
	{
		unsigned char c = *p;

		if (*p == 9)
		{
			p++;
			remaining--;
			column = (column + line->tabWidth) & (~(line->tabWidth - 1));
			len--;
		}
		else
		{
			if ((c < 0x20) || (c == 0x7f))
			{
				p++;
				column += 2;
				remaining--;
				len--;
			}
			else
			{
				int i = mbcsLen(fileCodePage, p, remaining);
				p += i;
				remaining -= i;
				column++;
				len -= i;
			}
		}
	}

	return column;
}

void edlinPaint(struct edlinReadline* line)
{
	unsigned int column = line->leftColumn, len = line->lineLen;
	const unsigned char* p = line->line;

	while ((column < line->currentColumn) && len)
	{
		unsigned char c = *p;

		if (c == 9)
		{
			column = (column + line->tabWidth) & (~(line->tabWidth - 1));
			len--;
			p++;
		}
		else
		{
			if ((c < 0x20) || (c == 0x7f))
			{
				column += 2;
				len--;
				p++;
			}
			else
			{
				int i = mbcsLen(fileCodePage, p, len);

				if (i < 1)
				{
					abort();
				}

				column++;
				len -= i;
				p += i;
			}
		}
	}

	while (len)
	{
		unsigned char c = *p;

		if (c == 9)
		{
			column = (column + line->tabWidth) & (~(line->tabWidth - 1));
			len--;
			p++;

			while (column > line->currentColumn)
			{
				unsigned char buf[8];
				unsigned int bufLen = column - line->currentColumn;

				if (bufLen > sizeof(buf))
				{
					bufLen = sizeof(buf);
				}

				memset(buf, ' ', bufLen);

				edlinPrint(buf, bufLen);

				line->currentColumn += bufLen;
			}
		}
		else
		{
			if ((c < 0x20) || (c == 0x7f))
			{
				unsigned char buf[] = { '^','?' };

				if (c < 0x20)
				{
					buf[1] = edlinControlChar[c];
				}

				edlinPrint(buf, sizeof(buf));

				column += sizeof(buf);
				len--;
				p++;
			}
			else
			{
				int i = mbcsLen(fileCodePage, p, len);

				if (i > 0)
				{
					edlinPrint(p, i);
					column++;
					len -= i;
					p += i;
				}
				else
				{
					abort();
				}
			}

			line->currentColumn = column;
		}
	}
}

static int edlinConsume(struct edlinReadline* line, unsigned char addToLine)
{
	int consumable = line->originalLen - line->originalConsumed;
	int rc = 0;

	if (consumable)
	{
		int i = mbcsLen(fileCodePage, line->original + line->originalConsumed, consumable);

		if ((i > 0) && (i <= consumable))
		{
			if (addToLine)
			{
				if (i <= (int)(sizeof(line->line) - line->lineLen))
				{
					memcpy(line->line + line->lineLen, line->original + line->originalConsumed, i);
					line->lineLen += i;
					line->originalConsumed += i;
					rc = i;
				}
			}
			else
			{
				line->originalConsumed += i;
				rc = i;
			}
		}
	}

	return rc;
}

static int edlinLastLength(const unsigned char* p, size_t len)
{
	int i = 0;

	while (len)
	{
		i = mbcsLen(fileCodePage, p, len);

		if ((i > 0) && (i <= (int)len))
		{
			p += i;
			len -= i;
		}
		else
		{
			i = -1;
			break;
		}
	}

	return i;
}

static void edlinBackspace(struct edlinReadline* line)
{
	if (line->leftColumn == line->currentColumn)
	{
		if (line->originalConsumed)
		{
			int i = edlinLastLength(line->original, line->originalConsumed);

			if (i > 0 && i <= (int)line->originalConsumed)
			{
				line->originalConsumed -= i;
			}
		}
	}
	else
	{
		unsigned int newLen = 0;

		while (newLen < line->lineLen)
		{
			int i = mbcsLen(fileCodePage, line->line + newLen, line->lineLen - newLen);

			if ((newLen + i) < line->lineLen)
			{
				newLen += i;
			}
			else
			{
				break;
			}
		}

		if (newLen != line->lineLen)
		{
			unsigned int newColumn = edlinMeasure(line, newLen);

			while (newColumn < line->currentColumn)
			{
				static unsigned char bsb[] = { 8,0x20,8 };
				edlinPrint(bsb, sizeof(bsb));
				line->currentColumn--;
			}

			if (line->originalConsumed)
			{
				line->originalConsumed--;
			}

			line->lineLen = newLen;
		}
	}
}

static void edlinEscape(struct edlinReadline* line)
{
	edlinPrintMessage(EDLMES_ESCAPE);

	if (line->leftColumn)
	{
		unsigned int paint = line->leftColumn;
		while (paint)
		{
			unsigned int spaces = paint;
			if (spaces > sizeof(line->line)) spaces = sizeof(line->line);
			memset(line->line, ' ', spaces);
			edlinPrint(line->line, spaces);
			paint -= spaces;
		}

	}

	line->currentColumn = line->leftColumn;
	line->lineLen = 0;
	line->originalConsumed = 0;
}

int edlinReadLine(struct edlinReadline* line)
{
	line->insertMode = 0;
	line->currentColumn = line->leftColumn;

	while (TRUE)
	{
		unsigned short vk;
		wchar_t ch;

		edlinPaint(line);

#if defined(_DEBUG) && defined(_WIN32)
		{
			static char oldbuf[256];
			static int oldLen;
			char buf[sizeof(oldbuf)];
			int len = sprintf_s(buf, sizeof(buf), "lineLen=%d,consumed=%d,available=%d,column=%d\r\n", line->lineLen, line->originalConsumed, line->originalLen, line->currentColumn);
			if ((len != oldLen) || memcmp(oldbuf, buf, len))
			{
				OutputDebugStringA(buf);
				memcpy(oldbuf, buf, len);
				oldLen = len;
			}
		}
#endif
		if (edlinChar(&vk, &ch) < 1)
		{
			return -1;
		}

		if (ch)
		{
			unsigned char ansi[4];
			int ansiLen;
#ifdef _WIN32
			BOOL defaultCharUsed = FALSE;
#else
			char defaultCharUsed = FALSE;
#endif

			switch (ch)
			{
			case 3:
				edlinPrintMessage(EDLMES_CTRLC);
				line->exitChar = ch;
				return 0;

			case 8:
			case 0x7f:
				edlinBackspace(line);
				break;

			case 0xa:
				break;

			case 0xd:
				edlinPrintLine(NULL, 0);
				line->exitChar = ch;
				return line->lineLen;

			case 0x1b:
				edlinEscape(line);
				break;

			default:
#ifdef _WIN32
				ansiLen = WideCharToMultiByte(fileCodePage, 0, &ch, 1, ansi, _countof(ansi), NULL, (fileCodePage == CP_UTF8) ? NULL : &defaultCharUsed);
#else
				ansiLen = mbcsFromChar(fileCodePage, ch, ansi);
#endif
				if ((ansiLen > 0) && ((line->lineLen + ansiLen) < sizeof(line->line)) && !defaultCharUsed)
				{
					memcpy(line->line + line->lineLen, ansi, ansiLen);
					line->lineLen += ansiLen;

					if (!line->insertMode)
					{
						edlinConsume(line, 0);
					}
				}
				break;
			}
		}
		else
		{
			switch (vk)
			{
			case VK_DELETE:
				edlinConsume(line, 0);
				break;

			case VK_INSERT:
				line->insertMode = !line->insertMode;
				break;

			case VK_ESCAPE:
				edlinEscape(line);
				break;

			case VK_RIGHT:
				edlinConsume(line, 1);
				break;

			case VK_LEFT:
				edlinBackspace(line);
				break;

			case VK_F3:
				while ((line->originalLen > line->originalConsumed) && edlinConsume(line, 1));
				break;
			}
		}
	}

	return line->lineLen;
}

int edlinConfirm(void)
{
	wchar_t ch;
	unsigned short vk;
	const char* no = edlinGetMessage(EDLMES_NN);
	const char* yes = edlinGetMessage(EDLMES_YY);

	while (edlinChar(&vk, &ch))
	{
		if (ch)
		{
			const char* n = strchr(no, ch);
			const char* y = strchr(yes, ch);

			if (n || y)
			{
				unsigned char buf[32];
#ifdef _WIN32
				int i = WideCharToMultiByte(fileCodePage, 0, &ch, 1, buf, sizeof(buf), NULL, NULL);
#else
				int i = mbcsFromChar(fileCodePage, ch, buf);
#endif

				if (i > 0)
				{
					edlinPrintLine(buf, i);

					return y ? 1 : 0;
				}
			}
		}
	}

	return -1;
}
