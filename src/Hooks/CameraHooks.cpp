#include "Helpers/HookInstaller.hpp"
#include "MRCConfig.hpp"
#include "main.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

#include "UnityEngine/Time.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/Texture.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/RenderTexture.hpp"

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

using namespace GlobalNamespace;

long originalCullMask = 0;
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

    // Apply anti-aliasing
    auto& modcfg = getConfig().config;
    auto* mainCamera = UnityEngine::Camera::get_main();
    auto* bgCamera = self->backgroundCamera;

    bool mrcPlusActive = MRCPlusEnabled();
    int aafactor = modcfg["antiAliasing"].GetInt();
    aafactor = std::clamp((aafactor & (aafactor - 1) ? aafactor : 0), 0, 8);

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


void MRCPlus::Hooks::Install_CameraHooks()
{
    INSTALL_HOOK(getLogger(), OVRPlugin_InitializeMR);
    INSTALL_HOOK(getLogger(), OVRCameraRig_Start);
    INSTALL_HOOK(getLogger(), OVRExternalComposition_Update);
    INSTALL_HOOK(getLogger(), OVRManager_LateUpdate);
    getLogger().info("[MRCPlus] Installed Camera hooks");
}

