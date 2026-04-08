#include "Plant.h"
#include "Database.h"
#include "Utils.h"

std::string Plant::getTabName() const
{
    return Database::getTableName(Field::DataType::Plant);
}

std::string Plant::getForeignName(const Field::DataType dataType) const
{
    if(dataType == Field::DataType::Species)
    {
        if(!hasSpecies())
        {
            return "";
        }
        std::string tabName = Database::getTableName(dataType);
        return Database::getInstance().getNameById(tabName, speciesId_);
    }
    return "";
}

std::vector<Field> Plant::getFields() 
{
    const std::string orderNum = std::to_string(orderNum_);
    const std::string speciesId = std::to_string(speciesId_);

    return 
    {//   ColNam        Label         Var           Length  InputType                       DataType                    Setter
        { "name",       "Name    : ", name_,        40,     Field::InputType::Mandatory,    Field::DataType::Text,      [this](std::string v){ setName(v);       }},
        { "speciesId",  "Species : ", speciesId,    9,      Field::InputType::List,         Field::DataType::Species,   [this](std::string v){ setSpeciesId(v);  }},
        { "lastWatered","Watered : ", lastWatered_, 10,     Field::InputType::Optional,     Field::DataType::Date,      [this](std::string v){ setLastWatered(v);}},
        { "lastFed",    "Fed     : ", lastFed_,     10,     Field::InputType::Optional,     Field::DataType::Date,      [this](std::string v){ setLastFed(v);    }},
        { "notes",      "Notes   : ", notes_,       120,    Field::InputType::Optional,     Field::DataType::Text,      [this](std::string v){ setNotes(v);      }},
        { "orderNum",   "Order   : ", orderNum,     9,      Field::InputType::NoInput,      Field::DataType::Number,    [this](std::string v){ setOrderNum(v);   }}
    };
}

std::vector<DetailLine> Plant::getExtraDetails() const 
{
    std::vector<DetailLine> extraDetails;
    //Dormancy
    bool dormant = isDormant();
    if(dormant)
    {
        extraDetails.push_back({1, "Dormant", Colors::Dormant}); //after lastWatered field
    }

    //Days until watering
    std::optional<int> daysWatering = daysUntilWatering(dormant);
    if(daysWatering.has_value())
    {
        extraDetails.push_back(daysUntilDetail(4+(int)extraDetails.size(), "Watering: ", daysWatering.value())); //after lastWatered field
    }

    //Days until feeding
    std::optional<int> daysFeeding = daysUntilFeeding(dormant);
    if(daysFeeding.has_value())
    {
        extraDetails.push_back(daysUntilDetail(5+(int)extraDetails.size(), "Feeding : ", daysFeeding.value())); //after lastWatered field
    }

    return extraDetails;
}

DetailLine Plant::daysUntilDetail(const int pos, std::string text, const int daysUntil) const
{
    std::string dayOrDays = abs(daysUntil) == 1 ? " day" : " days";
    Colors color;
    if(daysUntil == 0)
    {
        text += "Today";
        color = Colors::DueToday;
    }
    else if(daysUntil > 0)
    {
        text += "In " + std::to_string(daysUntil) + dayOrDays;
        color = Colors::DueFuture;
    }
    else //days < 0
    {
        text += std::to_string(abs(daysUntil)) + dayOrDays + " ago";
        color = Colors::DuePast;
    }
    return {pos, text, color};
}

bool Plant::hasSpecies() const
{
    return speciesId_ > 0;
}

bool Plant::isDormant() const
{
    const std::string orderQuery = 
    "SELECT CASE "
        "WHEN schedules.dormancyStart <= schedules.dormancyEnd "
          "THEN CAST(strftime('%m', 'now', 'localtime') AS INTEGER) BETWEEN schedules.dormancyStart AND schedules.dormancyEnd "
        "ELSE CAST(strftime('%m', 'now', 'localtime') AS INTEGER) >= schedules.dormancyStart "
          "OR CAST(strftime('%m', 'now', 'localtime') AS INTEGER) <= schedules.dormancyEnd "
        "END as isDormant "
    "FROM plants "
    "JOIN species ON species.id = plants.speciesId "
    "JOIN schedules ON schedules.id = species.scheduleId "
    "WHERE plants.id = " + Database::sqlString(std::to_string(getId()));

    return Database::getInstance().getResult(orderQuery) == "1";
}

std::optional<int> Plant::countDaysUntil(const std::string& intervalCol, const std::string& checkCol) const
{
    //Positive = days until, negative = days late
    const std::string orderQuery = 
    "SELECT CASE "
        "WHEN schedules." + intervalCol + " = -1 THEN NULL " //-1 = null = disabled
        "ELSE CAST((julianday(plants." + checkCol + ", '+' || schedules." + intervalCol + " || ' day') - julianday('now', 'localtime', 'start of day')) AS INTEGER) "
        "END "
    "FROM plants "
    "JOIN species ON species.id = plants.speciesId "
    "JOIN schedules ON schedules.id = species.scheduleId "
    "WHERE plants.id = " + Database::sqlString(std::to_string(getId()));

    std::string result = Database::getInstance().getResult(orderQuery);
    if(result.empty())
    {
        return std::nullopt;
    }

    return stoi(Database::getInstance().getResult(orderQuery));
}

std::optional<int> Plant::daysUntilWatering(bool dormant) const
{
    const std::string intervalColNam = dormant ? "waterIntervalDormant" : "waterInterval";

    return countDaysUntil(intervalColNam, "lastWatered");
}

std::optional<int> Plant::daysUntilFeeding(bool dormant) const
{
    const std::string intervalColNam = dormant ? "feedIntervalDormant" : "feedInterval";

    return countDaysUntil(intervalColNam, "lastFed");
}

void Plant::setSpeciesId(int speciesId)
{
    speciesId_ = speciesId;
}

void Plant::setSpeciesId(std::string speciesId)
{
    if(!speciesId.empty()) //nullable col
    {
        setSpeciesId(stoi(speciesId));
    }
}

void Plant::setLastWatered(std::string isoDate)
{
    if(isoDate.empty() || DateUtils::isValidDateLog(isoDate))
    {
        lastWatered_ = isoDate;
    }
}

void Plant::waterNow()
{
    setLastWatered(DateUtils::today());
    updateRecord();
}

void Plant::setLastFed(std::string isoDate)
{
    if(isoDate.empty() || DateUtils::isValidDateLog(isoDate))
    {
        lastFed_ = isoDate;
    }
}

void Plant::feedNow()
{
    setLastFed(DateUtils::today());
    updateRecord();
}

void Plant::setNotes(std::string notes)
{
    notes_ = notes;
}