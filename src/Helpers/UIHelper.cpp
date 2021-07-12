#include "main.hpp"
#include "MRCConfig.hpp"
#include "Helpers/ObjectHelper.hpp"
#include "Helpers/UIHelper.hpp"

#include "questui/shared/BeatSaberUI.hpp"

#include "questui/shared/CustomTypes/Components/Settings/IncrementSetting.hpp"
#include "UnityEngine/UI/Toggle.hpp"

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

void LocalizeComponent(QuestUI::IncrementSetting* component, std::string key)
{
    TMPro::TextMeshProUGUI* labelObj = component->GetComponentInChildren<TMPro::TextMeshProUGUI*>();
    LocalizeComponent(labelObj, key);
}

void LocalizeComponent(UnityEngine::UI::Toggle* component, std::string key)
{
    TMPro::TextMeshProUGUI* labelObj = component->get_transform()->get_parent()->Find(il2cpp_utils::newcsstr("NameText"))->GetComponent<TMPro::TextMeshProUGUI*>();
    LocalizeComponent(labelObj, key);
}

void LocalizeComponent(TMPro::TextMeshProUGUI* labelObj, std::string key)
{
    UnityEngine::GameObject* gameObject = labelObj->get_gameObject();    
    TMPro::TextMeshProUGUI* newTextObj = UnityEngine::Object::Instantiate(labelObj, gameObject->get_transform(), false);
    UnityEngine::Object::Destroy(labelObj);

    Il2CppString* localestr = Polyglot::Localization::Get(il2cpp_utils::newcsstr(key));

    newTextObj->get_rectTransform()->set_sizeDelta(UnityEngine::Vector2(60.0f, 10.0f));
    newTextObj->set_enableAutoSizing(false);
    newTextObj->set_text(localestr);
}

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