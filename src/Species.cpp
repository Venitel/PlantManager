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
        std::string tabName = Database::getTableName(dataType);
        return Database::getInstance().getNameById(tabName, scheduleId_);
    }
    return "";
}

int Species::getId() const
{
    return id_;
}

std::string Species::getName() const
{
    return name_;
}

std::vector<Field> Species::getFields() 
{
    const std::string orderNum = std::to_string(orderNum_);
    const std::string scheduleId = std::to_string(scheduleId_);

    return 
    {//   ColNam        Label         Var           Length  InputType                      DataType                  Setter
        { "name",       "Name    : ", name_,        40,     Field::InputType::Mandatory,   Field::DataType::Text,    [this](std::string v){ setName(v);      }},
        { "scheduleId", "Schedule: ", scheduleId,   9,      Field::InputType::List,        Field::DataType::Schedule,[this](std::string v){ setScheduleId(v);}},
        { "orderNum",   "Order   : ", orderNum,     9,      Field::InputType::NoInput,     Field::DataType::Number,  [this](std::string v){ setOrderNum(v);  }}
    };
}

std::string Species::getDetailsHeader() const
{
    return "Details";
}

void Species::addRecord()
{
    const std::string orderQuery = "SELECT IFNULL(MAX(orderNum), 0)+1 FROM species";
    const std::string queryResult = Database::getInstance().getResult(orderQuery);
    orderNum_ = !queryResult.empty() ? stoi(queryResult) : 999999999;

    Database::getInstance().insertDb(this);
}

void Species::deleteRecord()
{
    Database::getInstance().deleteDb(this);
}

void Species::editRecord()
{
    Database::getInstance().updateDb(this);
}

void Species::setId(int id)
{
    id_ = id;
}

void Species::setName(std::string name)
{
    name_ = name;
}

bool Species::hasSchedule() const
{
    return scheduleId_ > 0;
}

void Species::setScheduleId(int scheduleId)
{
    scheduleId_ = scheduleId;
}

void Species::setScheduleId(std::string scheduleId)
{
    if(!scheduleId.empty()) //nullable col
    {
        setScheduleId(stoi(scheduleId));
    }
}

void Species::setOrderNum(std::string orderNum)
{
    setOrderNum(stoi(orderNum));
}

void Species::setOrderNum(int orderNum)
{
    orderNum_ = orderNum;
}

void Species::swapOrder(Species& speciesSwap)
{
    int orgOrder = orderNum_;
    setOrderNum(speciesSwap.orderNum_);
    editRecord();

    speciesSwap.setOrderNum(orgOrder);
    speciesSwap.editRecord();
}

std::string Species::toString()
{
    std::string ret = "SPECIES Id: " + std::to_string(id_);
    for(Field& field : getFields())
    {
        ret += ", " + field.colNam + ": " + field.value;
    }

    return ret;
}