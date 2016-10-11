#include "VRSceneModules.h"
#include "VRSceneGlobals.h"
#include "VRScriptManagerT.h"

#include "VRPyObject.h"
#include "VRPyGeometry.h"
#include "VRPyAnimation.h"
#include "VRPySocket.h"
#include "VRPySprite.h"
#include "VRPySound.h"
#include "VRPyDevice.h"
#include "VRPyIntersection.h"
#include "VRPyPose.h"
#include "VRPyPath.h"
#include "VRPyGraph.h"
#include "VRPyPolygon.h"
#include "VRPyTriangulator.h"
#include "VRPyStroke.h"
#include "VRPyColorChooser.h"
#include "VRPyTextureRenderer.h"
#include "VRPyConstraint.h"
#include "VRPyHaptic.h"
#include "VRPyMouse.h"
#include "VRPyMobile.h"
#include "VRPyBaseT.h"
#include "VRPyMaterial.h"
#include "VRPyTextureGenerator.h"
#include "VRPyLight.h"
#include "VRPyLightBeacon.h"
#include "VRPyCamera.h"
#include "VRPyLod.h"
#include "VRPyRecorder.h"
#include "VRPyPathtool.h"
#include "VRPyConstructionKit.h"
#include "VRPySnappingEngine.h"
#include "VRPyAnnotationEngine.h"
#include "VRPyAnalyticGeometry.h"
#include "VRPySelector.h"
#include "VRPySelection.h"
#include "VRPyPatchSelection.h"
#include "VRPyPolygonSelection.h"
#include "VRPyMenu.h"
#include "VRPyClipPlane.h"
#include "VRPyListMath.h"
#include "VRPySetup.h"
#include "VRPyNavigator.h"
#include "VRPyNavPreset.h"
#include "VRPyWaypoint.h"
#include "VRPyMeasure.h"
#include "VRPyJointTool.h"
#include "VRPyImage.h"
#include "VRPyProjectManager.h"
#include "VRPyGeoPrimitive.h"
#include "VRPyProgress.h"
#include "VRPyUndoManager.h"

#include "addons/Algorithms/VRPyGraphLayout.h"
#include "addons/CaveKeeper/VRPyCaveKeeper.h"
#include "addons/Bullet/Particles/VRPyParticles.h"
#include "addons/Bullet/Fluids/VRPyFluids.h"
#include "addons/Bullet/CarDynamics/VRPyCarDynamics.h"
#include "addons/Engineering/Factory/VRPyFactory.h"
#include "addons/Engineering/Factory/VRPyLogistics.h"
#include "addons/Engineering/Factory/VRPyProduction.h"
#include "addons/Engineering/Factory/VRPyAMLLoader.h"
#include "addons/Engineering/Mechanics/VRPyMechanism.h"
#include "addons/Engineering/VRPyNumberingEngine.h"
#include "addons/CEF/VRPyCEF.h"
#include "addons/CEF/VRPyWebCam.h"
#include "addons/Semantics/Segmentation/VRPySegmentation.h"
#include "addons/Semantics/Segmentation/VRPyAdjacencyGraph.h"
#include "addons/Semantics/Processes/VRPyProcess.h"
#include "addons/Engineering/Chemistry/VRPyMolecule.h"
#include "addons/Engineering/Milling/VRPyMillingMachine.h"
#include "addons/Engineering/Milling/VRPyMillingWorkPiece.h"
#include "addons/Engineering/Milling/VRPyMillingCuttingToolProfile.h"
#include "addons/Engineering/VRPyRobotArm.h"
#include "addons/RealWorld/nature/VRPyTree.h"

// not yet ported dependencies
#ifndef _WIN32
#include "addons/Engineering/CSG/VRPyCSG.h"
#include "addons/RealWorld/VRPyRealWorld.h"
#include "addons/RealWorld/traffic/VRPyTrafficSimulation.h"
#include "addons/SimViDekont/VRPySimViDekont.h"
#include "addons/Semantics/Reasoning/VRPyOntology.h"
#endif

using namespace OSG;

void VRSceneModules::setup(VRScriptManager* sm, PyObject* pModVR) {
    sm->registerModule<VRPyObject>("Object", pModVR, VRPyStorage::typeRef);
    sm->registerModule<VRPyTransform>("Transform", pModVR, VRPyObject::typeRef);
    sm->registerModule<VRPyGeometry>("Geometry", pModVR, VRPyTransform::typeRef);
    sm->registerModule<VRPyMaterial>("Material", pModVR, VRPyObject::typeRef);
    sm->registerModule<VRPyTextureGenerator>("TextureGenerator", pModVR);
    sm->registerModule<VRPyImage>("Image", pModVR);
    sm->registerModule<VRPyLight>("Light", pModVR, VRPyObject::typeRef);
    sm->registerModule<VRPyLightBeacon>("LightBeacon", pModVR, VRPyTransform::typeRef);
    sm->registerModule<VRPyCamera>("Camera", pModVR, VRPyTransform::typeRef);
    sm->registerModule<VRPyLod>("Lod", pModVR, VRPyObject::typeRef);
    sm->registerModule<VRPySprite>("Sprite", pModVR, VRPyGeometry::typeRef);
    sm->registerModule<VRPySound>("Sound", pModVR);
    sm->registerModule<VRPySocket>("Socket", pModVR);
    sm->registerModule<VRPyStroke>("Stroke", pModVR, VRPyGeometry::typeRef);
    sm->registerModule<VRPyConstraint>("Constraint", pModVR);
    sm->registerModule<VRPyDevice>("Device", pModVR);
    sm->registerModule<VRPyIntersection>("Intersection", pModVR);
    sm->registerModule<VRPyHaptic>("Haptic", pModVR, VRPyDevice::typeRef);
    sm->registerModule<VRPyMobile>("Mobile", pModVR, VRPyDevice::typeRef);
    sm->registerModule<VRPyMouse>("Mouse", pModVR, VRPyDevice::typeRef);
    sm->registerModule<VRPyAnimation>("Animation", pModVR);
    sm->registerModule<VRPyPose>("Pose", pModVR);
    sm->registerModule<VRPyPath>("Path", pModVR);
    sm->registerModule<VRPyGraph>("Graph", pModVR);
    sm->registerModule<VRPyGraphLayout>("GraphLayout", pModVR);
    sm->registerModule<VRPyPolygon>("Polygon", pModVR);
    sm->registerModule<VRPyTriangulator>("Triangulator", pModVR);
    sm->registerModule<VRPyRecorder>("Recorder", pModVR);
    sm->registerModule<VRPyProjectManager>("ProjectManager", pModVR, VRPyObject::typeRef);
    sm->registerModule<VRPyGeoPrimitive>("GeoPrimitive", pModVR, VRPyGeometry::typeRef);
    sm->registerModule<VRPyStorage>("Storage", pModVR);
    sm->registerModule<VRPySnappingEngine>("SnappingEngine", pModVR);
    sm->registerModule<VRPyAnnotationEngine>("AnnotationEngine", pModVR, VRPyGeometry::typeRef);
    sm->registerModule<VRPyAnalyticGeometry>("AnalyticGeometry", pModVR, VRPyObject::typeRef);
    sm->registerModule<VRPyConstructionKit>("ConstructionKit", pModVR);
    sm->registerModule<VRPyPathtool>("Pathtool", pModVR, VRPyObject::typeRef);
    sm->registerModule<VRPySelector>("Selector", pModVR);
    sm->registerModule<VRPySelection>("Selection", pModVR);
    sm->registerModule<VRPyPatchSelection>("PatchSelection", pModVR, VRPySelection::typeRef);
    sm->registerModule<VRPyPolygonSelection>("PolygonSelection", pModVR, VRPySelection::typeRef);
    sm->registerModule<VRPyNavigator>("Navigator", pModVR);
    sm->registerModule<VRPyNavPreset>("NavPreset", pModVR);

    sm->registerModule<VRPyProgress>("Progress", pModVR);
    sm->registerModule<VRPyUndoManager>("UndoManager", pModVR);
    sm->registerModule<VRPyMenu>("Menu", pModVR, VRPyGeometry::typeRef);
    sm->registerModule<VRPyClipPlane>("ClipPlane", pModVR, VRPyGeometry::typeRef);
    sm->registerModule<VRPyWaypoint>("Waypoint", pModVR, VRPyGeometry::typeRef);
    sm->registerModule<VRPyMeasure>("Measure", pModVR, VRPyAnalyticGeometry::typeRef);
    sm->registerModule<VRPyJointTool>("JointTool", pModVR, VRPyGeometry::typeRef);
	sm->registerModule<VRPyColorChooser>("ColorChooser", pModVR);
	sm->registerModule<VRPyTextureRenderer>("TextureRenderer", pModVR, VRPyObject::typeRef);
    sm->registerModule<VRPyCaveKeeper>("CaveKeeper", pModVR);
    sm->registerModule<VRPyParticles>("Particles", pModVR, VRPyGeometry::typeRef);
    sm->registerModule<VRPyFluids>("Fluids", pModVR, VRPyParticles::typeRef);
    sm->registerModule<VRPyMetaBalls>("MetaBalls", pModVR, VRPyObject::typeRef);
    sm->registerModule<VRPyCarDynamics>("CarDynamics", pModVR);
    sm->registerModule<VRPyCEF>("CEF", pModVR);
    sm->registerModule<VRPyWebCam>("Webcam", pModVR, VRPySprite::typeRef);
    sm->registerModule<VRPySegmentation>("Segmentation", pModVR);
    sm->registerModule<VRPyAdjacencyGraph>("AdjacencyGraph", pModVR);
    sm->registerModule<VRPyMechanism>("Mechanism", pModVR);
    sm->registerModule<VRPyNumberingEngine>("NumberingEngine", pModVR, VRPyGeometry::typeRef);
    sm->registerModule<VRPyTree>("Tree", pModVR, VRPyGeometry::typeRef);
    sm->registerModule<VRPyMillingMachine>("MillingMachine", pModVR);
    sm->registerModule<VRPyMillingWorkPiece>("MillingWorkPiece", pModVR, VRPyGeometry::typeRef);
    sm->registerModule<VRPyMillingCuttingToolProfile>("MillingCuttingToolProfile", pModVR);
    sm->registerModule<VRPyMolecule>("Molecule", pModVR, VRPyGeometry::typeRef);
    sm->registerModule<VRPyRobotArm>("RobotArm", pModVR);
    sm->registerModule<VRPyOntology>("Ontology", pModVR);
    sm->registerModule<VRPyProcess>("Process", pModVR);
    sm->registerModule<VRPyProcessNode>("ProcessNode", pModVR);
    sm->registerModule<VRPyProcessLayout>("ProcessLayout", pModVR, VRPyTransform::typeRef);
    sm->registerModule<VRPyOntologyRule>("OntologyRule", pModVR);
    sm->registerModule<VRPyProperty>("Property", pModVR);
    sm->registerModule<VRPyConcept>("Concept", pModVR);
    sm->registerModule<VRPyEntity>("Entity", pModVR);
    sm->registerModule<VRPyReasoner>("Reasoner", pModVR);

#ifndef _WIN32
	sm->registerModule<VRPyCSG>("CSGGeometry", pModVR, VRPyGeometry::typeRef);
	sm->registerModule<VRPyRealWorld>("RealWorld", pModVR);
	sm->registerModule<VRPyTrafficSimulation>("TrafficSimulation", pModVR);
	sm->registerModule<VRPySimViDekont>("SimViDekont", pModVR);
#endif

    PyObject* pModSetup = Py_InitModule3("Setup", VRSceneGlobals::methods, "VR Module");
    sm->registerModule<VRPySetup>("Setup", pModSetup, 0, "Setup");
    sm->registerModule<VRPyView>("View", pModSetup, 0, "Setup");
    sm->registerModule<VRPyWindow>("Window", pModSetup, 0, "Setup");

    PyObject* pModFactory = Py_InitModule3("Factory", VRSceneGlobals::methods, "VR Module");
    sm->registerModule<FPyNode>("Node", pModFactory, 0, "Factory");
    sm->registerModule<FPyNetwork>("Network", pModFactory, 0, "Factory");
    sm->registerModule<FPyPath>("FPath", pModFactory, 0, "Factory");
    sm->registerModule<FPyTransporter>("Transporter", pModFactory, 0, "Factory");
    sm->registerModule<FPyContainer>("Container", pModFactory, 0, "Factory");
    sm->registerModule<FPyProduct>("Product", pModFactory, 0, "Factory");
    sm->registerModule<FPyLogistics>("Logistics", pModFactory, 0, "Factory");
    sm->registerModule<VRPyFactory>("Factory", pModFactory, 0, "Factory");
    sm->registerModule<VRPyProduction>("Production", pModFactory, 0, "Factory");
    sm->registerModule<VRPyAMLLoader>("AMLLoader", pModFactory, 0, "Factory");
    PyModule_AddObject(pModVR, "Factory", pModFactory);
}




