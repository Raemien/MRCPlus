#include "Helpers/HookInstaller.hpp"
#include "Helpers/ObjectHelper.hpp"
#include "Helpers/UIHelper.hpp"
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
#include "GlobalNamespace/BloomPrePass.hpp"

#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/PrimitiveType.hpp"

using namespace GlobalNamespace;

bool postGrabPassFix = false;

MAKE_HOOK_MATCH(ConditionalMaterialSwitcher_Awake, &GlobalNamespace::ConditionalMaterialSwitcher::Awake, void, GlobalNamespace::ConditionalMaterialSwitcher* instance)
{
    if (IsRegexMatch(instance->get_name(), "ObstacleCore|ObstacleFrame") && getConfig().config["enablePCWalls"].GetBool())
    {
        // Swap to HD GrabPass material
        BoolSO* use_grappass = (BoolSO*)UnityEngine::ScriptableObject::CreateInstance(csTypeOf(BoolSO*));
        use_grappass->value = true;
        instance->value = use_grappass;

        // Prevent right-eye distortion flickering in Quest 1 (not offically supported)
        if (!postGrabPassFix && !IsHardwareCapable())
        {
            UnityEngine::GameObject* stereoFix = UnityEngine::GameObject::CreatePrimitive(UnityEngine::PrimitiveType::Cube);
            stereoFix->set_name(il2cpp_utils::newcsstr("Quest1GrabPassFix"));
            stereoFix->get_transform()->set_localScale(UnityEngine::Vector3(0.001f, 0.001f, 0.001f));
            stereoFix->get_transform()->GetComponent<UnityEngine::MeshRenderer*>()->SetMaterial(instance->material1);
            UnityEngine::Object::DontDestroyOnLoad(stereoFix);
            postGrabPassFix = true;
        }
        
        // // Render LW placeholder in headset (MAKE SURE THIS IS ACTUALLY PERFORMANT, ESPECIALLY IN MOD CHARTS)
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
        UnityEngine::QualitySettings::set_antiAliasing(1);
    }
}

void MRCPlus::Hooks::Install_RenderingHooks()
{
    INSTALL_HOOK(getLogger(), ConditionalMaterialSwitcher_Awake);
    INSTALL_HOOK(getLogger(), ConditionalActivation_Awake);
    getLogger().info("[MRCPlus] Installed Rendering hooks");
}
