/*  This tests general section operations:
- database loading 
- navigation - up, down, reset position, activate, deactivate etc.
- add / delete record as well as database operations
*/
#include <gtest/gtest.h>
#include <filesystem>
#include "Database.h"
#include "Sections.h"
#include "Plant.h"
#include "Species.h"
#include "Schedule.h"
#include "Setting.h"

void initTestDb()
{
    std::error_code error;
    std::filesystem::remove("PMTests.db", error);
    std::filesystem::copy_file("../Tests/TestDatabaseSource.db", "PMTests.db", std::filesystem::copy_options::overwrite_existing);
    Database::getInstance().open("PMTests.db");
}

void terminateTestDb()
{
    Database::getInstance().close();
    std::error_code error;
    std::filesystem::remove("PMTests.db", error);
}

class DbTestObject
{
protected:
    DbTestObject() 
    {
        initTestDb();
    }
    ~DbTestObject()
    {
        terminateTestDb();
    }
};

//List sections
//Navigation
using ListSectionNaviTypes = ::testing::Types<
                        ListSection<Plant>, 
                        ListSection<Species>,
                        ListSection<Schedule>,
                        ListSection<Setting>
                    >;

template <typename T>
class ListSectionTest : public ::testing::Test, public DbTestObject 
{
protected:
    T section;
    ListSectionTest()
    {
        section.loadFromDb();
    }
};

template <typename T>
class ListSectionNaviTest : public ListSectionTest<T> {};

TYPED_TEST_SUITE(ListSectionNaviTest, ListSectionNaviTypes);

TYPED_TEST(ListSectionNaviTest, Move)
{
    int index = 0;
    while(index < this->section.recordCount() - 1)
    {
        this->section.moveDown();
        ++index;
        EXPECT_EQ(this->section.getPosition(), index);
    }
    while(index > 0)
    {
        this->section.moveUp();
        --index;
        EXPECT_EQ(this->section.getPosition(), index);
    }
}

TYPED_TEST(ListSectionNaviTest, MoveAboveMax)
{
    int index = 0;
    while(index < this->section.recordCount() - 1)
    {
        this->section.moveDown();
        ++index;
        EXPECT_EQ(this->section.getPosition(), index);
    }
    this->section.moveDown();
    EXPECT_EQ(this->section.getPosition(), index);
}

TYPED_TEST(ListSectionNaviTest, MoveBelowMin)
{
    EXPECT_EQ(this->section.getPosition(), 0);
    this->section.moveUp();
    EXPECT_EQ(this->section.getPosition(), 0);
}

TYPED_TEST(ListSectionNaviTest, ResetPosition)
{
    this->section.moveDown();
    EXPECT_EQ(this->section.getPosition(), 1);
    this->section.resetPosition();
    EXPECT_EQ(this->section.getPosition(), 0);
}

//Add/Delete
using ListSectionAddDelTypes = ::testing::Types<
                        ListSection<Plant>, 
                        ListSection<Species>,
                        ListSection<Schedule>
                    >;

template <typename T>
class ListSectionAddDelTest : public ListSectionTest<T> {};

TYPED_TEST_SUITE(ListSectionAddDelTest, ListSectionAddDelTypes);

TYPED_TEST(ListSectionAddDelTest, Delete)
{
    int initialCount = this->section.recordCount();
    this->section.deleteRecord(0);

    //memory
    EXPECT_EQ(this->section.recordCount(), initialCount - 1);

    //database
    this->section.loadFromDb();
    EXPECT_EQ(this->section.recordCount(), initialCount - 1);
}

TYPED_TEST(ListSectionAddDelTest, Add)
{
    int initialCount = this->section.recordCount();
    auto newRecord = this->section.getBlankRecord();

    using SectionType = std::decay_t<decltype(this->section)>;
    if constexpr (std::is_same_v<SectionType, ListSection<Plant>>) 
    {
        newRecord.setSpeciesId(1);
    }
    else if constexpr (std::is_same_v<SectionType, ListSection<Species>>)
    {
        newRecord.setScheduleId(1);
    }

    this->section.addRecord(newRecord);

    //memory
    EXPECT_EQ(this->section.recordCount(), initialCount + 1);

    //database
    this->section.loadFromDb();
    EXPECT_EQ(this->section.recordCount(), initialCount + 1);
}

//Details sections
using DetailsSectionTypes = ::testing::Types<
                        DetailsSection<Plant>, 
                        DetailsSection<Species>,
                        DetailsSection<Schedule>,
                        DetailsSection<Setting>
                    >;

template <typename T>
class DetailsSectionTest : public ::testing::Test, public DbTestObject 
{
protected:
    T section;
};

TYPED_TEST_SUITE(DetailsSectionTest, DetailsSectionTypes);

TYPED_TEST(DetailsSectionTest, Move)
{
    int index = 0;
    while(index < this->section.editableFieldsCount() - 1)
    {
        this->section.moveDown();
        ++index;
        EXPECT_EQ(this->section.getPosition(), index);
    }
    while(index > 0)
    {
        this->section.moveUp();
        --index;
        EXPECT_EQ(this->section.getPosition(), index);
    }
}

TYPED_TEST(DetailsSectionTest, MoveAboveMax)
{
    int index = 0;
    while(index < this->section.editableFieldsCount() - 1)
    {
        this->section.moveDown();
        ++index;
        EXPECT_EQ(this->section.getPosition(), index);
    }
    this->section.moveDown();
    EXPECT_EQ(this->section.getPosition(), index);
}

TYPED_TEST(DetailsSectionTest, MoveBelowMin)
{
    EXPECT_EQ(this->section.getPosition(), 0);
    this->section.moveUp();
    EXPECT_EQ(this->section.getPosition(), 0);
}

TYPED_TEST(DetailsSectionTest, ResetPosition)
{
    if(this->section.editableFieldsCount() > 1)
    {
        this->section.moveDown();
        EXPECT_EQ(this->section.getPosition(), 1);
    }
    this->section.resetPosition();
    EXPECT_EQ(this->section.getPosition(), 0);
}

//Pairs
using SectionPairTypes = ::testing::Types<
                    std::pair<ListSection<Plant>,   DetailsSection<Plant>>,
                    std::pair<ListSection<Species>, DetailsSection<Species>>,
                    std::pair<ListSection<Schedule>,DetailsSection<Schedule>>,
                    std::pair<ListSection<Setting>, DetailsSection<Setting>>
                    >;

template <typename T>
class SectionPairTest : public ::testing::Test, public DbTestObject 
{
protected:
    T sectionPair;
    SectionPairTest()
    {
        sectionPair.first.loadFromDb();
    }
};

TYPED_TEST_SUITE(SectionPairTest, SectionPairTypes);

TYPED_TEST(SectionPairTest, SectionsActivation)
{
    auto& list = this->sectionPair.first;
    auto& details = this->sectionPair.second;

    list.moveDown();
    details.activate();

    EXPECT_EQ(list.getPosition(), 1);
    EXPECT_TRUE(details.isActive());
    EXPECT_FALSE(list.isActive());

    details.moveDown();
    list.activate();

    EXPECT_EQ(list.getPosition(), 1);
    EXPECT_EQ(details.getPosition(), 0);
    EXPECT_TRUE(list.isActive());
    EXPECT_FALSE(details.isActive());
}