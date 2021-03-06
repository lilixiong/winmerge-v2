#pragma once

#include <vector>
#include "UnicodeString.h"
#include "dllpstub.h"
#include "../ArchiveSupport/Merge7z/Merge7z.h"

struct Merge7zFormatRegister
{
	Merge7zFormatRegister(Merge7z::Format *(*pfnGuessFormat)(const String& path))
	{
		Add(pfnGuessFormat);
	}
	static void Add(Merge7z::Format *(*pfnGuessFormat)(const String& path))
	{
		optionalFormats.push_back(pfnGuessFormat);
	}
	static Merge7z::Format *GuessFormat(const String& path)
	{
		for (int i = 0; i < Merge7zFormatRegister::optionalFormats.size(); ++i)
		{
			Merge7z::Format* pFormat = Merge7zFormatRegister::optionalFormats[i](path);
			if (pFormat)
				return pFormat;
		}
		return NULL;
	}

	static std::vector<Merge7z::Format *(*)(const String& path)> optionalFormats;
};

