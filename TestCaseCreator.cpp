#include <iostream>
#include <fstream>

#include "TrackStruct.hpp"


int main()
{

    Tracked T;

    std::ofstream OUT("TrackedBin.bin",std::ios::out | std::ios::binary);

    for(int i = 0;i < 100;++i)
    {
        OUT.write((char*)&T,sizeof(T));
    }

    OUT.close();
    return 0;


}