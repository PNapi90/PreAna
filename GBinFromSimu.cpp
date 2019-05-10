#include <iostream>
#include <fstream>
#include <string>

struct GammaAna
{
    int nPSA;
    double xPSA[10], yPSA[10], zPSA[10];
    double EPSA[10];
    double SourceXY[2];

    GammaAna()
    {
        for (int i = 0; i < 10; ++i)
        {
            xPSA[i] = 0;
            yPSA[i] = 0;
            zPSA[i] = 0;
            EPSA[i] = 0;
        }
        for (int i = 0; i < 2; ++i)
            SourceXY[i] = 0;
    };
    ~GammaAna(){};
};

const char* format = "%lf %lf %lf %lf %lf %lf"; 


int main()
{

    std::ifstream DATA("GammaEvents.0000");
    std::string line;

    std::ofstream OUT("GEvents.bin",std::ios::out | std::ios::binary);

    GammaAna G;
    int iter = 0;
    bool dataComing  = false;
    double x[6];
    while(std::getline(DATA,line))
    {
        if(line[0] == '$')
        {
            dataComing = true;
            continue;
        }
        if(!dataComing)
            continue;

        sscanf(line.c_str(),format,&x[0],&x[1],&x[2],&x[3],&x[4],&x[5]);
        
        if(x[0] == -1)
        {
            if (iter > 0)
            {
                G.nPSA = iter;

                OUT.write((char*)&G,sizeof(G));
            }
            for(int i = 0;i < 10;++i)
            {
                G.EPSA[i] = 0;
                G.xPSA[i] = 0;
                G.yPSA[i] = 0;
                G.zPSA[i] = 0;
            }             
            G.nPSA = 0;
            iter = 0;
        }
        else if(iter < 10)
        {
            G.EPSA[iter] = x[1];
            G.xPSA[iter] = -x[2];
            G.yPSA[iter] = x[3];
            G.zPSA[iter] = -x[4];

            ++iter;
        }
        

    }

    OUT.close();



    return 0;
}