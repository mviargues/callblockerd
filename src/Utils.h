/*
 callblocker - blocking unwanted calls from your home phone
 Copyright (C) 2015-2020 Patrick Ammann <pammann@gmx.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <string>
#include <vector>
#include <sys/time.h>

class Utils {
public:
    // file system
    static std::string pathJoin(const std::string& rPath, const std::string& rFilename);
    static std::string pathBasename(const std::string& rPath);
    static std::string pathDirname(const std::string& rPath);
    static std::string pathAbsname(const std::string& rPath);
    static bool pathExists(const std::string& rPath);
    static bool fileCopy(const std::string& rFrom, const std::string& rTo);

    // string
    static bool startsWith(const std::string& rStr, const char* pPrefix);
    static void trim(std::string* pStr);
    static std::string escapeSqString(const std::string& rStr);
    static void replaceAll(std::string* pStr, const std::string& rFrom, const std::string& rTo);

    // phone number
    static void parseCallerID(std::string& rData, std::vector<std::pair<std::string, std::string>>* pResult);
};

#endif
