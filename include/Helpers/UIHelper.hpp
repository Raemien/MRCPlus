#include "UnityEngine/Vector2Int.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

#include "questui/shared/CustomTypes/Components/Settings/IncrementSetting.hpp"
#include "UnityEngine/UI/Toggle.hpp"

enum WarningText {Default, PerfWarning, RestartOBS, RestartGame};

extern UnityEngine::Transform* SettingsContainer;

extern void OnChangeFov(float newval);

extern void OnChangeSmoothing(float newval);

extern void OnChangeCameraMode_Deprecated(std::string newval);

extern void SetWarningText(WarningText txtnum);

extern std::vector<std::string> ModeValues;

extern std::string GetModeText(int mode);

extern void LocalizeComponent(UnityEngine::UI::Toggle* component, std::string key);

extern void LocalizeComponent(QuestUI::IncrementSetting* component, std::string key);

extern void LocalizeComponent(TMPro::TextMeshProUGUI* textObj, std::string key);

extern TMPro::TextMeshProUGUI* CreateLocalizableText(std::string key, UnityEngine::Transform* parent, bool italics = true);

extern std::string GetLocale(std::string key);

extern bool IsEnglish();

extern Array<UnityEngine::Vector2Int>* GetMRCResolutions();