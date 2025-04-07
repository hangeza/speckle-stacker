#pragma once
#include <cmath>
#include <limits>
#include <type_traits>
#include "types.h"

namespace smip {

/**
 * @brief Range class for indicating a contiguous range of values between start and end
 * @tparam T the data type of the value range
 */
template <typename T>
requires concept_arithmetic<T> || concept_valarray_of_arithmetic<T>
struct Range {
    T start {};
    T end {};
    bool contains(const T& value) const;
    /**
    * @todo: still to be implemented for valarray-based types
    */
    template <typename U = T>
    requires concept_floating<U>
    auto extent() const -> T;
    /**
    * @todo: still to be implemented for valarray-based types
    */
    template <typename U = T>
    requires concept_integral<U>
    auto extent() const -> T;
};

//********************
// deduction guides
//********************
template <typename T>
requires concept_arithmetic<T>
Range(T a, T b) -> Range<T>;

//********************
// implementation part
//********************

template <typename T>
requires concept_arithmetic<T> || concept_valarray_of_arithmetic<T>
bool Range<T>::contains(const T& value) const
{
    if constexpr (std::is_arithmetic_v<T>) {
        return ((value >= start) && (value <= end));
    } else if constexpr (concept_valarray_of_arithmetic<T>) {
        assert(value.size() == start.size());
        for (auto it {std::begin(value)}; it != std::end(value); ++it) {
            auto pos { std::distance(std::begin(value), it) };
            if ( *it < start[pos] || *it > end[pos] ) return false;
        }
        return true;
    }
}

template <typename T>
requires concept_arithmetic<T>
template <typename U>
requires concept_floating<U>
auto Range<T>::extent() const -> T
{
    return static_cast<T>(std::abs(end - start) + std::numeric_limits<T>::epsilon());
}

template <typename T>
requires concept_arithmetic<T>
template <typename U>
requires concept_integral<U>
auto Range<T>::extent() const -> T
{
    return static_cast<T>(std::abs(end - start) + T(1));
}

template <typename T>
requires concept_arithmetic<T> || concept_valarray_of_arithmetic<T>
std::ostream& operator<<(std::ostream& os, const Range<T>& obj)
{
    // write obj to stream
    os << "(" << obj.start << "," << obj.end << ")";
    return os;
}

} // namespace smip
