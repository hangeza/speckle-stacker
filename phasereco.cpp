#include <cmath>
#include <complex>
#include <cstdlib>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#include "constants.h"
#include "phasemap.h"
#include "phasereco.h"

namespace smip {

void NextRecoIndex(double& r, double& phi, int& i, int& j)
{
    constexpr double dr { 1.0 };
    const double dphi { 1.0 / (constants::c_2pi<double> * r) };
    //   cout<<"r="<<*r<<" , i="<<*i<<" , j="<<*j<<endl;
    int ii { i };
    int jj { j };

    while ((i == ii) && (j == jj)) {
        if ((phi += dphi) > constants::c_2pi<double>) {
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

} // namespace smip
