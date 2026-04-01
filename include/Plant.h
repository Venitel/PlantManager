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

    std::string getForeignName(const std::string& tableName) const override;

    void setName(std::string name);
    bool hasSpecies() const;
    void setSpeciesId(int speciesId);
    void setSpeciesId(std::string speciesId); //string as an argument because we read every db column as a text
    void setLastWatered(std::string isoDate);
    void setNotes(std::string notes);
    void setOrderNum(int orderNum);
    void setOrderNum(std::string orderNum); //string as an argument because we read every db column as a text
    void swapOrder(Plant& plantSwap);

  private: 
    int id_;
    std::string name_;
    int speciesId_ = -1;
    std::string lastWatered_;
    std::string notes_;
    int orderNum_;
};

#endif