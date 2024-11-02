#include <stddef.h>

#include <emscripten.h>
#include <emscripten/bind.h>

#include "util.cpp"
#include "callback.cpp"
#include "character-controller.cpp"
#include "collision-object.cpp"
#include "collision-shape.cpp"
#include "collision-world.cpp"
#include "constraint.cpp"
#include "debug-draw.cpp"
#include "linear-math.cpp"
#include "macro.h"
#include "material.cpp"
#include "motion-state.cpp"
#include "rigid-body.cpp"

using namespace emscripten;

struct btDebugDrawWrapper : public wrapper<btDebugDraw> {
        EMSCRIPTEN_WRAPPER(btDebugDrawWrapper)
        void clearLines() override {
                return call<void>("clearLines");
        }
        void flushLines() override {
                return call<void>("flushLines");
        }
        void onDebugDrawLine(const int from, const int to, int color) override {
                return call<void>("onDebugDrawLine", from, to, color);
        }
};

struct btControllerHitReportWrapper : public wrapper<btControllerHitReport> {
        EMSCRIPTEN_WRAPPER(btControllerHitReportWrapper)
        void onShapeHitExt(const int hit, const int controller) override {
                return call<void>("onShapeHitExt", hit, controller);
        }
};

struct ccMotionStateWrapper : public wrapper<cc::ccMotionState> {
        EMSCRIPTEN_WRAPPER(ccMotionStateWrapper)
        void syncPhysicsToGraphics(const int id) override {
                return call<void>("syncPhysicsToGraphics", id);
        }
};

//
//----------------------------------------------- START EMBINDING ------------------------------------------
//
EMSCRIPTEN_BINDINGS(bullet) {

        //bind class 
        class_<btDebugDraw>("DebugDraw")
                .function("clearLines", &btDebugDraw::clearLines)
                .function("flushLines", &btDebugDraw::flushLines)
                .function("onDebugDrawLine", &btDebugDraw::onDebugDrawLine)
                .allow_subclass<btDebugDrawWrapper>("DrawWrapper", constructor<>());

        class_<btControllerHitReport>("ControllerHitReport")
                .function("onShapeHitExt", &btControllerHitReport::onShapeHitExt)
                .allow_subclass<btControllerHitReportWrapper>("ControllerHitReportWrapper", constructor<>());

        class_<cc::ccMotionState>("MotionState")
                .function("syncPhysicsToGraphics", &cc::ccMotionState::syncPhysicsToGraphics)
                .allow_subclass<ccMotionStateWrapper>("MotionStateWrapper", constructor<>());

        //_malloc
        function("_malloc", &_malloc);
        function("_free", &_free);
        function("_read_f32", &_read_f32);
        function("_write_f32", &_write_f32);
        function("_safe_delete", &_safe_delete);

        //Vec3
        function("Vec3_new", &Vec3_new, allow_raw_pointers());
        function("Vec3_x", &Vec3_x, allow_raw_pointers());
        function("Vec3_y", &Vec3_y, allow_raw_pointers());
        function("Vec3_z", &Vec3_z, allow_raw_pointers());
        function("Vec3_set", &Vec3_set, allow_raw_pointers());

        //Quat
        function("Quat_new", &Quat_new, allow_raw_pointers());
        function("Quat_x", &Quat_x, allow_raw_pointers());
        function("Quat_y", &Quat_y, allow_raw_pointers());
        function("Quat_z", &Quat_z, allow_raw_pointers());
        function("Quat_w", &Quat_w, allow_raw_pointers());
        function("Quat_set", &Quat_set, allow_raw_pointers());

        //Transform
        function("Transform_new", &Transform_new, allow_raw_pointers());
        function("Transform_setIdentity", &Transform_setIdentity, allow_raw_pointers());
        function("Transform_setRotation", &Transform_setRotation, allow_raw_pointers());
        function("Transform_getOrigin", &Transform_getOrigin, allow_raw_pointers());
        function("Transform_getRotation", &Transform_getRotation, allow_raw_pointers());
        function("Transform_getRotationAndOrigin", &Transform_getRotationAndOrigin, allow_raw_pointers());

        //MotionState
        function("MotionState_getWorldTransform", &MotionState_getWorldTransform, allow_raw_pointers());
        function("MotionState_setWorldTransform", &MotionState_setWorldTransform, allow_raw_pointers());
        // function("ccMotionState_new", &ccMotionState_new, allow_raw_pointers());
        function("ccMotionState_setup", &ccMotionState_setup, allow_raw_pointers());

        //int_array
        function("int_array_size", &int_array_size);
        function("int_array_at", &int_array_at);
        function("Vec3_array_at", &Vec3_array_at);

        //constraints
        function("TypedConstraint_getFixedBody", &TypedConstraint_getFixedBody, allow_raw_pointers());
        function("TypedConstraint_getDbgDrawSize", &TypedConstraint_getDbgDrawSize, allow_raw_pointers());
        function("TypedConstraint_setDbgDrawSize", &TypedConstraint_setDbgDrawSize, allow_raw_pointers());
        function("TypedConstraint_setMaxImpulseThreshold", &TypedConstraint_setMaxImpulseThreshold, allow_raw_pointers());

        function("HingeConstraint_new", &HingeConstraint_new, allow_raw_pointers());
        function("HingeConstraint_setFrames", &HingeConstraint_setFrames, allow_raw_pointers());
        function("HingeConstraint_setLimit", &HingeConstraint_setLimit, allow_raw_pointers());
        function("HingeConstraint_enableMotor", &HingeConstraint_enableMotor, allow_raw_pointers());
        function("HingeConstraint_setAngularOnly", &HingeConstraint_setAngularOnly, allow_raw_pointers());
        function("HingeConstraint_setMaxMotorImpulse", &HingeConstraint_setMaxMotorImpulse, allow_raw_pointers());
        function("HingeConstraint_setMotorTarget", &HingeConstraint_setMotorTarget, allow_raw_pointers());
        function("HingeConstraint_setMotorVelocity", &HingeConstraint_setMotorVelocity, allow_raw_pointers());

        function("P2PConstraint_new", &P2PConstraint_new, allow_raw_pointers());
        function("P2PConstraint_setPivotA", &P2PConstraint_setPivotA, allow_raw_pointers());
        function("P2PConstraint_setPivotB", &P2PConstraint_setPivotB, allow_raw_pointers());

        function("FixedConstraint_new", &FixedConstraint_new, allow_raw_pointers());
        function("FixedConstraint_setFrames", &FixedConstraint_setFrames, allow_raw_pointers());

        function("Generic6DofSpring2Constraint_new", &Generic6DofSpring2Constraint_new, allow_raw_pointers());
        function("Generic6DofSpring2Constraint_setFrames", &Generic6DofSpring2Constraint_setFrames, allow_raw_pointers());
        function("Generic6DofSpring2Constraint_setLimit", &Generic6DofSpring2Constraint_setLimit, allow_raw_pointers());
        function("Generic6DofSpring2Constraint_enableSpring", &Generic6DofSpring2Constraint_enableSpring, allow_raw_pointers());
        function("Generic6DofSpring2Constraint_setStiffness", &Generic6DofSpring2Constraint_setStiffness, allow_raw_pointers());
        function("Generic6DofSpring2Constraint_setDamping", &Generic6DofSpring2Constraint_setDamping, allow_raw_pointers());
        function("Generic6DofSpring2Constraint_setBounce", &Generic6DofSpring2Constraint_setBounce, allow_raw_pointers());
        function("Generic6DofSpring2Constraint_setEquilibriumPoint", &Generic6DofSpring2Constraint_setEquilibriumPoint, allow_raw_pointers());
        function("Generic6DofSpring2Constraint_enableMotor", &Generic6DofSpring2Constraint_enableMotor, allow_raw_pointers());
        function("Generic6DofSpring2Constraint_setMaxMotorForce", &Generic6DofSpring2Constraint_setMaxMotorForce, allow_raw_pointers());
        function("Generic6DofSpring2Constraint_setTargetVelocity", &Generic6DofSpring2Constraint_setTargetVelocity, allow_raw_pointers());
        function("Generic6DofSpring2Constraint_setServo", &Generic6DofSpring2Constraint_setServo, allow_raw_pointers());
        function("Generic6DofSpring2Constraint_setServoTarget", &Generic6DofSpring2Constraint_setServoTarget, allow_raw_pointers());

        //shapes
        function("CollisionShape_isCompound", &CollisionShape_isCompound, allow_raw_pointers());
        function("CollisionShape_setLocalScaling", &CollisionShape_setLocalScaling, allow_raw_pointers());
        function("CollisionShape_calculateLocalInertia", &CollisionShape_calculateLocalInertia, allow_raw_pointers());
        function("CollisionShape_getAabb", &CollisionShape_getAabb, allow_raw_pointers());
        function("CollisionShape_setMargin", &CollisionShape_setMargin, allow_raw_pointers());
        function("CollisionShape_setMaterial", &CollisionShape_setMaterial, allow_raw_pointers());
        function("CollisionShape_setUserPointer", &CollisionShape_setUserPointer, allow_raw_pointers());

        function("EmptyShape_static", &EmptyShape_static, allow_raw_pointers());
        function("ConvexInternalShape_getImplicitShapeDimensions", &ConvexInternalShape_getImplicitShapeDimensions, allow_raw_pointers());

        function("BoxShape_new", &BoxShape_new, allow_raw_pointers());
        function("BoxShape_setUnscaledHalfExtents", &BoxShape_setUnscaledHalfExtents, allow_raw_pointers());
        function("SphereShape_new", &SphereShape_new, allow_raw_pointers());
        function("SphereShape_setUnscaledRadius", &SphereShape_setUnscaledRadius, allow_raw_pointers());
        function("CylinderShape_new", &CylinderShape_new, allow_raw_pointers());
        function("CylinderShape_updateProp", &CylinderShape_updateProp, allow_raw_pointers());
        function("CapsuleShape_new", &CapsuleShape_new, allow_raw_pointers());
        function("CapsuleShape_updateProp", &CapsuleShape_updateProp, allow_raw_pointers());
        function("ConeShape_new", &ConeShape_new, allow_raw_pointers());
        function("ConeShape_setRadius", &ConeShape_setRadius, allow_raw_pointers());
        function("ConeShape_setHeight", &ConeShape_setHeight, allow_raw_pointers());
        function("ConeShape_setConeUpIndex", &ConeShape_setConeUpIndex, allow_raw_pointers());

        function("StaticPlaneShape_new", &StaticPlaneShape_new, allow_raw_pointers());
        function("StaticPlaneShape_getPlaneNormal", &StaticPlaneShape_getPlaneNormal, allow_raw_pointers());
        function("StaticPlaneShape_setPlaneConstant", &StaticPlaneShape_setPlaneConstant, allow_raw_pointers());

        function("TerrainShape_new", &TerrainShape_new, allow_raw_pointers());

        function("TriangleMesh_new", &TriangleMesh_new, allow_raw_pointers());
        function("TriangleMesh_addTriangle", &TriangleMesh_addTriangle, allow_raw_pointers());
        function("BvhTriangleMeshShape_new", &BvhTriangleMeshShape_new, allow_raw_pointers());
        function("BvhTriangleMeshShape_getOptimizedBvh", &BvhTriangleMeshShape_getOptimizedBvh, allow_raw_pointers());
        function("BvhTriangleMeshShape_setOptimizedBvh", &BvhTriangleMeshShape_setOptimizedBvh, allow_raw_pointers());
        function("ScaledBvhTriangleMeshShape_new", &ScaledBvhTriangleMeshShape_new, allow_raw_pointers());
        function("ConvexTriangleMeshShape_new", &ConvexTriangleMeshShape_new, allow_raw_pointers());

        function("SimplexShape_new", &SimplexShape_new, allow_raw_pointers());
        function("SimplexShape_addVertex", &SimplexShape_addVertex, allow_raw_pointers());

        function("ccCompoundShape_new", &ccCompoundShape_new, allow_raw_pointers());
        function("CompoundShape_getNumChildShapes", &CompoundShape_getNumChildShapes, allow_raw_pointers());
        function("CompoundShape_addChildShape", &CompoundShape_addChildShape, allow_raw_pointers());
        function("CompoundShape_getChildShape", &CompoundShape_getChildShape, allow_raw_pointers());
        function("CompoundShape_removeChildShape", &CompoundShape_removeChildShape, allow_raw_pointers());
        function("CompoundShape_updateChildTransform", &CompoundShape_updateChildTransform, allow_raw_pointers());
        // function("CompoundShape_setMaterial", &CompoundShape_setMaterial, allow_raw_pointers());

        //collision object
        function("CollisionObject_new", &CollisionObject_new, allow_raw_pointers());
        function("CollisionObject_getCollisionShape", &CollisionObject_getCollisionShape, allow_raw_pointers());
        function("CollisionObject_setCollisionShape", &CollisionObject_setCollisionShape, allow_raw_pointers());
        function("CollisionObject_setContactProcessingThreshold", &CollisionObject_setContactProcessingThreshold, allow_raw_pointers());
        function("CollisionObject_isSleeping", &CollisionObject_isSleeping, allow_raw_pointers());
        function("CollisionObject_getActivationState", &CollisionObject_getActivationState, allow_raw_pointers());
        // function("CollisionObject_setActivationState", &CollisionObject_setActivationState, allow_raw_pointers());
        function("CollisionObject_forceActivationState", &CollisionObject_forceActivationState, allow_raw_pointers());
        function("CollisionObject_activate", &CollisionObject_activate, allow_raw_pointers());
        function("CollisionObject_isActive", &CollisionObject_isActive, allow_raw_pointers());
        function("CollisionObject_isKinematicObject", &CollisionObject_isKinematicObject, allow_raw_pointers());
        function("CollisionObject_isStaticObject", &CollisionObject_isStaticObject, allow_raw_pointers());
        function("CollisionObject_isStaticOrKinematicObject", &CollisionObject_isStaticOrKinematicObject, allow_raw_pointers());
        function("CollisionObject_getWorldTransform", &CollisionObject_getWorldTransform, allow_raw_pointers());
        // function("CollisionObject_setWorldTransform", &CollisionObject_setWorldTransform, allow_raw_pointers());
        function("CollisionObject_setCollisionFlags", &CollisionObject_setCollisionFlags, allow_raw_pointers());
        function("CollisionObject_getCollisionFlags", &CollisionObject_getCollisionFlags, allow_raw_pointers());
        function("CollisionObject_setCcdMotionThreshold", &CollisionObject_setCcdMotionThreshold, allow_raw_pointers());
        function("CollisionObject_setCcdSweptSphereRadius", &CollisionObject_setCcdSweptSphereRadius, allow_raw_pointers());
        function("CollisionObject_setUserIndex", &CollisionObject_setUserIndex, allow_raw_pointers());
        function("CollisionObject_getUserIndex", &CollisionObject_getUserIndex, allow_raw_pointers());
        // function("CollisionObject_setUserPointer", &CollisionObject_setUserPointer, allow_raw_pointers());
        // function("CollisionObject_getUserPointer", &CollisionObject_getUserPointer, allow_raw_pointers());
        function("CollisionObject_setMaterial", &CollisionObject_setMaterial, allow_raw_pointers());
        function("CollisionObject_setIgnoreCollisionCheck", &CollisionObject_setIgnoreCollisionCheck, allow_raw_pointers());

        //Rigid Body
        function("RigidBody_new", &RigidBody_new, allow_raw_pointers());
        function("RigidBody_getFlags", &RigidBody_getFlags, allow_raw_pointers());
        function("RigidBody_setFlags", &RigidBody_setFlags, allow_raw_pointers());
        function("RigidBody_setGravity", &RigidBody_setGravity, allow_raw_pointers());
        function("RigidBody_setDamping", &RigidBody_setDamping, allow_raw_pointers());
        function("RigidBody_setMass", &RigidBody_setMass, allow_raw_pointers());
        function("RigidBody_setMassProps", &RigidBody_setMassProps, allow_raw_pointers());
        function("RigidBody_setLinearFactor", &RigidBody_setLinearFactor, allow_raw_pointers());
        function("RigidBody_setAngularFactor", &RigidBody_setAngularFactor, allow_raw_pointers());
        function("RigidBody_setLinearVelocity", &RigidBody_setLinearVelocity, allow_raw_pointers());
        function("RigidBody_getLinearVelocity", &RigidBody_getLinearVelocity, allow_raw_pointers());
        function("RigidBody_setAngularVelocity", &RigidBody_setAngularVelocity, allow_raw_pointers());
        function("RigidBody_getAngularVelocity", &RigidBody_getAngularVelocity, allow_raw_pointers());
        function("RigidBody_clearState", &RigidBody_clearState, allow_raw_pointers());
        function("RigidBody_clearForces", &RigidBody_clearForces, allow_raw_pointers());
        function("RigidBody_wantsSleeping", &RigidBody_wantsSleeping, allow_raw_pointers());
        function("RigidBody_setSleepingThresholds", &RigidBody_setSleepingThresholds, allow_raw_pointers());
        function("RigidBody_getLinearSleepingThreshold", &RigidBody_getLinearSleepingThreshold, allow_raw_pointers());
        function("RigidBody_getMotionState", &RigidBody_getMotionState, allow_raw_pointers());
        function("RigidBody_applyTorque", &RigidBody_applyTorque, allow_raw_pointers());
        function("RigidBody_applyForce", &RigidBody_applyForce, allow_raw_pointers());
        function("RigidBody_applyImpulse", &RigidBody_applyImpulse, allow_raw_pointers());
        function("RigidBody_getWorldTransform", &RigidBody_getWorldTransform, allow_raw_pointers());
        //dynamic
        function("DefaultCollisionConfiguration_static", &DefaultCollisionConfiguration_static, allow_raw_pointers());
        function("CollisionDispatcher_new", &CollisionDispatcher_new, allow_raw_pointers());
        function("Dispatcher_getNumManifolds", &Dispatcher_getNumManifolds, allow_raw_pointers());
        function("Dispatcher_getManifoldByIndexInternal", &Dispatcher_getManifoldByIndexInternal, allow_raw_pointers());

        //manifold
        function("ManifoldPoint_getShape0", &ManifoldPoint_getShape0, allow_raw_pointers());
        function("ManifoldPoint_getShape1", &ManifoldPoint_getShape1, allow_raw_pointers());
        function("ManifoldPoint_get_m_index0", &ManifoldPoint_get_m_index0, allow_raw_pointers());
        function("ManifoldPoint_get_m_index1", &ManifoldPoint_get_m_index1, allow_raw_pointers());
        function("PersistentManifold_getBody0", &PersistentManifold_getBody0, allow_raw_pointers());
        function("PersistentManifold_getBody1", &PersistentManifold_getBody1, allow_raw_pointers());
        function("PersistentManifold_getNumContacts", &PersistentManifold_getNumContacts, allow_raw_pointers());
        function("PersistentManifold_getContactPoint", &PersistentManifold_getContactPoint, allow_raw_pointers());
        function("ManifoldPoint_get_m_localPointA", &ManifoldPoint_get_m_localPointA, allow_raw_pointers());
        function("ManifoldPoint_get_m_localPointB", &ManifoldPoint_get_m_localPointB, allow_raw_pointers());
        function("ManifoldPoint_get_m_positionWorldOnA", &ManifoldPoint_get_m_positionWorldOnA, allow_raw_pointers());
        function("ManifoldPoint_get_m_positionWorldOnB", &ManifoldPoint_get_m_positionWorldOnB, allow_raw_pointers());
        function("ManifoldPoint_get_m_normalWorldOnB", &ManifoldPoint_get_m_normalWorldOnB, allow_raw_pointers());
        function("ManifoldPoint_get_m_positionWorldOnB", &ManifoldPoint_get_m_positionWorldOnB, allow_raw_pointers());

        function("DbvtBroadphase_new", &DbvtBroadphase_new, allow_raw_pointers());
        function("SequentialImpulseConstraintSolver_new", &SequentialImpulseConstraintSolver_new, allow_raw_pointers());

        function("ccDiscreteDynamicsWorld_reset", &ccDiscreteDynamicsWorld_reset, allow_raw_pointers());

        function("CollisionWorld_addCollisionObject", &CollisionWorld_addCollisionObject, allow_raw_pointers());
        function("CollisionWorld_removeCollisionObject", &CollisionWorld_removeCollisionObject, allow_raw_pointers());
        function("CollisionWorld_rayTest", &CollisionWorld_rayTest, allow_raw_pointers());
        function("CollisionWorld_convexSweepTest", &CollisionWorld_convexSweepTest, allow_raw_pointers());
        function("CollisionWorld_setDebugDrawer", &CollisionWorld_setDebugDrawer, allow_raw_pointers());
        function("CollisionWorld_debugDrawWorld", &CollisionWorld_debugDrawWorld, allow_raw_pointers());

        function("ccDiscreteDynamicsWorld_new", &ccDiscreteDynamicsWorld_new, allow_raw_pointers());
        function("ccDiscreteDynamicsWorld_setAllowSleep", &ccDiscreteDynamicsWorld_setAllowSleep, allow_raw_pointers());
        function("DynamicsWorld_setGravity", &DynamicsWorld_setGravity, allow_raw_pointers());
        function("DynamicsWorld_stepSimulation", &DynamicsWorld_stepSimulation, allow_raw_pointers());
        function("DynamicsWorld_addRigidBody", &DynamicsWorld_addRigidBody, allow_raw_pointers());
        function("DynamicsWorld_removeRigidBody", &DynamicsWorld_removeRigidBody, allow_raw_pointers());
        function("DynamicsWorld_addConstraint", &DynamicsWorld_addConstraint, allow_raw_pointers());
        function("DynamicsWorld_removeConstraint", &DynamicsWorld_removeConstraint, allow_raw_pointers());
        function("DynamicsWorld_addAction", &DynamicsWorld_addAction, allow_raw_pointers());
        function("DynamicsWorld_removeAction", &DynamicsWorld_removeAction, allow_raw_pointers());

        // function("DebugDraw_new", &DebugDraw_new, allow_raw_pointers());
        function("DebugDraw_setDebugMode", &DebugDraw_setDebugMode, allow_raw_pointers());
        function("DebugDraw_getDebugMode", &DebugDraw_getDebugMode, allow_raw_pointers());
        function("DebugDraw_setActiveObjectColor", &DebugDraw_setActiveObjectColor, allow_raw_pointers());
        function("DebugDraw_setDeactiveObjectColor", &DebugDraw_setDeactiveObjectColor, allow_raw_pointers());
        function("DebugDraw_setWantsDeactivationObjectColor", &DebugDraw_setWantsDeactivationObjectColor, allow_raw_pointers());
        function("DebugDraw_setDisabledDeactivationObjectColor", &DebugDraw_setDisabledDeactivationObjectColor, allow_raw_pointers());
        function("DebugDraw_setDisabledSimulationObjectColor", &DebugDraw_setDisabledSimulationObjectColor, allow_raw_pointers());
        function("DebugDraw_setAABBColor", &DebugDraw_setAABBColor, allow_raw_pointers());
        function("DebugDraw_setContactPointColor", &DebugDraw_setContactPointColor, allow_raw_pointers());
        function("DebugDraw_setConstraintLimitColor", &DebugDraw_setConstraintLimitColor, allow_raw_pointers());

        //ray cast
        function("RayCallback_hasHit", &RayCallback_hasHit, allow_raw_pointers());
        function("ConvexCallback_hasHit", &ConvexCallback_hasHit, allow_raw_pointers());

        function("ccAllRayCallback_static", &ccAllRayCallback_static, allow_raw_pointers());
        function("ccAllRayCallback_setFlags", &ccAllRayCallback_setFlags, allow_raw_pointers());
        function("ccAllRayCallback_reset", &ccAllRayCallback_reset, allow_raw_pointers());
        function("ccAllRayCallback_getHitPointWorld", &ccAllRayCallback_getHitPointWorld, allow_raw_pointers());
        function("ccAllRayCallback_getHitNormalWorld", &ccAllRayCallback_getHitNormalWorld, allow_raw_pointers());
        function("ccAllRayCallback_getCollisionShapePtrs", &ccAllRayCallback_getCollisionShapePtrs, allow_raw_pointers());
        function("ccAllRayCallback_getClosestHitFraction", &ccAllRayCallback_getClosestHitFraction, allow_raw_pointers());

        function("ccClosestRayCallback_static", &ccClosestRayCallback_static, allow_raw_pointers());
        function("ccClosestRayCallback_setFlags", &ccClosestRayCallback_setFlags, allow_raw_pointers());
        function("ccClosestRayCallback_reset", &ccClosestRayCallback_reset, allow_raw_pointers());
        function("ccClosestRayCallback_getHitPointWorld", &ccClosestRayCallback_getHitPointWorld, allow_raw_pointers());
        function("ccClosestRayCallback_getHitNormalWorld", &ccClosestRayCallback_getHitNormalWorld, allow_raw_pointers());
        function("ccClosestRayCallback_getCollisionShapePtr", &ccClosestRayCallback_getCollisionShapePtr, allow_raw_pointers());

        function("ccAllConvexCallback_static", &ccAllConvexCallback_static, allow_raw_pointers());
        function("ccAllConvexCallback_reset", &ccAllConvexCallback_reset, allow_raw_pointers());
        function("ccAllConvexCallback_getHitPointWorld", &ccAllConvexCallback_getHitPointWorld, allow_raw_pointers());
        function("ccAllConvexCallback_getHitNormalWorld", &ccAllConvexCallback_getHitNormalWorld, allow_raw_pointers());
        function("ccAllConvexCallback_getCollisionShapePtrs", &ccAllConvexCallback_getCollisionShapePtrs, allow_raw_pointers());
        function("ccAllConvexCallback_getClosestHitFraction", &ccAllConvexCallback_getClosestHitFraction, allow_raw_pointers());


        function("ccClosestConvexCallback_static", &ccClosestConvexCallback_static, allow_raw_pointers());
        function("ccClosestConvexCallback_reset", &ccClosestConvexCallback_reset, allow_raw_pointers());
        function("ccClosestConvexCallback_getHitPointWorld", &ccClosestConvexCallback_getHitPointWorld, allow_raw_pointers());
        function("ccClosestConvexCallback_getHitNormalWorld", &ccClosestConvexCallback_getHitNormalWorld, allow_raw_pointers());
        function("ccClosestConvexCallback_getCollisionShapePtr", &ccClosestConvexCallback_getCollisionShapePtr, allow_raw_pointers());
        function("ccClosestConvexCallback_getClosestHitFraction", &ccClosestConvexCallback_getClosestHitFraction, allow_raw_pointers());

        function("ccMaterial_new", &ccMaterial_new, allow_raw_pointers());
        function("ccMaterial_set", &ccMaterial_set, allow_raw_pointers());

        // CharacterController
        // function("ControllerHitReport_new", &ControllerHitReport_new, allow_raw_pointers());
        function("CharacterController_getGhostObject", &CharacterController_getGhostObject, allow_raw_pointers());
        function("CharacterController_getCollisionShape", &CharacterController_getCollisionShape, allow_raw_pointers());
        // function("ControllerHit_getCurrentController", &ControllerHit_getCurrentController, allow_raw_pointers());
        function("ControllerHit_getHitWorldPos", &ControllerHit_getHitWorldPos, allow_raw_pointers());
        function("ControllerHit_getHitWorldNormal", &ControllerHit_getHitWorldNormal, allow_raw_pointers());
        function("ControllerHit_getHitMotionDir", &ControllerHit_getHitMotionDir, allow_raw_pointers());
        function("ControllerHit_getHitMotionLength", &ControllerHit_getHitMotionLength, allow_raw_pointers());
        function("ControllerShapeHit_getHitShape", &ControllerShapeHit_getHitShape, allow_raw_pointers());
        function("ControllerShapeHit_getHitCollisionObject", &ControllerShapeHit_getHitCollisionObject, allow_raw_pointers());
        function("CharacterController_move", &CharacterController_move, allow_raw_pointers());
        function("CharacterController_getPosition", &CharacterController_getPosition, allow_raw_pointers());
        function("CharacterController_setContactOffset", &CharacterController_setContactOffset, allow_raw_pointers());
        function("CharacterController_setStepOffset", &CharacterController_setStepOffset, allow_raw_pointers());
        function("CharacterController_setSlopeLimit", &CharacterController_setSlopeLimit, allow_raw_pointers());
        function("CharacterController_setCollision", &CharacterController_setCollision, allow_raw_pointers());
        function("CharacterController_setOverlapRecovery", &CharacterController_setOverlapRecovery, allow_raw_pointers());
        function("CapsuleCharacterControllerDesc_new", &CapsuleCharacterControllerDesc_new, allow_raw_pointers());
        function("CapsuleCharacterController_new", &CapsuleCharacterController_new, allow_raw_pointers());
        function("CapsuleCharacterController_setRadius", &CapsuleCharacterController_setRadius, allow_raw_pointers());
        function("CapsuleCharacterController_setHeight", &CapsuleCharacterController_setHeight, allow_raw_pointers());
        function("BoxCharacterControllerDesc_new", &BoxCharacterControllerDesc_new, allow_raw_pointers());
        function("BoxCharacterController_new", &BoxCharacterController_new, allow_raw_pointers());
        function("BoxCharacterController_setHalfHeight", &BoxCharacterController_setHalfHeight, allow_raw_pointers());
        function("BoxCharacterController_setHalfSideExtent", &BoxCharacterController_setHalfSideExtent, allow_raw_pointers());
        function("BoxCharacterController_setHalfForwardExtent", &BoxCharacterController_setHalfForwardExtent, allow_raw_pointers());        
}
