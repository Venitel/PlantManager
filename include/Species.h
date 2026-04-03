#ifndef SPECIES_H
#define SPECIES_H

#include "Record.h"

class Species : public Record
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
    bool hasSchedule() const;
    void setScheduleId(int scheduleId);
    void setScheduleId(std::string scheduleId); //string as an argument because we read every db column as a text
    void setOrderNum(int orderNum);
    void setOrderNum(std::string orderNum); //string as an argument because we read every db column as a text
    void swapOrder(Species& speciesSwap);

  private: 
    int id_;
    std::string name_;
    int scheduleId_ = -1;
    int orderNum_;
};

#endif