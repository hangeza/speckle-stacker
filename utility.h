#pragma once
#include <cmath>
#include <complex>
#include "types.h"

namespace smip {

struct PhaseMapElement;

static constexpr double c_2pi { 6.2831853071796E+00 }; /* 2*pi */
static constexpr double c_sqrt_2pi { std::sqrt(6.2831853071796E+00) }; /* sqrt(2*pi) */

template <concept_arithmetic T>
T complex_phase(const std::complex<T>& a) { return (std::arg(a) + M_PI) / (M_PI * 2); }
template <concept_arithmetic T>
T complex_abs(const std::complex<T>& z) { return std::abs(z); }
double get_phase_consistency(const PhaseMapElement& pme);


} // namespace smip
