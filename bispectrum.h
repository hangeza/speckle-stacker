#pragma once

#include <array>
#include <cassert>
#include <complex>
#include <cstdlib>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <valarray>
#include <vector>

#include "array2.h"

namespace smip {

//! 4-dim Container for handling a complex Bispectrum
/*! ...
 */
template <typename T>
class Bispectrum : public Array_base<T> {
public:
    typedef DimVector<std::size_t, 4> extents;
    typedef DimVector<int, 4> s_indices;
    typedef DimVector<std::size_t, 4> u_indices;

    struct ElementOutOfBounds : std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    Bispectrum();
    /*! Creates Bispectrum with sizes [<i>i,j,k,l</i>] */
    Bispectrum(const extents& dimsizes);
    Bispectrum(const Bispectrum& other);
    //Bispectrum(Bispectrum&& other);

    /*! default destructor */
    ~Bispectrum() { }

    /*! Prints many information about the actual instance to stdout
    */
    void print();
    /*! Write data to binary file <i>filename</i> */
    void write_to_file(const std::string& filename) const;
    /*! Read data from binary file <i>filename</i> \n
        adjusts array sizes and allocates memory if necessary
    */
    void read_from_file(const std::string& filename);
    /*! returns element with indices [<i>i,j,k,l</i>] */
    T get_element(s_indices indices) const;
    /*! write element \e value to position with indices [<i>i,j,k,l</i>] */
    void put_element(s_indices indices, const T& value);
    /*! overloaded = operator */
    Bispectrum& operator=(const Bispectrum& x);
    /*! overloaded += operator */
    Bispectrum& operator+=(const Bispectrum& x);
    /*! overloaded -= operator */
    Bispectrum& operator-=(const Bispectrum& x);
    /*! overloaded *= operator */
    Bispectrum& operator*=(const Bispectrum& x);
    /*! overloaded /= operator */
    Bispectrum& operator/=(const Bispectrum& x);
    /*! overloaded += operator */
    Bispectrum& operator+=(const T& c);
    /*! overloaded -= operator */
    Bispectrum& operator-=(const T& c);
    /*! overloaded *= operator */
    Bispectrum& operator*=(const T& c);
    /*! overloaded /= operator */
    Bispectrum& operator/=(const T& c);
    /*! calculate indices [<i>i,j,k,l</i>] to given address offset \e addr 
    */
    s_indices calc_indices(std::size_t addr) const;
    template <typename U>
    void accumulate_from_fft(const Array2<U>& fft);

public:
    std::size_t size() const { return base_size(); };
    extents sizes() const;
    extents base_sizes() const;
    std::size_t base_size() const { return { base_sizes()[0] * base_sizes()[1] * base_sizes()[2] * base_sizes()[3] }; }
    std::size_t totalsize() const { return { sizes().product() }; }
    s_indices min_indices() const;
    s_indices max_indices() const;

    /*! returns address offset of element with indices [<i>i,j,k,l</i>] */
    std::size_t calc_offset(s_indices indices) const;

private:
    extents m_dimsizes { 0, 0, 0, 0 };
};

// *************************************************
// Member definitions / implementation part
// *************************************************

template <typename T>
Bispectrum<T>::Bispectrum()
    : Array_base<T> {}
{
}

template <typename T>
Bispectrum<T>::Bispectrum(const Bispectrum<T>::extents& dimsizes)
    : m_dimsizes { dimsizes }
{
    this->resize(base_size());
    std::fill(Array_base<T>::begin(), Array_base<T>::end(), T {});
}

template <typename T>
Bispectrum<T>::Bispectrum(const Bispectrum<T>& other)
    : Array_base<T>(other.base_size())
    , m_dimsizes { other.m_dimsizes }
{
    std::copy(other.begin(), other.end(), Array_base<T>::begin());
}

template <typename T>
typename Bispectrum<T>::extents Bispectrum<T>::sizes() const
{
    // true sizes of ux,uy,vx,vy dimensions
    extents vec { m_dimsizes / 2 };
    vec *= 2;
    vec += 1UL;
    return vec;
}

template <typename T>
typename Bispectrum<T>::extents Bispectrum<T>::base_sizes() const
{
    // reduced sizes of ux,uy,vx,vy dimensions
    extents vec = { this->sizes() };
    vec -= sizes() * extents { 1, 0, 1, 0 } / 2;
    return vec;
}

template <typename T>
Bispectrum<T>& Bispectrum<T>::operator=(const Bispectrum<T>& x)
{
    m_dimsizes = x.m_dimsizes;
    this->resize(base_size());
    std::copy(x.begin(), x.end(), Array_base<T>::begin());
    return *this;
}

template <typename T>
Bispectrum<T>& Bispectrum<T>::operator+=(const Bispectrum<T>& x)
{
    assert(x.NrElements() == this->NrElements());
    std::transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        std::plus<T>());
    return *this;
}

template <typename T>
Bispectrum<T>& Bispectrum<T>::operator-=(const Bispectrum<T>& x)
{
    assert(x.NrElements() == this->NrElements());
    std::transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        std::minus<T>());
    return *this;
}

template <typename T>
Bispectrum<T>& Bispectrum<T>::operator*=(const Bispectrum<T>& x)
{
    assert(x.NrElements() == this->NrElements());
    std::transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        std::multiplies<T>());
    return *this;
}

template <typename T>
Bispectrum<T>& Bispectrum<T>::operator/=(const Bispectrum<T>& x)
{
    assert(x.NrElements() == this->NrElements());
    std::transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        std::divides<T>());
    return *this;
}

template <typename T>
Bispectrum<T>& Bispectrum<T>::operator+=(const T& c)
{
    for (auto it { Array_base<T>::begin() }; it != Array_base<T>::end(); ++it) {
        *it += c;
    }
    return *this;
}

template <typename T>
Bispectrum<T>& Bispectrum<T>::operator-=(const T& c)
{
    for (auto it { Array_base<T>::begin() }; it != Array_base<T>::end(); ++it) {
        *it -= c;
    }
    return *this;
}

template <typename T>
Bispectrum<T>& Bispectrum<T>::operator*=(const T& c)
{
    for (auto it { Array_base<T>::begin() }; it != Array_base<T>::end(); ++it) {
        *it *= c;
    }
    return *this;
}

template <typename T>
Bispectrum<T>& Bispectrum<T>::operator/=(const T& c)
{
    for (auto it { Array_base<T>::begin() }; it != Array_base<T>::end(); ++it) {
        *it /= c;
    }
    return *this;
}

template <typename T>
typename Bispectrum<T>::s_indices Bispectrum<T>::calc_indices(std::size_t addr) const
{
    assert(addr < base_size());
    std::size_t temp { base_size() / base_sizes()[0] };
    std::size_t rest { addr };
    s_indices indices { 0, 0, 0, 0 };
    indices[0] = -static_cast<int>(rest / temp);
    temp *= -indices[0];
    rest = rest - temp;
    temp = base_sizes()[2] * base_sizes()[3];
    indices[1] = static_cast<int>(rest / temp);
    temp *= indices[1];
    rest = rest - temp;
    temp = base_sizes()[3];
    indices[2] = -static_cast<int>(rest / temp);
    temp *= -indices[2];
    rest = rest - temp;
    indices[3] = rest;
    return indices;
}

template <typename T>
std::size_t Bispectrum<T>::calc_offset(s_indices indices) const
{
    indices *= { -1, 1, -1, 1 };

    indices += {
        (indices[0] < 0) ? static_cast<int>(sizes()[0]) : 0,
        (indices[1] < 0) ? static_cast<int>(sizes()[1]) : 0,
        (indices[2] < 0) ? static_cast<int>(sizes()[2]) : 0,
        (indices[3] < 0) ? static_cast<int>(sizes()[3]) : 0
    };
    // ToDo: hier noch Abfrage, ob [i,j,k,l] wirklich sinnvoll
    std::size_t addr { static_cast<std::size_t>(indices[0]) };

    addr *= base_sizes()[1];
    addr += indices[1];
    addr *= base_sizes()[2];
    addr += indices[2];
    addr *= base_sizes()[3];
    addr += indices[3];
    //std::cout<<"Indices="<<ii<<";"<<jj<<";"<<kk<<";"<<ll<<endl;
    return addr;
}

#ifdef __GNUC__
#ifndef __clang__
#pragma GCC optimize("unroll-loops")
#endif
#endif
template <typename T>
template <typename U>
void Bispectrum<T>::accumulate_from_fft(const Array2<U>& fft)
{
    const int min1 = std::max(fft.min_sindices()[0], min_indices()[0]);
    const int min2 = std::max(fft.min_sindices()[1], min_indices()[1]);
    const int min3 = std::max(fft.min_sindices()[0], min_indices()[2]);
    const int min4 = std::max(fft.min_sindices()[1], min_indices()[3]);
    const int max1 = std::min(fft.max_sindices()[0], max_indices()[0]);
    const int max2 = std::min(fft.max_sindices()[1], max_indices()[1]);
    // max3 is not used due to hermitian symmetry
    //   int max3 = std::min(fft.xindex_hi(),index3_hi());
    const int max4 = std::min(fft.max_sindices()[1], max_indices()[3]);

    for (int i = min1; i <= 0; i++) {
        for (int j = min2; j <= max2; j++) {
            for (int k = min3; k <= 0; k++) {
                for (int l = min4; l <= max4; l++) {
                    if (((i + k) >= min1) && ((j + l) >= min2)
                        && ((i + k) <= max1) && ((j + l) <= max2)) {
                        T t { fft.at({ i, j }) };
                        t *= fft.at({ k, l });
                        t *= std::conj(fft.at({ i + k, j + l }));
                        //cout<<"Indices="<<i<<";"<<j<<";"<<k<<";"<<l<<endl;
                        Array_base<T>::data().get()[calc_offset({ i, j, k, l })] += t;
                    }
                }
            }
        }
    }
}

template <typename T>
void Bispectrum<T>::write_to_file(const std::string& filename) const
{
    FILE* stream;

    errno = 0;
    stream = fopen(filename.c_str(), "wb");
    if (stream == NULL) {
        fprintf(stderr, "Couldn't write file %s; %s\n",
            filename.c_str(), strerror(errno));
        exit(EXIT_FAILURE);
        return;
    }
    std::size_t size { base_size() };
    //dimvector<T,4> sizes { base_size() };

    fwrite(&size, sizeof(size), 1, stream);
    fwrite(&m_dimsizes[0], sizeof(m_dimsizes[0]), 1, stream);
    fwrite(&m_dimsizes[1], sizeof(m_dimsizes[1]), 1, stream);
    fwrite(&m_dimsizes[2], sizeof(m_dimsizes[2]), 1, stream);
    fwrite(&m_dimsizes[3], sizeof(m_dimsizes[4]), 1, stream);
    fwrite(Array_base<T>::data().get(), sizeof(T), base_size(), stream);
    fclose(stream);
}

template <typename T>
void Bispectrum<T>::read_from_file(const std::string& filename)
{
    FILE* stream;

    errno = 0;
    stream = fopen(filename.c_str(), "rb");
    if (stream == NULL) {
        fprintf(stderr, "Couldn't open file %s; %s\n",
            filename.c_str(), strerror(errno));
        exit(EXIT_FAILURE);
        return;
    }
    std::size_t size {};
    extents dims {};

    fread(&size, sizeof(size), 1, stream);
    fread(&dims[0], sizeof(dims[0]), 1, stream);
    fread(&dims[1], sizeof(dims[1]), 1, stream);
    fread(&dims[2], sizeof(dims[2]), 1, stream);
    fread(&dims[3], sizeof(dims[3]), 1, stream);
    if (size != dims[0] * dims[1] * dims[2] * dims[3]) {
        std::cerr << "Bispectrum<T>::read_from_file(const std::string&): error reading metadata from file " << filename << std::endl;
        fclose(stream);
        return;
    }
    assert(Array_base<T>::resize(size) == base_size());
    m_dimsizes = dims;
    if (fread(Array_base<T>::data(), sizeof(T), size, stream) != size) {
        std::cerr << "Bispectrum<T>::read_from_file(const std::string&): error reading data chunck from file " << filename << std::endl;
        fclose(stream);
        return;
    }
    fclose(stream);
}

template <typename T>
void Bispectrum<T>::print()
{
    std::cout << "Object: Bispectrum" << std::endl;
    std::cout << "Address: " << this << std::endl;
    std::cout << "datatype: " << typeid(std::complex<T>).name() << std::endl;
    std::cout << "size: " << totalsize() << std::endl;
    std::cout << "sizes: " << sizes() << std::endl;
    std::cout << "base_sizes: " << base_sizes() << std::endl;
    std::cout << "min indices: " << min_indices() << std::endl;
    std::cout << "max indices: " << max_indices() << std::endl;
    std::cout << "size of datatype: " << sizeof(T) << " bytes" << std::endl;
    std::cout << "array size: " << sizeof(T) * totalsize() << " bytes" << std::endl;
    std::cout << "real memory size: " << sizeof(T) * base_size() / 1024 / 1024 << " MB" << std::endl;
    std::cout << "reduction: " << (double)base_size() * (double)sizeof(T) / (double)(totalsize() * sizeof(std::complex<double>));
    std::cout << " (vs. full size * 16 byte cmplx double)" << std::endl;
}

template <typename T>
T Bispectrum<T>::get_element(s_indices indices) const
{
    // wenn k,l ausserhalb, dann tausche i,j und k,l
    if ((std::abs(indices[2]) > max_indices()[2]) || (std::abs(indices[3]) > max_indices()[3])) {
        std::swap(indices[0], indices[2]);
        std::swap(indices[1], indices[3]);
    }
    // wenn jetzt noch ausserhalb, dann Abbruch
    if ((std::abs(indices[2]) > max_indices()[2]) || (std::abs(indices[3]) > max_indices()[3])) {
        throw ElementOutOfBounds("trying to access bispectrum element out of range");
    }
    bool conjug { false };

    s_indices uv { 0, 0, 0, 0 }; // ux,uy,vx,vy

    if ((indices[0] <= 0) && (indices[2] <= 0)) {
        // T1
        uv = indices;
    } else if ((indices[0] > 0) && (indices[2] > 0)) {
        // T7
        uv = -indices;
        conjug = true;
    } else if ((indices[0] > 0) && (indices[2] <= 0)) {
        uv = indices * s_indices { -1, -1, 1, 1 };
        if (indices[0] + indices[2] > 0) {
            // T6
            uv -= s_indices { indices[2], indices[3], 0, 0 };
        } else {
            // T9
            uv += s_indices { 0, 0, indices[0], indices[1] };
            conjug = true;
        }
    } else if ((indices[0] <= 0) && (indices[2] > 0)) {
        uv = indices * s_indices { 1, 1, -1, -1 };
        if (indices[0] + indices[2] > 0) {
            // T3
            uv -= s_indices { 0, 0, indices[0], indices[1] };
        } else {
            // T12
            uv += s_indices { indices[2], indices[3], 0, 0 };
            conjug = true;
        }
    } else {
        /*
        std::cerr<<"Bispectrum::GetElement(int,int,int,int):"<<std::endl;
        std::cerr<<"ups - ein Element ist durchgerutscht : ["
            <<uv[0]<<","<<uv[1]<<","<<uv[2]<<","<<uv[3]<<"]"<<std::endl;
    */
        throw ElementOutOfBounds("trying to access bispectrum element out of range - unaccounted index");
    }

    // wenn v ausserhalb, dann tausche u und v
    if ((std::abs(uv[2]) > max_indices()[2]) || (std::abs(uv[3]) > max_indices()[3])) {
        std::swap(uv[0], uv[2]);
        std::swap(uv[1], uv[3]);
    }

    std::size_t addr { calc_offset(uv) };

    if (addr >= base_size()) {
        /*
        std::cerr<<"error: trying to access element out of range"<<std::endl;
        std::cerr<<"Bispectrum::GetElement(int,int,int,int)m2"<<std::endl;
        std::cerr<<"addr="<<addr<<" basesize="<<base_size()<<"  Indices="<<uv[0]<<";"<<uv[1]<<";"<<uv[2]<<";"<<uv[3]<<std::endl;
        */
        throw ElementOutOfBounds("trying to access bispectrum element out of range");
    }
    if (conjug)
        return std::conj(Array_base<T>::data().get()[addr]);
    return Array_base<T>::data().get()[addr];
}

template <typename T>
void Bispectrum<T>::put_element(s_indices indices, const T& value)
{
    std::size_t addr { calc_offset(indices) };
    if (addr >= base_size()) {
        /*
        cerr<<"error: trying to access element out of range"<<endl;
        cerr<<"Bispectrum::PutElement(int,int,int,int)"<<endl;
        cerr<<"size="<<_size<<"  Indices="<<i<<";"<<j<<";"<<k<<";"<<l<<endl;
        */
        throw ElementOutOfBounds("trying to access bispectrum element out of range");
    }
    Array_base<T>::data().get()[addr] = value;
}

template <typename T>
typename Bispectrum<T>::s_indices Bispectrum<T>::min_indices() const
{
    s_indices mins {};
    //     std::cout << "s_indices Bispectrum<T>::min_indices() const : \n";
    //     std::cout << " mins = " << mins << " (size="<< mins.size()<<"\n";
    extents sizes = this->sizes();
    //     std::cout << " sizes = " << sizes << " (size="<< sizes.size()<<"\n";
    // Use std::transform to convert from unsigned long to int
    std::transform(std::begin(sizes), std::end(sizes), std::begin(mins),
        [](extents::value_type x) { return -static_cast<s_indices::value_type>(x) / 2; });
    return mins;
}

template <typename T>
typename Bispectrum<T>::s_indices Bispectrum<T>::max_indices() const
{
    s_indices maxes {};
    extents sizes = this->sizes();
    // following cast operation is equivalent to
    // maxes = sizes
    std::transform(std::begin(sizes), std::end(sizes), std::begin(maxes),
        [](extents::value_type x) { return static_cast<s_indices::value_type>(x); });
    maxes += min_indices();
    maxes -= 1;
    return maxes;
}

} // namespace smip
