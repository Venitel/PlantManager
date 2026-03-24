#ifndef DATABASE_H
#define DATABASE_H
 
#include "Record.h"
#include <vector>
#include <string>
#include "sqlite3.h"
 
class Database
{
  public:
    ~Database();
    static Database& getInstance();
 
    bool open(const std::string& file);
    void close();
 
    void insertDb(Record* record);
    void updateDb(Record* record);
    void deleteDb(Record* record);

    template <typename T>
    std::vector<T> getAll();
 
  private:
    sqlite3* m_db = nullptr;

    void createTables();

    std::string sqlString(const std::string& text);
 
    void exec(const std::string& sql);
    sqlite3_stmt* prepare(const std::string& sql);
};
 
#endif