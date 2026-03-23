#ifndef PLANT_H
#define PLANT_H

#include "Record.h"

class Plant : public Record
{
  public: 
    std::string getName() const override;
    std::vector<Field> getFields() override;

    void addRecord() override;
    void deleteRecord() override;
    void editRecord() override;

    std::string getSpecies();
    std::string getNotes();

    void setName(std::string name);
    void setSpecies(std::string species);
    void setNotes(std::string notes);

  private: 
    std::string name_;
    std::string species_;
    std::string notes_;
};

#endif