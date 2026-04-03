#ifndef DATEUTILS_H
#define DATEUTILS_H

#include <string>
#include <vector>

namespace DateUtils
{
    extern const std::vector<std::pair<int, std::string>> Months;
}

bool isValidDate(std::string& text);

#endif