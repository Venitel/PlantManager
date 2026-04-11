#ifndef USERACTIONHANDLER_H
#define USERACTIONHANDLER_H

#include "Sections.h"
#include <functional>

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
int getCooldown(const int key);
bool onCooldown(const int key);

bool moveActiveSectionUp();
bool moveActiveSectionDown();
bool activateList();
bool activateDetails();
bool nextTab();
bool goToForeignRecord();

bool handlePlantAction(std::function<bool(Plant&)> action, const std::string& field);
bool waterPlant();
bool feedPlant();
bool postponePlant();

void getFieldFromUser(const int x, const int y, Field& field);
std::string getValueByList(const int x, const int y, Field& field, const std::vector<std::pair<int, std::string>>& pairs);
std::string inputAt(const int x, const int y, const std::string& prompt, const int maxLength, const bool checkEmpty = false, const bool checkDate = false, const bool checkPositiveNumber = false);

bool userAdd();
bool userEdit();
bool userDelete();
bool userOrder();

#endif