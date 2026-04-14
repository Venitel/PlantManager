#ifndef PLANT_H
#define PLANT_H

#include "Record.h"
#include <optional>

class Plant : public Record
{
  public:
    std::string getTabName() const override;

    std::vector<Field> getFields() override;
    std::vector<DetailLine> getExtraDetails() const override;

    void onCreate();

    std::string getForeignName(const Field::DataType dataType) const override;

    bool hasSpecies() const;

    bool isDormant() const;
    std::optional<int> daysUntilWatering() const;
    std::optional<int> daysUntilFeeding() const;

    void checkDormancy();
    void checkDaysUntilWatering();
    void checkDaysUntilFeeding();

    void setSpeciesId(const int speciesId);
    void setSpeciesId(const std::string& speciesId); //string as an argument because we read every db column as a text
    int getSpeciesId() const;
    void speciesChanged();
    
    void setLastWatered(const std::string& isoDate);
    void lastWateredChanged();
    bool waterNow();
    void setWateringDelay(const int delay);
    void setWateringDelay(const std::string& delay); //string as an argument because we read every db column as a text
    bool delayWatering();
    
    void setLastFed(const std::string& isoDate);
    void lastFedChanged();
    bool feedNow();
    void setFeedingDelay(const int delay);
    void setFeedingDelay(const std::string& delay); //string as an argument because we read every db column as a text
    bool delayFeeding();

    void setNotes(const std::string& notes);

  private:
    //Db Fields
    int speciesId_ = -1;
    std::string lastWatered_;
    int wateringDelay_ = 0;
    std::string lastFed_;
    int feedingDelay_ = 0;
    std::string notes_;
    
    //Non-Db variables
    bool dormant_;
    std::optional<int> daysUntilWatering_ = std::nullopt;
    std::optional<int> daysUntilFeeding_ = std::nullopt;

    DetailLine daysUntilDetail(const int pos, std::string text, const int daysUntil, const int delay) const;
    std::optional<int> checkDaysUntil(const std::string& intervalCol, const std::string& checkCol, const std::string& delayCol) const;
};

#endif