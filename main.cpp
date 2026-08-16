#include "Console.h"
#include "Draw.h"
#include "UserActionHandler.h"
#include "Database.h"
#include "Logger.h"
#include "Sections.h"
#include "Utils.h"
#include "Menu.h"

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
    initConsole();
    drawAll();

    while(true) 
    {
        int key = toupper(getKey());
        if(!onCooldown(key))
        {
            Action action = Action::NoRedraw;
            for(auto& item : getMenu())
            {
                if(item.key == (Key)key && item.active())
                {
                    action = item.action();
                    if(action == Action::Quit)
                    {
                        quit(); 
                        return 0;
                    }
                    break;
                }
            }
            if(checkPlantCache() || action == Action::Redraw) {drawAll();}
        }
    }
}