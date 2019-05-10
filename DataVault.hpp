#ifndef DATAVAULT_HPP
#define DATAVAULT_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <memory>

#include "TrackStruct.hpp"

struct GammaAna
{
    int nPSA;
    double xPSA[10], yPSA[10], zPSA[10];
    double EPSA[10];
    double SourceXY[2];

    GammaAna()
    {
        for(int i = 0;i < 10;++i)
        {
            xPSA[i] = 0;
            yPSA[i] = 0;
            zPSA[i] = 0;
            EPSA[i] = 0;
        }
        for(int i = 0;i < 2;++i)
            SourceXY[i] = 0;
    };
    ~GammaAna(){};
};

class DataVault
{
  private:
    
    int maxBuffer;
    int nThreads;
    int nFilled;

    bool EOF_Reached;

    std::ifstream DATA;
    std::vector<int> Filled;
    Tracked ***DataBuffer;

    void CheckBadAccess(int thrNumber,
                        int EventNumber);
  



  public:
    DataVault(int _nThreads,
              int _processLen);
    ~DataVault();

    void LoadBuffer();
    bool CheckEOF();

    int GetFilled();
    int GetEventAmount(int thrNumber);
    void GetEvent(int thrNumber,
                  int EventNumber,
                  Tracked &GTmp);
};

#endif