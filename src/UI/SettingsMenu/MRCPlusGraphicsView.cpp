#include "main.hpp"
#include "Helpers/UIHelper.hpp"
#include "Helpers/ObjectHelper.hpp"
#include "UI/SharedUIManager.hpp"
#include "UI/SettingsMenu/MRCPlusGraphicsView.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/CustomTypes/Components/Settings/IncrementSetting.hpp"

#include "UnityEngine/Color.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
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
#include "TMPro/TextAlignmentOptions.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

#include "modloader/shared/modloader.hpp"
#include <vector>

using namespace MRCPlus;
DEFINE_TYPE(MRCPlus, MRCPlusGraphicsView);

MRCPlusGraphicsView* GraphicsView;

void MRCPlusGraphicsView::OnChangeVisibility(bool newval)
{
    this->gfxContainer->get_gameObject()->SetActive(newval);
}

void OnChangeShowViewfinder(bool newval)
{
    getConfig().config["showViewfinder"].SetBool(newval);
    getConfig().Write();
}

void SetWarningVisible(bool visible)
{
    TMPro::TextMeshProUGUI* warning = GraphicsView->warningText;
    if (warning)
    {
        float alpha = visible ? 1.0 : 0.0f;
        warning->set_color(warning->get_color() * UnityEngine::Color(1.0f, 1.0f, 1.0f, 0.0f));
        warning->set_color(warning->get_color() + UnityEngine::Color(0.0f, 0.0f, 0.0f, alpha));
    }
}

void OnChangeTransparentWalls(bool newval)
{
    auto& modcfg = getConfig().config;
    modcfg["enableTransparentWalls"].SetBool(newval);

    UnityEngine::UI::Toggle* pcToggle = GraphicsView->pcWallToggle;
    if (newval && pcToggle)
    {
        SetWarningVisible(false);
        modcfg["enablePCWalls"].SetBool(false);
        pcToggle->set_isOn(false);
    }
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
    SetWarningVisible(newval);
}

void OnChangeMSAA(int index)
{
    auto& modcfg = getConfig().config;

    std::vector<int> msaavals = {1, -1, 2, 4};
    int newval = msaavals[index];

    QuestUI::IncrementSetting* increment = GraphicsView->msaaIncrement;
    if (increment)
    {
        if (!GraphicsView->msaaText)
        {
            TMPro::TextMeshProUGUI* oldtext = increment->Text;
            GraphicsView->msaaText = UnityEngine::Object::Instantiate(oldtext, oldtext->get_transform()->get_parent());
            GraphicsView->msaaOffText = CreateLocalizableText("SETTINGS_OFF", oldtext->get_transform());
            GraphicsView->msaaOffText->set_alignment(TMPro::TextAlignmentOptions::Capline);
            GraphicsView->msaaOffText->get_rectTransform()->set_pivot(UnityEngine::Vector2(0.5f, 0.5f));
            oldtext->set_maxVisibleCharacters(0);
        }
        bool showlocale = newval == 1;
        bool isfxaa = newval == -1;
        GraphicsView->msaaText->set_text(StringW(isfxaa ? "FXAA*" : std::to_string(newval) + "x MSAA"));
        GraphicsView->msaaOffText->set_maxVisibleCharacters(showlocale ? 12 : 0);
        GraphicsView->msaaText->set_maxVisibleCharacters(showlocale ? 0 : 12);
    }
    modcfg["antiAliasing"].SetInt(newval);

}

void MRCPlusGraphicsView::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
    GraphicsView = this;
    if (firstActivation && addedToHierarchy)
    {
        auto& modcfg = getConfig().config;
        bool enablePCWalls = modcfg["enablePCWalls"].GetBool();
        bool enableInvisWalls = modcfg["enableTransparentWalls"].GetBool();
        bool showViewfinder = modcfg["showViewfinder"].GetBool();
        int antiAliasing = modcfg["antiAliasing"].GetInt();

        this->gfxContainer = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(get_rectTransform());
        gfxContainer->set_spacing(0.2f);
        gfxContainer->set_childControlHeight(true);
        gfxContainer->GetComponent<UnityEngine::UI::LayoutElement*>()->set_minWidth(25.0);

        // Title
        auto* titlecontainer = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(gfxContainer->get_rectTransform());
        titlecontainer->get_gameObject()->AddComponent<QuestUI::Backgroundable*>()->ApplyBackground(StringW("panel-top"));
        titlecontainer->set_padding(UnityEngine::RectOffset::New_ctor(10, 10, 0, 0));
        titlecontainer->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(2);

        auto* titletext = CreateLocalizableText("MENU_GRAPHICS", titlecontainer->get_rectTransform(), false);
        titletext->set_alignment(TMPro::TextAlignmentOptions::Center);
        titletext->set_fontSize(10);

        // Config Panel
        UnityEngine::UI::VerticalLayoutGroup* subcontainer = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(gfxContainer->get_rectTransform());
        subcontainer->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(2);

        UnityEngine::UI::Toggle* viewfinderToggle = QuestUI::BeatSaberUI::CreateToggle(subcontainer->get_rectTransform(), "Show Viewfinder", showViewfinder, UnityEngine::Vector2(0, 0), OnChangeShowViewfinder);

        this->transparentWallToggle = QuestUI::BeatSaberUI::CreateToggle(subcontainer->get_rectTransform(), "Transparent Walls", enableInvisWalls, UnityEngine::Vector2(0, 0), OnChangeTransparentWalls);
        QuestUI::BeatSaberUI::AddHoverHint(transparentWallToggle->get_gameObject(), "Use transparent walls in the output.");
        if (!IsEnglish()) LocalizeComponent(transparentWallToggle, "MODIFIER_NO_OBSTACLES");

        bool hasDistortions = Modloader::getMods().contains("Distortions");
        // if (!hasDistortions)
        // {
        //     this->pcWallToggle = QuestUI::BeatSaberUI::CreateToggle(subcontainer->get_rectTransform(), "PC Walls", enablePCWalls, UnityEngine::Vector2(0, 0), OnChangePCWalls);
        //     QuestUI::BeatSaberUI::AddHoverHint(pcWallToggle->get_gameObject(), IsHardwareCapable() ? "Requires the Distortions mod." : "Your headset does not support this setting.");
        //     if (!IsEnglish()) LocalizeComponent(pcWallToggle, "SETTINGS_SCREEN_DISTORTION_EFFECTS");
        //     this->pcWallToggle->set_interactable(enablePCWalls);
        // }

        int msaa_initval = (int)std::ceil(pow((float)antiAliasing - 1, 0.5f));
        if (antiAliasing < 1) msaa_initval = 1;
        if (antiAliasing > 1) msaa_initval += 1;
        

        msaaIncrement = QuestUI::BeatSaberUI::CreateIncrementSetting(subcontainer->get_rectTransform(), "", 0, 1, msaa_initval, 0, 3, OnChangeMSAA);
        msaaIncrement->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredHeight(6.2f);
        QuestUI::BeatSaberUI::AddHoverHint(msaaIncrement->get_gameObject(), "Smoothens rough edges.\nHigher values cost more performance.");
        LocalizeComponent(msaaIncrement, "SETTINGS_ANTIALIASING_MSAA");
        auto aanewstr = msaaIncrement->GetComponentsInChildren<TMPro::TextMeshProUGUI*>()[1]->get_text()->Replace(StringW(u"(MSAA)"), StringW(u""));
        msaaIncrement->GetComponentsInChildren<TMPro::TextMeshProUGUI*>()[1]->SetText(aanewstr);
        OnChangeMSAA(msaa_initval);


        if (!IsEnglish()) {
            this->warningText = CreateLocalizableText("PROMO_BANNER_RECOMMENDED_LABEL", subcontainer->get_rectTransform());
            this->warningText->SetText(this->warningText->get_text()->Insert(0, StringW("* ")));
            this->warningText->set_alignment(TMPro::TextAlignmentOptions::Left);
        }
        else this->warningText = QuestUI::BeatSaberUI::CreateText(subcontainer->get_rectTransform(), "* Recommended for your system");
        warningText->set_color(UnityEngine::Color(0.12549f, 0.75294f, 1.0f, enablePCWalls ? 1.0f : 1.0f));

        // this->warningText = QuestUI::BeatSaberUI::CreateText(subcontainer->get_rectTransform(), "NOTE: This setting may impact battery usage.");
        // warningText->set_color(UnityEngine::Color(0.12549f, 0.75294f, 1.0f, enablePCWalls ? 1.0f : 0.0f));
        // if (!IsHardwareCapable())
        // {
        //     warningText->SetText(StringW("WARNING: This causes visual issues on Quest 1!"));
        //     warningText->set_color(UnityEngine::Color(0.8471f, 0.0588f, 0.0588f, enablePCWalls ? 1.0f : 0.0f));
        // }

    }
}

void MRCPlusGraphicsView::DidDeactivate(bool removedFromHierarchy, bool systemScreenDisabling)
{
    getConfig().Write();
}