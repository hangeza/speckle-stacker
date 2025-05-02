#pragma once

#include <array>
#include <cassert>
#include <complex>
#include <cstdlib>
#include <cstddef>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <valarray>
#include <vector>
#include <optional>
#include <algorithm>

#include "array2.h"
#include "types.h"
#include "utility.h"

namespace smip {

//! 4-dim Container for handling a complex Bispectrum
/*! ...
 */
template <typename T>
class Bispectrum : public Array_base<T> {
public:
    using extents = DimVector<std::size_t, 4>;
    using s_indices = DimVector<int, 4>;
    using u_indices = DimVector<std::size_t, 4>;
    using Array_base<T>::operator+=;
    using Array_base<T>::operator-=;
    using Array_base<T>::operator*=;
    using Array_base<T>::operator/=;

    struct ElementOutOfBounds : std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    Bispectrum();
    /*! Creates Bispectrum with sizes [<i>i,j,k,l</i>] */
    Bispectrum(const extents& dimsizes);
    Bispectrum(const Bispectrum& other);
    Bispectrum(Bispectrum&& other) noexcept = default;
    Bispectrum& operator=(Bispectrum&& other) noexcept = default;

    ~Bispectrum() = default;

    /*! Prints many information about the actual instance to stdout
    */
    void print() const;
    /*! Write data to binary file <i>filename</i> */
    void write_to_file(const std::string& filename) const;
    /*! Read data from binary file <i>filename</i> \n
        adjusts array sizes and allocates memory if necessary
    */
    void read_from_file(const std::string& filename);
    /*! returns element with indices [<i>i,j,k,l</i>] */
    [[nodiscard]] T get_element(s_indices indices) const;
    T operator()(s_indices indices) const { return get_element(indices); }
    //T& operator()(s_indices indices);
    void get_elements(const std::vector<s_indices>& idx_list, std::vector<T>& results) const;
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
    /*! calculate indices [<i>i,j,k,l</i>] to given address offset \e addr 
    */
    [[nodiscard]] s_indices calc_indices(std::size_t addr) const;
    [[nodiscard]] std::size_t calc_offset(s_indices indices) const noexcept;
    template <typename U>
    void accumulate_from_fft(const Array2<U>& fft);

    [[nodiscard]] std::size_t size() const noexcept { return m_descriptor.base_size; }
    [[nodiscard]] extents sizes() const noexcept { return m_descriptor.sizes; }
    [[nodiscard]] extents base_sizes() const noexcept { return m_descriptor.base_sizes; }
    [[nodiscard]] std::size_t base_size() const noexcept { return m_descriptor.base_size; }
    [[nodiscard]] std::size_t totalsize() const noexcept { return m_descriptor.totalsize; }
    [[nodiscard]] s_indices min_indices() const noexcept { return m_descriptor.min_indices; }
    [[nodiscard]] s_indices max_indices() const noexcept { return m_descriptor.max_indices; }


private:
    enum class SymmetryCase { T1, T3, T6, T7, T9, T12 };
    
    extents m_dimsizes { 0, 0, 0, 0 };
    struct array_descriptor_t {
        extents sizes {};
        extents base_sizes {};
        std::size_t base_size {};
        std::size_t totalsize {};
        s_indices min_indices {};
        s_indices max_indices {};
    } m_descriptor;
    [[nodiscard]] static constexpr extents sizes(extents dimsizes) noexcept;
    [[nodiscard]] static constexpr extents base_sizes(extents dimsizes) noexcept;
    [[nodiscard]] static constexpr SymmetryCase classify_indices(const s_indices& indices) noexcept;
    [[nodiscard]] static constexpr s_indices canonicalize_indices(s_indices indices, bool& conjugate) noexcept;
    [[nodiscard]] static constexpr std::size_t calc_offset(array_descriptor_t descriptor, s_indices indices) noexcept;
    T& data_at(std::size_t offset) noexcept;
    const T& data_at(std::size_t offset) const noexcept;
    /*! returns address offset of element with indices [<i>i,j,k,l</i>] */

    [[nodiscard]] std::string build_error_message(const std::string& prefix, const s_indices& indices) const;
    [[nodiscard]] std::string to_string(const s_indices& indices) const;
    static constexpr array_descriptor_t compute_descriptor(extents dimsizes);
};

// *************************************************
// Member definitions / implementation part
// *************************************************

template <typename T>
constexpr typename Bispectrum<T>::array_descriptor_t Bispectrum<T>::compute_descriptor(extents dimsizes)
{
    Bispectrum<T>::array_descriptor_t descriptor;
    descriptor.sizes = Bispectrum<T>::sizes(dimsizes);
    descriptor.base_sizes = Bispectrum<T>::base_sizes(dimsizes);
    descriptor.base_size = descriptor.base_sizes.product();
    descriptor.totalsize = descriptor.sizes.product();
    std::transform(std::begin(descriptor.sizes), std::end(descriptor.sizes),
                       std::begin(descriptor.min_indices),
                       [](extents::value_type x) { return -static_cast<s_indices::value_type>(x) / 2; });
    std::transform(std::begin(descriptor.sizes), std::end(descriptor.sizes),
                       std::begin(descriptor.max_indices),
                       [](extents::value_type x) { return static_cast<s_indices::value_type>(x); });
    descriptor.max_indices += descriptor.min_indices;
    descriptor.max_indices -= 1;
    return descriptor;
}

template <typename T>
constexpr typename Bispectrum<T>::SymmetryCase Bispectrum<T>::classify_indices(const s_indices& indices) noexcept
{
    if (indices[0] <= 0 && indices[2] <= 0) return SymmetryCase::T1;
    if (indices[0] > 0 && indices[2] > 0) return SymmetryCase::T7;
    if (indices[0] > 0 && indices[2] <= 0) return (indices[0] + indices[2] > 0) ? SymmetryCase::T6 : SymmetryCase::T9;
    if (indices[0] <= 0 && indices[2] > 0) return (indices[0] + indices[2] > 0) ? SymmetryCase::T3 : SymmetryCase::T12;
    std::unreachable(); // unreachable since all combinations are covered
}

template <typename T>
Bispectrum<T>::Bispectrum()
    : Array_base<T> {}
{
}

template <typename T>
Bispectrum<T>::Bispectrum(const Bispectrum<T>::extents& dimsizes)
    : m_dimsizes { dimsizes }, m_descriptor{ compute_descriptor(dimsizes) }
{
    this->resize(base_size());
    // the following lines initialize the array with default zero values depending on type
    // both versions should be equally performant
//     std::fill(Array_base<T>::begin(), Array_base<T>::end(), T {});
    std::fill_n(Array_base<T>::data().get(), base_size(), T {});
}

template <typename T>
Bispectrum<T>::Bispectrum(const Bispectrum<T>& other)
    : Array_base<T>(other.base_size())
    , m_dimsizes { other.m_dimsizes }
    , m_descriptor{ compute_descriptor(other.m_dimsizes) }
{
    std::copy(other.begin(), other.end(), Array_base<T>::begin());
}

template <typename T>
constexpr typename Bispectrum<T>::extents Bispectrum<T>::sizes(extents dimsizes) noexcept
{
    // true sizes of ux,uy,vx,vy dimensions
    extents vec { dimsizes / 2 };
    vec *= 2;
    vec += 1UL;
    return vec;
}

template <typename T>
constexpr typename Bispectrum<T>::extents Bispectrum<T>::base_sizes(extents dimsizes) noexcept
{
    // reduced sizes of ux,uy,vx,vy dimensions
    extents vec = { Bispectrum<T>::sizes(dimsizes) };
    vec -= Bispectrum<T>::sizes(dimsizes) * extents { 1, 0, 1, 0 } / 2;
    return vec;
}

template <typename T>
Bispectrum<T>& Bispectrum<T>::operator=(const Bispectrum<T>& x)
{
    m_dimsizes = x.m_dimsizes;
    m_descriptor = compute_descriptor(m_dimsizes);
    this->resize(base_size());
    std::copy(x.begin(), x.end(), Array_base<T>::begin());
    return *this;
}

template <typename T>
Bispectrum<T>& Bispectrum<T>::operator+=(const Bispectrum<T>& x)
{
    if (m_dimsizes != x.m_dimsizes) {
        throw std::invalid_argument("Bispectrum::operator+=(const Bispectrum) : operand dimension size mismatch");
    }
    std::transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        std::plus<T>());
    return *this;
}

template <typename T>
Bispectrum<T>& Bispectrum<T>::operator-=(const Bispectrum<T>& x)
{
    if (m_dimsizes != x.m_dimsizes) {
        throw std::invalid_argument("Bispectrum::operator+=(const Bispectrum) : operand dimension size mismatch");
    }
    std::transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        std::minus<T>());
    return *this;
}

template <typename T>
Bispectrum<T>& Bispectrum<T>::operator*=(const Bispectrum<T>& x)
{
    if (m_dimsizes != x.m_dimsizes) {
        throw std::invalid_argument("Bispectrum::operator+=(const Bispectrum) : operand dimension size mismatch");
    }
    std::transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        std::multiplies<T>());
    return *this;
}

template <typename T>
Bispectrum<T>& Bispectrum<T>::operator/=(const Bispectrum<T>& x)
{
    if (m_dimsizes != x.m_dimsizes) {
        throw std::invalid_argument("Bispectrum::operator+=(const Bispectrum) : operand dimension size mismatch");
    }
    std::transform(this->begin(), this->end(),
        x.begin(), this->begin(),
        std::divides<T>());
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
std::size_t Bispectrum<T>::calc_offset(s_indices indices) const noexcept
{
    return calc_offset(m_descriptor, indices);
}

template <typename T>
constexpr std::size_t Bispectrum<T>::calc_offset(Bispectrum<T>::array_descriptor_t descriptor, s_indices indices) noexcept
{
    indices *= { -1, 1, -1, 1 };
    // add dimension size to the index in case the index is negative
    std::transform(
        std::begin(indices),
        std::end(indices),
        std::begin(descriptor.sizes),
        std::begin(indices),
        [](auto a, auto b) {
            return a < 0 ? a + b : a;
        }
    );
    
    std::size_t addr { static_cast<std::size_t>(indices[0]) };

    addr *= descriptor.base_sizes[1];
    addr += indices[1];
    addr *= descriptor.base_sizes[2];
    addr += indices[2];
    addr *= descriptor.base_sizes[3];
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
                        this->data_at(calc_offset({ i, j, k, l })) += t;
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
    Array_base<T>::resize(size);
    m_dimsizes = dims;
    m_descriptor = compute_descriptor(m_dimsizes);
    if (fread(Array_base<T>::data(), sizeof(T), size, stream) != size) {
        std::cerr << "Bispectrum<T>::read_from_file(const std::string&): error reading data chunck from file " << filename << std::endl;
        fclose(stream);
        return;
    }
    fclose(stream);
}

template <typename T>
void Bispectrum<T>::print() const
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
    const s_indices max_idx = max_indices(); // cache once

    if ((std::abs(indices[2]) > max_idx[2]) || (std::abs(indices[3]) > max_idx[3])) {
        std::swap(indices[0], indices[2]);
        std::swap(indices[1], indices[3]);
    }
    if ((std::abs(indices[2]) > max_idx[2]) || (std::abs(indices[3]) > max_idx[3])) [[unlikely]] {
        throw std::out_of_range(build_error_message("Bispectrum: Initial element access bounds check failed", indices));
    }

    bool conjugate { false };
    auto uv = Bispectrum<T>::canonicalize_indices(indices, conjugate);

    if ((std::abs(uv[2]) > max_idx[2]) || (std::abs(uv[3]) > max_idx[3])) {
        std::swap(uv[0], uv[2]);
        std::swap(uv[1], uv[3]);
    }

    std::size_t addr = calc_offset(uv);

    if (addr >= this->base_size()) [[unlikely]] {
        throw std::out_of_range(build_error_message("Bispectrum: element address out of bounds.", indices));
    }

    if (conjugate) [[likely]]
        return std::conj(data_at(addr));
    return data_at(addr);
}

template <typename T>
void Bispectrum<T>::get_elements(const std::vector<s_indices>& idx_list, std::vector<T>& results) const
{
    results.clear();
    results.reserve(idx_list.size());

    for (const auto& idx : idx_list) {
        results.push_back(get_element(idx));
    }
}

template <typename T>
void Bispectrum<T>::put_element(s_indices indices, const T& value)
{
    std::size_t addr = this->calc_offset(indices);
    if (addr >= this->base_size()) [[unlikely]] {
        throw std::out_of_range(build_error_message("Bispectrum: put_element address out of bounds.", indices));
        //throw ElementOutOfBounds("trying to access bispectrum element out of range");
    }
//     Array_base<T>::data().get()[addr] = value;
    data_at(addr) = value;
}

// -------------------- helpers ------------------------

template <typename T>
T& Bispectrum<T>::data_at(std::size_t offset) noexcept
{
    assert(offset < this->base_size());
    return Array_base<T>::data()[offset];
}

template <typename T>
const T& Bispectrum<T>::data_at(std::size_t offset) const noexcept
{
    assert(offset < this->base_size());
    return Array_base<T>::data()[offset];
}

template <typename T>
constexpr typename Bispectrum<T>::s_indices Bispectrum<T>::canonicalize_indices(s_indices indices, bool& conjugate) noexcept
{
    conjugate = false;
    auto scase = classify_indices(indices);

    switch (scase) {
        case SymmetryCase::T1: return indices;
        case SymmetryCase::T7:
            conjugate = true;
            return -indices;
        case SymmetryCase::T6: {
            s_indices uv = indices * s_indices{ -1, -1, 1, 1 };
            uv -= s_indices{ indices[2], indices[3], 0, 0 };
            return uv;
        }
        case SymmetryCase::T9: {
            s_indices uv = indices * s_indices{ -1, -1, 1, 1 };
            uv += s_indices{ 0, 0, indices[0], indices[1] };
            conjugate = true;
            return uv;
        }
        case SymmetryCase::T3: {
            s_indices uv = indices * s_indices{ 1, 1, -1, -1 };
            uv -= s_indices{ 0, 0, indices[0], indices[1] };
            return uv;
        }
        case SymmetryCase::T12: {
            s_indices uv = indices * s_indices{ 1, 1, -1, -1 };
            uv += s_indices{ indices[2], indices[3], 0, 0 };
            conjugate = true;
            return uv;
        }
    }
    std::unreachable();
}

template <typename T>
std::string Bispectrum<T>::build_error_message(const std::string& prefix, const s_indices& indices) const
{
    std::ostringstream oss;
    oss << prefix << ": indices = ["
        << indices[0] << ", " << indices[1] << ", "
        << indices[2] << ", " << indices[3] << "]";
    return oss.str();
}

// Optional: helper to_string for s_indices if needed
template <typename T>
std::string Bispectrum<T>::to_string(const s_indices& indices) const
{
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < indices.size(); ++i) {
        oss << indices[i];
        if (i + 1 < indices.size())
            oss << ", ";
    }
    oss << "]";
    return oss.str();
}

} // namespace smip
