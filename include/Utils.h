#ifndef DATEUTILS_H
#define DATEUTILS_H

#include <string>
#include <vector>

namespace DateUtils
{
    extern const std::vector<std::pair<int, std::string>> Months;
    std::string today();
    bool isValidDate(std::string& text);
    bool isValidDateLog(std::string& text);
}

namespace Utils
{
    bool isNumber(std::string& number);
    bool isNumberLog(std::string& number);
    void prepareOptionalIntFromString(std::string& number);
}

#endif