#pragma once
#include <cmath>
#include "types.h"

namespace smip {

/**
 * @brief Point class to store 2d point coordinates
 * @tparam T the data type of the coordinates
 */
template <typename T>
requires concept_arithmetic<T>
struct Point {
    T x {};
    T y {};
    Point<T>& operator+=(const Point<T>& other);
    Point<T>& operator-=(const Point<T>& other);
    // friends defined inside class body are inline and are hidden from non-ADL lookup
    friend Point<T> operator+(Point<T> lhs, const Point<T>& rhs)
    {
        // passing lhs by value helps optimize chained a+b+c
        // otherwise, both parameters may be const references
        lhs += rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }
    friend Point<T> operator-(Point<T> lhs, const Point<T>& rhs)
    {
        // passing lhs by value helps optimize chained a+b+c
        // otherwise, both parameters may be const references
        lhs -= rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }
};

//********************
// deduction guides
//********************
template <typename T>
requires concept_arithmetic<T>
Point(T a, T b) -> Point<T>;

//********************
// implementation part
//********************

// compound assignment (does not need to be a member,
// but often is, to modify the private members)
template <typename T>
requires concept_arithmetic<T>
Point<T>& Point<T>::operator+=(const Point<T>& other)
{
    x += other.x;
    y += other.y;
    return *this; // return the result by reference
}

template <typename T>
requires concept_arithmetic<T>
Point<T>& Point<T>::operator-=(const Point<T>& other)
{
    x -= other.x;
    y -= other.y;
    return *this; // return the result by reference
}

template <typename T>
requires concept_arithmetic<T>
std::ostream& operator<<(std::ostream& os, const Point<T>& obj)
{
    // write obj to stream
    os << "(" << obj.x << "," << obj.y << ")";
    return os;
}

} // namespace smip
