#ifndef VRSCENEGLOBALS_H_INCLUDED
#define VRSCENEGLOBALS_H_INCLUDED

#include <OpenSG/OSGConfig.h>
#include "VRPyBase.h"

OSG_BEGIN_NAMESPACE;

class VRSceneGlobals: public VRPyBase {
    private:
    public:
        VRSceneGlobals();

        static PyMethodDef methods[];

		static PyObject* exit(VRSceneGlobals* self);
		static PyObject* find(VRSceneGlobals* self, PyObject *args);
		static PyObject* loadGeometry(VRSceneGlobals* self, PyObject *args, PyObject *kwargs);
		static PyObject* getLoadGeometryProgress(VRSceneGlobals* self);
		static PyObject* exportGeometry(VRSceneGlobals* self, PyObject *args);
		static PyObject* pyTriggerScript(VRSceneGlobals* self, PyObject *args);
		static PyObject* stackCall(VRSceneGlobals* self, PyObject *args);
		static PyObject* openFileDialog(VRSceneGlobals* self, PyObject *args);
		static PyObject* updateGui(VRSceneGlobals* self);
		static PyObject* render(VRSceneGlobals* self);
		static PyObject* getRoot(VRSceneGlobals* self);
		static PyObject* printOSG(VRSceneGlobals* self);
		static PyObject* getNavigator(VRSceneGlobals* self);
		static PyObject* getSetup(VRSceneGlobals* self);
		static PyObject* loadScene(VRSceneGlobals* self, PyObject *args);
		static PyObject* startThread(VRSceneGlobals* self, PyObject *args);
		static PyObject* joinThread(VRSceneGlobals* self, PyObject *args);
		static PyObject* getSystemDirectory(VRSceneGlobals* self, PyObject *args);
		static PyObject* setPhysicsActive(VRSceneGlobals* self, PyObject *args);
};

OSG_END_NAMESPACE;

#endif // VRSCENEGLOBALS_H_INCLUDED