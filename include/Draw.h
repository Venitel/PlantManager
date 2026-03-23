#ifndef DRAW_H
#define DRAW_H

#include "Sections.h"

#include <vector>
#include <string>

void putText(int x, int y, const std::string& text);
void putError(int x, int y, const std::string& text);
std::string inputAt(int x, int y, const std::string& prompt, int maxLength, bool checkEmpty = false);

std::vector<std::string> wrapText(const std::string& text, int width);
void drawHeader();
void drawFooter(int row, bool listFuncs);
void drawLine(int x, int y, int width, char symbol);
void clearRow(int row);
int getBottomRow();

void drawList(int row);
void drawDetails(int row);
void drawAll();

#endif