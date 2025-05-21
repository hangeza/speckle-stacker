#include "array2.h"
#include "crosscorrel.h"
#include "math_functions.h"
#include "videoio.h"
#include <iostream>
#include <random>

int correl_test(int /*argc*/, char* /*argv*/[])
{
    constexpr std::size_t N_trials { 100 };
    constexpr std::size_t xsize { 128 };
    constexpr std::size_t ysize { 128 };
    constexpr double sigma { 5. };
    smip::Array2<double> gaus(xsize, ysize);
    smip::Array2<double> shiftsum(xsize, ysize , 0.);
    assert(gaus.size() == xsize * ysize);
    std::cout << "arr:\n";
    gaus.print();

    std::vector<smip::DimVector<double, 2>> gausparams { {sigma*2.*3.14, sigma }, { 0.5 * xsize, 0.5 * ysize }, { sigma, sigma } };

    for (std::size_t row {}; row < ysize; ++row) {
        for (std::size_t col {}; col < xsize; ++col) {
            gaus.at({ static_cast<int>(col), static_cast<int>(row) }) = smip::Gauss<double, std::size_t>({ col, row }, gausparams);
            //std::cout << col << "," << row << ": " << gaus.at({col, row});
        }
    }
    std::random_device rd; // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<int> distrib(gaus.min_sindices().max() + static_cast<int>(sigma), gaus.max_sindices().min() - static_cast<int>(sigma) - 1);
    std::cout << " random int value: " << distrib(gen) << "\n";
    smip::CrossCorrelation<double> cross_correl(gaus);
    for (auto i { 0UL }; i < N_trials; ++i) {
        const smip::DimVector<int, 2> gen_shift { distrib(gen), distrib(gen) };
        auto shifted_gaus = gaus.shifted(gen_shift);
        smip::DimVector<int, 2> meas_shift { cross_correl(shifted_gaus) };
        smip::DimVector<int, 2> shift_diff { meas_shift - gen_shift };
        shiftsum += shifted_gaus.shifted(-meas_shift);
        std::cout << "shift diff (generated vs. measured) [x,y] = " << shift_diff << "\n";
        assert(std::sqrt(shift_diff * shift_diff).max() <= 0.2 * sigma);
    }
    shiftsum.print();
    shiftsum /= N_trials;
    
    auto diff_image = shiftsum - gaus;
    
    auto sum_squares = std::accumulate( 
        diff_image.begin(),
        diff_image.end(),
        0.,
        [](double acc, double x){ 
            return std::move(acc) + smip::sqr(x);
        }
    );
    std:: cout << "sum of squares = " << sum_squares << " s.o.sq per elem = " << sum_squares/gaus.size() << "\n";
    
    smip::save_frame(Array2Mat<double, double, CV_16UC3>(shiftsum, std::fabs<double>, false), "shiftsum_test.png");
    return 0;
}
