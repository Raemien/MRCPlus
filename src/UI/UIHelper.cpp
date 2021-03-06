#include "main.hpp"
#include "MRCConfig.hpp"
#include "UI/UIHelper.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "GlobalNamespace/BoolSettingsController.hpp"
#include "GlobalNamespace/OVRPlugin.hpp"
#include "GlobalNamespace/BoolSO.hpp"
#include "UnityEngine/Vector2Int.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "Polyglot/Localization.hpp"

std::vector<std::string> ModeValues {"Disabled", "Mixed Reality", "First Person", "Third Person"};
UnityEngine::Transform* SettingsContainer = nullptr;

void OnChangeFov(UnityEngine::GameObject* instance, float newval)
{
    auto& modcfg = getConfig().config;
    float width = (float)modcfg["width"].GetInt();
    float height = (float)modcfg["height"].GetInt();
    modcfg["fov"].SetInt(newval);
    SetAsymmetricFOV(width, height);
}

void OnChangeSmoothing(UnityEngine::GameObject* instance, float newval)
{
    getConfig().config["positionSmoothness"].SetFloat(newval);
    getConfig().config["rotationSmoothness"].SetFloat(newval);
}

void OnChangeCameraMode(std::string newval)
{
    getConfig().config["cameraMode"].SetString(newval, getConfig().config.GetAllocator());
    getConfig().Write();
    float width = (float)getConfig().config["width"].GetInt();
    float height = (float)getConfig().config["height"].GetInt();
    bool mixedReality = (newval == "Mixed Reality");
    bool disabled = (newval == "Disabled");
    if (!SettingsContainer) {
        SettingsContainer = UnityEngine::GameObject::Find(il2cpp_utils::createcsstr("OculusMRCSettings/SettingsContainer"))->get_transform();
    }
    auto* boolsetting = SettingsContainer->GetComponentInChildren<GlobalNamespace::BoolSettingsController*>();

    SettingsContainer->GetComponentInChildren<GlobalNamespace::BoolSettingsController*>()->settingsValue->value = !disabled;
    SettingsContainer->GetComponentInChildren<GlobalNamespace::BoolSettingsController*>()->settingsValue->set_value(!disabled);

    UnityEngine::Vector3 scale = UnityEngine::Vector3(0.9, 0.9, 0.9);
    if (mixedReality || disabled)
    {
        GlobalNamespace::OVRPlugin::OverrideExternalCameraFov(0, true, mrcInfo.FOVPort);
        scale = UnityEngine::Vector3::get_zero();
    }
    else SetAsymmetricFOV(width, height);
    
    auto* subcontainer = SettingsContainer->Find(il2cpp_utils::createcsstr("NonMRSubContainer"));
    subcontainer->set_localScale(scale);
}

void SetWarningText(WarningText txtnum)
{
    TMPro::TextMeshProUGUI* warntext = SettingsContainer->Find(il2cpp_utils::createcsstr("WarningText"))->GetComponent<TMPro::TextMeshProUGUI*>();
    std::string message;
    switch (txtnum)
    {
    case WarningText::PerfWarning:
        message = "WARNING: These settings will impact your performance.";
        break;
    case WarningText::RestartGame:
        message = "NOTE: Restart your game to apply these settings.";
        break;
    case WarningText::RestartOBS:
        message = "NOTE: Reconnect via OBS to apply these settings.";
        break;
    default:
        message = "For more information on how to set up MRC, please visit Oculus's setup guide.";
        break;
    }
    warntext->SetText(il2cpp_utils::createcsstr(message));
}

std::string GetLocale(std::string key)
{
    Il2CppString* localestr = Polyglot::Localization::Get(il2cpp_utils::createcsstr(key));
    return to_utf8(csstrtostr(localestr));
}

Array<UnityEngine::Vector2Int>* GetMRCResolutions()
{
    using namespace UnityEngine;
    std::vector<Vector2Int> _mrcResolutions = {Vector2Int(640, 360), Vector2Int(854, 480), Vector2Int(1280, 720), Vector2Int(1366, 768)};

    auto* refreshrates = GlobalNamespace::OVRPlugin::get_systemDisplayFrequenciesAvailable();
    if (refreshrates->values[refreshrates->Length() - 1] >= 90.0f)
    {
        _mrcResolutions.push_back(Vector2Int(1600, 900));
        _mrcResolutions.push_back(Vector2Int(1920, 1080));
    }
    return il2cpp_utils::vectorToArray(_mrcResolutions);
}