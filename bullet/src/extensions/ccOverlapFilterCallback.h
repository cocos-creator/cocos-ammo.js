
#ifndef CC_OVERLAPPING_PAIR_CACHE_H
#define CC_OVERLAPPING_PAIR_CACHE_H

#include "BulletCollision/BroadphaseCollision/btOverlappingPairCache.h"
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"

namespace cc 
{

ATTRIBUTE_ALIGNED16(class) ccOverlapFilterCallback : public btOverlapFilterCallback
{
private:
    /* data */
public:
	
  virtual ~ccOverlapFilterCallback() {}

    // return true when pairs need collision
    virtual bool needBroadphaseCollision(btBroadphaseProxy *proxy0, btBroadphaseProxy *proxy1) const
    {
        bool collides = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) != 0;
        collides = collides && (proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask);

        //add some additional logic here that modified 'collides'
        if (collides)
        {
            btCollisionObject *co0 = (btCollisionObject *)proxy0->m_clientObject;
            btCollisionObject *co1 = (btCollisionObject *)proxy1->m_clientObject;
            
            if (co0->isStaticObject() && co1->isStaticObject())
                return false;

            if ((co0->getActivationState() == ISLAND_SLEEPING) && co1->getActivationState() == ISLAND_SLEEPING)
                return false;
        }
        return collides;
    }
};

}

#endif // CC_OVERLAPPING_PAIR_CACHE_H