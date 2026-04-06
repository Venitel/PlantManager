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

    return 
    {//   ColNam                 Label                        Var                   Length  InputType                       DataType                    Setter
        { "name",                "Name             : ",   name_,                31,     Field::InputType::Mandatory,    Field::DataType::Text,      [this](std::string v){ setName(v); }},
        { "dormancyStart",       "Dormancy Start   : ",   dormancyStart,        9,      Field::InputType::List,         Field::DataType::Month,     [this](std::string v){ setDormancyStart(v); }},
        { "dormancyEnd",         "Dormancy End     : ",   dormancyEnd,          9,      Field::InputType::List,         Field::DataType::Month,     [this](std::string v){ setDormancyEnd(v); }},
        { "waterInterval",       "Watering         : ",   waterInterval,        2,      Field::InputType::Mandatory,    Field::DataType::Number,    [this](std::string v){ setWaterInterval(v); }},
        { "waterIntervalDormant","Dormant Watering : ",   waterIntervalDormant, 2,      Field::InputType::Mandatory,    Field::DataType::Number,    [this](std::string v){ setWaterIntervalDormant(v); }},
        { "orderNum",            "Order            : ",   orderNum,             9,      Field::InputType::NoInput,      Field::DataType::Number,    [this](std::string v){ setOrderNum(v); }}
    };
}

std::vector<DetailLine> Schedule::getExtraDetails() const 
{
    std::vector<DetailLine> extraDetails;
   
    extraDetails.push_back({4, "> Intervals (Days) [0 = disabled]", Colors::Inactive}); //Before intervals

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
    if(Utils::isNumberLog(waterIntervalDormant))
    {
        setWaterIntervalDormant(stoi(waterIntervalDormant));
    }
}