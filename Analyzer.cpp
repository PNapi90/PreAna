#include "Analyzer.hpp"

//------------------------------------------

Analyzer::Analyzer(const int _thrNumber,
                   DataVault *_Data,
                   const double _Eg,
                   const int _processLen)
    : thrNumber(_thrNumber),
      CONE(_thrNumber),
      Data(_Data),
      Tracker(_Eg),
      AmountOfEvents(0),
      Eg(_Eg),
      MAX_ASYM(_processLen)
{
    Asymmetries = std::vector<std::vector<double>>(MAX_ASYM,
                  std::vector<double>(6,0));
}

//------------------------------------------

Analyzer::~Analyzer()
{
    Data = nullptr;
}

//------------------------------------------

void Analyzer::Process()
{
    AmountOfEvents = Data->GetEventAmount(thrNumber);
    GammaAna GammaTmp;

    int nProcessed = 0;

    for(int i = 0;i < AmountOfEvents;++i)
    {
        Data->GetEvent(thrNumber,i,GammaTmp);

        if(!Gate(GammaTmp))
        {
            Asymmetries[i][0] = -100;
            Asymmetries[i][1] = 0;
            continue;
        }
        //Starting analysis
        GammaAnalysis(GammaTmp);

        ++nProcessed;
        if(!thrNumber && nProcessed % 1000 == 0)
            std::cout << "Events processed -> " << nProcessed << " of " << AmountOfEvents << std::endl;
    }
}

//------------------------------------------

std::thread Analyzer::ANALYZE()
{
    return std::thread([=]{Process();});
} 

//------------------------------------------

void Analyzer::GetAsymmetries(std::vector<std::vector<double>> &Asymmetries)
{
    for(int i = 0;i < AmountOfEvents;++i)
    {
        for(int j = 0;j < 6;++j)
            Asymmetries[i][j] = this->Asymmetries[i][j];
    }
} 

//------------------------------------------

inline bool Analyzer::Gate(GammaAna &G)
{
    double Esum = 0;
    double dist = pow(G.xPSA[0] - G.xPSA[1],2) + 
                   pow(G.yPSA[0] - G.yPSA[1],2) +
                   pow(G.zPSA[0] - G.zPSA[1],2);
    for(int i = 0;i < G.nPSA;++i)
    {
        Esum += G.EPSA[i];
    }
    dist = sqrt(dist);
    
    return ((G.nPSA == 2 && G.nPSA > 1) && std::abs(Esum - Eg) <= 3 && dist >= 10);// && std::abs(G.EPSA[0] - Eg/2.) <= 10);
} 

//------------------------------------------

void Analyzer::GammaAnalysis(const GammaAna &Gamma)
{
    const int Hits = Gamma.nPSA;
    std::vector<int> SortArray(Hits,0);
    for(int i = 0;i < Hits;++i)
        SortArray[i] = i;

    double sigmaTheta = 0;
    double Theta = 0;
    //old vector for higher performance
    std::vector<int> Old(2,0);
    bool processed = false;

    int nbins = 0;

    std::vector<double> Edep(2,0);
    std::vector<std::vector<double>> X(2,std::vector<double>(3,0));
    bool firstLoop = true;
    //loop over all permutations
    do
    {
        if(!firstLoop)
            continue;
        else
            firstLoop = false;
        //Check if already processed
        processed = (Old[0] == SortArray[0])
                    && (Old[1] == SortArray[1]);
        for(int i = 0;i < 2;++i)
            Old[i] = SortArray[i];

        if(processed)
            continue;
        for(int i = 0;i < 2;++i)
        {
            Edep[i] = Gamma.EPSA[SortArray[i]];
            X[i][0] = Gamma.xPSA[SortArray[i]];
            X[i][1] = Gamma.yPSA[SortArray[i]];
            X[i][2] = Gamma.zPSA[SortArray[i]];
        }
        
        Theta = GetThetaE(Edep[0]);
        
        //check if scattering possible
        if(Theta == WRONG_CASE)
            continue;
        
        sigmaTheta = Tracker.GetUncertainty(Edep,X);
        bool checkZeroDegree = (Theta - sigmaTheta) < 0;

        nbins = (checkZeroDegree) ? (sigmaTheta+Theta)/binWidth : 2*sigmaTheta/binWidth;

        std::vector<double> ETheta(2,Theta);
        //std::vector<double> ETheta(nbins,Theta - sigmaTheta);
        //std::cout << "_----------------_\n" << Theta << " " << sigmaTheta << "\n----" << std::endl;
        //for(int i = 0;i < nbins;++i)
        //    ETheta[i] += i*(2*sigmaTheta)/nbins;
        
        //Check cone intersections with
        //both target planes
        CONE.Check(ETheta,X,sigmaTheta/Theta);

    } while (std::next_permutation(SortArray.begin(),SortArray.end()));
    
}

//------------------------------------------

inline double Analyzer::GetThetaE(const double E) const
{
    double cth = 1 - mc2/(Eg - E) + mc2/Eg;
    return (std::abs(cth) <= 1) ? acos(cth)*180./M_PI : WRONG_CASE;
}

//------------------------------------------