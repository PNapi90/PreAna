#include <thread>
#include <iostream>
#include <memory>
#include <vector>
#include <fstream>

#include "DataVault.hpp"
#include "Analyzer.hpp"

//-------------------------------------

struct FlagsAndValues
{
    int nThr = 1;
    int turns = 100;
    double Egamma = 661.7;//.;//547.5;
    int BufferLen = 1000;
};

//-------------------------------------

void GetFlags(int argc,
              char** argv,
              FlagsAndValues &F);


//=====================================

int main(int argc,char** argv)
{
    FlagsAndValues Flag;

    GetFlags(argc,argv,Flag);
    DataVault DATA(Flag.nThr,Flag.BufferLen);

    std::cout << "----------------------" << std::endl;
    std::cout << "Eg = " << Flag.Egamma << std::endl;
    std::cout << "Turns = " << Flag.turns << std::endl;
    std::cout << "Threads = " << Flag.nThr << std::endl;
    std::cout << "BufferLen = " << Flag.BufferLen << std::endl;
    std::cout << "------------" << std::endl;

    std::vector<std::shared_ptr<Analyzer>> Analyzers;
    Analyzers.reserve(Flag.nThr);
    for(int i = 0;i < Flag.nThr;++i)
        Analyzers.push_back(std::make_shared<Analyzer>(i,&DATA,Flag.Egamma,Flag.BufferLen));

    int RoundCount = 0;

    int MAXTURNS = Flag.turns;

    while(!DATA.CheckEOF() && RoundCount < MAXTURNS)
    {
        DATA.LoadBuffer();
        int nFilled = DATA.GetFilled();
        
        std::thread t[nFilled];
        for(int i = 0;i < nFilled;++i)
            t[i] = Analyzers[i]->ANALYZE();
        for (int i = 0; i < nFilled; ++i)
            t[i].join();


        std::cout << "\t\t\t\t\t\t\r";
        std::cout << "Turn # " << RoundCount << " done";
        std::cout.flush();
        
        ++RoundCount;
    }
    std::cout << "\nDeleting Analyzers... ";
    std::cout.flush();
    for (int i = 0; i < Flag.nThr; ++i)
        Analyzers.pop_back();
    std::cout << "done" << std::endl;

    std::cout << "\n\nData fully processed" << std::endl;
    std::cout << "----------------------" << std::endl;

    

    return 0;
}

//=====================================

void GetFlags(int argc,
              char **argv,
              FlagsAndValues &F)
{

    std::string s;
    bool threads = false,turnFlag = false,bufferFlag = false;
    bool EgFlag = false;

    for(int i = 0;i < argc;++i)
    {
        s = std::string(argv[i]);
        if(s == "-t")
        {
            threads = true;
            continue;
        }
        if(threads)
        {
            F.nThr = std::stoi(s);
            threads = false;
            continue;
        }
        if(s == "-turns")
        {
            turnFlag = true;
            continue;
        }
        if(turnFlag)
        {
            F.turns = std::stoi(s);
            turnFlag = false;
            continue;
        }
        if(s == "-b")
        {
            bufferFlag = true;
            continue;
        }
        if(bufferFlag)
        {
            F.BufferLen = std::stoi(s);
            bufferFlag = false;
            continue;
        }
        if(s == "-Eg")
        {
            EgFlag = true;
            continue;
        }
        if(EgFlag)
        {
            F.Egamma = std::stod(s);
            EgFlag = false;
            continue;
        }
    }
}

//-------------------------------------
