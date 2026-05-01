/*  This tests general section operations:
- database loading 
- navigation - up, down, reset position, activate, deactivate etc.
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
    std::filesystem::copy_file("../TestDatabaseSource.db", "PMTests.db", std::filesystem::copy_options::overwrite_existing);
    Database::getInstance().open("PMTests.db");
}

void terminateTestDb()
{
    Database::getInstance().close();
    std::filesystem::remove("PMTests.db");
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
using ListSectionTypes = ::testing::Types<
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

TYPED_TEST_SUITE(ListSectionTest, ListSectionTypes);

TYPED_TEST(ListSectionTest, Move)
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

TYPED_TEST(ListSectionTest, MoveAboveMax)
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

TYPED_TEST(ListSectionTest, MoveBelowMin)
{
    EXPECT_EQ(this->section.getPosition(), 0);
    this->section.moveUp();
    EXPECT_EQ(this->section.getPosition(), 0);
}

TYPED_TEST(ListSectionTest, ResetPosition)
{
    this->section.moveDown();
    EXPECT_EQ(this->section.getPosition(), 1);
    this->section.resetPosition();
    EXPECT_EQ(this->section.getPosition(), 0);
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