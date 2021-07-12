#include "Helpers/HookInstaller.hpp"
#include "Helpers/ObjectHelper.hpp"
#include "main.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/ScriptableObject.hpp"

#include "GlobalNamespace/ConditionalMaterialSwitcher.hpp"
#include "GlobalNamespace/ConditionalActivation.hpp"
#include "GlobalNamespace/BoolSO.hpp"

using namespace GlobalNamespace;

MAKE_HOOK_MATCH(ConditionalMaterialSwitcher_Awake, &GlobalNamespace::ConditionalMaterialSwitcher::Awake, void, GlobalNamespace::ConditionalMaterialSwitcher* instance)
{
    if (IsRegexMatch(instance->get_name(), "ObstacleCore|ObstacleFrame") && getConfig().config["enablePCWalls"].GetBool())
    {
        BoolSO* use_grappass = (BoolSO*)UnityEngine::ScriptableObject::CreateInstance(csTypeOf(BoolSO*));
        use_grappass->value = true;
        instance->value = use_grappass;
    }
    ConditionalMaterialSwitcher_Awake(instance);
}

MAKE_HOOK_MATCH(ConditionalActivation_Awake, &GlobalNamespace::ConditionalActivation::Awake, void, GlobalNamespace::ConditionalActivation* instance)
{
    ConditionalActivation_Awake(instance);
    if (IsRegexMatch(instance->get_name(), "GrabPassTexture1") && getConfig().config["enablePCWalls"].GetBool())
    {
        instance->get_gameObject()->SetActive(true);
    }
}

void MRCPlus::Hooks::Install_RenderingHooks()
{
    INSTALL_HOOK(getLogger(), ConditionalMaterialSwitcher_Awake);
    INSTALL_HOOK(getLogger(), ConditionalActivation_Awake);
    getLogger().info("[MRCPlus] Installed Rendering hooks");
}

