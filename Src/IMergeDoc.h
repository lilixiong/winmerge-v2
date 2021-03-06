#pragma once

#include "UnicodeString.h"

class CDirDoc;

struct IMergeDoc
{
	virtual void SetDirDoc(CDirDoc *pDirDoc) = 0;
	virtual bool CloseNow(void) = 0;
	virtual bool GenerateReport(const TCHAR *path) = 0;
	virtual void DirDocClosing(CDirDoc * pDirDoc) = 0;
	virtual void CheckFileChanged() = 0;
};

