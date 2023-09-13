#pragma once

#include "LinearMath/btVector3.h"
#include "BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h"
#include "BulletDynamics/Dynamics/btActionInterface.h"

#include "btCharacterControllerDefs.h"

class btCollisionShape;
class btConvexShape;
class btCollisionWorld;
class btCollisionDispatcher;
class btPairCachingGhostObject;

class btCharacterController : public btActionInterface {
public:
	//btCharacterController(btCollisionWorld* collisionWorld, btPairCachingGhostObject * ghostObject,
	//	btConvexShape * convexShape, btScalar stepHeight, btScalar contactOffset, btScalar masSlopeRadians,
	//	const btVector3& up = btVector3(0.0, 1.0, 0.0), btUserControllerHitReport* userControllerHitReport = nullptr,
	//	void* userObjectPointer = nullptr);
	btCharacterController(btCollisionWorld* collisionWorld, btCharacterControllerDesc* desc, void* userObjectPointer);
	~btCharacterController();

	//btActionInterface
	virtual void updateAction(btCollisionWorld* collisionWorld, btScalar deltaTimeStep) override;
	virtual void debugDraw(btIDebugDraw* debugDrawer) override {};

	virtual		btControllerShapeType::Enum	getType() { return mType; };

	void logMovementChange();

	//
	// Move the Character with a displacement.
	// Normally call this func every frame with a displacement of velocity * deltaTime
	//
	btControllerCollisionFlag move(const btVector3& disp, btScalar minDist, btScalar elapsedTime);

	//
	//setters and getters
	//
	void setUp(const btVector3& up) { m_up = up; };
	const btVector3& getUp() { return m_up; }

	void setContactOffset(btScalar h) { m_contactOffset = h; }
	btScalar getContactOffset() const { return m_contactOffset; }

	void setStepHeight(btScalar h) { m_stepHeight = h; }
	btScalar getStepHeight() const { return m_stepHeight; }

	void setMaxSlope(btScalar slopeRadians);
	btScalar getMaxSlope() const;

	btPairCachingGhostObject* getGhostObject() { return m_ghostObject; };
	btCollisionShape* getCollisionShape() { return (btCollisionShape*)m_convexShape; };
	void* getUserPointer() { return m_userObjectPointer; };
	void setUserPointer(void* userPointer) { m_userObjectPointer = userPointer;}

	// radius + height * 0.5  //height is distance between two sphere center
	virtual btScalar getFullHalfHeight() = 0;

	void setPosition(const btVector3& pos);
	const btVector3& getPosition();

	void setCollision(bool v) { m_bDetectCollisions = v; };
	void setOverlapRecovery(bool v) { m_bOverlapRecovery = v; };

protected:
	btPairCachingGhostObject* m_ghostObject{ nullptr };
	btConvexShape* m_convexShape{nullptr};  //is also in m_ghostObject, but it needs to be convex, so we store it here to avoid upcast
	btCollisionWorld* m_collisionWorld;
	btScalar m_halfHeight;
	btUserControllerHitReport* m_userControllerHitReport{nullptr};
	void* m_userObjectPointer{ nullptr };

	// paramsters
	btScalar m_maxSlopeRadians;  // Slope angle that is set (used for returning the exact value)
	btScalar m_stepHeight;
	btScalar m_contactOffset;
	btVector3 m_up;
	btControllerShapeType::Enum mType;

	// status
	bool m_bHitNonWalkable;
	bool walkExperiment = false;
	bool m_bDetectCollisions = true;
	bool m_bOverlapRecovery = true;
	bool m_bOnlyDownTest = false;
	bool m_bDidAutoStep = false;

	// contact info
	btScalar mContactPointHeight;
	btVector3 mContactWorldNormal;
	btVector3 mContactWorldPos;

	//
	btConvexShape* m_convexShapeBOX = nullptr; //only used for autoStep SWEEP_PASS_SIDE
	bool m_bCapsultCCTCollideGeomsUsingShapeBOX{false};

	//keep track of the contact manifolds
	btManifoldArray m_manifoldArray;//todo

	btControllerCollisionFlag m_prevCollisionFlag;
	btVector3 m_backupPos;

	// internal functions called by move()
	btControllerCollisionFlag moveCharacter(const btVector3& disp, btScalar minDist, btScalar elapsedTime);
	bool doSweepTest(const btVector3& disp, btScalar minDist, SweepPass sweepPass, int maxIter);
	bool collideGeoms(const btVector3& currentPosition, const btVector3& targetPosition, btSweptContact& sweepContact);
	bool recoverFromPenetration();
	void collisionResponse(const btVector3& currentPosition, btVector3& targetPosition, const btVector3& currentDirection,
							const btVector3& hitNormal, btScalar friction, bool normalize);
};

class btCapsuleCharacterController : public btCharacterController {
public:
	btCapsuleCharacterController(btCollisionWorld* collisionWorld, btCapsuleCharacterControllerDesc* desc, void* userObjectPointer);
	virtual btScalar getFullHalfHeight() override;
	void setRadius(btScalar radius);
	void ResetShape();
	// distance between two sphere center
	void setHeight(btScalar height); 
private:
	float m_fRadius = 0.5;
	float m_fHeight = 1;
};

class btBoxCharacterController : public btCharacterController {
public:
	btBoxCharacterController(btCollisionWorld* collisionWorld, btBoxCharacterControllerDesc* desc, void* userObjectPointer);
	virtual btScalar getFullHalfHeight() override;

	void ResetShape();
	void setHalfHeight(btScalar halfHeight);
	void setHalfSideExtent(btScalar halfSideExtent);
	void setHalfForwardExtent(btScalar halfForwardExtent);
private:
	btScalar m_fHalfHeight = 0.5;
	btScalar m_fHalfSideExtent = 0.5;
	btScalar m_fHalfForwardExtent = 0.5;
};