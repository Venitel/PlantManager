#include "UserActionHandler.h"
#include "Console.h"
#include "Draw.h"
#include "Database.h"
#include "Logger.h"
#include "Utils.h"

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
        if(!Field::isForeign(selectedField.dataType))
        {
            return;
        }

        auto it = std::find_if(allTabs.begin(), allTabs.end(), [&](const Tabs& tab) {
            bool match = false;
            std::visit([&](auto& pair) {
                match = pair.first->getTabName() == Database::getTableName(selectedField.dataType);
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
    if(field.inputType == Field::InputType::List) 
    {
        if(field.dataType == Field::DataType::Month)
        {
            field.setter(getValueByList(x, y, field, DateUtils::Months));
        }
        else
        {
            field.setter(getValueByList(x, y, field, Database::getInstance().getAllKeys(field.dataType)));
        }
    }
    else
    {
        if(field.inputType == Field::InputType::Optional)
        {
            setColor(Colors::Optional);
        }

        bool checkEmpty = field.inputType==Field::InputType::Mandatory;
        bool checkDate = field.dataType==Field::DataType::Date;
        bool checkPositive = field.dataType==Field::DataType::Number;
        field.setter(inputAt(x, y, field.label, field.size, checkEmpty, checkDate, checkPositive)); 
    }
}

std::string getValueByList(int x, int y, Field& field, const std::vector<std::pair<int, std::string>>& pairs)
{
    setColor(Colors::List);
    putText(x, y, field.label);
    resetColor();

    if(!pairs.empty())
    {
        int index = 0;
        try //in case stoi fails
        {
            auto it = std::find_if(pairs.begin(), pairs.end(), [&](auto& pair) {return stoi(field.value) == pair.first;});
            if(it != pairs.end())
            {
                index = std::distance(pairs.begin(), it);
            }
        }
        catch(const std::exception& exc)
        {
            Logger::getInstance().error(exc.what()); //continue with index=0
        }

        bool redraw = true;
        while(true)
        {
            if(redraw)
            {
                clearRow(y, field.label.length());
                std::string keyName = pairs[index].second;
                putText(field.label.length(), y, keyName);
            }

            redraw = true;
            int key = getKey();

            if((Key)key == Key::Right) 
            {
                index = (index + 1) % pairs.size();
            }
            else if((Key)key == Key::Left) 
            {
                --index;
                if(index < 0) 
                {
                    index = pairs.size() - 1;
                }
            }
            else if((Key)key == Key::Enter)
            {
                return std::to_string(pairs[index].first);
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
        return "";
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
            if(field.inputType != Field::InputType::NoInput)
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

        currentList->updateRecord(currentList->getPosition());

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

std::string inputAt(int x, int y, const std::string& prompt, int maxLength, bool checkEmpty, bool checkDate, bool checkPositiveNumber) 
{
    //input row requires one free row below for error messages

    putText(x, y, prompt);
    showCursor(true);
    resetColor(); //we allow other functions to set prompt color before calling inputAt
    
    const int inputStart = x + prompt.length();
    std::string input;
    std::string errorMessage;
    while(true)
    {
        if(!std::getline(std::cin, input)) {errorMessage = "Input invalid!";}
        else if(checkEmpty && input.empty()) {errorMessage = "Input cannot be empty!";}
        else if(checkPositiveNumber && !Utils::isNumber(input)) {errorMessage = "Not a number!";}
        else if(checkPositiveNumber && stoi(input) < 0) {errorMessage = "Number must be positive!";}
        //In date we check !input.empty() because empty date is allowed if optional. If it's not allowed, checkEmpty will catch it first
        else if(checkDate && !input.empty() && !DateUtils::isValidDate(input)) {errorMessage = "Not a valid date! Expected format YYYY-MM-DD.";}
        else if(checkDate && input > DateUtils::today()) {errorMessage = "Date cannot be in the future!";}
        else if(input.length() > maxLength) {errorMessage = "Input too long!";}
        else // Accepted input
        {
            showCursor(false);
            clearRow(y+1); //clear error
            return input;
        }
        clearRow(y, inputStart);
        clearRow(y+1);
        putError(inputStart, y+1, errorMessage);
        setCursor(inputStart, y);
    }
}

bool waterPlant()
{
    bool result = false;

    std::visit([&](auto& tab) {
        auto& currentList = tab.first;
        auto& currentDetails = tab.second;

        if(currentList->empty())
        {
            return;
        }
        using TabType = std::decay_t<decltype(tab)>;
        if constexpr (std::is_same_v<TabType, std::pair<ListSection<Plant>*, DetailsSection<Plant>*>>) 
        {
            if(!currentList->isActive())
            {
                if(currentList->getSelectedRecord().getFields()[currentDetails->getPosition()].colNam != "lastWatered")
                {
                    return;
                }
            }

            tab.first->getSelectedRecord().waterNow();
        }
        result = true;
    }, activeTab);

    return result;
}