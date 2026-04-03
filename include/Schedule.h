#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "Record.h"

class Schedule : public Record
{
  public:
    std::string getTabName() const override;

    int getId() const override;
    std::string getName() const override;
    std::vector<Field> getFields() override;

    void addRecord() override;
    void deleteRecord() override;
    void editRecord() override;

    void setId(int id) override;
    std::string toString() override;

    std::string getForeignName(const Field::DataType dataType) const override;

    void setName(std::string name);
    void setOrderNum(int orderNum);
    void setOrderNum(std::string orderNum); //string as an argument because we read every db column as a text
    void setDormancyStart(int dormancyStart);
    void setDormancyStart(std::string dormancyStart); //string as an argument because we read every db column as a text
    void setDormancyEnd(int dormancyEnd);
    void setDormancyEnd(std::string dormancyEnd); //string as an argument because we read every db column as a text
    void swapOrder(Schedule& scheduleSwap);

  private: 
    int id_;
    std::string name_;
    int dormancyStart_;
    int dormancyEnd_;
    int orderNum_;
};

#endif