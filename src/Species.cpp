#include "Species.h"
#include "Database.h"

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
        const std::string tabName = Database::getTableName(dataType);
        return Database::getInstance().getNameById(tabName, scheduleId_);
    }
    return "";
}

std::vector<Field> Species::getFields()
{
    const std::string orderNum = std::to_string(orderNum_);
    const std::string scheduleId = std::to_string(scheduleId_);

    return 
    {//   ColNam        Label         Var           Length  InputType                      DataType                  Setter                                     onEdit
        { "name",       "Name    : ", name_,        40,     Field::InputType::Mandatory,   Field::DataType::Text,    [this](std::string v){setName(v);},        [this](){updateRecord();} },
        { "scheduleId", "Schedule: ", scheduleId,   9,      Field::InputType::List,        Field::DataType::Schedule,[this](std::string v){setScheduleId(v);},  [this](){updateRecord();} },
        { "orderNum",   "Order   : ", orderNum,     9,      Field::InputType::NoInput,     Field::DataType::Number,  [this](std::string v){setOrderNum(v);},    [this](){} }
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