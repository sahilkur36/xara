/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
** ****************************************************************** */
//
//
#include <assert.h>
#include <tcl.h>
#include <G3_Logging.h>
#include "runtime/BasicAnalysisBuilder.h"

// convergence tests
#include <CTestNormUnbalance.h>
#include <CTestNormDispIncr.h>
#include <CTestEnergyIncr.h>
#include <CTestRelativeNormUnbalance.h>
#include <CTestRelativeNormDispIncr.h>
#include <CTestRelativeEnergyIncr.h>
#include <CTestRelativeTotalNormDispIncr.h>
#include <CTestFixedNumIter.h>
#include <NormDispAndUnbalance.h>
#include <NormDispOrUnbalance.h>

#ifndef G3_Char
#define G3_Char const char
#endif

ConvergenceTest*
TclDispatch_newConvergenceTest(ClientData clientData, Tcl_Interp* interp, int argc, G3_Char ** const argv);


int
specifyCTest(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char ** const argv)
{
  assert(clientData != nullptr);
  ConvergenceTest* theNewTest = TclDispatch_newConvergenceTest(clientData, interp, argc, argv);

  if (theNewTest == nullptr) {
    // Parse routine is expected to have reported an error
    return TCL_ERROR;

  } else {
    BasicAnalysisBuilder* builder = (BasicAnalysisBuilder*)clientData;

    assert(builder != nullptr);

    builder->set(theNewTest);
  }
  return TCL_OK;
}

ConvergenceTest*
TclDispatch_newConvergenceTest(ClientData clientData, Tcl_Interp* interp, int argc, G3_Char ** const argv)
{
  // Domain *domain = G3_getDomain(rt);

  // get the tolerence first
  double tol = 1e-12;
  double tol2 = 0.0;
  double tolp = 0.0;
  double tolp2 = 0.0;
  double tolrel = 0.0;
  double tolprel = 0.0;
  double maxTol = OPS_MAXTOL;

  int numIter = 10;
  int printIt = 0;
  int normType = 2;
  int maxIncr = -1;


  // make sure at least one other argument to contain numberer
  if (argc < 2) {
    opserr << G3_ERROR_PROMPT << "need to specify a ConvergenceTest Type type \n";
    return nullptr;
  }


  if ((strcmp(argv[1], "NormDispAndUnbalance") == 0) ||
      (strcmp(argv[1], "NormDispOrUnbalance") == 0)) {
    if (argc == 5) {
      if (Tcl_GetDouble(interp, argv[2], &tol) != TCL_OK)
        return nullptr;
      if (Tcl_GetDouble(interp, argv[3], &tol2) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[4], &numIter) != TCL_OK)
        return nullptr;
    } else if (argc == 6) {
      if (Tcl_GetDouble(interp, argv[2], &tol) != TCL_OK)
        return nullptr;
      if (Tcl_GetDouble(interp, argv[3], &tol2) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[4], &numIter) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[5], &printIt) != TCL_OK)
        return nullptr;
    } else if (argc == 7) {
      if (Tcl_GetDouble(interp, argv[2], &tol) != TCL_OK)
        return nullptr;
      if (Tcl_GetDouble(interp, argv[3], &tol2) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[4], &numIter) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[5], &printIt) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[6], &normType) != TCL_OK)
        return nullptr;
    } else if (argc == 8) {
      if (Tcl_GetDouble(interp, argv[2], &tol) != TCL_OK)
        return nullptr;
      if (Tcl_GetDouble(interp, argv[3], &tol2) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[4], &numIter) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[5], &printIt) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[6], &normType) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[7], &maxIncr) != TCL_OK)
        return nullptr;
    }

  } else if (strcmp(argv[1], "FixedNumIter") == 0) {

    if (argc == 3) {
      if (Tcl_GetInt(interp, argv[2], &numIter) != TCL_OK)
        return nullptr;
    } else if (argc == 4) {
      if (Tcl_GetInt(interp, argv[2], &numIter) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[3], &printIt) != TCL_OK)
        return nullptr;
    } else if (argc == 5) {
      if (Tcl_GetInt(interp, argv[2], &numIter) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[3], &printIt) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[4], &normType) != TCL_OK)
        return nullptr;
    } else if (argc == 6) {
      if (Tcl_GetInt(interp, argv[2], &numIter) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[3], &printIt) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[4], &normType) != TCL_OK)
        return nullptr;
      if (Tcl_GetDouble(interp, argv[5], &maxTol) != TCL_OK)
        return nullptr;
    }

  } else {
    if (argc == 4) {
      if (Tcl_GetDouble(interp, argv[2], &tol) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[3], &numIter) != TCL_OK)
        return nullptr;
    } else if (argc == 5) {
      if (Tcl_GetDouble(interp, argv[2], &tol) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[3], &numIter) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[4], &printIt) != TCL_OK)
        return nullptr;
    } else if (argc == 6) {
      if (Tcl_GetDouble(interp, argv[2], &tol) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[3], &numIter) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[4], &printIt) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[5], &normType) != TCL_OK)
        return nullptr;
    } else if (argc == 7) {
      if (Tcl_GetDouble(interp, argv[2], &tol) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[3], &numIter) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[4], &printIt) != TCL_OK)
        return nullptr;
      if (Tcl_GetInt(interp, argv[5], &normType) != TCL_OK)
        return nullptr;
      if (Tcl_GetDouble(interp, argv[6], &maxTol) != TCL_OK)
        return nullptr;
    }
  }

  ConvergenceTest *theNewTest = nullptr;

  if (numIter == 0) {
    opserr << G3_ERROR_PROMPT << "no numIter specified in test command\n";
    return nullptr;
  }

  if (strcmp(argv[1], "FixedNumIter") == 0)
    theNewTest = new CTestFixedNumIter(numIter, printIt, normType);
  else {
    if (tol == 0.0) {
      opserr << G3_ERROR_PROMPT << "no tolerance specified in test command\n";
      return nullptr;
    }
    if (strcmp(argv[1], "NormUnbalance") == 0)
      theNewTest = new CTestNormUnbalance(tol, numIter, printIt, normType,
                                          maxIncr, maxTol);
    else if (strcmp(argv[1], "NormDispIncr") == 0)
      theNewTest =
          new CTestNormDispIncr(tol, numIter, printIt, normType, maxTol);
    else if (strcmp(argv[1], "NormDispAndUnbalance") == 0)
      theNewTest = new NormDispAndUnbalance(tol, tol2, numIter, printIt,
                                            normType, maxIncr);
    else if (strcmp(argv[1], "NormDispOrUnbalance") == 0)
      theNewTest = new NormDispOrUnbalance(tol, tol2, numIter, printIt,
                                           normType, maxIncr);
    else if (strcmp(argv[1], "EnergyIncr") == 0)
      theNewTest = new CTestEnergyIncr(tol, numIter, printIt, normType, maxTol);
    else if (strcmp(argv[1], "RelativeNormUnbalance") == 0)
      theNewTest =
          new CTestRelativeNormUnbalance(tol, numIter, printIt, normType);
    else if (strcmp(argv[1], "RelativeNormDispIncr") == 0)
      theNewTest =
          new CTestRelativeNormDispIncr(tol, numIter, printIt, normType);
    else if (strcmp(argv[1], "RelativeEnergyIncr") == 0)
      theNewTest = new CTestRelativeEnergyIncr(tol, numIter, printIt, normType);
    else if (strcmp(argv[1], "RelativeTotalNormDispIncr") == 0)
      theNewTest =
          new CTestRelativeTotalNormDispIncr(tol, numIter, printIt, normType);
    else {
      opserr << G3_ERROR_PROMPT << "No ConvergenceTest type (NormUnbalance, NormDispIncr, "
                "EnergyIncr, \n";
      opserr << "RelativeNormUnbalance, RelativeNormDispIncr, "
                "RelativeEnergyIncr, \n";
      opserr << "RelativeTotalNormDispIncr, FixedNumIter)\n";
      return nullptr;
    }
  }
  return theNewTest;
}

int
getCTestNorms(ClientData clientData, Tcl_Interp *interp, int argc,
              TCL_Char ** const argv)
{
  assert(clientData != nullptr);
  ConvergenceTest *theTest =
      ((BasicAnalysisBuilder *)clientData)->getConvergenceTest();

  if (theTest != nullptr) {
    const Vector &data = theTest->getNorms();

    char buffer[40];
    int size = data.Size();
    for (int i = 0; i < size; i++) {
      sprintf(buffer, "%35.20e", data(i));
      Tcl_AppendResult(interp, buffer, NULL);
    }

    return TCL_OK;
  }

  opserr << G3_ERROR_PROMPT << "testNorms - no convergence test has been constructed.\n";
  return TCL_ERROR;
}

int
getCTestIter(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char ** const argv)
{
  assert(clientData != nullptr);
  ConvergenceTest *theTest =
      ((BasicAnalysisBuilder *)clientData)->getConvergenceTest();

  if (theTest != nullptr) {
    int res = theTest->getNumTests();

    char buffer[10];
    sprintf(buffer, "%d", res);
    Tcl_AppendResult(interp, buffer, NULL);

    return TCL_OK;
  }

  opserr << G3_ERROR_PROMPT << "testIter - no convergence test.\n";
  return TCL_ERROR;
}

