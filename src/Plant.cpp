#include "Plant.h"
#include "Database.h"

std::string Plant::getTabName() const
{
    return "plants";
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
        return 
        {
            { "name",       "Name    : ", name_,    40,   true,   [this](std::string v){ setName(v);    } },
            { "species",    "Species : ", species_, 40,   true,   [this](std::string v){ setSpecies(v); } },
            { "notes",      "Notes   : ", notes_,   120,  false,  [this](std::string v){ setNotes(v);   } }
        };
}

void Plant::addRecord()
{
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

std::string Plant::toString()
{
    std::string ret = "PLANT Id: " + std::to_string(id_);
    for(Field& field : getFields())
    {
        ret += ", " + field.colNam + ": " + field.value;
    }

    return ret;
}