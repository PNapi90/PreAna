#ifndef CONE_CHECKER_H
#define CONE_CHECKER_H

#include <stdexcept>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <iostream>

#include <Eigen/Dense>

class ConeChecker
{

private:

    const int XBins = 1000;
    const int YBins = 1000;
    const int alphaBins = 360;
    const double Range = 1000; //Range in mm
    const double ConeHeightBad = 99999;

    bool badCone;

    double theta,phi,tan_t,acosAngle;
    double ctheta_lim_m,ctheta_lim_p;
    double TargetOffset;

    std::vector<double> Target;

    double cth,sth,cphi,sphi;

    int EThetaBins,fileNum,nPlanes;

    std::vector<double> alpha;

    Eigen::Matrix3d Matrix;
    Eigen::Vector3d VecX,VecXOrig;

    double a,b,c;

    std::vector<std::vector<std::vector<double>>> Histograms;

    void SetDenominatorLimit(const double t);
    void SetMatrix(const std::vector<std::vector<double>> &X);
    void SetVector(const std::vector<double> &X);
    
    void SetPoint(const double al,
                  const int targetID);

    void IncrementHistogram(const int TargetID);

    void WriteHistograms() const;

    inline int GetBin(const double value) const;
    inline double ConeHeight(const int targetID,
                             const double alpha) const;

    inline bool BadAlpha(const double al) const;

    

public:
    ConeChecker(const int _fileNum);
    ~ConeChecker();

    void Check(const std::vector<double> &Etheta,
               const std::vector<std::vector<double>> &X,
               const double Relative);
};








#endif