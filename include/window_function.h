#pragma once
#include <stdexcept>

#include "smip_export.h"

namespace smip {

template <typename T>
class Array2;

template <typename T>
class SMIP_EXPORT WindowFunction : public Array2<T> {
public:
    WindowFunction() = delete;
    WindowFunction(std::size_t a_xsize, std::size_t a_ysize, double window_aperture)
        : Array2<T>(a_xsize, a_ysize)
        , m_aperture(window_aperture)
    {
        if (!(window_aperture > 0.)) {
            throw std::domain_error("WindowFunction aperture size <= 0");
        }
    }
    ~WindowFunction() { }

protected:
    double m_aperture {};
};

template <typename T>
class SMIP_EXPORT GeneralHamming : public WindowFunction<T> {
public:
    GeneralHamming(std::size_t a_xsize, std::size_t a_ysize, double window_aperture, double a_alpha);

protected:
    double m_alpha {};
};

template <typename T>
class SMIP_EXPORT Hann : public GeneralHamming<T> {
public:
    Hann(std::size_t a_xsize, std::size_t a_ysize, double window_aperture)
        : GeneralHamming<T>(a_xsize, a_ysize, window_aperture, 0.5)
    {
    }
};

template <typename T>
class SMIP_EXPORT Hamming : public GeneralHamming<T> {
public:
    Hamming(std::size_t a_xsize, std::size_t a_ysize, double window_aperture)
        : GeneralHamming<T>(a_xsize, a_ysize, window_aperture, 0.54)
    {
    }
};

template <typename T, std::size_t xsize, std::size_t ysize>
class SMIP_EXPORT GeneralWindowFunction : public Array2<T> {
public:
    GeneralWindowFunction()
        : Array2<T>(xsize, ysize)
    {
    }
};

// ********************
// implementation part
// ********************

template <typename T>
GeneralHamming<T>::GeneralHamming(std::size_t a_xsize, std::size_t a_ysize, double window_aperture, double a_alpha)
    : WindowFunction<T>(a_xsize, a_ysize, window_aperture)
    , m_alpha(a_alpha)
{
    static constexpr double twopi { M_PI * 2 };
    const double trigarg { twopi / window_aperture };
    const double c1 { m_alpha };
    const double c2 { 1. - m_alpha };
    for (int i { this->min_sindices()[0] }; i <= this->max_sindices()[0]; ++i) {
        for (int j { this->min_sindices()[1] }; j <= this->max_sindices()[1]; ++j) {
            double temp1 { c1 + c2 * std::cos(trigarg * static_cast<double>(i)) };
            double temp2 { c1 + c2 * std::cos(trigarg * static_cast<double>(j)) };
            this->at({ i, j }) = static_cast<T>(temp1 * temp2);
        }
    }
}

} // namespace smip
