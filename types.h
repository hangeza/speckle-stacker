#pragma once
#include <concepts>
#include <type_traits>

namespace smip {

template <typename T>
concept concept_integral = std::is_integral_v<T>;
template <typename T>
concept concept_floating = std::is_floating_point_v<T>;
template <typename T>
concept concept_arithmetic = std::is_arithmetic_v<T>;

// typedef Array2<PhaseMapElement> PhaseMap;
// typedef std::complex<double> complex_t;
// typedef std::complex<float> bispec_complex_t;


} // namespace smip
