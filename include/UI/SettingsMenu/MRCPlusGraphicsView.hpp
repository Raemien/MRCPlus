#include "questui/shared/CustomTypes/Components/Settings/IncrementSetting.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"

DECLARE_CLASS_CODEGEN(MRCPlus, MRCPlusGraphicsView, HMUI::ViewController,
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::VerticalLayoutGroup*, gfxContainer);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Toggle*, pcWallToggle);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Toggle*, transparentWallToggle);

    DECLARE_INSTANCE_METHOD(void, OnChangeVisibility, bool);

    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::FindMethodUnsafe("HMUI", "ViewController", "DidActivate", 3), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_OVERRIDE_METHOD(void, DidDeactivate, il2cpp_utils::FindMethodUnsafe("HMUI", "ViewController", "DidDeactivate", 2), bool removedFromHierarchy, bool systemScreenDisabling);
)