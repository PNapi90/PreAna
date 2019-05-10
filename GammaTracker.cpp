#include "GammaTracker.hpp"

//------------------------------------------

GammaTracker::GammaTracker(const double _Eg)
    : multiplicity(0),
      DX(),
      Esum(0),
      Eg(_Eg)
{
    GammaBuffer = std::vector<std::vector<double>>(100,std::vector<double>(4,0));
    Source = std::vector<std::vector<double>>(2, std::vector<double>(3, 0));
    mu_vec = std::vector<std::vector<double>>(3, std::vector<double>(3, 0));
    

    
    std::vector<std::vector<double>> delta_X(3, std::vector<double>());
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

//------------------------------------------

void GammaTracker::Feed(GammaAna *Gamma)
{
    //set source positions and measured Agata data
    multiplicity = Gamma->nPSA;
    Esum = 0;
    for(int i = 0;i < multiplicity;++i)
    {
        GammaBuffer[i][0] = Gamma->EPSA[i];
        GammaBuffer[i][1] = Gamma->xPSA[i];
        GammaBuffer[i][2] = Gamma->yPSA[i];
        GammaBuffer[i][3] = Gamma->zPSA[i];

        Esum += GammaBuffer[i][0];
    }
    for(int i = 0;i < 2;++i)
    {
        for(int j = 0;j < 2;++j)
        {
            //Source position from Lycca
            Source[i][j] = Gamma->SourceXY[j];
        }
        //z position of targets (0 cm and 10 cm from AGATA nominal)
        Source[i][2] = -100*i;
    }



    //do tracking
    double sumP12 = 0; 
    for(int i = 0;i < 2;++i)
    {    
        P12[i] = Tracking(i);
        sumP12 += P12[i];
    }
    if(sumP12 > 0)
    {
        for(int i = 0;i < 2;++i)
            P12[i] /= 1;//sumP12;
    }
    Asymmetry = 0;
    if(P12[0] + P12[1] > 0)
        Asymmetry = (P12[0] - P12[1])/(P12[0] + P12[1]);

}

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

    double Da2_0 = 0, Da2_1 = 0, Da2_2 = 0;
    double Db2_0 = 0, Db2_1 = 0, Db2_2 = 0;

    int factor = 1;

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

double GammaTracker::Tracking(int Target12)
{   
    this->Target12 = Target12;
    int iter = multiplicity;

    std::vector<int> sortarray(iter, 0);
    for (int i = 0; i < iter; ++i)
        sortarray[i] = i;

    int start_val = 0;
    int perm_iter = 0;

    double Estart = 0;

    double delta_tmp = 0;

    std::sort(sortarray.begin(), sortarray.end());

    //PrintDataSet(iter);

    std::vector<double> edeps(iter, 0);
    double tmpS = 0, PValue = 0;

    std::vector<double> EtmpVec(2, 0);

    bool FirstStep = true;

    double PVal = 0;
    double Ptmp = 0;

    //Permutations of all convoluted points
    do
    {

        Estart = Esum;
        
        delta_tmp = 0;

        for (int i = 0; i < iter; ++i)
            edeps[i] = GammaBuffer[sortarray[i]][0];

        GetSigmaE(edeps, iter);

        FirstStep = true;

        //loop over all points of permutation set (not last point since
        //no scattering information available)
        Ptmp = 1;
        for (int i = 0; i < iter - 1; ++i)
        {
            //for first point, set source position as start
            start_val = (i == 0) ? 1 : 0;

            if (i == 0)
            {
                for (int k = 0; k < 3; ++k)
                    mu_vec[0][k] = Source[Target12][k];
            }

            //set (remaining) points for scattering angle comparison
            for (int j = start_val; j < 3; ++j)
            {
                for (int k = 0; k < 3; ++k)
                    mu_vec[j][k] = GammaBuffer[sortarray[i + j - 1]][k + 1];
            }

            //reset scattering angles (j*10000 to ensure large difference if not used)
            for (int j = 0; j < 2; ++j)
                angle_vec[i][j] = j * 10000;

            //get sigmas from position error propagation
            get_sigmas(i + 1, perm_iter);
            //get Compton scattering angles from energies
            get_E_angle(Estart, GammaBuffer[sortarray[i]][0], i);

            tmpS = pow(sigma_Eth[i], 2) + pow(sigmas[i], 2);
            if (tmpS != tmpS)
            {
                tmpS = 0;
                delta_tmp = WRONG_CASE;
            }
            else
                delta_tmp = std::abs(angle_vec[i][0] - angle_vec[i][1]) / std::sqrt(tmpS);
            
            //probablilities for each permutation (multiplied)
            if(delta_tmp != WRONG_CASE)
                Ptmp *= 1./sqrt(2*M_PI*tmpS)*exp(-0.5*delta_tmp*delta_tmp);

            //set for intersection only after first iteration
            FirstStep = false;

            //if untrackable part reached -> break
            //(no additional calculations needed for permutation)
            //if (delta_tmp >= 5)
            //    break;

            //set "incident" photon energy for next iteration
            Estart -= GammaBuffer[sortarray[i]][0];
        }
        //total probability for all permutations (sum of all Ptmp)
        PVal += Ptmp;
        //increment permutation iterator
        ++perm_iter;

    } while (std::next_permutation(sortarray.begin(), sortarray.end()));


    return PVal;
}

//--------------------------------------------------------------

void GammaTracker::get_E_angle(double Estart, double Ed, int pos)
{

    double cth = 0;

    if (Estart < Ed)
    {
        angle_vec[pos][1] = WRONG_CASE;
        return;
    }

    cth = 1. - mc2 / (Estart - Ed) + mc2 / Estart;

    if (cth < -1)
    {
        angle_vec[pos][1] = WRONG_CASE;
        return;
    }
    angle_vec[pos][1] = cth;
} 

//--------------------------------------------------------------

inline int GammaTracker::Factorial(int N)
{
    if(N <= 1)
        return 1;
    else
    {
        return N*Factorial(N-1);
    }
} 

//--------------------------------------------------------------

double GammaTracker::getAsymmetry()
{
    return Asymmetry;
}

//--------------------------------------------------------------

double GammaTracker::getP12(int pos)
{
    return P12[pos];
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