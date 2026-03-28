#include "Draw.h"
#include "Console.h"

#include <iostream>

namespace
{
    constexpr int sectionWidth = 50;
    constexpr int sectionHeight = 10;
    constexpr int headerOffset = 3;
    constexpr int footerOffset = 3;
}

int getBottomRow()
{
    return headerOffset + sectionHeight + footerOffset;
}

std::vector<std::string> wrapText(const std::string& text, int width) 
{
    std::vector<std::string> lines;
    for (int i = 0; i <= (int)text.length()/width; i++)
    {
        lines.push_back(text.substr(i*width, width));
    }

    return lines;
}

void drawLine(int x, int y, int width, char symbol) 
{
    setCursor(x, y);
    std::string line(width, symbol);
    std::cout << line;
}

void clearRow(int row)
{
    drawLine(0, row, sectionWidth*2, ' ');
}

void putText(int x, int y, const std::string& text) 
{
    setCursor(x, y);
    std::cout << text;
}

void putError(int x, int y, const std::string& text)
{
    setColor(BACKGROUND_RED);
    putText(x, y, text);
    resetColor();
}

std::string inputAt(int x, int y, const std::string& prompt, int maxLength, bool checkEmpty) 
{
    //input row requires one free row below for error messages

    setCursor(x, y);
    std::cout << prompt;
    showCursor(true);
    resetColor(); //we allow other functions to set prompt color before calling inputAt
    
    const int inputStart = x + prompt.length();
    const std::string errorMessage = "Input invalid!";
    std::string input;
    while(!(std::getline(std::cin, input) && input.length() <= maxLength && !(checkEmpty && input.empty())))
    {
        drawLine(inputStart, y, sectionWidth*2 - inputStart, ' '); //clear input
        clearRow(y+1);
        putError(inputStart, y+1, errorMessage);
        setCursor(inputStart, y);
    }
    showCursor(false);
    drawLine(inputStart, y+1, errorMessage.length(), ' '); //clear error

    return input;
}

void drawHeader() 
{
    setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    putText(0, 0, "PLANT MANAGER");
    resetColor();
    //Tabs
    clearRow(1);
    bool firstTab = true;
    int tabPos = 0;
    std::string tabSeparator = " | ";
    for(auto& tab : allTabs)
    {
        std::visit([&](auto& pair) {
            std::string tabName = pair.first->getTabName();
            tabName[0] = toupper(tabName[0]); //capitalize first letter to look pretty

            setColor(FOREGROUND_INTENSITY); //this by itself is actually grey
            if(!firstTab)
            {
                putText(tabPos, 1, tabSeparator);
                tabPos += tabSeparator.length(); 
            }

            if(tab == activeTab)
            {
                resetColor();
            }
            putText(tabPos, 1, tabName);
            resetColor();
            tabPos += tabName.length();

            firstTab = false;
        }, tab);
    }

    drawLine(0, 2, 2*sectionWidth, '-');

    //Section names
    clearRow(3);
    std::visit([&](auto& tab) {
        auto& currentList = tab.first;
        
        std::string currentTab = currentList->getTabName();
        currentTab[0] = toupper(currentTab[0]); //capitalize first letter to look pretty

        setColor(FOREGROUND_GREEN);
        putText(0, 3, currentTab);
        putText(sectionWidth, 3, "Details");
        resetColor();
    }, activeTab);
}

void clearSection(int x, int y, int width, int height)
{
    for(int i = 0; i < height; i++)
    {
        drawLine(x, y + i, width, ' '); // overwrite with spaces
    }
}

void drawList(int row)
{
    std::visit([&](auto& tab) {
        auto& currentList = tab.first;

        clearSection(0, row, sectionWidth, sectionHeight);

        if(currentList->empty()) 
        {
            putError(0, row, "List empty");
            return;
        }

        int startRow = currentList->getPosition() - (currentList->getPosition() % sectionHeight); //rounded down the nearest sectionHeight
        int maxRow = startRow + sectionHeight;
        if(maxRow > currentList->recordCount())
        {
            maxRow = currentList->recordCount();
        }
        for(int i = startRow; i < maxRow; i++) 
        {
            if(currentList->isActive() && i == currentList->getPosition()) 
            {
                setColor(BACKGROUND_GREEN);
            }
            putText(0, row + (i % sectionHeight), currentList->getRecord(i).getName());
            resetColor();
        }
    }, activeTab);
}

void drawDetails(int row) 
{
    std::visit([&](auto& tab) {
        auto& currentList = tab.first;
        auto& currentDetails = tab.second;

        clearSection(sectionWidth, row, sectionWidth, sectionHeight);

        if(currentList->empty()) return;

        auto& record = currentList->getSelectedRecord();

        int selection = currentDetails->isActive() ? currentDetails->getPosition() : -1;

        std::vector<Field> fields = record.getFields();
        int fieldsNum = 0;
        int printedRows = 0; 
        //Name has different color if not selected - it is always first so we set it before loop
        setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        for(Field& field : fields)
        {
            if(!field.userEditable)
            {
                continue;
            }
            if(selection == fieldsNum) 
            {
                setColor(BACKGROUND_GREEN);
            }
            ++fieldsNum;

            int labelLength = (int)field.label.length();
            std::vector<std::string> textLines = wrapText(field.value, sectionWidth - labelLength);
            for(int i = 0; i < (int)textLines.size(); i++)
            {
                std::string linePrefix = i == 0 ? field.label : std::string(labelLength, ' ');
                putText(sectionWidth, row + printedRows, linePrefix + textLines[i]);
                ++printedRows;
            }
            resetColor();

            if(fieldsNum == 1) //add a horizontal line after name (first field)
            {
                drawLine(sectionWidth, row+1, sectionWidth, '-');
                ++printedRows;
            }
        }
    }, activeTab);
}

void drawFooter(int row) 
{
    std::visit([&](auto& tab) {
        auto& currentList = tab.first;

        drawLine(0, row, 2*sectionWidth, '-');
        clearRow(row + 1);
        if(currentList->isActive())
        {
            putText(0, row + 1, "↑ ↓ ← →: Select   TAB: Next Tab   A: Add   D: Delete   M: Move Up   Q: Quit");
        }
        else
        {
            putText(0, row + 1, "↑ ↓ ← →: Select   TAB: Next Tab   E: Edit                           Q: Quit");
        }
    }, activeTab);
}

void drawAll() 
{
    drawHeader();
    drawList(4); //offset by header
    drawDetails(4); //offset by header
    drawFooter(sectionHeight + 4); //offset by header
}