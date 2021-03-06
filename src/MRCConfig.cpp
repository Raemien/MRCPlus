#include "modloader/shared/modloader.hpp"
#include "main.hpp"

#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"

struct Config_t
{
    std::string cameraMode = "Disabled";
    int width = 1280;
    int height = 720;
    int fov = 90;
    int antiAliasing = 0;
    float positionSmoothness = 4;
    float rotationSmoothness = 4;
    float posX = 0;
    float posY = 1.75f;
    float posZ = 0;
    float angX = 0;
    float angY = 0;
    float angZ = 0;
}
Settings;

void SetupConfig()
{
    getConfig().config.RemoveAllMembers();
    getConfig().config.SetObject();
    auto& allocator = getConfig().config.GetAllocator();
    getConfig().config.AddMember("cameraMode", Settings.cameraMode, allocator);
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

    bool enabled = (std::string)modcfg["cameraMode"].GetString() != "Disabled";
    bool moddedcam = (std::string)modcfg["cameraMode"].GetString() != "Mixed Reality";
    return (enabled && moddedcam);
}