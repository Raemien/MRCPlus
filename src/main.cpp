#include "EmbeddedContent.hpp"
#include "UI/UIHelper.hpp"
#include "UI/MRCMenu.hpp"
#include "MRCConfig.hpp"

#include "modloader/shared/modloader.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp" 
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "questui/shared/QuestUI.hpp"

#include "GlobalNamespace/MainCamera.hpp"
#include "GlobalNamespace/WindowResolutionSettingsController.hpp"
#include "GlobalNamespace/SettingsNavigationController.hpp"
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

#include "System/Func_2.hpp"

using namespace GlobalNamespace;

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
        rotationRef = UnityEngine::GameObject::New_ctor(il2cpp_utils::createcsstr("MRCPlusObject"));
        UnityEngine::Object::DontDestroyOnLoad(rotationRef);

        // Placement visualizer
        UnityEngine::GameObject* cubeObj = UnityEngine::GameObject::CreatePrimitive((int)3);
        cubeObj->set_name(il2cpp_utils::newcsstr("Visualizer"));
        cubeObj->get_transform()->set_parent(rotationRef->get_transform());
        cubeObj->get_transform()->set_localScale(UnityEngine::Vector3::get_one() * 0.25);
        UnityEngine::Component::Destroy(cubeObj->GetComponent<UnityEngine::Collider*>());

        // Viewfinder
        cubeObj->GetComponent<UnityEngine::Renderer*>()->get_material()->SetTexture(il2cpp_utils::newcsstr("_MainTex"), (UnityEngine::Texture*)camTexture);
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

MAKE_HOOK_OFFSETLESS(OVRCameraRig_Start, void, GlobalNamespace::OVRCameraRig* self)
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

MAKE_HOOK_OFFSETLESS(OVRPlugin_InitializeMR, void, GlobalNamespace::OVRPlugin* self)
{
    OVRPlugin_InitializeMR(self);
    CreateReferenceObject();
    getLogger().info("[MRCPlus] Mixed Reality Capture initialized.");
}

MAKE_HOOK_OFFSETLESS(OVRExternalComposition_Update, void, GlobalNamespace::OVRExternalComposition* self, UnityEngine::GameObject* parentObj, UnityEngine::Camera* mainCam, System::Func_2<UnityEngine::Camera*, UnityEngine::GameObject*>* instMrcBGCamObj, System::Func_2<UnityEngine::Camera*, UnityEngine::GameObject*>* instMrcFGCamObj)
{
    OVRExternalComposition_Update(self, parentObj, mainCam, instMrcBGCamObj, instMrcFGCamObj);

    // Apply global changes
    auto& modcfg = getConfig().config;
    auto* bgCamera = self->backgroundCamera;
    bool mrcPlusActive = MRCPlusEnabled();
    int aafactor = modcfg["antiAliasing"].GetInt();
    aafactor = aafactor & (aafactor - 1) ? aafactor : 0;

    // Set custom camera properties
    if (!mrcPlusActive) return;
    bool doFpCull = (std::string)modcfg["cameraMode"].GetString() == "First Person";
    originalCullMask = (originalCullMask == 0) ? bgCamera->get_cullingMask() : originalCullMask;
    bgCamera->set_cullingMask(doFpCull ? UnityEngine::Camera::get_main()->get_cullingMask(): originalCullMask);

    // Override camera placement
    UnityEngine::Transform* refTransform = rotationRef->get_transform();
    bgCamera->get_transform()->SetPositionAndRotation(refTransform->get_position(), refTransform->get_rotation());
}

MAKE_HOOK_OFFSETLESS(OVRManager_LateUpdate, void, GlobalNamespace::OVRManager* self)
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

MAKE_HOOK_OFFSETLESS(WindowResSetting_InitVals, bool, GlobalNamespace::WindowResolutionSettingsController* instance, int& index, int& size)
{
    // Don't initialize our values as we'll set them manually
    if (instance->get_transform()->get_parent() == SettingsContainer) return false;
    WindowResSetting_InitVals(instance, index, size);
    return false;
}

MAKE_HOOK_OFFSETLESS(WindowResSetting_ApplyValue, void, GlobalNamespace::WindowResolutionSettingsController* instance, int index)
{
    WindowResSetting_ApplyValue(instance, index);
    if (instance->get_transform()->get_parent() != SettingsContainer) return;
    auto resolution = instance->windowResolutions->values[index];
    getConfig().config["width"].SetInt(resolution.get_x());
    getConfig().config["height"].SetInt(resolution.get_y());
    getConfig().Write();
    instance->RefreshUI();
    if (OVRPlugin::IsMixedRealityInitialized()) 
    {
        OVRPlugin::Media::SetMrcFrameSize(resolution.get_x(), resolution.get_y());
        SetWarningText(WarningText::RestartOBS);
    }
}

MAKE_HOOK_OFFSETLESS(SettingsNavController_DidActivate, void, GlobalNamespace::SettingsNavigationController* instance, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
    SettingsNavController_DidActivate(instance, firstActivation, addedToHierarchy, screenSystemEnabling);

    // Find container's transform
    SettingsContainer = instance->get_transform()->Find(il2cpp_utils::createcsstr("OculusMRCSettings/SettingsContainer"));
    if (SettingsContainer == nullptr) SettingsContainer = instance->get_transform()->Find(il2cpp_utils::createcsstr("Container/OculusMRCSettings/SettingsContainer"));

    // Apply MRC+'s modified menu
    if (firstActivation && addedToHierarchy) ModifyMRCMenu();
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
    INSTALL_HOOK_OFFSETLESS(getLogger(), SettingsNavController_DidActivate, il2cpp_utils::FindMethodUnsafe("", "SettingsNavigationController", "DidActivate", 3));
    INSTALL_HOOK_OFFSETLESS(getLogger(), WindowResSetting_InitVals, il2cpp_utils::FindMethodUnsafe("", "WindowResolutionSettingsController", "GetInitValues", 2));
    INSTALL_HOOK_OFFSETLESS(getLogger(), WindowResSetting_ApplyValue, il2cpp_utils::FindMethodUnsafe("", "WindowResolutionSettingsController", "ApplyValue", 1));
    INSTALL_HOOK_OFFSETLESS(getLogger(), OVRPlugin_InitializeMR, il2cpp_utils::FindMethod("", "OVRPlugin", "InitializeMixedReality"));
    INSTALL_HOOK_OFFSETLESS(getLogger(), OVRCameraRig_Start, il2cpp_utils::FindMethod("", "OVRCameraRig", "Start"));
    INSTALL_HOOK_OFFSETLESS(getLogger(), OVRExternalComposition_Update, il2cpp_utils::FindMethodUnsafe("", "OVRExternalComposition", "Update", 4));
    INSTALL_HOOK_OFFSETLESS(getLogger(), OVRManager_LateUpdate, il2cpp_utils::FindMethod("", "OVRManager", "LateUpdate"));
}
