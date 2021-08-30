#include "modloader/shared/modloader.hpp"
#include "main.hpp"

#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"

struct Config_t
{
    std::string cameraMode = "Disabled";
    bool useCameraHotkey = false;
    bool showViewfinder = true;
    bool enablePCWalls = false;
    bool enableTransparentWalls = false;
    int width = 1280;
    int height = 720;
    int fov = 100;
    int antiAliasing = 1;
    float positionSmoothness = 4;
    float rotationSmoothness = 4;
    float posX = 0.77f;
    float posY = 1.8f;
    float posZ = -1.25f;
    float angX = 12;
    float angY = 325;
    float angZ = 0;
}
Settings;

void SetupConfig()
{
    getConfig().config.RemoveAllMembers();
    getConfig().config.SetObject();
    auto& allocator = getConfig().config.GetAllocator();
    getConfig().config.AddMember("cameraMode", Settings.cameraMode, allocator);
    getConfig().config.AddMember("useCameraHotkey", Settings.useCameraHotkey, allocator);
    getConfig().config.AddMember("showViewfinder", Settings.showViewfinder, allocator);
    getConfig().config.AddMember("enablePCWalls", Settings.enablePCWalls, allocator);
    getConfig().config.AddMember("enableTransparentWalls", Settings.enableTransparentWalls, allocator);
    getConfig().config.AddMember("width", Settings.width, allocator);
    getConfig().config.AddMember("height", Settings.height, allocator);
    getConfig().config.AddMember("fov", Settings.fov, allocator);
    getConfig().config.AddMember("antiAliasing", Settings.antiAliasing, allocator);
    getConfig().config.AddMember("positionSmoothness", Settings.positionSmoothness, allocator);
    getConfig().config.AddMember("rotationSmoothness", Settings.rotationSmoothness, allocator);
    getConfig().config.AddMember("posX", Settings.posX, allocator);
    getConfig().config.AddMember("posY", Settings.posY, allocator);
    getConfig().config.AddMember("posZ", Settings.posZ, allocator);
    getConfig().config.AddMember("angX", Settings.angX, allocator);
    getConfig().config.AddMember("angY", Settings.angY, allocator);
    getConfig().config.AddMember("angZ", Settings.angZ, allocator);
    getConfig().Write();
}

bool LoadConfig()
{
    getConfig().Load();
    if(!getConfig().config.HasMember("cameraMode") || !getConfig().config["cameraMode"].IsString()) return false;
    if(!getConfig().config.HasMember("useCameraHotkey") || !getConfig().config["useCameraHotkey"].IsBool()) return false;
    if(!getConfig().config.HasMember("showViewfinder") || !getConfig().config["showViewfinder"].IsBool()) return false;
    if(!getConfig().config.HasMember("enablePCWalls") || !getConfig().config["enablePCWalls"].IsBool()) return false;
    if(!getConfig().config.HasMember("enableTransparentWalls") || !getConfig().config["enableTransparentWalls"].IsBool()) return false;
    if(!getConfig().config.HasMember("width") || !getConfig().config["width"].IsInt()) return false;
    if(!getConfig().config.HasMember("height") || !getConfig().config["height"].IsInt()) return false;
    if(!getConfig().config.HasMember("fov") || !getConfig().config["fov"].IsInt()) return false;
    if(!getConfig().config.HasMember("antiAliasing") || !getConfig().config["antiAliasing"].IsInt()) return false;
    if(!getConfig().config.HasMember("positionSmoothness") || !getConfig().config["positionSmoothness"].IsFloat()) return false;
    if(!getConfig().config.HasMember("rotationSmoothness") || !getConfig().config["rotationSmoothness"].IsFloat()) return false;
    if(!getConfig().config.HasMember("posX") || !getConfig().config["posX"].IsFloat()) return false;
    if(!getConfig().config.HasMember("posY") || !getConfig().config["posY"].IsFloat()) return false;
    if(!getConfig().config.HasMember("posZ") || !getConfig().config["posZ"].IsFloat()) return false;
    if(!getConfig().config.HasMember("angX") || !getConfig().config["angX"].IsFloat()) return false;
    if(!getConfig().config.HasMember("angY") || !getConfig().config["angY"].IsFloat()) return false;
    if(!getConfig().config.HasMember("angZ") || !getConfig().config["angZ"].IsFloat()) return false;
    return true;
}

bool MRCPlusEnabled()
{
    auto& modcfg = getConfig().config;
    bool enabled = strcmp(modcfg["cameraMode"].GetString(), "Disabled") != 0;
    bool moddedcam = strcmp(modcfg["cameraMode"].GetString(), "Mixed Reality") != 0;
    return (enabled && moddedcam);
}