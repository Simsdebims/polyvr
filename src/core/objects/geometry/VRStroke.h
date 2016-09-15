#ifndef VRStroke_H_INCLUDED
#define VRStroke_H_INCLUDED

#include "VRGeometry.h"
#include "core/objects/VRObjectFwd.h"

OSG_BEGIN_NAMESPACE;
using namespace std;

class path;

class VRStroke : public VRGeometry {
    public:
        enum CAP {
            NONE,
            ARROW
        };

    private:
        vector<path*> paths;

        int mode = -1;
        vector<Vec3f> profile;
        bool closed = false;
        bool doColor = true;

        CAP cap_beg = NONE;
        CAP cap_end = NONE;

        VRGeometryPtr strewGeo = 0;

    public:
        VRStroke(string name);

        static VRStrokePtr create(string name = "None");
        VRStrokePtr ptr();

        void setPath(path* p);
        void addPath(path* p);
        void setPaths(vector<path*> p);

        vector<path*>& getPaths();

        void strokeProfile(vector<Vec3f> profile, bool closed, bool doColor = true, CAP l = NONE, CAP r = NONE);
        void strokeStrew(VRGeometryPtr geo);

        void update();
};

OSG_END_NAMESPACE;

#endif // VRStroke_H_INCLUDED
