#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>


#include "DeltaX.hpp"
#include "DataVault.hpp"

class GammaTracker
{
private:

    
    const double mc2 = 511.;
    const double WRONG_CASE = 100000;

    std::vector<std::vector<std::vector<double>>> deltaX_Vec;

    std::vector<std::vector<double>> GammaBuffer,Source,mu_vec,angle_vec;
    std::vector<double> sigmas,sigma_Eth,P12;

    const double E_197Au = 547.5;
    const int MAX_ITER = 6;

    double Esum,Eg;

    int multiplicity,Target12;
    double Asymmetry;

    DeltaX DX;


    void get_E_angle(double Estart,
                     double Ed,
                     int pos);
    double Tracking(int Target12);
    void get_sigmas(int pos,
                    int perm_iter);
    void GetSigmaE(std::vector<double> &Edeps, 
                   int len);
    
    inline int Factorial(int N);

public:
    GammaTracker(const double _Eg);
    ~GammaTracker();

    void Feed(GammaAna *Gamma);

    double getAsymmetry();

    double getP12(int pos);

    double GetUncertainty(std::vector<double> &Edep,
                          const std::vector<std::vector<double>> &X);
};

