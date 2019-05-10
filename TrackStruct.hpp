#ifndef TRACKSTRUCT_H
#define TRACKSTRUCT_H

struct Tracked
{
    double gammaX[2], gammaY[2], gammaZ[2];
    double E[2];
    double SourceXY[2];

    double dE_Lycca;
    double E_Lycca;
    int mult_Lycca;

    Tracked()
    {
        for (int i = 0; i < 2; ++i)
        {
            gammaX[i] = 0;
            gammaY[i] = 0;
            gammaZ[i] = 0;
            E[i] = 0;
        }
        for (int i = 0; i < 2; ++i)
            SourceXY[i] = 0;

        dE_Lycca = 0;
        E_Lycca = 0;
        mult_Lycca = 0;
    };
    ~Tracked(){};
};

#endif