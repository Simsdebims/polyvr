#ifndef VRSNAPPINGENGINE_H_INCLUDED
#define VRSNAPPINGENGINE_H_INCLUDED

#include <OpenSG/OSGVector.h>
#include <OpenSG/OSGMatrix.h>
#include <OpenSG/OSGLine.h>
#include <OpenSG/OSGPlane.h>
#include <map>
#include "core/utils/VRFunctionFwd.h"
#include "core/utils/VRDeviceFwd.h"
#include "core/objects/VRObjectFwd.h"


using namespace std;
OSG_BEGIN_NAMESPACE;

class Octree;
class VRSignal;
class VRDevice;


class VRSnappingEngine {
    private:
        struct Rule;

    public:
        enum PRESET {
            SIMPLE_ALIGNMENT,
            SNAP_BACK,
        };

        enum Type {
            NONE,
            POINT,
            LINE,
            PLANE,
            POINT_LOCAL,
            LINE_LOCAL,
            PLANE_LOCAL
        };

        struct EventSnap {
            int snap = 0;
            int snapID = 0;
            VRTransformPtr o1 = 0;
            VRTransformPtr o2 = 0;
            Matrix4d m;
            VRDevicePtr dev = 0;
            void set(VRTransformPtr O1, VRTransformPtr O2, Matrix4d M, VRDevicePtr DEV, int Snap, int SnapID) {
                o1 = O1; o2 = O2; m = M; dev = DEV; snap = Snap; snapID = SnapID;
            }
        };

        typedef VRFunction<EventSnap> VRSnapCb;
        typedef shared_ptr<VRSnapCb> VRSnapCbPtr;

    private:
        map<int, Rule*> rules; // snapping rules, translation and orientation
        map<VRTransformPtr, Matrix4d> objects; // map objects to reference matrix
        map<VRTransformPtr, vector<VRTransformPtr> > anchors; // object anchors
        vector<VRSnapCbPtr> callbacks; // object anchors
        Octree* positions = 0; // objects by positions
        VRGeometryPtr hintGeo = 0;
        VRUpdateCbPtr updatePtr;

        float influence_radius = 1000;
        float distance_snap = 0.05;
        bool showHints = false;

        EventSnap* event = 0;
        VRSignalPtr snapSignal = 0;

    public:
        VRSnappingEngine();
        ~VRSnappingEngine();
        static shared_ptr<VRSnappingEngine> create();

        VRSignalPtr getSignalSnap();

        void clear();

        Type typeFromStr(string t);

        void addCallback(VRSnapCbPtr cb);

        int addRule(Type t, Type o, Line pt, Line po, float d, float w = 1, VRTransformPtr l = 0);
        void remRule(int i);

        void addObjectAnchor(VRTransformPtr obj, VRTransformPtr a);
        void clearObjectAnchors(VRTransformPtr obj);
        void remLocalRules(VRTransformPtr obj);

        void addObject(VRTransformPtr obj, float weight = 1);
        void addTree(VRObjectPtr obj, float weight = 1);
        void remObject(VRTransformPtr obj);

        void setVisualHints(bool b = true);
        void setPreset(PRESET preset);

        void update();
};

OSG_END_NAMESPACE;

#endif // VRSNAPPINGENGINE_H_INCLUDED
