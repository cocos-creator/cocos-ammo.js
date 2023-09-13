#include "btCharacterController.h"

#include <stdio.h>
#include "LinearMath/btIDebugDraw.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletCollision/CollisionShapes/btMultiSphereShape.h"
#include "BulletCollision/BroadphaseCollision/btOverlappingPairCache.h"
#include "BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h"
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "BulletCollision/CollisionShapes/btCapsuleShape.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "LinearMath/btDefaultMotionState.h"
#include "BulletCollision/CollisionShapes/btCompoundShape.h"

#define MAX_ITER 10
#define BULLET_CharacterController_DEBUG_LOG 0

static bool needsCollision(const btCollisionObject* body0, const btCollisionObject* body1) {
	bool collides = (body0->getBroadphaseHandle()->m_collisionFilterGroup & body1->getBroadphaseHandle()->m_collisionFilterMask) != 0;
	collides = collides && (body1->getBroadphaseHandle()->m_collisionFilterGroup & body0->getBroadphaseHandle()->m_collisionFilterMask);
	return collides;
}

static bool testSlope(const btVector3& normal, const btVector3& upDirection, btScalar slopeLimit) {
	const float dp = normal.dot(upDirection);
	return (dp >= 0.0f) && (dp < slopeLimit);
}

static void computeReflexionVector(btVector3& reflected, const btVector3& incomingDir, const btVector3& outwardNormal) {
	reflected = incomingDir - outwardNormal * 2.0f * (incomingDir.dot(outwardNormal));
}

static void decomposeVector(btVector3& normalCompo, btVector3& tangentCompo, const btVector3& outwardDir, const btVector3& outwardNormal) {
	normalCompo = outwardNormal * (outwardDir.dot(outwardNormal));
	tangentCompo = outwardDir - normalCompo;
}

//static float getShapeHalfHeight(btConvexShape *convexShape) {
//	if (convexShape->getShapeType() == CAPSULE_SHAPE_PROXYTYPE) {
//		btCapsuleShape* capsule = (btCapsuleShape*)convexShape;
//		return capsule->getRadius() + capsule->getFullHalfHeight();
//	}
//	else if (convexShape->getShapeType() == BOX_SHAPE_PROXYTYPE) {
//		return ((btBoxShape*)convexShape)->getHalfExtentsWithoutMargin().y();
//	}
//}

///@todo Interact with dynamic objects,
///Ride kinematicly animated platforms properly
///More realistic (or maybe just a config option) falling
/// -> Should integrate falling velocity manually and use that in stepDown()
///Support jumping
///Support ducking
class btKinematicClosestNotMeRayResultCallback : public btCollisionWorld::ClosestRayResultCallback
{
public:
	btKinematicClosestNotMeRayResultCallback(btCollisionObject* me) : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
	{
		m_me = me;
	}

	virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
	{
		if (rayResult.m_collisionObject == m_me)
			return 1.0;

		return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
	}

protected:
	btCollisionObject* m_me;
};

class btKinematicClosestNotMeConvexResultCallback : public btCollisionWorld::ClosestConvexResultCallback
{
public:
	btKinematicClosestNotMeConvexResultCallback(const btVector3 &convexFromWorld, const btVector3 &convexToWorld, btCollisionObject* me)
		: btCollisionWorld::ClosestConvexResultCallback(convexFromWorld, convexToWorld), m_me(me)
	{
	}

	virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
	{
		if (convexResult.m_hitCollisionObject == m_me)
			return btScalar(1.0);

		if (!convexResult.m_hitCollisionObject->hasContactResponse())
			return btScalar(1.0);

		// const btCollisionShape* shape = convexResult.m_hitCollisionObject->getCollisionShape();
		// if (shape->isCompound()) {
		// 	if (convexResult.m_localShapeInfo->m_shapePart == -1) {
		// 		const int index = convexResult.m_localShapeInfo->m_triangleIndex;
		// 		m_hitCollisionShape = ((btCompoundShape*)(shape))->getChildShape(index);
		// 	}
		// 	else {//concave:TriangleMesh, HeightField
		// 		const int index = convexResult.m_localShapeInfo->m_shapePart;
		// 		m_hitCollisionShape = ((btCompoundShape*)(shape))->getChildShape(index);
		// 	}
		// }
		// else {
		// 	m_hitCollisionShape = shape;
		// }
		m_hitCollisionShape = convexResult.m_localShapeInfo->m_shapeTemp;

		btVector3 hitNormalWorld;
		if (normalInWorldSpace)
		{
			hitNormalWorld = convexResult.m_hitNormalLocal;
		}
		else
		{
			///need to transform normal into worldspace
			hitNormalWorld = convexResult.m_hitCollisionObject->getWorldTransform().getBasis() * convexResult.m_hitNormalLocal;
		}

		return ClosestConvexResultCallback::addSingleResult(convexResult, normalInWorldSpace);
	}
	const btCollisionShape* m_hitCollisionShape{nullptr};

protected:
	btCollisionObject* m_me;
};

void btCharacterController::logMovementChange()
{
	if (BULLET_CharacterController_DEBUG_LOG) {
		btVector3 finalPos = m_ghostObject->getWorldTransform().getOrigin();
		btVector3 finalMovement = finalPos - m_backupPos;
		printf("initi pos \t %f %f %f\n", m_backupPos.x(), m_backupPos.y(), m_backupPos.z());
		printf("current pos \t %f %f %f\n", finalPos.x(), finalPos.y(), finalPos.z());
		printf("movement\t %f %f %f\n", finalMovement.x(), finalMovement.y(), finalMovement.z());
	}
}

btControllerCollisionFlag btCharacterController::move(const btVector3& disp, btScalar minDist, btScalar elapsedTime) {
	//if (disp.fuzzyZero()) { //can not do this, since even through disp is zero, cct still need to collide with env
	//	return m_prevCollisionFlag;
	//}

	if (BULLET_CharacterController_DEBUG_LOG) {
		printf("-------------------\n");
		printf("---- move  START --\n");
		printf("-------------------\n");
	}

	// 
	// todo
	//
	m_halfHeight = getFullHalfHeight();	// UBI
	m_bDidAutoStep = false;
	m_contactOffset = btMax(m_contactOffset, 0.01f);

	//moveCharacter
	//m_bCapsultCCTCollideGeomsUsingShapeBOX = true;
	if (BULLET_CharacterController_DEBUG_LOG) {
		printf("\n-------- PART 0: UP-SIDE-DOWN -----\n");
	}
	m_backupPos = m_ghostObject->getWorldTransform().getOrigin();
	btControllerCollisionFlag collisionFlag = moveCharacter(disp, minDist, elapsedTime);// , SweepTestFlag::STF_SWEEP_UP_SIDE_DOWN);
	if (BULLET_CharacterController_DEBUG_LOG) {
		printf("-------- PART 0: UP-SIDE-DOWN END -\n");
	}
	logMovementChange();

	if(m_bHitNonWalkable){
		if (BULLET_CharacterController_DEBUG_LOG) {
			printf("\n---- PART 1: WALK EXP:SIDE-DOWN ---\n");
		}
		walkExperiment = true;
		m_ghostObject->getWorldTransform().setOrigin(m_backupPos);
		collisionFlag = moveCharacter(disp, minDist, elapsedTime);// , SweepTestFlag::STF_SWEEP_SIDE_DOWN);
		walkExperiment = false;
		if (BULLET_CharacterController_DEBUG_LOG) {
			printf("---- PART 1: WALK EXP:SIDE-DOWN END\n");
		}
		logMovementChange();
	}

	//extra down movement for capsule cct if it did autostep with boxshape
	if(m_bDidAutoStep && getType() == btControllerShapeType::eCAPSULE && (collisionFlag & btControllerCollisionFlag::BULLET_CONTROLLER_COLLISION_DOWN))
	{
		if (BULLET_CharacterController_DEBUG_LOG) {
			printf("\n---- PART 2: Extra Down for CAPS --\n");
		}
		m_bOnlyDownTest = true;
		moveCharacter(disp, minDist, elapsedTime);// , SweepTestFlag::STF_SWEEP_SIDE_DOWN);
		m_bOnlyDownTest = false;
		if (BULLET_CharacterController_DEBUG_LOG) {
			printf("---- PART 2: Extra Down for CAPS END\n");
		}
		logMovementChange();
	}

	if (BULLET_CharacterController_DEBUG_LOG) {
		printf("-------------------\n");
		printf("----- move END ----\n");
		printf("-------------------\n\n\n\n");
	}
	m_prevCollisionFlag = collisionFlag;
	return collisionFlag;
}

btControllerCollisionFlag btCharacterController::moveCharacter(const btVector3& disp, btScalar minDist, btScalar elapsedTime) {
	bool standingOnMovingUp = false;//todo
	m_bHitNonWalkable = false;
	btControllerCollisionFlag collisionFlag = (btControllerCollisionFlag)0;
	//btVector3& temp = m_ghostObject->getWorldTransform().getOrigin();

	//btVector3 currentPosition = m_ghostObject->getWorldTransform().getOrigin();

	const int maxIter = MAX_ITER;	// 1 for "collide and stop"
	const int maxIterUp = maxIter;
	const int maxIterSides = maxIter;
	const int maxIterDown = 1;

	// Save initial height
	const btVector3& upDirection = m_up;
	//const btScalar originalHeight = currentPosition.dot(upDirection);
    //const btScalar originalBottomPoint = originalHeight - m_halfHeight;	// UBI

	btVector3 UpVector(0.0f, 0.0f, 0.0f);
	btVector3 DownVector(0.0f, 0.0f, 0.0f);

	btVector3 normal_compo, tangent_compo;
	decomposeVector(normal_compo, tangent_compo, disp, upDirection);

	const float dir_dot_up = disp.dot(upDirection);
	if(dir_dot_up <= 0.0f) DownVector = normal_compo;
	else UpVector = normal_compo;

	btVector3 SideVector = tangent_compo;
	const bool sideVectorIsZero = !standingOnMovingUp && SideVector.fuzzyZero();
	if (!sideVectorIsZero) {
		m_bDidAutoStep = true;
	}
	else {
		m_bDidAutoStep = false;
	}

	if (BULLET_CharacterController_DEBUG_LOG) {
		printf("------moveCharacter------\n");
		printf("moveCharacter disp\t \n%f %f %f\n", disp.x(), disp.y(), disp.z());
	}

	//UP
	//doSweepTest
	if(!walkExperiment && !m_bOnlyDownTest){//(mask & SweepTestFlag::STF_SWEEP_UP) {
		if (!sideVectorIsZero) {
			UpVector += upDirection * m_stepHeight;
		}
		if (!UpVector.fuzzyZero()) {
			if(BULLET_CharacterController_DEBUG_LOG) printf("");
		}
		m_bCapsultCCTCollideGeomsUsingShapeBOX = false;
		if(doSweepTest(UpVector, minDist, SweepPass::SWEEP_PASS_UP, maxIterUp)) {
			collisionFlag = btControllerCollisionFlag(collisionFlag | btControllerCollisionFlag::BULLET_CONTROLLER_COLLISION_UP);
			if (BULLET_CharacterController_DEBUG_LOG)
			printf("doSweepTest UP collide\n");
		}
	}

	//SIDE
	//doSweepTest
	if(1 && !m_bOnlyDownTest){//(mask & SweepTestFlag::STF_SWEEP_SIDE) {
		if (!walkExperiment && getType() == btControllerShapeType::eCAPSULE) {
			m_bCapsultCCTCollideGeomsUsingShapeBOX = true;
		}
		if(doSweepTest(SideVector, minDist, SweepPass::SWEEP_PASS_SIDE, maxIterSides)) {
			collisionFlag = btControllerCollisionFlag(collisionFlag | btControllerCollisionFlag::BULLET_CONTROLLER_COLLISION_SIDES);
			if (BULLET_CharacterController_DEBUG_LOG)
			printf("doSweepTest SIDE collide\n");
		}
		if (!walkExperiment && getType() == btControllerShapeType::eCAPSULE) {
			m_bCapsultCCTCollideGeomsUsingShapeBOX = false;
		}
	}

	//DOWN
	//doSweepTest
	if(1){//(mask & SweepTestFlag::STF_SWEEP_DOWN) {
		if(!sideVectorIsZero)		// We disabled that before so we don't have to undo it in that case
			DownVector -= upDirection * m_stepHeight;	// Undo our artificial up motion

		if (!walkExperiment && getType() == btControllerShapeType::eCAPSULE) {
			m_bCapsultCCTCollideGeomsUsingShapeBOX = true;
		}
		if (m_bOnlyDownTest || !m_bDidAutoStep) {
			m_bCapsultCCTCollideGeomsUsingShapeBOX = false;
		}

		if(doSweepTest(DownVector, minDist, SweepPass::SWEEP_PASS_DOWN, maxIterDown)) {
			collisionFlag = btControllerCollisionFlag(collisionFlag | btControllerCollisionFlag::BULLET_CONTROLLER_COLLISION_DOWN);
			if (BULLET_CharacterController_DEBUG_LOG)
			printf("doSweepTest DOWN collide\n");

			//slope
			if (BULLET_CharacterController_DEBUG_LOG)
			printf("doSweepTest DOWN testSlope\n");
			if (!m_bOnlyDownTest)
			{
				if (testSlope(mContactWorldNormal, m_up, btCos(m_maxSlopeRadians))) {
					//if(mContactPointHeight > originalBottomPoint + m_stepHeight) {
					if (BULLET_CharacterController_DEBUG_LOG) {
						printf("doSweepTest DOWN testSlope hit NonWalkable\n");
						printf("mContactWorldNormal %f %f %f\n", mContactWorldNormal.x(),
							mContactWorldNormal.y(), mContactWorldNormal.z());
					}
					m_bHitNonWalkable = true;


					if (!walkExperiment) {//not walk experiment
						if (!m_bOnlyDownTest && !walkExperiment && getType() == btControllerShapeType::eCAPSULE) {
							m_bCapsultCCTCollideGeomsUsingShapeBOX = false;
						}
						return collisionFlag;
					}
					else {//walk experiment
						//btVector3 currentPosition = m_ghostObject->getWorldTransform().getOrigin();
						//const float tmp = currentPosition.dot(upDirection);
						//float Delta = tmp > originalHeight ? float(tmp - originalHeight) : 0.0f;
						//Delta += fabsf(disp.dot(upDirection));
						//float Recover = Delta;

						//const float minDistance = Recover < minDist ? Recover / float(maxIter) : minDist;

						//btVector3 RecoverPoint(0, 0, 0);
						//RecoverPoint = -upDirection * Recover;

						//// PT: we pass "SWEEP_PASS_UP" for compatibility with previous code, but it's technically wrong (this is a 'down' pass)
						//if (doSweepTest(RecoverPoint, minDistance, SWEEP_PASS_UP, maxIter))
						//{

						//}
					}
					//}
				}
			}
		}
		if (!walkExperiment && getType() == btControllerShapeType::eCAPSULE) {
			m_bCapsultCCTCollideGeomsUsingShapeBOX = false;
		}
	}

	if (BULLET_CharacterController_DEBUG_LOG) {
		printf("------moveCharacter END------\n");
	}
	return collisionFlag;
}

bool btCharacterController::doSweepTest(const btVector3& disp, btScalar minDist, SweepPass sweepPass, int maxIter) {

	if(disp.fuzzyZero()){
		return false;
	}

	bool hasMoved = false;
	bool hasCollided = false;

	btVector3 currentPosition = m_ghostObject->getWorldTransform().getOrigin();
	btVector3 targetPosition = currentPosition;
	targetPosition += disp;
	if (BULLET_CharacterController_DEBUG_LOG) {
		printf("------doSweepTest------\n");
		printf("doSweepTest SweepPass\t %s\n", gStrSweepPass[(int)sweepPass].c_str());
		printf("doSweepTest currentPosition\t \n%f %f %f\n", currentPosition.x(), currentPosition.y(), currentPosition.z());
		printf("doSweepTest disp\t \n%f %f %f\n", disp.x(), disp.y(), disp.z());
	}
	while(maxIter--) {
		if(BULLET_CharacterController_DEBUG_LOG)
		printf("doSweepTest iteration\t %d\n", maxIter);

		// Compute current direction
		btVector3 currentDirection = targetPosition - currentPosition;
		const float Length = currentDirection.length();
		if(Length <= minDist) //Use <= to handle the case where min_dist is zero.
			break;

		currentDirection /= Length;

		// From Quake2: "if velocity is against the original velocity, stop dead to avoid tiny occilations in sloping corners"
		if((currentDirection.dot(disp)) <= 0.0f)
			break;

		// From this point, we're going to update the position at least once
		hasMoved = true;

		//
		// collideGeoms : Find closest collision
		//
		btSweptContact sweepContact;
		sweepContact.mDistance = Length + m_contactOffset;
		if(!collideGeoms( currentPosition, targetPosition, sweepContact)) {
			// no collision found => move to desired position
			hasCollided = false;
			currentPosition = targetPosition;
			if (BULLET_CharacterController_DEBUG_LOG)
			printf("doSweepTest no collide, go to targetPos \n%f %f %f\n", targetPosition.x(), targetPosition.y(), targetPosition.z());
			break;
		}
		else {
			if (BULLET_CharacterController_DEBUG_LOG)
			printf("doSweepTest collide\n");
			hasCollided = true;
		}

		//
		// emit collision events
		//

		if (m_userControllerHitReport ) {
			if (sweepContact.mColliderObject->getInternalType() & btCollisionObject::CO_RIGID_BODY) {
				btControllerShapeHit hit;
				hit.controller = this;
				hit.mCollisionShape = sweepContact.mColliderShape;
				hit.mCollisionObject = sweepContact.mColliderObject;
				hit.worldNormal = sweepContact.mWorldNormal;
				hit.worldPos = sweepContact.mWorldPos;
				hit.dir = currentDirection;
				hit.length = Length;
				if(sweepContact.mColliderShape)//todo: sometimes it's null
					m_userControllerHitReport->onShapeHit(hit);
				else {
					if (BULLET_CharacterController_DEBUG_LOG)
						printf("doSweepTest sweepContact.mColliderShape is null\n");
				}	
			}
			else if (sweepContact.mColliderObject->getInternalType() & btCollisionObject::CO_GHOST_OBJECT) {//seting CCT as CF_NO_CONTACT_RESPONSE makes this impossible
				void* userptr = sweepContact.mColliderObject->getUserPointer();
				if (userptr) {
					btCharacterController* otherCharacter = (btCharacterController*)(userptr);
					btControllersHit hit;
					hit.controller = this;
					hit.other = otherCharacter;
					hit.worldNormal = sweepContact.mWorldNormal;
					hit.worldPos = sweepContact.mWorldPos;
					hit.dir = currentDirection;
					hit.length = Length;
					m_userControllerHitReport->onControllerHit(hit);
				}
			}
		}
		

		//
		// recover From penetration/overlaps. 
		//
		if(m_bOverlapRecovery && sweepContact.mDistance == 0.0f) { //overlaps
			if (BULLET_CharacterController_DEBUG_LOG)
			printf("doSweepTest has penetration\n");
			btVector3& temp = m_ghostObject->getWorldTransform().getOrigin();
			if (BULLET_CharacterController_DEBUG_LOG)
			printf("recoverFromPenetration before \n%f %f %f\n", temp.x(), temp.y(), temp.z());
			for (int t = 0; t < 4; t++) {
				//recoverFromPenetration directly updates ghostObject transform
				recoverFromPenetration();
			}
			if (BULLET_CharacterController_DEBUG_LOG)
			printf("recoverFromPenetration after  \n%f %f %f\n", temp.x(), temp.y(), temp.z());

			//return hasMoved;
			return hasCollided;
		}
		if (BULLET_CharacterController_DEBUG_LOG)
		printf("doSweepTest doesn't have penetration\n");

		if (sweepContact.mDistance - m_contactOffset > 0) {
			currentPosition += currentDirection * (sweepContact.mDistance - m_contactOffset);
			if (BULLET_CharacterController_DEBUG_LOG)
				printf("doSweepTest update currentPosition to \n%f %f %f\n", currentPosition.x(), currentPosition.y(), currentPosition.z());
		}
		//
		//	
		//	
		mContactPointHeight = sweepContact.mWorldPos.dot(m_up);	// UBI
		mContactWorldPos = sweepContact.mWorldPos;
		mContactWorldNormal = sweepContact.mWorldNormal;

		if (walkExperiment) {//walk experiment needs to cancel out vertical hit normal
			btVector3 normalCompo, tangentCompo;
			decomposeVector(normalCompo, tangentCompo, mContactWorldNormal, m_up);
			if (tangentCompo.fuzzyZero()) {
				if (BULLET_CharacterController_DEBUG_LOG) {
					printf("sweepContact.mWorldNormal zero %f %f %f\n", tangentCompo.x(),
						tangentCompo.y(), tangentCompo.z());
				}
				return hasCollided;
			}
			mContactWorldNormal = tangentCompo;
			mContactWorldNormal.normalize();
		}

		//
		// collisionResponse
		//
		//btVector3 WorldNormal = sweepContact.mWorldNormal;
		const float Friction = 1.0f;
		collisionResponse(currentPosition, targetPosition, currentDirection, mContactWorldNormal, Friction, false);
		if (BULLET_CharacterController_DEBUG_LOG)
		printf("doSweepTest collisionResponse update targetPosition to\n%f %f %f\n", targetPosition.x(), targetPosition.y(), targetPosition.z());

	}

	// Final box position that should be reflected in the graphics engine
	m_ghostObject->getWorldTransform().setOrigin(currentPosition);

	// If we didn't move, don't update the box position at all (keeping possible lazy-evaluated structures valid)
	//return hasMoved;
	return hasCollided;
}

bool btCharacterController::collideGeoms(const btVector3& currentPosition, const btVector3& targetPosition, btSweptContact& sweepContact) {
	bool hasCollide = false;
	
	//btConvexShape *convex_shape_test(static_cast<btConvexShape *>(m_ghostObject->getCollisionShape()));
	btConvexShape *convex_shape_test(m_convexShape);
	if (m_bCapsultCCTCollideGeomsUsingShapeBOX) {
		if (BULLET_CharacterController_DEBUG_LOG)
			printf("collideGeoms using Box Shape\n");
		convex_shape_test = m_convexShapeBOX;
	}
	else {
		if (BULLET_CharacterController_DEBUG_LOG)
			printf("collideGeoms using Calsule Shape\n");
	}
	btScalar targetSweepDistance = sweepContact.mDistance;
	btTransform shape_world_from = btTransform(btQuaternion::getIdentity(), currentPosition);
	//btTransform shape_world_to = btTransform(btQuaternion(), targetPosition);//currentPosition + currentDirection * sweepContact.mDistance);
	btTransform shape_world_to = btTransform(btQuaternion::getIdentity(), currentPosition + (targetPosition - currentPosition).normalize() * targetSweepDistance);

	btKinematicClosestNotMeConvexResultCallback btResult(currentPosition, targetPosition, m_ghostObject);
	btResult.m_collisionFilterGroup = m_ghostObject->getBroadphaseHandle()->m_collisionFilterGroup;
	btResult.m_collisionFilterMask = m_ghostObject->getBroadphaseHandle()->m_collisionFilterMask;

	m_collisionWorld->convexSweepTest(convex_shape_test, shape_world_from, shape_world_to, btResult, m_collisionWorld->getDispatchInfo().m_allowedCcdPenetration);

	if (btResult.hasHit()) {
		hasCollide = true;
		
		//btScalar mtd = (btResult.m_hitPointWorld - currentPosition).length();
		//btScalar mtd = (targetPosition - currentPosition).length() * btResult.m_closestHitFraction;
		btScalar mtd = targetSweepDistance * btResult.m_closestHitFraction;
		sweepContact.mWorldPos = btResult.m_hitPointWorld;
		sweepContact.mWorldNormal = btResult.m_hitNormalWorld;
		if (sweepContact.mWorldNormal.fuzzyZero()) {
			if (BULLET_CharacterController_DEBUG_LOG)
			printf("sweepContact.mWorldNormal zero %f %f %f\n", sweepContact.mWorldNormal.x(), sweepContact.mWorldNormal.y(), sweepContact.mWorldNormal.z());
		}
		sweepContact.mDistance = mtd;
		if (BULLET_CharacterController_DEBUG_LOG)
		printf("mtd %f\n", mtd);
		sweepContact.mColliderObject = (btCollisionObject*)btResult.m_hitCollisionObject;
		sweepContact.mColliderShape = (btCollisionShape*)btResult.m_hitCollisionShape;

	}	
	return hasCollide;
}

bool btCharacterController::recoverFromPenetration()
{
	bool penetration = false;
	btVector3& currentPosition = m_ghostObject->getWorldTransform().getOrigin();

	// Here we must refresh the overlapping paircache as the penetrating movement itself or the
	// previous recovery iteration might have used setWorldTransform and pushed us into an object
	// that is not in the previous cache contents from the last timestep, as will happen if we
	// are pushed into a new AABB overlap. Unhandled this means the next convex sweep gets stuck.
	//
	// Do this by calling the broadphase's setAabb with the moved AABB, this will update the broadphase
	// paircache and the ghostobject's internal paircache at the same time.    /BW

	btVector3 minAabb, maxAabb;
	m_convexShape->getAabb(m_ghostObject->getWorldTransform(), minAabb, maxAabb);
	m_collisionWorld->getBroadphase()->setAabb(m_ghostObject->getBroadphaseHandle(),
								minAabb, maxAabb, m_collisionWorld->getDispatcher());

	m_collisionWorld->getDispatcher()->dispatchAllCollisionPairs(m_ghostObject->getOverlappingPairCache(), 
								m_collisionWorld->getDispatchInfo(), m_collisionWorld->getDispatcher());

	///keep track of the contact manifolds
	btManifoldArray m_manifoldArray;
	for (int i = 0; i < m_ghostObject->getOverlappingPairCache()->getNumOverlappingPairs(); i++)
	{
		m_manifoldArray.resize(0);
		btBroadphasePair* collisionPair = &m_ghostObject->getOverlappingPairCache()->getOverlappingPairArray()[i];
		btCollisionObject* obj0 = static_cast<btCollisionObject*>(collisionPair->m_pProxy0->m_clientObject);
		btCollisionObject* obj1 = static_cast<btCollisionObject*>(collisionPair->m_pProxy1->m_clientObject);
		//if ((obj0 && !obj0->hasContactResponse()) || (obj1 && !obj1->hasContactResponse()))
		//	continue;

		if (!needsCollision(obj0, obj1))
			continue;

		if (collisionPair->m_algorithm)
			collisionPair->m_algorithm->getAllContactManifolds(m_manifoldArray);

		for (int j = 0; j < m_manifoldArray.size(); j++) {
			btPersistentManifold* manifold = m_manifoldArray[j];
			btScalar directionSign = manifold->getBody0() == m_ghostObject ? btScalar(-1.0) : btScalar(1.0);
			for (int p = 0; p < manifold->getNumContacts(); p++) {
				const btManifoldPoint& pt = manifold->getContactPoint(p);
				btScalar dist = pt.getDistance();
				currentPosition += pt.m_normalWorldOnB * directionSign * dist;// *btScalar(0.2);
				penetration = true;
			}
		}
	}
	return penetration;
}


// bool btCharacterController::computeMTD(const btVector3& currentPosition, const btVector3& targetPosition, btSweptContact& sweepContact) {
// 	btVector3 p = currentPosition;
// 	btVector3 mtd;
// 	btScalar depth;
// 	const PxTransform volumePose(p, sweep_test->mUserParams.mQuatFromUp);
// 	if(volume.getType()==SweptVolumeType::eCAPSULE)
// 	{
// 		const SweptCapsule& sc = static_cast<const SweptCapsule&>(volume);
// 		const PxCapsuleGeometry capsuleGeom(sc.mRadius+contactOffset, sc.mHeight*0.5f);
// 		isValid = PxGeometryQuery::computePenetration(mtd, depth, capsuleGeom, volumePose, gh.any(), globalPose);
// 	}
// 	else
// 	{
// 		PX_ASSERT(volume.getType()==SweptVolumeType::eBOX);
// 		const SweptBox& sb = static_cast<const SweptBox&>(volume);
// 		const PxBoxGeometry boxGeom(sb.mExtents+PxVec3(contactOffset));
// 		isValid = PxGeometryQuery::computePenetration(mtd, depth, boxGeom, volumePose, gh.any(), globalPose);
// 	}
// }

void btCharacterController::collisionResponse(const btVector3& currentPosition, btVector3& targetPosition, 
const btVector3& currentDirection, const btVector3& hitNormal, btScalar friction, bool normalize){
	// Compute reflect direction
	btVector3 reflectDir;
	computeReflexionVector(reflectDir, currentDirection, hitNormal);
	reflectDir.normalize();

	// Decompose it
	btVector3 normalCompo, tangentCompo;
	decomposeVector(normalCompo, tangentCompo, reflectDir, hitNormal);

	// Compute new destination position
    const float amplitude = (targetPosition - currentPosition).length();
    
	targetPosition = currentPosition; 
	if(friction != 0.0f) {
		if(normalize)
			tangentCompo.normalize();
        targetPosition += tangentCompo*friction*amplitude;
	}
}

//btCharacterController::btCharacterController(btCollisionWorld* collisionWorld, btPairCachingGhostObject* ghostObject, 
//	btConvexShape* convexShape, btScalar stepHeight, btScalar contactOffset, btScalar masSlopeRadians, const btVector3& up,
//	btUserControllerHitReport* userControllerHitReport, void* userObjectPointer) {
//	m_collisionWorld = collisionWorld;
//	m_ghostObject = ghostObject;
//	m_up = up;
//	m_convexShape = convexShape;
//
//	setStepHeight(stepHeight);
//	setContactOffset(contactOffset);
//	setMaxSlope(masSlopeRadians);
//	m_userControllerHitReport = userControllerHitReport;
//	m_userObjectPointer = userObjectPointer;
//}

btCharacterController::btCharacterController(btCollisionWorld* collisionWorld, btCharacterControllerDesc* desc, void* userObjectPointer) {
	m_collisionWorld = collisionWorld;
	m_userObjectPointer = userObjectPointer;

	mType = desc->mType;
	setUp(desc->m_up);
	setStepHeight(desc->m_stepHeight);
	setContactOffset(desc->m_contactOffset);
	setMaxSlope(desc->m_maxSlopeRadians);
	m_userControllerHitReport = desc->m_userControllerHitReport;
	m_prevCollisionFlag = btControllerCollisionFlag(0);
}

btCharacterController::~btCharacterController() {
}

void btCharacterController::updateAction(btCollisionWorld* collisionWorld, btScalar deltaTimeStep) {
	if (m_bDetectCollisions) {
		for (int t = 0; t < 4; t++) {
			recoverFromPenetration();
		}
	}
}

void btCharacterController::setMaxSlope(btScalar slopeRadians) {
	m_maxSlopeRadians = slopeRadians;
}

btScalar btCharacterController::getMaxSlope() const {
	return m_maxSlopeRadians;
}

void btCharacterController::setPosition(const btVector3& pos) {
	m_ghostObject->getWorldTransform().setOrigin(pos);
}
const btVector3& btCharacterController::getPosition() {
	return m_ghostObject->getWorldTransform().getOrigin();
}

btCapsuleCharacterController::btCapsuleCharacterController(btCollisionWorld* collisionWorld, btCapsuleCharacterControllerDesc* desc, void* userObjectPointer)
	:btCharacterController(collisionWorld, desc, userObjectPointer) {
	m_fRadius = desc->m_fRadius;
	m_fHeight = desc->m_fHeight;
	m_ghostObject = new btPairCachingGhostObject();
	m_ghostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	m_ghostObject->getWorldTransform().setOrigin(desc->m_initPos);
	m_ghostObject->setUserPointer(this);

	ResetShape();
}

btScalar btCapsuleCharacterController::getFullHalfHeight() {
	return m_fRadius + m_fHeight * 0.5f;
}

void btCapsuleCharacterController::ResetShape() {
	if(!m_convexShape) {
		m_convexShape = new btCapsuleShape(m_fRadius, m_fHeight);
	}else{
		((btCapsuleShape*)m_convexShape)->updateProp(m_fRadius, m_fHeight * 0.5, 1/*upAxis*/);
	}
	
	m_ghostObject->setCollisionShape(m_convexShape);

	// if (m_convexShapeBOX) {
	// 	delete m_convexShapeBOX;
	// 	m_convexShapeBOX = NULL;
	// }
	btVector3 halfExtent(m_fRadius, getFullHalfHeight(), m_fRadius);
	// m_convexShapeBOX = new btBoxShape(halfExtent);
	if(!m_convexShapeBOX) {
	 m_convexShapeBOX = new btBoxShape(halfExtent);
	} else {
		((btBoxShape*)m_convexShapeBOX)->setUnscaledHalfExtents(halfExtent);
	}
}

void btCapsuleCharacterController::setRadius(btScalar radius) {
	m_fRadius = radius;
	ResetShape();
}

void btCapsuleCharacterController::setHeight(btScalar height) {
	m_fHeight = height;
	ResetShape();
}

btBoxCharacterController::btBoxCharacterController(btCollisionWorld* collisionWorld, btBoxCharacterControllerDesc* desc, void* userObjectPointer)
	:btCharacterController(collisionWorld, desc, userObjectPointer) {
	m_fHalfSideExtent = desc->m_fHalfSideExtent;
	m_fHalfHeight = desc->m_fHalfHeight;
	m_fHalfForwardExtent = desc->m_fHalfForwardExtent;
	m_ghostObject = new btPairCachingGhostObject();
	m_ghostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	m_ghostObject->getWorldTransform().setOrigin(desc->m_initPos);
	m_ghostObject->setUserPointer(this);

	ResetShape();
}

void btBoxCharacterController::ResetShape() {
	btVector3 halfExtent(m_fHalfSideExtent, m_fHalfHeight, m_fHalfForwardExtent);
	if(!m_convexShape) {
	 m_convexShape = new btBoxShape(halfExtent);
	} else {
		((btBoxShape*)m_convexShape)->setUnscaledHalfExtents(halfExtent);
	}

	m_ghostObject->setCollisionShape(m_convexShape);
}

btScalar btBoxCharacterController::getFullHalfHeight() {
	btBoxShape* box = (btBoxShape*)m_convexShape;
	return box->getHalfExtentsWithoutMargin().y();
}

void btBoxCharacterController::setHalfHeight(btScalar halfHeight) {
	m_fHalfHeight = halfHeight;
	ResetShape();
}

void btBoxCharacterController::setHalfSideExtent(btScalar halfSideExtent) {
	m_fHalfSideExtent = halfSideExtent;
	ResetShape();
}

void btBoxCharacterController::setHalfForwardExtent(btScalar halfForwardExtent) {
	m_fHalfForwardExtent = halfForwardExtent;
	ResetShape();
}