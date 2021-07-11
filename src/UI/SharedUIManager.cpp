#include "main.hpp"
#include "UI/SettingsMenu/MRCPlusFlowCoordinator.hpp"
#include "HMUI/FlowCoordinator.hpp"

using namespace MRCPlus;

HMUI::FlowCoordinator* MRCPlusSettingsFC;

// void ReloadAllElements()
// {
//     if (MRCPlusFC != nullptr)
//     {
//         MRCPlusFlowCoordinator* targetFC = (MRCPlusFlowCoordinator*)MRCPlusFC;
//         targetFC->ReloadUIElements();
//     }
// }

void ApplyPanelVisibility(bool leftVisible, bool rightVisible)
{
    if (MRCPlusSettingsFC != nullptr)
    {
        MRCPlusFlowCoordinator* targetFC = (MRCPlusFlowCoordinator*)MRCPlusSettingsFC;
        targetFC->UpdateSettingsVisibility(leftVisible, rightVisible);
    }
}