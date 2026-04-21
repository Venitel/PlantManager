#ifndef SPECIES_H
#define SPECIES_H

#include "Record.h"

class Species : public Record
{
  public:
    std::string getTabName() const override;

    std::vector<Field> getFields() override;

    void onCreate() override;
    void onDelete() override;

    std::string getForeignName(const Field::DataType dataType) const override;

    bool hasSchedule() const;
    void setScheduleId(const int scheduleId);
    void setScheduleId(const std::string& scheduleId); //string as an argument because we read every db column as a text
    int getScheduleId() const;
    void scheduleChanged();

  private: 
    int scheduleId_ = -1;
};

#endif