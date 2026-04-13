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
        const std::string tabName = Database::getTableName(dataType);
        return Database::getInstance().getNameById(tabName, speciesId_);
    }
    return "";
}

std::vector<Field> Plant::getFields()
{
    const std::string orderNum = std::to_string(orderNum_);
    const std::string speciesId = std::to_string(speciesId_);
    const std::string wateringDelay = std::to_string(wateringDelay_);
    const std::string feedingDelay = std::to_string(feedingDelay_);

    return 
    {//   ColNam            Label         Var             Length  InputType                       DataType                    Setter (DB)                                   onEdit       
        { "name",           "Name    : ", name_,          40,     Field::InputType::Mandatory,    Field::DataType::Text,      [this](std::string v){setName(v);},           [this](){updateRecord();} },
        { "speciesId",      "Species : ", speciesId,      9,      Field::InputType::List,         Field::DataType::Species,   [this](std::string v){setSpeciesId(v);},      [this](){updateRecord();} },
        { "lastWatered",    "Watered : ", lastWatered_,   10,     Field::InputType::Optional,     Field::DataType::Date,      [this](std::string v){setLastWatered(v);},    [this](){lastWateredChanged();} },
        { "wateringDelay",  "W. Delay: ", wateringDelay,  3,      Field::InputType::NoInput,      Field::DataType::Number,    [this](std::string v){setWateringDelay(v);},  [this](){} },
        { "lastFed",        "Fed     : ", lastFed_,       10,     Field::InputType::Optional,     Field::DataType::Date,      [this](std::string v){setLastFed(v);},        [this](){lastFedChanged();} },
        { "feedingDelay",   "F. Delay: ", feedingDelay,   3,      Field::InputType::NoInput,      Field::DataType::Number,    [this](std::string v){setFeedingDelay(v);},   [this](){} },
        { "notes",          "Notes   : ", notes_,         120,    Field::InputType::Optional,     Field::DataType::Text,      [this](std::string v){setNotes(v);},          [this](){updateRecord();} },
        { "orderNum",       "Order   : ", orderNum,       9,      Field::InputType::NoInput,      Field::DataType::Number,    [this](std::string v){setOrderNum(v);},       [this](){} }
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
    std::optional<int> daysWatering = daysUntilWatering();
    if(daysWatering.has_value())
    {
        extraDetails.push_back(daysUntilDetail(4+(int)extraDetails.size(), "Watering: ", daysWatering.value(), wateringDelay_)); //after lastWatered field
    }

    //Days until feeding
    std::optional<int> daysFeeding = daysUntilFeeding();
    if(daysFeeding.has_value())
    {
        extraDetails.push_back(daysUntilDetail(5+(int)extraDetails.size(), "Feeding : ", daysFeeding.value(), feedingDelay_)); //after lastWatered field
    }

    return extraDetails;
}

DetailLine Plant::daysUntilDetail(const int pos, std::string text, const int daysUntil, const int delay) const
{
    Colors color;
    if(daysUntil == 0)
    {
        text += "Today";
        color = Colors::DueToday;
    }
    else if(daysUntil == 1)
    {
        text += "Tomorrow";
        color = Colors::DueFuture;
    }
    else if(daysUntil > 1)
    {
        text += "In " + std::to_string(daysUntil) + " days";
        color = Colors::DueFuture;
    }
    else if(daysUntil == -1)
    {
        text += "Yesterday";
        color = Colors::DuePast;
    }
    else //days < 0
    {
        text += std::to_string(abs(daysUntil)) + " days ago";
        color = Colors::DuePast;
    }

    if(delay > 0)
    {
        text += " (" + std::to_string(delay) + (delay == 1 ? " day" : " days") + " delayed)";
    }
    return {pos, text, color};
}

void Plant::onCreate()
{
    addRecord();
    if(hasSpecies())
    {
        checkDaysUntilWatering();
        checkDaysUntilFeeding();
    }
}

bool Plant::hasSpecies() const
{
    return speciesId_ > 0;
}

void Plant::checkDormancy()
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

    dormant_ =  Database::getInstance().getResult(orderQuery) == "1";
}

bool Plant::isDormant() const
{
    return dormant_;
}

std::optional<int> Plant::checkDaysUntil(const std::string& intervalCol, const std::string& checkCol, const std::string& delayCol) const
{
    //Positive = days until, negative = days late
    const std::string orderQuery = 
    "SELECT CASE "
        "WHEN schedules." + intervalCol + " = -1 THEN NULL " //-1 = null = disabled
        "ELSE CAST((julianday(plants." + checkCol + ", '+' || schedules." + intervalCol + " || ' day') - julianday('now', 'localtime', 'start of day')) AS INTEGER) + plants." + delayCol + " "
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

    return stoi(result);
}

void Plant::checkDaysUntilWatering()
{
    const std::string intervalColNam = isDormant() ? "waterIntervalDormant" : "waterInterval";

    daysUntilWatering_ = checkDaysUntil(intervalColNam, "lastWatered", "wateringDelay");
}

void Plant::checkDaysUntilFeeding() 
{
    const std::string intervalColNam = isDormant() ? "feedIntervalDormant" : "feedInterval";

    daysUntilFeeding_ =  checkDaysUntil(intervalColNam, "lastFed", "FeedingDelay");
}

std::optional<int> Plant::daysUntilWatering() const
{
    return daysUntilWatering_;
}

std::optional<int> Plant::daysUntilFeeding() const
{
    return daysUntilFeeding_;
}

void Plant::setSpeciesId(const int speciesId)
{
    speciesId_ = speciesId;
}

void Plant::setSpeciesId(const std::string& speciesId)
{
    if(!speciesId.empty()) //nullable col
    {
        setSpeciesId(stoi(speciesId));
    }
}

void Plant::setLastWatered(const std::string& isoDate)
{
    if(isoDate.empty() || DateUtils::isValidDateLog(isoDate))
    {
        lastWatered_ = isoDate;
    }
}

void Plant::lastWateredChanged()
{
    setWateringDelay(0);
    updateRecord();
    checkDaysUntilWatering();
}

bool Plant::waterNow()
{
    std::string today = DateUtils::today();
    if(lastWatered_ != today)
    {
        setLastWatered(today);
        lastWateredChanged();
        return true;
    }
    return false;
}

void Plant::setWateringDelay(const int delay)
{
    wateringDelay_ = delay;
}

void Plant::setWateringDelay(const std::string& delay)
{
    setWateringDelay(stoi(delay));
}

bool Plant::delayWatering()
{
    std::optional<int> daysUntil = daysUntilWatering();
    if(daysUntil.has_value())
    {
        if(daysUntil.value() < 0) //if watering is due, delay it to tomorrow
        {
            setWateringDelay(abs(daysUntil.value()) + 1);
        }
        else //add one day
        {
            setWateringDelay(wateringDelay_ + 1);
        }
        updateRecord();
        checkDaysUntilWatering(); //technically we could just set daysUntilWatering to 1 / +1 but its cleaner this way
        return true;
    }
    return false;
}

void Plant::setLastFed(const std::string& isoDate)
{
    if(isoDate.empty() || DateUtils::isValidDateLog(isoDate))
    {
        lastFed_ = isoDate;
    }
}

void Plant::lastFedChanged()
{
    setFeedingDelay(0);
    updateRecord();
    checkDaysUntilFeeding();
}

bool Plant::feedNow()
{
    std::string today = DateUtils::today();
    if(lastFed_ != today)
    {
        setLastFed(today);
        lastFedChanged();
        return true;
    }
    return false;
}

void Plant::setFeedingDelay(const int delay)
{
    feedingDelay_ = delay;
}

void Plant::setFeedingDelay(const std::string& delay)
{
    setFeedingDelay(stoi(delay));
}

bool Plant::delayFeeding()
{
    std::optional<int> daysUntil = daysUntilFeeding();
    if(daysUntil.has_value())
    {
        if(daysUntil.value() <= 0) //if feeding is due, set it to tomorrow
        {
            setFeedingDelay(abs(daysUntil.value()) + 1);
        }
        else //add one day
        {
            setFeedingDelay(feedingDelay_ + 1);
        }
        updateRecord();
        checkDaysUntilFeeding(); //technically we could just set daysUntilFeeding to 1 / +1 but its cleaner this way
        return true;
    }
    return false;
}

void Plant::setNotes(const std::string& notes)
{
    notes_ = notes;
}