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

        switch(toupper(getKey())) 
        {
            //Each key handler returns true/false if successful
            //Only redraw on true (if something changed) to prevent unnecessary flicker
            case Key::Up : redraw = moveActiveSectionUp(); break;
            case Key::Down : redraw = moveActiveSectionDown(); break;
            case Key::Left : redraw = activateList(); break;
            case Key::Right : redraw = activateDetails(); break;
            case Key::Tab : redraw = nextTab(); break;
            case 'A': redraw = userAdd(); break; 
            case 'D': redraw = userDelete(); break;
            case 'E': redraw = userEdit(); break;
            case 'M': redraw = userOrder(); break;
            case 'Q': Database::getInstance().close(); terminate(); return 0;
            default: redraw = false;
        }
    }
}