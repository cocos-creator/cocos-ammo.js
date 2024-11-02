#include "extensions/ccDiscreteDynamicsWorld.h"
#include "btBulletDynamicsCommon.h"
#include "macro.h"

// extern "C"
// {
    using cc::ccDiscreteDynamicsWorld;

    // btDefaultCollisionConfiguration
    int DLL_EXPORT DefaultCollisionConfiguration_static()
    {
        static btDefaultCollisionConfiguration c;
        return (int)&c;
    }

    // btDispatcher
    int DLL_EXPORT Dispatcher_getNumManifolds(int ptr)
    {
        btDispatcher *dispatcher = (btDispatcher *)ptr;
        return dispatcher->getNumManifolds();
    }

    int DLL_EXPORT Dispatcher_getManifoldByIndexInternal(int ptr, int index)
    {
        btDispatcher *dispatcher = (btDispatcher *)ptr;
        return (int)dispatcher->getManifoldByIndexInternal(index);
    }

    // btCollisionDispatcher
    int DLL_EXPORT CollisionDispatcher_new()
    {
        return (int)new btCollisionDispatcher(
            (btDefaultCollisionConfiguration *)DefaultCollisionConfiguration_static());
    }

    // btDbvtBroadphase
    int DLL_EXPORT DbvtBroadphase_new()
    {
        return (int)(new btDbvtBroadphase());
    }

    // btSequentialImpulseConstraintSolver
    int DLL_EXPORT SequentialImpulseConstraintSolver_new()
    {
        return (int)new btSequentialImpulseConstraintSolver();
    }

    void DLL_EXPORT ccDiscreteDynamicsWorld_reset(int dispatcher,int broadphase,int solver)
    {
        btCollisionDispatcher *m_dispatcher = (btCollisionDispatcher *)dispatcher;
        btDbvtBroadphase *m_broadphase = (btDbvtBroadphase *)broadphase;
        btConstraintSolver *m_solver = (btConstraintSolver *)solver;

        m_broadphase->resetPool(m_dispatcher);
        m_solver->reset();
    }

    // btDispatcherInfo
    bool DLL_EXPORT CollisionWorld_get_m_useContinuous(int ptr)
    {
        btDispatcherInfo *info = (btDispatcherInfo *)ptr;
        return info->m_useContinuous;
    }

    void DLL_EXPORT CollisionWorld_set_m_useContinuous(int ptr, bool useContinuous)
    {
        btDispatcherInfo *info = (btDispatcherInfo *)ptr;
        info->m_useContinuous = useContinuous;
    }

    void DLL_EXPORT CollisionWorld_rayTest(int ptr, int rayFromWorld, int rayToWorld, int resultCallback)
    {
        btCollisionWorld *world = (btCollisionWorld *)ptr;
        world->rayTest(*(btVector3 *)rayFromWorld, *(btVector3 *)rayToWorld,
                       *(btCollisionWorld::RayResultCallback *)resultCallback);
    }

    int DLL_EXPORT CollisionWorld_getDispatchInfo(int ptr)
    {
        btCollisionWorld *world = (btCollisionWorld *)ptr;
        return (int)&world->getDispatchInfo();
    }

    void DLL_EXPORT CollisionWorld_addCollisionObject(int ptr, int co, unsigned int group, unsigned int mask)
    {
        btCollisionWorld *world = (btCollisionWorld *)ptr;
        world->addCollisionObject((btCollisionObject *)co, (int)group, (int)mask);
    }

    void DLL_EXPORT CollisionWorld_removeCollisionObject(int ptr, int collisionObject)
    {
        btCollisionWorld *world = (btCollisionWorld *)ptr;
        world->removeCollisionObject((btCollisionObject *)collisionObject);
    }

    void DLL_EXPORT CollisionWorld_convexSweepTest(int ptr, int castShape, int from, int to,
                                                   int resultCallback, float allowedCcdPenetration)
    {
        btCollisionWorld *world = (btCollisionWorld *)ptr;
        world->convexSweepTest((btConvexShape *)castShape, *(btTransform *)from, *(btTransform *)to,
                               *(btCollisionWorld::ConvexResultCallback *)resultCallback, allowedCcdPenetration);
    }

    void DLL_EXPORT CollisionWorld_setDebugDrawer(int ptr, int debugDrawer)
    {
        btCollisionWorld *world = (btCollisionWorld *)ptr;
        world->setDebugDrawer((btIDebugDraw *)debugDrawer);
    }

    void DLL_EXPORT CollisionWorld_debugDrawWorld(int ptr)
    {
        btCollisionWorld *world = (btCollisionWorld *)ptr;
        world->debugDrawWorld();
    }

    // btDynamicsWorld
    void DLL_EXPORT DynamicsWorld_addAction(int ptr, int action)
    {
        btDynamicsWorld *world = (btDynamicsWorld *)ptr;
        world->addAction((btActionInterface *)action);
    }

    void DLL_EXPORT DynamicsWorld_removeAction(int ptr, int action)
    {
        btDynamicsWorld *world = (btDynamicsWorld *)ptr;
        world->removeAction((btActionInterface *)action);
    }

    int DLL_EXPORT DynamicsWorld_getSolverInfo(int ptr)
    {
        btDynamicsWorld *world = (btDynamicsWorld *)ptr;
        return (int)&world->getSolverInfo();
    }

    // ccDiscreteDynamicsWorld
    int DLL_EXPORT ccDiscreteDynamicsWorld_new(int dispatcher, int pairCache, int constraintSolver)
    {
        return (int)new ccDiscreteDynamicsWorld(
            (btDispatcher *)dispatcher, (btBroadphaseInterface *)pairCache, (btConstraintSolver *)constraintSolver,
            (btDefaultCollisionConfiguration *)DefaultCollisionConfiguration_static());
    }

    void DLL_EXPORT DynamicsWorld_setGravity(int ptr, int gravity)
    {
        btDynamicsWorld *world = (btDynamicsWorld *)ptr;
        world->setGravity(*(btVector3 *)gravity);
    }

    void DLL_EXPORT DynamicsWorld_addRigidBody(int ptr, int body, unsigned int group, unsigned int mask)
    {
        btDynamicsWorld *world = (btDynamicsWorld *)ptr;
        return world->addRigidBody((btRigidBody *)body, (int)group, (int)mask);
    }

    void DLL_EXPORT DynamicsWorld_removeRigidBody(int ptr, int body)
    {
        btDynamicsWorld *world = (btDynamicsWorld *)ptr;
        return world->removeRigidBody((btRigidBody *)body);
    }

    void DLL_EXPORT DynamicsWorld_addConstraint(int ptr, int c, bool v)
    {
        btDynamicsWorld *world = (btDynamicsWorld *)ptr;
        world->addConstraint((btTypedConstraint *)c, v);
    }

    void DLL_EXPORT DynamicsWorld_removeConstraint(int ptr, int c)
    {
        btDynamicsWorld *world = (btDynamicsWorld *)ptr;
        world->removeConstraint((btTypedConstraint *)c);
    }

    void DLL_EXPORT DynamicsWorld_stepSimulation(int ptr, float timeStep, int maxSubSteps, float fixedTimeStep)
    {
        btDynamicsWorld *world = (btDynamicsWorld *)ptr;
        world->stepSimulation(timeStep, maxSubSteps, fixedTimeStep);
    }

    void DLL_EXPORT ccDiscreteDynamicsWorld_setAllowSleep(int ptr, bool v)
    {
        ccDiscreteDynamicsWorld *world = (ccDiscreteDynamicsWorld *)ptr;
        world->setAllowSleep(v);
    }
// }
