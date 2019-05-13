#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int main(int argc,char** argv)
{

    int nthr = 1,planes = 2;
    bool nFlag = false,turnFlag = false;
    for(int i = 0;i < argc;++i)
    {
        std::string s = std::string(argv[i]);
        if(s == "-t")
        {
            nFlag = true;
            continue;
        }
        if(nFlag)
        {
            nthr = std::stoi(s);
            nFlag = false;
            continue;
        }
        if(s == "-p")
        {
            turnFlag = true;
            continue;
        }
        if(turnFlag)
        {
            planes = std::stoi(s);
            turnFlag = false;
            continue;
        }
    }

    std::ifstream in;
    std::ofstream out;

    int val;

    std::string name;

    int nPlanes = planes;

    for(int i = 0;i < nPlanes;++i)
    {   
        std::vector<std::vector<int>> hist(1000,std::vector<int>(1000,0));
        out.open("File_Merge_Cs_" + std::to_string(i));
        for(int j = 0;j < nthr;++j)
        {
            name = "File_" + std::to_string(j) + "_" + std::to_string(i);
            in.open(name);
            if(in.fail())
            {
                std::cerr << "Could not open " << name << std::endl;
                return 1;
            }
            for(int k = 0;k < 1000;++k)
            {
                for(int o = 0;o < 1000;++o)
                {
                    in >> val;
                    hist[k][o] += val;
                }
            }
            in.close();
            in.clear();
        }

        for(auto Y : hist)
        {
            for(auto X : Y)
                out << X << " ";
            out << std::endl;
        }

        out.close();
        out.clear();
    }


    return 0;
}