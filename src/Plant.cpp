#include "Plant.h"
#include "Console.h"

std::string Plant::getName() const
{
    return name_;
}

std::vector<Field> Plant::getFields() 
{
        return 
        {
            { "Name    : ", name_,    40,   true,   [this](std::string v){ setName(v);    } },
            { "Species : ", species_, 40,   true,   [this](std::string v){ setSpecies(v); } },
            { "Notes   : ", notes_,   120,  false,  [this](std::string v){ setNotes(v);   } },
        };
}

void Plant::addRecord()
{

}

void Plant::deleteRecord()
{

}

void Plant::editRecord()
{

}

std::string Plant::getSpecies()
{
    return species_;
}

std::string Plant::getNotes()
{
    return notes_.empty() ? "-" : notes_;
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