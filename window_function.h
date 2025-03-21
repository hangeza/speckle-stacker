#pragma once

template <typename T>
class Array<T, 2>;

template <typename T, std::size_t window_aperture>
class WindowFunction : public Array<T, 2> {
public:
    WindowFunction() { }
    WindowFunction(std::size_t a_xsize, std::size_t a_ysize)
        : Array<T, 2>(a_xsize, a_ysize)
    {
    }
    ~WindowFunction() { }

protected:
};

template <typename T, std::size_t window_aperture>
class GeneralHamming : public WindowFunction<T, window_aperture> {
public:
    GeneralHamming(std::size_t a_xsize, std::size_t a_ysize, double a_alpha);

protected:
    double m_alpha {};
};

template <typename T, std::size_t window_aperture>
class Hann : public GeneralHamming<T, window_aperture> {
public:
    Hann(std::size_t a_xsize, std::size_t a_ysize)
        : GeneralHamming<T, window_aperture>(a_xsize, a_ysize, 0.5)
    {
    }
};

template <typename T, std::size_t window_aperture>
class Hamming : public GeneralHamming<T, window_aperture> {
public:
    Hamming(std::size_t a_xsize, std::size_t a_ysize)
        : GeneralHamming<T, window_aperture>(a_xsize, a_ysize, 0.54)
    {
    }
};

template <typename T, std::size_t xsize, std::size_t ysize>
class GeneralWindowFunction : public Array<T, 2> {
public:
    GeneralWindowFunction()
        : Array<T, 2>(xsize, ysize)
    {
    }
};

// ********************
// implementation part
// ********************

template <typename T, std::size_t window_aperture>
GeneralHamming<T, window_aperture>::GeneralHamming(std::size_t a_xsize, std::size_t a_ysize, double a_alpha)
    : WindowFunction<T, window_aperture>(a_xsize, a_ysize)
    , m_alpha(a_alpha)
{
    static constexpr double twopi { M_PI * 2 };
    const double trigarg { twopi / static_cast<double>(window_aperture) };
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
