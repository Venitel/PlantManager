#include "Species.h"
#include "Database.h"

std::string Species::getTabName() const
{
    return "species";
}

std::string Species::getOrderBy() const
{
    return "orderNum ASC";
}

std::string Species::getForeignName(const std::string& tableNam) const
{
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
    return 
    {//   ColNam        Label         Var       Length  Mandatory  UserEditable Setter
        { "name",       "Name    : ", name_,     40,     true,     true,         [this](std::string v){ setName(v);    } },
        { "orderNum",   "Order   : ", orderNum,   9,    false,     false,        [this](std::string v){ setOrderNum(v);} }
    };
}

void Species::addRecord()
{
    const std::string orderQuery = "SELECT IFNULL(MAX(orderNum), 0)+1 FROM plants";
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

void Species::setOrderNum(std::string orderNum)
{
    orderNum_ = stoi(orderNum);
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