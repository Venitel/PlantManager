#include "Species.h"
#include "Database.h"
#include "Plant.h"
#include "Sections.h"
#include "Utils.h"

std::string Species::getTabName() const
{
    return Database::getTableName(Field::DataType::Species);
}

std::string Species::getForeignName(const Field::DataType dataType) const
{
    if(dataType == Field::DataType::Schedule)
    {
        if(!hasSchedule())
        {
            return "";
        }
        return CommonCache::getName(dataType, getScheduleId());
    }
    return "";
}

std::vector<Field> Species::getFields()
{
    const std::string orderNum = std::to_string(orderNum_);
    const std::string scheduleId = std::to_string(scheduleId_);

    return 
    {//   ColNam        Label         Var           Length  InputType                      DataType                  Setter                                     onEdit
        { "name",       "Name    : ", name_,        40,     Field::InputType::Mandatory,   Field::DataType::Text,    [this](std::string v){setName(v);},        [this](){updateRecord(); CommonCache::updateElement(Field::DataType::Species, {getId(), getName()});} },
        { "scheduleId", "Schedule: ", scheduleId,   9,      Field::InputType::List,        Field::DataType::Schedule,[this](std::string v){setScheduleId(v);},  [this](){updateRecord(); scheduleChanged();} },
        { "orderNum",   "Order   : ", orderNum,     9,      Field::InputType::NoDisplay,   Field::DataType::Number,  [this](std::string v){setOrderNum(v);},    {} }
    };
}

bool Species::hasSchedule() const
{
    return scheduleId_ > 0;
}

void Species::setScheduleId(const int scheduleId)
{
    scheduleId_ = scheduleId;
}

void Species::setScheduleId(const std::string& scheduleId)
{
    if(!scheduleId.empty()) //nullable col
    {
        setScheduleId(stoi(scheduleId));
    }
}

int Species::getScheduleId() const
{
    return scheduleId_;
}

void Species::scheduleChanged()
{
    for(Plant& plant : getAllPlants())
    {
        if(plant.hasSpecies() && plant.getSpeciesId() == getId())
        {
            plant.speciesChanged();
        }
    }
}

void Species::onCreate()
{
    addRecord();
    CommonCache::getIdNames()[Field::DataType::Species].push_back({getId(), getName()});
}

void Species::onDelete()
{
    deleteRecord();
    CommonCache::deleteElement(Field::DataType::Species, getId());
}