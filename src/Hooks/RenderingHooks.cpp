#include "Helpers/HookInstaller.hpp"
#include "Helpers/ObjectHelper.hpp"
#include "main.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/QualitySettings.hpp"
#include "UnityEngine/ScriptableObject.hpp"

#include "GlobalNamespace/ConditionalMaterialSwitcher.hpp"
#include "GlobalNamespace/ConditionalActivation.hpp"
#include "GlobalNamespace/BoolSO.hpp"

using namespace GlobalNamespace;

MAKE_HOOK_MATCH(ConditionalMaterialSwitcher_Awake, &GlobalNamespace::ConditionalMaterialSwitcher::Awake, void, GlobalNamespace::ConditionalMaterialSwitcher* instance)
{
    if (IsRegexMatch(instance->get_name(), "ObstacleCore|ObstacleFrame") && getConfig().config["enablePCWalls"].GetBool())
    {
        // Swap to HD GrabPass material (also culls walls in HMD to prevent visual + perf issues)
        BoolSO* use_grappass = (BoolSO*)UnityEngine::ScriptableObject::CreateInstance(csTypeOf(BoolSO*));
        use_grappass->value = true;
        instance->value = use_grappass;
        // instance->get_gameObject()->set_layer(0);

        // // Render LW placeholder in headset
        // if (instance->get_transform()->get_childCount() == 2)
        // {
        //     UnityEngine::Transform* lqWall = instance->get_transform()->GetChild(0);
        //     lqWall->GetComponent<UnityEngine::Renderer*>()->set_enabled(true);
        //     lqWall->GetComponent<UnityEngine::Renderer*>()->set_sortingOrder(3636);
        //     lqWall->get_gameObject()->SetActive(true);
        //     lqWall->get_gameObject()->set_layer(0);
        // }
    }
    ConditionalMaterialSwitcher_Awake(instance);
}

MAKE_HOOK_MATCH(ConditionalActivation_Awake, &GlobalNamespace::ConditionalActivation::Awake, void, GlobalNamespace::ConditionalActivation* instance)
{
    ConditionalActivation_Awake(instance);
    if (IsRegexMatch(instance->get_name(), "GrabPassTexture1") && getConfig().config["enablePCWalls"].GetBool())
    {
        instance->get_gameObject()->SetActive(true);
        UnityEngine::QualitySettings::set_antiAliasing(0);
    }
}

void MRCPlus::Hooks::Install_RenderingHooks()
{
    INSTALL_HOOK(getLogger(), ConditionalMaterialSwitcher_Awake);
    INSTALL_HOOK(getLogger(), ConditionalActivation_Awake);
    getLogger().info("[MRCPlus] Installed Rendering hooks");
}
