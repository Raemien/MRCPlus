#pragma once
#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/Texture2D.hpp"
#include <string>

DECLARE_CLASS_CODEGEN(MRCPlus, PreloadedFrames, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(UnityEngine::Texture2D*, PreviewTitle);
    DECLARE_INSTANCE_FIELD(UnityEngine::Texture2D*, DisabledFrame_BG);
    DECLARE_STATIC_METHOD(UnityEngine::Texture2D*, LoadFromBase64, Il2CppString*);
    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, OnDestroy);
)