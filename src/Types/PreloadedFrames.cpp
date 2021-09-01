#include "Types/PreloadedFrames.hpp"
#include "EmbeddedContent.hpp"
#include "main.hpp"

#include "beatsaber-hook/shared/utils/utils.h"

#include "UnityEngine/Texture.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/TextureFormat.hpp"
#include "UnityEngine/ImageConversion.hpp"
#include "UnityEngine/Component.hpp"

#include "System/Convert.hpp"

DEFINE_TYPE(MRCPlus, PreloadedFrames);

namespace MRCPlus
{
    void PreloadedFrames::Awake()
    {
        if (!PreviewTitle) this->PreviewTitle = PreloadedFrames::LoadFromBase64(il2cpp_utils::newcsstr(IMG_PREVIEWTXT));
    }

    void PreloadedFrames::OnDestroy()
    {
        if (PreviewTitle) UnityEngine::Component::Destroy(this->PreviewTitle);
    }

    UnityEngine::Texture2D* PreloadedFrames::LoadFromBase64(Il2CppString* input)
    {
        Array<uint8_t>* bytes = System::Convert::FromBase64String(input);
        UnityEngine::Texture2D* output = UnityEngine::Texture2D::New_ctor(480, 360, UnityEngine::TextureFormat::RGBA32, false, false);
        UnityEngine::ImageConversion::LoadImage(output, bytes, false);
        return output;
    }

}
