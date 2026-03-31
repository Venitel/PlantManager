#include "Console.h"
#include "Draw.h"
#include "UserActionHandler.h"
#include "Database.h"
#include "Logger.h"
#include "Sections.h"

void quit()
{
    Database::getInstance().close(); 
    terminateConsole(); 
    Logger::getInstance().info("--- QUIT ---\n");
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

int main() 
{
    if(!Database::getInstance().open("PlantManager.db")
        || !Logger::getInstance().open("PlantManager.log"))
    {
        putError(0, 0, "Failed to open Database/Log files!");
        getKey();
        return 1;
    }

    SetConsoleCtrlHandler(exitHandler, TRUE);
    Logger::getInstance().info("--- RUN ---");
    loadAllListsFromDb();
    initConsole();
    drawAll();

    while(true) 
    {
        int key = toupper(getKey());
        if(!onCooldown(key))
        {
            bool redraw;
            switch(key) 
            {
                //Each key handler returns true/false if successful
                //Only redraw on true (if something changed) to prevent unnecessary flicker
                case Key::Up : redraw = moveActiveSectionUp(); break;
                case Key::Down : redraw = moveActiveSectionDown(); break;
                case Key::Left : redraw = activateList(); break;
                case Key::Right : redraw = activateDetails() || goToForeignRecord(); break;
                case Key::Tab : redraw = nextTab(); break;
                case 'A': redraw = userAdd(); break; 
                case 'D': redraw = userDelete(); break;
                case 'E': redraw = userEdit(); break;
                case 'M': redraw = userOrder(); break;
                case 'Q': quit(); return 0;
                default: redraw = false;
            }
            if(redraw) {drawAll();}
        }
    }
}