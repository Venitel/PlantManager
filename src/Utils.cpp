#include "Utils.h"
#include "Logger.h"
#include "Database.h"

#include <regex>
#include <algorithm>

namespace DateUtils
{
    const std::vector<std::pair<int, std::string>> Months = 
    {
        {1,  "January"},
        {2,  "February"},
        {3,  "March"},
        {4,  "April"},
        {5,  "May"},
        {6,  "June"},
        {7,  "July"},
        {8,  "August"},
        {9,  "September"},
        {10, "October"},
        {11, "November"},
        {12, "December"}
    };

    std::string today() 
    {
        std::time_t now = std::time(nullptr);
        char buf[11];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", std::localtime(&now));
        return buf; 
    }

    bool isValidDateLog(const std::string& text)
    {
        bool ret = isValidDate(text);
        if(!ret)
        {
            Logger::getInstance().error(text + " is not a valid date!");
        }
        return ret;
    }

    bool isValidDate(const std::string& text)
    {
        const std::regex pattern(R"(^\d{4}-\d{2}-\d{2}$)");
        if(!std::regex_match(text, pattern)) {return false;}

        int year = std::stoi(text.substr(0, 4));
        bool leapYear = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;

        int month = std::stoi(text.substr(5, 2));
        if (month < 1 || month > 12) {return false;}

        int daysInMonth[] = { 31, 28, 31, 30, 31, 30,
                            31, 31, 30, 31, 30, 31 };

        int day = std::stoi(text.substr(8, 2));
        if (month == 2 && leapYear) 
        {
            if (day < 1 || day > 29) {return false;}
        } 
        else 
        {
            if (day < 1 || day > daysInMonth[month - 1]) {return false;}
        }

        return true;
    }
}

namespace Utils
{
    bool isNumberLog(const std::string& number)
    {
        bool ret = isNumber(number);
        if(!ret)
        {
            Logger::getInstance().error(number + " is not a number!");
        }
        return ret;
    }

    bool isNumber(const std::string& number)
    {
        const std::regex pattern(R"(-?[0-9]+)");
        return std::regex_match(number, pattern);
    }

    void prepareOptionalIntFromString(std::string& number)
    {
        if(number.empty())
        {
            number = "-1";
        }
    }
}

namespace CommonCache
{
    std::map<std::string, int>& getSettings()
    {
        static std::map<std::string, int> settings;

        return settings;
    }

    std::map<Field::DataType, std::vector<std::pair<int, std::string>>>& getIdNames()
    {
        static std::map<Field::DataType, std::vector<std::pair<int, std::string>>> IdNames =
        {
            {Field::DataType::Species, Database::getInstance().getAllKeys(Field::DataType::Species)},
            {Field::DataType::Schedule, Database::getInstance().getAllKeys(Field::DataType::Schedule)},
        };

        return IdNames;
    }

    std::string getName(const Field::DataType dataType, const int id)
    {
        std::vector<std::pair<int, std::string>>& names = getIdNames()[dataType];
        auto it = std::find_if(names.begin(), names.end(), [&](auto& pair) {return id == pair.first;});
        if(it != names.end())
        {
            return it->second;
        }
        return "";
    }

    void updateElement(const Field::DataType dataType, const std::pair<int, std::string>& changedIdName)
    {
        std::vector<std::pair<int, std::string>>& names = getIdNames()[dataType];
        {
            auto it = std::find_if(names.begin(), names.end(), [&](auto& pair) {return changedIdName.first == pair.first;});
            if(it != names.end())
            {
                it->second = changedIdName.second;
            }
        }
    }

    void deleteElement(const Field::DataType dataType, const int id)
    {
        std::vector<std::pair<int, std::string>>& names = getIdNames()[dataType];
        {
            auto it = std::find_if(names.begin(), names.end(), [&](auto& pair) {return id == pair.first;});
            if(it != names.end())
            {
                names.erase(it);
            }
        }
    }
}