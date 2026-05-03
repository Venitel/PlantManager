#include "DbTestObject.h"
#include <filesystem>
#include "Database.h"
#include "Logger.h"

const bool log = false;

void removeTestDb()
{
    std::error_code error;
    std::filesystem::remove("PMTests.db", error);
}

DbTestObject::DbTestObject() 
{
    removeTestDb();

    std::filesystem::copy_file("../Tests/TestDatabaseSource.db", "PMTests.db", std::filesystem::copy_options::overwrite_existing);
    Database::getInstance().open("PMTests.db");
    if(log)
    {
        Logger::getInstance().open("PMTests.log");
    }
}

DbTestObject::~DbTestObject()
{
    Database::getInstance().close();
    removeTestDb();
    if(log)
    {
        Logger::getInstance().close();
    }
}