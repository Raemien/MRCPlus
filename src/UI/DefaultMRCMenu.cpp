#include "Helpers/UIHelper.hpp"
#include "MRCConfig.hpp"
#include "main.hpp"

#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "UnityEngine/UI/Toggle.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/Transform.hpp"

#include "TMPro/TextMeshProUGUI.hpp"

void ModifyMRCMenu()
{
    UnityEngine::GameObject* infotext = SettingsContainer->Find(il2cpp_utils::newcsstr("InfoText"))->get_gameObject();
    infotext->SetActive(false);
    UnityEngine::GameObject* warningtext = SettingsContainer->Find(il2cpp_utils::newcsstr("WarningText"))->get_gameObject();
    warningtext->SetActive(false);

    auto* titlecontainer = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(SettingsContainer->get_transform());
    titlecontainer->get_gameObject()->AddComponent<QuestUI::Backgroundable*>()->ApplyBackground(il2cpp_utils::newcsstr("panel-top"));
    titlecontainer->set_padding(UnityEngine::RectOffset::New_ctor(10, 10, 0, 0));
    titlecontainer->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(2);

    auto* titletext = QuestUI::BeatSaberUI::CreateText(titlecontainer->get_transform(), "Ain't nobody here but us chickens.");
    titletext->set_alignment(TMPro::TextAlignmentOptions::Center);
    titletext->set_fontSize(7);

    auto* subtext = QuestUI::BeatSaberUI::CreateText(SettingsContainer, "To modify MRCPlus, navigate to the Mod Settings menu.");
    subtext->set_fontSize(4.2);
    subtext->set_alignment(TMPro::TextAlignmentOptions::Center);

    UnityEngine::Transform* mrcToggle = SettingsContainer->Find(il2cpp_utils::newcsstr("MixedRealityCaptureEnabled"));
    mrcToggle->GetComponent<UnityEngine::UI::LayoutElement*>()->set_ignoreLayout(true);
    mrcToggle->set_localScale(UnityEngine::Vector3::get_zero());
}