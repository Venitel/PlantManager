#ifndef USERACTIONHANDLER_H
#define USERACTIONHANDLER_H

#include "Sections.h"

enum class Key
{
    //For explaination why 256 see getKey
    Up = 72 + 256,
    Down = 80 + 256,
    Left = 75 + 256,
    Right = 77 + 256,
    Tab = 9,
    Enter = 13
};

int getKey();
int getCooldown(int key);
bool onCooldown(int key);

bool moveActiveSectionUp();
bool moveActiveSectionDown();
bool activateList();
bool activateDetails();
bool nextTab();
bool goToForeignRecord();
bool waterPlant();

void getFieldFromUser(int x, int y, Field& field);
std::string getValueByList(int x, int y, Field& field, const std::vector<std::pair<int, std::string>>& pairs);
std::string inputAt(int x, int y, const std::string& prompt, int maxLength, bool checkEmpty = false, bool checkDate = false, bool checkPositiveNumber = false);

bool userAdd();
bool userEdit();
bool userDelete();
bool userOrder();

#endif