#include "UI/SettingsMenu/MRCPlusFlowCoordinator.hpp"
#include "UI/DefaultMRCMenu.hpp"
#include "Helpers/UIHelper.hpp"
#include "Helpers/ObjectHelper.hpp"
#include "EmbeddedContent.hpp"
#include "MRCConfig.hpp"

#include "modloader/shared/modloader.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp" 
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/config/config-utils.hpp"

#include "questui/shared/QuestUI.hpp"
#include "custom-types/shared/register.hpp"

#include "GlobalNamespace/MainCamera.hpp"
#include "GlobalNamespace/ConditionalActivation.hpp"
#include "GlobalNamespace/ConditionalMaterialSwitcher.hpp"
#include "GlobalNamespace/WindowResolutionSettingsController.hpp"
#include "GlobalNamespace/SettingsNavigationController.hpp"
#include "GlobalNamespace/OVRMixedRealityCaptureConfiguration.hpp"
#include "GlobalNamespace/OVRManager_TrackingOrigin.hpp"
#include "GlobalNamespace/OVRExternalComposition.hpp"
#include "GlobalNamespace/OVRMixedReality.hpp"
#include "GlobalNamespace/OVRExtensions.hpp"
#include "GlobalNamespace/OVRCameraRig.hpp"
#include "GlobalNamespace/OVRManager.hpp"
#include "GlobalNamespace/OVRPlugin.hpp"
#include "GlobalNamespace/OVRPlugin_Result.hpp"
#include "GlobalNamespace/OVRPlugin_Media.hpp"
#include "GlobalNamespace/OVRPlugin_Fovf.hpp"
#include "GlobalNamespace/OVRPlugin_OVRP_1_15_0.hpp"
#include "GlobalNamespace/OVRPlugin_CameraIntrinsics.hpp"
#include "GlobalNamespace/OVRInput.hpp"
#include "GlobalNamespace/OVRInput_Button.hpp"
#include "GlobalNamespace/BoolSO.hpp"

#include "UnityEngine/ScriptableObject.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/RenderTexture.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Component.hpp"
#include "UnityEngine/Collider.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Texture.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/Time.hpp"

using namespace GlobalNamespace;
using namespace MRCPlus;

static ModInfo modInfo;

OVRPlugin::CameraIntrinsics mrcInfo = OVRPlugin::CameraIntrinsics(false, 0.0, OVRPlugin::Fovf(0, 0, 0, 0));
UnityEngine::GameObject* rotationRef;
UnityEngine::RenderTexture* camTexture;
long originalCullMask = 0;
bool canSwitch = true;

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}

void SetAsymmetricFOV(float width, float height) // Using an asymmetric fov system for a centered image. Well played, Oculus.
{
    if (!MRCPlusEnabled()) return;
    float userfov = (float)getConfig().config["fov"].GetInt();
    float aratio = (width / height);
    float dec2rad = 0.0174532924f;
 
    float vfov_rad = userfov * dec2rad;
    float hfov_rad = std::atan(std::tan(vfov_rad * 0.5f) * aratio) * 2.0f;

    float vfov = std::tan(vfov_rad / 2);
    float hfov = std::tan(hfov_rad / 2);

    OVRPlugin::OverrideExternalCameraFov(0, true, OVRPlugin::Fovf(vfov, vfov, hfov, hfov));
}

void CreateReferenceObject()
{
    if (!rotationRef)
    {
        // Create reference object
        rotationRef = UnityEngine::GameObject::New_ctor(il2cpp_utils::newcsstr("MRCPlusObject"));
        UnityEngine::Object::DontDestroyOnLoad(rotationRef);

        // Placement visualizer
        UnityEngine::GameObject* cubeObj = UnityEngine::GameObject::CreatePrimitive((int)3);
        cubeObj->set_name(il2cpp_utils::newcsstr("Visualizer"));
        cubeObj->get_transform()->set_parent(rotationRef->get_transform());
        cubeObj->get_transform()->set_localScale(UnityEngine::Vector3::get_one() * 0.25);
        UnityEngine::Component::Destroy(cubeObj->GetComponent<UnityEngine::Collider*>());

        // Viewfinder
        cubeObj->GetComponent<UnityEngine::Renderer*>()->get_material()->SetTexture(il2cpp_utils::newcsstr("_MainTex"), (UnityEngine::Texture*)camTexture);

        // Apply visibility
        bool isVisible = getConfig().config["showViewfinder"].GetBool();
        ApplyViewfinderVisibility(isVisible);

    }
}

void HotSwitchCamera()
{
    auto& modcfg = getConfig().config;
    if (!modcfg["useCameraHotkey"].GetBool()) return;

    std::string mode = (std::string)modcfg["cameraMode"].GetString();
    if (mode == "First Person") modcfg["cameraMode"].SetString("Third Person", modcfg.GetAllocator());
    else if (mode == "Third Person") modcfg["cameraMode"].SetString("First Person", modcfg.GetAllocator());
    canSwitch = false;
}

MAKE_HOOK_MATCH(OVRCameraRig_Start, &GlobalNamespace::OVRCameraRig::Start, void, GlobalNamespace::OVRCameraRig* self)
{
    OVRCameraRig_Start(self);
    if (!OVRPlugin::GetUseOverriddenExternalCameraFov(0)) {
        OVRPlugin::Result success = OVRPlugin::OVRP_1_15_0::ovrp_GetExternalCameraIntrinsics(0, mrcInfo);
    }

    auto& modcfg = getConfig().config;
    int fwidth = modcfg["width"].GetInt();
    int fheight = modcfg["height"].GetInt();

    OVRPlugin::Media::SetMrcFrameSize(fwidth, fheight);
    SetAsymmetricFOV((float)fwidth, (float)fheight);
}

MAKE_HOOK_FIND_CLASS(OVRPlugin_InitializeMR, "", "OVRPlugin", "InitializeMixedReality", void)
{
    OVRPlugin_InitializeMR();
    CreateReferenceObject();

    auto& modcfg = getConfig().config;
    int fwidth = modcfg["width"].GetInt();
    int fheight = modcfg["height"].GetInt();
    OVRPlugin::Media::SetMrcFrameSize(fwidth, fheight);

    getLogger().info("[MRCPlus] Mixed Reality Capture initialized.");
}

MAKE_HOOK_MATCH(OVRExternalComposition_Update, &GlobalNamespace::OVRExternalComposition::Update, void, GlobalNamespace::OVRExternalComposition* self, UnityEngine::GameObject* parentObj, UnityEngine::Camera* mainCam, GlobalNamespace::OVRMixedRealityCaptureConfiguration* mrcConfig, GlobalNamespace::OVRManager_TrackingOrigin trackingOrigin)
{
    OVRExternalComposition_Update(self, parentObj, mainCam, mrcConfig, trackingOrigin);

    // Apply global changes
    auto& modcfg = getConfig().config;
    auto* mainCamera = UnityEngine::Camera::get_main();
    auto* bgCamera = self->backgroundCamera;
    bool mrcPlusActive = MRCPlusEnabled();
    int aafactor = modcfg["antiAliasing"].GetInt();
    aafactor = std::clamp((aafactor & (aafactor - 1) ? aafactor : 0), 0, 4);
    bgCamera->set_allowMSAA(true);
    camTexture = bgCamera->get_targetTexture();
    camTexture->set_antiAliasing(aafactor);

    // Set custom camera properties
    if (!mrcPlusActive) return;
    bool doFpCull = (std::string)modcfg["cameraMode"].GetString() == "First Person";
    originalCullMask = (originalCullMask == 0) ? bgCamera->get_cullingMask() : originalCullMask;
    bgCamera->set_cullingMask(doFpCull && mainCamera ? mainCamera->get_cullingMask(): originalCullMask);

    // Override camera placement
    UnityEngine::Transform* refTransform = rotationRef->get_transform();
    bgCamera->get_transform()->SetPositionAndRotation(refTransform->get_position(), refTransform->get_rotation());
}

MAKE_HOOK_MATCH(OVRManager_LateUpdate, &GlobalNamespace::OVRManager::LateUpdate, void, GlobalNamespace::OVRManager* self)
{
    OVRManager_LateUpdate(self);
    UnityEngine::Camera* maincam = UnityEngine::Camera::get_main();
    auto& modcfg = getConfig().config;
    if (rotationRef && maincam && MRCPlusEnabled())
    {
        // References
        auto* refTransform = rotationRef->get_transform();
        UnityEngine::Vector3 targetPos;
        UnityEngine::Quaternion targetRot;

        // Camera modes
        bool button_x = OVRInput::Get(OVRInput::Button::One, OVRInput::Controller::LTouch);
        bool button_anyclick = OVRInput::Get(OVRInput::Button::PrimaryIndexTrigger, OVRInput::Controller::All);
        if (button_x && canSwitch) HotSwitchCamera();

        if ((std::string)modcfg["cameraMode"].GetString() == "First Person")
        {
            auto* targetTransform = maincam->get_transform();
            targetRot = targetTransform->get_rotation();
            targetPos = targetTransform->get_position();
        }
        if ((std::string)modcfg["cameraMode"].GetString() == "Third Person")
        {
            targetRot = UnityEngine::Quaternion::Euler(modcfg["angX"].GetFloat(), modcfg["angY"].GetFloat(), modcfg["angZ"].GetFloat());
            targetPos = UnityEngine::Vector3(modcfg["posX"].GetFloat(), modcfg["posY"].GetFloat(), modcfg["posZ"].GetFloat());
        }
        canSwitch = !button_x;

        // Lerp smoothing
        float smoothpos = 10.0f - modcfg["positionSmoothness"].GetFloat();
        float smoothrot = 10.0f - modcfg["rotationSmoothness"].GetFloat();
        auto lerpPos = UnityEngine::Vector3::Lerp(refTransform->get_position(), targetPos, UnityEngine::Time::get_deltaTime() * std::clamp(smoothpos, 1.0f, 10.0f));
        auto lerpRot = UnityEngine::Quaternion::Slerp(refTransform->get_rotation(), targetRot, UnityEngine::Time::get_deltaTime() * std::clamp(smoothrot, 1.0f, 10.0f));

        refTransform->SetPositionAndRotation(lerpPos, lerpRot);
    }
}

MAKE_HOOK_MATCH(WindowResSetting_InitVals, &GlobalNamespace::WindowResolutionSettingsController::GetInitValues, bool, GlobalNamespace::WindowResolutionSettingsController* instance, int& index, int& size)
{
    // Don't initialize our values as we'll set them manually
    if (strcmp(to_utf8(csstrtostr(instance->get_transform()->get_parent()->get_name())).c_str(), "MRCResolutionContainer") == 0) return false;
    WindowResSetting_InitVals(instance, index, size);
    return false;
}

MAKE_HOOK_MATCH(WindowResSetting_ApplyValue, &GlobalNamespace::WindowResolutionSettingsController::ApplyValue, void, GlobalNamespace::WindowResolutionSettingsController* instance, int index)
{
    WindowResSetting_ApplyValue(instance, index);
    if (strcmp(to_utf8(csstrtostr(instance->get_transform()->get_parent()->get_name())).c_str(), "MRCResolutionContainer") != 0) return;
    auto resolution = instance->windowResolutions->values[index];
    getConfig().config["width"].SetInt(resolution.get_x());
    getConfig().config["height"].SetInt(resolution.get_y());
    getConfig().Write();
    instance->RefreshUI();
    if (OVRPlugin::IsMixedRealityInitialized()) 
    {
        OVRPlugin::Media::SetMrcFrameSize(resolution.get_x(), resolution.get_y());
        // SetWarningText(WarningText::RestartOBS);
    }
}

MAKE_HOOK_MATCH(SettingsNavController_DidActivate, &GlobalNamespace::SettingsNavigationController::DidActivate, void, GlobalNamespace::SettingsNavigationController* instance, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
    SettingsNavController_DidActivate(instance, firstActivation, addedToHierarchy, screenSystemEnabling);

    // Find container's transform
    SettingsContainer = instance->get_transform()->Find(il2cpp_utils::newcsstr("OculusMRCSettings/SettingsContainer"));
    if (SettingsContainer == nullptr) SettingsContainer = instance->get_transform()->Find(il2cpp_utils::newcsstr("Container/OculusMRCSettings/SettingsContainer"));

    // Apply MRC+'s modified menu
    if (firstActivation && addedToHierarchy) ModifyMRCMenu();
}

MAKE_HOOK_MATCH(ConditionalMaterialSwitcher_Awake, &GlobalNamespace::ConditionalMaterialSwitcher::Awake, void, GlobalNamespace::ConditionalMaterialSwitcher* instance)
{
    if (IsRegexMatch(instance->get_name(), "ObstacleCore|ObstacleFrame") && getConfig().config["enablePCWalls"].GetBool())
    {
        BoolSO* use_grappass = (BoolSO*)UnityEngine::ScriptableObject::CreateInstance(csTypeOf(BoolSO*));
        use_grappass->value = true;
        instance->value = use_grappass;
    }
    ConditionalMaterialSwitcher_Awake(instance);
}

MAKE_HOOK_MATCH(ConditionalActivation_Awake, &GlobalNamespace::ConditionalActivation::Awake, void, GlobalNamespace::ConditionalActivation* instance)
{
    ConditionalActivation_Awake(instance);
    if (IsRegexMatch(instance->get_name(), "GrabPassTexture1") && getConfig().config["enablePCWalls"].GetBool())
    {
        instance->get_gameObject()->SetActive(true);
    }
}

extern "C" void setup(ModInfo& info) {

    info.id = "MRCPlus";
    info.version = "0.5.2";
    modInfo = info;
    getConfig().Load();
}

extern "C" void load() {
    if(!LoadConfig()) SetupConfig();
    std::string mrcpath = "sdcard/android/data/" + Modloader::getApplicationId() + "/files/mrc.xml";
    if(!fileexists(mrcpath)) writefile(mrcpath, FILE_MRCXML);
    il2cpp_functions::Init();
    QuestUI::Init();

    // UI Hooks
    INSTALL_HOOK(getLogger(), SettingsNavController_DidActivate);
    INSTALL_HOOK(getLogger(), WindowResSetting_InitVals);
    INSTALL_HOOK(getLogger(), WindowResSetting_ApplyValue);

    // Effect Hooks
    INSTALL_HOOK(getLogger(), ConditionalMaterialSwitcher_Awake);
    INSTALL_HOOK(getLogger(), ConditionalActivation_Awake);

    // Camera Hooks
    INSTALL_HOOK(getLogger(), OVRPlugin_InitializeMR);
    INSTALL_HOOK(getLogger(), OVRCameraRig_Start);
    INSTALL_HOOK(getLogger(), OVRExternalComposition_Update);
    INSTALL_HOOK(getLogger(), OVRManager_LateUpdate);

    // Register types + UI
    custom_types::Register::AutoRegister();
    QuestUI::Register::RegisterModSettingsFlowCoordinator<MRCPlus::MRCPlusFlowCoordinator*>(modInfo);
}
