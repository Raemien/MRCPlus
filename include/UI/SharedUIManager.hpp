#include "modloader/shared/modloader.hpp"

#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "HMUI/FlowCoordinator.hpp"

extern HMUI::FlowCoordinator* MRCPlusSettingsFC;
extern void ReloadAllElements();
extern void ApplyPanelVisibility(bool leftVisible, bool rightVisible);