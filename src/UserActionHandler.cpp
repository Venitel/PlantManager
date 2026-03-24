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
    plantList.activate();
}

void activateDetails()
{
    plantDetails.activate();
}

void userAdd() 
{
    if(!plantList.isActive())
    {
        return;
    }

    const int bottomRow = getBottomRow();
    setCursor(0, bottomRow);
    std::cout<< "--- Add plant (grey = optional) ---\n";

    auto newRecord = plantList.getBlankRecord();;
    std::vector<Field> fields = newRecord.getFields();
    int rowNum = 1; //title is first
    for(Field& field : fields)
    {
        if(!field.mandatory)
        {
            setColor(FOREGROUND_INTENSITY);
        }
        field.setter(inputAt(0, bottomRow + rowNum, field.label, field.size, field.mandatory));
        resetColor();
        ++rowNum;
    }
    plantList.addRecord(newRecord);
    plantList.moveLast();

    // Clear the input area
    for(int i = bottomRow; i <= bottomRow + 4; i++) //one extra line for error, multi line notes etc.
    { 
        clearRow(i);
    }
}

void userEdit()
{
    if(!plantDetails.isActive() || plantList.empty())
    {
        return;
    }

    auto& record = plantList.getSelectedRecord();

    int bottomRow = getBottomRow();
    setCursor(0, bottomRow);
    std::cout<< "--- Edit plant " + record.getName() + " ---\n";

    Field editField = record.getFields()[plantDetails.getPosition()];
    editField.setter(inputAt(0, bottomRow + 1, editField.label, editField.size, editField.mandatory));
 
    plantList.editRecord(plantList.getPosition());

    for(int i = bottomRow; i <= bottomRow + 2; i++) //one extra line for error, multi line notes etc.
    { 
        clearRow(i);
    }
}

void userDelete() 
{
    if(!plantList.isActive() || plantList.empty())
    {
        return;
    }
    const int bottomRow = getBottomRow();
    std::string confirmation = inputAt(0, bottomRow, "Confirm delete " + plantList.getSelectedRecord().getName() + " (Y/N): ", 1, true);
    clearRow(bottomRow);

    if(toupper(confirmation[0]) != 'Y')
    {
        return;
    }

    plantList.deleteRecord(plantList.getPosition());

    if(plantList.getPosition() >= plantList.recordCount() && plantList.getPosition() > 0) 
    {
        plantList.moveUp();
    }
}