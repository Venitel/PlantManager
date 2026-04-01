#include "Plant.h"
#include "Database.h"

std::string Plant::getTabName() const
{
    return "plants";
}

std::string Plant::getForeignName(const std::string& tableNam) const
{
    if(tableNam == "species")
    {
        if(!hasSpecies())
        {
            return "";
        }
        std::string tabName = tableNam;
        return Database::getInstance().getNameById(tabName, speciesId_);
    }
    return "";
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
    const std::string speciesId = std::to_string(speciesId_);

    return 
    {//   ColNam        Label         Var           Length  InputType                       DataType                    Setter                                     Foreign record
        { "name",       "Name    : ", name_,        40,     Field::InputType::Mandatory,    Field::DataType::Text,      [this](std::string v){ setName(v);       }, "" },
        { "speciesId",  "Species : ", speciesId,    9,      Field::InputType::Mandatory,    Field::DataType::Number,    [this](std::string v){ setSpeciesId(v);  }, "species"},
        { "lastWatered","Watered : ", lastWatered_, 10,     Field::InputType::Optional,     Field::DataType::Date,      [this](std::string v){ setLastWatered(v);}, "" },
        { "notes",      "Notes   : ", notes_,       120,    Field::InputType::Optional,     Field::DataType::Text,      [this](std::string v){ setNotes(v);      }, "" },
        { "orderNum",   "Order   : ", orderNum,     9,      Field::InputType::NoInput,      Field::DataType::Number,    [this](std::string v){ setOrderNum(v);   }, "" }
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

bool Plant::hasSpecies() const
{
    return speciesId_ > 0;
}

void Plant::setSpeciesId(int speciesId)
{
    speciesId_ = speciesId;
}

void Plant::setSpeciesId(std::string speciesId)
{
    if(!speciesId.empty()) //nullable col
    {
        setSpeciesId(stoi(speciesId));
    }
}

void Plant::setLastWatered(std::string isoDate)
{
    lastWatered_ = isoDate;
}

void Plant::setNotes(std::string notes)
{
    notes_ = notes;
}

void Plant::setOrderNum(std::string orderNum)
{
    setOrderNum(stoi(orderNum));
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