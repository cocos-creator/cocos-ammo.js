
#ifndef CC_RAY_RESULT_CALLBACK_H
#define CC_RAY_RESULT_CALLBACK_H

#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "LinearMath/btAlignedObjectArray.h"
#include "LinearMath/btScalar.h"

namespace cc
{

typedef btCollisionWorld::ClosestRayResultCallback ClosestRayResultCallback;
typedef btCollisionWorld::AllHitsRayResultCallback AllHitsRayResultCallback;
typedef btCollisionWorld::LocalRayResult LocalRayResult;

namespace {
	bool isCharacterController(btCollisionObject* obj) {
		uintptr_t userPointer = reinterpret_cast<uintptr_t>(obj->getUserPointer());
		// character controller set user pointer in CapsuleCharacterController_new().
		return (userPointer == static_cast<uintptr_t>(0xffffffff));
	}
}

struct ccClosestRayResultCallback : public ClosestRayResultCallback
{
	int m_shapeUserPointer;
	bool m_queryTrigger;

	ccClosestRayResultCallback(const btVector3&	rayFromWorld,const btVector3&	rayToWorld)
	:ClosestRayResultCallback(rayFromWorld, rayToWorld), m_shapeUserPointer(0), m_queryTrigger(true)
	{
		m_collisionFilterGroup = -1;
	}

	// return true when pairs need collision
	virtual bool needsCollision(btBroadphaseProxy* proxy0) const
	{
		btCollisionObject* co = static_cast<btCollisionObject*>(proxy0->m_clientObject);
		// Ignore charactor controller as Physx doesn't handle it in raycast.
		if (isCharacterController(co)) {
                    return false;
                }

		if ((proxy0->m_collisionFilterGroup & m_collisionFilterMask) != 0) {			
			if (!m_queryTrigger && proxy0->m_clientObject) {
				return co->hasContactResponse();
			}
			return true;
		} else {
			return false;
		}
	}
	
	virtual	btScalar	addSingleResult(LocalRayResult& rayResult,bool normalInWorldSpace)
	{
		const btCollisionShape* shape = rayResult.m_collisionObject->getCollisionShape();
		if(shape->isCompound() && rayResult.m_localShapeInfo) {
			const btCompoundShape* compound = static_cast<const btCompoundShape*>(shape);
			const int index = rayResult.m_localShapeInfo->m_shapePart;
			m_shapeUserPointer = compound->getChildShape(index)->getUserPointerAsInt();
		} else {
			m_shapeUserPointer = shape->getUserPointerAsInt();
		}
		return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
	}

	int getCollisionShapePtr() const 
	{
		return m_shapeUserPointer;
	}

	btVector3 &getHitNormalWorld()
	{
		return m_hitNormalWorld;
	}

	btVector3 &getHitPointWorld()
	{
		return m_hitPointWorld;
	}

	btScalar getClosestHitFraction() 
	{
		return m_closestHitFraction;
	}

	void reset(int mask, bool queryTrigger) {
		m_collisionFilterMask = mask;
		m_queryTrigger = queryTrigger;
		m_closestHitFraction = btScalar(1.);
		m_collisionObject = 0;
	}
};

struct ccAllHitsRayResultCallback : public AllHitsRayResultCallback
{
	btAlignedObjectArray<int> m_shapeUserPointers;
	bool m_queryTrigger;

	ccAllHitsRayResultCallback(const btVector3&	rayFromWorld,const btVector3&	rayToWorld)
	:AllHitsRayResultCallback(rayFromWorld, rayToWorld), m_queryTrigger(true)
	{
		m_collisionFilterGroup = -1;
	}

	// return true when pairs need collision
	virtual bool needsCollision(btBroadphaseProxy* proxy0) const
	{
		btCollisionObject* co = static_cast<btCollisionObject*>(proxy0->m_clientObject);
		// Ignore charactor controller as Physx doesn't handle it in raycast.
		if (isCharacterController(co)) {
                    return false;
                }

		if ((proxy0->m_collisionFilterGroup & m_collisionFilterMask) != 0) {			
			if (!m_queryTrigger && proxy0->m_clientObject) {
				return co->hasContactResponse();
			}
			return true;
		} else {
			return false;
		}
	}
	
	virtual	btScalar	addSingleResult(LocalRayResult& rayResult,bool normalInWorldSpace)
	{
		const btCollisionShape* shape = rayResult.m_collisionObject->getCollisionShape();
		if(shape->isCompound() && rayResult.m_localShapeInfo) {
			const btCompoundShape* compound = static_cast<const btCompoundShape*>(shape);
			const int index = rayResult.m_localShapeInfo->m_shapePart;
			m_shapeUserPointers.push_back(compound->getChildShape(index)->getUserPointerAsInt());
		} else {
			m_shapeUserPointers.push_back(shape->getUserPointerAsInt());
		}
		return AllHitsRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
	}

	btAlignedObjectArray<int> &getCollisionShapePtrs() 
	{
		return m_shapeUserPointers;
	}

	btAlignedObjectArray<btVector3> &getHitNormalWorld()
	{
		return m_hitNormalWorld;
	}

	btAlignedObjectArray<btVector3> &getHitPointWorld()
	{
		return m_hitPointWorld;
	}

	btAlignedObjectArray<btScalar> &getHitFractions() 
	{
		return m_hitFractions;
	}

	void reset(unsigned int mask, bool queryTrigger)
	{
		m_collisionFilterMask = mask;
		m_queryTrigger = queryTrigger;
		m_closestHitFraction = btScalar(1.);
		m_collisionObject = 0;
		m_shapeUserPointers.resize(0);
		m_hitFractions.resize(0);
		m_collisionObjects.resize(0);
		m_hitPointWorld.resize(0);
		m_hitNormalWorld.resize(0);
	}
};

}

#endif //CC_RAY_RESULT_CALLBACK_H