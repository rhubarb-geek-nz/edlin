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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif
#include "edlin.h"
#include "edlmes.h"
#include "readline.h"
#include "mbcs.h"

unsigned int fileCodePage, consoleCodePage;
unsigned char insertMode;
size_t contentLow, contentHigh, contentLength, currentLine, contentLowCount, contentHighCount;
unsigned char* contentStore;
const char* mainFileName;
char tmpFileName[MAX_PATH];
FILE* fileMainRead;
FILE* fileTmpWrite;
char edlinCommand, edlinOption, edlinNewFile;
unsigned int edlinArgCount, edlinArgLength;
long edlinArgList[4];
const unsigned char* edlinArgValue;
unsigned char edlinLastCommand[256];
unsigned int edlinLastCommandLen;

const char edlinControlChar[] = {
	'@','A','B','C','D','E','F','G',
	'H','I','J','K','L','M','N','O',
	'P','Q','R','S','T','U','V','W',
	'X','Y','Z','[','\\',']','^','_' };

void edExit(void)
{
	if (fileMainRead)
	{
		fclose(fileMainRead);
		fileMainRead = NULL;
	}

	if (fileTmpWrite)
	{
		fclose(fileTmpWrite);
		fileTmpWrite = NULL;
	}

	if (tmpFileName[0])
	{
#ifdef _WIN32
		DeleteFileA(tmpFileName);
#else
		unlink(tmpFileName);
#endif
		tmpFileName[0] = 0;
	}
}

int findChar(const unsigned char* p, char c, size_t len)
{
	size_t offset = 0;

	while (len--)
	{
		if (p[offset] == c) return (int)offset;

		offset++;
	}

	return -1;
}

int edlinFileRead(unsigned char* line, FILE* fp)
{
	unsigned char* p = line + 1;
	int rc = -1;
	size_t lineLen = 0;
	int len = 0, eol = 0;
	unsigned char buf[4];
	int any = 0;

	while (fp)
	{
		int c = fgetc(fp);
		int i;

		if (c == EOF)
		{
			if (feof(fp))
			{
				rc = any;
			}
			else
			{
				int err = ferror(fp);
				edlinPrintError(err);
				rc = -1;
			}
			break;
		}

		any = 1;

		buf[len++] = c;

		i = mbcsLen(fileCodePage, buf, len);

		if (i == len)
		{
			if (i == 1 && c == '\n')
			{
				rc = 2;
				break;
			}

			if (eol == 0)
			{
				if ((lineLen + i) > 255)
				{
					eol = 1;
					edlinPrintMessage(EDLMES_TOOLNG);
				}
				else
				{
					memcpy(p, buf, i);
					lineLen += i;
					p += i;
				}
			}

			len = 0;
		}
		else
		{
			if (i < 0)
			{
				wchar_t replacement = 0xfffd;
#ifdef _WIN32
				int i = WideCharToMultiByte(fileCodePage, 0, &replacement, 1, buf, sizeof(buf), NULL, NULL);
#else
				int i = mbcsFromChar(fileCodePage, replacement, buf);
#endif

				if (i > 0)
				{
					if ((lineLen + i) <= 255)
					{
						memcpy(p, buf, i);
						lineLen += i;
						p += i;

					}
				}

				len = 0;
			}
		}
	}

	line[0] = (unsigned char)lineLen;

	return rc;
}

void edlinSeek(size_t index)
{
	size_t count = contentLowCount + contentHighCount;

	if (index > count)
	{
		abort();
	}

	if (index != contentLowCount)
	{
		if (index < contentLowCount)
		{
			/* move from low to high */
			size_t lineCount = contentLowCount - index;
			size_t i = lineCount;
			size_t byteCount = 0;
			const unsigned char* p = edlinLineFromIndex(index);
			while (i--)
			{
				size_t lineLength = 1 + *p;
				byteCount += lineLength;
				p += lineLength;
			}
#if defined(_WIN32) && defined(_DEBUG)
			{
				char buf[256];
				sprintf_s(buf, sizeof(buf), "move %d lines, %d bytes from low to high\r\n", (int)lineCount, (int)byteCount);
				OutputDebugStringA(buf);
			}
#endif
			contentLow -= byteCount;
			contentHigh += byteCount;
			contentLowCount -= lineCount;
			contentHighCount += lineCount;
			memmove(contentStore + contentLength - contentHigh, contentStore + contentLow, byteCount);
		}
		else
		{
			/* move from high to low */
			size_t lineCount = index - contentLowCount;
			size_t i = lineCount;
			size_t byteCount = 0;
			const unsigned char* p = edlinLineFromIndex(contentLowCount);
			while (i--)
			{
				size_t lineLength = 1 + *p;
				byteCount += lineLength;
				p += lineLength;
			}
#if defined(_WIN32) && defined(_DEBUG)
			{
				char buf[256];
				sprintf_s(buf, sizeof(buf), "move %d lines, %d bytes from high to low\r\n", (int)lineCount, (int)byteCount);
				OutputDebugStringA(buf);
			}
#endif
			memmove(contentStore + contentLow, contentStore + contentLength - contentHigh, byteCount);
			contentHigh -= byteCount;
			contentLow += byteCount;
			contentHighCount -= lineCount;
			contentLowCount += lineCount;
		}
	}
}

void edlinAppend(void)
{
	if (edlinArgCount || edlinArgLength || edlinOption)
	{
		edlinPrintMessage(EDLMES_BADCOM);
		edlinArgLength = 0;
		return;
	}

	if (fileMainRead == NULL)
	{
		edlinPrintMessage(EDLMES_EOF);
		edlinArgLength = 0;
		return;
	}

	edlinSeek(contentLowCount + contentHighCount);

	if (fileMainRead)
	{
		size_t headRoom = (contentLength >> 1) + (contentLength >> 2);

		while (contentLow < headRoom)
		{
			unsigned char* line = contentStore + contentLow;
			int rc = edlinFileRead(line, fileMainRead);

			if (rc < 0)
			{
				break;
			}

			if (rc > 0)
			{
				contentLow += 1 + line[0];
				contentLowCount++;
			}

			if (rc == 0 || rc == 1)
			{
				fclose(fileMainRead);
				fileMainRead = NULL;
				edlinPrintMessage(EDLMES_EOF);
				break;
			}
		}
	}
}

static int edlinParseCmd(const unsigned char* p, unsigned int len)
{
	int rc = 0;
	long value = 0;
	char valueSet = 0, signSet = 0, commaSet = 0, spaceSet = 0;
	unsigned int argCount = 0;

	edlinCommand = 0;
	edlinArgCount = 0;
	edlinOption = 0;
	edlinArgLength = 0;
	edlinArgValue = NULL;

	while (len--)
	{
		unsigned char c = *p++;

		if (isalpha(c) || (c == '?') || (c == ','))
		{
			if (valueSet)
			{
				if (commaSet)
				{
					return -1;
				}

				switch (signSet)
				{
				case '-':
					value = (long)(currentLine + 1 - value);
					break;
				case '+':
					value = (long)(currentLine + 1 + value);
					break;
				}

				if (edlinArgCount >= _countof(edlinArgList))
				{
					return -1;
				}

				edlinArgList[edlinArgCount++] = value;

				signSet = 0;
				valueSet = 0;
				value = 0;
			}
			else
			{
				if (signSet)
				{
					return -1;
				}
			}

			spaceSet = 0;
		}

		switch (c)
		{
		case ' ':
		case '\t':
			if (valueSet)
			{
				spaceSet = 1;
			}
			break;

		case '?':
			if (edlinOption || commaSet) return -1;
			edlinOption = c;
			break;

		case '-':
		case '+':
			if (signSet || valueSet || edlinArgCount >= 3) return -1;
			signSet = c;
			break;

		case ',':
			argCount++;

			if (!valueSet)
			{
				if (signSet)
				{
					return -1;
				}

				if (argCount == (edlinArgCount + 1))
				{
					edlinArgList[edlinArgCount++] = (argCount < 4) ? (long)currentLine + 1 : 1;
				}
			}

			commaSet = 1;
			break;

		default:
			if ((c >= '0') && (c <= '9'))
			{
				if (edlinOption) return -1;
				if (valueSet && (commaSet || spaceSet)) return -1;
				if (edlinArgCount >= _countof(edlinArgList)) return -1;
				commaSet = 0;
				valueSet = 1;
				value = value * 10 + (c - '0');
			}
			else
			{
				if (isalpha(c))
				{
					if (commaSet)
					{
						if (argCount == edlinArgCount)
						{
							if (argCount < 3)
							{
								edlinArgList[edlinArgCount++] = (long)currentLine + 1;
							}
							else
							{
								edlinArgList[edlinArgCount++] = 1;
							}

							commaSet = 0;
						}
						else
						{
							return -1;
						}
					}

					edlinCommand = toupper(c);

					if (len)
					{
						edlinArgValue = p;
						edlinArgLength = len;
						len = 0;
					}
				}
				else
				{
					return -1;
				}
			}
			break;
		}
	}

	if (commaSet)
	{
		return -1;
	}

	if (valueSet)
	{
		switch (signSet)
		{
		case '-':
			value = (long)(currentLine + 1 - value);
			break;

		case '+':
			value = (long)(currentLine + 1 + value);
			break;
		}

		edlinArgList[edlinArgCount++] = value;
	}

	return rc;
}

size_t edlinLengthFromIndex(size_t index, size_t count)
{
	size_t length = 0;

	if (index < contentLowCount)
	{
		const unsigned char* p = contentStore;
		size_t lineCount = index;

		while (lineCount--)
		{
			unsigned char len = 1 + *p;
			p += len;
		}

		lineCount = contentLowCount - index;

		while (count && lineCount--)
		{
			unsigned char len = 1 + *p;
			p += len;
			count--;
			length += len;
		}

		lineCount = contentHighCount;

		p = contentStore + contentLength - contentHigh;

		while (count && lineCount--)
		{
			unsigned char len = 1 + *p;
			p += len;
			count--;
			length += len;
		}
	}
	else
	{
		const unsigned char* p = contentStore + contentLength - contentHigh;
		size_t lineCount;
		index -= contentLowCount;
		lineCount = index;

		while (lineCount--)
		{
			unsigned char len = 1 + *p;
			p += len;
		}

		lineCount = contentHighCount - index;

		while (count && lineCount--)
		{
			unsigned char len = 1 + *p;
			p += len;
			count--;
			length += len;
		}
	}

	return length;
}

unsigned char* edlinLineFromIndex(size_t index)
{
	unsigned char* p = contentStore;

	if (index < contentLowCount)
	{
		while (index--)
		{
			unsigned char len = *p;
			p += len + 1;
		}

		return p;
	}

	index -= contentLowCount;

	if (index < contentHighCount)
	{
		p += contentLength - contentHigh;

		while (index--)
		{
			unsigned char len = *p;
			p += len + 1;
		}

		return p;
	}

	return NULL;
}

int edlinIndexFromArg(long arg, size_t* s)
{
	int rc = 0;

	if (!arg)
	{
		edlinPrintMessage(EDLMES_BADCOM);
		edlinArgLength = 0;
	}
	else
	{
		size_t count = contentLowCount + contentHighCount;
		size_t index = arg - 1;

		if (index > count)
		{
			index = count;
		}

		*s = index;

		rc = 1;
	}

	return rc;
}

int edlinPrintLinePrompt(size_t index)
{
	unsigned char buf[11];
#if defined(_WIN32) && !defined(__WATCOMC__)
	int i = sprintf_s(buf, sizeof(buf), "%ld:%c", (long)(index + 1), currentLine == index ? '*' : ' ');
#else
	int i = snprintf((char*)buf, sizeof(buf), "%ld:%c", (long)(index + 1), currentLine == index ? '*' : ' ');
#endif

	if (i < sizeof(buf))
	{
		int diff = sizeof(buf) - i;
		memmove(buf + diff, buf, i);
		memset(buf, ' ', diff);
		i += diff;
	}

	return edlinPrint(buf, i);
}

int edlinEnumerateLines(size_t index, size_t count, int (*func)(size_t, const unsigned char*))
{
	int rc = -1;
	size_t total = contentLowCount + contentHighCount;

	if (index < total)
	{
		rc = 0;

		if ((index + count) > total)
		{
			count = total - index;
		}

		if (index < contentLowCount)
		{
			const unsigned char* p = edlinLineFromIndex(index);
			size_t lowCount = contentLowCount - index;
			if (lowCount > count) lowCount = count;
			count -= lowCount;

			while (lowCount--)
			{
				int i = func(index, p);
				if (i)
				{
					rc = i;
					break;
				}
				index++;
				p += 1 + *p;
			}
		}

		if (count && !rc)
		{
			const unsigned char* p = edlinLineFromIndex(index);

			while (count--)
			{
				int i = func(index, p);
				if (i)
				{
					rc = i;
					break;
				}
				index++;
				p += 1 + *p;
			}
		}
	}

	return rc;
}

static int edlinListLine(size_t index, const unsigned char* p)
{
	struct edlinReadline line = READLINE_INIT;

	line.insertMode = 0;
	line.original = NULL;
	line.originalLen = 0;
	line.leftColumn = edlinPrintLinePrompt(index);
	line.currentColumn = line.leftColumn;
	line.lineLen = *p++;

	if (line.lineLen)
	{
		memcpy(line.line, p, line.lineLen);
	}

	edlinPaint(&line);
	edlinPrintLine(NULL, 0);

	return 0;
}

void edlinListLines(void)
{
	size_t index = 0;
	size_t count = contentLowCount + contentHighCount;

	if (edlinArgCount > 2)
	{
		edlinPrintMessage(EDLMES_BADCOM);
		edlinArgLength = 0;
		return;
	}

	if (edlinArgCount)
	{
		if (!edlinIndexFromArg(edlinArgList[0], &index))
		{
			return;
		}

		if (edlinArgCount > 1)
		{
			size_t to = count;

			if (!edlinIndexFromArg(edlinArgList[1], &to))
			{
				return;
			}

			if (to < index)
			{
				count = 0;
			}
			else
			{
				count = 1 + to - index;
			}
		}
	}
	else
	{
		if (currentLine > 11)
		{
			index = (long)(currentLine - 11);
			count -= index;
		}
	}

	if ((edlinArgCount < 2) && (count > 23))
	{
		count = 23;
	}

	edlinEnumerateLines(index, count, edlinListLine);
}

void edlinPage(void)
{
	size_t index = currentLine;
	size_t total = contentLowCount + contentHighCount;
	size_t count = total;

	if (edlinArgCount > 2)
	{
		edlinPrintMessage(EDLMES_BADCOM);
		edlinArgLength = 0;
		return;
	}

	if (edlinArgCount)
	{
		if (!edlinIndexFromArg(edlinArgList[0], &index))
		{
			return;
		}

		if (edlinArgCount > 1)
		{
			size_t to = count;

			if (!edlinIndexFromArg(edlinArgList[1], &to))
			{
				return;
			}

			if (to < index)
			{
				count = 0;
			}
			else
			{
				count = 1 + to - index;
			}
		}
	}
	else
	{
		if (index)
		{
			index++;
		}
	}

	if ((count + index) >= total)
	{
		count = total - index;
	}

	if ((edlinArgCount < 2) && (count > 23))
	{
		count = 23;
	}

	currentLine = index + count - 1;

	edlinEnumerateLines(index, count, edlinListLine);
}

void edlinEditSingleLine(void)
{
	unsigned char* p;

	if (edlinArgCount > 1)
	{
		edlinPrintMessage(EDLMES_BADCOM);
		edlinArgLength = 0;
		return;
	}

	if (edlinArgCount)
	{
		if (!edlinIndexFromArg(edlinArgList[0], &currentLine))
		{
			return;
		}
	}
	else
	{
		size_t count = contentLowCount + contentHighCount;

		if (currentLine < count)
		{
			currentLine++;
		}
	}

	p = edlinLineFromIndex(currentLine);

	if (p)
	{
		int i;
		struct edlinReadline line = READLINE_INIT;

		line.insertMode = 0 /*insertMode*/;
		line.original = p + 1;
		line.originalLen = *p;
		line.leftColumn = edlinPrintLinePrompt(currentLine);
		line.currentColumn = line.leftColumn;

		if (line.originalLen)
		{
			line.lineLen = line.originalLen;
			memcpy(line.line, p + 1, line.originalLen);
			edlinPaint(&line);
		}

		edlinPrintLine(NULL, 0);

		line.lineLen = 0;
		line.leftColumn = edlinPrintLinePrompt(currentLine);

		i = edlinReadLine(&line);

		if ((i < 0) || (line.exitChar == 3))
		{
			edlinArgLength = 0;
			return;
		}

		if ((line.lineLen == 0) && (line.exitChar == '\r'))
		{
			/* special case, no change to the line */
			return;
		}

		if ((line.lineLen + 1) > (contentLength - contentLow - contentHigh))
		{
			edlinArgLength = 0;
			edlinPrintMessage(EDLMES_MEMFUL);
			return;
		}

		edlinSeek(currentLine);

		contentHigh += ((int)line.lineLen) - (int)line.originalLen;

		p = contentStore + contentLength - contentHigh;

		*p++ = line.lineLen;

		if (line.lineLen)
		{
			memcpy(p, line.line, line.lineLen);
		}
	}
}

static int edlinMatchLine(const unsigned char* p, const unsigned char* match, size_t len)
{
	size_t c = *p++;

	if (c >= len)
	{
		size_t diff = 1 + c - len;

		while (diff--)
		{
			if (!memcmp(p, match, len))
			{
				return 1;
			}

			p++;
		}
	}

	return 0;
}

static int edlinSearchLine(size_t index, const unsigned char* p)
{
	if (edlinMatchLine(p, edlinArgValue, edlinArgLength))
	{
		edlinListLine(index, p);

		if (edlinOption)
		{
			int i;

			edlinPrintMessage(EDLMES_ASK);

			i = edlinConfirm();

			if (i <= 0)
			{
				return i;
			}
		}

		currentLine = index;

		return 1;
	}

	return 0;
}

void edlinSearch(void)
{
	size_t index = currentLine + 1;
	size_t count = contentLowCount + contentHighCount;

	if ((edlinArgCount > 2) || !edlinArgLength)
	{
		edlinPrintMessage(EDLMES_BADCOM);
		return;
	}

	if (edlinArgCount)
	{
		if (!edlinIndexFromArg(edlinArgList[0], &index))
		{
			return;
		}

		if (edlinArgCount > 1)
		{
			size_t to = count;

			if (!edlinIndexFromArg(edlinArgList[1], &to))
			{
				return;
			}

			if (to < index)
			{
				count = 0;
			}
			else
			{
				count = 1 + to - index;
			}
		}
	}
	else
	{
		if (index > count)
		{
			count = 0;
		}
		else
		{
			count -= index;
		}
	}

	if (edlinEnumerateLines(index, count, edlinSearchLine) < 1)
	{
		edlinPrintMessage(EDLMES_NOSUCH);
	}
}

void edlinInsert(void)
{
	size_t index = currentLine;
	size_t count = contentLowCount + contentHighCount;

	if (edlinArgCount > 1)
	{
		edlinPrintMessage(EDLMES_BADCOM);
		edlinArgLength = 0;
		return;
	}

	if (edlinArgCount)
	{
		if (!edlinIndexFromArg(edlinArgList[0], &index))
		{
			return;
		}
	}

	if (index > count)
	{
		index = count;
	}

	currentLine = index;

	edlinSeek(index);

	while (TRUE)
	{
		int i;
		struct edlinReadline line = READLINE_INIT;
		line.insertMode = 0 /*insertMode*/;
		line.leftColumn = edlinPrintLinePrompt(currentLine);

		i = edlinReadLine(&line);

		if ((i < 0) || (line.exitChar == 3))
		{
			edlinArgLength = 0;
			break;
		}

		if (i == 1 && line.line[0] == 0x1A)
		{
			break;
		}

		if ((line.lineLen + 1) > (contentLength - contentLow - contentHigh))
		{
			edlinArgLength = 0;
			edlinPrintMessage(EDLMES_MEMFUL);
			break;
		}

		contentStore[contentLow++] = line.lineLen;

		if (line.lineLen)
		{
			memcpy(contentStore + contentLow, line.line, line.lineLen);
			contentLow += line.lineLen;
		}

		contentLowCount++;
		currentLine++;
	}
}

void edlinDelete(void)
{
	size_t index = currentLine;
	size_t count = 1;

	if (edlinArgCount > 2)
	{
		edlinPrintMessage(EDLMES_BADCOM);
		edlinArgLength = 0;
		return;
	}

	if (edlinArgCount)
	{
		if (!edlinIndexFromArg(edlinArgList[0], &index))
		{
			return;
		}

		if (edlinArgCount > 1)
		{
			size_t to = currentLine;

			if (!edlinIndexFromArg(edlinArgList[1], &to))
			{
				return;
			}

			if (to < index)
			{
				edlinPrintMessage(EDLMES_BADCOM);
				edlinArgLength = 0;
				return;
			}

			count = to + 1 - index;
		}
	}

	if (index < (contentHighCount + contentLowCount))
	{
		edlinSeek(index);

		currentLine = index;

		while (count-- && contentHighCount && contentHigh)
		{
			const unsigned char* p = contentStore + contentLength - contentHigh;
			contentHighCount--;
			contentHigh -= 1 + *p;
		}
	}
}

void edlinCopy(void)
{
	size_t first = currentLine, last = currentLine, dest, repeat = 1, count = 1;
	const unsigned char* source;
	size_t totalCount = contentLowCount + contentHighCount;
	size_t totalBytes;

	if (edlinArgCount < 3 || edlinArgCount > 4)
	{
		edlinPrintMessage(EDLMES_BADCOM);
		edlinArgLength = 0;
		return;
	}

	if (!edlinIndexFromArg(edlinArgList[0], &first) ||
		!edlinIndexFromArg(edlinArgList[1], &last) ||
		!edlinIndexFromArg(edlinArgList[2], &dest))
	{
		return;
	}

	if (edlinArgCount > 3)
	{
		repeat = edlinArgList[3];
	}

	if (((dest >= first) && (dest <= last))
		|| (last < first)
		|| (first >= totalCount)
		|| (last >= totalCount))
	{
		edlinPrintMessage(EDLMES_BADCOM);
		return;
	}

	count = last + 1 - first;

	totalBytes = edlinLengthFromIndex(first, count);

	if ((totalBytes * repeat) > (contentLength - contentLow - contentHigh))
	{
		edlinPrintMessage(EDLMES_MEMFUL);
		edlinArgLength = 0;
		return;
	}

	edlinSeek(dest);

	currentLine = dest;

	source = edlinLineFromIndex(first);

	while (repeat--)
	{
		memcpy(contentStore + contentLow, source, totalBytes);

		contentLow += totalBytes;
		contentLowCount += count;
	}
}

void edlinMove(void)
{
	size_t first = currentLine, last = currentLine, dest = currentLine, count = 1;
	size_t totalCount = contentLowCount + contentHighCount;
	size_t byteCount;
	const unsigned char* source;

	if (edlinArgCount > 3)
	{
		edlinPrintMessage(EDLMES_BADCOM);
		edlinArgLength = 0;
		return;
	}

	if (edlinArgCount && !edlinIndexFromArg(edlinArgList[0], &first))
	{
		return;
	}

	if (edlinArgCount > 1 && !edlinIndexFromArg(edlinArgList[1], &last))
	{
		return;
	}

	if (edlinArgCount > 2 && !edlinIndexFromArg(edlinArgList[2], &dest))
	{
		return;
	}

	if (((dest >= first) && (dest <= last))
		|| (last < first)
		|| (first >= totalCount)
		|| (last >= totalCount))
	{
		edlinPrintMessage(EDLMES_BADCOM);
		return;
	}

	count = last + 1 - first;

	byteCount = edlinLengthFromIndex(first, count);

	if (byteCount > (contentLength - contentLow - contentHigh))
	{
		edlinPrintMessage(EDLMES_MEMFUL);
		edlinArgLength = 0;
		return;
	}

	edlinSeek(dest);

	source = edlinLineFromIndex(first);

	memcpy(contentStore + contentLow, source, byteCount);

	contentLow += byteCount;
	contentLowCount += count;

	if (dest > first)
	{
		currentLine = dest - count;
		edlinSeek(first);
	}
	else
	{
		currentLine = dest;
		edlinSeek(first + count);
	}

	contentHigh -= byteCount;
	contentHighCount -= count;
}

void edlinMerge(void)
{
	FILE* fp;
	int err;
	size_t index = currentLine;
	char name[256];
	unsigned char* store;
	size_t contentNew = 0, contentNewCount = 0;

	if ((edlinArgCount > 1) || !edlinArgLength)
	{
		edlinPrintMessage(EDLMES_BADCOM);
		return;
	}

	if (edlinArgCount && !edlinIndexFromArg(edlinArgList[0], &index))
	{
		return;
	}

	memcpy(name, edlinArgValue, edlinArgLength);
	name[edlinArgLength] = 0;

#if defined(_WIN32) && !defined(__WATCOMC__)
	err = fopen_s(&fp, name, "r");
#else
	fp = fopen(name, "r");
	err = fp ? 0 : errno;
#endif

	if (err || fp == NULL)
	{
		edlinPrintError(err);
		return;
	}

	edlinSeek(index);

	currentLine = index;

	store = contentStore + contentLow;

	while (TRUE)
	{
		size_t room = contentLength - contentLow - contentHigh - contentNew;
		int i;

		if (room < 256)
		{
			edlinPrintMessage(EDLMES_MRGERR);
			store = NULL;
			break;
		}

		i = edlinFileRead(store, fp);

		if (i > 0)
		{
			size_t len = 1 + store[0];
			contentNew += len;
			contentNewCount++;
			store += len;
		}

		if (i < 2)
		{
			break;
		}
	}

	if (store)
	{
		contentLow += contentNew;
		contentLowCount += contentNewCount;
	}

	fclose(fp);
}

int edlinReplaceLine(size_t index, const unsigned char* matchValue, size_t matchLen)
{
	const unsigned char* replaceValue = matchValue + 1 + matchLen;
	size_t replaceLen = edlinArgLength - matchLen - 1;
	const unsigned char* p = edlinLineFromIndex(index);
	size_t offset = 0;

	while ((offset + matchLen) <= p[0])
	{
		if (!memcmp(p + offset + 1, matchValue, matchLen))
		{
			int change = 1;
			size_t remaining = p[0] - matchLen - offset;
			const unsigned char* src = p + 1;
			struct edlinReadline line = READLINE_INIT;

			line.insertMode = 0;
			line.original = NULL;
			line.originalLen = 0;
			line.currentColumn = line.leftColumn;
			line.lineLen = (unsigned int)offset;

			if (line.lineLen)
			{
				memcpy(line.line, src, line.lineLen);
				src += line.lineLen;
			}

			src += matchLen;

			if (replaceLen)
			{
				if (line.lineLen + replaceLen > 255)
				{
					edlinPrintMessage(EDLMES_TOOLNG);
					return -1;
				}

				memcpy(line.line + line.lineLen, replaceValue, replaceLen);
				line.lineLen += (unsigned int)replaceLen;
			}

			if (remaining)
			{
				if (line.lineLen + remaining > 255)
				{
					edlinPrintMessage(EDLMES_TOOLNG);
					return -1;
				}

				memcpy(line.line + line.lineLen, src, remaining);
				line.lineLen += (unsigned int)remaining;
			}

			line.leftColumn = edlinPrintLinePrompt(index);
			edlinPaint(&line);
			edlinPrintLine(NULL, 0);

			if (edlinOption)
			{
				edlinPrintMessage(EDLMES_ASK);

				change = edlinConfirm();

				if (change < 0)
				{
					return -1;
				}
			}

			if (change)
			{
				if (line.lineLen > p[0])
				{
					size_t diff = line.lineLen - p[0];

					if (diff > (contentLength - contentLow - contentHigh))
					{
						edlinPrintMessage(EDLMES_MEMFUL);

						return -1;
					}
				}

				if (p[0] != line.lineLen)
				{
					contentHigh -= p[0];
					contentHigh += line.lineLen;
					p = contentStore + contentLength - contentHigh;
					contentStore[contentLength - contentHigh] = line.lineLen;
				}

				if (line.lineLen)
				{
					memcpy(contentStore + contentLength - contentHigh + 1, line.line, line.lineLen);
				}

				offset += replaceLen;
			}
			else
			{
				offset++;
			}
		}
		else
		{
			offset++;
		}
	}

	return 0;
}

void edlinReplace(void)
{
	size_t index = currentLine + 1;
	size_t count = contentLowCount + contentHighCount;
	size_t total = contentLowCount + contentHighCount;
	const unsigned char* matchValue = edlinArgValue;
	int i = findChar(matchValue, 0x1A, edlinArgLength);
	size_t matchLen = i;

	if ((edlinArgCount > 2) || (edlinArgLength < 2) || (i < 0))
	{
		edlinPrintMessage(EDLMES_BADCOM);
		return;
	}

	if (edlinArgCount)
	{
		if (!edlinIndexFromArg(edlinArgList[0], &index))
		{
			return;
		}

		if (edlinArgCount > 1)
		{
			size_t to = count;

			if (!edlinIndexFromArg(edlinArgList[1], &to))
			{
				return;
			}

			if (to < index)
			{
				count = 0;
			}
			else
			{
				count = 1 + to - index;
			}
		}
	}
	else
	{
		if (index > count)
		{
			count = 0;
		}
		else
		{
			count -= index;
		}
	}

	if (index >= total)
	{
		return;
	}

	if ((index + count) >= total)
	{
		count = total - index;
	}

	if (count)
	{
		const unsigned char* line = edlinLineFromIndex(index);

		while (count--)
		{
			if (edlinMatchLine(line, matchValue, matchLen))
			{
				edlinSeek(index);

				if (edlinReplaceLine(index, matchValue, matchLen) < 0)
				{
					return;
				}

				line = edlinLineFromIndex(index);
			}

			line += (1 + line[0]);
			index++;
		}
	}
}

const char* makeFileName(const char* original, char* name, size_t nameLen, const char* ext)
{
	char dot = ext[0];
	size_t len = strlen(original);
	if ((len + strlen(ext) + 1) > nameLen) return NULL;

	memcpy(name, original, len);
	name[len] = 0;

	while (len--)
	{
		char c = name[len];
		if (c == dot)
		{
			strcpy_s(name + len, nameLen - len, ext);
			return name;
		}

#ifdef EDLIN_DOSFILESYSTEM
		if (c == '/' || c == '\\' || c == ':')
#else
		if (c == '/')
#endif
		{
			break;
		}
	}

	strcat_s(name, nameLen, ext);

	return name;
}

int edlinWrite(void)
{
	size_t count = edlinArgCount ? edlinArgList[0] : 0;
	size_t total = contentLowCount + contentHighCount;
	const unsigned char* p;

	if (edlinArgCount != 1)
	{
		edlinPrintMessage(EDLMES_BADCOM);
		edlinArgLength = 0;
		return -1;
	}

	if (count > total)
	{
		count = total;
	}

	if (count == 0)
	{
		return 0;
	}

	edlinSeek(0);

	p = contentStore + contentLength - contentHigh;

	while (count--)
	{
		size_t len = *p++;
		size_t written, toWrite = len + 1;
		char buf[256];

		if (len)
		{
			memcpy(buf, p, len);
		}

		buf[len] = '\n';

		written = fwrite(buf, 1, toWrite, fileTmpWrite);

		if (written != toWrite)
		{
			edlinPrintError(errno);
			edlinArgLength = 0;
			return -1;
		}

		if (currentLine)
		{
			currentLine--;
		}

		contentHighCount--;
		contentHigh -= toWrite;
		p += len;
	}

	return 0;
}

int edlinExit(void)
{
	char fileName[MAX_PATH];

	while (fileMainRead || contentLowCount || contentHighCount)
	{
		int i;

		edlinArgList[0] = (long)(contentLowCount + contentHighCount);
		edlinArgCount = 1;

		i = edlinWrite();

		if (i) return i;

		if (fileMainRead)
		{
			edlinAppend();
		}
	}

	if (edlinNewFile == 0)
	{
		if (!makeFileName(mainFileName, fileName, sizeof(fileName), ".bak"))
		{
			return -1;
		}

#ifdef EDLIN_DOSFILESYSTEM
#	ifdef _WIN32
		DeleteFileA(fileName);
#	else
		unlink(fileName);
#	endif
#endif

#ifdef _WIN32
		if (!MoveFileA(mainFileName, fileName))
		{
			edlinPrintWin32Error(GetLastError());
			edlinArgLength = 0;

			return -1;
		}
#else
		if (rename(mainFileName, fileName))
		{
			edlinPrintError(errno);
			edlinArgLength = 0;

			return -1;
		}
#endif
	}

	if (fileTmpWrite)
	{
		fclose(fileTmpWrite);
		fileTmpWrite = NULL;
	}

	if (tmpFileName[0])
	{
#ifdef _WIN32
		if (!MoveFileA(tmpFileName, mainFileName))
		{
			edlinPrintWin32Error(GetLastError());
			edlinArgLength = 0;

			return -1;
		}
#else
		if (rename(tmpFileName, mainFileName))
		{
			edlinPrintError(errno);
			edlinArgLength = 0;

			return -1;
		}
#endif

		tmpFileName[0] = 0;
	}

	return 0;
}

int edMainLoop(int argc, char** argv)
{
	int err;
	char running = 1;

	while (--argc)
	{
		const char* p = *++argv;

#ifdef EDLIN_DOSFILESYSTEM
		if (_stricmp(p, "/B"))
#endif
		{
			if (mainFileName)
			{
				edlinPrintMessage(EDLMES_BADCOM);
				return 0;
			}

			mainFileName = p;
		}
	}

	if (!mainFileName)
	{
		edlinPrintMessage(EDLMES_NDNAME);

		return 0;
	}

#if defined(_WIN32) && !defined(__WATCOMC__)
	err = fopen_s(&fileMainRead, mainFileName, "r");
#else
	fileMainRead = fopen(mainFileName, "r");
	err = fileMainRead ? 0 : errno;
#endif

	if (err || fileMainRead == NULL)
	{
		if (err != ENOENT)
		{
			edlinPrintError(err);

			return 0;
	}

		edlinNewFile = 1;
}

	makeFileName(mainFileName, tmpFileName, sizeof(tmpFileName), ".$$$");

#if defined(_WIN32) && !defined(__WATCOMC__)
	err = fopen_s(&fileTmpWrite, tmpFileName, "w");
#else
	fileTmpWrite = fopen(tmpFileName, "w");
	err = fileTmpWrite ? 0 : errno;
#endif

	if (err || fileTmpWrite == NULL)
	{
		edlinPrintError(err);

		return 0;
	}

#ifdef EDLIN_MALLOC
	contentLength = EDLIN_MALLOC;
#else
	contentLength = 0x100000;
#endif

	contentStore = malloc(contentLength);

	if (contentStore == NULL)
	{
		edlinPrintMessage(EDLMES_MEMFUL);
		return 0;
	}

	if (fileMainRead)
	{
		edlinAppend();
	}
	else
	{
		edlinPrintMessage(EDLMES_NEWFIL);
	}

	while (running)
	{
		int i;
		struct edlinReadline line = READLINE_INIT;
		line.insertMode = 0 /*insertMode*/;
		line.original = edlinLastCommand;
		line.originalLen = edlinLastCommandLen;

		line.leftColumn = edlinPrintMessage(EDLMES_PROMPT);

		i = edlinReadLine(&line);

		if (i < 0) break;

		if (line.exitChar == '\r')
		{
			edlinArgCount = 0;
			edlinCommand = 0;
			edlinOption = 0;

			if (i)
			{
				edlinLastCommandLen = line.lineLen;

				if (edlinLastCommandLen)
				{
					if (edlinLastCommandLen > sizeof(edlinLastCommand))
					{
						edlinLastCommandLen = sizeof(edlinLastCommand);
					}

					memcpy(edlinLastCommand, line.line, edlinLastCommandLen);
				}

				edlinArgValue = line.line;
				edlinArgLength = line.lineLen;

				while (edlinArgLength)
				{
					if (edlinParseCmd(edlinArgValue, edlinArgLength) < 0)
					{
						edlinPrintMessage(EDLMES_BADCOM);
						break;
					}
					else
					{
#if defined(_WIN32) && defined(_DEBUG)
						{
							char buf[256];
							sprintf_s(buf, sizeof(buf), "args %d {%ld,%ld,%ld,%ld}, %04X\r\n", edlinArgCount, edlinArgList[0], edlinArgList[1], edlinArgList[2], edlinArgList[3], edlinCommand);
							OutputDebugStringA(buf);
						}
#endif
						switch (edlinCommand)
						{
						case 0:
							edlinEditSingleLine();
							break;

						case 'A':
							edlinAppend();
							break;

						case 'C':
							edlinCopy();
							break;

						case 'D':
							edlinDelete();
							break;

						case 'E':
							if (!edlinExit())
							{
								edlinArgLength = 0;
								running = 0;
							}
							break;

						case 'I':
							edlinInsert();
							break;

						case 'L':
							edlinListLines();
							break;

						case 'M':
							edlinMove();
							break;

						case 'P':
							edlinPage();
							break;

						case 'Q':
							edlinPrintMessage(EDLMES_QMES);
							edlinFlush();
							running = (0 == edlinConfirm());
							break;

						case 'R':
							edlinReplace();
							edlinArgLength = 0;
							break;

						case 'S':
							edlinSearch();
							edlinArgLength = 0;
							break;

						case 'T':
							edlinFlush();
							edlinMerge();
							edlinArgLength = 0;
							break;

						case 'W':
							edlinWrite();
							break;

#if defined(_WIN32) && defined(_DEBUG)
						case 'Z':
							if (edlinCommand)
							{
								char buf[256];
								int i = sprintf_s(buf, sizeof(buf), "Z=%d", edlinArgCount);
								for (unsigned int k = 0; k < edlinArgCount; k++)
								{
									i += sprintf_s(buf + i, sizeof(buf) - i, ",%ld", edlinArgList[k]);
								}
								if (edlinOption)
								{
									i += sprintf_s(buf + i, sizeof(buf) - i, ",?");
								}
								if (edlinArgLength)
								{
									i += sprintf_s(buf + i, sizeof(buf) - i, ":%d", (int)edlinArgLength);
								}
								edlinPrintLine(buf, i);
							}
							break;
#endif

						default:
							edlinPrintMessage(EDLMES_BADCOM);
							edlinArgLength = 0;
							break;
						}
					}

					if (!running)
					{
						break;
					}

					while (edlinArgLength)
					{
						char c = *edlinArgValue;

						if (c == ' ' || c == '\t')
						{
							edlinArgValue++;
							edlinArgLength--;
						}
						else
						{
							break;
						}
					}
				}
			}
			else
			{
				edlinLastCommandLen = 0;
				edlinEditSingleLine();
			}
		}

		edlinArgValue = NULL;
		edlinArgLength = 0;
	}

	return 0;
}
