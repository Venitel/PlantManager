#include "Console.h"
#include "Draw.h"
#include "UserActionHandler.h"
#include "Database.h"
#include "Logger.h"
#include "Sections.h"
#include "Utils.h"

void quit()
{
    Database::getInstance().close(); 
    terminateConsole(); 
    Logger::getInstance().close();
}

BOOL WINAPI exitHandler(DWORD signal)
{
    switch(signal)
    {
        case CTRL_CLOSE_EVENT:
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            quit();
            return TRUE;
    }
    return FALSE;
}

bool checkPlantCache()
{
    static std::string dateCached;
    
    const std::string today = DateUtils::today();
    if(dateCached != today)
    {
        cachePlantData();
        dateCached = today;
        return true;
    }
    return false;
}

int main() 
{
    if(!Logger::getInstance().open("PlantManager.log")
       || !Database::getInstance().open("PlantManager.db"))
    {
        putError(0, 0, "Failed to open Database/Log files!");
        getKey();
        return 1;
    }

    SetConsoleCtrlHandler(exitHandler, TRUE);
    loadAllListsFromDb();
    checkPlantCache();
    cacheSettings();
    initConsole();
    drawAll();

    while(true) 
    {
        int key = toupper(getKey());
        if(!onCooldown(key))
        {
            bool redraw;
            switch((Key)key) 
            {
                //Each key handler returns true/false if successful
                //Only redraw on true (if something changed) to prevent unnecessary flicker
                case Key::Up : redraw = moveActiveSectionUp(); break;
                case Key::Down : redraw = moveActiveSectionDown(); break;
                case Key::Left : redraw = activateList(); break;
                case Key::Right : redraw = activateDetails() || goToForeignRecord(); break;
                case Key::Tab : redraw = nextTab(); break;
                case (Key)'A': redraw = userAdd(); break; 
                case (Key)'D': redraw = userDelete(); break;
                case (Key)'E': redraw = userEdit(); break;
                case (Key)'M': redraw = userOrder(); break;
                case (Key)'W': redraw = waterPlant(); break;
                case (Key)'F': redraw = feedPlant(); break;
                case (Key)'P': redraw = postponePlant(); break;
                case (Key)'Q': quit(); return 0;
                default: redraw = false;
            }
            if(checkPlantCache() || redraw) {drawAll();}
        }
    }
}