#pragma once
#include "types.h"
#include <cmath>
#include <complex>
#include <constants.h>

#include "global.h"
// #include "smip_export.h"

namespace smip {

struct PhaseMapElement;

template <concept_arithmetic T>
T complex_phase(const std::complex<T>& a) { return (std::arg(a) + M_PI) / (M_PI * 2); }
template <concept_arithmetic T>
T complex_abs(const std::complex<T>& z) { return std::abs(z); }
double SMIP_PUBLIC get_phase_consistency(const PhaseMapElement& pme);

} // namespace smip
