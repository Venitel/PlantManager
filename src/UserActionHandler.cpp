#include "UserActionHandler.h"
#include "Console.h"
#include "Draw.h"
#include "Database.h"

#include <iostream>
#include <conio.h>

int getKey() 
{
    int key = _getch();
    if(key == 0 || key == 0xE0) //Arrow keys send two bytes: 0xE0 then a code
    {
        //Multiply these by -1 to differ from normal characters (e.g. M = 77; Right Arrow = 0xE0 + 77)
        return _getch()*-1; 
    }
    return key;
}

void moveActiveSectionUp()
{
    activeSection->moveUp();
}

void moveActiveSectionDown()
{
    activeSection->moveDown();
}

void activateList()
{
    std::visit([](auto& tab) {
        tab.first->activate();
    }, activeTab);
}

void activateDetails()
{
    std::visit([](auto& tab) {
        if(!tab.first->empty())
        {
            tab.second->activate();     
        }
    }, activeTab);
}

void nextTab()
{
    activeTabIndex = (activeTabIndex + 1) % allTabs.size();
    activeTab = allTabs[activeTabIndex];

    std::visit([](auto& tab) {
        tab.first->activate(); //activate list section on tab change
    }, activeTab);
}

void getFieldFromUser(int x, int y, Field& field)
{
    if(!field.mandatory)
    {
        setColor(Colors::Optional);
    }

    if(field.foreignTableName.empty()) //no reference, any input
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
            while(1)
            {
                if(redraw)
                {
                    clearRow(y, field.label.length());
                    std::string& keyName = allKeys[allKeysIndex].second;
                    putText(field.label.length(), y, keyName);
                }

                redraw = true;
                int key = getKey();

                if(key == Key::Left) 
                {
                    allKeysIndex = (allKeysIndex + 1) % allKeys.size();
                }
                else if(key == Key::Right) 
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

void userAdd() 
{
    std::visit([](auto& tab) {
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
    }, activeTab);
}

void userEdit()
{
    std::visit([](auto& tab) {
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
    }, activeTab);
}

void userDelete() 
{
    std::visit([](auto& tab) {
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
    }, activeTab);
}

void userOrder() 
{
    std::visit([](auto& tab) {
        auto& currentList = tab.first;

        if(!currentList->isActive() || currentList->empty() || currentList->getPosition() == 0)
        {
            return;
        }
        currentList->orderUp(currentList->getPosition());
        currentList->moveUp();
    }, activeTab);
}