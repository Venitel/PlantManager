#include "Menu.h"
#include "Sections.h"
#include "UserActionHandler.h"

#include <vector>

std::vector<MenuItem> getMenu()
{
    std::vector<MenuItem> buttons;

    //Navigation
    buttons.push_back( {0, "↑: Up",         
                        []() { return true; }, 
                        Key::Up, []() { return toAction(moveActiveSectionUp()); } 
                    });
    buttons.push_back({ 0, "↓: Down",       
                        []() { return true; }, 
                        Key::Down, []() { return toAction(moveActiveSectionDown()); } 
                    });
    buttons.push_back({ 0, "←: List",       
                        []() { return detailsActive(); }, 
                        Key::Left, []() { return toAction(activateList()); } 
                    });
    buttons.push_back({ 0, "→: Details",    
                        []() { return listActive(); }, 
                        Key::Right, []() { return toAction(activateDetails()); } 
                    });
    buttons.push_back({ 0, "TAB: Next Tab", 
                        []() { return true; }, 
                        Key::Tab, []() { return toAction(nextTab()); } 
                    });
    buttons.push_back({ 0, "Q: Quit",       
                        []() { return true; }, 
                        (Key)'Q', []() { return Action::Quit; } 
                    });

    //List actions
    buttons.push_back({ 1, "A: Add",        
                        []() { return listActive() && !isSettingsTab(); }, 
                        (Key)'A', []() { return toAction(userAdd()); } 
                    });
    buttons.push_back({ 1, "D: Delete",     
                        []() { return listActive() && !isSettingsTab(); }, 
                        (Key)'D', []() { return toAction(userDelete()); } 
                    });
    buttons.push_back({ 1, "M: Move Up",    
                        []() { return listActive() && !isSettingsTab(); }, 
                        (Key)'M', []() { return toAction(userOrder()); } 
                    });

    //Details actions
    buttons.push_back({ 1, "E: Edit",       
                        []() { return detailsActive(); }, 
                        (Key)'E', []() { return toAction(userEdit()); } 
                    });
    buttons.push_back({ 1, "→: Go To",      
                        []() { return isSelectedFieldForeign(); }, 
                        Key::Right, []() { return toAction(goToForeignRecord()); } 
                    });

    //Plant only actions
    buttons.push_back({ 1, "W: Water Now",  
                        []() { return isPlantList() || isPlantFieldSelected("lastWatered"); }, 
                        (Key)'W', []() { return toAction(waterPlant()); } 
                    });
    buttons.push_back({ 1, "F: Feed Now",   
                        []() { return isPlantList() || isPlantFieldSelected("lastFed"); }, 
                        (Key)'F', []() { return toAction(feedPlant()); } 
                    });
    buttons.push_back({ 1, "P: Postpone",   
                        []() { return isPlantList() || isPlantFieldSelected("lastWatered") || isPlantFieldSelected("lastFed"); }, 
                        (Key)'P', []() { return toAction(postponePlant()); } 
                    });

    return buttons;
}

Action toAction(bool result)
{
    return result ? Action::Redraw : Action::NoRedraw;
}

bool listActive()
{
    return activeSection->getType() == Section::SectionType::List;
}

bool detailsActive()
{
    return activeSection->getType() == Section::SectionType::Details;
}

bool isSettingsTab()
{
    return std::holds_alternative<std::pair<ListSection<Setting>*, DetailsSection<Setting>*>>(activeTab);
}

bool isSelectedFieldForeign()
{
    if(!detailsActive())
    {
        return false;
    }

    bool result = false;

    std::visit([&](auto& tab) {
        auto& currentList = tab.first;
        auto& currentDetails = tab.second;

        auto& record = currentList->getSelectedRecord();
        Field selectedField = record.getEditableFields()[currentDetails->getPosition()];
        result = selectedField.isForeign();
    }, activeTab);

    return result;
}

bool isPlantList()
{
    return std::holds_alternative<std::pair<ListSection<Plant>*, DetailsSection<Plant>*>>(activeTab) && listActive();
}

bool isPlantFieldSelected(const std::string& colNam)
{
    if (!std::holds_alternative<std::pair<ListSection<Plant>*, DetailsSection<Plant>*>>(activeTab) && detailsActive()) 
    {
        return false;
    }

    bool result = false;

    std::visit([&](auto& tab) {
        auto& currentList = tab.first;
        auto& currentDetails = tab.second;

        auto& record = currentList->getSelectedRecord();
        Field selectedField = record.getEditableFields()[currentDetails->getPosition()];
        if(selectedField.colNam == colNam)
        {
            result = true;
        }
    }, activeTab);

    return result;
}