#ifndef RECORD_H
#define RECORD_H

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

class Record 
{
  public:
    virtual ~Record() = default;

    virtual int getId() const = 0;
    virtual std::string getTabName() const = 0;
    virtual std::string getDetailsHeader() const = 0;
    virtual std::string getName() const = 0;
    virtual std::string getForeignName(const Field::DataType dataType) const = 0;

    virtual std::vector<Field> getFields() = 0;

    virtual void setId(int id) = 0;

    virtual std::string toString() = 0;

    virtual void addRecord() = 0;
    virtual void deleteRecord() = 0;
    virtual void editRecord() = 0;
};

#endif