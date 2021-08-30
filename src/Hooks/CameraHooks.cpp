#include "Helpers/HookInstaller.hpp"
#include "Helpers/ObjectHelper.hpp"
#include "Types/PreloadedFrames.hpp"
#include "MRCConfig.hpp"
#include "main.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

#include "UnityEngine/Time.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Texture.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/RenderTexture.hpp"
#include "UnityEngine/TextureFormat.hpp"
#include "UnityEngine/ImageConversion.hpp"

#include "GlobalNamespace/MainSettingsModelSO.hpp"
#include "GlobalNamespace/OVRCameraRig.hpp"
#include "GlobalNamespace/OVRPlugin.hpp"
#include "GlobalNamespace/OVRPlugin_Media.hpp"
#include "GlobalNamespace/OVRPlugin_Result.hpp"
#include "GlobalNamespace/OVRPlugin_OVRP_1_15_0.hpp"
#include "GlobalNamespace/OVRManager.hpp"
#include "GlobalNamespace/OVRManager_TrackingOrigin.hpp"
#include "GlobalNamespace/OVRExternalComposition.hpp"
#include "GlobalNamespace/OVRMixedRealityCaptureConfiguration.hpp"
#include "GlobalNamespace/OVRInput_Button.hpp"
#include "GlobalNamespace/OVRInput.hpp"
#include "GlobalNamespace/BoolSO.hpp"

#include "UnityEngine/Shader.hpp"

using namespace GlobalNamespace;

bool canSwitch = true;

void HotSwitchCamera()
{
    auto& modcfg = getConfig().config;
    if (!modcfg["useCameraHotkey"].GetBool()) return;

    std::string mode = (std::string)modcfg["cameraMode"].GetString();
    if (mode == "First Person") modcfg["cameraMode"].SetString("Third Person", modcfg.GetAllocator());
    else if (mode == "Third Person") modcfg["cameraMode"].SetString("First Person", modcfg.GetAllocator());
    canSwitch = false;
}

void InitPreloadedFrames()
{
    UnityEngine::GameObject* frameObj = UnityEngine::GameObject::New_ctor(il2cpp_utils::newcsstr("PreloadedMRCFrames"));
    UnityEngine::Object::DontDestroyOnLoad(frameObj);
    preloadedFrames = frameObj->AddComponent<MRCPlus::PreloadedFrames*>();
}

void SetCullingMasks(UnityEngine::Camera* hmdCam, UnityEngine::Camera* mrcCam)
{
    if (!hmdCam || !mrcCam) return;
    auto& modcfg = getConfig().config;
    long hmdMask = hmdCam->get_cullingMask();
    long mrcMask = mrcCam->get_cullingMask();    
    mrcMask = hmdMask;

    // Only render this layer in third-person
    hmdMask = hmdMask &= ~(1 << 3);
    mrcMask = mrcMask |= 1 << 3;

    // Only render this layer in first-person
    hmdMask = hmdMask |= 1 << 6;
    mrcMask = mrcMask &= ~(1 << 6);

    // Apply transparent wall setting
    mrcMask = mrcMask |= 1 << 27;
    if (modcfg["enableTransparentWalls"].GetBool())
    {
        mrcMask = mrcMask &= ~(1 << 27);
    }
    hmdCam->set_cullingMask(hmdMask);
    mrcCam->set_cullingMask(mrcMask);
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
    InitPreloadedFrames();
    CreateReferenceObject();

    auto& modcfg = getConfig().config;
    int fwidth = modcfg["width"].GetInt();
    int fheight = modcfg["height"].GetInt();
    OVRPlugin::Media::SetMrcFrameSize(fwidth, fheight);

    getLogger().info("[MRCPlus] Mixed Reality Capture initialized.");
}

MAKE_HOOK_MATCH(OVRExternalComposition_Update, &GlobalNamespace::OVRExternalComposition::Update, void, GlobalNamespace::OVRExternalComposition* instance, UnityEngine::GameObject* parentObj, UnityEngine::Camera* mainCam, GlobalNamespace::OVRMixedRealityCaptureConfiguration* mrcConfig, GlobalNamespace::OVRManager_TrackingOrigin trackingOrigin)
{
    OVRExternalComposition_Update(instance, parentObj, mainCam, mrcConfig, trackingOrigin);

    auto& modcfg = getConfig().config;
    auto* mainCamera = UnityEngine::Camera::get_main();
    auto* bgCamera = instance->backgroundCamera;
    auto* fgCamera = instance->foregroundCamera;

    bool mrcPlusActive = MRCPlusEnabled();

    if (viewfinderMat && bgCamera && modcfg["showViewfinder"].GetBool())
    {
        viewfinderMat->SetTexture(il2cpp_utils::newcsstr("_NearTex"), bgCamera->get_targetTexture());
        viewfinderMat->SetTexture(il2cpp_utils::newcsstr("_FarTex"), preloadedFrames->PreviewTitle);
    }

    // Extended camera properties
    if (!mrcPlusActive) return;
    SetCullingMasks(mainCamera, bgCamera);
    
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

        if (strcmp(modcfg["cameraMode"].GetString(), "First Person") == 0)
        {
            auto* targetTransform = maincam->get_transform();
            targetRot = targetTransform->get_rotation();
            targetPos = targetTransform->get_position();
        }
        if (strcmp(modcfg["cameraMode"].GetString(), "Third Person") == 0)
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

MAKE_HOOK_MATCH(MainSettingsModel_Load, &GlobalNamespace::MainSettingsModelSO::Load, void, GlobalNamespace::MainSettingsModelSO* instance, bool forced)
{
    MainSettingsModel_Load(instance, forced);
    BoolSO* mrcEnabled = (BoolSO*)UnityEngine::ScriptableObject::CreateInstance(csTypeOf(BoolSO*));
    mrcEnabled->value = true;
    instance->oculusMRCEnabled = mrcEnabled;
}

void MRCPlus::Hooks::Install_CameraHooks()
{
    INSTALL_HOOK(getLogger(), OVRPlugin_InitializeMR);
    INSTALL_HOOK(getLogger(), OVRCameraRig_Start);
    INSTALL_HOOK(getLogger(), OVRExternalComposition_Update);
    INSTALL_HOOK(getLogger(), OVRManager_LateUpdate);
    INSTALL_HOOK(getLogger(), MainSettingsModel_Load);
    getLogger().info("[MRCPlus] Installed Camera hooks");
}

