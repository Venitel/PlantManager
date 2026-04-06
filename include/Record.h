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

  static bool isForeign(const DataType type) 
  {
    return type == DataType::Plant ||
           type == DataType::Species ||
           type == DataType::Schedule;
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

    virtual void setId(int id);
    virtual void setName(std::string name);

    virtual std::string getForeignName(const Field::DataType dataType) const;
    virtual std::string getDetailsHeader() const;
    virtual std::vector<DetailLine> getExtraDetails() const;

    void setOrderNum(int orderNum);
    void setOrderNum(std::string orderNum); //string as an argument because we read every db column as a text
    void swapOrder(Record& recordSwap);

    virtual std::string toString();
    //Pure virtual
    virtual std::string getTabName() const = 0;
    virtual std::vector<Field> getFields() = 0;

protected:
    int id_;
    std::string name_;
    int orderNum_;
};

#endif