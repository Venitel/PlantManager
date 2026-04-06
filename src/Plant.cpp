#include "Plant.h"
#include "Database.h"
#include "DateUtils.h"

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
        { "notes",      "Notes   : ", notes_,       120,    Field::InputType::Optional,     Field::DataType::Text,      [this](std::string v){ setNotes(v);      }},
        { "orderNum",   "Order   : ", orderNum,     9,      Field::InputType::NoInput,      Field::DataType::Number,    [this](std::string v){ setOrderNum(v);   }}
    };
}

std::vector<DetailLine> Plant::getExtraDetails() const 
{
    std::vector<DetailLine> extraDetails;
    //Dormancy
    if(isDormant())
    {
        extraDetails.push_back({1, "Dormant", Colors::Dormant}); //after lastWatered field
    }

    //Days until watering
    std::optional<int> days = daysUntilWatering();
    if(days.has_value())
    {
        std::string dayOrDays = abs(days.value()) == 1 ? " day" : " days";
        std::string wateringDetailText = "Watering: ";
        Colors color;
        if(days.value() == 0)
        {
            wateringDetailText += "Today";
            color = Colors::DueToday;
        }
        else if(days.value() > 0)
        {
            wateringDetailText += "In " + std::to_string(days.value()) + dayOrDays;
            color = Colors::DueFuture;
        }
        else //days < 0
        {
            wateringDetailText += std::to_string(abs(days.value())) + dayOrDays + " ago";
            color = Colors::DuePast;
        }
        extraDetails.push_back({4, wateringDetailText, color}); //after lastWatered field
    }

    return extraDetails;
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

std::optional<int> Plant::daysUntilWatering() const
{
    //Positive = days until, negative = days late
    std::string intervalColNam = isDormant() ? "waterIntervalDormant" : "waterInterval";
    const std::string orderQuery = 
    "SELECT CAST((julianday(plants.lastWatered, '+' || schedules." + intervalColNam + " || ' day') - julianday('now', 'localtime', 'start of day')) AS INTEGER) "
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
    lastWatered_ = isoDate;
}

void Plant::waterNow()
{
    setLastWatered(DateUtils::today());
    updateRecord();
}

void Plant::setNotes(std::string notes)
{
    notes_ = notes;
}