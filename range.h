#pragma once
#include <cmath>
#include <limits>
#include <type_traits>
#include <stdexcept>
#include <valarray>
#include <tuple>
#include <ranges>

#include "types.h"

namespace smip {

/**
 * @brief Range class for indicating a contiguous range of values between start and end
 * @tparam T the data type of the value range
 */
template <typename T>
requires concept_arithmetic<T> || concept_valarray_of_arithmetic<T>
class Range {
public:
    T low {};
    T high {};

    Range(T lo, T hi) : low(lo), high(hi) {}
    /**
    * @brief test if value is contained inside the range
    * @param value value to check, T is of type arithmetic scalar or std::valarray (or derived) of arithmetic type
    */
    bool contains(const T& value) const;
    /**
    * @brief get extent of the range for a single arithmetic range instance
    */
    template <typename U = T>
    requires concept_arithmetic<U>
    auto extent() const -> T;
    /**
    * @brief get extents of the ranges for a std::valarray range instance
    */
    template <typename U = T>
    requires concept_valarray_of_arithmetic<U>
    auto extent() const -> T;
    
    // Iterator types
    template <typename U = T>
    requires concept_integral<U> || ( concept_valarray_of_arithmetic<U> && concept_integral<typename std::decay_t<U>::value_type> )
    class iterator {
    private:
        T m_current, m_low, m_high;
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        iterator() = default;
        iterator(T current, T low, T high) : m_current(current), m_low(low), m_high(high) {}
        value_type operator*() const { return m_current; }
        // Prefix increment
        iterator& operator++(); /*{ if (m_current<m_high) ++m_current; return *this; }*/
        // Postfix increment
        iterator operator++(int) { iterator temp = *this; ++(*this); return temp; }
        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;
    };
    template <typename U = T>
    requires concept_integral<U> || ( concept_valarray_of_arithmetic<U> && concept_integral<typename std::decay_t<U>::value_type> )
    iterator<U> begin() const { return iterator<U>(low, low, high); }
    template <typename U = T>
    requires concept_integral<U> || ( concept_valarray_of_arithmetic<U> && concept_integral<typename std::decay_t<U>::value_type> )
    iterator<U> end() const { return iterator<U>(high+1, low, high); }

};

//********************
// deduction guides
//********************
template <typename T>
requires concept_arithmetic<T> || concept_valarray_of_arithmetic<T>
Range(T a, T b) -> Range<T>;

//********************
// implementation part
//********************

template <typename T>
requires concept_arithmetic<T> || concept_valarray_of_arithmetic<T>
bool Range<T>::contains(const T& value) const
{
    if constexpr (std::is_arithmetic_v<T>) {
        return ((value >= low) && (value <= high));
    } else if constexpr (concept_valarray_of_arithmetic<T>) {
        assert(value.size() == low.size());
        for (auto it {std::begin(value)}; it != std::end(value); ++it) {
            auto pos { std::distance(std::begin(value), it) };
            if ( *it < low[pos] || *it > high[pos] ) return false;
        }
        return true;
    }
    throw std::invalid_argument("calling Range<T>::contains() with invalid template type");
}

template <typename T>
requires concept_arithmetic<T> || concept_valarray_of_arithmetic<T>
template <typename U>
requires concept_arithmetic<U>
auto Range<T>::extent() const -> T
{
    if constexpr ( std::is_floating_point_v<U> ) {
        return static_cast<T>(std::abs(high - low) + std::numeric_limits<T>::epsilon());
    } else if constexpr ( std::is_integral_v<U> ) {
        return static_cast<T>(std::abs(high - low) + T(1));
    }
    throw std::invalid_argument("using Range<T>::extent with invalid template type");
}

template <typename T>
requires concept_arithmetic<T> || concept_valarray_of_arithmetic<T>
template <typename U>
requires concept_valarray_of_arithmetic<U>
auto Range<T>::extent() const -> T
{
    using val_type = typename std::decay_t<U>::value_type;
    if constexpr ( std::is_floating_point_v<val_type> ) {
        return std::abs(high - low) + std::numeric_limits<val_type>::epsilon();
    } else if constexpr ( std::is_integral_v<val_type> ) {
        return std::abs(high - low) + val_type(1);
    }
    throw std::invalid_argument("using Range<T>::extent with invalid template type");
}

template <typename T>
requires concept_arithmetic<T> || concept_valarray_of_arithmetic<T>
template <typename U>
requires concept_integral<U> || ( concept_valarray_of_arithmetic<U> && concept_integral<typename std::decay_t<U>::value_type> )
Range<T>::iterator<U>& Range<T>::iterator<U>::operator++()
{
    if constexpr (concept_arithmetic<T>) {
        if (m_current<m_high) ++m_current;
        return *this;
    } else if constexpr (concept_valarray_of_arithmetic<T>) {
        // go through each dimension and increment if not at upper bound
        // Using std::ranges::zip to iterate over multiple containers simultaneously
        auto pos = std::begin(m_current);
        auto low = std::begin(m_low);
        auto high = std::begin(m_high);
        bool at_end { false };
        // Loop through containers using iterators
        // from C++23 on the following range-for loop is possible:
        // for (auto& [pos, low, high] : zip(m_current, m_low, m_high)) {
        for (;pos != std::end(m_current) && low != std::end(m_low) && high != std::end(m_high); ++pos, ++low, ++high) {
            if (*pos < *high) { 
                (*pos)++;
                at_end = false;
                break;
            } else {
                *pos = *low;
                at_end = true;
            }
        }
        if (at_end) m_current = m_high + 1;
        return *this;
    }
    throw std::invalid_argument("using Range<T>::iterator::operator++() with invalid template type");
}

template <typename T>
requires concept_arithmetic<T> || concept_valarray_of_arithmetic<T>
template <typename U>
requires concept_integral<U> || ( concept_valarray_of_arithmetic<U> && concept_integral<typename std::decay_t<U>::value_type> )
bool Range<T>::iterator<U>::operator==(const iterator& other) const
{
    if constexpr (concept_arithmetic<T>) {
        return (m_current == other.m_current);
    } else if constexpr (concept_valarray_of_arithmetic<T>) {
        return (m_current == other.m_current).min();
    }
    throw std::invalid_argument("using Range<T>::iterator::operator++() with invalid template type");
}

template <typename T>
requires concept_arithmetic<T> || concept_valarray_of_arithmetic<T>
template <typename U>
requires concept_integral<U> || ( concept_valarray_of_arithmetic<U> && concept_integral<typename std::decay_t<U>::value_type> )
bool Range<T>::iterator<U>::operator!=(const iterator& other) const
{
    if constexpr (concept_arithmetic<T>) {
        return (m_current != other.m_current);
    } else if constexpr (concept_valarray_of_arithmetic<T>) {
        return !(m_current == other.m_current).min();
    }
    throw std::invalid_argument("using Range<T>::iterator::operator++() with invalid template type");
}

template <typename T>
requires concept_arithmetic<T> || concept_valarray_of_arithmetic<T>
std::ostream& operator<<(std::ostream& os, const Range<T>& obj)
{
    // write obj to stream
    os << "(" << obj.low << "," << obj.high << ")";
    return os;
}

} // namespace smip
