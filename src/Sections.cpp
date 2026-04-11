#include "Sections.h"
#include "Database.h"

#include <algorithm>

ListSection<Plant> plantList;
DetailsSection<Plant> plantDetails;
ListSection<Species> speciesList;
DetailsSection<Species> speciesDetails;
ListSection<Schedule> scheduleList;
DetailsSection<Schedule> scheduleDetails;

std::vector<Tabs> allTabs = {
    std::make_pair(&plantList,   &plantDetails),
    std::make_pair(&speciesList, &speciesDetails),
    std::make_pair(&scheduleList, &scheduleDetails),
};
int activeTabIndex = 0;
Tabs activeTab = allTabs[activeTabIndex];
Section* activeSection = &plantList;

void loadAllListsFromDb()
{
    for(auto& tab : allTabs)
    {
        std::visit([](auto& pair) {
            pair.first->loadFromDb(); // list
        }, tab);
    }
}

// BASE SECTION
Section::Section(SectionType type) 
    :   type_{type}
{}

int Section::getPosition() const 
{
    return position_;
}

void Section::resetPosition() 
{
    position_ = 0;
}

Section::SectionType Section::getType() const
{
    return type_;
}

bool Section::isActive() const
{
    return activeSection == this;
}

bool Section::activate()
{
    if(activeSection != this)
    {
        if(activeSection != nullptr) 
        {
            activeSection->deactivate(); //deactivate old
        }
        activeSection = this; //activate new
        return true;
    }
    return false;
}

void Section::deactivate()
{
}

bool Section::moveUp() 
{
    if(position_ > 0) 
    {
        position_--;
        return true;
    }
    return false;
}

// LIST SECTION
template <typename T>
ListSection<T>::ListSection() : Section(Section::SectionType::List) 
{}

template <typename T>
T ListSection<T>::getBlankRecord() const 
{
    return T{}; 
}

template <typename T>
std::string ListSection<T>::getTabName() const 
{ 
    return getBlankRecord().getTabName(); 
}

template <typename T>
void ListSection<T>::addRecord(T& record)
{
    record.addRecord();
    records_.push_back(record);
}

template <typename T>
void ListSection<T>::deleteRecord(const int index)
{
    records_[index].deleteRecord();
    records_.erase(records_.begin() + index);
    /* Deletion can break foreign table references
       Reload all lists to get their current state
       Perhaps there's a better way to handle this
       But this will do for now */ 
    loadAllListsFromDb();
}

template <typename T>
T& ListSection<T>::getRecord(const int index)
{
    return records_[index];
}

template <typename T>
T& ListSection<T>::getSelectedRecord()
{
    return records_[getPosition()];
}

template <typename T>
void ListSection<T>::updateRecord(const int index)
{
    records_[index].updateRecord();
}

template <typename T>
void ListSection<T>::orderUp(const int index)
{
    T& currentRecord = records_[index];
    T& previousRecord = records_[index - 1];

    currentRecord.swapOrder(previousRecord);
    std::swap(currentRecord, previousRecord);
}


template <typename T>
int ListSection<T>::recordCount() const
{
    return (int)records_.size();
}

template <typename T>
bool ListSection<T>::empty() const
{
    return records_.empty();
}

template <typename T>
bool ListSection<T>::moveDown() 
{
    if(position_ < recordCount() - 1) 
    {
        position_++;
        return true;
    }
    return false;
}

template <typename T>
void ListSection<T>::moveLast() 
{
    if(recordCount() > 0)
    {
        position_ = recordCount() - 1;
    }
}

template <typename T>
bool ListSection<T>::moveToRecord(const int id)
{
    auto it = std::find_if(records_.begin(), records_.end(), [&](const T& record) {return record.getId() == id;});

    if(it != records_.end())
    {
        position_ = std::distance(records_.begin(), it);
        return true;
    }

    return false;
}

template <typename T>
bool ListSection<T>::moveToRecord(const std::string& id)
{
    if(!id.empty())
    {
        return moveToRecord(stoi(id));
    }
    return false;
}

template <typename T>
void ListSection<T>::loadFromDb()
{
    records_ = Database::getInstance().getAll<T>();
}

//explicit instantiation
template class ListSection<Plant>;
template class ListSection<Species>;
template class ListSection<Schedule>;

// DETAILS SECTION
template <typename T>
DetailsSection<T>::DetailsSection() : Section(Section::SectionType::Details) 
{}

template <typename T>
void DetailsSection<T>::deactivate()
{
    resetPosition();
}

template <typename T>
int DetailsSection<T>::editableFieldsCount() const
{
    T dummy;
    std::vector<Field> fields = dummy.getEditableFields();
    return fields.size();
}

template <typename T>
bool DetailsSection<T>::moveDown() 
{
    if(position_ < editableFieldsCount() - 1)
    {
        position_++;
        return true;
    }
    return false;
}

template <typename T>
void DetailsSection<T>::moveLast() 
{
    position_ = editableFieldsCount() - 1; 
}

//explicit instantiation
template class DetailsSection<Plant>;
template class DetailsSection<Species>;
template class DetailsSection<Schedule>;