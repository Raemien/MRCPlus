#include "main.hpp"
#include "MRCConfig.hpp"
#include "Helpers/ObjectHelper.hpp"
#include "Helpers/UIHelper.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "GlobalNamespace/NamedIntListController.hpp"
#include "GlobalNamespace/OVRPlugin.hpp"
#include "GlobalNamespace/BoolSO.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Component.hpp"
#include "UnityEngine/Vector2Int.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "TMPro/TMP_FontAsset.hpp"
#include "TMPro/FontStyles.hpp"
#include "Polyglot/Localization.hpp"
#include "Polyglot/LocalizedTextMeshProUGUI.hpp"
#include "Polyglot/Language.hpp"

std::vector<std::string> ModeValues {"Disabled", "Mixed Reality", "First Person", "Third Person"};
UnityEngine::Transform* SettingsContainer = nullptr;

void OnChangeFov(float newval)
{
    auto& modcfg = getConfig().config;
    float width = (float)modcfg["width"].GetInt();
    float height = (float)modcfg["height"].GetInt();
    modcfg["fov"].SetInt(newval);
    getConfig().Write();
    SetAsymmetricFOV(width, height);
}

void OnChangeSmoothing(float newval)
{
    getConfig().config["positionSmoothness"].SetFloat(newval);
    getConfig().config["rotationSmoothness"].SetFloat(newval);
    getConfig().Write();
}

void OnChangeCameraMode_Deprecated(std::string newval)
{
    getConfig().config["cameraMode"].SetString(newval, getConfig().config.GetAllocator());
    getConfig().Write();
    float width = (float)getConfig().config["width"].GetInt();
    float height = (float)getConfig().config["height"].GetInt();
    bool mixedReality = (newval == "Mixed Reality");
    bool disabled = (newval == "Disabled");
    if (!SettingsContainer) {
        SettingsContainer = UnityEngine::GameObject::Find(il2cpp_utils::newcsstr("OculusMRCSettings/SettingsContainer"))->get_transform();
    }

    GlobalNamespace::BoolSO* boolsetting = GetMRCBoolSO();
    boolsetting->set_value(!disabled);

    UnityEngine::Vector3 scale = UnityEngine::Vector3(0.9, 0.9, 0.9);
    if (mixedReality || disabled)
    {
        GlobalNamespace::OVRPlugin::OverrideExternalCameraFov(0, true, mrcInfo.FOVPort);
        scale = UnityEngine::Vector3::get_zero();
    }
    else SetAsymmetricFOV(width, height);
    
    auto* subcontainer = SettingsContainer->Find(il2cpp_utils::newcsstr("NonMRSubContainer"));
    subcontainer->set_localScale(scale);
}

void SetWarningText(WarningText txtnum)
{
    TMPro::TextMeshProUGUI* warntext = SettingsContainer->Find(il2cpp_utils::newcsstr("WarningText"))->GetComponent<TMPro::TextMeshProUGUI*>();
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
    warntext->SetText(il2cpp_utils::newcsstr(message));
}

// void LocalizeComponent()
// {

// }

TMPro::TextMeshProUGUI* CreateLocalizableText(std::string key, UnityEngine::Transform* parent, bool italics)
{
    Polyglot::LocalizedTextMeshProUGUI* polyglotTextObj = (Polyglot::LocalizedTextMeshProUGUI*)UnityEngine::Resources::FindObjectsOfTypeAll<Polyglot::LocalizedTextMeshProUGUI*>()->values[0];
    TMPro::TextMeshProUGUI* localizedTMPro = polyglotTextObj->localizedComponent;    

    TMPro::TextMeshProUGUI* newTextObj = UnityEngine::Object::Instantiate(localizedTMPro, parent->get_transform(), false);
    UnityEngine::Object::Destroy(newTextObj->GetComponent<Polyglot::LocalizedTextMeshProUGUI*>());
    Il2CppString* localestr = Polyglot::Localization::Get(il2cpp_utils::newcsstr(key));
    newTextObj->get_rectTransform()->set_sizeDelta(UnityEngine::Vector2(60.0f, 10.0f));
    newTextObj->set_enableAutoSizing(false);
    newTextObj->set_fontStyle(italics ? TMPro::FontStyles::Italic : TMPro::FontStyles::Normal);
    newTextObj->set_text(localestr);

    return newTextObj;
}

std::string GetLocale(std::string key)
{
    Il2CppString* localestr = Polyglot::Localization::Get(il2cpp_utils::newcsstr(key));
    return to_utf8(csstrtostr(localestr));
}

bool IsEnglish()
{
    return (Polyglot::Localization::get_Instance()->selectedLanguage == Polyglot::Language::English);
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