#include "Helpers/UIHelper.hpp"
#include "MRCConfig.hpp"
#include "main.hpp"

#include "modloader/shared/modloader.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp" 
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "questui/shared/CustomTypes/Components/Settings/IncrementSetting.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Events/UnityAction.hpp"
#include "UnityEngine/Events/UnityAction_1.hpp"

#include "UnityEngine/UI/Toggle.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "HMUI/SimpleTextDropdown.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/Transform.hpp"

#include "TMPro/TextMeshProUGUI.hpp"
#include "GlobalNamespace/WindowResolutionSettingsController.hpp"
#include "GlobalNamespace/SettingsNavigationController.hpp"

void ModifyMRCMenu()
{
    auto& modcfg = getConfig().config;
    std::string newinfotext = "Mixed Reality Capture sends an alternate view of your game to OBS Studio, usually for green-screen setups.\n\nThe MRC+ mod repurposes this feature as a recording/streaming tool.";
    std::string cameraMode = modcfg["cameraMode"].GetString();
    float smoothpos = modcfg["positionSmoothness"].GetFloat();
    int height = modcfg["height"].GetInt();
    int width = modcfg["width"].GetInt();
    int userfov = modcfg["fov"].GetInt();

    // Modify existing text
    UnityEngine::GameObject* infotext = SettingsContainer->Find(il2cpp_utils::newcsstr("InfoText"))->get_gameObject();
    infotext->GetComponent<TMPro::TextMeshProUGUI*>()->SetText(il2cpp_utils::newcsstr(newinfotext));
    UnityEngine::GameObject* warningtext = SettingsContainer->Find(il2cpp_utils::newcsstr("WarningText"))->get_gameObject();
    SetWarningText(WarningText::Default);

    // Global settings
    UnityEngine::Transform* mrcToggle = SettingsContainer->Find(il2cpp_utils::newcsstr("MixedRealityCaptureEnabled"));
    mrcToggle->GetComponent<UnityEngine::UI::LayoutElement*>()->set_ignoreLayout(true);
    mrcToggle->set_localScale(UnityEngine::Vector3::get_zero());
    
    HMUI::SimpleTextDropdown* mrcModeDropdown = QuestUI::BeatSaberUI::CreateDropdown(SettingsContainer->get_transform(), "Camera Mode", cameraMode, ModeValues,
    [](std::string newval) {OnChangeCameraMode_Deprecated(newval);});

    auto* resreference = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::WindowResolutionSettingsController*>()->values[0];
    auto* resolutionsetting = UnityEngine::Object::Instantiate(resreference, SettingsContainer);
    resolutionsetting->windowResolutions = GetMRCResolutions();
    resolutionsetting->numberOfElements = resolutionsetting->windowResolutions->Length();
    for (size_t i = 0; i < resolutionsetting->numberOfElements; i++)
    {
        auto* resolutions = resolutionsetting->windowResolutions;
        if (resolutions->values[i].GetHashCode() == UnityEngine::Vector2Int(width, height).GetHashCode()) {
            resolutionsetting->idx = i;
            resolutionsetting->ApplyValue(i);
        }
    }
    resolutionsetting->set_text(il2cpp_utils::newcsstr(std::to_string(width) + " x " + std::to_string(height)));

    // Non-MR settings
    UnityEngine::UI::VerticalLayoutGroup* subContainer = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(SettingsContainer->get_transform());
    subContainer->get_gameObject()->set_name(il2cpp_utils::newcsstr("NonMRSubContainer"));
    
    QuestUI::IncrementSetting* fovinc = QuestUI::BeatSaberUI::CreateIncrementSetting(subContainer->get_rectTransform(), GetLocale("SETTINGS_FIELD_OF_VIEW"), 0, (int)5, userfov, 70, 120, OnChangeFov);
    fovinc->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredHeight(6.2f);

    QuestUI::IncrementSetting* smoothposinc = QuestUI::BeatSaberUI::CreateIncrementSetting(subContainer->get_rectTransform(), GetLocale("SETTINGS_SMOOTHNESS"), 0, (int)1, smoothpos, 0, 10, OnChangeSmoothing);
    smoothposinc->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredHeight(6.2f);

    infotext->get_transform()->SetAsLastSibling();
    warningtext->get_transform()->SetAsLastSibling();
    subContainer->get_transform()->set_localScale(MRCPlusEnabled() ? UnityEngine::Vector3(0.9, 0.9, 0.9) : UnityEngine::Vector3::get_zero());
}