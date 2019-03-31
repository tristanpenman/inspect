#pragma once

#include <string>

inline std::string getStr(const char * beg, const char * end)
{
    return std::string(beg).substr(0, end-beg);
}
