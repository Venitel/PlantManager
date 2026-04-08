#ifndef CONSOLE_H
#define CONSOLE_H

#include "Colors.h"
#include <windows.h>

void initConsole();
void terminateConsole();
void setConsoleSize(const int width, const int height);
void clearScreen();
void clearSection(const int x, const int y, const int width, const int height);

void setCursor(const int x, const int y);
void showCursor(const bool show);

void setColor(const WORD attribute);
void setColor(const Colors color);
void resetColor();

#endif