#ifndef USERACTIONHANDLER_H
#define USERACTIONHANDLER_H

#include "Sections.h"

int getKey();

void moveActiveSectionUp();
void moveActiveSectionDown();
void activateList();
void activateDetails();
void nextTab();

void userAdd();
void userEdit();
void userDelete();
void userOrder();

#endif