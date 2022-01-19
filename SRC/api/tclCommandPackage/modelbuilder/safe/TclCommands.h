//
// cmp
//
#include <tcl.h>

typedef int (TclCharFn)(ClientData, Tcl_Interp*, int, const char**);
// typedef int (TclObjFn)(ClientData,  Tcl_Interp*, int, Tcl_Obj**);

static TclCharFn  TclCommand_addNode;
static TclCharFn  TclCommand_addPattern;
static TclCharFn  TclCommand_addTimeSeries;

extern TclCharFn  TclCommand_addElement;
extern TclCharFn  TclCommand_addUniaxialMaterial;
extern TclCharFn  TclCommand_addSection;
extern TclCharFn  TclCommand_addHomogeneousBC;
extern TclCharFn  TclCommand_addNodalLoad;

TclCharFn TclCommand_addParameter;
TclCharFn TclCommand_mesh;
TclCharFn TclCommand_remesh;
TclCharFn TclCommand_backgroundMesh; 
TclCharFn TclCommand_addBeamIntegration;
TclCharFn TclCommand_addLimitCurve;
TclCharFn TclCommand_addNDMaterial;
TclCharFn TclCommand_addSeries;
TclCharFn TclCommand_addElementalLoad;
TclCharFn TclCommand_addNodalMass;
TclCharFn TclCommand_addHomogeneousBC_X;
TclCharFn TclCommand_addHomogeneousBC_Y; 
TclCharFn TclCommand_addHomogeneousBC_Z;
TclCharFn TclCommand_addEqualDOF_MP;
TclCharFn TclCommand_addEqualDOF_MP_Mixed;
TclCharFn TclCommand_addMP;
TclCharFn TclCommand_addSP;
TclCharFn TclCommand_RigidLink;
TclCharFn TclCommand_addImposedMotionSP;
TclCharFn TclCommand_RigidDiaphragm;


struct char_cmd {
  const char* name;
  TclCharFn*  func;

}  tcl_char_cmds[] =  {

  {"node",             TclCommand_addNode},
  {"element",          TclCommand_addElement},
  {"uniaxialMaterial", TclCommand_addUniaxialMaterial},
  {"section",          TclCommand_addSection},
  {"pattern",          TclCommand_addPattern},
  {"timeSeries",       TclCommand_addTimeSeries},
  {"load",             TclCommand_addNodalLoad},
  {"fix",              TclCommand_addHomogeneousBC},

/*
  {"mass",             TclCommand_addNodalMass},
  {"patch",            TclCommand_addRemoPatch},
  {"layer",            TclCommand_addRemoLayer},
  {"fiber",            TclCommand_addRemoFiber},
  {"beamIntegration",  TclCommand_addBeamIntegration},
  {"nDMaterial",       TclCommand_addNDMaterial},
  {"eleLoad",          TclCommand_addElementalLoad},
  {"geomTransf",       TclCommand_addRemoGeomTransf},
*/

#if defined(OPSDEF_Element_PFEM)
  {"mesh",             TclCommand_mesh},
  {"remesh",           TclCommand_remesh},
  {"background",      &TclCommand_backgroundMesh},
#endif // OPSDEF_Element_PFEM

/*
  {"fixX",                 TclCommand_addHomogeneousBC_X},
  {"fixY",                 TclCommand_addHomogeneousBC_Y},
  {"fixZ",                 TclCommand_addHomogeneousBC_Z},
  {"sp",                   TclCommand_addSP},
  {"mp",                   TclCommand_addMP},
  {"equalDOF",             TclCommand_addEqualDOF_MP},
  {"imposedMotion",        TclCommand_addImposedMotionSP},
  {"imposedSupportMotion", TclCommand_addImposedMotionSP},
  {"groundMotion",         TclCommand_addGroundMotion},
  {"equalDOF_Mixed",       TclCommand_addEqualDOF_MP_Mixed},
  {"rigidLink",            &TclCommand_RigidLink},
  {"rigidDiaphragm",       &TclCommand_RigidDiaphragm},
  {"PySimple1Gen",         TclCommand_doPySimple1Gen},
  {"TzSimple1Gen",         TclCommand_doTzSimple1Gen},
  {"ShallowFoundationGen", TclSafeBuilder_doShallowFoundationGen},
  {"block2D",              TclCommand_doBlock2D},
  {"block3D",              TclCommand_doBlock3D},

  {"Hfiber",             TclSafeBuilder_addRemoHFiber},
*/

/*
  {"yieldSurface_BC",      TclCommand_addYieldSurface_BC},
  {"ysEvolutionModel",     TclCommand_addYS_EvolutionModel},
  {"plasticMaterial",      TclCommand_addYS_PlasticMaterial},
  {"hystereticBackbone",   TclCommand_addHystereticBackbone},
  {"cyclicModel",          TclCommand_addCyclicModel},
  {"limitCurve",           TclCommand_addLimitCurve},
  {"damageModel",          TclCommand_addDamageModel},
  {"frictionModel",        TclCommand_addFrictionModel},
  {"stiffnessDegradation", TclCommand_addStiffnessDegradation},
  {"unloadingRule",        TclCommand_addUnloadingRule},
  {"strengthDegradation",  TclCommand_addStrengthDegradation},
  {"loadPackage",          TclCommand_Package},
*/

/*
  {"parameter",       TclCommand_addParameter},
  {"addToParameter",  TclCommand_addParameter},
  {"updateParameter", TclCommand_addParameter},
*/

/*
// new command for elast2plast in Multi-yield
// plasticity, by ZHY
  {"updateMaterialStage", TclCommand_UpdateMaterialStage},
  {"updateMaterials",     TclCommand_UpdateMaterials},
*/

/*
// new command for updating properties of
// soil materials, by ZHY
   {"updateParameter", TclCommand_UpdateParameter},
*/

};



TclCharFn TclCommand_Package;

// Added by Scott J. Brandenberg
TclCharFn TclCommand_doPySimple1Gen;
TclCharFn TclCommand_doTzSimple1Gen;

// End added by SJB
// Added by Prishati Raychowdhury (UCSD)
TclCharFn TclSafeBuilder_doShallowFoundationGen;
// End PRC
TclCharFn TclCommand_doBlock2D;
TclCharFn TclCommand_doBlock3D;
TclCharFn TclCommand_addRemoPatch;
TclCharFn TclCommand_addRemoLayer;
TclCharFn TclCommand_addRemoFiber;
//Leo
TclCharFn TclSafeBuilder_addRemoHFiber;
TclCharFn TclCommand_addRemoGeomTransf;
TclCharFn TclCommand_addFrictionModel;
TclCharFn TclCommand_addStiffnessDegradation;
TclCharFn TclCommand_addUnloadingRule;
TclCharFn TclCommand_addStrengthDegradation;
TclCharFn TclCommand_addHystereticBackbone;
TclCharFn TclCommand_addGroundMotion;
/// added by ZHY
TclCharFn TclCommand_UpdateMaterialStage;
TclCharFn TclCommand_UpdateMaterials;
/// added by ZHY
TclCharFn TclCommand_UpdateParameter;
////////////////gnp adding rayleigh /////////////////////
TclCharFn TclCommand_addElementRayleigh;
/////////////////////////////////////////////////////////

// Added by Alborz Ghofrani - U.Washington
TclCharFn TclCommand_GenerateInterfacePoints;



