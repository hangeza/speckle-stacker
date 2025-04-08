#pragma once

#include <cassert>
#include <cstdarg>
#include <deque>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <valarray>

namespace smip {

/**
 * @brief DimVector class for fixed size array storage of arithmetic values.
 * @tparam T value type
 * @tparam NrDims fixed size of array
 * The DimVector class is a fixed size array derived from std::valarray.
 * The type of the elements is constrained to be of arithmetic nature.
 */
template <concept_arithmetic T, std::size_t NrDims>
class DimVector : public std::valarray<T> {
public:
    using typename std::valarray<T>::value_type;
    using std::valarray<T>::valarray;
    using std::valarray<T>::operator=;
    using std::valarray<T>::operator+;
    using std::valarray<T>::operator-;
    using std::valarray<T>::operator+=;
    using std::valarray<T>::operator-=;
    using std::valarray<T>::operator*=;
    using std::valarray<T>::operator/=;
    using std::valarray<T>::operator[];
    using std::valarray<T>::size;
    using std::valarray<T>::sum;

    DimVector();
    DimVector(const DimVector<T, NrDims>& x);
    DimVector(std::initializer_list<T> l);

    DimVector& operator=(const DimVector<T, NrDims>& x);

    [[nodiscard]] DimVector<T, NrDims + 1> operator,(T in);
    [[nodiscard]] DimVector<T, NrDims + 1> appended_back(T new_dimsize);
    [[nodiscard]] DimVector<T, NrDims + 1> appended_front(T new_dimsize);
    [[nodiscard]] DimVector<T, NrDims - 1> removed_back();
    [[nodiscard]] DimVector<T, NrDims - 1> removed_front();

    /**
    * @brief fill all elemts of object with value
    * @param value value of type T to be assigned to all elements
    */
    void fill(T value);
    /**
    * @brief calculate product of all array elements
    * @return product of all array elements
    */
    inline T product() const { return std::accumulate(std::begin(*this), std::end(*this), T { 1 }, std::multiplies<T>()); }
};

//********************
// implementation part
//********************
template <concept_arithmetic T, std::size_t NrDims>
DimVector<T, NrDims>::DimVector()
    : std::valarray<T>(NrDims)
{
}

template <concept_arithmetic T, std::size_t NrDims>
DimVector<T, NrDims>::DimVector(const DimVector<T, NrDims>& x)
    : std::valarray<T>(NrDims)
{
    if (x.size() != NrDims && x.size() != 0) {
        throw std::domain_error("invalid initializer size");
    }
    if (x.size() > 0) {
        std::copy(std::begin(x), std::end(x), std::begin(*this));
    }
}

template <concept_arithmetic T, std::size_t NrDims>
DimVector<T, NrDims>::DimVector(std::initializer_list<T> l)
    : std::valarray<T>(NrDims)
{
    if (l.size() == NrDims) {
        std::copy(l.begin(), l.end(), std::begin(*this));
    }
    //     assert(l.size() == NrDims);
}

template <concept_arithmetic T, std::size_t NrDims>
DimVector<T, NrDims>& DimVector<T, NrDims>::operator=(const DimVector<T, NrDims>& x)
{
    if (x.size() != NrDims) {
        throw std::domain_error("invalid initializer size");
    }
    std::copy(std::begin(x), std::end(x), std::begin(*this));
    return *this;
}

template <concept_arithmetic T, std::size_t NrDims>
DimVector<T, NrDims + 1> DimVector<T, NrDims>::appended_back(T new_dimsize)
{
    DimVector<T, NrDims + 1> newvec {};
    newvec[std::slice(0, NrDims, 1)] = *this;
    newvec[NrDims] = new_dimsize;
    return newvec;
}

template <concept_arithmetic T, std::size_t NrDims>
DimVector<T, NrDims + 1> DimVector<T, NrDims>::appended_front(T new_dimsize)
{
    DimVector<T, NrDims + 1> newvec {};
    newvec[std::slice(1, NrDims, 1)] = *this;
    newvec[0] = new_dimsize;
    return newvec;
}

template <concept_arithmetic T, std::size_t NrDims>
DimVector<T, NrDims - 1> DimVector<T, NrDims>::removed_back()
{
    DimVector<T, NrDims - 1> newvec {};
    newvec[std::slice(0, NrDims - 1, 1)] = *this;
    return newvec;
}

template <concept_arithmetic T, std::size_t NrDims>
DimVector<T, NrDims - 1> DimVector<T, NrDims>::removed_front()
{
    DimVector<T, NrDims - 1> newvec {};
    newvec[std::slice(1, NrDims - 1, 1)] = *this;
    return newvec;
}

template <concept_arithmetic T, std::size_t NrDims>
void DimVector<T, NrDims>::fill(T value)
{
    *this = value;
}

// *************************************************
// non-member definitions
// *************************************************

template <concept_arithmetic T, std::size_t NrDims>
std::ostream& operator<<(std::ostream& os, const DimVector<T, NrDims>& obj)
{
    // write obj to stream
    os << "[ ";
    for (auto dim : obj) {
        os << dim << " ";
    }
    os << "]";
    return os;
}

} // namespace smip
