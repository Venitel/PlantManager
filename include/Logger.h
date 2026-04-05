#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>

class Logger {
public:
    static Logger& getInstance();

    bool open(const std::string& path);
    std::string timestamp(); 
    void info(const std::string& msg);
    void error(const std::string& msg);
    void query(const std::string& msg);
    void result(const std::string& msg);

private:
    std::ofstream file_;
};


#endif