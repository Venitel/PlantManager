/*  This tests plant object behavior, which also tests schedules/species/settings by relation to them
    Because of that, we dont test species/schedules/settings specifically
    Theres nothing special about them that does not impact plant itself 
    
    To avoid overcomplicating things in the tests itself, certain values used in this test are set in TestDatabaseSource.db
    Changing TestDatabaseSource.db requires changing expected values here */
#include <gtest/gtest.h>

#include "DbTestObject.h"

#include "Database.h"
#include "Sections.h"
#include "Plant.h"
#include "Schedule.h"
#include "Setting.h"
#include "Utils.h"

void makeNotDormant(int index)
{
    //Test database has the same indexes for each plant/specie/schedule that reference each other
    //Set dormancy of indexed schedule (plant) to start and end in next month
    ListSection<Schedule> schedules;
    schedules.loadFromDb();
    Schedule testSchedule = schedules.getRecord(index);

    const std::string today = DateUtils::today();
    const int month = std::stoi(today.substr(5, 2));
    const int nextMonth = month < 12 ? month + 1 : 1;

    testSchedule.setDormancyStart(nextMonth);
    testSchedule.setDormancyEnd(nextMonth);
    testSchedule.updateRecord();
    testSchedule.scheduleChanged();
}

void editSetting(const int index, const int days)
{
    Setting postponeSetting = getAllSettings()[index];
    postponeSetting.setValue(days);
    postponeSetting.updateRecord();
    postponeSetting.updateCache();
}

void editDueSetting(const int days)
{
    editSetting(0, days);
}

void editPostponeSetting(const int days)
{
    editSetting(1, days);
}

class PlantListSectionTest : public ::testing::Test, public DbTestObject 
{
protected:
    PlantListSectionTest()
    {
        loadAllListsFromDb();
        cacheSettings();
        cachePlantData();
    }
};

TEST_F(PlantListSectionTest, Dormant)
{
    // Plant 1 (index 0) is dormant all year
    EXPECT_TRUE(getAllPlants()[0].isDormant());
}

TEST_F(PlantListSectionTest, NotDormant)
{
    makeNotDormant(0);
    EXPECT_FALSE(getAllPlants()[0].isDormant());
}

void testWatering(Plant& plant, const int expectedDays)
{
    //Test schedules interactions
    EXPECT_LT(plant.daysUntilWatering(), 0);
    EXPECT_EQ(plant.getNameColor(), Colors::DuePast);
    plant.waterNow();
    EXPECT_EQ(plant.daysUntilWatering(), expectedDays);
    plant.delayWatering();
    EXPECT_EQ(plant.daysUntilWatering(), expectedDays+1);
    EXPECT_EQ(plant.getNameColor(), Colors::Default);

    //Test settings interactions
    editPostponeSetting(3);
    plant.delayWatering();
    EXPECT_EQ(plant.daysUntilWatering(), expectedDays+4);
    EXPECT_EQ(plant.getNameColor(), Colors::Default);

    editDueSetting(50);
    EXPECT_EQ(plant.getNameColor(), Colors::DueNow);
}

TEST_F(PlantListSectionTest, Watering)
{
    makeNotDormant(0);

    Plant testPlant = getAllPlants()[0];
    testWatering(testPlant, 20);
}

TEST_F(PlantListSectionTest, WateringDormant)
{
    Plant testPlant = getAllPlants()[0];
    testWatering(testPlant, 40);
}

void testFeeding(Plant& plant, const int expectedDays)
{
    //Test schedules interactions
    EXPECT_LT(plant.daysUntilFeeding(), 0);
    EXPECT_EQ(plant.getNameColor(), Colors::DuePast);
    plant.feedNow();
    EXPECT_EQ(plant.daysUntilFeeding(), expectedDays);
    plant.delayFeeding();
    EXPECT_EQ(plant.daysUntilFeeding(), expectedDays+1);
    EXPECT_EQ(plant.getNameColor(), Colors::Default);

    //Test settings interactions
    editPostponeSetting(3);
    plant.delayFeeding();
    EXPECT_EQ(plant.daysUntilFeeding(), expectedDays+4);
    EXPECT_EQ(plant.getNameColor(), Colors::Default);

    editDueSetting(50);
    EXPECT_EQ(plant.getNameColor(), Colors::DueNow);
}

TEST_F(PlantListSectionTest, Feeding)
{
    makeNotDormant(1);

    Plant testPlant = getAllPlants()[1];
    testFeeding(testPlant, 20);
}

TEST_F(PlantListSectionTest, FeedingDormant)
{
    Plant testPlant = getAllPlants()[1];
    testFeeding(testPlant, 40);
}