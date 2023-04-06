/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
** ****************************************************************** */
//
//
#include <tcl.h>
#include <assert.h>
#include <Matrix.h>
#include <ID.h>
#include <Node.h>
#include <Domain.h>
#include <g3_api.h>
#include <runtime/BasicModelBuilder.h>

#include <SP_Constraint.h>
#include <SP_ConstraintIter.h>
#include <MP_Constraint.h>

#include <LoadPattern.h>

#include <ImposedMotionSP.h>
#include <ImposedMotionSP1.h>
#include <MultiSupportPattern.h>


int
TclCommand_addHomogeneousBC(ClientData clientData, Tcl_Interp *interp, int argc,
                            TCL_Char ** const argv)
{
  assert(clientData != nullptr);
  BasicModelBuilder *builder = (BasicModelBuilder*)clientData;
  Domain *theTclDomain = builder->getDomain();

  int ndf = argc - 2;

  // check number of arguments
  if (argc < (2 + ndf)) {
    opserr << "WARNING bad command - want: fix nodeId " << ndf
           << " [0,1] conditions";
    return TCL_ERROR;
  }

  // get the id of the node
  int nodeId;
  if (Tcl_GetInt(interp, argv[1], &nodeId) != TCL_OK) {
    opserr << "WARNING invalid nodeId - fix nodeId " << ndf
           << " [0,1] conditions\n";
    return TCL_ERROR;
  }

  char buffer[80];
  strcpy(buffer, "");

  // get the fixity condition and add the constraint if fixed
  for (int i = 0; i < ndf; i++) {
    int theFixity;
    if (Tcl_GetInt(interp, argv[2 + i], &theFixity) != TCL_OK) {
      opserr << "WARNING invalid fixity " << i + 1 << " - load " << nodeId;
      opserr << " " << ndf << " fixities\n";
      return TCL_ERROR;
    } else {
      if (theFixity != 0) {

        // create a homogeneous constraint
        SP_Constraint *theSP = new SP_Constraint(nodeId, i, 0.0, true);
        if (theSP == 0) {
          opserr << "WARNING ran out of memory for SP_Constraint ";
          opserr << "fix " << nodeId << " " << ndf << " [0,1] conditions\n";
          return TCL_ERROR;
        }

        // add it to the domain
        if (theTclDomain->addSP_Constraint(theSP) == false) {
          opserr << "WARNING could not add SP_Constraint to domain using fix "
                    "command - node may already be constrained\n";
          sprintf(buffer, "%d ", 0);
          delete theSP;
        } else {
          sprintf(buffer, "%d ", theSP->getTag());
          Tcl_AppendResult(interp, buffer, NULL);
        }
      }
    }
  }

  // if get here we have sucessfully created the node and added it to the domain
  return TCL_OK;
}

int
TclCommand_addHomogeneousBC_X(ClientData clientData, Tcl_Interp *interp,
                                   int argc, TCL_Char ** const argv)
{
  assert(clientData != nullptr);
  BasicModelBuilder *builder = (BasicModelBuilder*)clientData;

  if (builder == 0 || clientData == 0) {
    opserr << "WARNING builder has been destroyed - elasticBeam \n";
    return TCL_ERROR;
  }

  int ndf = argc - 2;
  if (strcmp(argv[argc-2],"-tol") == 0)
    ndf -= 2;

  // check number of arguments
  if (argc < (2 + ndf)) {
    opserr << "WARNING bad command - want: fixX xLoc " << ndf << " [0,1] conditions";
    return TCL_ERROR;
  }

  // get the xCrd of nodes to be constrained
  double xLoc;
  if (Tcl_GetDouble(interp, argv[1], &xLoc) != TCL_OK) {
      opserr << "WARNING invalid xCrd - fixX xLoc " << ndf << " [0,1] conditions\n";
      return TCL_ERROR;
  }

  // read in the fixities
  ID fixity(ndf);
  for (int i=0; i<ndf; i++) {
    if (Tcl_GetInt(interp, argv[2+i], &fixity(i)) != TCL_OK) {
      opserr << "WARNING invalid fixity " << i+1 << " - fixX " << xLoc;
      opserr << " " << ndf << " fixities\n";
      return TCL_ERROR;
    }
  }

  // set the tolerance, the allowable difference in nodal coordinate and
  // what the value user specified to see if node is constrained or not
  double tol = 1.0e-10;
  if (argc >= (4 + ndf)) {
    if (strcmp(argv[2+ndf],"-tol") == 0)
    if (Tcl_GetDouble(interp, argv[3+ndf], &tol) != TCL_OK) {
      opserr << "WARNING invalid tol specified - fixX " << xLoc << endln;
      return TCL_ERROR;
    }
  }

  builder->addSP_Constraint(0, xLoc, fixity, tol);

  // if get here we have sucessfully created the node and added it to the domain
  return TCL_OK;
}

int
TclCommand_addHomogeneousBC_Y(ClientData clientData, Tcl_Interp *interp,
                                   int argc, TCL_Char ** const argv)
{
  assert(clientData != nullptr);
  BasicModelBuilder *builder = (BasicModelBuilder*)clientData;

  int ndf = argc - 2;
  if (strcmp(argv[argc-2],"-tol") == 0)
    ndf -= 2;

  // check number of arguments
  if (argc < (2 + ndf)) {
    opserr << "WARNING bad command - want: fixY yLoc " << ndf << " [0,1] conditions";
    return TCL_ERROR;
  }

  // get the yCrd of nodes to be constrained
  double yLoc;
  if (Tcl_GetDouble(interp, argv[1], &yLoc) != TCL_OK) {
      opserr << "WARNING invalid yCrd - fixY yLoc " << ndf << " [0,1] conditions\n"; return TCL_ERROR;
  }

  // read in the fixities
  ID fixity(ndf);
  for (int i=0; i<ndf; i++) {
    if (Tcl_GetInt(interp, argv[2+i], &fixity(i)) != TCL_OK) {
      opserr << "WARNING invalid fixity " << i+1 << " - fixY " << yLoc;
      opserr << " " << ndf << " fixities\n";
      return TCL_ERROR;
    }
  }

  // set the tolerance, the allowable difference in nodal coordinate and
  // what the value user specified to see if node is constrained or not
  double tol = 1.0e-10;
  if (argc >= (4 + ndf)) {
    if (strcmp(argv[2+ndf],"-tol") == 0)
    if (Tcl_GetDouble(interp, argv[3+ndf], &tol) != TCL_OK) {
      opserr << "WARNING invalid tol specified - fixY " << yLoc << endln;
      return TCL_ERROR;
    }
  }


  builder->addSP_Constraint(1, yLoc, fixity, tol);

  return TCL_OK;
}

int
TclCommand_addHomogeneousBC_Z(ClientData clientData, Tcl_Interp *interp,
                                   int argc, TCL_Char ** const argv)
{

  assert(clientData != nullptr);
  BasicModelBuilder *builder = (BasicModelBuilder*)clientData;

  int ndf = argc - 2;
  if (strcmp(argv[argc-2],"-tol") == 0)
    ndf -= 2;

  // check number of arguments
  if (argc < (2 + ndf)) {
    opserr << "WARNING bad command - want: fixZ zLoc " << ndf << " [0,1] conditions";
    return TCL_ERROR;
  }

  // get the yCrd of nodes to be constrained
  double zLoc;
  if (Tcl_GetDouble(interp, argv[1], &zLoc) != TCL_OK) {
      opserr << "WARNING invalid zCrd - fixZ zLoc " << ndf << " [0,1] conditions\n"; return TCL_ERROR;
  }

  // read in the fixities
  ID fixity(ndf);
  for (int i=0; i<ndf; i++) {
    if (Tcl_GetInt(interp, argv[2+i], &fixity(i)) != TCL_OK) {
      opserr << "WARNING invalid fixity " << i+1 << " - fixZ " << zLoc;
      opserr << " " << ndf << " fixities\n";
      return TCL_ERROR;
    }
  }

  // set the tolerance, the allowable difference in nodal coordinate and
  // what the value user specified to see if node is constrained or not
  double tol = 1.0e-10;
  if (argc >= (4 + ndf)) {
    if (strcmp(argv[2+ndf],"-tol") == 0)
      if (Tcl_GetDouble(interp, argv[3+ndf], &tol) != TCL_OK) {
        opserr << "WARNING invalid tol specified - fixZ " << zLoc << endln;
        return TCL_ERROR;
      }
  }

  builder->addSP_Constraint(2, zLoc, fixity, tol);

  return TCL_OK;
}



int
TclCommand_addSP(ClientData clientData, Tcl_Interp *interp, int argc,
                      TCL_Char ** const argv)
{
  G3_Runtime *rt = G3_getRuntime(interp);
  Domain *theTclDomain = G3_getDomain(rt);

  // TODO!! 
  LoadPattern *theTclLoadPattern = (LoadPattern*)clientData; // theTclBuilder->getCurrentLoadPattern();

  // check number of arguments
  if (argc < 4) {
    opserr << "WARNING bad command - want: sp nodeId dofID value";
    return TCL_ERROR;
  }

  // get the nodeID, dofId and value of the constraint
  int nodeId, dofId;
  double value;

  if (Tcl_GetInt(interp, argv[1], &nodeId) != TCL_OK) {
    opserr << "WARNING invalid nodeId: " << argv[1] << " -  sp nodeId dofID value\n"; return TCL_ERROR;
  }
  if (Tcl_GetInt(interp, argv[2], &dofId) != TCL_OK) {
    opserr << "WARNING invalid dofId: " << argv[2] << " -  sp ";
    opserr << nodeId << " dofID value\n";
      return TCL_ERROR;
  }
  dofId--; // DECREMENT THE DOF VALUE BY 1 TO GO TO OUR C++ INDEXING

  if (Tcl_GetDouble(interp, argv[3], &value) != TCL_OK) {
    opserr << "WARNING invalid value: " << argv[3] << " -  sp ";
    opserr << nodeId << " dofID value\n";
      return TCL_ERROR;
  }

  bool isSpConst = false;
  bool userSpecifiedPattern = false;
  int loadPatternTag = 0; // some pattern that will never be used!

  int endMarker = 4;
  while (endMarker != argc) {
    if (strcmp(argv[endMarker],"-const") == 0) {
      // allow user to specify const load
      isSpConst = true;
    } else if (strcmp(argv[endMarker],"-pattern") == 0) {
      // allow user to specify load pattern other than current
      endMarker++;
      userSpecifiedPattern = true;
      if (endMarker == argc ||
          Tcl_GetInt(interp, argv[endMarker], &loadPatternTag) != TCL_OK) {

        opserr << "WARNING invalid patternTag - load " << nodeId << "\n";
        return TCL_ERROR;
      }
    }
    endMarker++;
  }

  // if load pattern tag has not changed - get the pattern tag from current one
  if (userSpecifiedPattern == false) {
    if (theTclLoadPattern == nullptr) {
      opserr << "WARNING no current pattern - sp " 
             << nodeId << " dofID value\n"; 
      return TCL_ERROR;
    } else {
      loadPatternTag = theTclLoadPattern->getTag();
    }
  }

  // LoadPattern *thePattern = theTclDomain->getLoadPattern(loadPatternTag);

  // create a homogeneous constraint
  SP_Constraint *theSP = new SP_Constraint(nodeId, dofId, value, isSpConst);

  if (theSP == 0) {
    opserr << "WARNING ran out of memory for SP_Constraint ";
    opserr << " - sp " << nodeId << " dofID value\n";
    return TCL_ERROR;
  }
  if (theTclDomain->addSP_Constraint(theSP, loadPatternTag) == false) {
    opserr << "WARNING could not add SP_Constraint to domain ";
    delete theSP;
    return TCL_ERROR;
  }

  // if get here we have sucessfully created the node and added it to the domain
  return TCL_OK;
}

int
TclCommand_addEqualDOF_MP(ClientData clientData, Tcl_Interp *interp,
                                int argc, TCL_Char ** const argv)
{
    BasicModelBuilder *builder = (BasicModelBuilder*)clientData;
    Domain     *theTclDomain   = builder->getDomain();


    // Check number of arguments
    if (argc < 4) {
      opserr << "WARNING bad command - want: equalDOF RnodeID? CnodeID? DOF1? DOF2? ...";
      return TCL_ERROR;
    }

    // Read in the node IDs and the DOF
    int RnodeID, CnodeID, dofID;

    if (Tcl_GetInt (interp, argv[1], &RnodeID) != TCL_OK) {
      opserr << "WARNING invalid RnodeID: " << argv[1]
           << " equalDOF RnodeID? CnodeID? DOF1? DOF2? ...";
      return TCL_ERROR;
    }
    if (Tcl_GetInt (interp, argv[2], &CnodeID) != TCL_OK) {
      opserr << "WARNING invalid CnodeID: " << argv[2]
           << " equalDOF RnodeID? CnodeID? DOF1? DOF2? ...";
      return TCL_ERROR;
    }

    // The number of DOF to be coupled
    int numDOF = argc - 3;

    // The constraint matrix ... U_c = C_cr * U_r
    Matrix Ccr (numDOF, numDOF);
    Ccr.Zero();

    // The vector containing the retained and constrained DOFs
    ID rcDOF (numDOF);

    int i, j;
    // Read the degrees of freedom which are to be coupled
    for (i = 3, j = 0; i < argc; i++, j++) {
      if (Tcl_GetInt (interp, argv[i], &dofID) != TCL_OK) {
        opserr << "WARNING invalid dofID: " << argv[3]
               << " equalDOF RnodeID? CnodeID? DOF1? DOF2? ...";
        return TCL_ERROR;
      }

      dofID -= 1; // Decrement for C++ indexing
      if (dofID < 0) {
        opserr << "WARNING invalid dofID: " << argv[i]
               << " must be >= 1";
        return TCL_ERROR;
      }
      rcDOF (j) = dofID;
      Ccr (j,j) = 1.0;
    }

    // Create the multi-point constraint
    MP_Constraint *theMP = new MP_Constraint (RnodeID, CnodeID, Ccr, rcDOF, rcDOF);
    if (theMP == 0) {
      opserr << "WARNING ran out of memory for equalDOF MP_Constraint ";
      return TCL_ERROR;
    }

    // Add the multi-point constraint to the domain
    if (theTclDomain->addMP_Constraint (theMP) == false) {
      opserr << "WARNING could not add equalDOF MP_Constraint to domain ";
      delete theMP;
      return TCL_ERROR;
    }

    char buffer[80];
    sprintf(buffer, "%d", theMP->getTag());
    Tcl_SetResult(interp, buffer, TCL_VOLATILE);

    return TCL_OK;
}

#if 0
int
TclCommand_addEqualDOF_MP_Mixed(ClientData clientData, Tcl_Interp *interp,
                                int argc, TCL_Char ** const argv)
{
        // Ensure the destructor has not been called
        BasicModelBuilder *builder = (BasicModelBuilder*)clientData;

        if (theTclBuilder == 0 || clientData == 0) {
          opserr << "WARNING builder has been destroyed - equalDOF \n";
          return TCL_ERROR;
        }

        // Check number of arguments
        if (argc < 4) {
          opserr << "WARNING bad command - want: equalDOFmixed RnodeID? CnodeID? numDOF? RDOF1? CDOF1? ... ...";
          return TCL_ERROR;
        }

        // Read in the node IDs and the DOF
        int RnodeID, CnodeID, dofIDR, dofIDC, numDOF;

        if (Tcl_GetInt (interp, argv[1], &RnodeID) != TCL_OK) {
          opserr << "WARNING invalid RnodeID: " << argv[1]
               << " equalDOF RnodeID? CnodeID? numDOF? RDOF1? CDOF1? ...";
          return TCL_ERROR;
        }
        if (Tcl_GetInt (interp, argv[2], &CnodeID) != TCL_OK) {
          opserr << "WARNING invalid CnodeID: " << argv[2]
               << " equalDOF RnodeID? CnodeID? numDOF? RDOF1? CDOF1? ...";
          return TCL_ERROR;
        }

        if (Tcl_GetInt (interp, argv[3], &numDOF) != TCL_OK) {
          opserr << "WARNING invalid numDOF: " << argv[2]
               << " equalDOF RnodeID? CnodeID? numDOF? RDOF1? CDOF1? ...";
          return TCL_ERROR;
        }

        // The number of DOF to be coupled
        //        int numDOF = argc - 3;

        // The constraint matrix ... U_c = C_cr * U_r
        Matrix Ccr (numDOF, numDOF);
        Ccr.Zero();

        // The vector containing the retained and constrained DOFs
        ID rDOF (numDOF);
        ID cDOF (numDOF);

        int i, j, k;
        // Read the degrees of freedom which are to be coupled
        for (i = 4, j = 5, k = 0; k < numDOF; i+=2, j+=2, k++) {
          if (Tcl_GetInt (interp, argv[i], &dofIDR) != TCL_OK) {
            opserr << "WARNING invalid dofID: " << argv[3]
                 << " equalDOF RnodeID? CnodeID? DOF1? DOF2? ...";
            return TCL_ERROR;
          }
          if (Tcl_GetInt(interp, argv[j], &dofIDC) != TCL_OK) {
            opserr << "WARNING invalid dofID: " << argv[3]
                 << " equalDOF RnodeID? CnodeID? DOF1? DOF2? ...";
            return TCL_ERROR;
          }

          dofIDR -= 1; // Decrement for 0-based indexing
          dofIDC -= 1;
          if (dofIDC < 0 || dofIDR < 0) {
            opserr << "WARNING invalid dofID: " << argv[i]
                   << " must be >= 1";
            return TCL_ERROR;
          }
          rDOF(k) = dofIDR;
          cDOF(k) = dofIDC;
          Ccr(k,k) = 1.0;
        }

        // Create the multi-point constraint
        MP_Constraint *theMP = new MP_Constraint (RnodeID, CnodeID, Ccr, cDOF, rDOF); 
        if (theMP == 0) { 
          opserr << "WARNING ran out of memory for equalDOF MP_Constraint "; 
          return TCL_ERROR;
        }

        // Add the multi-point constraint to the domain
        if (theTclDomain->addMP_Constraint (theMP) == false) {
          opserr << "WARNING could not add equalDOF MP_Constraint to domain ";
          delete theMP;
          return TCL_ERROR;
        }

        char buffer[80];
        sprintf(buffer, "%d", theMP->getTag());
        Tcl_SetResult(interp, buffer, TCL_VOLATILE);

        return TCL_OK;
}
#endif


int
TclCommand_addImposedMotionSP(ClientData clientData,
                                   Tcl_Interp *interp,
                                   int argc,
                                   TCL_Char ** const argv)
{
  // TODO: Cleanup
  G3_Runtime* rt = G3_getRuntime(interp);
  Domain *domain = G3_getDomain(rt);

  // BasicModelBuilder *theTclBuilder = G3_getSafeBuilder(G3_getRuntime(interp));
  // // ensure the destructor has not been called -
  // BasicModelBuilder *builder = (BasicModelBuilder*)clientData;



  // check number of arguments
  if (argc < 4) {
    opserr << "WARNING bad command - want: imposedMotion nodeId dofID gMotionID\n";
    return TCL_ERROR;
  }

  // get the nodeID, dofId and value of the constraint
  int nodeId, dofId, gMotionID;

  if (Tcl_GetInt(interp, argv[1], &nodeId) != TCL_OK) {
    opserr << "WARNING invalid nodeId: " << argv[1];
    opserr << " - imposedMotion nodeId dofID gMotionID\n";
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[2], &dofId) != TCL_OK) {
    opserr << "WARNING invalid dofId: " << argv[2] << " -  imposedMotion ";
    opserr << nodeId << " dofID gMotionID\n";
      return TCL_ERROR;
  }
  dofId--; // DECREMENT THE DOF VALUE BY 1 TO GO TO OUR C++ INDEXING

  if (Tcl_GetInt(interp, argv[3], &gMotionID) != TCL_OK) {
    opserr << "WARNING invalid gMotionID: " << argv[3] << " -  imposedMotion ";
    opserr << nodeId << " dofID gMotionID\n";
    return TCL_ERROR;
  }

  bool alt = false;
  if (argc == 5) {
    if (strcmp(argv[4],"-other") == 0)
      alt = true;
  }

  //
  // check valid node & dof
  //

  Node *theNode = domain->getNode(nodeId);
  if (theNode == nullptr) {
    opserr << "WARNING invalid node " << argv[2] << " node not found\n ";
    return -1;
  }

  int nDof = theNode->getNumberDOF();
  if (dofId < 0 || dofId >= nDof) {
    opserr << "WARNING invalid dofId: " << argv[2] << " dof specified cannot be <= 0 or greater than num dof at nod\n "; 
    return -2;
  }


  MultiSupportPattern *thePattern = 
    (MultiSupportPattern*)Tcl_GetAssocData(interp, "theTclMultiSupportPattern", NULL);
  if (thePattern == 0) {
    opserr << "ERROR no multi-support pattern found\n";
    return TCL_ERROR;
  }
  int loadPatternTag = thePattern->getTag();

  // create a new ImposedMotionSP
  SP_Constraint *theSP;
  if (alt == true) {
    theSP = new ImposedMotionSP1(nodeId, dofId, loadPatternTag, gMotionID);
  }
  else {
    theSP = new ImposedMotionSP(nodeId, dofId, loadPatternTag, gMotionID);
  }

  if (theSP == nullptr) {
    opserr << "WARNING ran out of memory for ImposedMotionSP ";
    opserr << " -  imposedMotion ";
    opserr << nodeId << " " << dofId++ << " " << gMotionID << endln;
    return TCL_ERROR;
  }
  if (thePattern->addSP_Constraint(theSP) == false) {
    opserr << "WARNING could not add SP_Constraint to pattern ";
    delete theSP;
    return TCL_ERROR;
  }
  return TCL_OK;
}

