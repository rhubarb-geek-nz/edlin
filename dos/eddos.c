/************************************
 * Copyright (c) 2024 Roger Brown.
 * Licensed under the MIT License.
 */

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

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
#include <unistd.h>
#include <conio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "edlin.h"
#include "mbcs.h"
#include "readline.h"
#include "edlmes.h"

static const char * messageYY = "Yy";
static const char * messageNN = "Nn";
static const char * messagePrompt = "*";

static int isTTY;

void edlinFlush(void)
{
}

int edlinStdIn(wchar_t* ch)
{
	unsigned char buf[8];
	static wchar_t lastChar;

	while (1 == read(0,buf,1))
	{
		wchar_t wide;
		int i = mbcsLen(fileCodePage, buf, 1);

		if (i > 1)
		{
			int len = 1;

			while (len < i)
			{
				int j = read(0, buf + len, i - len);

				if (j > 0)
				{
					len += j;
				}
				else
				{
					return FALSE;
				} 
			}
		}

		wide = mbcsToChar(fileCodePage, buf, i);

		if ((0xD == lastChar) && (0xA == wide))
		{
			lastChar = 0;
		}
		else
		{
			*ch = lastChar = wide;

			return TRUE;
		}
	}

	return FALSE;
}

int edlinChar(unsigned short* vk, wchar_t* ch)
{
	wchar_t c;

	edlinFlush();

	if (isTTY)
	{
		int c;

		while (-1 != (c = getch()))
		{
			char buf[1];
#ifdef __OS2__
			if (isTTY && c == 0xE0)
#else
			if (isTTY && c == 0)
#endif
			{
				*ch = 0;

				switch (getch())
				{
					case 28: *vk = 0; *ch = 0xD; return TRUE;
					case 61: *vk = VK_F3; return TRUE;
					case 64: *vk = VK_F6; *ch = 0x1A; return TRUE;
					case 72: *vk = VK_UP; return TRUE;
					case 75: *vk = VK_LEFT; return TRUE;
					case 77: *vk = VK_RIGHT; return TRUE;
					case 80: *vk = VK_DOWN; return TRUE;
					case 82: *vk = VK_INSERT; return TRUE;
					case 83: *vk = VK_DELETE; return TRUE;
				}

				continue;
			}

			buf[0] = (char)c;

			if (1 == mbcsLen(fileCodePage, buf, 1))
			{
				*ch = mbcsToChar(fileCodePage, buf, 1);

				return TRUE;
			}
		}
	}
	else
	{
		if (edlinStdIn(&c))
		{
			*vk = 0;
			*ch = c;
			return TRUE;
		}
	}

	return FALSE;
}

static void exitHandler(void)
{
	edExit();
}

int edlinPrint(const unsigned char* p, size_t len)
{
	return write(1,p,len);
}

int edlinPrintLine(const unsigned char* p, size_t len)
{
	static unsigned char crlf[] = { 13, 10 };
	int rc = edlinPrint(p, len);

	if (rc >= 0)
	{
		rc += edlinPrint(crlf,sizeof(crlf));
	}

	return rc;
}

static struct
{
	int id;
	const char *text;
} edlmes[]={
    { EDLMES_EOF, "End of input file\r\n" },
    { EDLMES_QMES, "Abort edit (Y/N)? " },
    { EDLMES_BADCOM, "Entry error\r\n" },
    { EDLMES_ESCAPE, "\\\r\n" },
    { EDLMES_CTRLC, "^C\r\n\r\n" },
    { EDLMES_NDNAME, "File name must be specified\r\n" },
    { EDLMES_NOSUCH, "Not found.\r\n" },
    { EDLMES_ASK, "O.K.? " },
    { EDLMES_MRGERR, "Not enough room to merge the entire file\r\n" },
    { EDLMES_MEMFUL, "Insufficient memory\r\n" },
    { EDLMES_TOOLNG, "Line too long\r\n" },
    { EDLMES_NEWFIL, "New file\r\n" }
};

const char *edlinGetMessage(int message)
{
	int count = _countof(edlmes);

	switch (message)
	{
		case EDLMES_PROMPT: return messagePrompt;
		case EDLMES_NN: return messageNN;
		case EDLMES_YY: return messageYY;
		default:
			while (count--)
			{
				if (edlmes[count].id == message)
				{
					return edlmes[count].text;
				}
			}
			break;
	}

	return NULL;
}

int edlinPrintMessage(int message)
{
	int len = 0;
	const char *p = edlinGetMessage(message);

	if (p)
	{
		len = strlen(p);

		if (len > 0)
		{
			len = write(1, p, len);
		}
	}

	return len;
}

int main(int argc, char** argv)
{
	isTTY = isatty(0);

	if (isTTY == -1)
	{
		perror("isatty");
		return 1;
	}

	fileCodePage = 437;

#ifdef __OS2__
#	ifdef M_I386
	{
		ULONG codePages[8];
		ULONG dataLength=_countof(codePages);
		if (!DosQueryCp(sizeof(codePages),codePages,&dataLength))
		{
			fileCodePage = codePages[0];
		}
	}
#	else
	{
		USHORT codePages[8];
		USHORT dataLength=_countof(codePages);
		if (!DosGetCp(sizeof(codePages),codePages,&dataLength))
		{
			fileCodePage = codePages[0];
		}
	}
#	endif
#endif

	atexit(exitHandler);

	return edMainLoop(argc, argv);
}
