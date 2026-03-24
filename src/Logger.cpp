#include "Logger.h"

#include <ctime>
#include <iostream>
#include <filesystem>

Logger& Logger::getInstance() 
{
    static Logger logger;
    return logger;
}

bool Logger::open(const std::string& file)
{
    std::filesystem::path path = std::filesystem::current_path() / file;
    file_.open(path, std::ios::app);
    if(!file_.is_open())
    {
        return false;
    }
    return true;
}

std::string Logger::timestamp() 
{
    std::time_t now = std::time(nullptr);
    char time[20];
    std::strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return time;
}

void Logger::info(const std::string& msg) 
{
    file_ << "[" << timestamp() << "] [INFO]  " << msg << "\n";
}

void Logger::error(const std::string& msg) 
{
    file_ << "[" << timestamp() << "] [ERROR] " << msg << "\n";
}