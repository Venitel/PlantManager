#ifndef SETTING_H
#define SETTING_H

#include "Record.h"

class Setting : public Record
{
  public:
    std::string getTabName() const override;

    std::vector<Field> getFields() override;

    void setNotes(const std::string& notes);
    void setValue(const std::string& value); //string as an argument because we read every db column as a text
    void setValue(const int value);

    int getValue() const;

    void updateCache();

    static void initializeSettings();
    void addRecord() override;
    void deleteRecord() override;
  private: 
    std::string notes_;
    int value_ = 0;

    static void insertDbSetting(const std::string& name, const std::string& description, const int defaultValue);
};

#endif