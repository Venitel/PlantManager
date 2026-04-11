#ifndef RECORD_H
#define RECORD_H

#include "Colors.h"
#include <string>
#include <vector>
#include <functional>

struct Field
{
  enum class InputType
  {
    NoInput,
    Optional,
    Mandatory,
    List
  };

  enum class DataType
  {
    Text,
    Number,
    Date,
    Month,
    //Foreign keys
    Plant,
    Species,
    Schedule
  };

  bool isForeign() const
  {
    return dataType == DataType::Plant ||
           dataType == DataType::Species ||
           dataType == DataType::Schedule;
  }

  std::string colNam;
  std::string label;
  std::string value;
  int size;
  InputType inputType;
  DataType dataType;
  std::function<void(std::string)> setter;
};

struct DetailLine
{
  int row;
  std::string text;
  Colors color = Colors::Default;
};

class Record 
{
  public:
    virtual ~Record() = default;
    
    virtual void addRecord();
    virtual void deleteRecord();
    virtual void updateRecord();

    virtual int getId() const;
    virtual std::string getName() const;

    virtual void setId(const int id);
    virtual void setName(const std::string& name);

    virtual std::string getForeignName(const Field::DataType dataType) const;
    virtual std::vector<DetailLine> getExtraDetails() const;

    void setOrderNum(const int orderNum);
    void setOrderNum(const std::string& orderNum); //string as an argument because we read every db column as a text
    void swapOrder(Record& recordSwap);

    virtual std::string toString();

    virtual std::vector<Field> getEditableFields();
    //Pure virtual
    virtual std::string getTabName() const = 0;
    virtual std::vector<Field> getFields() = 0;

protected:
    int id_ = -1;
    std::string name_;
    int orderNum_ = -1;
};

#endif