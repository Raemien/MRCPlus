#include "Types/MSAAPostEffect.hpp"
#include "Helpers/AssetHelper.hpp"
#include "MRCConfig.hpp"
#include "main.hpp"

#include "beatsaber-hook/shared/utils/utils.h"

#include "UnityEngine/Shader.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/Graphics.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/RenderTexture.hpp"

DEFINE_TYPE(MRCPlus, MSAAPostEffect);

namespace MRCPlus
{
	void MSAAPostEffect::Awake()
    {
		auto& modcfg = getConfig().config;
        this->width = modcfg["width"].GetInt();
        this->height = modcfg["height"].GetInt();
        int aafactor = modcfg["antiAliasing"].GetInt();
        this->msaaLevel = std::clamp((aafactor & (aafactor - 1) ? 1 : aafactor), 1, 4);
	}

    void MSAAPostEffect::Update()
    {
		auto& modcfg = getConfig().config;
        this->width = modcfg["width"].GetInt();
        this->height = modcfg["height"].GetInt(); 
        int aafactor = modcfg["antiAliasing"].GetInt();
        this->msaaLevel = std::clamp((aafactor & (aafactor - 1) ? 1 : aafactor), 1, 4);
	}

    void MSAAPostEffect::OnPreRender()
    {
        mrcCamera->set_allowMSAA(msaaLevel > 1);

        UnityEngine::RenderTextureDescriptor texdesc = mrcCamera->get_targetTexture()->get_descriptor();

        int currentaa = mrcCamera->get_targetTexture()->get_antiAliasing();
        if (currentaa != msaaLevel || texdesc.width != width)
        {
           
            texdesc.msaaSamples = msaaLevel;
            texdesc.set_msaaSamples(msaaLevel);
            texdesc.width = width;
            texdesc.height = height;

            mrcCamera->get_targetTexture()->Release();
            mrcCamera->get_targetTexture()->set_descriptor(texdesc);
            mrcCamera->get_targetTexture()->Create();
        }
    }
}
