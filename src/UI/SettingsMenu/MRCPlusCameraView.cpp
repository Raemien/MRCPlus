#include "UI/SettingsMenu/MRCPlusCameraView.hpp"
#include "Helpers/ObjectHelper.hpp"
#include "Helpers/UIHelper.hpp"
#include "main.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/CustomTypes/Components/Settings/IncrementSetting.hpp"

#include <map>

#include "UnityEngine/Object.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "HMUI/ViewController.hpp"
#include "UnityEngine/UI/Toggle.hpp"
#include "UnityEngine/UI/Selectable.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "UnityEngine/Events/UnityAction.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

using namespace MRCPlus;
DEFINE_TYPE(MRCPlus, MRCPlusCameraView);

MRCPlusCameraView* CameraView;

void MRCPlusCameraView::OnChangeVisibility(bool newval)
{
    CameraView->camContainer->get_gameObject()->SetActive(newval);
}

void OnChangeShowViewfinder(bool newval)
{
    getConfig().config["showViewfinder"].SetBool(newval);
    getConfig().Write();
    ApplyViewfinderVisibility(newval);
}

void OnChangeUseHotkey(bool newval)
{
    getConfig().config["useCameraHotkey"].SetBool(newval);
    getConfig().Write();
}

void MRCPlusCameraView::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
    CameraView = this;
    if (firstActivation && addedToHierarchy) 
    {
        auto& modcfg = getConfig().config;
        std::string cameraMode = modcfg["cameraMode"].GetString();
        float smoothpos = modcfg["positionSmoothness"].GetFloat();
        bool useCameraHotkey = modcfg["useCameraHotkey"].GetBool();
        bool showViewfinder = modcfg["showViewfinder"].GetBool();
        int height = modcfg["height"].GetInt();
        int width = modcfg["width"].GetInt();
        int userfov = modcfg["fov"].GetInt();

        this->camContainer = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(get_rectTransform());
        camContainer->set_spacing(0.2f);
        camContainer->set_childControlHeight(true);
        camContainer->GetComponent<UnityEngine::UI::LayoutElement*>()->set_minWidth(25.0);

        // Title
        auto* titlecontainer = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(camContainer->get_rectTransform());
        titlecontainer->get_gameObject()->AddComponent<QuestUI::Backgroundable*>()->ApplyBackground(il2cpp_utils::createcsstr("panel-top"));
        titlecontainer->set_padding(UnityEngine::RectOffset::New_ctor(10, 10, 0, 0));
        titlecontainer->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(2);

        auto* titletext = CreateLocalizableText("MENU_CAMERA", titlecontainer->get_rectTransform(), false);
        titletext->set_alignment(TMPro::TextAlignmentOptions::Center);
        titletext->set_fontSize(10);

        UnityEngine::UI::VerticalLayoutGroup* subContainer = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(camContainer->get_rectTransform());
        subContainer->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(2);

        QuestUI::IncrementSetting* fovInc = QuestUI::BeatSaberUI::CreateIncrementSetting(subContainer->get_rectTransform(), GetLocale("SETTINGS_FIELD_OF_VIEW"), 0, (int)5, userfov, 70, 120, OnChangeFov);
        fovInc->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredHeight(6.2f);

        QuestUI::IncrementSetting* smoothposInc = QuestUI::BeatSaberUI::CreateIncrementSetting(subContainer->get_rectTransform(), GetLocale("SETTINGS_SMOOTHNESS"), 0, (int)1, smoothpos, 0, 10, OnChangeSmoothing);
        smoothposInc->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredHeight(6.2f);

        UnityEngine::UI::Toggle* hotkeyToggle = QuestUI::BeatSaberUI::CreateToggle(subContainer->get_rectTransform(), "Use Camera Hotkey", useCameraHotkey, UnityEngine::Vector2(0, 0), OnChangeUseHotkey);
        QuestUI::BeatSaberUI::AddHoverHint(hotkeyToggle->get_gameObject(), "Switch the camera perspective using the 'X' button.");

        UnityEngine::UI::Toggle* viewfinderToggle = QuestUI::BeatSaberUI::CreateToggle(subContainer->get_rectTransform(), "Show Viewfinder", showViewfinder, UnityEngine::Vector2(0, 0), OnChangeShowViewfinder);
    }
}

void MRCPlusCameraView::DidDeactivate(bool removedFromHierarchy, bool systemScreenDisabling)
{
    getConfig().Write();
}