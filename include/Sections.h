#ifndef SECTIONS_H
#define SECTIONS_H

#include "Plant.h"
#include "Species.h"
#include <vector>
#include <variant>

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
    virtual void activate();

    void moveUp();
    virtual void moveDown() = 0;
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
    T getBlankRecord() const { return T{}; }
    std::string getTabName() const { return T{}.getTabName(); }

    void addRecord(T record);
    void deleteRecord(int index);
    void editRecord(int index);
    void orderUp(int index);

    T& getRecord(int index);
    T& getSelectedRecord();

    int recordCount() const;
    bool empty() const;

    void moveDown();
    void moveLast();

    void initFromDb();

  private:
    std::vector<T> records_;
};

template <typename T>
class DetailsSection : public Section 
{
  public:
    DetailsSection();

    int fieldsCount() const;

    void moveDown();
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