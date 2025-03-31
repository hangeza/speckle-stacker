#pragma once
#include <algorithm>
#include <climits>
#include <complex>
#include <concepts>
#include <cstring>
#include <errno.h>
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>

#include "array_base.h"
#include "dimvector.h"
#include "rect.h"

namespace smip {
template <typename T>
class Array2;
}
template <typename T>
std::ostream& operator<<(std::ostream& o, smip::Array2<T>& v);
template <typename T>
std::ostream& operator<<(std::ostream& o, const std::vector<T>& v);

namespace smip {

//! Container class for 2d arrays
/*!
 * ...
*/
template <typename T>
class Array2 : public Array_base<T> {
    using Array_base<T>::_mem;
    using Array_base<T>::_size;

public:
    typedef DimVector<std::size_t, 2> extends;
    typedef DimVector<int, 2> s_indices;
    typedef DimVector<std::size_t, 2> u_indices;

    Array2() = default;
    // Copy constructor from Array2 of same type
    Array2(const Array2<T>& src);
    // Copy constructor from Array2 of foreign arithmetic type
    template <concept_arithmetic U>
    Array2(const Array2<U>& src);

    // Move constructor
    Array2(Array2<T>&& other) noexcept;
    Array2(std::size_t xsize, std::size_t ysize);
    Array2(std::size_t xsize, std::size_t ysize, const T& init);
    Array2(const extends& a_extends);
    Array2(const extends& a_extends, const T& init);
    Array2(std::initializer_list<std::initializer_list<T>> l);
    ~Array2() = default;

    // Assignment operator
    Array2<T>& operator=(const Array2<T>& x);
    // Assignment operator from foreign arithmetic Array2 type
    template <concept_arithmetic U>
    Array2<T>& operator=(const Array2<U>& x);
    // Assignment operator from value
    Array2<T>& operator=(const T& val);
    // Assignment operator from value of foreign arithmetic type
    template <concept_arithmetic U>
    Array2<T>& operator=(const U& val);
    // Move assignment operator
    Array2<T>& operator=(Array2<T>&& other) noexcept;
    // Compound assignment operators from same type
    Array2<T>& operator+=(const Array2<T>& x);
    Array2<T>& operator-=(const Array2<T>& x);
    Array2<T>& operator*=(const Array2<T>& x);
    Array2<T>& operator/=(const Array2<T>& x);
    // Compound assignment operators from foreign arithmetic Array2 type
    template <concept_arithmetic U>
    Array2<T>& operator+=(const Array2<U>& x);
    template <concept_arithmetic U>
    Array2<T>& operator-=(const Array2<U>& x);
    template <concept_arithmetic U>
    Array2<T>& operator*=(const Array2<U>& x);
    template <concept_arithmetic U>
    Array2<T>& operator/=(const Array2<U>& x);
    // Compound assignment operator from value
    Array2<T>& operator+=(const T& val);
    Array2<T>& operator-=(const T& val);
    Array2<T>& operator*=(const T& val);
    Array2<T>& operator/=(const T& val);
    // Compound assignment operator from value of foreign arithmetic type
    template <concept_arithmetic U>
    Array2<T>& operator+=(const U& val);
    template <concept_arithmetic U>
    Array2<T>& operator-=(const U& val);
    template <concept_arithmetic U>
    Array2<T>& operator*=(const U& val);
    template <concept_arithmetic U>
    Array2<T>& operator/=(const U& val);
    // Unary plus operator (identity, returns the matrix as-is)
    Array2<T> operator+() const;
    // Unary minus operator (negates all elements of the matrix)
    Array2<T> operator-() const;

    const T* operator[](int row) const;
    T* operator[](int row);
    // Access with operator() for non-const objects (x[y] access)
    T& operator()(std::size_t col, std::size_t row);
    // Access with operator() for const objects (x[y] access)
    const T& operator()(std::size_t col, std::size_t row) const;

    /*! returns const element with given signed indices */
    const T& at(s_indices indices) const;
    /*! returns element with given signed indices */
    T& at(s_indices indices);

    std::vector<T> get_col(std::size_t col) const;
    std::vector<T> get_row(std::size_t row) const;
    Array2<T> get_subarray(const Rect<std::size_t>& rect);
    void shift(const DimVector<int, 2>& distance);
    Array2<T> shifted(const DimVector<int, 2>& distance);

    std::size_t xsize() const { return m_xsize; }
    std::size_t ysize() const { return m_ysize; }
    std::size_t ncols() const { return m_xsize; }
    std::size_t nrows() const { return m_ysize; }
    s_indices min_sindices() const { return { -static_cast<int>(ncols() >> 1), -static_cast<int>(nrows() >> 1) }; }
    s_indices max_sindices() const
    {
        return {
            -static_cast<int>(ncols()) / 2 + static_cast<int>(ncols()) - 1,
            -static_cast<int>(nrows()) / 2 + static_cast<int>(nrows()) - 1
        };
    }

    void print() const;

    // Conversion from Array2 of foreign type using a conversion functor
    template <typename U>
    void import(const Array2<U>& src, std::function<T(const U&)> conversion);
    // Conversion from Array2 of foreign type without a conversion functor
    template <concept_arithmetic U>
    void import(const Array2<U>& src);

    // Static conversion from Array2 of foreign type with conversion functor
    template <typename U>
    static Array2<T> convert(const Array2<U>& src, std::function<T(const U&)> conversion);
    // Static conversion from Array2 of foreign arithmetic type without conversion functor
    template <concept_arithmetic U>
    static Array2<T> convert(const Array2<U>& src);

    //     friend Array2<T> operator+<>(const Array2<T> &x,const Array2<T> &y);

private:
    std::size_t stride() const { return m_xsize; }
    std::size_t m_xsize {};
    std::size_t m_ysize {};
};

// *************************************************
// Member definitions / implementation part
// *************************************************

template <typename T>
Array2<T>::Array2(const Array2<T>& src)
    : Array_base<T>(src)
    , m_xsize(src.m_xsize)
    , m_ysize(src.m_ysize)
{
}

template <typename T>
template <concept_arithmetic U>
Array2<T>::Array2(const Array2<U>& src)
    : Array_base<T>(src)
    , m_xsize(src.m_xsize)
    , m_ysize(src.m_ysize)
{
}

template <typename T>
Array2<T>::Array2(Array2<T>&& other) noexcept
    : Array_base<T>(std::move(other))
    , m_xsize(other.m_xsize)
    , m_ysize(other.m_ysize)
{
    other.m_xsize = 0;
    other.m_ysize = 0;
}

template <typename T>
Array2<T>& Array2<T>::operator=(Array2<T>&& other) noexcept
{
    if (this != &other) {
        Array_base<T>::operator=(std::move(other));
        m_xsize = other.m_xsize;
        m_ysize = other.m_ysize;
        other.m_xsize = 0;
        other.m_ysize = 0;
    }
    return *this;
}

template <typename T>
Array2<T>::Array2(std::size_t xsize, std::size_t ysize)
    : Array_base<T>(xsize * ysize)
    , m_xsize(xsize)
    , m_ysize(ysize)
{
}

template <typename T>
Array2<T>::Array2(std::size_t xsize, std::size_t ysize, const T& init)
    : Array_base<T>(xsize * ysize, init)
    , m_xsize(xsize)
    , m_ysize(ysize)
{
}

template <typename T>
Array2<T>::Array2(const extends& a_extends)
    : Array_base<T>(a_extends.product())
{
    assert(a_extends.size() == 2);
    m_xsize = a_extends[0];
    m_ysize = a_extends[1];
}

template <typename T>
Array2<T>::Array2(const extends& a_extends, const T& init)
    : Array_base<T>(a_extends.product(), init)
{
    assert(a_extends.size() == 2);
    m_xsize = a_extends[0];
    m_ysize = a_extends[1];
}

template <typename T>
Array2<T>::Array2(std::initializer_list<std::initializer_list<T>> l)
{
    const std::size_t rows { l.size() };
    if (rows == 0)
        return;
    const std::size_t cols { l.begin()->size() };
    assert(this->resize(cols * rows));
    auto memit = Array_base<T>::begin();
    for (auto row = l.begin(); row != l.end(); ++row) {
        assert(cols == row->size());
        std::copy(row->begin(), row->end(), memit);
        memit += cols;
    }
    m_xsize = cols;
    m_ysize = rows;
}

template <typename T>
const T* Array2<T>::operator[](int row) const
{
    std::size_t urow { (row < 0) ? m_xsize + row : row };
    if (urow >= nrows()) {
        throw std::out_of_range("Row index out of bounds");
    }
    return this->data().get() + urow * stride();
}

template <typename T>
T* Array2<T>::operator[](int row)
{
    std::size_t urow { (row < 0) ? m_xsize + row : row };
    if (urow >= nrows()) {
        throw std::out_of_range("Row index out of bounds");
    }
    return this->data().get() + urow * stride();
}

template <typename T>
T& Array2<T>::operator()(std::size_t col, std::size_t row)
{
    return this->data().get()[row * stride() + col];
}

template <typename T>
const T& Array2<T>::operator()(std::size_t col, std::size_t row) const
{
    return this->data().get()[row * stride() + col];
}

template <typename T>
const T& Array2<T>::at(s_indices indices) const
{
    //     std::cout<<"const T& Array2<T>::at(s_indices indices) const\n";
    if (indices[0] < 0)
        indices[0] += xsize();
    if (indices[1] < 0)
        indices[1] += ysize();
    std::size_t addr { static_cast<std::size_t>(indices[0]) + static_cast<std::size_t>(indices[1]) * stride() };
    if (addr >= this->size()) {
        std::cout << "indices: [" << indices[0] << "," << indices[1] << "]\n";
        assert(addr < this->size());
    }
    //     std::cout<<"indices="<<indices[0]<<" "<<indices[1]<<" addr="<<addr<<" v="<<Array_base<T>::operator[](addr)<<"\n";
    return Array_base<T>::operator[](addr);
}

template <typename T>
T& Array2<T>::at(s_indices indices)
{
    //     std::cout<<"T& Array2<T>::at(s_indices indices)\n";
    if (indices[0] < 0)
        indices[0] += xsize();
    if (indices[1] < 0)
        indices[1] += ysize();
    std::size_t addr { static_cast<std::size_t>(indices[0]) + static_cast<std::size_t>(indices[1]) * stride() };
    //     std::cout<<"indices="<<indices[0]<<" "<<indices[1]<<" addr="<<addr<<" v="<<Array_base<T>::operator[](addr)<<"\n";
    assert(addr < this->size());
    return Array_base<T>::operator[](addr);
}

template <typename T>
Array2<T>& Array2<T>::operator=(const Array2<T>& src)
{
    if (this == &src)
        return *this;
    m_xsize = src.m_xsize;
    m_ysize = src.m_ysize;
    if (this->size() != src.size()) {
        assert(this->resize(src.size()));
    }
    std::copy(src.begin(), src.end(), this->begin());
    return *this;
}

template <typename T>
template <concept_arithmetic U>
Array2<T>& Array2<T>::operator=(const Array2<U>& src)
{
    if (this == &src)
        return *this;
    m_xsize = src.m_xsize;
    m_ysize = src.m_ysize;
    if (this->size() != src.size()) {
        assert(this->resize(src.size()));
    }
    std::copy(src.begin(), src.end(), this->begin());
    return *this;
}

template <typename T>
Array2<T>& Array2<T>::operator=(const T& val)
{
    std::fill(this->begin(), this->end(), val);
    return *this;
}

template <typename T>
template <concept_arithmetic U>
Array2<T>& Array2<T>::operator=(const U& val)
{
    std::fill(this->begin(), this->end(), val);
    return *this;
}

template <typename T>
Array2<T>& Array2<T>::operator+=(const Array2<T>& x)
{
    assert(x.size() == this->size());
    std::transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        std::plus<T>());
    return *this;
}

template <typename T>
template <concept_arithmetic U>
Array2<T>& Array2<T>::operator+=(const Array2<U>& x)
{
    assert(x.size() == this->size());
    std::transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        [](const T& a, const U& b) {
            return a + static_cast<T>(b);
        });
    return *this;
}

template <typename T>
Array2<T>& Array2<T>::operator-=(const Array2<T>& x)
{
    assert(x.size() == this->size());
    std::transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        std::minus<T>());
    return *this;
}

template <typename T>
template <concept_arithmetic U>
Array2<T>& Array2<T>::operator-=(const Array2<U>& x)
{
    assert(x.size() == this->size());
    std::transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        [](const T& a, const U& b) {
            return a - static_cast<T>(b);
        });
    return *this;
}

template <typename T>
Array2<T>& Array2<T>::operator*=(const Array2<T>& x)
{
    assert(x.size() == this->size());
    std::transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        std::multiplies<T>());
    return *this;
}

template <typename T>
template <concept_arithmetic U>
Array2<T>& Array2<T>::operator*=(const Array2<U>& x)
{
    assert(x.size() == this->size());
    std::transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        [](const T& a, const U& b) {
            return a * static_cast<T>(b);
        });
    return *this;
}

template <typename T>
Array2<T>& Array2<T>::operator/=(const Array2<T>& x)
{
    assert(x.size() == this->size());
    std::transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        std::divides<T>());
    return *this;
}

template <typename T>
template <concept_arithmetic U>
Array2<T>& Array2<T>::operator/=(const Array2<U>& x)
{
    assert(x.size() == this->size());
    std::transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        [](const T& a, const U& b) {
            return a / static_cast<T>(b);
        });
    return *this;
}

template <typename T>
Array2<T>& Array2<T>::operator+=(const T& x)
{
    std::for_each(this->begin(), this->end(), [&x](T& a) { a += x; });
    return *this;
}

template <typename T>
template <concept_arithmetic U>
Array2<T>& Array2<T>::operator+=(const U& x)
{
    std::for_each(this->begin(), this->end(), [&x](T& a) { a += static_cast<T>(x); });
    return *this;
}

template <typename T>
Array2<T>& Array2<T>::operator-=(const T& x)
{
    std::for_each(this->begin(), this->end(), [&x](T& a) { a -= x; });
    return *this;
}

template <typename T>
template <concept_arithmetic U>
Array2<T>& Array2<T>::operator-=(const U& x)
{
    std::for_each(this->begin(), this->end(), [&x](T& a) { a -= static_cast<T>(x); });
    return *this;
}

template <typename T>
Array2<T>& Array2<T>::operator*=(const T& x)
{
    std::for_each(this->begin(), this->end(), [&x](T& a) { a *= x; });
    return *this;
}

template <typename T>
template <concept_arithmetic U>
Array2<T>& Array2<T>::operator*=(const U& x)
{
    std::for_each(this->begin(), this->end(), [&x](T& a) { a *= static_cast<T>(x); });
    return *this;
}

template <typename T>
Array2<T>& Array2<T>::operator/=(const T& x)
{
    std::for_each(this->begin(), this->end(), [&x](T& a) { a /= x; });
    return *this;
}

template <typename T>
template <concept_arithmetic U>
Array2<T>& Array2<T>::operator/=(const U& x)
{
    std::for_each(this->begin(), this->end(), [&x](T& a) { a /= static_cast<T>(x); });
    return *this;
}

// Unary plus operator (identity, returns the matrix as-is)
template <typename T>
Array2<T> Array2<T>::operator+() const
{
    return *this; // Simply return the matrix as-is
}

// Unary minus operator (negates all elements of the matrix)
template <typename T>
Array2<T> Array2<T>::operator-() const
{
    Array2<T> result(ncols(), nrows());
    auto result_it = result.begin();
    for (auto it = this->begin(); it != this->end(); ++it, ++result_it) {
        *result_it = -(*it);
    }
    return result;
}

template <typename T>
std::vector<T> Array2<T>::get_col(std::size_t col) const
{
    std::vector<T> vec(nrows());
    for (std::size_t i { 0 }; i < nrows(); ++i) {
        vec.at(i) = Array_base<T>::data()[col + m_xsize * i];
    }
    return vec;
}

template <typename T>
std::vector<T> Array2<T>::get_row(std::size_t row) const
{
    std::vector<T> vec(ncols());
    for (std::size_t i { 0 }; i < ncols(); ++i) {
        vec.at(i) = Array_base<T>::data().get()[m_xsize * row + i];
    }
    return vec;
}

template <typename T>
Array2<T> Array2<T>::get_subarray(const Rect<std::size_t>& rect)
{
    Array2<T> subarr(rect.width(), rect.height());
    assert(rect.bottomright.x < xsize());
    assert(rect.bottomright.y < ysize());
    for (std::size_t y { 0 }; y < rect.height(); y++) {
        for (std::size_t x { 0 }; x < rect.width(); x++) {
            subarr[y][x] = Array_base<T>::data()[(y + rect.topleft.y) * stride() + rect.topleft.x + x];
        }
    }
    return subarr;
}

template <typename T>
void Array2<T>::shift(const DimVector<int, 2>& distance)
{
    Array2<T> shifted_arr(std::move(this->shifted(distance)));
    std::swap(shifted_arr.data(), (*this).data());
}

template <typename T>
Array2<T> Array2<T>::shifted(const DimVector<int, 2>& distance)
{
    Array2<T> shifted_arr(m_xsize, m_ysize, T {});
    Point<int> startpos {
        std::clamp(distance[0], 0, static_cast<int>(m_xsize) - 1),
        std::clamp(distance[1], 0, static_cast<int>(m_ysize) - 1)
    };
    Point<int> endpos {
        std::clamp(distance[0] + static_cast<int>(m_xsize), 1, static_cast<int>(m_xsize)),
        std::clamp(distance[1] + static_cast<int>(m_ysize), 1, static_cast<int>(m_ysize))
    };

    for (int row { startpos.y }; row < endpos.y; ++row) {
        for (int col { startpos.x }; col < endpos.x; ++col) {
            shifted_arr(col, row) = (*this)(col - distance[0], row - distance[1]);
        }
    }
    return shifted_arr;
}

// non-static conversion
template <typename T>
template <concept_arithmetic U>
void Array2<T>::import(const Array2<U>& src)
{
    m_xsize = src.m_xsize;
    m_ysize = src.m_ysize;
    if (this->size() != src.size()) {
        assert(this->resize(src.size()));
    }
    std::transform(src.begin(), src.end(), this->begin(), []() {});
}

// non-static conversion
template <typename T>
template <typename U>
void Array2<T>::import(const Array2<U>& src, std::function<T(const U&)> conversion)
{
    m_xsize = src.xsize();
    m_ysize = src.ysize();
    if (this->size() != src.size()) {
        assert(this->resize(src.size()));
    }
    std::transform(src.begin(), src.end(), this->begin(), conversion);
}

// Static conversion from Array2 of foreign type with conversion functor
template <typename T>
template <typename U>
Array2<T> Array2<T>::convert(const Array2<U>& src, std::function<T(const U&)> conversion)
{
    Array2<T> arr {};
    arr.import(src, conversion);
    return arr;
}
// Static conversion from Array2 of foreign arithmetic type without conversion functor
template <typename T>
template <concept_arithmetic U>
Array2<T> Array2<T>::convert(const Array2<U>& src)
{
    return std::move(Array2<T>(src));
}

template <typename T>
void Array2<T>::print() const
{
    std::cout << "object: Array2" << std::endl;
    std::cout << "field address: " << this << std::endl;
    std::cout << "data address: " << this->data().get() << std::endl;
    std::cout << "datatype: " << typeid(T).name() << std::endl;
    std::cout << "size of datatype: " << sizeof(T) << " bytes" << std::endl;
    std::cout << "nr. of elements: " << this->size() << std::endl;
    std::cout << "extends: x=" << xsize() << ", y=" << ysize() << std::endl;
    std::cout << "strides: x=" << 1 << ", y=" << stride() << std::endl;
    std::cout << "min indices: x=" << min_sindices()[0] << ", y=" << min_sindices()[1] << std::endl;
    std::cout << "max indices: x=" << max_sindices()[0] << ", y=" << max_sindices()[1] << std::endl;
    std::cout << "size: " << this->size() << std::endl;
    std::cout << "size of array: " << sizeof(T) * this->size() << " bytes" << std::endl;
}

// *************************************************
// non-member definitions
// *************************************************

template <typename T>
Array2<T> operator+(const Array2<T>& x, const Array2<T>& y)
{
    Array2<T> z { x };
    z += y;
    return std::move(z);
}

template <typename T>
Array2<T> operator-(const Array2<T>& x, const Array2<T>& y)
{
    Array2<T> z { x };
    z -= y;
    return std::move(z);
}

template <typename T>
Array2<T> operator*(const Array2<T>& x, const Array2<T>& y)
{
    Array2<T> z { x };
    z *= y;
    return std::move(z);
}

template <typename T>
Array2<T> operator/(const Array2<T>& x, const Array2<T>& y)
{
    Array2<T> z { x };
    z /= y;
    return std::move(z);
}

} // namespace smip

template <typename T>
std::ostream& operator<<(std::ostream& o, const std::vector<T>& v)
{
    o << "(";
    std::copy(v.begin(), v.end(), std::ostream_iterator<T>(o, " "));
    o << "\b)";
    return o;
}

template <typename T>
std::ostream& operator<<(std::ostream& o, smip::Array2<T>& v)
{
    for (std::size_t i = 0; i < v.nrows(); i++) {
        o << v.get_row(i) << std::endl;
    }
    return o;
}
