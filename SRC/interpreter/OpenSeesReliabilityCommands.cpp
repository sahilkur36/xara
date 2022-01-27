/* *****************************************************************************
Copyright (c) 2015-2017, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written: Minjie

// Description: all reliability APIs are defined or declared here
//

#include "OpenSeesReliabilityCommands.h"
#include <elementAPI.h>

#include <vector>
#include <ID.h>

#include <RandomVariable.h>
#include <RandomVariableIter.h>
#include <NormalRV.h>
#include <LognormalRV.h>
#include <GammaRV.h>
#include <ShiftedExponentialRV.h>
#include <ShiftedRayleighRV.h>
#include <ExponentialRV.h>
#include <RayleighRV.h>
#include <UniformRV.h>
#include <BetaRV.h>
#include <Type1LargestValueRV.h>
#include <Type1SmallestValueRV.h>
#include <Type2LargestValueRV.h>
#include <Type3SmallestValueRV.h>
#include <ChiSquareRV.h>
#include <GumbelRV.h>
#include <WeibullRV.h>
#include <UserDefinedRV.h>
#include <LaplaceRV.h>
#include <ParetoRV.h>
#ifdef _PYTHON3
#include <PythonRV.h>
#include <PythonEvaluator.h>
#endif

#include <AllIndependentTransformation.h>
#include <NatafProbabilityTransformation.h>

#include <CStdLibRandGenerator.h>

#include <StandardReliabilityConvergenceCheck.h>
#include <OptimalityConditionReliabilityConvergenceCheck.h>

#include <HLRFSearchDirection.h>
#include <PolakHeSearchDirectionAndMeritFunction.h>
#include <SQPsearchDirectionMeritFunctionAndHessian.h>
#include <GradientProjectionSearchDirection.h>

#include <AdkZhangMeritFunctionCheck.h>

#include <ArmijoStepSizeRule.h>
#include <FixedStepSizeRule.h>

#include <SecantRootFinding.h>

#include <FiniteDifferenceGradient.h>
#include <ImplicitGradient.h>

// active object
static OpenSeesReliabilityCommands* cmds = 0;

OpenSeesReliabilityCommands::OpenSeesReliabilityCommands(Domain* structuralDomain)
  :theDomain(0), theStructuralDomain(structuralDomain),
   theProbabilityTransformation(0), theRandomNumberGenerator(0),
   theReliabilityConvergenceCheck(0), theSearchDirection(0), theMeritFunctionCheck(0),
   theStepSizeRule(0), theRootFinding(0), theFunctionEvaluator(0),
   theGradientEvaluator(0)
{
    if (structuralDomain != 0) {
	theDomain = new ReliabilityDomain(structuralDomain);	
    }

    cmds = this;
}

OpenSeesReliabilityCommands::~OpenSeesReliabilityCommands()
{
    if (theDomain != 0) delete theDomain;
    cmds = 0;
}

ReliabilityDomain*
OpenSeesReliabilityCommands::getDomain()
{
    return theDomain;
}

Domain*
OpenSeesReliabilityCommands::getStructuralDomain()
{
    return theStructuralDomain;
}

void OpenSeesReliabilityCommands::wipe() {
  // wipe reliability domain
  if (theDomain != 0) {
    theDomain->clearAll();
  }

  if (theProbabilityTransformation != 0) {
    delete theProbabilityTransformation;
    theReliabilityConvergenceCheck = 0;
  }
  if (theRandomNumberGenerator != 0) {
    delete theRandomNumberGenerator;
    theRandomNumberGenerator = 0;
  }
  if (theReliabilityConvergenceCheck != 0) {
    delete theReliabilityConvergenceCheck;
    theReliabilityConvergenceCheck = 0;
  }
  if (theSearchDirection != 0) {
    delete theSearchDirection;
    theSearchDirection = 0;
  }
  if (theMeritFunctionCheck != 0) {
    delete theMeritFunctionCheck;
    theMeritFunctionCheck = 0;
  }
  if (theStepSizeRule != 0) {
    delete theStepSizeRule;
    theStepSizeRule = 0;
  }
  if (theRootFinding != 0) {
    delete theRootFinding;
    theRootFinding = 0;
  }
  if (theFunctionEvaluator != 0) {
    delete theFunctionEvaluator;
    theFunctionEvaluator = 0;
  }
  if (theGradientEvaluator != 0) {
    delete theGradientEvaluator;
    theGradientEvaluator = 0;
  }
}

int OPS_wipeReliability() {
  if (cmds != 0) {
    cmds->wipe();
  }
  return 0;
}

int OPS_performanceFunction() {
  LimitStateFunction *theLSF = 0;
  int tag;

  // Check enough arguments
  if (OPS_GetNumRemainingInputArgs() < 1) {
    opserr << "ERROR: invalid number of arguments to performanceFunction "
              "command: performanceFunction tag ...\n";
    return -1;
  }

  // Get tag
  int numdata = 1;
  if (OPS_GetIntInput(&numdata, &tag) < 0) {
    opserr << "ERROR: invalid tag for performanceFunction: tag \n";
    return -1;
  }

  ReliabilityDomain *theReliabilityDomain = cmds->getDomain();

  // Evaluate performance function
  FunctionEvaluator *theEvaluator = cmds->getFunctionEvaluator();
  if (OPS_GetNumRemainingInputArgs() < 1) {
    if (theEvaluator == 0) {
      opserr << "Function evaluator must be defined in order to evaluate "
                "performance function"
             << endln;
      return -1;
    }
    theReliabilityDomain->setTagOfActiveLimitStateFunction(tag);
    double g = theEvaluator->evaluateExpression();
    if (OPS_SetDoubleOutput(&numdata, &g, true) < 0) {
      opserr
          << "ERROR: performanceFunction - failed to set double output\n";
      return -1;
    }
    return 0;
  }

  if (theEvaluator != 0) {
    opserr << "ERROR: A limit-state function should not be created after "
              "the GFunEvaluator has been instantiated.\n";
    return -1;
  }

  // Create new performance function
  const char *lsf = OPS_GetString();
  theLSF = new LimitStateFunction(tag, lsf);

  // Add the performance function to the domain
  if (theReliabilityDomain->addLimitStateFunction(theLSF) == false) {
    opserr << "ERROR: failed to add performance function to the "
              "reliability domain\n";
    opserr << "performanceFunction: " << tag << "\n";
    delete theLSF;  // otherwise memory leak
    return -1;
  }

  return 0;
}

int OPS_randomVariable()
{
    RandomVariable *theRandomVariable = 0;
    int tag;
    double mean = 0.0;
    double stdv = 1.0;
    double startPt = 0.0;
    int use_start_pt = 0;
	
    double param = 0;
    Vector parameters;

    // CHECK THAT AT LEAST ENOUGH ARGUMENTS ARE GIVEN
    if (OPS_GetNumRemainingInputArgs() < 2) {
	opserr << "ERROR: invalid number of arguments to randomVariable command : randomVariable tag dist -mean mean -stdv stdv -startPoint startPoint -parameters pram1 pram2 ...\n";
	return -1;
    }

    // GET TAG NUMBER
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) < 0) {
	opserr << "ERROR: invalid input: tag \n";
	return -1;
    }

    // GET DISTRIBUTION
    const char* dist = OPS_GetString();

    char *filename = 0;
    char *functionname = 0;
    
    // read options
    while (OPS_GetNumRemainingInputArgs() > 0) {

	const char* arg = OPS_GetString();

	if (strcmp(arg,"-file") == 0) {
	    if (OPS_GetNumRemainingInputArgs() < 1) {
		opserr << "WARNING not enough args, need -file filename??\n";
		opserr << "for random variable: " << tag << "\n";
		return -1;
	    }
	    filename = (char*)OPS_GetString();
	}
	if (strcmp(arg,"-function") == 0) {
	    if (OPS_GetNumRemainingInputArgs() < 1) {
		opserr << "WARNING not enough args, need -function functionname??\n";
		opserr << "for random variable: " << tag << "\n";
		return -1;
	    }
	    functionname = (char*)OPS_GetString();
	}	
	
	// user specified mean directly
	if (strcmp(arg,"-mean") == 0) {
	    if (OPS_GetNumRemainingInputArgs() < 1) {
		opserr << "WARNING not enough args, need -mean mean??\n";
		opserr << "for random variable: " << tag << "\n";
		return -1;
	    }
	    if (OPS_GetDoubleInput(&numdata, &mean) < 0) {
		opserr << "WARNING invalid mean\n";
		opserr << " for random variable: " << tag << "\n";
		return -1;
	    }
	}
		
	// user specified standard deviation directly
	else if (strcmp(arg,"-stdv") == 0) {
	    if (OPS_GetNumRemainingInputArgs() < 1) {
		opserr << "WARNING not enough args, need -stdv stdv??\n";
		opserr << " for random variable: " << tag << "\n";
		return -1;
	    }
	    if (OPS_GetDoubleInput(&numdata, &stdv) < 0) {
		opserr << "WARNING invalid standard deviation\n";
		opserr << " for random variable: " << tag << "\n";
		return -1;
	    }
	}
		
	// user specified starting point directly
	else if (strcmp(arg,"-startPoint") == 0) {
	    if (OPS_GetNumRemainingInputArgs() < 1) {
		opserr << "WARNING not enough args, need -startPoint startPt??\n";
		opserr << " for random variable: " << tag << "\n";
		return -1;
	    }
	    if (OPS_GetDoubleInput(&numdata, &startPt) < 0) {
		opserr << "WARNING invalid starting point\n";
		opserr << " for random variable: " << tag << "\n";
		return -1;
	    }
            use_start_pt = 1;
	}
		
	// user input distribution specific parameters directly
	else if (strcmp(arg,"-parameters") == 0) {
	    if (OPS_GetNumRemainingInputArgs() < 1) {
		opserr << "WARNING not enough args, need -parameters param1 ...??\n";
		opserr << " for random variable: " << tag << "\n";
		return -1;
	    }

	    while (true) {
		if (OPS_GetDoubleInput(&numdata, &param) < 0) {
		    OPS_ResetCurrentInputArg(-1);
		    break;
		}
		parameters[parameters.Size()] = param;
	    }
	}
    }
    
    // GET INPUT PARAMETER (string) AND CREATE THE OBJECT
    int param_indx = parameters.Size();
    if (strcmp(dist,"normal") == 0) {
	if (param_indx > 0)
	    theRandomVariable = new NormalRV(tag, parameters);
	else
	    theRandomVariable = new NormalRV(tag, mean, stdv);
    }
	
    else if (strcmp(dist,"lognormal") == 0) {
	if (param_indx > 0)
	    theRandomVariable = new LognormalRV(tag, parameters);
	else
	    theRandomVariable = new LognormalRV(tag, mean, stdv);
    }
	
    else if (strcmp(dist,"gamma") == 0) {
	if (param_indx > 0)
	    theRandomVariable = new GammaRV(tag, parameters);
	else
	    theRandomVariable = new GammaRV(tag, mean, stdv);
    }
	
    else if (strcmp(dist,"shiftedExponential") == 0) {
	if (param_indx > 0)
	    theRandomVariable = new ShiftedExponentialRV(tag, parameters);
	else
	    theRandomVariable = new ShiftedExponentialRV(tag, mean, stdv);
    }
	
    else if (strcmp(dist,"shiftedRayleigh") == 0) {
	if (param_indx > 0)
	    theRandomVariable = new ShiftedRayleighRV(tag, parameters);
	else
	    theRandomVariable = new ShiftedRayleighRV(tag, mean, stdv);
    }
	
    else if (strcmp(dist,"exponential") == 0) {
	if (param_indx > 0)
	    theRandomVariable = new ExponentialRV(tag, parameters);
	else
	    theRandomVariable = new ExponentialRV(tag, mean, stdv);
    }

    else if (strcmp(dist,"rayleigh") == 0) {
	if (param_indx > 0)
	    theRandomVariable = new RayleighRV(tag, parameters);
	else {
	    opserr << "Rayleigh random variable with tag " << tag << " cannot be created with only mean/stdv." << "\n";
	    return TCL_ERROR;
	}
    }
	
    else if (strcmp(dist,"uniform") == 0) {
	if (param_indx > 0)
	    theRandomVariable = new UniformRV(tag, parameters);
	else
	    theRandomVariable = new UniformRV(tag, mean, stdv);
    }
	
    else if (strcmp(dist,"beta") == 0) {
	if (param_indx > 0)
	    theRandomVariable = new BetaRV(tag, parameters);
	else {
	    opserr << "Beta random variable with tag " << tag << " cannot be created with only mean/stdv." << "\n";
	    return TCL_ERROR;
	}
    }
	
    else if (strcmp(dist,"type1LargestValue") == 0) {
	if (param_indx > 0)
	    theRandomVariable = new Type1LargestValueRV(tag, parameters);
	else
	    theRandomVariable = new Type1LargestValueRV(tag, mean, stdv);
    }
	
    else if (strcmp(dist,"type1SmallestValue") == 0) {
	if (param_indx > 0)
	    theRandomVariable = new Type1SmallestValueRV(tag, parameters);
	else
	    theRandomVariable = new Type1SmallestValueRV(tag, mean, stdv);
    }
	
    else if (strcmp(dist,"type2LargestValue") == 0) {
	if (param_indx > 0)
	    theRandomVariable = new Type2LargestValueRV(tag, parameters);
	else
	    theRandomVariable = new Type2LargestValueRV(tag, mean, stdv);
    }
	
    else if (strcmp(dist,"type3SmallestValue") == 0) {
	if (param_indx > 0)
	    theRandomVariable = new Type3SmallestValueRV(tag, parameters);
	else {
	    opserr << "T3S random variable with tag " << tag << " cannot be created with only mean/stdv." << "\n";
	    return TCL_ERROR;
	}
    }
		
    else if (strcmp(dist,"chiSquare") == 0) {
	if (param_indx > 0)
	    theRandomVariable = new ChiSquareRV(tag, parameters);
	else
	    theRandomVariable = new ChiSquareRV(tag, mean, stdv);
    }
		
    else if (strcmp(dist,"gumbel") == 0) {
	if (param_indx > 0)
	    theRandomVariable = new GumbelRV(tag, parameters);
	else
	    theRandomVariable = new GumbelRV(tag, mean, stdv);
    }

    else if (strcmp(dist,"weibull") == 0) {
	if (param_indx > 0)
	    theRandomVariable = new WeibullRV(tag, parameters);
	else
	    theRandomVariable = new WeibullRV(tag, mean, stdv);
    }
	
    else if (strcmp(dist,"laplace") == 0) {
	if (param_indx > 0)
	    theRandomVariable = new LaplaceRV(tag, parameters);
	else
	    theRandomVariable = new LaplaceRV(tag, mean, stdv);
    }
	
    else if (strcmp(dist,"pareto") == 0) {
	if (param_indx > 0)
	    theRandomVariable = new ParetoRV(tag, parameters);
	else {
	    opserr << "Pareto random variable with tag " << tag << " cannot be created with only mean/stdv." << "\n";
	    return TCL_ERROR;
	}
    }

    else if (strcmp(dist,"userdefined") == 0) {
	// note userdefined is a special case and will not have any input read from the command line yet
	// unless user defined mean and standard deviation for some reason, which will break this input
	// because we assume argi starts at 3 here
        
        // KRM 4/22/2012 userdefined currently not implemented.
	// Vector xPoints;
	// Vector PDFpoints;
	// int numPoints = 0;
		
	// if (strcmp(argv[3],"-list") == 0) {
			
	//     numPoints = (argc-4) % 2;
	//     Vector temp_xPoints(numPoints);
	//     Vector temp_PDFpoints(numPoints);
			
	//     double x = 0.0;
	//     double pdf = 0.0;
	//     double x_old = 0.0;
			
	//     // Read the points
	//     for (int i=0; i < numPoints; i++) {
	// 	if (Tcl_GetDouble(interp, argv[4+2*i], &x) != TCL_OK) {
	// 	    opserr << "ERROR: Invalid x point to user-defined random variable." << "\n";
	// 	    return TCL_ERROR;
	// 	}
	// 	if (Tcl_GetDouble(interp, argv[5+2*i], &pdf) != TCL_OK) {
	// 	    opserr << "ERROR: Invalid PDF value point to user-defined random variable." << "\n";
	// 	    return TCL_ERROR;
	// 	}
	// 	if (i>0 && x<=x_old) {
	// 	    opserr << "ERROR: x-points to user-defined random variable must be consequtive!" << "\n";
	// 	    return TCL_ERROR;
	// 	}
	// 	temp_xPoints(i) = x;
	// 	temp_PDFpoints(i) = pdf;
	// 	x_old = x;
	//     }
			
	//     xPoints = temp_xPoints;
	//     PDFpoints = temp_PDFpoints;
			
	// }
	// else if (strcmp(argv[3],"-file") == 0) {
			
	//     // Open file where the vectors are given
	//     ifstream inputFile( argv[4], ios::in );
	//     if (inputFile.fail()) {
	// 	opserr << "File " << argv[4] << " could not be opened. " << "\n";
	// 	return TCL_ERROR;
	//     }
			
	//     // Loop through file to see how many entries there are
	//     double dummy;
	//     numPoints = 0;
	//     while (inputFile >> dummy) {
	// 	inputFile >> dummy;
	// 	numPoints++;
	//     }
	//     if (numPoints == 0) {
	// 	opserr << "ERROR: No entries in the direction file read by " << "\n"
	// 	       << "user-defined random variable, number " << tag << "\n";
	// 	return TCL_ERROR;
	//     }
			
	//     // rewind
	//     inputFile.clear(); 
	//     inputFile.seekg(0); 
			
	//     // Allocate vectors of correct size
	//     Vector temp_xPoints(numPoints);
	//     Vector temp_PDFpoints(numPoints);
						
	//     // Store the vector
	//     for (int i=0; i<numPoints; i++) {
	// 	inputFile >> temp_xPoints(i);
	// 	inputFile >> temp_PDFpoints(i);
	//     }
	//     inputFile.close();
			
	//     xPoints = temp_xPoints;
	//     PDFpoints = temp_PDFpoints;
	// }
	// else {
	//     opserr << "ERROR: Invalid argument to user-defined random variable, number " << tag << "\n";
	//     return TCL_ERROR;
	// }
		
	//theRandomVariable = new UserDefinedRV(tag, xPoints, PDFpoints);
		
    }

    else if (strcmp(dist,"python") == 0) {
#ifdef PY_VERSION
        if (filename == 0 || functionname == 0) {
	opserr << "ERROR: PythonRV filename or functionname not specified" << endln;
	return -1;
      }
      if (param_indx > 0)
	theRandomVariable = new PythonRV(tag, parameters, filename, functionname);
      else
	theRandomVariable = new PythonRV(tag, mean, stdv, filename, functionname);
#endif
    }
    
    else {
	opserr << "ERROR: unknown random variable type: " << dist << " provided. Must be one of " << "\n";
	return -1;
    }

    if (theRandomVariable == 0) {
	opserr << "ERROR: could not create random variable number " << tag << "\n";
	return -1;
    }
	
    // set start point on object if user provided
    if (use_start_pt == 1) {
	theRandomVariable->setStartValue(startPt);
        theRandomVariable->setCurrentValue(startPt);
    }
    else {
	theRandomVariable->setStartValue(theRandomVariable->getMean());
        theRandomVariable->setCurrentValue(theRandomVariable->getMean());
    }

    // Add the random variable to the domain
    ReliabilityDomain* theReliabilityDomain = cmds->getDomain();
    if (theReliabilityDomain->addRandomVariable(theRandomVariable) == false) {
	opserr << "ERROR: failed to add random variable to the domain (wrong number of arguments?)\n";
	opserr << "random variable: " << tag << "\n";
	delete theRandomVariable; // otherwise memory leak
	return -1;
    }

    //RVParameter *theRVParam = new RVParameter(tag, theRandomVariable);
    //theStructuralDomain->addParameter(theRVParam);

    return 0;

}

int OPS_getRVTags()
{
  ReliabilityDomain* theReliabilityDomain = cmds->getDomain();
  if (theReliabilityDomain == 0)
    return -1;

  std::vector<int> rvTags;
  RandomVariable *theRV;
  RandomVariableIter &rvIter = theReliabilityDomain->getRandomVariables();
  while ((theRV = rvIter()) != 0)
    rvTags.push_back(theRV->getTag());

  int size = 0;
  int *data = 0;
  if (!rvTags.empty()) {
      size = (int)rvTags.size();
      data = &rvTags[0];
  }

  if (OPS_SetIntOutput(&size,data,false) < 0) {
    opserr << "ERROR: failed to set outputs in getRVTags" << endln;
    return -1;
  }

  return 0;
}

int OPS_getRVMean()
{
  // CHECK THAT AT LEAST ENOUGH ARGUMENTS ARE GIVEN
  if (OPS_GetNumRemainingInputArgs() < 1) {
    opserr << "ERROR: invalid number of arguments to getMean command : getMean rvTag\n";
    return -1;
  }

  // GET TAG NUMBER
  int rvTag;
  int numData = 1;
  if (OPS_GetIntInput(&numData, &rvTag) < 0) {
    opserr << "ERROR: invalid input to getMean: tag \n";
    return -1;
  }
    
  ReliabilityDomain *theReliabilityDomain = cmds->getDomain();
  RandomVariable *rv = theReliabilityDomain->getRandomVariablePtr(rvTag);
  if (rv == 0) {
    opserr << "ERROR: getMean - random variable with tag " << rvTag << " not found" << endln;
    return -1;
  }

  double mean = rv->getMean();
  if (OPS_SetDoubleOutput(&numData, &mean, true) < 0) {
    opserr << "ERROR: getMean - failed to set double output\n";
    return -1;
  }
  
  return 0;
}

int OPS_getRVStdv()
{
  // CHECK THAT AT LEAST ENOUGH ARGUMENTS ARE GIVEN
  if (OPS_GetNumRemainingInputArgs() < 1) {
    opserr << "ERROR: invalid number of arguments to getStdv command : getStdv rvTag\n";
    return -1;
  }

  // GET TAG NUMBER
  int rvTag;
  int numData = 1;
  if (OPS_GetIntInput(&numData, &rvTag) < 0) {
    opserr << "ERROR: invalid input to getStdv: tag \n";
    return -1;
  }
    
  ReliabilityDomain *theReliabilityDomain = cmds->getDomain();
  RandomVariable *rv = theReliabilityDomain->getRandomVariablePtr(rvTag);
  if (rv == 0) {
    opserr << "ERROR: getStdv - random variable with tag " << rvTag << " not found" << endln;
    return -1;
  }

  double stdv = rv->getStdv();
  if (OPS_SetDoubleOutput(&numData, &stdv, true) < 0) {
    opserr << "ERROR: getStdv - failed to set double output\n";
    return -1;
  }
  
  return 0;
}

int OPS_getRVPDF()
{
  // CHECK THAT AT LEAST ENOUGH ARGUMENTS ARE GIVEN
  if (OPS_GetNumRemainingInputArgs() < 2) {
    opserr << "ERROR: invalid number of arguments to getPDF command : getPDF rvTag X\n";
    return -1;
  }

  // GET TAG NUMBER
  int rvTag;
  int numData = 1;
  if (OPS_GetIntInput(&numData, &rvTag) < 0) {
    opserr << "ERROR: invalid input to getPDF: tag \n";
    return -1;
  }

  double x;
  if (OPS_GetDoubleInput(&numData, &x) < 0) {
    opserr << "ERROR: invalid input to getPDF: x \n";
    return -1;
  }
  
  ReliabilityDomain *theReliabilityDomain = cmds->getDomain();
  RandomVariable *rv = theReliabilityDomain->getRandomVariablePtr(rvTag);
  if (rv == 0) {
    opserr << "ERROR: getPDF - random variable with tag " << rvTag << " not found" << endln;
    return -1;
  }

  double pdf = rv->getPDFvalue(x);
  if (OPS_SetDoubleOutput(&numData, &pdf, true) < 0) {
    opserr << "ERROR: getPDF - failed to set double output\n";
    return -1;
  }
  
  return 0;
}

int OPS_getRVCDF()
{
  // CHECK THAT AT LEAST ENOUGH ARGUMENTS ARE GIVEN
  if (OPS_GetNumRemainingInputArgs() < 2) {
    opserr << "ERROR: invalid number of arguments to getCDF command : getCDF rvTag X\n";
    return -1;
  }

  // GET TAG NUMBER
  int rvTag;
  int numData = 1;
  if (OPS_GetIntInput(&numData, &rvTag) < 0) {
    opserr << "ERROR: invalid input to getCDF: tag \n";
    return -1;
  }

  double x;
  if (OPS_GetDoubleInput(&numData, &x) < 0) {
    opserr << "ERROR: invalid input to getCDF: x \n";
    return -1;
  }
  
  ReliabilityDomain *theReliabilityDomain = cmds->getDomain();
  RandomVariable *rv = theReliabilityDomain->getRandomVariablePtr(rvTag);
  if (rv == 0) {
    opserr << "ERROR: getCDF - random variable with tag " << rvTag << " not found" << endln;
    return -1;
  }

  double cdf = rv->getCDFvalue(x);
  if (OPS_SetDoubleOutput(&numData, &cdf, true) < 0) {
    opserr << "ERROR: getCDF - failed to set double output\n";
    return -1;
  }
  
  return 0;
}

int OPS_getRVInverseCDF()
{
  // CHECK THAT AT LEAST ENOUGH ARGUMENTS ARE GIVEN
  if (OPS_GetNumRemainingInputArgs() < 2) {
    opserr << "ERROR: invalid number of arguments to getInverseCDF command : getInverseCDF rvTag p\n";
    return -1;
  }

  // GET TAG NUMBER
  int rvTag;
  int numData = 1;
  if (OPS_GetIntInput(&numData, &rvTag) < 0) {
    opserr << "ERROR: invalid input to getInverseCDF: tag \n";
    return -1;
  }

  double p;
  if (OPS_GetDoubleInput(&numData, &p) < 0) {
    opserr << "ERROR: invalid input to getInverseCDF: p \n";
    return -1;
  }
  
  ReliabilityDomain *theReliabilityDomain = cmds->getDomain();
  RandomVariable *rv = theReliabilityDomain->getRandomVariablePtr(rvTag);
  if (rv == 0) {
    opserr << "ERROR: getInverseCDF - random variable with tag " << rvTag << " not found" << endln;
    return -1;
  }

  double invcdf = rv->getInverseCDFvalue(p);
  if (OPS_SetDoubleOutput(&numData, &invcdf, true) < 0) {
    opserr << "ERROR: getInverseCDF - failed to set double output\n";
    return -1;
  }
  
  return 0;
}

int OPS_addCorrelate()
{
  if (OPS_GetNumRemainingInputArgs() < 3) {
    opserr << "ERROR: Wrong number of arguments to correlate command" << endln;
    return -1;
  }

  int rvTag[2];
  double correlationValue;

  int numData = 2;
  if (OPS_GetIntInput(&numData, rvTag) < 0) {
    opserr << "ERROR: invalid input to correlate: tag" << endln;;
    return -1;
  }

  numData = 1;
  if (OPS_GetDoubleInput(&numData, &correlationValue) < 0) {
    opserr << "ERROR: invalid input to correlate: value" << endln;;
    return -1;    
  }

  ReliabilityDomain *theReliabilityDomain = cmds->getDomain();
  int tag = theReliabilityDomain->getNumberOfCorrelationCoefficients();
  CorrelationCoefficient *theCorrelationCoefficient = new CorrelationCoefficient(tag+1, rvTag[0], rvTag[1], correlationValue);
  if (theCorrelationCoefficient == 0) {
    opserr << "ERROR: failed to add correlation coefficient to domain" << endln;
    return -1;
  }

  if (theReliabilityDomain->addCorrelationCoefficient(theCorrelationCoefficient) == false) {
    opserr << "ERROR: failed to add correlation coefficient to domain\n";
    opserr << "tag, rv1, rv2: " << tag << ' ' << rvTag[0] << ' ' << rvTag[1] << endln;
    return -1;
  }

  return 0;
}

void
OpenSeesReliabilityCommands::setProbabilityTransformation(ProbabilityTransformation *transform)
{
  if (theProbabilityTransformation != 0) {
    delete theProbabilityTransformation;
    theProbabilityTransformation = 0;
  }

  theProbabilityTransformation = transform;
  if (transform == 0)
    return;
}

void
OpenSeesReliabilityCommands::setRandomNumberGenerator(RandomNumberGenerator *generator)
{
  if (theRandomNumberGenerator != 0) {
    delete theRandomNumberGenerator;
    theRandomNumberGenerator = 0;
  }

  theRandomNumberGenerator = generator;
  if (generator == 0)
    return;
}

void
OpenSeesReliabilityCommands::setReliabilityConvergenceCheck(ReliabilityConvergenceCheck *check)
{
  if (theReliabilityConvergenceCheck != 0) {
    delete theReliabilityConvergenceCheck;
    theReliabilityConvergenceCheck = 0;
  }

  theReliabilityConvergenceCheck = check;
  if (check == 0)
    return;
}

void
OpenSeesReliabilityCommands::setSearchDirection(SearchDirection *search)
{
  if (theSearchDirection != 0) {
    delete theSearchDirection;
    theSearchDirection = 0;
  }

  theSearchDirection = search;
  if (search == 0)
    return;
}

void
OpenSeesReliabilityCommands::setMeritFunctionCheck(MeritFunctionCheck *merit)
{
  if (theMeritFunctionCheck != 0) {
    delete theMeritFunctionCheck;
    theMeritFunctionCheck = 0;
  }

  theMeritFunctionCheck = merit;
  if (merit == 0)
    return;
}

void
OpenSeesReliabilityCommands::setStepSizeRule(StepSizeRule *rule)
{
  if (theStepSizeRule != 0) {
    delete theStepSizeRule;
    theStepSizeRule = 0;
  }

  theStepSizeRule = rule;
  if (rule == 0)
    return;
}

void
OpenSeesReliabilityCommands::setRootFinding(RootFinding *root)
{
  if (theRootFinding != 0) {
    delete theRootFinding;
    theRootFinding = 0;
  }

  theRootFinding = root;
  if (root == 0)
    return;
}

void
OpenSeesReliabilityCommands::setGradientEvaluator(GradientEvaluator *eval)
{
  if (theGradientEvaluator != 0) {
    delete theGradientEvaluator;
    theGradientEvaluator = 0;
  }

  theGradientEvaluator = eval;
  if (eval == 0)
    return;
}

void OpenSeesReliabilityCommands::setFunctionEvaluator(
    FunctionEvaluator *eval) {
  if (theFunctionEvaluator != 0) {
    delete theFunctionEvaluator;
    theFunctionEvaluator = 0;
  }

  theFunctionEvaluator = eval;
  if (eval == 0) {
    return;
  }
}

int
OPS_startPoint()
{
  if (OPS_GetNumRemainingInputArgs() < 1) {
    opserr << "ERROR: wrong number of arguments to startPoint" << endln;
    return -1;
  }

  ReliabilityDomain *theReliabilityDomain = cmds->getDomain();
  int nrv = theReliabilityDomain->getNumberOfRandomVariables();
  RandomVariable *aRandomVariable;

  // Get the type of start point
  const char *type = OPS_GetString();
  int meanOrZero = -1;
  if (strcmp(type,"Mean") == 0)
    meanOrZero = 1;
  if (strcmp(type,"Zero") == 0)
    meanOrZero = 0;  

  RandomVariableIter rvIter = theReliabilityDomain->getRandomVariables();
  while ((aRandomVariable = rvIter()) != 0) {
    if (meanOrZero == 0)
      aRandomVariable->setStartValue(0.0);
    if (meanOrZero == 1) {
      double mean = aRandomVariable->getMean();
      aRandomVariable->setStartValue(mean);      
    }
  }

  if (strcmp(type,"-file") == 0) {
    opserr << "ERROR: startPoint read from file option not implemented" << endln;
    return -1;
  }

  return 0;
}

int
OPS_randomNumberGenerator()
{
  if (OPS_GetNumRemainingInputArgs() < 1) {
    opserr << "ERROR: wrong number of arguments to randomNumberGenerator" << endln;
    return -1;
  }

  // Get the type of generator
  const char *type = OPS_GetString();
  if (strcmp(type,"CStdLib") != 0) {
    opserr << "ERROR: unrecognized type of RandomNumberGenerator " << type << endln;
    return -1;
  }

  RandomNumberGenerator *theGenerator = new CStdLibRandGenerator();
  if (theGenerator == 0) {
    opserr << "ERROR: could not create randomNumberGenerator" << endln;
    return -1;
  } 
  if (cmds != 0) {
    cmds->setRandomNumberGenerator(theGenerator);
  }
  
  return 0;
}

int
OPS_reliabilityConvergenceCheck()
{
  if (OPS_GetNumRemainingInputArgs() < 1) {
    opserr << "ERROR: wrong number of arguments to reliabilityConvergenceCheck" << endln;
    return -1;
  }

  // Get the type of convergence check
  const char *type = OPS_GetString();

  double e1 = 1.0e-3;
  double e2 = 1.0e-3;  
  double scaleValue = 0.0;
  int print = 1;

  // Standard and Optimality convergence checks have the same optional arguments
  while (OPS_GetNumRemainingInputArgs() > 0) {
    const char *arg = OPS_GetString();
    int numdata = 1;
    if (strcmp(arg,"-e1") == 0 && OPS_GetNumRemainingInputArgs() > 0) {
      if (OPS_GetDoubleInput(&numdata,&e1) < 0) {
	opserr << "ERROR: unable to read -e1 value for reliability convergence check" << endln;
	return -1;
      }
    }
    if (strcmp(arg,"-e2") == 0 && OPS_GetNumRemainingInputArgs() > 0) {
      if (OPS_GetDoubleInput(&numdata,&e2) < 0) {
	opserr << "ERROR: unable to read -e2 value for reliability convergence check" << endln;
	return -1;
      }
    }
    if (strcmp(arg,"-scaleValue") == 0 && OPS_GetNumRemainingInputArgs() > 0) {
      if (OPS_GetDoubleInput(&numdata,&scaleValue) < 0) {
	opserr << "ERROR: unable to read -scaleValue value for reliability convergence check" << endln;
	return -1;
      }
    }
    if (strcmp(arg,"-print") == 0 && OPS_GetNumRemainingInputArgs() > 0) {
      if (OPS_GetIntInput(&numdata,&print) < 0) {
	opserr << "ERROR: unable to read -print value for reliability convergence check" << endln;
	return -1;
      }
    }            
  }

  ReliabilityConvergenceCheck *theCheck = 0;
  if (strcmp(type,"Standard") == 0) {
    theCheck = new StandardReliabilityConvergenceCheck(e1,e2,scaleValue,print);
  }
  else if (strcmp(type,"OptimalityCondition") == 0) {
    theCheck = new OptimalityConditionReliabilityConvergenceCheck(e1,e2,scaleValue,print);
  }
  else {
    opserr << "ERROR: unrecognized type of reliabilityConvergenceCheck " << type << endln;
    return -1;
  }

  if (theCheck == 0) {
    opserr << "ERROR: could not create reliabilityConvergenceCheck" << endln;
    return -1;
  } else {
    if (cmds != 0)
      cmds->setReliabilityConvergenceCheck(theCheck);
  }
  
  return 0;
}

int
OPS_searchDirection()
{
  if (OPS_GetNumRemainingInputArgs() < 1) {
    opserr << "ERROR: wrong number of arguments to searchDirection" << endln;
    return -1;
  }

  // Get the type of search direction
  const char *type = OPS_GetString();

  SearchDirection *theSearch = 0;
  if (strcmp(type,"iHLRF") == 0) {
    theSearch = new HLRFSearchDirection();
  }
  else if (strcmp(type,"PolakHe") == 0) {
    double gamma = 1.0;
    double delta = 1.0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
      const char *arg = OPS_GetString();
      int numdata = 1;
      if (strcmp(arg,"-gamma") == 0 && OPS_GetNumRemainingInputArgs() > 0) {
	if (OPS_GetDoubleInput(&numdata,&gamma) < 0) {
	  opserr << "ERROR: unable to read -gamma value for PolakHe search direction" << endln;
	  return -1;
	}
      }
      if (strcmp(arg,"-delta") == 0 && OPS_GetNumRemainingInputArgs() > 0) {
	if (OPS_GetDoubleInput(&numdata,&delta) < 0) {
	  opserr << "ERROR: unable to read -delta value for PolakHe search direction" << endln;
	  return -1;
	}
      }      
    }
    theSearch = new PolakHeSearchDirectionAndMeritFunction(gamma, delta);
  }
  else if (strcmp(type,"GradientProjection") == 0) {
    opserr << "ERROR: GradientProjection search direction not yet implemented" << endln;
    return -1;
  }
  else if (strcmp(type,"SQP") == 0) {
    opserr << "ERROR: SQP search direction not yet implemented" << endln;
    return -1;
  }
  else {
    opserr << "ERROR: unrecognized type of searchDirection " << type << endln;
    return -1;
  }  

  if (theSearch == 0) {
    opserr << "ERROR: could not create searchDirection" << endln;
    return -1;
  } else {
    if (cmds != 0)
      cmds->setSearchDirection(theSearch);
  }
  
  return 0;
}

int
OPS_meritFunctionCheck()
{
  if (OPS_GetNumRemainingInputArgs() < 1) {
    opserr << "ERROR: wrong number of arguments to meritFunctionCheck" << endln;
    return -1;
  }

  // Get the type of merit function check
  const char *type = OPS_GetString();

  MeritFunctionCheck *theFunction = 0;
  if (strcmp(type,"AdkZhang") == 0) {
    double multi = 2.0;
    double add = 2.0;
    double factor = 0.0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
      const char *arg = OPS_GetString();
      int numdata = 1;
      if (strcmp(arg,"-multi") == 0 && OPS_GetNumRemainingInputArgs() > 0) {
	if (OPS_GetDoubleInput(&numdata,&multi) < 0) {
	  opserr << "ERROR: unable to read -multi value for AdkZhang merit function check" << endln;
	  return -1;
	}
      }
      if (strcmp(arg,"-add") == 0 && OPS_GetNumRemainingInputArgs() > 0) {
	if (OPS_GetDoubleInput(&numdata,&add) < 0) {
	  opserr << "ERROR: unable to read -add value for AdkZhang merit function check" << endln;
	  return -1;
	}
      }
      if (strcmp(arg,"-factor") == 0 && OPS_GetNumRemainingInputArgs() > 0) {
	if (OPS_GetDoubleInput(&numdata,&factor) < 0) {
	  opserr << "ERROR: unable to read -factor value for AdkZhang merit function check" << endln;
	  return -1;
	}
      }
    }
      
    // Do a quick input check
    if (multi < 1.0 || add < 0.0) {
      opserr << "ERROR: Invalid values of multi/add parameters to AdkZhang merit function check" << endln;
      return -1;
    }
    
    theFunction = new AdkZhangMeritFunctionCheck(multi,add,factor);      
  }
  else if (strcmp(type,"PolakHe") == 0) {
    opserr << "ERROR: PolakHe merit function check not yet implemented" << endln;
    return -1;
  }
  else if (strcmp(type,"SQP") == 0) {
    opserr << "ERROR: SQP merit function check not yet implemented" << endln;
    return -1;
  }
  else {
    opserr << "ERROR: unrecognized type of meritFunctionCheck " << type << endln;
    return -1;
  }  

  if (theFunction == 0) {
    opserr << "ERROR: could not create meritFunctionCheck" << endln;
    return -1;
  } else {
    if (cmds != 0)
      cmds->setMeritFunctionCheck(theFunction);
  }
  
  return 0;
}

int
OPS_stepSizeRule()
{
  if (OPS_GetNumRemainingInputArgs() < 1) {
    opserr << "ERROR: wrong number of arguments to stepSizeRule" << endln;
    return -1;
  }

  // Get the type of step size rule
  const char *type = OPS_GetString();

  StepSizeRule *theRule = 0;
  if (strcmp(type,"Armijo") == 0) {
    double base = 0.5;
    int maxNumReductions = 10;
    double b0 = 1.0;
    int numberOfShortSteps = 2;
    double radius = 50.0;
    double surfaceDistance = 0.1;
    double evolution = 0.5;
    int printFlag = 0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
      const char *arg = OPS_GetString();
      int numdata = 1;
      if (strcmp(arg,"-print") == 0 && OPS_GetNumRemainingInputArgs() > 0) {
	if (OPS_GetIntInput(&numdata,&printFlag) < 0) {
	  opserr << "ERROR: unable to read -print value for Armijo step size rule" << endln;
	  return -1;
	}
      }
      if (strcmp(arg,"-maxNum") == 0 && OPS_GetNumRemainingInputArgs() > 0) {
	if (OPS_GetIntInput(&numdata,&maxNumReductions) < 0) {
	  opserr << "ERROR: unable to read -maxNum value for Armijo step size rule" << endln;
	  return -1;
	}
      }
      if (strcmp(arg,"-base") == 0 && OPS_GetNumRemainingInputArgs() > 0) {
	if (OPS_GetDoubleInput(&numdata,&base) < 0) {
	  opserr << "ERROR: unable to read -base value for Armijo step size rule" << endln;
	  return -1;
	}
      }
      if (strcmp(arg,"-initial") == 0 && OPS_GetNumRemainingInputArgs() > 1) {
	if (OPS_GetDoubleInput(&numdata,&b0) < 0) {
	  opserr << "ERROR: unable to read -initial b0 value for Armijo step size rule" << endln;
	  return -1;
	}
	if (OPS_GetIntInput(&numdata,&numberOfShortSteps) < 0) {
	  opserr << "ERROR: unable to read -initial numberOfShortSteps value for Armijo step size rule" << endln;
	  return -1;
	}	
      }
      if (strcmp(arg,"-sphere") == 0 && OPS_GetNumRemainingInputArgs() > 2) {
	if (OPS_GetDoubleInput(&numdata,&radius) < 0) {
	  opserr << "ERROR: unable to read -sphere radius value for Armijo step size rule" << endln;
	  return -1;
	}
	if (OPS_GetDoubleInput(&numdata,&surfaceDistance) < 0) {
	  opserr << "ERROR: unable to read -sphere surfaceDistance value for Armijo step size rule" << endln;
	  return -1;
	}
	if (OPS_GetDoubleInput(&numdata,&evolution) < 0) {
	  opserr << "ERROR: unable to read -sphere evolution value for Armijo step size rule" << endln;
	  return -1;
	}		
      }                        
    }

    ReliabilityDomain *theRelDomain = cmds->getDomain();
    RootFinding *theAlgorithm = cmds->getRootFinding();
    ProbabilityTransformation *theTransformation = cmds->getProbabilityTransformation();
    if (theTransformation == 0) {
      opserr << "Probability transformation must be defined before ArmijoStepSize rule" << endln;
      return -1;
    }
    FunctionEvaluator *theEvaluator = cmds->getFunctionEvaluator();
    if (theEvaluator == 0) {
      opserr << "Function evaluator must be defined before ArmijoStepSize rule" << endln;
      return -1;
    }
    MeritFunctionCheck *theMeritFunction = cmds->getMeritFunctionCheck();
    if (theMeritFunction == 0) {
      opserr << "Merit function check must be defined before ArmijoStepSize rule" << endln;
      return -1;
    }
    
    theRule = new ArmijoStepSizeRule(theRelDomain, theEvaluator, theTransformation,
				     theMeritFunction, theAlgorithm,
				     base, maxNumReductions,
				     b0, numberOfShortSteps,
				     radius, surfaceDistance, evolution,
				     printFlag);      
  }
  else if (strcmp(type,"Fixed") == 0) {
    double stepSize = 1.0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
      const char *arg = OPS_GetString();
      int numdata = 1;
      if (strcmp(arg,"-stepSize") == 0 && OPS_GetNumRemainingInputArgs() > 0) {
	if (OPS_GetDoubleInput(&numdata,&stepSize) < 0) {
	  opserr << "ERROR: unable to read -stepSize value for Fixed step size rule" << endln;
	  return -1;
	}
      }
    }
    theRule = new FixedStepSizeRule(stepSize);
  }
  else {
    opserr << "ERROR: unrecognized type of stepSizeRule " << type << endln;
    return -1;
  }  

  if (theRule == 0) {
    opserr << "ERROR: could not create stepSizeRule" << endln;
    return -1;
  } else {
    if (cmds != 0)
      cmds->setStepSizeRule(theRule);
  }
  
  return 0;
}

int
OPS_rootFinding()
{
  if (OPS_GetNumRemainingInputArgs() < 1) {
    opserr << "ERROR: wrong number of arguments to rootFinding" << endln;
    return -1;
  }

  // Get the type of rootFinding
  const char *type = OPS_GetString();

  int maxIter = 50;
  double tol = 1.0e-3;
  double maxStepLength = 1.0;
  RootFinding *theFinding = 0;
  while (OPS_GetNumRemainingInputArgs() > 0) {
    const char *arg = OPS_GetString();
    int numdata = 1;
    if (strcmp(arg,"-maxIter") == 0 && OPS_GetNumRemainingInputArgs() > 0) {
      if (OPS_GetIntInput(&numdata,&maxIter) < 0) {
	opserr << "ERROR: unable to read -maxIter value for " << type << " root finding" << endln;
	return -1;
      }
    }
    if (strcmp(arg,"-tol") == 0 && OPS_GetNumRemainingInputArgs() > 0) {
      if (OPS_GetDoubleInput(&numdata,&tol) < 0) {
	opserr << "ERROR: unable to read -tol value for " << type << " root finding" << endln;
	return -1;
      }
    }
    if (strcmp(arg,"-maxStepLength") == 0 && OPS_GetNumRemainingInputArgs() > 0) {
      if (OPS_GetDoubleInput(&numdata,&maxStepLength) < 0) {
	opserr << "ERROR: unable to read -maxStepLength value for " << type << " root finding" << endln;
	return -1;
      }
    }        
  }

  if (strcmp(type,"Secant") == 0) {
    ReliabilityDomain *theRelDomain = cmds->getDomain();
    ProbabilityTransformation *theTransformation = cmds->getProbabilityTransformation();
    if (theTransformation == 0) {
      opserr << "Probability transformation must be defined before ArmijoStepSize rule" << endln;
      return -1;
    }
    FunctionEvaluator *theEvaluator = cmds->getFunctionEvaluator();
    if (theEvaluator == 0) {
      opserr << "Function evaluator must be defined before ArmijoStepSize rule" << endln;
      return -1;
    }    
    theFinding = new SecantRootFinding(theRelDomain,
				       theTransformation,
				       theEvaluator,
				       maxIter, tol, maxStepLength);
  }
  else {
    opserr << "ERROR: unrecognized type of rootFinding: " << type << endln;
    return -1;
  }  
  
  if (theFinding == 0) {
    opserr << "ERROR: could not create rootFinding" << endln;
    return -1;
  } else {
    if (cmds != 0)
      cmds->setRootFinding(theFinding);
  }
  
  return 0;
}

int OPS_functionEvaluator() {
  if (OPS_GetNumRemainingInputArgs() < 1) {
    opserr << "ERROR: wrong number of arguments to functionEvaluator"
           << endln;
    return -1;
  }

  if (cmds == 0) {
    opserr << "WARING: Reliability is not initialized\n";
    return -1;
  }
  if (cmds->getStructuralDomain() == 0) {
    opserr << "WARING: Reliability has no structural domain\n";
    return -1;
  }
  if (cmds->getDomain() == 0) {
    opserr << "WARING: Reliability has no domain\n";
    return -1;
  }

  FunctionEvaluator *theEval = 0;

  // Get the type of functionEvaluator
  const char *type = OPS_GetString();
  const char *filename = 0;
  if (OPS_GetNumRemainingInputArgs() > 0) {
    filename = OPS_GetString();
  }
  if (strcmp(type, "Matlab") == 0) {
    opserr << "ERROR: Matlab function evaluator not implemented" << endln;
    return -1;
  } else if (strcmp(type, "Tcl") == 0) {
    opserr << "ERROR: Tcl function evaluator not implemented" << endln;
    return -1;
  } else if (strcmp(type, "Python") == 0) {
#ifdef _PYTHON3
    if (filename == 0) {
      theEval = new PythonEvaluator(cmds->getDomain(),
                                    cmds->getStructuralDomain());
    } else {
      theEval = new PythonEvaluator(cmds->getDomain(),
                                    cmds->getStructuralDomain(), filename);
    }
#else
    opserr << "ERROR: Python function evaluator not implemented" << endln;
    return -1;
#endif
  } else {
    opserr << "ERROR: unrecognized type of function evaluator: " << type
           << endln;
    return -1;
  }

  if (theEval == 0) {
    opserr << "ERROR: could not create function evaluator" << endln;
    return -1;
  } else {
    cmds->setFunctionEvaluator(theEval);
  }

  return 0;
}

int
OPS_gradientEvaluator()
{
  if (OPS_GetNumRemainingInputArgs() < 1) {
    opserr << "ERROR: wrong number of arguments to gradientEvaluator" << endln;
    return -1;
  }

  GradientEvaluator *theEval = 0;
  
  // Get the type of gradientEvaluator
  const char *type = OPS_GetString();
  if (strcmp(type,"FiniteDifference") == 0) {
    double perturbationFactor = 1000.0;
    bool doGradientCheck = false;
    while (OPS_GetNumRemainingInputArgs() > 0) {
      const char *arg = OPS_GetString();
      int numdata = 1;
      if (strcmp(arg,"-pert") == 0 && OPS_GetNumRemainingInputArgs() > 0) {
	if (OPS_GetDoubleInput(&numdata,&perturbationFactor) < 0) {
	  opserr << "ERROR: unable to read -pert value for " << type << " gradient evaluator" << endln;
	  return -1;
	}
      }
      if (strcmp(arg,"-check") == 0) {
	doGradientCheck = true;
      }
    }

    ReliabilityDomain *theRelDomain = cmds->getDomain();
    Domain *theStrDomain = cmds->getStructuralDomain();
    FunctionEvaluator *theEvaluator = cmds->getFunctionEvaluator();
    if (theEvaluator == 0) {
      opserr << "Function evaluator must be defined before gradient evaluator" << endln;
      return -1;
    }
    
    theEval = new FiniteDifferenceGradient(theEvaluator, theRelDomain, theStrDomain);
  }
  else if (strcmp(type,"OpenSees") == 0 || strcmp(type,"Implicit") == 0) {
    bool doGradientCheck = false;
    while (OPS_GetNumRemainingInputArgs() > 0) {
      const char *arg = OPS_GetString();
      if (strcmp(arg,"-check") == 0) {
	doGradientCheck = true;
      }
    }

    ReliabilityDomain *theRelDomain = cmds->getDomain();
    Domain *theStrDomain = cmds->getStructuralDomain();
    FunctionEvaluator *theEvaluator = cmds->getFunctionEvaluator();
    if (theEvaluator == 0) {
      opserr << "Function evaluator must be defined before gradient evaluator" << endln;
      return -1;
    }    
    theEval = new ImplicitGradient(theEvaluator, theRelDomain, theStrDomain,
				   0/*theSensitivityAlgorithm*/);
    
  }
  else {
    opserr << "ERROR: unrecognized type of gradient evaluator: " << type << endln;
    return -1;
  }  

  if (theEval == 0) {
    opserr << "ERROR: could not create function evaluator" << endln;
    return -1;
  } else {
    if (cmds != 0)
      cmds->setGradientEvaluator(theEval);
  }
  
  return 0;
}

int OPS_probabilityTransformation()
{
  if (OPS_GetNumRemainingInputArgs() < 1) {
    opserr << "ERROR: wrong number of arguments to probabilityTransformation" << endln;
    return -1;
  }

  // Get transformation type
  const char* type = OPS_GetString();

  ReliabilityDomain *theReliabilityDomain = cmds->getDomain();
  ProbabilityTransformation *theTransf = 0;

  // Nataf and AllIndependent have the same optional arguments
  int print = 0;
  while (OPS_GetNumRemainingInputArgs() > 0) {
    const char *arg = OPS_GetString();
    int numdata = 1;
    if (strcmp(arg,"-print") == 0 && OPS_GetNumRemainingInputArgs() > 0) {
      if (OPS_GetIntInput(&numdata,&print) < 0) {
	opserr << "ERROR: unable to read -print value for probability transformation" << endln;
	return -1;
      }
    }            
  }
  
  if (strcmp(type,"Nataf") == 0) {
    theTransf = new NatafProbabilityTransformation(theReliabilityDomain, print);
  }

  else if (strcmp(type,"AllIndependent") == 0) {
    theTransf = new AllIndependentTransformation(theReliabilityDomain, print);
  }

  else {
    opserr << "ERROR: unrecognized type of probabilityTransformation " << type << endln;
    return -1;
  }

  if (theTransf == 0) {
    opserr << "ERROR: could not create probabilityTransformation" << endln;
    return -1;
  } else {
    if (cmds != 0)
      cmds->setProbabilityTransformation(theTransf);
  }
  
  return 0;
}

int OPS_transformUtoX()
{
  ProbabilityTransformation *theTransf = cmds->getProbabilityTransformation();
  if (theTransf == 0) {
    opserr << "ERROR: probability transformation has not been set" << endln;
    return -1;
  }

  ReliabilityDomain* theReliabilityDomain = cmds->getDomain();
  int nrv = theReliabilityDomain->getNumberOfRandomVariables();

  if (OPS_GetNumRemainingInputArgs() < nrv) {
    opserr << "ERROR: transformUtoX insufficient # args" << endln;
    return -1;
  }
  if (OPS_GetNumRemainingInputArgs() > nrv && OPS_GetNumRemainingInputArgs() < 2*nrv) {
    opserr << "ERROR: transformUtoX insufficient # rv tags" << endln;
    return -1;
  }

  int numData = 1;
  double val;
  Vector u(nrv);
  int loc = 0;
  // Read in u-values
  while (loc < nrv && OPS_GetNumRemainingInputArgs() > 0) {
    if (OPS_GetDoubleInput(&numData,&val) < 0) {
      OPS_ResetCurrentInputArg(-1);
      break;
    }
    u(loc) = val;
    loc++;
  }

  ID rvIndex(nrv);
  // Initialize the index to be sequential (default)
  for (int i = 0; i < nrv; i++)
    rvIndex(i) = i;

  int rvTag;
  loc = 0;
  // Now read in rv tags and get their indices
  while (loc < nrv && OPS_GetNumRemainingInputArgs() > 0) {
    if (OPS_GetIntInput(&numData,&rvTag) < 0) {
      OPS_ResetCurrentInputArg(-1);
      break;
    }
    rvIndex(loc) = theReliabilityDomain->getRandomVariableIndex(rvTag);
    loc++;
  }
  
  // Map in
  Vector uSorted(nrv);
  for (int i = 0; i < nrv; i++)
    uSorted(rvIndex(i)) = u(i);

  Vector x(nrv);
  theTransf->transform_u_to_x(uSorted, x);

  // Map out
  Vector xSorted(nrv);
  for (int i = 0; i < nrv; i++)
    xSorted(i) = x(rvIndex(i));

  if (OPS_SetDoubleOutput(&nrv, &xSorted[0], false) < 0) {
    opserr << "ERROR: failed to set output in transformUtoX" << endln;
    return -1;
  }
  
  return 0;
}
