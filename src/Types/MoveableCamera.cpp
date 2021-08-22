#include "Types/MoveableCamera.hpp"
#include "main.hpp"

#include "beatsaber-hook/shared/utils/utils.h"

#include "GlobalNamespace/OVRInput.hpp"
#include "GlobalNamespace/VRController.hpp"
#include "VRUIControls/VRPointer.hpp"

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Physics.hpp"
#include "UnityEngine/Collider.hpp"
#include "UnityEngine/RaycastHit.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Time.hpp"

using namespace MRCPlus;
DEFINE_TYPE(MRCPlus, MoveableCamera);

namespace MRCPlus
{
    float grabDist = 0.0f;
    bool isHolding = false;
    UnityEngine::Vector3 raycastPos;
    UnityEngine::Quaternion raycastRot;

    void MoveableCamera::Awake()
    {
        auto& modcfg = getConfig().config;
        this->renderer = this->GetComponent<UnityEngine::MeshRenderer*>();
        this->collider = this->GetComponent<UnityEngine::Collider*>();
        raycastPos = UnityEngine::Vector3(modcfg["posX"].GetFloat(), modcfg["posY"].GetFloat(), modcfg["posZ"].GetFloat());
        raycastRot = UnityEngine::Quaternion::Euler(modcfg["angX"].GetFloat(), modcfg["angY"].GetFloat(), modcfg["angZ"].GetFloat());
        this->pointerArray = UnityEngine::Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer*>();
    }

    void MoveableCamera::Update()
    {
        if (!pointerArray->values[0] || !pointerArray->values[0]->m_CachedPtr) pointerArray = UnityEngine::Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer*>();
        if (pointerArray->Length() < 1) return;
        auto& modcfg = getConfig().config;

        bool visible = modcfg["showViewfinder"].GetBool() && strcmp(modcfg["cameraMode"].GetString(), "First Person") != 0;
        this->collider->set_enabled(visible);
        this->renderer->set_enabled(visible);
        if (pointerArray->values[0] == nullptr || !visible) return;

        GlobalNamespace::VRController* vrcontroller = pointerArray->values[0]->get_vrController();
        if (vrcontroller->get_triggerValue() > 0.75f)
        {
            UnityEngine::RaycastHit hitInfoRef;
            if (UnityEngine::Physics::Raycast(vrcontroller->get_position(), vrcontroller->get_forward(), hitInfoRef, 32.0f))
            {
                UnityEngine::RaycastHit hitInfo = hitInfoRef;
                if (hitInfo.get_collider()->get_transform() == this->get_transform()) isHolding = true;
            }
            if (isHolding)
            {
                raycastPos = vrcontroller->get_transform()->InverseTransformPoint(this->get_transform()->get_position());
                raycastRot = UnityEngine::Quaternion::Inverse(vrcontroller->get_transform()->get_rotation()) * this->get_transform()->get_rotation();

                UnityEngine::Vector3 grabPos = vrcontroller->get_transform()->get_position();
                grabPos = grabPos + (vrcontroller->get_forward() * grabDist);
                modcfg["posX"].SetFloat(grabPos.x);
                modcfg["posY"].SetFloat(grabPos.y);
                modcfg["posZ"].SetFloat(grabPos.z);
                grabDist += vrcontroller->get_verticalAxisValue() * UnityEngine::Time::get_deltaTime() * -0.8f;

                UnityEngine::Quaternion grabAngs = vrcontroller->get_transform()->get_rotation();
                UnityEngine::Vector3 eulerangs = grabAngs.get_eulerAngles();
                modcfg["angX"].SetFloat(eulerangs.x - 180.0f);
                modcfg["angY"].SetFloat(eulerangs.y);
                modcfg["angZ"].SetFloat(eulerangs.z - 180.0f);
                getConfig().Write();
            }
        }
        else isHolding = false;
    }
}
