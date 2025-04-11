#pragma once

#include "array2.h"
#include "dimvector.h"
#include <algorithm>
#include <cmath>
#include <concepts>
#include <fftw3.h>
#include <stdexcept>
#include <type_traits>

#include "smip_export.h"

namespace smip {

template <typename T>
class Array2;

/**
 * @brief CrossCorrelation class for fast calculation of cross correlation between two Array2 objects
 * @tparam T array element value type, constrained to floating point types
 * @details This class provides the fast calculation of the 2d cross correlation of two objects of type Array2<T>,
 * where T is constrained to floating point types (float, double, long double, and special types recognized
 * by the std library through the concept std::floating_point.
 * The reference frame is passed by reference on object construction through the only constructor
 * {@link #CrossCorrelation(const Array2<T>&) CrossCorrelation}.
 * This class provides two functionalities. First, the 2d cross correlation array is calculated by 
 * {@link #correlate(const Array2<T>&)} and afterwards accessed through the 
 * {@link #get_correlation_array()} method.
 * Second, the position of the maximum of the computed distribution can be accessed through 
 * {@link #get_displacement()} in order to obtain the mutual displacement between the two
 * frames. The return value of this method is a 2d {@link DimVector<int,2>} object containing 
 * the displacement vector in x and y coordinates, respectively.
 * There are two recommended access methods to retrieve the displacement vector for a given Array2
 * object wrt. the reference frame supplied to the constructor. First is the non-static 
 * {@link #operator()(const Array2<T>&) operator()} member and the other one the 
 * static {@link #get_displacement(const Array2<T>&, const Array2<T>&)} function. 
 * The latter can be called without object and therefore needs to be supplied with the reference frame 
 * as the first argument.
 * @note: Calling {@link #get_correlation_array()} or {@link #get_displacement()} without a previous
 * call to {@link #correlate(const Array2<T>&)} or {@link #operator()(const Array2<T>&) operator()} 
 * in order to provide the second argument required for the computation of the cross correlation
 * throws a std::invalid_argument error.
 */
template <typename T>
requires std::floating_point<T>
class SMIP_EXPORT CrossCorrelation {
public:
    CrossCorrelation() = delete;
    CrossCorrelation(const Array2<T>& ref);
    void correlate(const Array2<T>& frame);
    auto get_correlation_array() -> const Array2<T>&;
    auto get_displacement() -> DimVector<int, 2>;

    auto operator()(const Array2<T>& frame) -> DimVector<int, 2>;
    static auto get_displacement(const Array2<T>& a, const Array2<T>& b) -> DimVector<int, 2>
    {
        CrossCorrelation<T> correl(a);
        return correl(b);
    }

private:
    // clang-format off
    enum class readiness : std::uint8_t {
        none    = 0x00,
        correl  = 0x01,
        shift   = 0x02
    } m_readiness { readiness::none };
    // clang-format on

    void calculate_displacement();

    Array2<double> m_refframe;
    Array2<double> m_correlation;
    DimVector<int, 2> m_shift {};
};

//********************
// deduction guides
//********************

template <typename T>
requires std::floating_point<T>
CrossCorrelation(const Array2<T>& ref) -> CrossCorrelation<T>;

//********************
// implementation part
//********************

template <typename T>
requires std::floating_point<T>
CrossCorrelation<T>::CrossCorrelation(const Array2<T>& ref)
    : m_refframe(ref)
{
}

template <typename T>
requires std::floating_point<T>
void CrossCorrelation<T>::correlate(const Array2<T>& frame)
{
    if ((frame.ncols() != m_refframe.ncols()) || (frame.nrows() != m_refframe.nrows())) {
        throw std::invalid_argument("Matrix dimensions must match for correlation");
    }
    Array2<double> y(frame);
    assert(y.size() == m_refframe.size());
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
    std::transform(fft1.begin(), fft1.end(), fft2.begin(), fft1.begin(),
        [](const std::complex<double>& a, const std::complex<double>& b) {
            return std::conj(a) * b;
        });
    // prepare m_correlation for data reception
    m_correlation = Array2<double>(m_refframe.ncols(), m_refframe.nrows());
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

template <typename T>
requires std::floating_point<T>
auto CrossCorrelation<T>::get_correlation_array() -> const Array2<T>&
{
    if (m_readiness < readiness::correl)
        throw std::bad_function_call();
    return m_correlation;
}

template <typename T>
requires std::floating_point<T>
auto CrossCorrelation<T>::get_displacement() -> DimVector<int, 2>
{
    if (m_readiness < readiness::correl)
        throw std::bad_function_call();
    if (m_readiness == readiness::correl) {
        calculate_displacement();
    }
    return m_shift;
}

template <typename T>
requires std::floating_point<T>
void CrossCorrelation<T>::calculate_displacement()
{
    if (m_readiness == readiness::correl) {
        auto max_it = std::max_element(m_correlation.begin(), m_correlation.end());
        auto dist { static_cast<int>(std::distance(m_correlation.begin(), max_it)) };
        auto stride { static_cast<int>(m_correlation.xsize()) };
        m_shift = { dist % stride, dist / stride };
        m_shift[0] -= (m_shift[0] < static_cast<int>(m_correlation.ncols() + 1) / 2) ? 0 : m_correlation.ncols();
        m_shift[1] -= (m_shift[1] < static_cast<int>(m_correlation.nrows() + 1) / 2) ? 0 : m_correlation.nrows();
        m_readiness = readiness::shift;
    }
}

template <typename T>
requires std::floating_point<T>
auto CrossCorrelation<T>::operator()(const Array2<T>& frame) -> DimVector<int, 2>
{
    m_readiness = readiness::none;
    correlate(frame);
    calculate_displacement();
    return m_shift;
}

} // namespace smip
