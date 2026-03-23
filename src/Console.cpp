#include "Console.h"

void initConsole()
{
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    setConsoleSize(100, 25);
    showCursor(false);
    clearScreen();
}

void terminate()
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
    setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
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