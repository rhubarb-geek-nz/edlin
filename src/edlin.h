/************************************
 * Copyright (c) 2024 Roger Brown.
 * Licensed under the MIT License.
 */

#ifdef _WIN32
#	ifdef __WATCOMC__
#		define _countof(x)     	(sizeof(x)/sizeof(x[0]))
#		define strcat_s(a,b,c)	strncat(a,c,b)
#		define strcpy_s(a,b,c)	strncpy(a,c,b)
#	endif
#else
#	define MAX_PATH	260
#	define FALSE 0
#	define TRUE	1
#	define _countof(x)     		(sizeof(x)/sizeof(x[0]))
#	define strcat_s(a,b,c)		strncat(a,c,b)
#	define strcpy_s(a,b,c)		strncpy(a,c,b)
#endif

extern unsigned int consoleCodePage, fileCodePage;
extern unsigned char insertMode;

/************************************
 * format is [len][num chars] for each line, max 253 bytes per line
 * contentStore is split into low section and high section
 */

extern unsigned char* contentStore;
extern size_t contentLength;            /* total length of contentStore */
extern size_t contentLow;               /* low content length */
extern size_t contentHigh;              /* high content length */
extern size_t currentLine;              /* zero based line number of current line */
extern size_t contentLowCount;          /* count of lines in low area */
extern size_t contentHighCount;         /* count of lines in high area */

/*******************************
 * parsed command elements 
 */

extern char edlinCommand;                    /* command, uppercase */
extern char edlinOption;                     /* if ? was present */
extern unsigned int edlinArgCount;           /* numerics before command */
extern long edlinArgList[];                  /* numerics before command */
extern const unsigned char* edlinArgValue;   /* parameter after command */
extern unsigned int edlinArgLength;          /* parameter after command */

/*******************************
 * file data
 */

extern FILE* fileMainRead;
extern FILE* fileMergeRead;
extern const char* mainFileName;

/*******************************
 * functions
 */

extern int edlinStdIn(wchar_t* ch);
extern int edlinChar(unsigned short* vk, wchar_t* ch);
extern int edlinPrint(const unsigned char*, size_t);
extern int edlinPrintLine(const unsigned char*, size_t);
extern int edlinPrintMessage(int message);
extern const char *edlinGetMessage(int message);
extern void edlinFlush(void);
extern const char edlinControlChar[];

extern int edMainLoop(int argc, char** argv);
extern void edExit(void);
extern unsigned char* edlinLineFromIndex(size_t);
extern int findChar(const unsigned char* p, char c, size_t len);

#ifdef _WIN32
extern void edlinPrintWin32Error(DWORD err);
#endif

/********************************
 * fin
 */
