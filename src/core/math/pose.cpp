#include "pose.h"

using namespace OSG;

pose::pose() {}
pose::pose(Vec3f p, Vec3f d, Vec3f u) { set(p,d,u); }

void pose::set(Vec3f p, Vec3f d, Vec3f u) {
    data.resize(3);
    data[0] = p;
    data[1] = d;
    data[2] = u;
}

Vec3f pose::pos() { return data.size() > 0 ? data[0] : Vec3f(); }
Vec3f pose::dir() { return data.size() > 1 ? data[1] : Vec3f(); }
Vec3f pose::up() { return data.size() > 2 ? data[2] : Vec3f(); }