#include "GammaTracker.hpp"

//------------------------------------------

GammaTracker::GammaTracker(const double _Eg)
    : multiplicity(0),
      DX(),
      Esum(0),
      Eg(_Eg)
{
    
    Source = std::vector<std::vector<double>>(2, std::vector<double>(3, 0));
    mu_vec = std::vector<std::vector<double>>(3, std::vector<double>(3, 0));
    
    int loopStart = 0;

    int max_fac = Factorial(MAX_ITER);

    sigmas = std::vector<double>(max_fac, 0);
    sigma_Eth = std::vector<double>(max_fac, 0);
    angle_vec = std::vector<std::vector<double>>(max_fac,std::vector<double>(2,0));

    P12 = std::vector<double>(2,0);
}

//------------------------------------------

GammaTracker::~GammaTracker()
{}

//--------------------------------------------------------------

void GammaTracker::get_sigmas(int pos, int perm_iter)
{

    int o = 0;

    double source[3] = {0, 0, 0};

    for (int i = 0; i < 3; ++i)
        source[i] = mu_vec[0][i];

    double x_s[2][3];
    for (int i = 1; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            x_s[i - 1][j] = mu_vec[i][j];

    //norm of vector x0 - source
    double norm1 = 0;
    //norm of vector x1 - x0
    double norm2 = 0;

    double scalar = 0;
    double c_th = 0, delta_cos_th = 0;
    double Da_1 = 0, Da_2 = 0, Db_1 = 0, Db_2 = 0, Da_0 = 0, Db_0 = 0;
    double delta_t1 = 0, delta_t2 = 0, delta_t0 = 0;

    for (int i = 0; i < 3; ++i)
    {
        norm1 += pow(x_s[o][i] - source[i], 2.);
        norm2 += pow(x_s[1 - o][i] - x_s[o][i], 2.);
        scalar += (x_s[o][i] - source[i]) * (x_s[1 - o][i] - x_s[o][i]);
    }
    norm1 = sqrt(norm1);
    norm2 = sqrt(norm2);

    c_th = scalar / (norm1 * norm2);

    double DD0 = 0, DD1 = 0, DD2 = 0;

    double a = scalar;
    double b = norm1 * norm2;

   
    angle_vec[pos - 1][0] = c_th;
    
    double delta_X[3];
    for(int i = 0;i < 3;++i)
        delta_X[i] = DX.GetUncertainty(pos,i,Target12);


    for (int i = 0; i < 3; ++i)
    {
        Da_0 = x_s[0][i] - x_s[1][i];
        Db_0 = norm2 / norm1 * (-x_s[0][i] + source[i]);

        DD0 = ((Da_0 * b - a * Db_0) / pow(b, 2)) * delta_X[0];

        delta_t0 += DD0 * DD0;

        Da_1 = x_s[1][i] - 2 * x_s[0][i] + source[i];
        Db_1 = norm2 / norm1 * (-source[i] + x_s[0][i]) + norm1 / norm2 * (-x_s[1][i] + x_s[0][i]);

        DD1 = ((Da_1 * b - a * Db_1) / pow(b, 2)) * delta_X[1];

        delta_t1 += DD1 * DD1;

        Da_2 = x_s[0][i] - source[i];
        Db_2 = norm1 / norm2 * (-x_s[0][i] + x_s[1][i]);

        DD2 = ((Da_2 * b - a * Db_2) / pow(b, 2)) * delta_X[2];

        delta_t2 += DD2 * DD2;
    }

    delta_cos_th = sqrt(delta_t1 + delta_t2 + delta_t0);

    sigmas[pos - 1] = std::abs(delta_cos_th)/sqrt(1 - c_th*c_th); 
}

//--------------------------------------------------------------

void GammaTracker::GetSigmaE(std::vector<double> &Edeps, int len)
{

    std::vector<double> EdepDelta(len, 0);
    std::vector<double> E(len, 0);
    std::vector<double> Edelta(len, 0);
    std::vector<double> delta_cth_E(len - 1, 0);
    for (int i = 0; i < len; ++i)
    {
        EdepDelta[i] = 0.002 * Edeps[i];
        E[i] = (i == 0) ? Eg : E[i - 1] - Edeps[i - 1];
        Edelta[i] = 0;
        for (int j = 0; j < i; ++j)
            Edelta[i - 1] += pow(EdepDelta[j], 2);
    }
    for (int i = 0; i < len; ++i)
        Edelta[i] = sqrt(Edelta[i]);
    std::vector<double> cth_E(len - 1, 0);
    for (int i = 1; i < len; ++i)
        cth_E[i - 1] = 1. - mc2 / E[i] + mc2 / E[i - 1];

    double tmp_Val = 0, DE = 0, DEd = 0;

    for (int i = 0; i < len - 1; ++i)
    {

        DE = mc2 / pow(E[i] - Edeps[i], 2) + mc2 / pow(E[i], 2);
        DEd = mc2 / pow(E[i] - Edeps[i], 2);

        tmp_Val = pow(Edelta[i] * DE, 2) + pow(EdepDelta[i] * DEd, 2);
        delta_cth_E[i] = sqrt(tmp_Val);

        sigma_Eth[i] = delta_cth_E[i]/sqrt(1. - pow(cth_E[i],2));
    }
}



//--------------------------------------------------------------

inline int GammaTracker::Factorial(int N)
{
    return (N <= 1) ? 1 : N * Factorial(N - 1);
} 

//--------------------------------------------------------------

double GammaTracker::GetUncertainty(std::vector<double> &Edep,
                                    const std::vector<std::vector<double>> &X)
{
    GetSigmaE(Edep,1);
    //for first point, set source position as start
    for (int k = 0; k < 3; ++k)
        mu_vec[0][k] = Source[0][k];
    

    //set (remaining) points for scattering angle comparison
    for (int j = 0; j < 2; ++j)
    {
        for (int k = 0; k < 3; ++k)
            mu_vec[j+1][k] = X[j][k];
    }

    //get sigmas from position error propagation
    get_sigmas(1,0);

    double sigTot = pow(sigma_Eth[0] * 180 / M_PI, 2) + pow(sigmas[0] * 180 / M_PI, 2);
    return sqrt(sigTot);
}

//--------------------------------------------------------------