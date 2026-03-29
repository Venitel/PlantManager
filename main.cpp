#include "Console.h"
#include "Draw.h"
#include "UserActionHandler.h"
#include "Database.h"
#include "Logger.h"
#include "Sections.h"

int main() 
{
    if(!Database::getInstance().open("PlantManager.db")
        || !Logger::getInstance().open("PlantManager.log"))
    {
        putError(0, 0, "Failed to open Database/Log files!");
        getKey();
        return 1;
    }
    Logger::getInstance().info("--- RUN ---");
    loadAllListsFromDb();

    initConsole();

    bool redraw = true;
    while(true) 
    {
        if(redraw) {drawAll();}
        redraw = true;

        switch(toupper(getKey())) 
        {
            case Key::Up : moveActiveSectionUp(); break;
            case Key::Down : moveActiveSectionDown(); break;
            case Key::Left : activateList(); break;
            case Key::Right : activateDetails(); break;
            case Key::Tab : nextTab(); break;
            case 'A': userAdd(); break; 
            case 'D': userDelete(); break;
            case 'E': userEdit(); break;
            case 'M': userOrder(); break;
            case 'Q': Database::getInstance().close(); terminate(); return 0;
            default: redraw = false; //unsupported key - prevent flicker
        }
    }
}