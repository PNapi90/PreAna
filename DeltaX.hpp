#ifndef DELTAX_HPP
#define DELTAX_HPP

#include <vector>
#include <iostream>


class DeltaX
{
private:
  const double deltaX_Agata = 2.13;
  const double deltaX_Lycca[4] = {14.5, 7.4, 1.04, 0.52};

public:
    DeltaX();
    ~DeltaX();

    double GetUncertainty(int pos_Gamma,
                          int xyz,
                          int Target12);
};


#endif