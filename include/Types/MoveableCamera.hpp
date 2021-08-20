#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

DECLARE_CLASS_CODEGEN(MRCPlus, MoveableCamera, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, Update);
)