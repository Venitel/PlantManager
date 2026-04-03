#include "Schedule.h"
#include "Database.h"

std::string Schedule::getTabName() const
{
    return Database::getTableName(Field::DataType::Schedule);
}

std::string Schedule::getForeignName(const Field::DataType dataType) const
{
    return "";
}

int Schedule::getId() const
{
    return id_;
}

std::string Schedule::getName() const
{
    return name_;
}

std::vector<Field> Schedule::getFields() 
{
    const std::string orderNum = std::to_string(orderNum_);
    const std::string dormancyStart = std::to_string(dormancyStart_);
    const std::string dormancyEnd = std::to_string(dormancyEnd_);

    return 
    {//   ColNam                Label                 Var           Length  InputType                       DataType                    Setter
        { "name",               "Name          : ",   name_,        34,     Field::InputType::Mandatory,    Field::DataType::Text,      [this](std::string v){ setName(v);}},
        { "dormancyStart",      "Dormancy Start: ",   dormancyStart,9,      Field::InputType::List,         Field::DataType::Month,     [this](std::string v){ setDormancyStart(v);}},
        { "dormancyEnd",        "Dormancy End  : ",   dormancyEnd,  9,      Field::InputType::List,         Field::DataType::Month,     [this](std::string v){ setDormancyEnd(v);}},
        { "orderNum",           "Order         : ",   orderNum,     9,      Field::InputType::NoInput,      Field::DataType::Number,    [this](std::string v){ setOrderNum(v);}}
    };
}

void Schedule::addRecord()
{
    const std::string orderQuery = "SELECT IFNULL(MAX(orderNum), 0)+1 FROM schedules";
    const std::string queryResult = Database::getInstance().getResult(orderQuery);
    orderNum_ = !queryResult.empty() ? stoi(queryResult) : 999999999;

    Database::getInstance().insertDb(this);
}

void Schedule::deleteRecord()
{
    Database::getInstance().deleteDb(this);
}

void Schedule::editRecord()
{
    Database::getInstance().updateDb(this);
}

void Schedule::setId(int id)
{
    id_ = id;
}

void Schedule::setName(std::string name)
{
    name_ = name;
}

void Schedule::setOrderNum(std::string orderNum)
{
    setOrderNum(stoi(orderNum));
}

void Schedule::setOrderNum(int orderNum)
{
    orderNum_ = orderNum;
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

void Schedule::swapOrder(Schedule& scheduleSwap)
{
    int orgOrder = orderNum_;
    setOrderNum(scheduleSwap.orderNum_);
    editRecord();

    scheduleSwap.setOrderNum(orgOrder);
    scheduleSwap.editRecord();
}

std::string Schedule::toString()
{
    std::string ret = "SCHEDULE Id: " + std::to_string(id_);
    for(Field& field : getFields())
    {
        ret += ", " + field.colNam + ": " + field.value;
    }

    return ret;
}