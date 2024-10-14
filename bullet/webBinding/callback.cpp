#include "extensions/ccOverlapFilterCallback.h"
#include "extensions/ccRayResultCallback.h"
#include "extensions/ccConvexCastCallBack.h"
#include "btBulletDynamicsCommon.h"
#include "macro.h"

// extern "C"
// {
    using cc::ccClosestRayResultCallback;
    using cc::ccAllHitsRayResultCallback;
    using cc::ccClosestConvexResultCallback;
    using cc::ccAllHitsConvexResultCallback;
    // RayResultCallback
    bool DLL_EXPORT RayCallback_hasHit(int ptr)
    {
        btCollisionWorld::RayResultCallback *callBack = (btCollisionWorld::RayResultCallback *)ptr;
        return callBack->hasHit();
    }

    bool DLL_EXPORT ConvexCallback_hasHit(int ptr)
    {
        btCollisionWorld::ConvexResultCallback *callBack = (btCollisionWorld::ConvexResultCallback *)ptr;
        return callBack->hasHit();
    }

    // ccClosestRayResultCallback
    int DLL_EXPORT ccClosestRayCallback_static()
    {
        static ccClosestRayResultCallback cb{btVector3{}, btVector3{}};
        return (int)&cb;
    }

    void DLL_EXPORT ccClosestRayCallback_setFlags(int ptr, int flags)
    {
        ccClosestRayResultCallback *cb = (ccClosestRayResultCallback *)ptr;
        cb->m_flags = flags;
    }

    void DLL_EXPORT ccClosestRayCallback_reset(int ptr, int from, int to, unsigned int mask, bool queryTrigger)
    {
        ccClosestRayResultCallback *cb = (ccClosestRayResultCallback *)ptr;
        cb->m_rayFromWorld = *(btVector3 *)from;
        cb->m_rayToWorld = *(btVector3 *)to;
        cb->reset((int)mask, queryTrigger);
    }

    int DLL_EXPORT ccClosestRayCallback_getHitNormalWorld(int ptr)
    {
        ccClosestRayResultCallback *cb = (ccClosestRayResultCallback *)ptr;
        return (int)&cb->getHitNormalWorld();
    }

    int DLL_EXPORT ccClosestRayCallback_getHitPointWorld(int ptr)
    {
        ccClosestRayResultCallback *cb = (ccClosestRayResultCallback *)ptr;
        return (int)&cb->getHitPointWorld();
    }

    int DLL_EXPORT ccClosestRayCallback_getCollisionShapePtr(int ptr)
    {
        ccClosestRayResultCallback *cb = (ccClosestRayResultCallback *)ptr;
        return cb->getCollisionShapePtr();
    }

    // ccAllHitsRayResultCallback
    int DLL_EXPORT ccAllRayCallback_static()
    {
        static ccAllHitsRayResultCallback cb{btVector3{}, btVector3{}};
        return (int)&cb;
    }

    void DLL_EXPORT ccAllRayCallback_setFlags(int ptr, int flags)
    {
        ccAllHitsRayResultCallback *cb = (ccAllHitsRayResultCallback *)ptr;
        cb->m_flags = flags;
    }

    void DLL_EXPORT ccAllRayCallback_reset(int ptr, int from, int to, unsigned int mask, bool queryTrigger)
    {
        ccAllHitsRayResultCallback *cb = (ccAllHitsRayResultCallback *)ptr;
        cb->m_rayFromWorld = *(btVector3 *)from;
        cb->m_rayToWorld = *(btVector3 *)to;
        cb->reset((int)mask, queryTrigger);
    }

    int DLL_EXPORT ccAllRayCallback_getHitPointWorld(int ptr)
    {
        ccAllHitsRayResultCallback *cb = (ccAllHitsRayResultCallback *)ptr;
        return (int)&cb->getHitPointWorld();
    }

    int DLL_EXPORT ccAllRayCallback_getHitNormalWorld(int ptr)
    {
        ccAllHitsRayResultCallback *result = (ccAllHitsRayResultCallback *)ptr;
        return (int)&result->getHitNormalWorld();
    }

    int DLL_EXPORT ccAllRayCallback_getCollisionShapePtrs(int ptr)
    {
        ccAllHitsRayResultCallback *result = (ccAllHitsRayResultCallback *)ptr;
        return (int)&result->getCollisionShapePtrs();
    }

    float DLL_EXPORT ccAllRayCallback_getClosestHitFraction(int ptr)
    {
        ccAllHitsRayResultCallback *result = (ccAllHitsRayResultCallback *)ptr;
        return result->getClosestHitFraction();
    }
    
    // ccClosestConvexResultCallback
    int DLL_EXPORT ccClosestConvexCallback_static()
    {
        static ccClosestConvexResultCallback cb{btVector3{}, btVector3{}};
        return (int)&cb;
    }

    void DLL_EXPORT ccClosestConvexCallback_reset(int ptr, int from, int to, unsigned int mask, bool queryTrigger)
    {
        ccClosestConvexResultCallback *cb = (ccClosestConvexResultCallback *)ptr;
        cb->m_convexFromWorld = *(btVector3 *)from;
        cb->m_convexToWorld = *(btVector3 *)to;
        cb->reset((int)mask, queryTrigger);
    }

    int DLL_EXPORT ccClosestConvexCallback_getHitNormalWorld(int ptr)
    {
        ccClosestConvexResultCallback *cb = (ccClosestConvexResultCallback *)ptr;
        return (int)&cb->getHitNormalWorld();
    }

    int DLL_EXPORT ccClosestConvexCallback_getHitPointWorld(int ptr)
    {
        ccClosestConvexResultCallback *cb = (ccClosestConvexResultCallback *)ptr;
        return (int)&cb->getHitPointWorld();
    }

    int DLL_EXPORT ccClosestConvexCallback_getCollisionShapePtr(int ptr)
    {
        ccClosestConvexResultCallback *cb = (ccClosestConvexResultCallback *)ptr;
        return cb->getCollisionShapePtr();
    }

    float DLL_EXPORT ccClosestConvexCallback_getClosestHitFraction(int ptr)
    {
        ccClosestConvexResultCallback *cb = (ccClosestConvexResultCallback *)ptr;
        return cb->getClosestHitFraction();
    }

    // ccAllHitsConvexResultCallback
    int DLL_EXPORT ccAllConvexCallback_static()
    {
        static ccAllHitsConvexResultCallback cb{btVector3{}, btVector3{}};
        return (int)&cb;
    }

    void DLL_EXPORT ccAllConvexCallback_reset(int ptr, int from, int to, unsigned int mask, bool queryTrigger)
    {
        ccAllHitsConvexResultCallback *cb = (ccAllHitsConvexResultCallback *)ptr;
        cb->m_convexFromWorld = *(btVector3 *)from;
        cb->m_convexToWorld = *(btVector3 *)to;
        cb->reset((int)mask, queryTrigger);
    }

    int DLL_EXPORT ccAllConvexCallback_getHitPointWorld(int ptr)
    {
        ccAllHitsConvexResultCallback *cb = (ccAllHitsConvexResultCallback *)ptr;
        return (int)&cb->getHitPointWorld();
    }

    int DLL_EXPORT ccAllConvexCallback_getHitNormalWorld(int ptr)
    {
        ccAllHitsConvexResultCallback *result = (ccAllHitsConvexResultCallback *)ptr;
        return (int)&result->getHitNormalWorld();
    }

    int DLL_EXPORT ccAllConvexCallback_getCollisionShapePtrs(int ptr)
    {
        ccAllHitsConvexResultCallback *result = (ccAllHitsConvexResultCallback *)ptr;
        return (int)&result->getCollisionShapePtrs();
    }

    float DLL_EXPORT ccAllConvexCallback_getClosestHitFraction(int ptr)
    {
        ccAllHitsConvexResultCallback *result = (ccAllHitsConvexResultCallback *)ptr;
        return result->getClosestHitFraction();
    }

    // btManifoldPoint
    int DLL_EXPORT ManifoldPoint_get_m_positionWorldOnA(int ptr)
    {
        btManifoldPoint *point = (btManifoldPoint *)ptr;
        return (int)&point->m_positionWorldOnA;
    }

    int DLL_EXPORT ManifoldPoint_get_m_positionWorldOnB(int ptr)
    {
        btManifoldPoint *point = (btManifoldPoint *)ptr;
        return (int)&point->m_positionWorldOnB;
    }

    int DLL_EXPORT ManifoldPoint_get_m_normalWorldOnB(int ptr)
    {
        btManifoldPoint *point = (btManifoldPoint *)ptr;
        return (int)&point->m_normalWorldOnB;
    }

    int DLL_EXPORT ManifoldPoint_get_m_localPointA(int ptr)
    {
        btManifoldPoint *point = (btManifoldPoint *)ptr;
        return (int)&point->m_localPointA;
    }

    int DLL_EXPORT ManifoldPoint_get_m_localPointB(int ptr)
    {
        btManifoldPoint *point = (btManifoldPoint *)ptr;
        return (int)&point->m_localPointB;
    }

    int DLL_EXPORT ManifoldPoint_getDistance(int ptr)
    {
        btManifoldPoint *point = (btManifoldPoint *)ptr;
        return point->getDistance();
    }

    int DLL_EXPORT ManifoldPoint_getShape0(int ptr)
    {
        btManifoldPoint *point = (btManifoldPoint *)ptr;
        return (int)point->getShape0();
    }

    int DLL_EXPORT ManifoldPoint_getShape1(int ptr)
    {
        btManifoldPoint *point = (btManifoldPoint *)ptr;
        return (int)point->getShape1();
    }

    int DLL_EXPORT ManifoldPoint_get_m_index0(int ptr)
    {
        btManifoldPoint *point = (btManifoldPoint *)ptr;
        return point->m_index0;
    }

    int DLL_EXPORT ManifoldPoint_get_m_index1(int ptr)
    {
        btManifoldPoint *point = (btManifoldPoint *)ptr;
        return point->m_index1;
    }
    // btPersistentManifold
    int DLL_EXPORT PersistentManifold_getBody0(int ptr)
    {
        btPersistentManifold *manifold = (btPersistentManifold *)ptr;
        return (int)manifold->getBody0();
    }

    int DLL_EXPORT PersistentManifold_getBody1(int ptr)
    {
        btPersistentManifold *manifold = (btPersistentManifold *)ptr;
        return (int)manifold->getBody1();
    }

    int DLL_EXPORT PersistentManifold_getNumContacts(int ptr)
    {
        btPersistentManifold *manifold = (btPersistentManifold *)ptr;
        return manifold->getNumContacts();
    }

    int DLL_EXPORT PersistentManifold_getContactPoint(int ptr, int index)
    {
        btPersistentManifold *manifold = (btPersistentManifold *)ptr;
        return (int)&manifold->getContactPoint(index);
    }
// }