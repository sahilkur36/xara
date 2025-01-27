//===----------------------------------------------------------------------===//
//
//        OpenSees - Open System for Earthquake Engineering Simulation    
//
//===----------------------------------------------------------------------===//
//
// Description: This file contains the class definition for 
// FrameSolidSection3d.h. FrameSolidSection3d provides the abstraction of a 
// 3D beam section discretized by fibers. The section stiffness and
// stress resultants are obtained by summing fiber contributions.
//
// Adapted from NDFiberSection3d
//
// Written: cmp
// Created: Spring 2025
//
#ifndef FrameSolidSection3d_h
#define FrameSolidSection3d_h
#include <array>
#include <memory>
#include <FrameSection.h>
#include <Vector.h>
#include <vector>
#include <Matrix.h>
#include <MatrixND.h>

class NDMaterial;
class Response;

class FrameSolidSection3d : public FrameSection
{
  public:
    FrameSolidSection3d(); 
    FrameSolidSection3d(int tag, int numFibers, double alpha, bool compCentroid);
    ~FrameSolidSection3d();

    int addFiber(NDMaterial&, double area, double y, double z);

    // Element
    const char *getClassType() const {
      return "FrameSolidSection3d";
    }

    int   setTrialSectionDeformation(const Vector &e); 
    const Vector &getSectionDeformation();

    int   getIntegral(Field field, State state, double& value) const override final;
    const Vector &getStressResultant();
    const Matrix &getSectionTangent();
    const Matrix &getInitialTangent();

    int   commitState();
    int   revertToLastCommit();    
    int   revertToStart();
 
    FrameSection *getFrameCopy();
    const ID &getType();
    int getOrder () const;
    
    int sendSelf(int tag, Channel &);
    int recvSelf(int tag, Channel &, FEM_ObjectBroker &);
    void Print(OPS_Stream &s, int flag = 0);
	    
    Response *setResponse(const char **argv, int argc, OPS_Stream &s);
    int getResponse(int responseID, Information &info);


    // Sensitivity
    int setParameter(const char **argv, int argc, Parameter &);
    int updateParameter(int id, Information &);
    int activateParameter(int id);
    const Vector& getStressResultantSensitivity(int gradIndex, bool conditional);
    const Vector& getSectionDeformationSensitivity(int gradIndex);
    const Matrix& getInitialTangentSensitivity(int gradIndex);
    int commitSensitivity(const Vector& strainGrad, int gradIndex, int numGrads);


  private: 
    int stateDetermination(Matrix& ksi, Vector* s_trial, const Vector *e_trial, int);

    struct Param {
      enum : int {
        FiberWarpX=0,
        FiberWarpXY,
        FiberWarpXZ,
        FiberWarpY,
        FiberWarpYY,
        FiberWarpYZ,
        FiberWarpZ,
        FiberWarpZY,
        FiberWarpZZ,
        FiberY,
        FiberZ,
        FiberArea,
        alpha,
        FiberFieldBase=10000
      };
    };

    constexpr static int nsr = 12;
    constexpr static int nwm =  3; // Number of warping modes

    enum: int {
      CurrentTangent, InitialTangent
    };

    enum : int {
# if 1 
      inx = 0, 
      iny,
      inz,

      imx,
      imy,
      imz,

      iwx,     
      iwy,
      iwz,

      ivx,     
      ivy,
      ivz
#else
      inx = 0, 
      imz,  
      imy,
      iny,
      inz,
      imx,

      iwx,     
      iwy,
      iwz,

      ivx,     
      ivy,
      ivz
#endif
    };
    
    struct FiberData {
      // NOTE: this may be faster if qualified with const, but this would
      // prevent parameterizing the fiber data.
      double y;
      double z;
      double area;
      std::array<std::array<double,3>,nwm> warp{0};
      OpenSees::VectorND<3> r;
    };
    std::shared_ptr<std::vector<FiberData>> fibers;
    std::vector<NDMaterial*> materials;


    OpenSees::MatrixND<3,3> Knn,      Knw, Knv, 
                            Kmn, Kmm, Kmw, Kmv, 
                                      Kww,
                                           Kvv;
    double   kData[nsr*nsr];            // data for ks matrix 
    double   sData[nsr];                // data for s vector 

    double Abar,QyBar, QzBar;
    Vector3D centroid;
    double yBar;                      // Section centroid
    double zBar;                      // Section centroid
    bool computeCentroid;
    double alpha;      // Shear shape factor


    static ID code;

    Vector e;          // trial section deformations 
    Vector *s;         // section resisting forces  (axial force, bending moment)
    Matrix *ks;        // section stiffness

    int parameterID;
    Vector dedh;
};

#endif
