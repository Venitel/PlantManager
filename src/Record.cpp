#include "Database.h"

std::string Record::toString()
{
    std::string ret = getTabName() +  " Id: " + std::to_string(id_);
    for(Field& field : getFields())
    {
        ret += ", " + field.colNam + ": " + (field.value.empty() ? " null" : field.value);;
    }

    return ret;
}

int Record::getId() const
{
    return id_;
}

std::string Record::getName() const
{
    return name_;
}

void Record::setId(int id)
{
    id_ = id;
}

void Record::setName(std::string name)
{
    name_ = name;
}

std::string Record::getForeignName(const Field::DataType dataType) const
{
    return "";
}

std::string Record::getDetailsHeader() const
{
    return "Details";
}

std::vector<DetailLine> Record::getExtraDetails() const 
{
    return {};
}

void Record::addRecord()
{
    const std::string orderQuery = "SELECT MIN(IFNULL(MAX(orderNum), 0)+1, 999999999) FROM " + getTabName();
    const std::string queryResult = Database::getInstance().getResult(orderQuery);
    orderNum_ = stoi(queryResult);

    Database::getInstance().insertDb(this);
}

void Record::deleteRecord()
{
    Database::getInstance().deleteDb(this);
}

void Record::updateRecord()
{
    Database::getInstance().updateDb(this);
}

void Record::setOrderNum(std::string orderNum)
{
    setOrderNum(stoi(orderNum));
}

void Record::setOrderNum(int orderNum)
{
    orderNum_ = orderNum;
}

void Record::swapOrder(Record& recordSwap)
{
    int orgOrder = orderNum_;
    setOrderNum(recordSwap.orderNum_);
    updateRecord();

    recordSwap.setOrderNum(orgOrder);
    recordSwap.updateRecord();
}