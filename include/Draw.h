#ifndef DRAW_H
#define DRAW_H

#include "Sections.h"

#include <vector>
#include <string>

void putText(int x, int y, const std::string& text);
void putError(int x, int y, const std::string& text);

std::vector<std::string> wrapText(const std::string& text, int width);
void drawHeader();
void drawFooter(int row, bool listFuncs);
void drawLine(int x, int y, int width, char symbol);
void clearRow(int row, int offset=0);
int getBottomRow();

void drawInstructionsRow(int row, const std::string& title);

void drawList(int row);
void drawDetails(int row);
void drawExtraDetailLine(int row, int& printedRows, const std::vector<DetailLine>& extraDetails);
void drawAll();

#endif