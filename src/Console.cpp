#include "Console.h"

void initConsole()
{
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    setConsoleSize(100, 25);
    showCursor(false);
    clearScreen();
}

void terminateConsole()
{
    clearScreen();
    showCursor(true);
}

void setConsoleSize(int width, int height) 
{    
    COORD bufferSize = {(SHORT)width, (SHORT)height};
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize);

    SMALL_RECT windowSize = {0, 0, (SHORT)(width - 1), (SHORT)(height - 1)};
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize);
}

void setCursor(int x, int y) 
{
    COORD coord{(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void setColor(WORD attribute) 
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attribute);
}

void resetColor() 
{
    setColor(Colors::Default);
}

void setColor(Colors color)
{
    switch(color)
    {
        case Colors::Error : setColor(BACKGROUND_RED); break;
        case Colors::Title : setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY); break;
        case Colors::Selection : setColor(BACKGROUND_GREEN); break;
        case Colors::Name : setColor(FOREGROUND_GREEN); break;
        case Colors::Inactive :
        case Colors::Optional : setColor(FOREGROUND_INTENSITY); break; //this by itself is grey
        case Colors::Dormant:
        case Colors::List : setColor(FOREGROUND_GREEN | FOREGROUND_BLUE); break;
        case Colors::DueToday : setColor(FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); break;
        case Colors::DuePast : setColor(FOREGROUND_RED | FOREGROUND_INTENSITY); break;
        case Colors::DueFuture :
        case Colors::Default : setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); break; //White
        default : resetColor(); 
    }
}

void showCursor(bool show) 
{
    CONSOLE_CURSOR_INFO cursorInfo{1, show};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void clearScreen() 
{
    system("cls");
}