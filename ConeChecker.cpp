#include "ConeChecker.hpp"

//------------------------------------------

ConeChecker::ConeChecker(const int _fileNum)
    : tan_t(0),
      phi(0),
      theta(0),
      ctheta_lim_m(0),
      ctheta_lim_p(0),
      a(0),
      b(0),
      c(0),
      EThetaBins(0),
      badCone(false),
      fileNum(_fileNum),
      acosAngle(0),
      TargetOffset(200),
      nPlanes(40)
{



    Histograms = std::vector<std::vector<std::vector<double>>>(nPlanes,
                 std::vector<std::vector<double>>(YBins,
                 std::vector<double>(XBins,0)));

    alpha = std::vector<double>(alphaBins,0);
    for(int i = 0;i < alphaBins;++i)
        alpha[i] = -180. + i*360./((double)alphaBins);

    Target = std::vector<double>(nPlanes,0);

    for(int i = 0;i < nPlanes;++i)
        Target[i] = -200. + i*400./((double)nPlanes);
    
}

//------------------------------------------

ConeChecker::~ConeChecker()
{
    WriteHistograms();
}

//------------------------------------------


void ConeChecker::Check(const std::vector<double> &Etheta,
                        const std::vector<std::vector<double>> &X,
                        const double Relative)
{
    //set rotation matrices for cone trafos
    SetMatrix(X);
    badCone = false;

    for(auto t : Etheta)
    {
        //set denominator limits for cone angle alpha
        SetDenominatorLimit(t);
        SetVector(X[0]);
        //loop over the 2 target areas
        for(int i = 0;i < nPlanes;++i)
        {
            //loop over binned spherical angles of Compton cone
            for(auto al : alpha)
            {
                al *= M_PI/180.;
                //Check if alphaBin in forbidden area
                //if(BadAlpha(al))
                //    continue;
                //Set cone points and transform to
                //target reference frame
                SetPoint(al,i);

                //Write Events that intersect with target
                

                //increment intersection point on histogram
                if(!badCone && Relative < 0.1)
                    IncrementHistogram(i);
            } 
            

        }
    }
}

//------------------------------------------

void ConeChecker::SetMatrix(const std::vector<std::vector<double>> &X)
{
    //Calculate spherical coordinates theta,phi of X
    double norm = 0;
    phi = 0;

    std::vector<double> XForTrafo(3,0);

    for(int i = 0;i < 3;++i)
        XForTrafo[i] = X[1][i] - X[0][i];

    for (auto x : XForTrafo)
        norm += x*x;
    norm = sqrt(norm);

    theta = acos(XForTrafo[2]/norm);
    phi = atan2(XForTrafo[1],XForTrafo[0]);

    cth = cos(theta);
    sth = sin(theta);
    cphi = cos(phi);
    sphi = sin(phi);

    Matrix(0, 0) = cphi * cth;
    Matrix(1, 0) = sphi * cth;
    Matrix(2, 0) = -sth;

    Matrix(0, 1) = -sphi;
    Matrix(1, 1) = cphi;
    Matrix(2, 1) = 0;

    Matrix(0, 2) = cphi * sth;
    Matrix(1, 2) = cphi * sth;
    Matrix(2, 2) = cth;

    double cotPhi = 1. / tan(phi), cotTheta = 1. / tan(theta);
    acosAngle = std::abs(cotPhi * cotTheta) <= 1 ? acos(cotPhi * cotTheta) : -99999;

    for(int i = 0;i < 3;++i)
        VecXOrig(i) = X[0][i];
}

//------------------------------------------

void ConeChecker::SetDenominatorLimit(const double t)
{
    tan_t = tan(t*M_PI/180.);

    a = Matrix(2,0) * tan_t;
    b = Matrix(2,1) * tan_t;
    c = Matrix(2,2);

    double over_ab = 1./(a*a + b*b);
    double inSqrt = sqrt(a*a*b*b -c*c*b*b + b*b*b*b);

    ctheta_lim_m = over_ab * (-a * c - inSqrt);
    ctheta_lim_p = over_ab * (-a * c + inSqrt);
} 

//------------------------------------------

inline double ConeChecker::ConeHeight(const int targetID,
                                      const double al) const
{
    //check if al in bad range
    bool badValue = false;
    for(int i = 0;i < 2;++i)
        badValue = badValue || (std::abs(al - pow(-1,i)*acosAngle) < 0.1);
    if(badValue)
        return ConeHeightBad;
    
    //if not, return good value
    return (Target[targetID] - VecXOrig(2)) / (cos(theta) - cos(al)*sin(theta)*tan_t);
    //return (100*targetID - VecX[2])*(a*cos(al) + b*sin(al) + c);
} 

//------------------------------------------

inline int ConeChecker::GetBin(const double value) const
{
    return (int)((value + Range)*(XBins)/(2.*Range));
} 

//------------------------------------------

inline bool ConeChecker::BadAlpha(const double al) const
{
    double cal = cos(al);
    double lim = 0;
    
    for(int i = 0;i < 2;++i)
    {
        lim = i ? ctheta_lim_p : ctheta_lim_m;

        if(std::abs(std::abs(cal/lim) - 1) <= 0.1)
            return true;
    }   
    return false;
}

//------------------------------------------

void ConeChecker::SetPoint(const double al,
                           const int targetID)
{
    double h = ConeHeight(targetID,al);
    
    //only allow cone pointing towards targets
    badCone = al < M_PI ? (h > 0) : (h < 0);
    if (badCone || h == ConeHeightBad)
        return;

    VecX(0) = h * cphi * sth + h * cos(al) * cth * cphi * tan_t -
              h * sin(al) * sphi * tan_t;
    VecX(1) = h * sth * sphi +
              h * cphi * sin(al) * tan_t +
              h * cos(al) * cth * sphi * tan_t;
    VecX(2) = h * cth - h * cos(al) * sth * tan_t;

    for(int i = 0;i < 3;++i)
        VecX(i) += VecXOrig(i);// + VecRot(i);
    
}

//------------------------------------------

void ConeChecker::SetVector(const std::vector<double> &X)
{
    for (int i = 0; i < 3; ++i)
        VecX(i) = X[i];
}

//------------------------------------------

void ConeChecker::IncrementHistogram(const int TargetID)
{
    int XBin = GetBin(VecX(0));
    int YBin = GetBin(VecX(1));

    

    //Check if z coordinate == Target position
    if(std::abs(VecX(2) - Target[TargetID]) >= 1)
    {
        std::cerr << "======================================" << std::endl;
        std::cerr << "VecX(2) = " << VecX(2) << std::endl;
        std::cerr << "Target position " << Target[TargetID] << std::endl;
        
        throw std::invalid_argument("Invalid Cone Intersection");
    }
    if (XBin < XBins && 
        YBin < YBins && 
        XBin >= 0 && 
        YBin >= 0)
    {
        ++Histograms[TargetID][XBin][YBin];
    }
}


//------------------------------------------

void ConeChecker::WriteHistograms() const
{
    std::ofstream DATA;
    std::string name;
    for(int i = 0;i < nPlanes;++i)
    {
        name = "Histograms/File_" + std::to_string(fileNum) + "_" + std::to_string(i);
        DATA.open(name);
        if(DATA.fail())
        {
            std::cerr << "Could not open " << name << std::endl;
            throw 1;
        }
        for(auto Y : Histograms[i])
        {
            for(auto X : Y)
                DATA << X << " ";
            DATA << std::endl;
        }
        DATA.close();
        DATA.clear();
    }
}

//------------------------------------------
