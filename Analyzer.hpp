#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include <thread>
#include <iostream>
#include <cmath>
#include <vector>

#include "DataVault.hpp"
#include "GammaTracker.hpp"
#include "ConeChecker.hpp"

class Analyzer
{
private:

    DataVault *Data;

    GammaTracker Tracker;
    ConeChecker CONE;

    
    const double mc2 = 511;
    const double WRONG_CASE = -9999;
    const double binWidth = 0.5;

    int AmountOfEvents;
    int MAX_ASYM;
    double Eg;

    
    std::vector<std::vector<double>> Asymmetries;

    int thrNumber;

    void Process();

    void GammaAnalysis(const GammaAna &G);

    inline bool Gate(GammaAna &G);
    inline double GetThetaE(const double E) const;

public:
    Analyzer(const int _thrNumber,
             DataVault *_Data,
             const double _Eg,
             const int _processLen);
    ~Analyzer();

    std::thread ANALYZE();

    void GetAsymmetries(std::vector<std::vector<double>> &Asymmetries);
};



#endif