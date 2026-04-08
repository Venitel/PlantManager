#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "Record.h"

class Schedule : public Record
{
  public:
    std::string getTabName() const override;

    std::vector<Field> getFields() override;
    std::vector<DetailLine> getExtraDetails() const override;

    void setDormancyStart(const int dormancyStart);
    void setDormancyStart(const std::string& dormancyStart); //string as an argument because we read every db column as a text
    void setDormancyEnd(const int dormancyEnd);
    void setDormancyEnd(const std::string& dormancyEnd); //string as an argument because we read every db column as a text
    void setWaterInterval(const int waterInterval);
    void setWaterInterval(std::string& waterInterval); //string as an argument because we read every db column as a text
    void setWaterIntervalDormant(const int waterIntervalDormant);
    void setWaterIntervalDormant(std::string& waterIntervalDormant); //string as an argument because we read every db column as a text
    void setFeedInterval(const int feedInterval);
    void setFeedInterval(std::string& feedInterval); //string as an argument because we read every db column as a text
    void setFeedIntervalDormant(const int feedIntervalDormant);
    void setFeedIntervalDormant(std::string& feedIntervalDormant); //string as an argument because we read every db column as a text

  private: 
    int dormancyStart_;
    int dormancyEnd_;
    int waterInterval_ = -1;
    int waterIntervalDormant_ = -1;
    int feedInterval_ = -1;
    int feedIntervalDormant_ = -1;

};

#endif