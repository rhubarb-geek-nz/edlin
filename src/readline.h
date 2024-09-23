/************************************
 * Copyright (c) 2024 Roger Brown.
 * Licensed under the MIT License.
 */

struct edlinReadline
{
	unsigned int leftColumn, tabWidth, lineLen, currentColumn, originalLen, originalConsumed;
	char insertMode;
	const char* original;
	char line[256];
	wchar_t exitChar;
};

#define READLINE_INIT {0,8,0,0,0,0,0,NULL}

extern int edlinReadLine(struct edlinReadline *line);
extern int edlinConfirm(const char* str, wchar_t* pch);
extern void edlinPaint(struct edlinReadline* line);
