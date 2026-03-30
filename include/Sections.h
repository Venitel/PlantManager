#ifndef SECTIONS_H
#define SECTIONS_H

#include "Plant.h"
#include "Species.h"
#include <vector>
#include <variant>

void loadAllListsFromDb();

class Section 
{
  public:
    enum SectionType
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

    void addRecord(T record);
    void deleteRecord(int index);
    void editRecord(int index);
    void orderUp(int index);

    T& getRecord(int index);
    T& getSelectedRecord();

    int recordCount() const;
    bool empty() const;

    bool moveDown();
    void moveLast();

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

using Tabs = std::variant <
    std::pair<ListSection<Plant>*,   DetailsSection<Plant>*>,
    std::pair<ListSection<Species>*, DetailsSection<Species>*>
>;

extern Section* activeSection;
extern std::vector<Tabs> allTabs;
extern Tabs activeTab;
extern int activeTabIndex;

#endif