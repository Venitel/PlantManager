#ifndef PLANT_H
#define PLANT_H

#include "Record.h"
#include <optional>

class Plant : public Record
{
  public:
    std::string getTabName() const override;

    std::vector<Field> getFields() override;
    std::string getDetailsHeader() const override;
    std::vector<DetailLine> getExtraDetails() const override;

    std::string getForeignName(const Field::DataType dataType) const override;

    bool hasSpecies() const;
    bool isDormant() const;
    std::optional<int> daysUntilWatering() const;
    void setSpeciesId(int speciesId);
    void setSpeciesId(std::string speciesId); //string as an argument because we read every db column as a text
    void setLastWatered(std::string isoDate);
    void setNotes(std::string notes);
  private:
    int speciesId_ = -1;
    std::string lastWatered_;
    std::string notes_;
};

#endif