#include <gtest/gtest.h>
#include "Database.h"
#include "Sections.h"
#include "Plant.h"

using SectionTypes = ::testing::Types<ListSection<Plant>, DetailsSection<Plant>>;

template <typename T>
class SectionsTest : public ::testing::Test 
{
protected:
    T section;
    SectionsTest() 
    {
        if constexpr (std::is_same_v<T, ListSection<Plant>>) 
        {   // Add 3 dummy plants to the list so we can test movement
            Database::getInstance().open("testPM.db");
            for(int i=0; i<3; i++)
            {
                section.addRecord(Plant());
            }
            Database::getInstance().close();
        }
    }
};

TYPED_TEST_SUITE(SectionsTest, SectionTypes);

TYPED_TEST(SectionsTest, Move)
{
    this->section.moveDown();
    EXPECT_EQ(this->section.getPosition(), 1);
    this->section.moveDown();
    EXPECT_EQ(this->section.getPosition(), 2);
    this->section.moveUp();
    EXPECT_EQ(this->section.getPosition(), 1);
}

TYPED_TEST(SectionsTest, MoveAboveMax)
{
    this->section.moveDown();
    this->section.moveDown();
    EXPECT_EQ(this->section.getPosition(), 2);
    this->section.moveDown();
    EXPECT_EQ(this->section.getPosition(), 2);
}

TYPED_TEST(SectionsTest, MoveBelowMin)
{
    EXPECT_EQ(this->section.getPosition(), 0);
    this->section.moveUp();
    EXPECT_EQ(this->section.getPosition(), 0);
}

TYPED_TEST(SectionsTest, ResetPosition)
{
    this->section.moveDown();
    this->section.moveDown();

    EXPECT_EQ(this->section.getPosition(), 2);

    this->section.resetPosition();

    EXPECT_EQ(this->section.getPosition(), 0);
}

TEST(SectionsTest, SectionsActivation)
{
    Database::getInstance().open("testPM.db");
    ListSection<Plant> list;
    list.addRecord(Plant());
    list.addRecord(Plant());
    DetailsSection<Plant> details;
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
    Database::getInstance().close();
}