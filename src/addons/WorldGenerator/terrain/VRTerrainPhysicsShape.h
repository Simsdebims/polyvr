#ifndef VRTERRAINPHYSICSSHAPE_H_INCLUDED
#define VRTERRAINPHYSICSSHAPE_H_INCLUDED

#include "../VRWorldGeneratorFwd.h"
#include "core/math/boundingbox.h"
#include <vector>
#include <memory>
#include <OpenSG/OSGConfig.h>
#include <BulletCollision/CollisionShapes/btConcaveShape.h>

using namespace std;
OSG_BEGIN_NAMESPACE;

ATTRIBUTE_ALIGNED16(class)  VRTerrainPhysicsShape : public btConcaveShape {
    protected:
        VRTerrainPtr terrain;
        Boundingbox boundingbox;
        double Hmax;
        int	m_heightStickWidth;
        int m_heightStickLength;
        btScalar m_width;
        btScalar m_length;

        btVector3 m_localScaling;
        void quantizeWithClamp(int* out, const btVector3& point) const;
        void getVertex(int x,int y,btVector3& vertex) const;
        void initialize();

    public:
        BT_DECLARE_ALIGNED_ALLOCATOR();

        VRTerrainPhysicsShape(VRTerrainPtr terrain);
        ~VRTerrainPhysicsShape();

        virtual void getAabb(const btTransform& t,btVector3& aabbMin,btVector3& aabbMax) const;
        virtual void processAllTriangles(btTriangleCallback* callback,const btVector3& aabbMin,const btVector3& aabbMax) const;
        virtual void calculateLocalInertia(btScalar mass,btVector3& inertia) const;
        virtual void setLocalScaling(const btVector3& scaling);
        virtual const btVector3& getLocalScaling() const;
        virtual const char*	getName()const {return "HEIGHTFIELD";}
};

OSG_END_NAMESPACE;

#endif // VRTERRAINPHYSICSSHAPE_H_INCLUDED
