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
#ifdef HAVE_LIBINTL_H
#	include <libintl.h>
#else
#	ifdef HAVE_NL_TYPES_H
#		include <nl_types.h>
#	endif
#endif
#include "edlin.h"
#include "mbcs.h"
#include "readline.h"
#include "edlmes.h"
#include "edlmesen.h"

static struct termios ttyAttr;
static int restoreAttr;
#if defined(HAVE_NL_TYPES_H) && !defined(HAVE_LIBINTL_H)
static nl_catd nlCat=(nl_catd)-1;
#endif
static char messageYY[32], messageNN[32], messagePrompt[32];

#if !defined(HAVE_CFMAKERAW)
static void cfmakeraw(struct termios *tt)
{
	tt->c_iflag=0;
	tt->c_oflag&=~OPOST;
	tt->c_lflag&=~(ISIG|ICANON|ECHO
#ifdef XCASE
			|XCASE
#endif
			);
	tt->c_cflag&=~(CSIZE|PARENB);
	tt->c_cflag|=CS8;
	tt->c_cc[VMIN]=1;
	tt->c_cc[VTIME]=1;
}
#endif

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

static struct vk_map
{
	int key;
	unsigned short vk;
	wchar_t ch;
} vk_map_O[] = {
	{ 'A',VK_UP },
	{ 'B',VK_UP },
	{ 'C',VK_RIGHT },
	{ 'D',VK_LEFT },
	{ 'F',VK_END },
	{ 'H',VK_HOME },
	{ 'P',VK_F1 },
	{ 'Q',VK_F2 },
	{ 'R',VK_F3 },
	{ 'S',VK_F4 }
}, vk_map_tilde[] = {
	{ 2, VK_INSERT },
	{ 3, VK_DELETE },
	{ 5, VK_PRIOR },
	{ 6, VK_NEXT },
	{ 15, VK_F5 },
	{ 17, VK_F6, 0x1A },
	{ 18, VK_F7 },
	{ 19, VK_F8 },
	{ 20, VK_F9 },
	{ 21, VK_F10 }
};

static int vk_map_lookup(int key, struct vk_map* map, size_t num, unsigned short* vk, wchar_t* ch)
{
	while (num--)
	{
		if (map->key == key)
		{
			*vk = map->vk;
			*ch = map->ch;
			return TRUE;
		}

		map++;
	}

	return FALSE;
}

int edlinChar(unsigned short* vk, wchar_t* ch)
{
	wchar_t c;

	edlinFlush();

	while (edlinStdIn(&c))
	{
		if (c != 0x1B)
		{
			*vk = 0;
			*ch = c;

			return 1;
		}

		*ch = 0;

		if (edlinStdIn(&c))
		{
			int argCount = 0;
			int args[10];
			int arg = 0;

			switch (c)
			{
			case 'O':
				if (edlinStdIn(&c))
				{
					if (vk_map_lookup(c, vk_map_O, _countof(vk_map_O), vk, ch))
					{
						return TRUE;
					}
				}
				break;

			case '[':
				while (edlinStdIn(&c))
				{
					if (c >= '0' && c <= '9')
					{
						arg = (arg * 10) + c - '0';
					}
					else
					{
						if (argCount < _countof(args))
						{
							args[argCount++] = arg;
						}

						arg = 0;

						switch (c)
						{
						case ';':
							break;

						case '~':
							if (argCount)
							{
								if (vk_map_lookup(args[0], vk_map_tilde, _countof(vk_map_tilde), vk, ch))
								{
									return TRUE;
								}
							}
							c = 0;
							break;

						default:
							if (vk_map_lookup(c, vk_map_O, _countof(vk_map_O), vk, ch))
							{
								return TRUE;
							}
							c = 0;
							break;
						}

						if (!c) break;
					}
				}
				break;

			default:
				*ch = c;
				*vk = 0;
				return TRUE;
			}
		}
	}

	return FALSE;
}

static void exitHandler(void)
{
	edExit();

	if (restoreAttr)
	{
		tcsetattr(0,TCSADRAIN,&ttyAttr);
	}

#if defined(HAVE_NL_TYPES_H) && !defined(HAVE_LIBINTL_H)
	if (nlCat != (nl_catd)-1)
	{
		catclose(nlCat);
		nlCat = (nl_catd)-1;
	}
#endif
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

#define EDLMESEN_MAP(x)    { EDLMES_##x, EDLMESEN_##x }

static struct
{
	int id;
	const char *text;
} edlmes[]={
    EDLMESEN_MAP(EOF),
    EDLMESEN_MAP(QMES),
    EDLMESEN_MAP(BADCOM),
    EDLMESEN_MAP(ESCAPE),
    EDLMESEN_MAP(CTRLC),
    EDLMESEN_MAP(NDNAME),
    EDLMESEN_MAP(NOSUCH),
    EDLMESEN_MAP(ASK),
    EDLMESEN_MAP(MRGERR),
    EDLMESEN_MAP(MEMFUL),
    EDLMESEN_MAP(TOOLNG),
    EDLMESEN_MAP(NEWFIL)
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
					const char *p = edlmes[count].text;

#ifdef HAVE_LIBINTL_H
						p = gettext(p);
#else
#	ifdef HAVE_NL_TYPES_H
					if (nlCat != (nl_catd)-1)
					{
						p = catgets(nlCat, 1, message, p);
					}
#	endif
#endif

					return p;
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

void edlinPrintError(int err)
{
	const char* p = strerror(err);
	edlinPrintLine((const unsigned char*)p, strlen(p));
}

static void loadString(int message, char *str, size_t len, const char *p)
{
#ifdef HAVE_LIBINTL_H
	p = gettext(p);
#else
#	ifdef HAVE_NL_TYPES_H
	if (nlCat != (nl_catd)-1)
	{
		p = catgets(nlCat, 1, message, p);
	}
#	endif
#endif

	strncat(str, p, len);
}

int main(int argc, char** argv)
{
	if (isatty(0))
	{
		struct termios attr;
		if (tcgetattr(0, &ttyAttr))
		{
			perror("tcgetattr");
			return 1;
		}
		attr = ttyAttr;
		cfmakeraw(&attr);
		if (tcsetattr(0, TCSADRAIN, &attr))
		{
			perror("tcsetattr");
			return 1;
		}
		restoreAttr = 1;
	}

	fileCodePage = mbcsCodePage();

	atexit(exitHandler);

#ifdef HAVE_LIBINTL_H
#	ifdef EDLIN_BINDTEXTDOMAIN
	bindtextdomain("edlin", EDLIN_BINDTEXTDOMAIN);
#	endif
	textdomain("edlin");
#else
#	ifdef HAVE_NL_TYPES_H
	nlCat = catopen("edlin", 0);
#	endif
#endif

	loadString(EDLMES_PROMPT, messagePrompt, sizeof(messagePrompt), EDLMESEN_PROMPT);
	loadString(EDLMES_NN, messageNN, sizeof(messageNN), EDLMESEN_NN);
	loadString(EDLMES_YY, messageYY, sizeof(messageYY), EDLMESEN_YY);

	return edMainLoop(argc, argv);
}
