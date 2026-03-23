#ifndef CONSOLE_H
#define CONSOLE_H

#include <windows.h>

void initConsole();
void terminate();
void setConsoleSize(int width, int height);
void clearScreen();
void clearSection(int x, int y, int width, int height);

void setCursor(int x, int y);
void showCursor(bool show);

void setColor(WORD attribute);
void resetColor();

#endif