#include "UserActionHandler.h"
#include "Console.h"
#include "Draw.h"

#include <iostream>
#include <conio.h>

int getKey() 
{
    return _getch();
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
     std::visit([&](auto& tab) {
        tab.first->activate();
    }, activeTab);
}

void activateDetails()
{
    std::visit([&](auto& tab) {
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

    std::visit([&](auto& tab) {
        tab.first->activate(); //activate list section on tab change
    }, activeTab);
}

void userAdd() 
{
    std::visit([&](auto& tab) {
        auto& currentList = tab.first;

        if(!currentList->isActive())
        {
            return;
        }

        const int bottomRow = getBottomRow();
        setCursor(0, bottomRow);

        auto newRecord = currentList->getBlankRecord();

        std::cout<< "--- Add " + newRecord.getTabName() + " (grey = optional) ---\n";

        std::vector<Field> fields = newRecord.getFields();
        int rowNum = 1; //title is first
        for(Field& field : fields)
        {
            if(!field.userEditable)
            {
                continue;
            }
            if(!field.mandatory)
            {
                setColor(FOREGROUND_INTENSITY); //this by itself is actually grey
            }
            field.setter(inputAt(0, bottomRow + rowNum, field.label, field.size, field.mandatory));
            resetColor();
            ++rowNum;
        }
        currentList->addRecord(newRecord);
        currentList->moveLast();

        // Clear the input area
        for(int i = bottomRow; i <= bottomRow + 4; i++) //one extra line for error, multi line notes etc.
        { 
            clearRow(i);
        }
    }, activeTab);
}

void userEdit()
{
    std::visit([&](auto& tab) {
        auto& currentList = tab.first;
        auto& currentDetails = tab.second;

        if(!currentDetails->isActive() || currentList->empty())
        {
            return;
        }

        auto& record = currentList->getSelectedRecord();

        int bottomRow = getBottomRow();
        setCursor(0, bottomRow);
        std::cout<< "--- Edit plant " + record.getName() + " ---\n";

        Field editField = record.getFields()[currentDetails->getPosition()];
        editField.setter(inputAt(0, bottomRow + 1, editField.label, editField.size, editField.mandatory));
    
        currentList->editRecord(currentList->getPosition());

        for(int i = bottomRow; i <= bottomRow + 2; i++) //one extra line for error, multi line notes etc.
        { 
            clearRow(i);
        }
    }, activeTab);
}

void userDelete() 
{
    std::visit([&](auto& tab) {
        auto& currentList = tab.first;

        if(!currentList->isActive() || currentList->empty())
        {
            return;
        }
        const int bottomRow = getBottomRow();
        std::string confirmation = inputAt(0, bottomRow, "Confirm delete " + currentList->getSelectedRecord().getName() + " (Y/N): ", 1, true);
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
    std::visit([&](auto& tab) {
        auto& currentList = tab.first;

        if(!currentList->isActive() || currentList->empty() || currentList->getPosition() == 0)
        {
            return;
        }
        currentList->orderUp(currentList->getPosition());
        currentList->moveUp();
    }, activeTab);
}