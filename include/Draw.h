#ifndef DRAW_H
#define DRAW_H

#include "Sections.h"

#include <vector>
#include <string>

void putText(const int x, const int y, const std::string& text);
void putError(const int x, const int y, const std::string& text);

std::vector<std::string> wrapText(const std::string& text, const int width);
void drawHeader();
void drawFooter(const int row);
void drawLine(const int x, const int y, const int width, const char symbol);
void clearRow(const int row, const int offset=0);
int getBottomRow();

void drawInstructionsRow(const int row, const std::string& title);

void drawList(const int row);
void drawDetails(const int row);
void drawExtraDetailLine(const int row, int& printedRows, const std::vector<DetailLine>& extraDetails);
void drawAll();

#endif