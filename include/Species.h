#ifndef SPECIES_H
#define SPECIES_H

#include "Record.h"

class Species : public Record
{
  public:
    std::string getTabName() const override;

    std::vector<Field> getFields() override;

    std::string getForeignName(const Field::DataType dataType) const override;

    bool hasSchedule() const;
    void setScheduleId(int scheduleId);
    void setScheduleId(std::string scheduleId); //string as an argument because we read every db column as a text

  private: 
    int scheduleId_ = -1;
};

#endif