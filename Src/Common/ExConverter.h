#pragma once

#include "UnicodeString.h"

struct CodePageInfo
{
	int codepage;
	wchar_t desc[256];
    wchar_t fixedWidthFont[256];
    char bGDICharset;
};

struct IExconverter
{
	virtual bool initialize() = 0;
	virtual bool convert(int srcCodepage, int dstCodepage, const unsigned char * src, size_t * srcbytes, unsigned char * dest, size_t * destbytes) = 0;
	virtual bool convertFromUnicode(int dstCodepage, const wchar_t * src, size_t * srcchars, char * dest, size_t *destbytes) = 0;
	virtual bool convertToUnicode(int srcCodepage, const char * src, size_t * srcbytes, wchar_t * dest, size_t *destchars) = 0;
	virtual void clearCookie() = 0;
	virtual int detectInputCodepage(int autodetectType, int defcodepage, const char *data, size_t size) = 0;
	virtual int enumCodePages(CodePageInfo *cpinfo, int count) = 0;
	virtual bool getCodepageFromCharsetName(const String& sCharsetName, int& codepage) = 0;
	virtual bool getCodepageDescription(int codepage, String& sCharsetName) = 0;
	virtual bool isValidCodepage(int codepage) = 0;
	virtual bool getCodePageInfo(int codePage, CodePageInfo *pCodePageInfo) = 0;
};

struct Exconverter
{
	static IExconverter *getInstance();
};
