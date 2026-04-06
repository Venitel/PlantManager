#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "Record.h"

class Schedule : public Record
{
  public:
    std::string getTabName() const override;

    std::vector<Field> getFields() override;

    void setDormancyStart(int dormancyStart);
    void setDormancyStart(std::string dormancyStart); //string as an argument because we read every db column as a text
    void setDormancyEnd(int dormancyEnd);
    void setDormancyEnd(std::string dormancyEnd); //string as an argument because we read every db column as a text
    void setWaterInterval(int waterInterval);
    void setWaterInterval(std::string waterInterval); //string as an argument because we read every db column as a text
    void setWaterIntervalDormant(int waterIntervalDormant);
    void setWaterIntervalDormant(std::string waterIntervalDormant); //string as an argument because we read every db column as a text
  private: 
    int dormancyStart_;
    int dormancyEnd_;
    int waterInterval_;
    int waterIntervalDormant_;
};

#endif