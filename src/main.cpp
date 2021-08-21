#include "UI/SettingsMenu/MRCPlusFlowCoordinator.hpp"
#include "Types/MoveableCamera.hpp"
#include "Helpers/ObjectHelper.hpp"
#include "Helpers/HookInstaller.hpp"
#include "EmbeddedContent.hpp"
#include "MRCConfig.hpp"

#include "modloader/shared/modloader.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp" 
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"

#include "questui/shared/QuestUI.hpp"
#include "custom-types/shared/register.hpp"

#include "GlobalNamespace/OVRPlugin.hpp"
#include "GlobalNamespace/OVRPlugin_Fovf.hpp"
#include "GlobalNamespace/OVRPlugin_CameraIntrinsics.hpp"

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

using namespace GlobalNamespace;
using namespace MRCPlus;

static ModInfo modInfo;

OVRPlugin::CameraIntrinsics mrcInfo = OVRPlugin::CameraIntrinsics(false, 0.0, OVRPlugin::Fovf(0, 0, 0, 0));

// Move these to separate MRCViewfinder class later
UnityEngine::GameObject* rotationRef;
UnityEngine::RenderTexture* camTexture;

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

// Move this to MRCViewfinder's ctor later
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
        cubeObj->AddComponent<MRCPlus::MoveableCamera*>();

        // Viewfinder
        // cubeObj->GetComponent<UnityEngine::Renderer*>()->get_material()->SetTexture(il2cpp_utils::newcsstr("_MainTex"), (UnityEngine::Texture*)camTexture);
    }
}

extern "C" void setup(ModInfo& info) {

    info.id = "MRCPlus";
    info.version = "0.6.0";
    modInfo = info;
    getConfig().Load();
}

extern "C" void load() {
    if(!LoadConfig()) SetupConfig();
    std::string mrcpath = "sdcard/android/data/" + Modloader::getApplicationId() + "/files/mrc.xml";
    if(!fileexists(mrcpath)) writefile(mrcpath, FILE_MRCXML);
    il2cpp_functions::Init();
    QuestUI::Init();

    // Install Hooks
    MRCPlus::Hooks::Install_UIHooks();
    MRCPlus::Hooks::Install_CameraHooks();
    MRCPlus::Hooks::Install_RenderingHooks();

    // Register types + UI
    custom_types::Register::AutoRegister();
    QuestUI::Register::RegisterModSettingsFlowCoordinator<MRCPlus::MRCPlusFlowCoordinator*>(modInfo);
}
