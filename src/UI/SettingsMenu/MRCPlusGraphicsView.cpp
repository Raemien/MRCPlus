#include "main.hpp"
#include "Helpers/UIHelper.hpp"
#include "UI/SharedUIManager.hpp"
#include "UI/SettingsMenu/MRCPlusGraphicsView.hpp"

#include <string>

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/CustomTypes/Components/Settings/IncrementSetting.hpp"

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
DEFINE_TYPE(MRCPlus, MRCPlusGraphicsView);

MRCPlusGraphicsView* GraphicsView;

void MRCPlusGraphicsView::OnChangeVisibility(bool newval)
{
    this->gfxContainer->get_gameObject()->SetActive(newval);
}

void OnChangePCWalls(bool newval)
{
    auto& modcfg = getConfig().config;
    modcfg["enablePCWalls"].SetBool(newval);

    UnityEngine::UI::Toggle* transparentToggle = GraphicsView->transparentWallToggle;
    if (newval && transparentToggle)
    {
        modcfg["enableTransparentWalls"].SetBool(false);
        transparentToggle->set_isOn(false);
    }
}

void OnChangeTransparentWalls(bool newval)
{
    auto& modcfg = getConfig().config;
    modcfg["enablePCWalls"].SetBool(newval);

    UnityEngine::UI::Toggle* pcToggle = GraphicsView->pcWallToggle;
    if (newval && pcToggle)
    {
        modcfg["enablePCWalls"].SetBool(false);
        pcToggle->set_isOn(false);
    }
}

void MRCPlusGraphicsView::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
    GraphicsView = this;
    if (firstActivation && addedToHierarchy)
    {
        auto& modcfg = getConfig().config;
        bool enablePCWalls = modcfg["enablePCWalls"].GetBool();
        bool enableInvisWalls = modcfg["enableTransparentWalls"].GetBool();

        this->gfxContainer = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(get_rectTransform());
        gfxContainer->set_spacing(0.2f);
        gfxContainer->GetComponent<UnityEngine::UI::LayoutElement*>()->set_minWidth(25.0);

        // Title
        auto* titlecontainer = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(gfxContainer->get_rectTransform());
        titlecontainer->get_gameObject()->AddComponent<QuestUI::Backgroundable*>()->ApplyBackground(il2cpp_utils::createcsstr("panel-top"));
        titlecontainer->set_padding(UnityEngine::RectOffset::New_ctor(10, 10, 0, 0));
        titlecontainer->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(2);

        auto* titletext = QuestUI::BeatSaberUI::CreateText(titlecontainer->get_rectTransform(), GetLocale("MENU_GRAPHICS"), false);
        titletext->set_alignment(TMPro::TextAlignmentOptions::Center);
        titletext->set_fontSize(10);

        // Config Panel
        UnityEngine::UI::VerticalLayoutGroup* subcontainer = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(gfxContainer->get_rectTransform());
        subcontainer->set_childAlignment(UnityEngine::TextAnchor::UpperCenter);
        subcontainer->set_childForceExpandHeight(false);
        subcontainer->set_childControlHeight(true);

        this->pcWallToggle = QuestUI::BeatSaberUI::CreateToggle(subcontainer->get_rectTransform(), IsEnglish() ? "PC Walls" : GetLocale("SETTINGS_SCREEN_DISTORTION_EFFECTS"), enablePCWalls, UnityEngine::Vector2(0, 0), OnChangePCWalls);
        this->transparentWallToggle = QuestUI::BeatSaberUI::CreateToggle(subcontainer->get_rectTransform(), IsEnglish() ? "Transparent Walls" : GetLocale("MODIFIER_NO_OBSTACLES"), enableInvisWalls, UnityEngine::Vector2(0, 0), OnChangeTransparentWalls);
        // this->ReloadUIValues();
    }
}

void MRCPlusGraphicsView::DidDeactivate(bool removedFromHierarchy, bool systemScreenDisabling)
{
    getConfig().Write();
}