#pragma once
#include <cmath>
#include "point.h"

template <typename T>
struct Rect {
    Point<T> topleft {};
    Point<T> bottomright {};
    Rect() = default;
    Rect(const Point<T>& a, const Point<T>& b);
    Rect(Point<T> a_center, T a_width, T a_height);
    auto width() const -> T;
    auto height() const -> T;
    auto area() const -> T;
    auto center() const -> Point<T>;
};


//********************
// deduction guides
//********************
template <typename T>
Rect(Point<T> a, Point<T> b) -> Rect<T>;


//********************
// implementation part
//********************

template <typename T>
Rect<T>::Rect(const Point<T>& a, const Point<T>& b)
    : topleft{a}, bottomright{b} 
{
}

template <typename T>
Rect<T>::Rect(Point<T> a_center, T a_width, T a_height)
    : topleft{a_center - Point<T>{a_width/2, a_height/2}}
    , bottomright{ topleft + Point<T>{ a_width, a_height } }
{
}

template <typename T>
auto Rect<T>::width() const -> T 
{ 
    return static_cast<T>(std::abs(static_cast<double>(bottomright.x) - topleft.x));
}

template <typename T>
auto Rect<T>::height() const -> T 
{ 
    return static_cast<double>(std::abs(static_cast<double>(bottomright.y) - topleft.y));
}

template <typename T>
auto Rect<T>::area() const -> T 
{ 
    return width() * height(); 
}

template <typename T>
auto Rect<T>::center() const -> Point<T> 
{ 
    return topleft + Point<T>{ width()/2, height()/2}; 
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Rect<T>& obj)
{
    // write obj to stream
    os << "( " << obj.topleft << " " << obj.bottomright << " )";
    return os;
}
