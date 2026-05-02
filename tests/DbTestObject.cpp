#include "DbTestObject.h"
#include <filesystem>
#include "Database.h"

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
}

DbTestObject::~DbTestObject()
{
    Database::getInstance().close();
    removeTestDb();
}