#include <cmath>
#include <complex>
#include <cstdlib>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#include "multidimarray.h"
#include "phasemap.h"
#include "phasereco.h"

static constexpr double c_2pi { 6.2831853071796E+00 }; /* 2*pi */

void NextRecoIndex(double& r, double& phi, int& i, int& j)
{
    constexpr double dr { 1.0 };
    const double dphi { 1.0 / (c_2pi * r) };
    //   cout<<"r="<<*r<<" , i="<<*i<<" , j="<<*j<<endl;
    int ii { i };
    int jj { j };

    while ((i == ii) && (j == jj)) {
        if ((phi += dphi) > c_2pi) {
            phi = 0;
            r += dr;
            //         std::cout<<"r="<<*r<<std::endl;
        }
        ii = static_cast<int>(r * std::cos(phi));
        jj = static_cast<int>(r * std::sin(phi));
    }
    i = ii;
    j = jj;
}
