#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

#include "main.hpp"
#include "EmbeddedContent/Shaders.hpp"


#include "UnityEngine/ImageConversion.hpp"
#include "UnityEngine/AssetBundle.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Graphics.hpp"
#include "UnityEngine/Texture.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/Object.hpp"
#include "System/Convert.hpp"
#include "System/Type.hpp"

 // Load asset bundle from Base64 string
UnityEngine::AssetBundle* PrepareBundle(std::string encodedbytes)
{
    ArrayW<uint8_t> bytes = System::Convert::FromBase64String(il2cpp_utils::newcsstr(encodedbytes));
    static auto ab_LoadFromMemory = reinterpret_cast<function_ptr_t<UnityEngine::AssetBundle*, ArrayW<uint8_t>, uint>>(il2cpp_functions::resolve_icall("UnityEngine.AssetBundle::LoadFromMemory_Internal"));
    return ab_LoadFromMemory(bytes, 0);
}

UnityEngine::Shader* LoadFXAAEffect()
{
    UnityEngine::AssetBundle* bundle = PrepareBundle(SHADER_FXAA);
    static auto ab_LoadAllAssets = reinterpret_cast<function_ptr_t<ArrayW<UnityEngine::Object*>, UnityEngine::AssetBundle*, Il2CppString*, System::Type*>>(il2cpp_functions::resolve_icall("UnityEngine.AssetBundle::LoadAssetWithSubAssets_Internal"));
    ArrayW<UnityEngine::Object*> shaderArray = ab_LoadAllAssets(bundle, il2cpp_utils::newcsstr(""), csTypeOf(UnityEngine::Shader*));
    if (shaderArray.Length() < 1) return nullptr;
    UnityEngine::Shader* clonedShader = reinterpret_cast<UnityEngine::Shader*>(shaderArray[0]);
    bundle->Unload(false);
    return clonedShader;
}
