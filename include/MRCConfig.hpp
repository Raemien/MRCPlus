#include "modloader/shared/modloader.hpp"
#include "main.hpp"

#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"

struct Config_t
{
    std::string cameraMode;
    bool useCameraHotkey;
    bool showViewfinder;
    bool enablePCWalls;
    bool enableTransparentWalls;
    int width;
    int height;
    int fov;
    int antiAliasing;
    float rotationSmoothness;
    float positionSmoothness;
    float posX;
    float posY;
    float posZ;
    float angX;
    float angY;
    float angZ;
};

bool MRCPlusEnabled();
bool LoadConfig();
void SetupConfig();