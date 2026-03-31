#include "UserActionHandler.h"
#include "Console.h"
#include "Draw.h"
#include "Database.h"

#include <iostream>
#include <conio.h>
#include <chrono>
#include <unordered_map>
#include <algorithm>

namespace
{
    std::unordered_map<int, std::chrono::steady_clock::time_point> buttonCooldowns;
}

int getKey() 
{
    int key = _getch();
    if(key == 0 || key == 0xE0) //Arrow keys send two bytes: 0xE0 then a code
    {
        //Offset these by 256 to differ from normal characters (e.g. M = 77; Right Arrow = 0xE0 + 77)
        return _getch() + 256; 
    }
    return key;
}

int getCooldown(int key)
{
    if(key >= 256)
    {
        //Arrow keys are offset by 256, we want arrows to have shorter cooldown
        return 50;
    }
    return 100;
}

bool onCooldown(int key)
{
    auto now     = std::chrono::steady_clock::now();
    auto& last   = buttonCooldowns[key];
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last);
        
    if(elapsed.count() < getCooldown(key)) 
    {
        return true;
    }
    last = now;
    return false;
}

bool moveActiveSectionUp()
{
    return activeSection->moveUp();
}

bool moveActiveSectionDown()
{
    return activeSection->moveDown();
}

bool activateList()
{
    bool result = false;

    std::visit([&](auto& tab) {
        result = tab.first->activate();
    }, activeTab);

    return result;
}

bool activateDetails()
{
    bool result = false;

    std::visit([&](auto& tab) {
        if(!tab.first->empty())
        {
            result = tab.second->activate();     
        }
    }, activeTab);

    return result;
}

bool nextTab()
{
    bool result = false;

    activeTabIndex = (activeTabIndex + 1) % allTabs.size();
    activeTab = allTabs[activeTabIndex];

    std::visit([&](auto& tab) {
        result = tab.first->activate(); //activate list section on tab change
    }, activeTab);

    return result;
}

bool goToForeignRecord()
{
    bool result = false;

    std::visit([&](auto& tab) {
        auto& currentList = tab.first;
        auto& currentDetails = tab.second;

        if(!currentDetails->isActive() || currentList->empty())
        {
            return;
        }

        auto& record = currentList->getSelectedRecord();
        Field selectedField = record.getFields()[currentDetails->getPosition()];
        std::string targetTab = selectedField.foreignTableName;
        if(targetTab.empty())
        {
            return;
        }

        auto it = std::find_if(allTabs.begin(), allTabs.end(), [&](const Tabs& tab) {
            bool match = false;
            std::visit([&](auto& pair) {
                match = pair.first->getTabName() == targetTab;
            }, tab);
            return match; //find lambda return, not the main visit
        });

        if(it != allTabs.end())
        {
            activeTabIndex = std::distance(allTabs.begin(), it);
            activeTab = *it;
            std::visit([&](auto& newTab) {
                result = newTab.first->activate() && newTab.first->moveToRecord(selectedField.value);
            }, activeTab);
        }
    }, activeTab);

    return result;
}

void getFieldFromUser(int x, int y, Field& field)
{
    if(!field.mandatory)
    {
        setColor(Colors::Optional);
    }

    if(field.foreignTableName == "") //no reference, any input
    {
        field.setter(inputAt(x, y, field.label, field.size, field.mandatory));
    }
    else //foreign reference - select from list of records
    {
        setColor(Colors::List);
        putText(x, y, field.label);
        resetColor();

        std::vector<std::pair<int, std::string>> allKeys = Database::getInstance().getAllKeys(field.foreignTableName);
        int allKeysIndex = 0;

        if(!allKeys.empty())
        {
            bool redraw = true;
            while(true)
            {
                if(redraw)
                {
                    clearRow(y, field.label.length());
                    std::string& keyName = allKeys[allKeysIndex].second;
                    putText(field.label.length(), y, keyName);
                }

                redraw = true;
                int key = getKey();

                if(key == Key::Right) 
                {
                    allKeysIndex = (allKeysIndex + 1) % allKeys.size();
                }
                else if(key == Key::Left) 
                {
                    --allKeysIndex;
                    if(allKeysIndex < 0) 
                    {
                        allKeysIndex = allKeys.size() - 1;
                    }
                }
                else if(key == Key::Enter)
                {
                    field.setter(std::to_string(allKeys[allKeysIndex].first));
                    break;
                }
                else
                {
                    redraw = false; //unsupported key - prevent flicker
                }
            }
        }
        else
        {
            putError(field.label.length(), y, "List empty!");
        }
    }
}

bool userAdd() 
{
    bool result = false;

    std::visit([&](auto& tab) {
        auto& currentList = tab.first;

        if(!currentList->isActive())
        {
            return;
        }

        const int bottomRow = getBottomRow();
        auto newRecord = currentList->getBlankRecord();

        drawInstructionsRow(bottomRow, "Add " + newRecord.getTabName());

        std::vector<Field> fields = newRecord.getFields();
        int rowNum = 1; //title is first
        for(Field& field : fields)
        {
            if(field.userEditable)
            {
                getFieldFromUser(0, bottomRow + rowNum, field);
            }
            
            resetColor();
            ++rowNum;
        }

        currentList->addRecord(newRecord);
        currentList->moveLast();

        // Clear the input area
        for(int i = bottomRow; i <= bottomRow + rowNum; i++)
        { 
            clearRow(i);
        }
        result = true;
    }, activeTab);

    return result;
}

bool userEdit()
{
    bool result = false;

    std::visit([&](auto& tab) {
        auto& currentList = tab.first;
        auto& currentDetails = tab.second;

        if(!currentDetails->isActive() || currentList->empty())
        {
            return;
        }

        auto& record = currentList->getSelectedRecord();

        int bottomRow = getBottomRow();
        drawInstructionsRow(bottomRow, "Edit " + record.getTabName());

        Field editField = record.getFields()[currentDetails->getPosition()];
        getFieldFromUser(0, bottomRow + 1, editField);

        currentList->editRecord(currentList->getPosition());

        for(int i = bottomRow; i <= bottomRow + 2; i++) //one extra line for error, multi line notes etc.
        { 
            clearRow(i);
        }
        result = true;
    }, activeTab);

    return result;
}

bool userDelete() 
{
    bool result = false;

    std::visit([&](auto& tab) {
        auto& currentList = tab.first;

        if(!currentList->isActive() || currentList->empty())
        {
            return;
        }
        const int bottomRow = getBottomRow();
        std::string confirmation = inputAt(0, bottomRow, "Confirm delete " + currentList->getSelectedRecord().getName() + " [Y/N]: ", 1, true);
        clearRow(bottomRow);

        if(toupper(confirmation[0]) != 'Y')
        {
            return;
        }

        currentList->deleteRecord(currentList->getPosition());

        if(currentList->getPosition() >= currentList->recordCount() && currentList->getPosition() > 0) 
        {
            currentList->moveUp();
        }
        result = true;
    }, activeTab);

    return result;
}

bool userOrder() 
{
    bool result = false;
    std::visit([&](auto& tab) {
        auto& currentList = tab.first;

        if(!currentList->isActive() || currentList->empty() || currentList->getPosition() == 0)
        {
            return;
        }
        currentList->orderUp(currentList->getPosition());
        currentList->moveUp();
        result = true;
    }, activeTab);

    return result;
}