#include "Console.h"
#include "Draw.h"
#include "UserActionHandler.h"
#include "Database.h"

enum Key
{
    Up = 72,
    Down = 80,
    Left = 75,
    Right = 77
};

int main() 
{
    Database::getInstance().open("PlantManager.db");
    plantList.initFromDb();
    initConsole();
    drawAll();

    while(true) 
    {
        int key = getKey();

        if(key == 0 || key == 0xE0) //Arrow keys send two bytes: 0xE0 then a code
        {
            key = getKey();
            if(key == Key::Up) {moveActiveSectionUp();} 
            else if(key == Key::Down) {moveActiveSectionDown();} 
            else if(key == Key::Left) {activateList();}
            else if(key == Key::Right) {activateDetails();}
        } 
        else 
        {
            switch(toupper(key)) 
            {
                case 'A': userAdd(); break; 
                case 'D': userDelete(); break;
                case 'E': userEdit(); break;
                case 'Q': Database::getInstance().close(); terminate(); return 0;
            }
        }

        drawAll();
    }
}