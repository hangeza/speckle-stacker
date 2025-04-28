#pragma once
#include <complex>
#include <concepts>
#include <type_traits>
#include <valarray>

template <typename T>
concept concept_integral = std::is_integral_v<T>;
template <typename T>
concept concept_floating = std::is_floating_point_v<T>;
template <typename T>
concept concept_arithmetic = std::is_arithmetic_v<T>;
/**
 * @brief Concept for std::valarray of arithmetic types.
 * This is a shorthand definition describing a concept to select only types with following properties:
 * * a std::valarray<T> with T an arithmetic type (integral or floating type)
 * * a type derived from std::valarray<T> with T an arithmetic type
 */
template <typename T>
concept concept_valarray_of_arithmetic = (std::is_same_v<T, std::valarray<typename std::decay_t<T>::value_type>> && std::is_arithmetic_v<typename std::decay_t<T>::value_type>) || (std::is_base_of_v<std::valarray<typename std::decay_t<T>::value_type>, T> && std::is_arithmetic_v<typename std::decay_t<T>::value_type>);

template <typename T>
concept concept_complex = std::is_same_v<T, std::complex<typename std::decay_t<T>::value_type>>;

namespace smip {

typedef std::complex<double> complex_t;
typedef std::complex<float> bispec_complex_t;

} // namespace smip
