/** 
 * @file  Environment.h
 *
 * @brief Declaration file for Environment-related routines.
 */
#pragma once

#include "UnicodeString.h"

void env_SetTempPath(const String& path);
String env_GetTempPath();
String env_GetTempFileName(const String& lpPathName, const String& lpPrefixString,
		int * pnerr = NULL);
String env_GetTempChildPath();
void env_SetProgPath(const String& path);
String env_GetProgPath();

String env_GetWindowsDirectory();
String env_GetMyDocuments();
String env_GetSystemTempPath();

String env_GetPerInstanceString(const String& name);

bool env_LoadRegistryFromFile(const String& sRegFilePath);
bool env_SaveRegistryToFile(const String& sRegFilePath, const String& sRegDir);
