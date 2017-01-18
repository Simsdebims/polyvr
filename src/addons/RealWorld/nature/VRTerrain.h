#ifndef VRTERRAIN_H_INCLUDED
#define VRTERRAIN_H_INCLUDED

#include <OpenSG/OSGVector.h>
#include "core/objects/geometry/VRGeometry.h"
#include "addons/RealWorld/VRRealWorldFwd.h"

using namespace std;
OSG_BEGIN_NAMESPACE;

class VRTerrain : public VRGeometry {
    private:
        static string vertexShader;
        static string fragmentShader;
        static string tessControlShader;
        static string tessEvaluationShader;

        Vec2f size = Vec2f(100,100);
        float resolution = 1;
        float grid = 64;
        VRTexturePtr tex;
        VRMaterialPtr mat;

        void setupGeo();
        void setupMat();

    public:
        VRTerrain(string name);
        ~VRTerrain();
        static VRTerrainPtr create(string name);

        void setParameters( Vec2f size, float resolution );
        void setMap( VRTexturePtr tex );
};

/* TODO:
- physics, custom callback?
*/


OSG_END_NAMESPACE;

#endif // VRTERRAIN_H_INCLUDED