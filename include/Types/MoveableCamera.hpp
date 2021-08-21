#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/Collider.hpp"

DECLARE_CLASS_CODEGEN(MRCPlus, MoveableCamera, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(UnityEngine::MeshRenderer*, renderer);
    DECLARE_INSTANCE_FIELD(UnityEngine::Collider*, collider);
    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, Update);
)