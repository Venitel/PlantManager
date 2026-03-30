#ifndef CONSOLE_H
#define CONSOLE_H

#include <windows.h>

enum Colors 
{
    Error,
    Title,
    Selection,
    Name,
    Inactive,
    Optional, 
    List
};

void initConsole();
void terminateConsole();
void setConsoleSize(int width, int height);
void clearScreen();
void clearSection(int x, int y, int width, int height);

void setCursor(int x, int y);
void showCursor(bool show);

void setColor(WORD attribute);
void setColor(Colors color);
void resetColor();

#endif