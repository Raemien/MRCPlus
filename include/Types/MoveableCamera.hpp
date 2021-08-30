#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/Collider.hpp"

#include "VRUIControls/VRPointer.hpp"

DECLARE_CLASS_CODEGEN(MRCPlus, MoveableCamera, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(Array<VRUIControls::VRPointer*>*, pointerArray);
    DECLARE_INSTANCE_FIELD(UnityEngine::MeshRenderer*, cubeRenderer);
    DECLARE_INSTANCE_FIELD(UnityEngine::MeshRenderer*, previewRenderer);
    DECLARE_INSTANCE_FIELD(UnityEngine::Collider*, collider);
    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, Update);
)