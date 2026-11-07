#pragma once

#include <string>

inline std::string getStr(const char* begin, const char* end)
{
    return std::string(begin).substr(0, end - begin);
}
