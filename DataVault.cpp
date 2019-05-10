#include "DataVault.hpp"

//------------------------------------------

DataVault::DataVault(int _nThreads,
                     int _processLen)
    : nThreads(_nThreads),
      maxBuffer(_processLen)
{
    DataBuffer = new Tracked**[nThreads];
    for(int i = 0;i < nThreads;++i)
    {
        DataBuffer[i] = new Tracked*[maxBuffer];
        for(int j = 0;j < maxBuffer;++j)
            DataBuffer[i][j] = new Tracked();
    }
    Filled = std::vector<int>(nThreads,0);

    //-------------------------------------------
    std::string dataName = "TrackedBin.bin";//"GBin_Raw.bin";//GBin_RawCs.bin";
    //-------------------------------------------
    
    DATA.open(dataName,std::ios::in | std::ios::binary);

    if(DATA.fail())
    {
        std::cerr << "------------------------------" << std::endl;
        std::cerr << "Could not find " << dataName << std::endl;
        std::cerr << "------------------------------" << std::endl;
        throw 2;
    }
    EOF_Reached = false;
}

//------------------------------------------

DataVault::~DataVault()
{
    for(int i = 0;i < nThreads;++i)
    {
        for(int j = 0;j < maxBuffer;++j)
        {
            
            if(DataBuffer[i][j])
                delete DataBuffer[i][j];
            DataBuffer[i][j] = nullptr;
        }
        if(DataBuffer[i])
        {
            delete[] DataBuffer[i];
            DataBuffer[i] = nullptr;
        }
    }
    delete[] DataBuffer;
    DataBuffer = nullptr;
}

//------------------------------------------

void DataVault::LoadBuffer()
{
    //Temporary Tracked Object for data input
    Tracked GTmp;

    for(int i = 0;i < nThreads;++i)
        Filled[i] = 0;

    for(int i = 0;i < nThreads;++i)
    {
        for(int j = 0;j < maxBuffer;++j)
        {
            if(DATA.good())
            {
                DATA.read((char*)&GTmp,sizeof(GTmp));

                for(int k = 0;k < 2;++k)
                {
                    DataBuffer[i][j]->gammaX[k] = GTmp.gammaX[k];
                    DataBuffer[i][j]->gammaY[k] = GTmp.gammaY[k];
                    DataBuffer[i][j]->gammaZ[k] = GTmp.gammaZ[k];
                    DataBuffer[i][j]->E[k] = GTmp.E[k];
                }
                for(int k = 0;k < 2;++k)
                    DataBuffer[i][j]->SourceXY[k] = GTmp.SourceXY[k];
                DataBuffer[i][j]->dE_Lycca = GTmp.dE_Lycca;
                DataBuffer[i][j]->E_Lycca = GTmp.E_Lycca;
                DataBuffer[i][j]->mult_Lycca = GTmp.mult_Lycca;
                
                ++Filled[i];
            }
            else
            {
                std::cout << "-----------------------" << std::endl;
                std::cout << "End of File reached" << std::endl;
                std::cout << "-----------------------" << std::endl;
                EOF_Reached = true;
                return;
            }
        }
    }
    //Amount of data filled threads
    nFilled = 0;
    for(int i = 0;i < nThreads;++i)
        nFilled += (Filled[i] > 0) ? 1 : 0;
}

//------------------------------------------

bool DataVault::CheckEOF()
{
    return EOF_Reached;
}

//------------------------------------------

int DataVault::GetFilled()
{
    return nFilled;
}

//------------------------------------------

int DataVault::GetEventAmount(int thrNumber)
{
    return Filled[thrNumber]; 
} 

//------------------------------------------

void DataVault::GetEvent(int thrNumber,
                         int EventNumber,
                         Tracked &GTmp)
{
    if(EventNumber >= Filled[thrNumber])
    {
        std::cerr << "EventNumber " << EventNumber << " exceeding array size";
        std::cerr << " " << Filled[thrNumber] << std::endl;
        throw 0;
    }
    try
    {
        CheckBadAccess(thrNumber, EventNumber);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    for (int k = 0; k < 2; ++k)
    {
        GTmp.gammaX[k] = DataBuffer[thrNumber][EventNumber]->gammaX[k];
        GTmp.gammaY[k] = DataBuffer[thrNumber][EventNumber]->gammaY[k];
        GTmp.gammaZ[k] = DataBuffer[thrNumber][EventNumber]->gammaZ[k];
        GTmp.E[k] = DataBuffer[thrNumber][EventNumber]->E[k];
    }
    for (int k = 0; k < 2; ++k)
        GTmp.SourceXY[k] = DataBuffer[thrNumber][EventNumber]->SourceXY[k];

    GTmp.dE_Lycca = DataBuffer[thrNumber][EventNumber]->dE_Lycca;
    GTmp.E_Lycca = DataBuffer[thrNumber][EventNumber]->E_Lycca;
    GTmp.dE_Lycca = DataBuffer[thrNumber][EventNumber]->mult_Lycca;
} 

//------------------------------------------

void DataVault::CheckBadAccess(int thrNumber,int EventNumber)
{
    bool bad = false;
    bad = (thrNumber < 0) || thrNumber >= nThreads;
    bad = bad || (EventNumber < 0) || EventNumber >= maxBuffer;

    if(bad)
    {
        std::cerr << "ThrNumber " << thrNumber << " EventNumber " << EventNumber << std::endl;
        throw std::invalid_argument("Invalid iterator sizes");
    }
} 

//------------------------------------------