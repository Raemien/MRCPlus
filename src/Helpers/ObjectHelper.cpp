#include "main.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Component.hpp"
#include "UnityEngine/GameObject.hpp"
#include "GlobalNamespace/BoolSO.hpp"
#include "GlobalNamespace/BoolSettingsController.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

#include <regex>

GlobalNamespace::BoolSO* GetMRCBoolSO()
{
    Array<GlobalNamespace::BoolSettingsController*>* boolSOArray = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::BoolSettingsController*>();
    for (int i = 0; i < sizeof(boolSOArray); i++)
    {
        GlobalNamespace::BoolSettingsController* boolcontr = (GlobalNamespace::BoolSettingsController*)boolSOArray->values[i];
        if (boolcontr->get_gameObject()->get_name()->Contains(il2cpp_utils::newcsstr("MixedRealityCaptureEnabled")))
        {
            return boolcontr->settingsValue;
        } 
    }
    getLogger().warning("[MRCPlus] Unable to find the MRC toggle! Please yell at Raemien if you see this message.");
    return nullptr;
}

bool IsRegexMatch(Il2CppString* input, std::string pattern)
{
    return std::regex_search(to_utf8(csstrtostr(input)), std::regex(pattern));
}