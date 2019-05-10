#include "DeltaX.hpp"

//------------------------------------------

DeltaX::DeltaX()
{}

//------------------------------------------

DeltaX::~DeltaX()
{}

//------------------------------------------

double DeltaX::GetUncertainty(int pos_Gamma,
                              int xyz,
                              int Target12)
{
    if(pos_Gamma > 1)
        return deltaX_Agata;

    if(pos_Gamma == 0)
    {
        std::cerr << "Invalid position of Gamma!" << std::endl;
        throw 1337;
    }
    
    if(xyz == 2)
        return deltaX_Lycca[2+Target12];
    else if(xyz >= 0 && xyz < 2)
        return deltaX_Lycca[xyz];
    
    else
    {
        std::cerr << "Invalid xyz " << xyz << " encountered!" << std::endl;
        throw 1338;
    }
    
    return -1000;
}

//------------------------------------------