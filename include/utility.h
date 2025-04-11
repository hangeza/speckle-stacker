#pragma once
#include "types.h"
#include <cmath>
#include <complex>
#include <constants.h>

#include "smip_export.h"

namespace smip {

struct PhaseMapElement;

template <concept_arithmetic T>
T SMIP_EXPORT complex_phase(const std::complex<T>& a) { return (std::arg(a) + M_PI) / (M_PI * 2); }
template <concept_arithmetic T>
T SMIP_EXPORT complex_abs(const std::complex<T>& z) { return std::abs(z); }
double SMIP_EXPORT get_phase_consistency(const PhaseMapElement& pme);

} // namespace smip
