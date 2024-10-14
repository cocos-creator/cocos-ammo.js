
#ifndef CC_CONVEX_CAST_CALLBACK_H
#define CC_CONVEX_CAST_CALLBACK_H

#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "ccDiscreteDynamicsWorld.h"

namespace cc 
{
typedef btCollisionWorld::ClosestConvexResultCallback ClosestConvexResultCallback;
typedef btCollisionWorld::AllHitsConvexResultCallback AllHitsConvexResultCallback;
typedef btCollisionWorld::LocalConvexResult LocalConvexResult;

struct ccClosestConvexResultCallback : public ClosestConvexResultCallback
{
	int m_shapeUserPointer;
	bool m_queryTrigger;

	ccClosestConvexResultCallback(const btVector3&	rayFromWorld,const btVector3&	rayToWorld)
	:ClosestConvexResultCallback(rayFromWorld, rayToWorld), m_shapeUserPointer(0), m_queryTrigger(true)
	{
		m_collisionFilterGroup = -1;
	}

	// return true when pairs need collision
	virtual bool needsCollision(btBroadphaseProxy* proxy0) const
	{
		if ((proxy0->m_collisionFilterGroup & m_collisionFilterMask) != 0) {			
			if (!m_queryTrigger && proxy0->m_clientObject) {
				btCollisionObject* co = static_cast<btCollisionObject*>(proxy0->m_clientObject);
				return co->hasContactResponse();
			}
			return true;
		} else {
			return false;
		}
	}
	
	virtual	btScalar	addSingleResult(LocalConvexResult& convexResult,bool normalInWorldSpace)
	{
		// const btCollisionShape* shape = convexResult.m_hitCollisionObject->getCollisionShape();
		// if(shape->isCompound() && convexResult.m_localShapeInfo) {
		// 	const btCompoundShape* compound = static_cast<const btCompoundShape*>(shape);
		// 	const int index = convexResult.m_localShapeInfo->m_shapePart;
		// 	m_shapeUserPointer = compound->getChildShape(index)->getUserPointerAsInt();
		// } else {
		// 	m_shapeUserPointer = shape->getUserPointerAsInt();
		// }
		m_shapeUserPointer = convexResult.m_localShapeInfo->m_shapeTemp->getUserPointerAsInt();
		return ClosestConvexResultCallback::addSingleResult(convexResult, normalInWorldSpace);
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
		m_hitCollisionObject = 0;
	}
};

struct ccAllHitsConvexResultCallback : public AllHitsConvexResultCallback
{
	btAlignedObjectArray<int> m_shapeUserPointers;
	bool m_queryTrigger;

	ccAllHitsConvexResultCallback(const btVector3&	rayFromWorld,const btVector3&	rayToWorld)
	:AllHitsConvexResultCallback(rayFromWorld, rayToWorld), m_queryTrigger(true)
	{
		m_collisionFilterGroup = -1;
	}

	// return true when pairs need collision
	virtual bool needsCollision(btBroadphaseProxy* proxy0) const
	{
		if ((proxy0->m_collisionFilterGroup & m_collisionFilterMask) != 0) {			
			if (!m_queryTrigger && proxy0->m_clientObject) {
				btCollisionObject* co = static_cast<btCollisionObject*>(proxy0->m_clientObject);
				return co->hasContactResponse();
			}
			return true;
		} else {
			return false;
		}
	}
	
	virtual	btScalar	addSingleResult(LocalConvexResult& convexResult,bool normalInWorldSpace)
	{
		// const btCollisionShape* shape = rayResult.m_hitCollisionObject->getCollisionShape();
		// if(shape->isCompound() && rayResult.m_localShapeInfo) {
		// 	const btCompoundShape* compound = static_cast<const btCompoundShape*>(shape);
		// 	const int index = rayResult.m_localShapeInfo->m_shapePart;
		// 	m_shapeUserPointers.push_back(compound->getChildShape(index)->getUserPointerAsInt());
		// } else {
		// 	m_shapeUserPointers.push_back(shape->getUserPointerAsInt());
		// }
		m_shapeUserPointers.push_back(convexResult.m_localShapeInfo->m_shapeTemp->getUserPointerAsInt());
		return AllHitsConvexResultCallback::addSingleResult(convexResult, normalInWorldSpace);
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

	btScalar getClosestHitFraction ()
	{
		return m_closestHitFraction;
	}

	void reset(int mask, bool queryTrigger)
	{
		m_collisionFilterMask = mask;
		m_queryTrigger = queryTrigger;
		m_closestHitFraction = btScalar(1.);
		//m_collisionObject = 0;
		m_shapeUserPointers.resize(0);
		m_hitFractions.resize(0);
		m_collisionObjects.resize(0);
		m_hitPointWorld.resize(0);
		m_hitNormalWorld.resize(0);
	}
};

struct ccNotMeClosestConvexResultCallback : public ClosestConvexResultCallback {

public:
  const btCollisionShape *m_hit;
  btCollisionObject *m_me;
  ccDiscreteDynamicsWorld *m_world;

  ccNotMeClosestConvexResultCallback(btCollisionObject *me,
                                     ccDiscreteDynamicsWorld *world,
                                     const btVector3 &fromA,
                                     const btVector3 &toA)
      : btCollisionWorld::ClosestConvexResultCallback(fromA, toA), m_hit(0),
      m_me(me), m_world(world) {}

  virtual btScalar
  addSingleResult(btCollisionWorld::LocalConvexResult &convexResult,
                  bool normalInWorldSpace) {
    m_hit = convexResult.m_localShapeInfo->m_shapeTemp;
    return ClosestConvexResultCallback::addSingleResult(convexResult,
                                                        normalInWorldSpace);
  }

  virtual bool needsCollision(btBroadphaseProxy *proxy0) const {
    // don't collide with itself
    if (proxy0->m_clientObject == m_me)
      return false;

    /// don't do CCD when the collision filters are not matching
    if (!ClosestConvexResultCallback::needsCollision(proxy0))
      return false;

    /// check response
    btCollisionObject* otherObj = (btCollisionObject*) proxy0->m_clientObject;
    if (!m_world->getDispatcher()->needsResponse(m_me,otherObj)){        
      return false;
    }

    return true;
  }
};
}
#endif // CC_CONVEX_CAST_CALLBACK_H
