#ifndef RECORD_H
#define RECORD_H

#include <string>
#include <vector>
#include <functional>

struct Field
{
    std::string colNam;
    std::string label;
    std::string value;
    int size;
    bool mandatory;
    bool userEditable;
    std::function<void(std::string)> setter;
    std::string foreignTableName = "";
};

class Record 
{
  public:
    virtual ~Record() = default;

    virtual int getId() const = 0;
    virtual std::string getTabName() const = 0;

    virtual std::string getName() const = 0;
    virtual std::string getForeignName(const std::string& tableName) const = 0;

    virtual std::vector<Field> getFields() = 0;

    virtual void setId(int id) = 0;

    virtual std::string toString() = 0;

    virtual void addRecord() = 0;
    virtual void deleteRecord() = 0;
    virtual void editRecord() = 0;
};

#endif