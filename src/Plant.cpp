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

        return CommonCache::getName(dataType, getSpeciesId());
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
        { "speciesId",      "Species : ", speciesId,      9,      Field::InputType::List,         Field::DataType::Species,   [this](std::string v){setSpeciesId(v);},      [this](){speciesChanged();} },
        { "lastWatered",    "Watered : ", lastWatered_,   10,     Field::InputType::Optional,     Field::DataType::Date,      [this](std::string v){setLastWatered(v);},    [this](){lastWateredChanged();} },
        { "wateringDelay",  "W. Delay: ", wateringDelay,  3,      Field::InputType::NoDisplay,    Field::DataType::Number,    [this](std::string v){setWateringDelay(v);},  {} },
        { "lastFed",        "Fed     : ", lastFed_,       10,     Field::InputType::Optional,     Field::DataType::Date,      [this](std::string v){setLastFed(v);},        [this](){lastFedChanged();} },
        { "feedingDelay",   "F. Delay: ", feedingDelay,   3,      Field::InputType::NoDisplay,    Field::DataType::Number,    [this](std::string v){setFeedingDelay(v);},   {} },
        { "notes",          "Notes   : ", notes_,         120,    Field::InputType::Optional,     Field::DataType::Text,      [this](std::string v){setNotes(v);},          [this](){updateRecord();} },
        { "orderNum",       "Order   : ", orderNum,       9,      Field::InputType::NoDisplay,    Field::DataType::Number,    [this](std::string v){setOrderNum(v);},       {} }
    };
}

std::vector<DetailLine> Plant::getExtraDetails() const 
{
    std::vector<DetailLine> extraDetails;
    //Dormancy
    if(isDormant())
    {
        extraDetails.push_back({2, "Dormant", Colors::Dormant});
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
    Colors color = Colors::Default;

    int due = CommonCache::getSettings()["Action required"];
    if(daysUntil < 0)
    {
        color = Colors::DuePast;
    }
    else if(daysUntil < due)
    {
        color = Colors::DueNow;
    }

    if(daysUntil == 0)
    {
        text += "Today";
    }
    else if(daysUntil == 1)
    {
        text += "Tomorrow";
    }
    else if(daysUntil > 1)
    {
        text += "In " + std::to_string(daysUntil) + " days";
    }
    else if(daysUntil == -1)
    {
        text += "Yesterday";
    }
    else //days < 0
    {
        text += std::to_string(abs(daysUntil)) + " days ago";
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

int Plant::getSpeciesId() const
{
    return speciesId_;
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
        int postponeDays = CommonCache::getSettings()["Postpone"];
        if(daysUntil.value() < 0) //if watering is due, delay it to extra days ahead
        {
            setWateringDelay(abs(daysUntil.value()) + postponeDays);
        }
        else //add extra days
        {
            setWateringDelay(wateringDelay_ + postponeDays);
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
        int postponeDays = CommonCache::getSettings()["Postpone"];
        if(daysUntil.value() < 0) //if feeding is due, set it to extra days ahead
        {
            setFeedingDelay(abs(daysUntil.value()) + postponeDays);
        }
        else //add extra days
        {
            setFeedingDelay(feedingDelay_ + postponeDays);
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

void Plant::speciesChanged()
{
    setWateringDelay(0);
    setFeedingDelay(0);
    updateRecord();

    checkDormancy();
    checkDaysUntilWatering();
    checkDaysUntilFeeding();
}

Colors Plant::getNameColor() const
{
    std::optional<int> watering = daysUntilWatering();
    std::optional<int> feeding = daysUntilFeeding();

    int due = CommonCache::getSettings()["Action required"];

    if((watering.has_value() && watering.value() < 0) || (feeding.has_value() && feeding.value() < 0))
    {
        return Colors::DuePast;
    }
    else if((watering.has_value() && watering.value() < due) || (feeding.has_value() && feeding.value() < due))
    {
        return Colors::DueNow;
    }
    return Colors::Default;
}