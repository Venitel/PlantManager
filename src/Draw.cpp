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
    drawLine(0, 1, 2*sectionWidth, '-');

    setColor(FOREGROUND_GREEN);
    putText(0, 2, "Plants");
    putText(sectionWidth, 2, "Details");
    resetColor();
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
    if(plantList.empty()) 
    {
        putError(0, row, "List empty");
        return;
    }

    clearSection(0, row, sectionWidth, sectionHeight);

    for(int i = 0; i < plantList.recordCount(); i++) 
    {
        if(plantList.isActive() && i == plantList.getPosition()) 
        {
            setColor(BACKGROUND_GREEN);
        }
        putText(0, row + i, plantList.getRecord(i).getName());
        resetColor();
    }
}

void drawDetails(int row) 
{
    clearSection(sectionWidth, row, sectionWidth, sectionHeight);

    if(plantList.empty()) return;

    auto record = plantList.getSelectedRecord();

    int selection = plantDetails.isActive() ? plantDetails.getPosition() : -1;

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
}

void drawFooter(int row) 
{
    drawLine(0, row, 2*sectionWidth, '-');
    clearRow(row + 1);
    if(plantList.isActive())
    {
        putText(0, row + 1, "↑ ↓ ← →: Select   A: Add   D: Delete   M: Move Up   Q: Quit");
    }
    else
    {
        putText(0, row + 1, "↑ ↓ ← →: Select   E: Edit   Q: Quit");
    }
}

void drawAll() 
{
    drawHeader();
    drawList(3); //offset by header
    drawDetails(3); //offset by header
    drawFooter(sectionHeight + 3); //offset by header
}