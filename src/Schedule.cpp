#include "Schedule.h"
#include "Database.h"
#include "Utils.cpp"

std::string Schedule::getTabName() const
{
    return Database::getTableName(Field::DataType::Schedule);
}

std::vector<Field> Schedule::getFields() 
{
    const std::string orderNum = std::to_string(orderNum_);
    const std::string dormancyStart = std::to_string(dormancyStart_);
    const std::string dormancyEnd = std::to_string(dormancyEnd_);
    const std::string waterInterval = std::to_string(waterInterval_);
    const std::string waterIntervalDormant = std::to_string(waterIntervalDormant_);
    const std::string feedInterval = std::to_string(feedInterval_);
    const std::string feedIntervalDormant = std::to_string(feedIntervalDormant_);

    return 
    {//   ColNam                 Label                        Var                   Length  InputType                       DataType                    Setter
        { "name",                "Name             : ",   name_,                31,     Field::InputType::Mandatory,    Field::DataType::Text,      [this](std::string v){ setName(v); }},
        { "dormancyStart",       "Dormancy Start   : ",   dormancyStart,        9,      Field::InputType::List,         Field::DataType::Month,     [this](std::string v){ setDormancyStart(v); }},
        { "dormancyEnd",         "Dormancy End     : ",   dormancyEnd,          9,      Field::InputType::List,         Field::DataType::Month,     [this](std::string v){ setDormancyEnd(v); }},
        { "waterInterval",       "Watering         : ",   waterInterval,        3,      Field::InputType::Optional,     Field::DataType::Number,    [this](std::string v){ setWaterInterval(v); }},
        { "waterIntervalDormant","Dormant Watering : ",   waterIntervalDormant, 3,      Field::InputType::Optional,     Field::DataType::Number,    [this](std::string v){ setWaterIntervalDormant(v); }},
        { "feedInterval",        "Feeding          : ",   feedInterval,         3,      Field::InputType::Optional,     Field::DataType::Number,    [this](std::string v){ setFeedInterval(v); }},
        { "feedIntervalDormant", "Dormant Feeding  : ",   feedIntervalDormant,  3,      Field::InputType::Optional,     Field::DataType::Number,    [this](std::string v){ setFeedIntervalDormant(v); }},
        { "orderNum",            "Order            : ",   orderNum,             9,      Field::InputType::NoInput,      Field::DataType::Number,    [this](std::string v){ setOrderNum(v); }}
    };
}

std::vector<DetailLine> Schedule::getExtraDetails() const 
{
    std::vector<DetailLine> extraDetails;
   
    extraDetails.push_back({4, "--- Intervals (Days) ---", Colors::Inactive}); //Before intervals

    return extraDetails;
}

void Schedule::setDormancyStart(int dormancyStart)
{
    dormancyStart_ = dormancyStart;
}

void Schedule::setDormancyStart(std::string dormancyStart)
{
    setDormancyStart(stoi(dormancyStart));
}

void Schedule::setDormancyEnd(int dormancyEnd)
{
    dormancyEnd_ = dormancyEnd;
}

void Schedule::setDormancyEnd(std::string dormancyEnd)
{
    setDormancyEnd(stoi(dormancyEnd));
}

void Schedule::setWaterInterval(int waterInterval)
{
    waterInterval_ = waterInterval;
}

void Schedule::setWaterInterval(std::string waterInterval)
{
    Utils::prepareOptionalIntFromString(waterInterval);
    if(Utils::isNumberLog(waterInterval))
    {
        setWaterInterval(stoi(waterInterval));
    }
}

void Schedule::setWaterIntervalDormant(int waterIntervalDormant)
{
    waterIntervalDormant_ = waterIntervalDormant;
}

void Schedule::setWaterIntervalDormant(std::string waterIntervalDormant)
{
    Utils::prepareOptionalIntFromString(waterIntervalDormant);
    if(Utils::isNumberLog(waterIntervalDormant))
    {
        setWaterIntervalDormant(stoi(waterIntervalDormant));
    }
}

void Schedule::setFeedInterval(int feedInterval)
{
    feedInterval_ = feedInterval;
}

void Schedule::setFeedInterval(std::string feedInterval)
{
    Utils::prepareOptionalIntFromString(feedInterval);
    if(Utils::isNumberLog(feedInterval))
    {
        setFeedInterval(stoi(feedInterval));
    }
}

void Schedule::setFeedIntervalDormant(int feedIntervalDormant)
{
    feedIntervalDormant_ = feedIntervalDormant;
}

void Schedule::setFeedIntervalDormant(std::string feedIntervalDormant)
{
    Utils::prepareOptionalIntFromString(feedIntervalDormant);
    if(Utils::isNumberLog(feedIntervalDormant))
    {
        setFeedIntervalDormant(stoi(feedIntervalDormant));
    }
}