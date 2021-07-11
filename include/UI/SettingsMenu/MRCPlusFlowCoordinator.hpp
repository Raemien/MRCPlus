#include "custom-types/shared/macros.hpp"
#include "HMUI/FlowCoordinator.hpp"
#include "HMUI/ViewController.hpp"

DECLARE_CLASS_CODEGEN(MRCPlus, MRCPlusFlowCoordinator, HMUI::FlowCoordinator,
    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::FindMethodUnsafe("HMUI", "FlowCoordinator", "DidActivate", 3), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_OVERRIDE_METHOD(void, BackButtonWasPressed, il2cpp_utils::FindMethodUnsafe("HMUI", "FlowCoordinator", "BackButtonWasPressed", 1), HMUI::ViewController* topViewController);
    DECLARE_INSTANCE_METHOD(void, UpdateSettingsVisibility, bool, bool);

    DECLARE_INSTANCE_FIELD(HMUI::ViewController*, mrcMainView);
    DECLARE_INSTANCE_FIELD(HMUI::ViewController*, mrcCameraView);
    DECLARE_INSTANCE_FIELD(HMUI::ViewController*, mrcGraphicsView);
)