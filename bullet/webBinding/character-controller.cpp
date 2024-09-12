
#include "extensions/btCharacterController.h"

#include "LinearMath/btVector3.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletCollision/CollisionShapes/btMultiSphereShape.h"
#include "BulletCollision/BroadphaseCollision/btOverlappingPairCache.h"
#include "BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h"
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "BulletCollision/CollisionShapes/btCapsuleShape.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"

#include "macro.h"

// extern "C"
// {
//     void onShapeHitExt(const int hit, const int controller) DLL_IMPORT(onShapeHitExt);
//     // void onControllerHitExt(const int hit, const int controller) DLL_IMPORT(onControllerHit);
// }

class btControllerHitReport : public btUserControllerHitReport
{
public:

    /**
    \brief Called when current controller hits a shape.
    This is called when the CCT moves and hits a shape. This will not be called when a moving shape hits a non-moving CCT.
    \param[in] hit Provides information about the hit.
    @see btControllerShapeHit
    */
    virtual void onShapeHit(const btControllerShapeHit& hit)
    {
        onShapeHitExt((int)(&hit), (int)(hit.controller));
    };

    /**
    \brief Called when current controller hits another controller.
    \param[in] hit Provides information about the hit.
    @see btControllersHit
    */
    virtual void onControllerHit(const btControllersHit& hit)
    {
        //onControllerHitExt((int)(&hit), (int)(hit.controller));
    };

    virtual void onShapeHitExt(const int hit, const int controller) = 0;

// protected:
//     virtual ~btControllerHitReport() {}
};

extern "C"
{
    // // btControllerHitReport
    // int DLL_EXPORT ControllerHitReport_new()
    // {
    //     return (int)new btControllerHitReport();
    // }

    int DLL_EXPORT ControllerHit_getHitWorldPos(int ptr)
    {
        btControllerHit *hit = (btControllerHit *)ptr;
        return (int)(&(hit->worldPos));
    }

    int DLL_EXPORT ControllerHit_getHitWorldNormal(int ptr)
    {
        btControllerHit *hit = (btControllerHit *)ptr;
        return (int)(&(hit->worldNormal));
    }

    int DLL_EXPORT ControllerHit_getHitMotionDir(int ptr)
    {
        btControllerHit *hit = (btControllerHit *)ptr;
        return (int)(&(hit->dir));
    }

    float DLL_EXPORT ControllerHit_getHitMotionLength(int ptr)
    {
        btControllerHit *hit = (btControllerHit *)ptr;
        return hit->length;
    }

    int DLL_EXPORT ControllerShapeHit_getHitShape(int ptr)
    {
        btControllerShapeHit *hit = (btControllerShapeHit *)ptr;
        return (int)(hit->mCollisionShape);
    }

    int DLL_EXPORT ControllerShapeHit_getHitCollisionObject(int ptr)
    {
        btControllerShapeHit *hit = (btControllerShapeHit *)ptr;
        return (int)(hit->mCollisionObject);
    }

    // int DLL_EXPORT ControllersHit_getHitController(int ptr)
    // {
    //     btControllersHit *hit = (btControllersHit *)ptr;
    //     return (int)(hit->other);
    // }
    
    void DLL_EXPORT CharacterController_setContactOffset(int ptrCCT, float v)
    {
        btCharacterController *c0 = (btCharacterController *)ptrCCT;
        c0->setContactOffset(v);
    }

    void DLL_EXPORT CharacterController_setStepOffset(int ptrCCT, float v)
    {
        btCharacterController *c0 = (btCharacterController *)ptrCCT;
        c0->setStepHeight(v);
    }

    void DLL_EXPORT CharacterController_setSlopeLimit(int ptrCCT, float v)
    {
        btCharacterController *c0 = (btCharacterController *)ptrCCT;
        c0->setMaxSlope(v);
    }

    void DLL_EXPORT CharacterController_setCollision(int ptrCCT, bool v)
    {
        btCharacterController *c0 = (btCharacterController *)ptrCCT;
        c0->setCollision(v);
    }

    void DLL_EXPORT CharacterController_setOverlapRecovery(int ptrCCT, bool v)
    {
        btCharacterController *c0 = (btCharacterController *)ptrCCT;
        c0->setOverlapRecovery(v);
    }

    void DLL_EXPORT CharacterController_setUserPointer(int ptrCCT, int userPointer)
    {
        btCharacterController *c0 = (btCharacterController *)ptrCCT;
        void* ptr = (void*)userPointer;
        c0->setUserPointer(ptr);
    }

    int DLL_EXPORT CharacterController_getGhostObject(int ptrCCT)
    {
        btCharacterController *c0 = (btCharacterController *)ptrCCT;
        return (int)c0->getGhostObject();
    }

    int DLL_EXPORT CharacterController_getCollisionShape(int ptrCCT)
    {
        btCharacterController *c0 = (btCharacterController *)ptrCCT;
        return (int)c0->getCollisionShape();
    }

    int DLL_EXPORT CharacterController_move(int ptrCCT, int ptrMovement, float minDist, float deltaTime)
    {
        btCharacterController *c0 = (btCharacterController *)ptrCCT;
        btVector3 movement = *(btVector3 *)ptrMovement;
        return (int)(c0->move(movement, minDist, deltaTime));//return btControllerCollisionFlag
    }

    int DLL_EXPORT CharacterController_getPosition(int ptrCCT)
    {
        btCharacterController *c0 = (btCharacterController *)ptrCCT;
        return (int)&(c0->getPosition());
    }

    //btCapsuleCharacterControllerDesc
    int DLL_EXPORT CapsuleCharacterControllerDesc_new(float maxSlopeRadians, float stepHeight, float contactOffset, 
        int ptrUpAxis, int ptrInitPos, int ptruUserControllerHitReport, float radius, float height)
    {
        btCapsuleCharacterControllerDesc *desc = new btCapsuleCharacterControllerDesc();
        desc->m_maxSlopeRadians = maxSlopeRadians;
        desc->m_stepHeight = stepHeight;
        desc->m_contactOffset = contactOffset;
        desc->m_up = *(btVector3 *)ptrUpAxis;
        desc->m_initPos = *(btVector3 *)ptrInitPos;
        desc->m_userControllerHitReport = (btUserControllerHitReport*)ptruUserControllerHitReport;
        desc->m_fRadius = radius;
        desc->m_fHeight = height;
        return int(desc);
    }

    // btCapsuleCharacterController
    int DLL_EXPORT CapsuleCharacterController_new(int collisionWorld, int ptrBtCapsuleCharacterControllerDesc, int userObjectPointer)
    {
        btCollisionWorld *world = (btCollisionWorld *)collisionWorld;
        btCapsuleCharacterControllerDesc *desc = (btCapsuleCharacterControllerDesc *)ptrBtCapsuleCharacterControllerDesc;
        void* ptr = (void*)userObjectPointer;
        auto* controller = new btCapsuleCharacterController(world, desc, ptr);

        btGhostObject* ghostObject = controller->getGhostObject();
        uintptr_t intAsPointer = static_cast<uintptr_t>(0xffffffff);
        void* userPointer = reinterpret_cast<void*>(intAsPointer);
        ghostObject->setUserPointer(userPointer);

        return (int)controller;
    }

    void DLL_EXPORT CapsuleCharacterController_setRadius(int ptrCCT, float radius)
    {
        btCapsuleCharacterController *c0 = (btCapsuleCharacterController *)ptrCCT;
        c0->setRadius(radius);
    }

    void DLL_EXPORT CapsuleCharacterController_setHeight(int ptrCCT, float height)
    {
        btCapsuleCharacterController *c0 = (btCapsuleCharacterController *)ptrCCT;
        c0->setHeight(height);
    }

    //btBoxCharacterControllerDesc
    int DLL_EXPORT BoxCharacterControllerDesc_new(float maxSlopeRadians, float stepHeight, float contactOffset, 
        int ptrUpAxis, int ptrInitPos, int ptruUserControllerHitReport, float halfHeight, float halfSideExtent, float halfForwardExten)
    {
        btBoxCharacterControllerDesc *desc = new btBoxCharacterControllerDesc();
        desc->m_maxSlopeRadians = maxSlopeRadians;
        desc->m_stepHeight = stepHeight;
        desc->m_contactOffset = contactOffset;
        desc->m_up = *(btVector3 *)ptrUpAxis;
        desc->m_initPos = *(btVector3 *)ptrInitPos;
        desc->m_userControllerHitReport = (btUserControllerHitReport*)ptruUserControllerHitReport;
        desc->m_fHalfHeight = halfHeight;
        desc->m_fHalfSideExtent = halfSideExtent;
        desc->m_fHalfForwardExtent = halfForwardExten;
        return int(desc);
    }

    // btBoxCharacterController
    int DLL_EXPORT BoxCharacterController_new(int collisionWorld, int ptrBtBoxCharacterControllerDesc, int userObjectPointer)
    {
        btCollisionWorld *world = (btCollisionWorld *)collisionWorld;
        btBoxCharacterControllerDesc *desc = (btBoxCharacterControllerDesc *)ptrBtBoxCharacterControllerDesc;
        void* ptr = (void*)userObjectPointer;
        return (int)new btBoxCharacterController(world, desc, ptr);
    }
    
    void DLL_EXPORT BoxCharacterController_setHalfHeight(int ptrCCT, float v)
    {
        btBoxCharacterController *c0 = (btBoxCharacterController *)ptrCCT;
        c0->setHalfHeight(v);
    }

    void DLL_EXPORT BoxCharacterController_setHalfSideExtent(int ptrCCT, float v)
    {
        btBoxCharacterController *c0 = (btBoxCharacterController *)ptrCCT;
        c0->setHalfSideExtent(v);
    }

    void DLL_EXPORT BoxCharacterController_setHalfForwardExtent(int ptrCCT, float v)
    {
        btBoxCharacterController *c0 = (btBoxCharacterController *)ptrCCT;
        c0->setHalfForwardExtent(v);
    }
}