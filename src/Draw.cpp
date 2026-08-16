#include "Draw.h"
#include "Console.h"
#include "Utils.h"
#include "Menu.h"

#include <iostream>
#include <algorithm>
#include <cmath>

namespace
{
    constexpr int sectionWidth = 50;
    constexpr int sectionHeight = 10;
    constexpr int headerOffset = 3;
    constexpr int footerOffset = 4;
}

int getBottomRow()
{
    return headerOffset + sectionHeight + footerOffset;
}

std::vector<std::string> wrapText(const std::string& text, const int width) 
{
    std::vector<std::string> lines;
    for (int i = 0; i <= (int)text.length()/width; i++)
    {
        lines.push_back(text.substr(i*width, width));
    }

    return lines;
}

void drawLine(const int x, const int y, const int width, const char symbol) 
{
    std::string line(width, symbol);
    putText(x, y, line);
}

void clearRow(const int row, const int offset)
{
    drawLine(offset, row, sectionWidth*2 - offset, ' ');
}

void putText(const int x, const int y, const std::string& text) 
{
    setCursor(x, y);
    std::cout << text;
}

void putError(const int x, const int y, const std::string& text)
{
    setColor(Colors::Error);
    putText(x, y, text);
    resetColor();
}

void drawHeader() 
{
    setColor(Colors::Title);
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

            setColor(Colors::Inactive);
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
        
        int pages = std::ceil(static_cast<double>(currentList->recordCount()) / sectionHeight);
        if(pages > 1)
        {
            int currentPage = std::ceil(static_cast<double>(currentList->getPosition()+1) / sectionHeight); //Position starts with 0, add 1
            currentTab += " (Page " + std::to_string(currentPage) + "/" + std::to_string(pages) + ")";
        }

        setColor(Colors::Title);
        putText(0, 3, currentTab);

        putText(sectionWidth, 3, "Details");
        resetColor();
    }, activeTab);
}

void clearSection(const int x, const int y, const int width, const int height)
{
    for(int i = 0; i < height; i++)
    {
        drawLine(x, y + i, width, ' '); // overwrite with spaces
    }
}

void drawInstructionsRow(const int row, const std::string& title)
{
    int pos = 0;
    const std::string titleStart = "--- " + title + " [ ";
    const std::string helpSeparator = " | ";
    const std::string helpOptional = "optional";
    const std::string helpList = "← list →";
    const std::string titleEnd = " ] ---";

    putText(pos, row, titleStart);
    pos += titleStart.length();

    setColor(Colors::Optional);
    putText(pos, row, helpOptional);
    pos += helpOptional.length();
    resetColor();

    putText(pos, row, helpSeparator);
    pos += helpSeparator.length();

    setColor(Colors::List);
    putText(pos, row, helpList);
    pos += 8; //hardcoded because the arrows are multi-byte and give wrong .length()
    resetColor();

    putText(pos, row, titleEnd);
}

void drawList(const int row)
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
                setColor(Colors::Selection);
            }
            else
            {
                setColor(currentList->getRecord(i).getNameColor());
            }
            putText(0, row + (i % sectionHeight), currentList->getRecord(i).getName());
            resetColor();
        }
    }, activeTab);
}

void drawDetails(const int row) 
{
    std::visit([&](auto& tab) {
        auto& currentList = tab.first;
        auto& currentDetails = tab.second;

        clearSection(sectionWidth, row, sectionWidth, sectionHeight);

        if(currentList->empty()) return;

        auto& record = currentList->getSelectedRecord();

        int selection = currentDetails->isActive() ? currentDetails->getPosition() : -1;

        std::vector<Field> fields = record.getDisplayableFields();
        std::vector<DetailLine> extraDetails = record.getExtraDetails();

        int fieldsNum = 0;
        int printedRows = 0; 
        //Name has different color if not selected - it is always first so we set it before loop
        setColor(Colors::Name);
        for(Field& field : fields)
        {
            drawExtraDetailLine(row, printedRows, extraDetails);
            if(field.inputType != Field::InputType::ReadOnly)
            {
                ++fieldsNum;
            }

            int labelLength = (int)field.label.length();
            std::string text = field.value;
            if(field.isForeign())
            {
                if(field.value == "-1")
                {
                    //Empty foreign reference, either from delete or empty list
                    setColor(Colors::Error);
                }
                text = record.getForeignName(field.dataType);
            }
            else if(field.dataType == Field::DataType::Month)
            {
                text = DateUtils::Months[stoi(field.value)-1].second;
            }
            else if(field.dataType == Field::DataType::Date && text.empty())
            {
                text = "Never";
            }
            else if(field.dataType == Field::DataType::Number && text == "-1")
            {
                text = "Disabled";
            }

            if(selection >= 0 && selection+1 == fieldsNum) //Selection index starts from 0 
            {
                //this is intentionally at the end - overwrite other colors, selection has prio
                setColor(Colors::Selection);
            }

            std::vector<std::string> textLines = wrapText(text, sectionWidth - labelLength);
            for(int i = 0; i < (int)textLines.size(); i++)
            {
                std::string linePrefix = i == 0 ? field.label : std::string(labelLength, ' ');
                putText(sectionWidth, row + printedRows, linePrefix + textLines[i]);
                ++printedRows;
            }
            resetColor();
        }
    }, activeTab);
}

void drawExtraDetailLine(const int row, int& printedRows, const std::vector<DetailLine>& extraDetails)
{
    auto it = std::find_if(extraDetails.begin(), extraDetails.end(), [&](auto& detail) {return printedRows+1 == detail.row;});
    if(it != extraDetails.end())
    {
        setColor(it->color);
        putText(sectionWidth, row + printedRows, it->text);
        ++printedRows;
        resetColor();
        drawExtraDetailLine(row, printedRows, extraDetails);
    }
}

void drawFooter(const int row) 
{
    drawLine(0, row, 2*sectionWidth, '-');
    clearRow(row + 1);
    clearRow(row + 2);

    std::string keys[2];
    for(auto& item : getMenu())
    {
        if(item.active())
        {
            if(!keys[item.row].empty()) 
            {
                keys[item.row] += " | ";
            }
            keys[item.row] += item.label;
        }
    }
    putText(0, row + 1, keys[0]);
    putText(0, row + 2, keys[1]);
}

void drawAll() 
{
    drawHeader();
    drawList(4); //offset by header
    drawDetails(4); //offset by header
    drawFooter(sectionHeight + 4); //offset by header and sections
}