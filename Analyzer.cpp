#include "Analyzer.hpp"

//------------------------------------------

Analyzer::Analyzer(const int _thrNumber,
                   DataVault *_Data,
                   const double _Eg,
                   const int _processLen,
                   const int _mid)
    : thrNumber(_thrNumber),
      CONE(_thrNumber),
      Data(_Data),
      Tracker(_Eg),
      AmountOfEvents(0),
      Eg(_Eg),
      MAX_ASYM(_processLen),
      mid(_mid)
{}

//------------------------------------------

Analyzer::~Analyzer()
{
    Data = nullptr;
}

//------------------------------------------

void Analyzer::Process()
{
    AmountOfEvents = Data->GetEventAmount(thrNumber);
    Tracked TrackTmp;

    int nProcessed = 0;

    for(int i = 0;i < AmountOfEvents;++i)
    {
        Data->GetEvent(thrNumber, i, TrackTmp);

        //Starting analysis
        GammaAnalysis(TrackTmp);

        ++nProcessed;
        //if(!thrNumber && nProcessed % 1000 == 0)
        //    std::cout << "Events processed -> " << nProcessed << " of " << AmountOfEvents << std::endl;
    }
}

//------------------------------------------

std::thread Analyzer::ANALYZE()
{
    return std::thread([=]{Process();});
} 

//------------------------------------------

inline bool Analyzer::Gate(const Tracked &T)
{
    double Esum = 0;
    double dist = pow(T.gammaX[0] - T.gammaX[1], 2) +
                  pow(T.gammaY[0] - T.gammaY[1], 2) +
                  pow(T.gammaZ[0] - T.gammaZ[1], 2);
    
    Esum = T.E[1];
    dist = sqrt(dist);
    
    bool generalCondition = dist >= 10 && std::abs(T.E[0] - T.E[1]/2.) <= 50;

    if(T.gammaX[1] == 0)
        std::cout << "Not nice" << std::endl;
    
    //left from peak
    if(mid == -1)
        return Esum >= 535 && Esum <= 541 && generalCondition;
    
    //right from peak
    else if(mid == 1)
        return Esum >= 550 && Esum <= 556 && generalCondition;
    
    //peak
    else
        return Esum >= 543 && Esum <= 549 && generalCondition;
    //return std::abs(Eg - Esum) <= 10 && dist >= 10;// && std::abs(G.EPSA[0] - Eg/2.) <= 10);
} 

//------------------------------------------

void Analyzer::GammaAnalysis(Tracked &Track)
{

    double sigmaTheta = 0;
    double Theta = 0;

    bool processed = false;

    int nbins = 0;

    std::vector<double> Edep(2,0);
    std::vector<std::vector<double>> X(2,std::vector<double>(3,0));

    for(int i = 0;i < 2;++i)
    {
        Edep[i] = Track.E[i];
        X[i][0] = Track.gammaX[i];
        X[i][1] = Track.gammaY[i];
        X[i][2] = Track.gammaZ[i];
    }

    if(!Gate(Track))
        return;

    Theta = GetThetaE(Edep[0]);
    
    //check if scattering possible
    if(Theta == WRONG_CASE)
        return;

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
    
    if(Track.mult_Lycca >= 0)
    {
        double r = 0;
        for(int i = 0;i < 2;++i)
            r += pow(Track.SourceXY[i],2);
        r = sqrt(r);
        CONE.Check(ETheta,X,sigmaTheta/Theta,r);
    }
}

//------------------------------------------

inline double Analyzer::GetThetaE(const double E) const
{
    double cth = 1 - mc2/(Eg - E) + mc2/Eg;
    return (std::abs(cth) <= 1) ? acos(cth)*180./M_PI : WRONG_CASE;
}

//------------------------------------------