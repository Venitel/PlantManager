#include "Sections.h"
#include "Database.h"

ListSection<Plant> plantList;
DetailsSection<Plant> plantDetails;
Section* activeSection = &plantList;

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

void Section::activate()
{
    if(activeSection != this)
    {
        if(activeSection != nullptr) 
        {
            activeSection->deactivate();
        }
        activeSection = this;
    }
}

void Section::deactivate()
{
}

void Section::moveUp() 
{
    if(position_ > 0) position_--;
}

// LIST SECTION
template <typename T>
ListSection<T>::ListSection() : Section(Section::List) 
{}
    
template <typename T>
void ListSection<T>::addRecord(T record)
{
    record.addRecord();
    records_.push_back(record);
}

template <typename T>
void ListSection<T>::deleteRecord(int index)
{
    records_[index].deleteRecord();
    records_.erase(records_.begin() + index);
}

template <typename T>
T& ListSection<T>::getRecord(int index)
{
    return records_[index];
}

template <typename T>
T& ListSection<T>::getSelectedRecord()
{
    return records_[getPosition()];
}

template <typename T>
void ListSection<T>::editRecord(int index)
{
    records_[index].editRecord();
}

template <typename T>
void ListSection<T>::orderUp(int index)
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
void ListSection<T>::moveDown() 
{
    if(position_ < recordCount() - 1) position_++;
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
void ListSection<T>::initFromDb()
{
    records_ = Database::getInstance().getAll<T>();
}

//explicit instantiation
template class ListSection<Plant>;

// DETAILS SECTION
template <typename T>
DetailsSection<T>::DetailsSection() : Section(Section::Details) 
{}

template <typename T>
void DetailsSection<T>::deactivate()
{
    resetPosition();
}

template <typename T>
int DetailsSection<T>::fieldsCount() const
{
    T dummy;
    return (int)dummy.getFields().size();
}

template <typename T>
void DetailsSection<T>::moveDown() 
{
    if(position_ < fieldsCount() - 1) position_++; 
}

template <typename T>
void DetailsSection<T>::moveLast() 
{
    position_ = fieldsCount() - 1; 
}

//explicit instantiation
template class DetailsSection<Plant>;