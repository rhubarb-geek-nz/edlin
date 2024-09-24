/************************************
 * Copyright (c) 2024 Roger Brown.
 * Licensed under the MIT License.
 */

#ifdef _WIN32
#else
#	define CP_UTF8		65001
#	define CP_ACP		0
#endif

int mbcsLen(unsigned int cp, const unsigned char* p, size_t avail);
wchar_t mbcsToChar(unsigned int cp, const unsigned char* p, int len);
int mbcsFromChar(unsigned int cp, wchar_t ch, unsigned char* p);
