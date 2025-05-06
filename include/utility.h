#pragma once

#include "types.h"
#include <cmath>
#include <complex>
#include <constants.h>
#include <version> // For feature-test macros, if available

#include "global.h"

// Only define std::unreachable if not already available
// it should be part of std lib from C++23 (202202L)
// see https://en.cppreference.com/w/cpp/utility/unreachable
#if !defined(__cpp_lib_unreachable) //|| __cpp_lib_unreachable < 202202L
namespace std {
[[noreturn]] inline void unreachable()
{
    // Compiler-specific unreachable markers
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
    __assume(false);
#else // GCC, Clang
    __builtin_unreachable();
#endif
}
}
#endif

namespace smip {

struct PhaseMapElement;

template <concept_arithmetic T>
T complex_phase(const std::complex<T>& a) { return (std::arg(a) + M_PI) / (M_PI * 2); }
template <concept_arithmetic T>
T complex_abs(const std::complex<T>& z) { return std::abs(z); }
double SMIP_PUBLIC get_phase_consistency(const PhaseMapElement& pme);

} // namespace smip
