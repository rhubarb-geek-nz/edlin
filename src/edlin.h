/************************************
 * Copyright (c) 2024 Roger Brown.
 * Licensed under the MIT License.
 */

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

extern char edlinCommand;               /* command, uppercase */
extern char edlinOption;                /* if ? was present */
extern unsigned int edlinArgCount;      /* numerics before command */
extern long edlinArgList[];             /* numerics before command */
extern const char* edlinArgValue;       /* parameter after command */
extern unsigned int edlinArgLength;     /* parameter after command */

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
extern int edlinPrint(const char*, size_t);
extern int edlinPrintLine(const char*, size_t);
extern int edlinPrintMessage(int message);
extern void edlinFlush(void);
extern const char edlinControlChar[];

extern int edMainLoop(int argc, char** argv);
extern void edExit(void);
extern unsigned char* edlinLineFromIndex(size_t);
extern int findChar(const char* p, char c, size_t len);

/********************************
 * fin
 */
