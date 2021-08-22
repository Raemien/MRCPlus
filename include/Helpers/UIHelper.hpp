#include "UnityEngine/Vector2Int.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

#include "questui/shared/CustomTypes/Components/Settings/IncrementSetting.hpp"
#include "UnityEngine/UI/Toggle.hpp"

extern UnityEngine::Transform* SettingsContainer;

extern std::vector<std::string> ModeValues;

extern std::string GetModeText(int mode);

extern void LocalizeComponent(UnityEngine::UI::Toggle* component, std::string key);

extern void LocalizeComponent(QuestUI::IncrementSetting* component, std::string key);

extern void LocalizeComponent(TMPro::TextMeshProUGUI* textObj, std::string key);

extern TMPro::TextMeshProUGUI* CreateLocalizableText(std::string key, UnityEngine::Transform* parent, bool italics = true);

extern void SoftRestart();

extern bool IsEnglish();

extern bool IsHardwareCapable();

extern Array<UnityEngine::Vector2Int>* GetMRCResolutions();