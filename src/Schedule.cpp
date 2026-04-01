#include "Schedule.h"
#include "Database.h"

std::string Schedule::getTabName() const
{
    return "schedules";
}

std::string Schedule::getForeignName(const std::string& tableNam) const
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
    return 
    {//   ColNam        Label         Var           Length  InputType                       DataType                    Setter                                  Foreign record
        { "name",       "Name    : ", name_,        40,     Field::InputType::Mandatory,    Field::DataType::Text,      [this](std::string v){ setName(v);    }, "" },
        { "orderNum",   "Order   : ", orderNum,     9,      Field::InputType::NoInput,      Field::DataType::Number,    [this](std::string v){ setOrderNum(v);}, "" }
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