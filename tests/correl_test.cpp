#include <iostream>
#include <random>
#include "array2.h"
#include "math_functions.h"
#include "crosscorrel.h"

int correl_test(int /*argc*/, char* /*argv*/[])
{
    constexpr std::size_t N_trials { 100 };
    constexpr std::size_t xsize { 1000 };
    constexpr std::size_t ysize { 1000 };
    constexpr double sigma { 50. };
    smip::Array2<double> gaus( xsize, ysize );
    assert(gaus.size() == xsize*ysize);
    std::cout << "arr:\n";
    gaus.print();
//     std::cout << arr << "\n";
    
    std::vector<smip::DimVector<double,2>> gausparams { { 1., 1.}, { 0.5*xsize, 0.5*ysize }, { sigma, sigma } };
    
    for ( auto row { 0UL }; row < ysize; ++row) {
        for ( auto col { 0UL }; col < xsize; ++col) {
            gaus(col,row) = smip::Gauss<double, std::size_t>({col, row}, gausparams);
        }
    }
    std::random_device rd;  // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<int> distrib( gaus.min_sindices().max() + static_cast<int>(sigma), gaus.max_sindices().min() - static_cast<int>(sigma) - 1);
    smip::CrossCorrelation<double> cross_correl(gaus);
    for ( auto i { 0UL }; i < N_trials; ++i) {
        const smip::DimVector<int, 2> gen_shift{ distrib(gen), distrib(gen) };
//         std::cout << "shift wrt ref frame: generated [x,y] = " << gen_shift;
        auto shifted_gaus = gaus.shifted(gen_shift);
        auto meas_shift = cross_correl(shifted_gaus);
//         std::cout << " vs. measured  [x,y] = " << meas_shift << "\n";
        smip::DimVector<int,2> shift_diff { meas_shift - gen_shift };
        std::cout << "shift diff (generated vs. measured) [x,y] = " << shift_diff << "\n";
        assert(std::sqrt(shift_diff*shift_diff).max() <= 0.2 * sigma);
    }
    
    
    return 0;
}
