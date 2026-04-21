#ifndef DATEUTILS_H
#define DATEUTILS_H

#include "Record.h"

#include <string>
#include <vector>
#include <map>

namespace DateUtils
{
    extern const std::vector<std::pair<int, std::string>> Months;
    std::string today();
    bool isValidDate(const std::string& text);
    bool isValidDateLog(const std::string& text);
}

namespace Utils
{
    bool isNumber(const std::string& number);
    bool isNumberLog(const std::string& number);
    void prepareOptionalIntFromString(std::string& number);
}

namespace CommonCache
{
    std::map<Field::DataType, std::vector<std::pair<int, std::string>>>& getIdNames();
    std::map<std::string, int>& getSettings();
    void updateElement(const Field::DataType dataType, const std::pair<int, std::string>& changedIdName);
    void deleteElement(const Field::DataType dataType, const int id);

    std::string getName(const Field::DataType dataType, const int id);
}

#endif