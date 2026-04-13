#ifndef SECTIONS_H
#define SECTIONS_H

#include "Plant.h"
#include "Species.h"
#include "Schedule.h"
#include <vector>
#include <variant>

void loadAllListsFromDb();
void cachePlantData();

class Section 
{
  public:
    enum class SectionType
    {
        List,
        Details
    };

    virtual ~Section() = default;

    int getPosition() const;
    void resetPosition();
    SectionType getType() const;

    bool isActive() const;
    bool activate();

    bool moveUp();
    virtual bool moveDown() = 0;
    virtual void moveLast() = 0;

  protected:
    Section(SectionType type);
    virtual void deactivate();
    int position_ = 0;

  private:
    const SectionType type_;
};

template <typename T>
class ListSection : public Section 
{
  public:
    ListSection();
    using RecordType = T;
    T getBlankRecord() const;
    std::string getTabName() const;

    void addRecord(T& record);
    void deleteRecord(const int index);
    void updateRecord(const int index);
    void orderUp(const int index);

    T& getRecord(const int index);
    T& getSelectedRecord();
    std::vector<T>& getAllRecords();

    int recordCount() const;
    bool empty() const;

    bool moveDown();
    void moveLast();
    bool moveToRecord(const int id);
    bool moveToRecord(const std::string& id);

    void loadFromDb();

  private:
    std::vector<T> records_;
};

template <typename T>
class DetailsSection : public Section 
{
  public:
    DetailsSection();

    int editableFieldsCount() const;

    bool moveDown();
    void moveLast();
    
  protected:
    void deactivate() override;
};

extern template class ListSection<Plant>;
extern template class DetailsSection<Plant>;
extern template class ListSection<Species>;
extern template class DetailsSection<Species>;
extern template class ListSection<Schedule>;
extern template class DetailsSection<Schedule>;

using Tabs = std::variant <
    std::pair<ListSection<Plant>*,   DetailsSection<Plant>*>,
    std::pair<ListSection<Species>*, DetailsSection<Species>*>,
    std::pair<ListSection<Schedule>*, DetailsSection<Schedule>*>
>;

extern Section* activeSection;
extern std::vector<Tabs> allTabs;
extern Tabs activeTab;
extern int activeTabIndex;

#endif