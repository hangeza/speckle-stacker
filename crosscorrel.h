#pragma once

#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <type_traits>
#include <fftw3.h>
#include "array2.h"
#include "dimvector.h"
#include <concepts>

namespace smip {

template <typename T>
class Array2;

// the general class template for all types should never be called
template<typename T>
class CrossCorrelation {
public:
    static auto get_displacement(const Array2<T>& a, const Array2<T>& b) -> DimVector<int, 2> {
        throw std::invalid_argument("invalid SFINAE call of CrossCorrelation::get_displacement()");
        return DimVector<int, 2>();
    }
};

// the partial specialization with concept selects only floating point types
template <concept_floating T>
class CrossCorrelation <T>
{
public:
    CrossCorrelation() = delete;
    CrossCorrelation(const Array2<T>& ref);
    void correlate(const Array2<T>& frame);
    auto get_correlation_array() -> const Array2<T>&;
    auto get_displacement() -> DimVector<int,2>;

    auto operator()(const Array2<T>& frame) -> DimVector<int,2>;
    static auto get_displacement(const Array2<T>& a, const Array2<T>& b) -> DimVector<int,2>
    {
        CrossCorrelation<T> correl(a);
        return correl(b);
    }
    
private:
    enum class readiness : std::uint8_t
    {
        none    = 0x00,
        correl  = 0x01,
        shift   = 0x02
    } m_readiness { readiness::none };
    
    void calculate_displacement();
    
    Array2<double> m_refframe;
    Array2<double> m_correlation;
    DimVector<int,2> m_shift {};
};

//********************
// deduction guides
//********************
template <typename T>
CrossCorrelation(const Array2<T>& ref) -> CrossCorrelation<T>;

//********************
// implementation part
//********************
template <concept_floating T>
CrossCorrelation<T>::CrossCorrelation(const Array2<T>& ref)
    : m_refframe(ref)
{}

template <concept_floating T>
void CrossCorrelation<T>::correlate(const Array2<T>& frame)
{
    if ( (frame.ncols() != m_refframe.ncols()) || (frame.nrows() != m_refframe.nrows()) ) {
        throw std::invalid_argument("Matrix dimensions must match for correlation");
    }
    Array2<double> y(frame);
    Array2<std::complex<double>> fft1(m_refframe.ncols() / 2 + 1, m_refframe.nrows());
    Array2<std::complex<double>> fft2(m_refframe.ncols() / 2 + 1, m_refframe.nrows());
    
    // set up real-to-complex DFTs
    // ref: https://www.fftw.org/fftw3_doc/Real_002ddata-DFTs.html#Real_002ddata-DFTs
    // see definition of FFTW3's real-data DFT data format:
    // https://www.fftw.org/fftw3_doc/Real_002ddata-DFT-Array-Format.html
    fftw_plan p1 = fftw_plan_dft_r2c_2d(m_refframe.nrows(), m_refframe.ncols(),
                                        m_refframe.data().get(),
                                        reinterpret_cast<fftw_complex*>(fft1.data().get()),
                                        FFTW_ESTIMATE);
    fftw_plan p2 = fftw_plan_dft_r2c_2d(m_refframe.nrows(), m_refframe.ncols(),
                                        y.data().get(),
                                        reinterpret_cast<fftw_complex*>(fft2.data().get()),
                                        FFTW_ESTIMATE);
    
    fftw_execute(p1);
    fftw_execute(p2);
    fftw_destroy_plan(p1);
    fftw_destroy_plan(p2);
    
    // execute element-wise conj(fft1) * fft2 for power spectrum
    std::transform(fft1.cbegin(), fft1.cend(), fft2.cbegin(), fft1.begin(), 
                   [](const std::complex<double>& a, const std::complex<double>& b) 
                    {
                        return std::conj(a) * b;
                    });
    
    // prepare m_correlation for data reception
    m_correlation = Array2<double>{ m_refframe.ncols(), m_refframe.nrows() };
    // fft1 holds the power spectrum now
    // set up complex-to-real back transformation
    fftw_plan q = fftw_plan_dft_c2r_2d(m_refframe.nrows(), m_refframe.ncols(),
                                       reinterpret_cast<fftw_complex*>(fft1.data().get()),
                                       m_correlation.data().get(),
                                       FFTW_ESTIMATE);
    // back transformed power spectrum = cross correlation to m_correlation
    fftw_execute(q);
    fftw_destroy_plan(q);
    m_readiness = readiness::correl;
}

template <concept_floating T>
auto CrossCorrelation<T>::get_correlation_array() -> const Array2<T>&
{
    if ( m_readiness < readiness::correl ) throw std::bad_function_call("No correlation matrix available");
    return m_correlation;
}

template <concept_floating T>
auto CrossCorrelation<T>::get_displacement() -> DimVector<int,2>
{
    if ( m_readiness < readiness::correl ) throw std::bad_function_call("No correlation matrix available");
    if (m_readiness == readiness::correl) {
        calculate_displacement();
    }
    return m_shift;    
}

template <concept_floating T>
void CrossCorrelation<T>::calculate_displacement()
{
    if (m_readiness == readiness::correl) { 
        auto max_it = std::max_element(m_correlation.begin(), m_correlation.end());
        auto dist = std::distance(m_correlation.begin(), max_it);
        m_shift = { dist%m_correlation.xsize(), dist/m_correlation.xsize() };
        m_shift[0] -= (m_shift[0]<(m_correlation.ncols()+1)/2)?0:m_correlation.ncols();
        m_shift[1] -= (m_shift[1]<(m_correlation.nrows()+1)/2)?0:m_correlation.nrows();
        m_readiness = readiness::shift;
    }
}

template <concept_floating T>
auto CrossCorrelation<T>::operator()(const Array2<T>& frame) -> DimVector<int,2>
{
    m_readiness = readiness::none;
    correlate(frame);
    calculate_displacement();
    return m_shift;
}

} // namespace smip
