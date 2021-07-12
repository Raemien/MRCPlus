#pragma once
#include "modloader/shared/modloader.hpp"

#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/RenderTexture.hpp"

#include "GlobalNamespace/OVRPlugin_CameraIntrinsics.hpp"
#include "GlobalNamespace/OVRPlugin.hpp"

Configuration& getConfig();

extern Logger& getLogger();

extern void SetAsymmetricFOV(float width, float height);

extern void CreateReferenceObject();

extern UnityEngine::GameObject* rotationRef;

extern UnityEngine::RenderTexture* camTexture;

extern GlobalNamespace::OVRPlugin::CameraIntrinsics mrcInfo;