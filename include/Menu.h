#ifndef MENU_H
#define MENU_H

#include <string>
#include <functional>

enum class Key;

enum class Action
{
    Redraw,
    NoRedraw,
    Quit
};

struct MenuItem
{
    int row;
    std::string label;
    std::function<bool()> active;
    Key key;
    std::function<Action()> action;
};

std::vector<MenuItem> getMenu();

Action toAction(bool result);

bool listActive();
bool detailsActive();
bool isSettingsTab();
bool isSelectedFieldForeign();
bool isPlantList();
bool isPlantFieldSelected(const std::string& colNam);

#endif