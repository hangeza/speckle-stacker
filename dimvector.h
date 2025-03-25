#pragma once

#include <deque>
#include <cassert>
#include <cstdarg>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>


/*
 *! class DimVector
 */
template <typename T, std::size_t NrDims>
class DimVector : public std::deque<T> {
public:
    DimVector()
        : std::deque<T>(NrDims, T {})
    {
    }
    DimVector(T i0, ...);
    template <std::size_t otherDim>
    DimVector(const DimVector<T, otherDim>& x);
    ~DimVector() { }

    DimVector& operator()(T i0, ...);
    DimVector<T, 1>& operator=(T i0);
    DimVector<T, NrDims + 1>& operator,(T in);

    void fill(T value);
    inline T sum() const { return std::accumulate(this->begin(), this->end(), T {}); }
    inline T product() const { return std::accumulate(this->begin(), this->end(), T{1}, std::multiplies<T>()); }
};


// *************************************************
// Member definitions / implementation part
// *************************************************



/*
 * class DimVector
 */
template <typename T, std::size_t NrDims>
DimVector<T, NrDims>::DimVector(T i0, ...)
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
DimVector<T, NrDims>::DimVector(const DimVector<T, otherDim>& x)
    : std::deque<T>(NrDims, T {})
{
    std::copy(x.begin(), x.begin() + std::min(x.size(), this->size()), this->begin());
}

template <typename T, std::size_t NrDims>
DimVector<T, NrDims>& DimVector<T, NrDims>::operator()(T i0, ...)
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
DimVector<T, 1>& DimVector<T, NrDims>::operator=(T i0)
{
    DimVector<T, 1>* _x = (DimVector<T, 1>*)(this);
    _x->clear();
    _x->push_back(i0);
    //      std::cout<<"added index0="<<(*this)[0]<<std::endl;
    return *_x;
}

template <typename T, std::size_t NrDims>
DimVector<T, NrDims + 1>&DimVector<T, NrDims>::operator,(T in)
{
    DimVector<T, NrDims + 1>* _x = (DimVector<T, NrDims + 1>*)(this);
    _x->push_back(in);
    //      std::cout<<"added index"<<this->size()-1<<"="<<(*this)[this->size()-1]<<std::endl;
    return *_x;
}

template <typename T, std::size_t NrDims>
void DimVector<T, NrDims>::fill(T value)
{
    this->assign(NrDims, value);
}
