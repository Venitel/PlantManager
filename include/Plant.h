#ifndef PLANT_H
#define PLANT_H

#include "Record.h"

class Plant : public Record
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

    void setName(std::string name);
    void setSpecies(std::string species);
    void setNotes(std::string notes);

  private: 
    int id_;
    std::string name_;
    std::string species_;
    std::string notes_;
};

#endif