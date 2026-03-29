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

    std::string getResult(const std::string& sql);

    template <typename T>
    std::vector<T> getAll();
    std::string getNameById(std::string& table, int id);
    std::vector<std::pair<int, std::string>> getAllKeys(std::string& table);

    void exec(const std::string& sql);

  private:
    sqlite3* m_db = nullptr;

    void createTables();

    std::string sqlString(const std::string& text);
    sqlite3_stmt* prepare(const std::string& sql);
};
 
#endif