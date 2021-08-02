#include "UI/DefaultMRCMenu.hpp"
#include "Helpers/HookInstaller.hpp"
#include "Helpers/UIHelper.hpp"
#include "main.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"

#include "GlobalNamespace/OVRPlugin.hpp"
#include "GlobalNamespace/OVRPlugin_Media.hpp"
#include "GlobalNamespace/WindowResolutionSettingsController.hpp"
#include "GlobalNamespace/SettingsNavigationController.hpp"

using namespace GlobalNamespace;

MAKE_HOOK_MATCH(WindowResSetting_InitVals, &GlobalNamespace::WindowResolutionSettingsController::GetInitValues, bool, GlobalNamespace::WindowResolutionSettingsController* instance, ByRef<int> index, ByRef<int> size)
{
    // Don't initialize our values as we'll set them manually
    if (strcmp(to_utf8(csstrtostr(instance->get_transform()->get_parent()->get_name())).c_str(), "MRCResolutionContainer") == 0) return false;
    WindowResSetting_InitVals(instance, index, size);
    return false;
}

MAKE_HOOK_MATCH(WindowResSetting_ApplyValue, &GlobalNamespace::WindowResolutionSettingsController::ApplyValue, void, GlobalNamespace::WindowResolutionSettingsController* instance, int index)
{
    WindowResSetting_ApplyValue(instance, index);
    if (strcmp(to_utf8(csstrtostr(instance->get_transform()->get_parent()->get_name())).c_str(), "MRCResolutionContainer") != 0) return;
    auto resolution = instance->windowResolutions->values[index];
    getConfig().config["width"].SetInt(resolution.get_x());
    getConfig().config["height"].SetInt(resolution.get_y());
    getConfig().Write();
    instance->RefreshUI();
    if (OVRPlugin::IsMixedRealityInitialized()) 
    {
        OVRPlugin::Media::SetMrcFrameSize(resolution.get_x(), resolution.get_y());
    }
}

MAKE_HOOK_MATCH(SettingsNavController_DidActivate, &GlobalNamespace::SettingsNavigationController::DidActivate, void, GlobalNamespace::SettingsNavigationController* instance, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
    SettingsNavController_DidActivate(instance, firstActivation, addedToHierarchy, screenSystemEnabling);

    // Find container's transform
    SettingsContainer = instance->get_transform()->Find(il2cpp_utils::newcsstr("OculusMRCSettings/SettingsContainer"));
    if (SettingsContainer == nullptr) SettingsContainer = instance->get_transform()->Find(il2cpp_utils::newcsstr("Container/OculusMRCSettings/SettingsContainer"));

    // Modifiy the base-game MRC menu
    if (firstActivation && addedToHierarchy) ModifyMRCMenu();
}

void MRCPlus::Hooks::Install_UIHooks()
{
    INSTALL_HOOK(getLogger(), SettingsNavController_DidActivate);
    INSTALL_HOOK(getLogger(), WindowResSetting_InitVals);
    INSTALL_HOOK(getLogger(), WindowResSetting_ApplyValue);
    getLogger().info("[MRCPlus] Installed UI hooks");
}

