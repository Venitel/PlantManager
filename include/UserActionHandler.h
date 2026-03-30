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

bool moveActiveSectionUp();
bool moveActiveSectionDown();
bool activateList();
bool activateDetails();
bool nextTab();

void getFieldFromUser(int x, int y, Field& field);

bool userAdd();
bool userEdit();
bool userDelete();
bool userOrder();

#endif