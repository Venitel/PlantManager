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

    void bindFieldsToStmt(sqlite3_stmt* stmt, std::vector<Field>& fields); 

    static std::string sqlString(const std::string& text);
    std::string getResult(const std::string& sql) const;

    template <typename T>
    std::vector<T> getAll() const;
    std::string getNameById(const std::string& table, const int id) const;
    std::vector<std::pair<int, std::string>> getAllKeys(const Field::DataType dataType) const;

    void exec(const std::string& sql);

    static std::string getTableName(const Field::DataType dataType);
  private:
    sqlite3* m_db = nullptr;

    void createTables();
    sqlite3_stmt* prepare(const std::string& sql, bool log=true) const;
};
 
#endif