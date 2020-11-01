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

#include "Utils.h" // API

#include <arpa/inet.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <netdb.h>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "Logger.h"

#define PATH_SEPERATOR '/'

std::string Utils::pathJoin(const std::string& rPath, const std::string& rFilename)
{
    if (rPath.back() == PATH_SEPERATOR || rFilename.front() == PATH_SEPERATOR) {
        return rPath + rFilename;
    }
    return rPath + PATH_SEPERATOR + rFilename;
}

std::string Utils::pathBasename(const std::string& rPath)
{
    std::string::size_type pos = rPath.rfind(PATH_SEPERATOR);
    if (pos == std::string::npos) {
        return rPath;
    }
    return rPath.substr(pos + 1);
}

std::string Utils::pathDirname(const std::string& rPath)
{
    std::string::size_type pos = rPath.rfind(PATH_SEPERATOR);
    if (pos == std::string::npos) {
        return "";
    }
    return rPath.substr(0, pos);
}

std::string Utils::pathAbsname(const std::string& rPath)
{
    char resolved_path[PATH_MAX];
    if (realpath(rPath.c_str(), resolved_path) != 0) {
        return resolved_path;
    }
    return "";
}

// file or directory
bool Utils::pathExists(const std::string& rPath)
{
    struct stat st;
    if (stat(rPath.c_str(), &st) == 0) {
        return true;
    }
    return false;
}

bool Utils::startsWith(const std::string& rStr, const char* pPrefix)
{
    return rStr.find(pPrefix) == 0 ? true : false;
}

static void leftTrim(std::string* pStr)
{
    std::string::size_type pos = pStr->find_last_not_of(" \t\r\n");
    if (pos != std::string::npos) {
        pStr->erase(pos + 1);
    }
}

static void rightTrim(std::string* pStr)
{
    std::string::size_type pos = pStr->find_first_not_of(" \t\r\n");
    if (pos != std::string::npos) {
        pStr->erase(0, pos);
    }
}

void Utils::trim(std::string* pStr)
{
    leftTrim(pStr);
    rightTrim(pStr);
}

std::string Utils::escapeSqString(const std::string& rStr)
{
    std::size_t n = rStr.length();
    std::string escaped;
    escaped.reserve(n * 2); // pessimistic preallocation
    for (size_t i = 0; i < n; ++i) {
        if (rStr[i] == '\\' || rStr[i] == '\'') {
            escaped += '\\';
        }
        escaped += rStr[i];
    }
    return escaped;
}

void Utils::replaceAll(std::string* pStr, const std::string& rFrom, const std::string& rTo)
{
    size_t startPos = 0;
    while ((startPos = pStr->find(rFrom, startPos)) != std::string::npos) {
        pStr->replace(startPos, rFrom.length(), rTo);
        startPos += rTo.length(); // Handles case where 'to' is a substring of 'from'
    }
}
void Utils::parseCallerID(std::string& rData, std::vector<std::pair<std::string, std::string>>* pResult)
{
    // DATE=0306
    // TIME=1517
    // NMBR=0123456789
    // NAME=aasdasdd

    // split by newline
    std::stringstream ss(rData);
    std::string to;
    while (std::getline(ss, to, '\n')) {
        // key=val
        std::string::size_type pos = to.find('=');
        if (pos == std::string::npos)
            continue;
        std::string key = to.substr(0, pos);
        std::string val = "";
        if (pos + 1 < to.length()) {
            val = to.substr(pos + 1, to.length() - pos);
        }
        Utils::trim(&key);
        Utils::trim(&val);
        std::pair<std::string, std::string> p(key, val);
        pResult->push_back(p);
    }
}

bool Utils::executeCommand(const std::string& cmd, std::string* pRes)
{
    Logger::debug("executing(%s)...", cmd.c_str());

    FILE* fp = popen(cmd.c_str(), "r");
    if (fp == nullptr) {
        Logger::warn("popen failed (%s)", strerror(errno));
        return false;
    }

    std::string res = "";
    char buf[128];
    while (fgets(buf, sizeof(buf), fp) != nullptr) {
        res += buf;
    }
    Utils::trim(&res);

    int status = pclose(fp);
    if (status != 0) {
        Logger::warn("%s failed (%s)", cmd.c_str(), res.c_str());
        return false;
    }

    Logger::debug("result: '%s'", res.c_str());
    *pRes = res;
    return true;
}
