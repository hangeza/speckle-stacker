#pragma once
#include "types.h"
#include "point.h"
#include <cmath>
#include <limits>
#include <type_traits>

// #include "smip_export.h"

namespace smip {

/**
 * @brief Rect class to store boundaries of a 2d rectangle
 * @tparam T the data type of the coordinates
 */
template <typename T>
requires concept_arithmetic<T>
struct Rect {
    Point<T> topleft {};
    Point<T> bottomright {};
    Rect() = default;
    Rect(const Rect&) = default;
    Rect(Rect&&) = default;
    Rect(const Point<T>& a, const Point<T>& b);
    Rect(Point<T> a_center, T a_width, T a_height);
    auto width() const -> T;
    template <typename U = T, std::enable_if_t<!std::is_integral<U>::value, int> = 0>
    auto width() const -> T;
    auto height() const -> T;
    template <typename U = T, std::enable_if_t<!std::is_integral<U>::value, int> = 0>
    auto height() const -> T;
    auto area() const -> T;
    auto center() const -> Point<T>;
    void set_size(const Point<T>& sizes);
    Rect<T>& operator+=(const Point<T>& other);
    Rect<T>& operator-=(const Point<T>& other);
    // friends defined inside class body are inline and are hidden from non-ADL lookup
    friend Rect<T> operator+(Rect<T> lhs, const Point<T>& rhs)
    {
        // passing lhs by value helps optimize chained a+b+c
        // otherwise, both parameters may be const references
        lhs += rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }
    friend Rect<T> operator-(Rect<T> lhs, const Point<T>& rhs)
    {
        lhs -= rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }
};

//********************
// deduction guides
//********************
template <typename T>
requires concept_arithmetic<T>
Rect(Point<T> a, Point<T> b) -> Rect<T>;

//********************
// implementation part
//********************

template <typename T>
requires concept_arithmetic<T>
Rect<T>::Rect(const Point<T>& a, const Point<T>& b)
    : topleft { a }
    , bottomright { b }
{
}

template <typename T>
requires concept_arithmetic<T>
Rect<T>::Rect(Point<T> a_center, T a_width, T a_height)
    : topleft { a_center - Point<T> { a_width / 2, a_height / 2 } }
    , bottomright { topleft + Point<T> { a_width, a_height } }
{
}

template <typename T>
requires concept_arithmetic<T>
template <typename U, std::enable_if_t<!std::is_integral<U>::value, int>>
auto Rect<T>::width() const -> T
{
    return static_cast<T>(std::abs(bottomright.x - topleft.x) + std::numeric_limits<T>::epsilon());
}

template <typename T>
requires concept_arithmetic<T>
auto Rect<T>::width() const -> T
{
    auto maxval { std::max(bottomright.x, topleft.x) };
    auto minval { std::min(bottomright.x, topleft.x) };
    return static_cast<T>(maxval - minval + T(1));
}

template <typename T>
requires concept_arithmetic<T>
template <typename U, std::enable_if_t<!std::is_integral<U>::value, int>>
auto Rect<T>::height() const -> T
{
    return static_cast<T>(std::abs(bottomright.y - topleft.y) + std::numeric_limits<T>::epsilon());
}

template <typename T>
requires concept_arithmetic<T>
auto Rect<T>::height() const -> T
{
    auto maxval { std::max(bottomright.y, topleft.y) };
    auto minval { std::min(bottomright.y, topleft.y) };
    return static_cast<T>(maxval - minval + T(1));
}

template <typename T>
requires concept_arithmetic<T>
auto Rect<T>::area() const -> T
{
    return width() * height();
}

template <typename T>
requires concept_arithmetic<T>
auto Rect<T>::center() const -> Point<T>
{
    return topleft + Point<T> { width() / 2, height() / 2 };
}

template <typename T>
requires concept_arithmetic<T>
void Rect<T>::set_size(const Point<T>& sizes)
{
    bottomright = topleft + sizes;
}

// compound assignment (does not need to be a member,
// but often is, to modify the private members)
template <typename T>
requires concept_arithmetic<T>
Rect<T>& Rect<T>::operator+=(const Point<T>& other)
{
    topleft += other;
    bottomright += other;
    return *this; // return the result by reference
}

template <typename T>
requires concept_arithmetic<T>
Rect<T>& Rect<T>::operator-=(const Point<T>& other)
{
    topleft -= other;
    bottomright -= other;
    return *this; // return the result by reference
}

template <typename T>
requires concept_arithmetic<T>
std::ostream& operator<<(std::ostream& os, const Rect<T>& obj)
{
    // write obj to stream
    os << "( " << obj.topleft << " " << obj.bottomright << " )";
    return os;
}

} // namespace smip
