/************************************
 * Copyright (c) 2024 Roger Brown.
 * Licensed under the MIT License.
 */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "edlin.h"
#include "mbcs.h"
#include "edlmes.h"

static HANDLE hStdIn = INVALID_HANDLE_VALUE, hStdOut = INVALID_HANDLE_VALUE;
static DWORD dwStdInOriginalConsoleMode, dwStdOutConsoleMode, dwStdInConsoleMode;
static BOOL bStdInConsoleModeSet, bStdInConsole, bStdOutConsole;
static HMODULE hModule;

static wchar_t consoleBuffer[256];
static DWORD consoleBufferLen;
static char messageYY[32], messageNN[32];
static wchar_t messagePrompt[32];
static int messagePromptLen;
static WORD stringLang;

static UINT LoadStringForLangW(HMODULE hInstance, UINT id, WORD lang, wchar_t* buf, UINT len)
{
	HRSRC hResInfo = FindResourceExW(hInstance, RT_STRING, MAKEINTRESOURCE(1 + (id >> 4)), lang);
	UINT rc = 0;

	if (hResInfo)
	{
		int dwSize = SizeofResource(hInstance, hResInfo);
		HGLOBAL hGlobal = LoadResource(hInstance, hResInfo);

		if (hGlobal)
		{
			wchar_t* p = LockResource(hGlobal);

			id &= 0xF;

			while (dwSize > 0)
			{
				wchar_t slen = *p++;

				if (!id--)
				{
					rc = slen;

					if (rc >= len) rc = len - 1;

					memcpy(buf, p, (rc << 1));
					buf[rc] = 0;

					break;
				}

				p += slen;

				dwSize -= (1 + slen) << 1;
			}

			FreeResource(hGlobal);
		}
	}

	return rc;
}

static UINT LoadStringForLangA(HMODULE hInstance, UINT id, WORD lang, char* buf, UINT len)
{
	wchar_t tmp[256];
	UINT i = LoadStringForLangW(hInstance, id, lang, tmp, sizeof(tmp));
	if (i)
	{
		i = WideCharToMultiByte(fileCodePage, 0, tmp, i, buf, len - 1, NULL, NULL);
		if (i < len)
		{
			buf[i] = 0;
		}
	}
	return i;
}

void edlinFlush(void)
{
	if (consoleBufferLen)
	{
		DWORD dw;

		if (bStdOutConsole)
		{
			WriteConsoleW(hStdOut, consoleBuffer, consoleBufferLen, &dw, NULL);
		}
		else
		{
			wchar_t* p = consoleBuffer;
			int len = consoleBufferLen;

			while (len)
			{
				int gulp = (len > 256) ? 256 : len;
				char buf[1024];
				int i = WideCharToMultiByte(fileCodePage, 0, p, gulp, buf, sizeof(buf), NULL, NULL);

				if (i > 0)
				{
					WriteFile(hStdOut, buf, i, &dw, NULL);
				}

				p += gulp;
				len -= gulp;
			}
		}

		consoleBufferLen = 0;
	}
}

int edlinStdIn(wchar_t* ch)
{
	DWORD dw = 0;

	if (bStdInConsole)
	{
		if (ReadConsoleW(hStdIn, ch, 1, &dw, NULL) && (dw == 1))
		{
			return TRUE;
		}
	}
	else
	{
		char buf[8];
		DWORD dw;
		static wchar_t lastChar;

		while (ReadFile(hStdIn, buf, 1, &dw, NULL) && (dw == 1))
		{
			wchar_t wide[8];
			int i = mbcsLen(fileCodePage, buf, 1);

			if (i > 1)
			{
				DWORD diff = i - dw;

				if (!ReadFile(hStdIn, buf + 1, diff, &dw, NULL) || dw != diff) return FALSE;
			}

			if (1 != MultiByteToWideChar(fileCodePage, 0, buf, i, wide, _countof(wide)))
			{
				wide[0] = buf[0];
			}

			if ((0xD == lastChar) && (0xA == wide[0]))
			{
				lastChar = 0;
			}
			else
			{
				*ch = lastChar = wide[0];

				return TRUE;
			}
		}
	}

	return FALSE;
}

static const struct vk_map
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

static int vk_map_lookup(int key, const struct vk_map* map, size_t num, unsigned short* vk, wchar_t* ch)
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
	edlinFlush();

	if (bStdInConsole && !(dwStdInConsoleMode & ENABLE_VIRTUAL_TERMINAL_INPUT))
	{
		INPUT_RECORD inputRecord;
		DWORD dw;

		while (ReadConsoleInputW(hStdIn, &inputRecord, 1, &dw) && (dw == 1))
		{
			if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown)
			{
				if (!inputRecord.Event.KeyEvent.uChar.UnicodeChar)
				{
					switch (inputRecord.Event.KeyEvent.wVirtualKeyCode)
					{
					case VK_F6:
						*vk = inputRecord.Event.KeyEvent.wVirtualKeyCode;
						*ch = 0x1A;
						break;

					default:
						*vk = inputRecord.Event.KeyEvent.wVirtualKeyCode;
						*ch = 0;
						break;
					}
				}
				else
				{
					*vk = inputRecord.Event.KeyEvent.wVirtualKeyCode;
					*ch = inputRecord.Event.KeyEvent.uChar.UnicodeChar;
				}

				return TRUE;
			}
		}
	}
	else
	{
		wchar_t c;

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
	}

	return FALSE;
}

static void exitHandler(void)
{
	edlinFlush();
	edExit();
	if (bStdInConsoleModeSet)
	{
		SetConsoleMode(hStdIn, dwStdInOriginalConsoleMode);
	}
	edlinFlush();
}

int edlinPrint(const unsigned char* p, size_t len)
{
	int result = 0;

	while (len)
	{
		int runLength = 0;
		int gulp = (len > 256) ? 256 : (int)len;

		while (runLength < gulp)
		{
			int i = mbcsLen(fileCodePage, p + runLength, len - runLength);
			if (i < 1) break;
			runLength += i;
		}

		if (!runLength) break;

		if (len >= (size_t)runLength)
		{
			wchar_t wide[256];
			int i = MultiByteToWideChar(fileCodePage, 0, p, runLength, wide, _countof(wide));

			if (i < 1) break;

			if ((consoleBufferLen + i) > _countof(consoleBuffer))
			{
				edlinFlush();
			}

			memcpy(consoleBuffer + consoleBufferLen, wide, i * sizeof(consoleBuffer[0]));
			consoleBufferLen += i;

			p += runLength;
			len -= runLength;
			result += runLength;
		}
	}

	return result;
}

int edlinPrintLine(const unsigned char* p, size_t len)
{
	int rc = edlinPrint(p, len);

	if (rc >= 0)
	{
		if ((consoleBufferLen + 2) > _countof(consoleBuffer))
		{
			edlinFlush();
		}

		consoleBuffer[consoleBufferLen++] = '\r';
		consoleBuffer[consoleBufferLen++] = '\n';

		rc += 2;
	}

	return rc;
}

int edlinPrintMessage(int message)
{
	int len = 0;
	const wchar_t* p = NULL;
	wchar_t buf[64];

	switch (message)
	{
	case EDLMES_PROMPT:
		p = messagePrompt;
		len = messagePromptLen;
		break;

	default:
		len = LoadStringForLangW(hModule, message, stringLang, buf, _countof(buf));
		if (len > 0)
		{
			p = buf;
		}
		break;
	}

	if (len > 0)
	{
		if ((consoleBufferLen + len) > _countof(consoleBuffer))
		{
			edlinFlush();
		}

		memcpy(consoleBuffer + consoleBufferLen, p, len * sizeof(buf[0]));

		consoleBufferLen += len;
	}

	return len;
}

void edlinPrintWin32Error(DWORD err)
{
	wchar_t buf[256];
	DWORD len = FormatMessageW(
		FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		err,
		0,
		buf,
		_countof(buf),
		NULL);

	if (len)
	{
		if ((consoleBufferLen + len) > _countof(consoleBuffer))
		{
			edlinFlush();
		}

		memcpy(consoleBuffer + consoleBufferLen, buf, len * sizeof(buf[0]));
		consoleBufferLen += len;
		edlinFlush();
	}
}

extern const char* edlinGetMessage(int message)
{
	switch (message)
	{
	case EDLMES_NN: return messageNN;
	case EDLMES_YY: return messageYY;
	}

	return NULL;
}

static BOOL containsLang(const WORD* p, int len, WORD lang)
{
	while (len--)
	{
		if (lang == *p++) return TRUE;
	}

	return FALSE;
}

int main(int argc, char** argv)
{
	hModule = GetModuleHandleW(NULL);

	fileCodePage = GetACP();
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	bStdInConsole = GetConsoleMode(hStdIn, &dwStdInOriginalConsoleMode);
	bStdOutConsole = GetConsoleMode(hStdOut, &dwStdOutConsoleMode);

	if (bStdInConsole)
	{
		dwStdInConsoleMode = (dwStdInOriginalConsoleMode & ~(ENABLE_PROCESSED_INPUT | ENABLE_ECHO_INPUT | ENABLE_INSERT_MODE | ENABLE_LINE_INPUT)) | ENABLE_VIRTUAL_TERMINAL_INPUT;

		insertMode = (dwStdInOriginalConsoleMode & ENABLE_INSERT_MODE) ? 1 : 0;

		if (dwStdInConsoleMode != dwStdInOriginalConsoleMode)
		{
			bStdInConsoleModeSet = SetConsoleMode(hStdIn, dwStdInConsoleMode);

			if (!bStdInConsoleModeSet)
			{
				dwStdInConsoleMode &= ~ENABLE_VIRTUAL_TERMINAL_INPUT;

				bStdInConsoleModeSet = SetConsoleMode(hStdIn, dwStdInConsoleMode);
			}

			if (!bStdInConsoleModeSet)
			{
				DWORD dw = GetLastError();
				edlinPrintWin32Error(dw);
				return dw;
			}
		}
	}

	if (bStdOutConsole)
	{
		consoleCodePage = GetConsoleOutputCP();

		switch (consoleCodePage)
		{
		case CP_ACP:
			consoleCodePage = fileCodePage;
			break;
		}
	}
	else
	{
		consoleCodePage = fileCodePage;
	}

	atexit(exitHandler);

	{
		WORD langList[4];
		int i = 0, j = 0;
		WORD w;

		langList[i++] = LANGIDFROMLCID(GetThreadLocale());

		w = GetUserDefaultLangID();

		if (!containsLang(langList, i, w)) langList[i++] = w;

		w = GetSystemDefaultLangID();

		if (!containsLang(langList, i, w)) langList[i++] = w;

		w = MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL);

		if (!containsLang(langList, i, w)) langList[i++] = w;

		while (j < i)
		{
			stringLang = langList[j++];

			messagePromptLen = LoadStringForLangW(hModule, EDLMES_PROMPT, stringLang, messagePrompt, _countof(messagePrompt));

			if (messagePromptLen) break;
		}

		if (!messagePromptLen)
		{
			edlinPrintWin32Error(GetLastError());
			return 1;
		}
	}

	LoadStringForLangA(hModule, EDLMES_NN, stringLang, messageNN, sizeof(messageNN));
	LoadStringForLangA(hModule, EDLMES_YY, stringLang, messageYY, sizeof(messageYY));

	SetConsoleCtrlHandler(NULL, TRUE);

	return edMainLoop(argc, argv);
}
