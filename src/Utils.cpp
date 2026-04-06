#include "Utils.h"
#include "Logger.h"

#include <regex>

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

    bool isValidDateLog(std::string& text)
    {
        bool ret = isValidDate(text);
        if(!ret)
        {
            Logger::getInstance().error(text + " is not a valid date!");
        }
        return ret;
    }

    bool isValidDate(std::string& text)
    {
        std::regex pattern(R"(^\d{4}-\d{2}-\d{2}$)");
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
    bool isNumberLog(std::string& number)
    {
        bool ret = isNumber(number);
        if(!ret)
        {
            Logger::getInstance().error(number + " is not a number!");
        }
        return ret;
    }
    bool isNumber(std::string& number)
    {
        std::regex pattern(R"(-?[0-9]+)");
        return std::regex_match(number, pattern);
    }
}
