#pragma once

#include <deque>
#include <cassert>
#include <cstdarg>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <valarray>

namespace smip {

/*
 *! class DimVector
 */
template <concept_integral T, std::size_t NrDims>
class DimVector : public std::valarray<T> {
public:
    DimVector()
        : std::valarray<T>(NrDims, T {})
    {}
    DimVector(std::initializer_list<T> l);

    [[nodiscard]] DimVector<T, NrDims + 1> operator,(T in);
    [[nodiscard]] DimVector<T, NrDims + 1> appended_back(T new_dimsize);
    [[nodiscard]] DimVector<T, NrDims + 1> appended_front(T new_dimsize);
    [[nodiscard]] DimVector<T, NrDims - 1> removed_back();
    [[nodiscard]] DimVector<T, NrDims - 1> removed_front();
    
    void fill(T value);
    inline T product() const { return std::accumulate(std::begin(*this), std::end(*this), T{1}, std::multiplies<T>()); }
};

/*
 *! class DimVector
 */
template <concept_integral T, std::size_t NrDims>
DimVector<T, NrDims>::DimVector(std::initializer_list<T> l)
    : std::valarray<T>(l)
{
     assert(l.size() == NrDims);
}

template <concept_integral T, std::size_t NrDims>
DimVector<T, NrDims + 1> DimVector<T, NrDims>::appended_back(T new_dimsize)
{
    DimVector<T, NrDims + 1> newvec {};
    newvec[std::slice(0, NrDims, 1)] = *this;
    newvec[NrDims] = new_dimsize;
    return newvec;
}

template <concept_integral T, std::size_t NrDims>
DimVector<T, NrDims + 1> DimVector<T, NrDims>::appended_front(T new_dimsize)
{
    DimVector<T, NrDims + 1> newvec {};
    newvec[std::slice(1, NrDims, 1)] = *this;
    newvec[0] = new_dimsize;
    return newvec;
}

template <concept_integral T, std::size_t NrDims>
DimVector<T, NrDims - 1> DimVector<T, NrDims>::removed_back()
{
    DimVector<T, NrDims - 1> newvec {};
    newvec[std::slice(0, NrDims-1, 1)] = *this;
    return newvec;
}

template <concept_integral T, std::size_t NrDims>
DimVector<T, NrDims - 1> DimVector<T, NrDims>::removed_front()
{
    DimVector<T, NrDims - 1> newvec {};
    newvec[std::slice(1, NrDims-1, 1)] = *this;
    return newvec;
}

template <concept_integral T, std::size_t NrDims>
void DimVector<T, NrDims>::fill(T value)
{
    *this = value;
}


/*
 *! class DimVector_old
 */
template <typename T, std::size_t NrDims>
class DimVector_old : public std::deque<T> {
public:
    DimVector_old()
        : std::deque<T>(NrDims, T {})
    {
    }
    DimVector_old(T i0, ...);
    template <std::size_t otherDim>
    DimVector_old(const DimVector_old<T, otherDim>& x);
    ~DimVector_old() { }

    DimVector_old& operator()(T i0, ...);
    DimVector_old<T, 1>& operator=(T i0);
    DimVector_old<T, NrDims + 1>& operator,(T in);

    void fill(T value);
    inline T sum() const { return std::accumulate(this->begin(), this->end(), T {}); }
    inline T product() const { return std::accumulate(this->begin(), this->end(), T{1}, std::multiplies<T>()); }
};


// *************************************************
// Member definitions / implementation part
// *************************************************



/*
 * class DimVector_old
 */
template <typename T, std::size_t NrDims>
DimVector_old<T, NrDims>::DimVector_old(T i0, ...)
    : std::deque<T>()
{
    va_list ap;
    va_start(ap, i0);
    this->push_back(i0);
    int in;
    //   std::cout<<"i0="<<i0<<std::endl;
    for (std::size_t i = 1; i < NrDims; ++i) {
        in = va_arg(ap, int);
        //      std::cout<<"i"<<i<<"="<<in<<std::endl;
        this->push_back(in);
    }
    va_end(ap);
}

template <typename T, std::size_t NrDims>
template <std::size_t otherDim>
DimVector_old<T, NrDims>::DimVector_old(const DimVector_old<T, otherDim>& x)
    : std::deque<T>(NrDims, T {})
{
    std::copy(x.begin(), x.begin() + std::min(x.size(), this->size()), this->begin());
}

template <typename T, std::size_t NrDims>
DimVector_old<T, NrDims>& DimVector_old<T, NrDims>::operator()(T i0, ...)
{
    this->clear();
    va_list ap;
    va_start(ap, i0);
    this->push_back(i0);
    int in;
    //   std::cout<<"i0="<<i0<<std::endl;
    for (int i = 1; i < NrDims; ++i) {
        in = va_arg(ap, const int);
        //      std::cout<<"i"<<i<<"="<<in<<std::endl;
        this->push_back(in);
    }
    va_end(ap);
    return *this;
}

template <typename T, std::size_t NrDims>
DimVector_old<T, 1>& DimVector_old<T, NrDims>::operator=(T i0)
{
    DimVector_old<T, 1>* _x = (DimVector_old<T, 1>*)(this);
    _x->clear();
    _x->push_back(i0);
    //      std::cout<<"added index0="<<(*this)[0]<<std::endl;
    return *_x;
}

template <typename T, std::size_t NrDims>
DimVector_old<T, NrDims + 1>&DimVector_old<T, NrDims>::operator,(T in)
{
    DimVector_old<T, NrDims + 1>* _x = (DimVector_old<T, NrDims + 1>*)(this);
    _x->push_back(in);
    //      std::cout<<"added index"<<this->size()-1<<"="<<(*this)[this->size()-1]<<std::endl;
    return *_x;
}

template <typename T, std::size_t NrDims>
void DimVector_old<T, NrDims>::fill(T value)
{
    this->assign(NrDims, value);
}

// *************************************************
// non-member definitions
// *************************************************

template <concept_integral T, std::size_t NrDims>
std::ostream& operator<<(std::ostream& os, const DimVector<T, NrDims>& obj)
{
    // write obj to stream
    os << "[ ";
    for ( auto dim : obj ) {
        os << dim << " ";
    }
    os << "]";
    return os;
}

template <typename T, std::size_t NrDims>
std::ostream& operator<<(std::ostream& os, const DimVector_old<T, NrDims>& obj)
{
    // write obj to stream
    os << "[ ";
    for ( auto dim : obj ) {
        os << dim << " ";
    }
    os << "]";
    return os;
}

} // namespace smip
