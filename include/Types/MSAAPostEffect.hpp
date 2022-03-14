#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/RenderTexture.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/Camera.hpp"


DECLARE_CLASS_CODEGEN(MRCPlus, MSAAPostEffect, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(UnityEngine::Shader*, fxaaShader);
    DECLARE_INSTANCE_FIELD(UnityEngine::Material*, fxaaMat);
    DECLARE_INSTANCE_FIELD(UnityEngine::Camera*, mrcCamera);
    DECLARE_INSTANCE_FIELD(int, width);
    DECLARE_INSTANCE_FIELD(int, height);
    DECLARE_INSTANCE_FIELD(int, msaaLevel);
    DECLARE_INSTANCE_FIELD(bool, useFXAA);

    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, Update);
    DECLARE_INSTANCE_METHOD(void, OnPreRender);
    DECLARE_INSTANCE_METHOD(void, OnRenderImage, UnityEngine::RenderTexture* src, UnityEngine::RenderTexture* dest);
)