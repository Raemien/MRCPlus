#include "UI/SharedUIManager.hpp"
#include "UI/SettingsMenu/MRCPlusMainView.hpp"
#include "Helpers/ObjectHelper.hpp"
#include "Helpers/UIHelper.hpp"
#include "MRCConfig.hpp"
#include "main.hpp"

#include <dirent.h>
#include <regex>
#include <list>

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/CustomTypes/Components/ExternalComponents.hpp"
#include "GlobalNamespace/WindowResolutionSettingsController.hpp"

#include "UnityEngine/Color.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Vector2Int.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "UnityEngine/Events/UnityAction.hpp"
#include "TMPro/TextAlignmentOptions.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

using namespace MRCPlus;
DEFINE_TYPE(MRCPlus, MRCPlusMainView);

MRCPlusMainView* MainView;

void OnChangeCameraMode(std::string newval)
{
    getConfig().config["cameraMode"].SetString(newval, getConfig().config.GetAllocator());
    getConfig().Write();
    float width = (float)getConfig().config["width"].GetInt();
    float height = (float)getConfig().config["height"].GetInt();
    bool mixedReality = (newval == "Mixed Reality");
    bool disabled = (newval == "Disabled");

    GlobalNamespace::BoolSO* boolsetting = GetMRCBoolSO();
    boolsetting->set_value(!disabled);

    bool camViewVisible = true;
    if (mixedReality || disabled)
    {
        GlobalNamespace::OVRPlugin::OverrideExternalCameraFov(0, true, mrcInfo.FOVPort);
        camViewVisible = false;
    }
    else SetAsymmetricFOV(width, height);
    
    ApplyPanelVisibility(camViewVisible, !disabled);
}

void MRCPlusMainView::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
    MainView = this;
    if (firstActivation && addedToHierarchy) 
    {
        auto& modcfg = getConfig().config;
        std::string cameraMode = modcfg["cameraMode"].GetString();
        int height = modcfg["height"].GetInt();
        int width = modcfg["width"].GetInt();

        UnityEngine::UI::VerticalLayoutGroup* container = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(get_rectTransform());
        container->set_padding(UnityEngine::RectOffset::New_ctor(2, 2, 2, 2));
        container->set_childControlHeight(false);
        container->set_childForceExpandHeight(true);

        this->configcontainer = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(container->get_rectTransform());
        configcontainer->set_padding(UnityEngine::RectOffset::New_ctor(2, 2, 2, 2));
        configcontainer->set_childAlignment(UnityEngine::TextAnchor::MiddleLeft);
        configcontainer->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(2);
        configcontainer->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(2);
        configcontainer->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredWidth(110);

        HMUI::SimpleTextDropdown* mrcModeDropdown = QuestUI::BeatSaberUI::CreateDropdown(configcontainer->get_transform(), "Camera Mode", cameraMode, ModeValues,
        [](std::string newval) {OnChangeCameraMode(newval);});

        std::string infostr = "MRC is a tool designed to stream gameplay to another device. Originally intended for green-screen setups, MRCPlus repurposes this feature for streaming/recording normal footage.\n\nFor more information, visit this mod's GitHub wiki.";
        TMPro::TextMeshProUGUI* infoText = QuestUI::BeatSaberUI::CreateText(configcontainer->get_transform(), infostr);
        infoText->set_enableWordWrapping(true);
        infoText->set_alignment(TMPro::TextAlignmentOptions::Center);
        infoText->set_fontSize(4);

        UnityEngine::UI::VerticalLayoutGroup* rescontainer = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(configcontainer->get_rectTransform());
        rescontainer->set_name(il2cpp_utils::newcsstr("MRCResolutionContainer"));
        rescontainer->set_childAlignment(UnityEngine::TextAnchor::UpperCenter);
        rescontainer->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredWidth(110);
        
        auto* resreference = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::WindowResolutionSettingsController*>()->values[0];
        auto* resolutionsetting = UnityEngine::Object::Instantiate(resreference, rescontainer->get_transform(), false);
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

        TMPro::TextMeshProUGUI* warningText = CreateLocalizableText("SETTINGS_OCULUS_MRC_WARNING", configcontainer->get_transform());
        warningText->set_enableWordWrapping(true);
        warningText->set_alignment(TMPro::TextAlignmentOptions::Center);
        warningText->set_color(UnityEngine::Color(0.12549f, 0.75294f, 1.0f, 1.0f));
        warningText->set_fontSize(4);
    }
}

void MRCPlusMainView::DidDeactivate(bool removedFromHierarchy, bool systemScreenDisabling)
{
    getConfig().Write();
}