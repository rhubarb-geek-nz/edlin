/************************************
 * Copyright (c) 2024 Roger Brown.
 * Licensed under the MIT License.
 */

#ifdef _WIN32
#else
#	define VK_DELETE	1000
#	define VK_INSERT	1001
#	define VK_F3		1002
#	define VK_LEFT		1003
#	define VK_RIGHT		1004
#	define VK_ESCAPE	1005
#	define VK_PRIOR		1007
#	define VK_NEXT		1008
#	define VK_HOME		1009
#	define VK_F1		1010
#	define VK_F2		1011
#	define VK_F4		1012
#	define VK_F5		1013
#	define VK_F6		1014
#	define VK_F7		1015
#	define VK_F8		1016
#	define VK_F9		1017
#	define VK_F10		1018
#	define VK_UP		1019
#	define VK_END		1020
#	define VK_DOWN		1021
#endif

struct edlinReadline
{
	unsigned int leftColumn, tabWidth, lineLen, currentColumn, originalLen, originalConsumed;
	char insertMode;
	const unsigned char* original;
	unsigned char line[256];
	wchar_t exitChar;
};

#define READLINE_INIT {0,8,0,0,0,0,0,NULL}

extern int edlinReadLine(struct edlinReadline *line);
extern int edlinConfirm(void);
extern void edlinPaint(struct edlinReadline* line);
