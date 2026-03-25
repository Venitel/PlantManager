#include "Plant.h"
#include "Database.h"

std::string Plant::getTabName() const
{
    return "plants";
}

std::string Plant::getOrderBy() const
{
    return "orderNum ASC";
}

int Plant::getId() const
{
    return id_;
}

std::string Plant::getName() const
{
    return name_;
}

std::vector<Field> Plant::getFields() 
{
    const std::string orderNum = std::to_string(orderNum_);
    return 
    {//   ColNam        Label         Var       Length  Mandatory  UserEditable Setter
        { "name",       "Name    : ", name_,     40,     true,     true,         [this](std::string v){ setName(v);    } },
        { "species",    "Species : ", species_,  40,     true,     true,         [this](std::string v){ setSpecies(v); } },
        { "notes",      "Notes   : ", notes_,   120,    false,     true,         [this](std::string v){ setNotes(v);   } },
        { "orderNum",   "Order   : ", orderNum,   9,    false,     false,        [this](std::string v){ setOrderNum(v);} }
    };
}

void Plant::addRecord()
{
    const std::string orderQuery = "SELECT IFNULL(MAX(orderNum), 0)+1 FROM plants";
    const std::string queryResult = Database::getInstance().getResult(orderQuery);
    orderNum_ = !queryResult.empty() ? stoi(queryResult) : 999999999;

    Database::getInstance().insertDb(this);
}

void Plant::deleteRecord()
{
    Database::getInstance().deleteDb(this);
}

void Plant::editRecord()
{
    Database::getInstance().updateDb(this);
}

void Plant::setId(int id)
{
    id_ = id;
}

void Plant::setName(std::string name)
{
    name_ = name;
}

void Plant::setSpecies(std::string species)
{
    species_ = species;
}

void Plant::setNotes(std::string notes)
{
    notes_ = notes;
}

void Plant::setOrderNum(std::string orderNum)
{
    orderNum_ = stoi(orderNum);
}

void Plant::setOrderNum(int orderNum)
{
    orderNum_ = orderNum;
}

void Plant::swapOrder(Plant& plantSwap)
{
    int orgOrder = orderNum_;
    setOrderNum(plantSwap.orderNum_);
    editRecord();

    plantSwap.setOrderNum(orgOrder);
    plantSwap.editRecord();
}

std::string Plant::toString()
{
    std::string ret = "PLANT Id: " + std::to_string(id_);
    for(Field& field : getFields())
    {
        ret += ", " + field.colNam + ": " + field.value;
    }

    return ret;
}