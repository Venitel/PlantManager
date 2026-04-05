#ifndef DATEUTILS_H
#define DATEUTILS_H

#include <string>
#include <vector>

namespace DateUtils
{
    extern const std::vector<std::pair<int, std::string>> Months;
    std::string today();
    bool isValidDate(std::string& text);
}

#endif