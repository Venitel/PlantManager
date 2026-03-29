#ifndef USERACTIONHANDLER_H
#define USERACTIONHANDLER_H

#include "Sections.h"

enum Key
{
    Up = -72,
    Down = -80,
    Left = -75,
    Right = -77,
    Tab = 9,
    Enter = 13
};

int getKey();

void moveActiveSectionUp();
void moveActiveSectionDown();
void activateList();
void activateDetails();
void nextTab();

void getFieldFromUser(int x, int y, Field& field);

void userAdd();
void userEdit();
void userDelete();
void userOrder();

#endif