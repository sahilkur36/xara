//
// Created by Alex Hartloper on 17.04.18.
//
#include <Vector.h>
#include <cmath>
#include <Logging.h>
#include "UVCuniaxial.h"

#include <elementAPI.h>
#include <Channel.h>
#include <Information.h>
#include <Parameter.h>

#include <elementAPI.h>
#include <OPS_Globals.h>


template <typename T> static int 
sgn(T val) {
  return (T(0) < val) - (val < T(0));
}


/* ------------------------------------------------------------------------ */

// NOTE: Do not use the OPS_GetNumRemainingInputArgs() function or the
// OPS_GetString() function: causes crash with .dll
void * OPS_ADD_RUNTIME_VPV(OPS_UVCuniaxial) {
  static int numUVCuniaxial = 0;
  if (numUVCuniaxial == 0) {
    opslog << "Using the UVCuniaxial material, see "
      "https://www.epfl.ch/labs/resslab/resslab-tools/" << endln;
    numUVCuniaxial++;
  }
  UniaxialMaterial* theMaterial = 0;

  // Parameters for parsing
  const int N_TAGS = 1;
  const int N_BASIC_PROPERTIES = 4;
  const int N_UPDATED_PROPERTIES = 2;
  const int N_PARAM_PER_BACK = 2;
  const int MAX_BACKSTRESSES = 8;
  const int BACKSTRESS_SPACE = MAX_BACKSTRESSES * N_PARAM_PER_BACK;

  std::string inputInstructions = "Invalid args, want:\n"
    "uniaxialMaterial UVCuniaxial "
    "tag? E? fy? QInf? b? DInf? a? "
    "N? C1? gamma1? <C2? gamma2? C3? gamma3? ... C8? gamma8?>\n"
    "Note: to neglect the updated model, set DInf = 0.0";

  // Containers for the inputs
  int nInputsToRead;
  int nBackstresses[1];  // for N
  int materialTag[1];  // for the tag
  double basicProps[4];  // holds E, fy, QInf, b
  double updProps[2];  // holds DInf, a
  double backstressProps[BACKSTRESS_SPACE];  // holds C's and gamma's
  std::vector<double> cK;
  std::vector<double> gammaK;

  // Get the material tag
  nInputsToRead = N_TAGS;
  if (OPS_GetIntInput(&nInputsToRead, materialTag) != 0) {
    opserr << "WARNING invalid uniaxialMaterial UVCuniaxial tag" << endln;
    return 0;
  }

  // Get E, fy, qInf, b
  nInputsToRead = N_BASIC_PROPERTIES;
  if (OPS_GetDoubleInput(&nInputsToRead, basicProps) != 0) {
    opserr << inputInstructions.c_str() << endln;
    return 0;
  }

  // Read in the updated model parameters
  nInputsToRead = N_UPDATED_PROPERTIES;
  if (OPS_GetDoubleInput(&nInputsToRead, updProps) != 0) {
    opserr << inputInstructions.c_str() << endln;
    return 0;
  }

  // Get the number of backstresses
  nInputsToRead = 1;
  if (OPS_GetIntInput(&nInputsToRead, nBackstresses) != 0) {
    opserr << "WARNING N must be an integer" <<
      inputInstructions.c_str() << endln;
    return 0;
  }
  if (nBackstresses[0] > MAX_BACKSTRESSES) {
    opserr << "WARNING: Too many backstresses defined, maximum is: " <<
      MAX_BACKSTRESSES << endln <<
      inputInstructions.c_str() << endln;
    return 0;
  }

  // Get the backstress parameters
  nInputsToRead = 2 * nBackstresses[0];
  if (OPS_GetDoubleInput(&nInputsToRead, backstressProps) != 0) {
    opserr << inputInstructions.c_str() << endln;
    return 0;
  }
  // cK's alternate with gammaK's
  for (int i = 0; i < nBackstresses[0]; ++i) {
    cK.push_back(backstressProps[2 * i]);
    gammaK.push_back(backstressProps[1 + 2 * i]);
  }

  // Allocate the material
  theMaterial = new UVCuniaxial(materialTag[0],
    basicProps[0], basicProps[1],
    basicProps[2], basicProps[3],
    updProps[0], updProps[1],
    cK, gammaK);

  return theMaterial;
}


/* ------------------------------------------------------------------------ */

/**
 *
 * @param tag Material tag
 * @param E elastic modulus
 * @param sy0 initial yield stress
 * @param qInf increase in yield surface size at isotropic saturation (>= 0)
 * @param b controls the saturation rate of the isotropic hardening, must be
 nonzero
 * @param dInf difference in initial and steady state yield stress (>=0), set
 *              this to zero for the original Chaboche model
 * @param a controls the saturation of dInf, must be nonzero
 * @param cK backstress kinematic hardening moduli
 * @param gammaK controls the saturation rate of the kinematic hardening
 */
UVCuniaxial::UVCuniaxial(int tag, double E, double sy0, double qInf, double b,
  double dInf, double a,
  std::vector<double> cK, std::vector<double> gammaK)
  : UniaxialMaterial(tag, MAT_TAG_UVCuniaxial),
  elasticModulus(E),
  yieldStress(sy0),
  qInf(qInf),
  bIso(b),
  dInf(dInf),
  aIso(a),
  cK(cK),
  gammaK(gammaK),
  strainConverged(0.),
  strainTrial(0.),
  strainPEqConverged(0.),
  strainPEqTrial(0.),
  stressConverged(0.),
  stressTrial(0.),
  stiffnessInitial(E),
  stiffnessConverged(E),
  stiffnessTrial(E),
  flowDirection(0.),
  plasticLoading(false)
{
  nBackstresses = cK.size();
  if (nBackstresses > MAX_BACKSTRESSES) {
    opserr << "UVCuniaxial - num backstresses (" << nBackstresses << ") exceeds maximum\n";
    opserr << "  continuing with first " << MAX_BACKSTRESSES << " backstresses" << endln;
  }
  for (int i = 0; i < nBackstresses; ++i) {
    alphaKTrial.push_back(0.);
    alphaKConverged.push_back(0.);
  }
}

UVCuniaxial::UVCuniaxial()
  : UniaxialMaterial(0, MAT_TAG_UVCuniaxial),
  elasticModulus(0.),
  yieldStress(0.),
  qInf(0.),
  bIso(0.),
  dInf(0.),
  aIso(0.),
  cK(0.),
  gammaK(0.),
  strainConverged(0.),
  strainTrial(0.),
  strainPEqConverged(0.),
  strainPEqTrial(0.),
  stressConverged(0.),
  stressTrial(0.),
  stiffnessInitial(0.),
  stiffnessConverged(0.),
  stiffnessTrial(0.),
  flowDirection(0.),
  plasticLoading(false)
{

}

/* ------------------------------------------------------------------------ */

UVCuniaxial::~UVCuniaxial() {
  // todo: figure out who deletes the copies!!!
}

/* ------------------------------------------------------------------------ */

/**
 *
 * @param strainIncrement change in strain from the converged strain value
 */
void UVCuniaxial::returnMapping(double strainIncrement) {

  // Initialize all the variables
  double dit = 0.;
  double plasticStrainIncrement = 0.;
  double aux = 0.;
  double ePTotal = 0.;

  // Yield criteria
  double alpha = 0.;
  for (int i = 0; i < nBackstresses; ++i)
    alpha += alphaKConverged[i];

  double ePEq = strainPEqConverged;
  double sigmaY1 = qInf * (1. - exp(-bIso * ePEq));
  double sigmaY2 = dInf * (1. - exp(-aIso * ePEq));
  stressTrial = stressConverged + elasticModulus * strainIncrement;

  double sy = yieldStress + sigmaY1 - sigmaY2;
  double stressRadius = stressTrial - alpha;
  double phi = stressRadius*stressRadius - sy*sy;

  const double RETURN_MAP_TOL = 10.0e-10;
  const int MAXIMUM_ITERATIONS = 1000;
  // Determine if have elastic or plastic loading
  bool converged = true;
  if (phi > RETURN_MAP_TOL) 
    converged = false;

  int iterationNumber = 0;
  while ((!converged) && (iterationNumber < MAXIMUM_ITERATIONS)) {
    iterationNumber++;

    aux = elasticModulus;
    for (int i = 0; i < nBackstresses; ++i) {
      aux = aux + cK[i] - sgn<double>(stressRadius) * gammaK[i] * alphaKTrial[i];
    }

    // Calculate the plastic strain from the strain increment
    dit = 2. * stressRadius * aux +
          sgn<double>(stressRadius) * 2. * sy * qInf * bIso * exp(-bIso * ePEq) -
          sgn<double>(stressRadius) * 2. * sy * dInf * aIso * exp(-aIso * ePEq);
    plasticStrainIncrement = phi / dit;

    // Prevent Newton step from overshooting
    if (abs(plasticStrainIncrement) > abs(stressTrial / elasticModulus))
      plasticStrainIncrement = sgn(plasticStrainIncrement) * 0.95 
                              *abs(stressTrial / elasticModulus);

    // Update the variables
    ePTotal = ePTotal + plasticStrainIncrement;
    ePEq = strainPEqConverged + abs(ePTotal);
    stressTrial = stressTrial - elasticModulus * plasticStrainIncrement;
    sigmaY1 = qInf * (1. - exp(-bIso * ePEq));
    sigmaY2 = dInf * (1. - exp(-aIso * ePEq));
    sy = yieldStress + sigmaY1 - sigmaY2;

    alpha = 0.;
    for (int i = 0; i < nBackstresses; ++i) {
      alphaKTrial[i] = sgn<double>(stressRadius) * cK[i] / gammaK[i] -
        (sgn<double>(stressRadius) * cK[i] / gammaK[i] - alphaKConverged[i]) *
        exp(-gammaK[i] * (ePEq - strainPEqConverged));
      alpha += alphaKTrial[i];
    }

    // Check convergence
    stressRadius = stressTrial - alpha;
    phi = pow(stressRadius, 2) - pow(sy, 2);
    if (abs(phi) < RETURN_MAP_TOL) {
      converged = true;
    }
  }

  // Warn the user if the algorithm did not converge
  if (iterationNumber == MAXIMUM_ITERATIONS - 1) {
    opserr << "WARNING: return mapping in UVCuniaxial does not converge!" << endln;
    opserr << "\tStrain increment = " << strainIncrement << endln;
    opserr << "\tExiting with phi = " << phi << " > " << RETURN_MAP_TOL << endln;
  }

  // Condition for plastic loading is whether or not iterations were performed
  if (iterationNumber == 0)
    plasticLoading = false;
  else
    plasticLoading = true;

  flowDirection  = sgn(stressRadius);
  strainPEqTrial = ePEq;
  return;
}

/* ------------------------------------------------------------------------ */

void UVCuniaxial::calculateStiffness() {

  if (!plasticLoading) {
    stiffnessTrial = elasticModulus;
  }
  else {
    double sigmaY1 = qInf * (1. - exp(-bIso * strainPEqTrial));
    double sigmaY2 = dInf * (1. - exp(-aIso * strainPEqTrial));
    double plasticModulus = 0.;

    plasticModulus = bIso * (qInf - sigmaY1) -
      aIso * (dInf - sigmaY2);
    for (int i = 0; i < nBackstresses; ++i) {
      plasticModulus += gammaK[i] *
        (cK[i] / gammaK[i] - flowDirection * alphaKTrial[i]);
    }
    stiffnessTrial = (elasticModulus * plasticModulus) /
      (elasticModulus + plasticModulus);
  }
  return;
}

/* ------------------------------------------------------------------------ */

/**
 *
 * @param strain new strain value
 * @param strainRate new strain rate value
 * @return 0 if successful
 */
int UVCuniaxial::setTrialStrain(double strain, double strainRate) {

  // Reset the trial state
  revertToLastCommit();

  // Determine the strain increment
  double strainIncrement = strain - strainConverged;

  // Do the return mapping and calculate the tangent modulus
  strainTrial = strain;
  returnMapping(strainIncrement);
  calculateStiffness();

  return 0;
}

/* ------------------------------------------------------------------------ */

double UVCuniaxial::getStrain() {
  return strainTrial;
}

/* ------------------------------------------------------------------------ */

double UVCuniaxial::getStress() {
  return stressTrial;
}

/* ------------------------------------------------------------------------ */

double UVCuniaxial::getTangent() {
  return stiffnessTrial;
}

/* ------------------------------------------------------------------------ */

double UVCuniaxial::getInitialTangent() {
  return stiffnessInitial;
}

/* ------------------------------------------------------------------------ */

/**
 *
 * @return 0 if successful
 */
int UVCuniaxial::commitState() {
  strainConverged = strainTrial;
  strainPEqConverged = strainPEqTrial;
  stressConverged = stressTrial;
  alphaKConverged = alphaKTrial;
  stiffnessConverged = stiffnessTrial;
  return 0;
}

/* ------------------------------------------------------------------------ */

/**
 *
 * @return 0 if successful
 */
int UVCuniaxial::revertToLastCommit() {
  strainTrial = strainConverged;
  strainPEqTrial = strainPEqConverged;
  stressTrial = stressConverged;
  alphaKTrial = alphaKConverged;
  stiffnessTrial = stiffnessConverged;
  return 0;
}

/* ------------------------------------------------------------------------ */

/**
 *
 * @return 0 if successful
 */
int UVCuniaxial::revertToStart() {
  strainConverged = 0.;
  strainPEqConverged = 0.;
  stressConverged = 0.;
  stiffnessConverged = elasticModulus;
  for (int i = 0; i < nBackstresses; ++i) {
    alphaKConverged[i] = 0.;
  }
  plasticLoading = false;
  flowDirection = 0.0;
  revertToLastCommit();
  return 0;
}

/* ------------------------------------------------------------------------ */

/**
 * Returns a new UniaxialMaterial with all the internal values copied.
 * @return a to pointer to the copy
 */
UniaxialMaterial* UVCuniaxial::getCopy() {

  UVCuniaxial* theCopy;  //todo: what deletes this?
  theCopy = new UVCuniaxial(this->getTag(), elasticModulus,
    yieldStress, qInf, bIso,
    dInf, aIso,
    cK, gammaK);

  // Copy all the internals
  theCopy->strainConverged = strainConverged;
  theCopy->strainTrial = strainTrial;
  theCopy->strainPEqConverged = strainPEqConverged;
  theCopy->strainPEqTrial = strainPEqTrial;
  theCopy->stressConverged = stressConverged;
  theCopy->stressTrial = stressTrial;
  theCopy->alphaKConverged = alphaKConverged;
  theCopy->alphaKTrial = alphaKTrial;
  theCopy->stiffnessConverged = stiffnessConverged;
  theCopy->stiffnessTrial = stiffnessTrial;

  theCopy->flowDirection = flowDirection;
  theCopy->plasticLoading = plasticLoading;

  return theCopy;
}

/* ------------------------------------------------------------------------ */

/**
 * todo: fill out
 * @param commitTag
 * @param theChannel
 * @return 0 if successful
 */
int UVCuniaxial::sendSelf(int commitTag, Channel& theChannel) {

  static Vector data(15 + 2*MAX_BACKSTRESSES);  // enough space for 8 backstresses
  // Material properties
  data(0) = elasticModulus;
  data(1) = yieldStress;
  data(2) = qInf;
  data(3) = bIso;
  data(4) = dInf;
  data(5) = aIso;
  data(6) = stiffnessInitial;

  // Internal variables
  data(7) = strainConverged;
  data(8) = strainPEqConverged;
  data(9) = stressConverged;
  data(10) = stiffnessConverged;
  data(11) = flowDirection;
  data(12) = plasticLoading ? 1.0 : -1.0;

  data(13) = this->getTag();
  data(14) = nBackstresses;
  
  // Kinematic hardening related, 12 total spaces required
  int cKStart = 15;  // start index
  int gammaKStart = cKStart + nBackstresses;
  int alpha_k_start = gammaKStart + nBackstresses;
  for (int i = 0; i < nBackstresses; ++i) {
    data(cKStart + i) = cK[i];
    data(gammaKStart + i) = gammaK[i];
    data(alpha_k_start + i) = alphaKConverged[i];
  }

  if (theChannel.sendVector(this->getDbTag(), commitTag, data) < 0) {
    opserr << "UVCuniaxial::sendSelf() - failed to sendSelf\n";
    return -1;
  }
  return 0;
}

/* ------------------------------------------------------------------------ */

/**
 * todo: fill out
 * @param commitTag
 * @param theChannel
 * @param theBroker
 * @return 0 if successful
 */
int UVCuniaxial::recvSelf(int commitTag, Channel& theChannel,
  FEM_ObjectBroker& theBroker) {
  static Vector data(15 + 2*MAX_BACKSTRESSES);

  if (theChannel.recvVector(this->getDbTag(), commitTag, data) < 0) {
    opserr << "UVCuniaxial::recvSelf() - failed to recvSelf\n";
    return -1;
  }

  // Material properties
  elasticModulus = data(0);
  yieldStress = data(1);
  qInf = data(2);
  bIso = data(3);
  dInf = data(4);
  aIso = data(5);
  stiffnessInitial = data(6);

  // Internal variables
  strainConverged = data(7);
  strainPEqConverged = data(8);
  stressConverged = data(9);
  stiffnessConverged = data(10);
  flowDirection = data(11);
  plasticLoading = data(12) > 0.0 ? true : false;

  this->setTag(int(data(13)));
  nBackstresses = int(data(14));
  cK.resize(nBackstresses);
  gammaK.resize(nBackstresses);
  alphaKConverged.resize(nBackstresses);
  
  // Kinematic hardening related, 12 total spaces required
  int cKStart = 15;  // start index 
  int gammaKStart = cKStart + nBackstresses;
  int alpha_k_start = gammaKStart + nBackstresses;
  for (int i = 0; i < nBackstresses; ++i) {
    cK[i] = data(cKStart + i);
    gammaK[i] = data(gammaKStart + i);
    alphaKConverged[i] = data(alpha_k_start + i);
  }

  // Set the trial to the converged values
  revertToLastCommit();

  return 0;
}

/* ------------------------------------------------------------------------ */

void UVCuniaxial::Print(OPS_Stream& s, int flag) {

  // todo: change these back when not only .dll
  // if (flag == OPS_PRINT_PRINTMODEL_MATERIAL) {
  if (flag == 2) {
    s << "UVCuniaxial tag: " << this->getTag() << endln;
    s << "   E: " << elasticModulus << " ";
    s << "  fy: " << yieldStress << " ";
    s << "   Q: " << qInf << " ";
    s << "   b: " << bIso << " ";
    for (int i = 0; i < nBackstresses; ++i) {
      s << "  C" << (i + 1) << ": " << cK[i] << " ";
      s << "gam" << (i + 1) << ": " << gammaK[i] << " ";
    }
  }

  if (flag == OPS_PRINT_PRINTMODEL_JSON) {
    s << OPS_PRINT_JSON_MATE_INDENT << "{";
    s << "\"name\": \"" << this->getTag() << "\", ";
    s << "\"type\": \"UVCuniaxial\", ";
    s << "\"E\": " << elasticModulus << ", ";
    s << "\"fy\": " << yieldStress << ", ";
    s << "\"Q\": " << qInf << ", ";
    s << "\"b\": " << bIso << ", ";
    for (int i = 0; i < nBackstresses; ++i) {
      s << "\"C\": " << cK[i] << ", ";
      s << "\"gam\": " << gammaK[i] << ", ";
    }
    return;
  }

}
/* ------------------------------------------------------------------------ */


