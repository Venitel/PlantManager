#include "Schedule.h"
#include "Database.h"
#include "Utils.cpp"
#include "Sections.h"
#include "Species.h"

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
    {//    ColNam                Label                    Var                   Length  InputType                       DataType                    Setter                                              onEdit
        { "name",                "Name             : ",   name_,                31,     Field::InputType::Mandatory,    Field::DataType::Text,      [this](std::string v){setName(v);},                 [this](){updateRecord(); CommonCache::updateElement(Field::DataType::Schedule, {getId(), getName()});} }, 
        { "dormancyStart",       "Dormancy Start   : ",   dormancyStart,        9,      Field::InputType::List,         Field::DataType::Month,     [this](std::string v){setDormancyStart(v);},        [this](){updateRecord(); scheduleChanged();} },
        { "dormancyEnd",         "Dormancy End     : ",   dormancyEnd,          9,      Field::InputType::List,         Field::DataType::Month,     [this](std::string v){setDormancyEnd(v);},          [this](){updateRecord(); scheduleChanged();} },
        { "waterInterval",       "Watering         : ",   waterInterval,        3,      Field::InputType::Optional,     Field::DataType::Number,    [this](std::string v){setWaterInterval(v);},        [this](){updateRecord(); scheduleChanged();} },
        { "waterIntervalDormant","Dormant Watering : ",   waterIntervalDormant, 3,      Field::InputType::Optional,     Field::DataType::Number,    [this](std::string v){setWaterIntervalDormant(v);}, [this](){updateRecord(); scheduleChanged();} },
        { "feedInterval",        "Feeding          : ",   feedInterval,         3,      Field::InputType::Optional,     Field::DataType::Number,    [this](std::string v){setFeedInterval(v);},         [this](){updateRecord(); scheduleChanged();} },
        { "feedIntervalDormant", "Dormant Feeding  : ",   feedIntervalDormant,  3,      Field::InputType::Optional,     Field::DataType::Number,    [this](std::string v){setFeedIntervalDormant(v);},  [this](){updateRecord(); scheduleChanged();} },
        { "orderNum",            "Order            : ",   orderNum,             9,      Field::InputType::NoInput,      Field::DataType::Number,    [this](std::string v){setOrderNum(v);},             [this](){} }
    };
}

std::vector<DetailLine> Schedule::getExtraDetails() const 
{
    std::vector<DetailLine> extraDetails;
   
    extraDetails.push_back({4, "--- Intervals (Days) ---", Colors::Inactive}); //Before intervals

    return extraDetails;
}

void Schedule::setDormancyStart(const int dormancyStart)
{
    dormancyStart_ = dormancyStart;
}

void Schedule::setDormancyStart(const std::string& dormancyStart)
{
    setDormancyStart(stoi(dormancyStart));
}

void Schedule::setDormancyEnd(const int dormancyEnd)
{
    dormancyEnd_ = dormancyEnd;
}

void Schedule::setDormancyEnd(const std::string& dormancyEnd)
{
    setDormancyEnd(stoi(dormancyEnd));
}

void Schedule::setWaterInterval(const int waterInterval)
{
    waterInterval_ = waterInterval;
}

void Schedule::setWaterInterval(std::string& waterInterval)
{
    Utils::prepareOptionalIntFromString(waterInterval);
    if(Utils::isNumberLog(waterInterval))
    {
        setWaterInterval(stoi(waterInterval));
    }
}

void Schedule::setWaterIntervalDormant(const int waterIntervalDormant)
{
    waterIntervalDormant_ = waterIntervalDormant;
}

void Schedule::setWaterIntervalDormant(std::string& waterIntervalDormant)
{
    Utils::prepareOptionalIntFromString(waterIntervalDormant);
    if(Utils::isNumberLog(waterIntervalDormant))
    {
        setWaterIntervalDormant(stoi(waterIntervalDormant));
    }
}

void Schedule::setFeedInterval(const int feedInterval)
{
    feedInterval_ = feedInterval;
}

void Schedule::setFeedInterval(std::string& feedInterval)
{
    Utils::prepareOptionalIntFromString(feedInterval);
    if(Utils::isNumberLog(feedInterval))
    {
        setFeedInterval(stoi(feedInterval));
    }
}

void Schedule::setFeedIntervalDormant(const int feedIntervalDormant)
{
    feedIntervalDormant_ = feedIntervalDormant;
}

void Schedule::setFeedIntervalDormant(std::string& feedIntervalDormant)
{
    Utils::prepareOptionalIntFromString(feedIntervalDormant);
    if(Utils::isNumberLog(feedIntervalDormant))
    {
        setFeedIntervalDormant(stoi(feedIntervalDormant));
    }
}

void Schedule::scheduleChanged()
{
    for(Species& species : getAllSpecies())
    {
        if(species.hasSchedule() && species.getScheduleId() == getId())
        {
            species.scheduleChanged();
        }
    }
}