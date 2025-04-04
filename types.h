#pragma once
#include <concepts>
#include <type_traits>
#include <complex>

namespace smip {

template <typename T>
concept concept_integral = std::is_integral_v<T>;
template <typename T>
concept concept_floating = std::is_floating_point_v<T>;
template <typename T>
concept concept_arithmetic = std::is_arithmetic_v<T>;
// Concept for std::valarray of arithmetic types
template <typename T>
concept concept_valarray_of_arithmetic = requires(T x) {
    typename T::value_type;
    requires std::is_arithmetic_v<typename T::value_type>;
};

typedef std::complex<double> complex_t;
typedef std::complex<float> bispec_complex_t;

} // namespace smip
