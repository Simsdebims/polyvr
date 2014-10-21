#ifndef VRPYSPRITE_H_INCLUDED
#define VRPYSPRITE_H_INCLUDED

#include "VRPyBase.h"
#include "core/objects/geometry/VRSprite.h"

struct VRPySprite : VRPyBaseT<OSG::VRSprite> {
    static PyMemberDef members[];
    static PyMethodDef methods[];

    static PyObject* getText(VRPySprite* self);
    static PyObject* setText(VRPySprite* self, PyObject* args);
    static PyObject* setSize(VRPySprite* self, PyObject* args);
};

#endif // VRPYSPRITE_H_INCLUDED