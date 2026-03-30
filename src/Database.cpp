#include "Database.h"
#include "Plant.h"
#include "Species.h"
#include "Logger.h"

#include <filesystem>
#include <stdexcept>
#include <iostream>

Database::~Database() 
{
    close();
}
 
Database& Database::getInstance()
{
    static Database database;
    return database;
}
  
bool Database::open(const std::string& file)
{
    std::filesystem::path path = std::filesystem::current_path() / file;
    if (sqlite3_open(path.string().c_str(), &m_db) != SQLITE_OK) 
    {
        Logger::getInstance().error("Failed to open: " + std::string(sqlite3_errmsg(m_db)));
        return false;
    }
 
    exec("PRAGMA foreign_keys = ON;");
    createTables();
    return true;
}
 
void Database::close()
{
    if (m_db) 
    { 
        sqlite3_close(m_db); 
        m_db = nullptr; 
    }
}
 
void Database::createTables()
{
    exec(R"(
        CREATE TABLE IF NOT EXISTS plants (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            name        TEXT NOT NULL,
            speciesId   INTEGER REFERENCES species(id) ON DELETE SET NULL,
            notes       TEXT DEFAULT '',
            orderNum    INTEGER NOT NULL
        );
    )");

    exec(R"(
        CREATE TABLE IF NOT EXISTS species (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            name        TEXT NOT NULL,
            orderNum    INTEGER NOT NULL
        );
    )");
}

template <typename T>
std::vector<T> Database::getAll() const
{
    std::string cols = "";
    bool first = true;

    std::vector<Field> fields = T().getFields();
    for(Field& field : fields)
    {
        if(!first)
        {
            cols += + ", ";
        }
        cols += field.colNam;
        first = false;
    }

    std::string tabName = T().getTabName();
    std::string sql = "SELECT id, " + cols + " FROM " + tabName + " ORDER BY orderNum";
    auto* stmt = prepare(sql);

    std::vector<T> retVec;
    while (sqlite3_step(stmt) == SQLITE_ROW) 
    {
        T record;
        record.setId(sqlite3_column_int(stmt, 0));
        int colCounter = 1;
        for(Field& recordField : record.getFields())
        {
            if(sqlite3_column_type(stmt, colCounter) != SQLITE_NULL)
            {
                recordField.setter(reinterpret_cast<const char*>(sqlite3_column_text(stmt, colCounter)));
            }
            else
            {
                recordField.setter("");
            }
            ++colCounter;
        }
        retVec.push_back(record);
        Logger::getInstance().info(record.toString());
    }
 
    sqlite3_finalize(stmt);
    return retVec;
}
//explicit instantiation
template std::vector<Plant> Database::getAll<Plant>() const;
template std::vector<Species> Database::getAll<Species>() const;

std::string Database::sqlString(const std::string& text) const
{
    return "'" + text + "'";
}

void Database::insertDb(Record* record)
{
    std::string cols = "";
    std::string values = "";
    bool first = true;

    std::vector<Field> fields = record->getFields();
    for(Field& field : fields)
    {
        if(!first)
        {
            cols += + ", ";
            values += + ", ";
        }
        cols += field.colNam;
        values += sqlString(field.value);
        first = false;
    }

    std::string sql = "INSERT INTO " + record->getTabName() + " (" + cols + ") VALUES (" + values + ")";
    auto* stmt = prepare(sql);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
 
    record->setId((int)sqlite3_last_insert_rowid(m_db));
}
 
void Database::updateDb(Record* record)
{
    std::string eqs = "";
    bool first = true;

    std::vector<Field> fields = record->getFields();
    for(Field& field : fields)
    {
        if(!first)
        {
            eqs += + ", ";
        }
        eqs += field.colNam + "=" + sqlString(field.value);
        first = false;
    }

    std::string sql = "UPDATE " + record->getTabName() + " SET " + eqs + " WHERE id=" + std::to_string(record->getId());
    auto* stmt = prepare(sql);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void Database::deleteDb(Record* record)
{
    std::string sql = "DELETE FROM " + record->getTabName() + " WHERE id=" + std::to_string(record->getId());
    auto* stmt = prepare(sql);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void Database::exec(const std::string& sql)
{
    char* err = nullptr;
    if (sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) 
    {
        std::string msg = err ? err : "Unknown error";
        sqlite3_free(err);
        Logger::getInstance().error("Execute error: " + msg);
    }
}
 
sqlite3_stmt* Database::prepare(const std::string& sql) const
{
    Logger::getInstance().info(sql);

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) 
    {
        Logger::getInstance().error("Prepare error: " + std::string(sqlite3_errmsg(m_db)));
    }
    return stmt;
}

std::string Database::getResult(const std::string& sql) const
{
    auto* stmt = prepare(sql);

    if(sqlite3_step(stmt) == SQLITE_ROW) 
    {
        if(sqlite3_column_type(stmt, 0) != SQLITE_NULL)
        {
            return reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        }
    }

    return "";
}

std::string Database::getNameById(std::string& table, int id) const
{
    std::string name;
    std::string sql = "SELECT name FROM " + table + " WHERE id = " + std::to_string(id);
    auto* stmt = prepare(sql);
    while (sqlite3_step(stmt) == SQLITE_ROW) 
    {
        name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        Logger::getInstance().info("Name: " + name);
    }
    sqlite3_finalize(stmt);
    return name;
}

std::vector<std::pair<int, std::string>> Database::getAllKeys(std::string& table) const
{
    std::vector<std::pair<int, std::string>> allKeys;
    std::string sql = "SELECT id, name FROM " + table + " ORDER BY orderNum ASC";
    auto* stmt = prepare(sql);
    while (sqlite3_step(stmt) == SQLITE_ROW) 
    {
        int id = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        allKeys.push_back({id, name});
        Logger::getInstance().info("Id: " + std::to_string(id) + ", Name: " + name);
    }
    sqlite3_finalize(stmt);
    return allKeys;
}