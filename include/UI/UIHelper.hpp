#include "UnityEngine/Vector2Int.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"

enum WarningText {Default, PerfWarning, RestartOBS, RestartGame};

extern UnityEngine::Transform* SettingsContainer;

extern void OnChangeFov(UnityEngine::GameObject* instance, float newval);

extern void OnChangeSmoothing(UnityEngine::GameObject* instance, float newval);

extern void OnChangeFirstPerson(UnityEngine::GameObject* instance, bool newval);

extern void OnChangeCameraMode(std::string newval);

extern void SetWarningText(WarningText txtnum);

extern std::vector<std::string> ModeValues;

extern std::string GetModeText(int mode);

extern std::string GetLocale(std::string key);

extern Array<UnityEngine::Vector2Int>* GetMRCResolutions();