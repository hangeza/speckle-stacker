#pragma once

#include "constants.h"
#include "dimvector.h"
#include "types.h"
#include "utility.h"
#include <array>
#include <cmath>
#include <complex>
#include <type_traits>
#include <valarray>
#include <vector>

namespace smip {

template <typename T>
requires concept_arithmetic<T> || concept_valarray_of_arithmetic<T>
constexpr T sqr(T x)
{
    //static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type");
    return x * x;
}

template <concept_floating F, concept_arithmetic X>
F Gauss(const X& x, const std::vector<F>& params);
template <concept_floating F, concept_arithmetic X>
F Gauss(const DimVector<X, 2>& x, const std::vector<DimVector<F, 2>>& params);

//********************
// implementation part
//********************

template <concept_floating F, concept_arithmetic X>
F Gauss(const X& x, const std::vector<F>& params)
{
    assert(params.size() == 3);
    const F& p0 = params[0];
    const F& p1 = params[1];
    const F& p2 = params[2];
    F f_x { std::exp(-sqr(static_cast<F>(x) - p1) / (sqr(p2) * 2)) / (constants::c_sqrt_2pi<double> * p2) };
    return f_x;
}

template <concept_floating F, concept_arithmetic X>
F Gauss(const DimVector<X, 2>& x, const std::vector<DimVector<F, 2>>& params)
{
    assert(params.size() == 3);
    DimVector<F, 2> xf { static_cast<F>(x[0]), static_cast<F>(x[1]) };
    DimVector<F, 2> p0 = params[0];
    DimVector<F, 2> p1 = params[1];
    DimVector<F, 2> p2 = params[2];
    DimVector<F, 2> f_xy { xf - p1 };
    f_xy *= -f_xy;
    f_xy /= sqr(p2) * 2;
    f_xy = std::exp(f_xy);
    f_xy /= p2 * constants::c_sqrt_2pi<double>;
    f_xy *= p0;
    return f_xy.product();
}

} // namespace smip
