#pragma once

#include <algorithm>
#include <functional>
#include <string>

#include <opencv2/opencv.hpp>

#include "array2.h"
#include "phasemap.h"
#include "types.h"
#include "utility.h"

// #include "smip_export.h"
#include "global.h"

namespace smip {

// clang-format off
enum color_channel_t : std::uint8_t {
    black   = 0b0000'0000,
    blue    = 0b0000'0001,
    green   = 0b0000'0010,
    red     = 0b0000'0100,
    white   = blue | green | red
};
// clang-format on

template <concept_arithmetic T>
struct RGB {
    T r, g, b;
};

template <concept_arithmetic T>
RGB<T> mapToColor(double value);

class SMIP_PUBLIC FrameExtractor {
public:
    FrameExtractor() = delete;
    FrameExtractor(const std::string& filename);
    ~FrameExtractor();

    inline bool is_valid() const { return m_cap.isOpened(); }
    std::size_t nframes() const;
    const std::string& filename() const { return m_filename; }
    inline std::size_t current_frame() const { return m_frameindex; }
    cv::Mat& extract_next_frame();

private:
    std::string m_filename {};
    cv::Mat m_frame {};
    cv::VideoCapture m_cap {};
    std::size_t m_frameindex { 0 };
};

void SMIP_PUBLIC save_frame(const cv::Mat& frame, const std::string& outfilename);
template <typename T>
Array2<T> Mat2Array(cv::Mat& mat, color_channel_t color = color_channel_t::red);
template <typename T, typename U>
cv::Mat Array2Mat(const Array2<T>& arr,
    std::function<U(const T&)> converter = std::fabs<U>,
    int cv_datatype = CV_8U,
    bool signed_symmetry = true);

//********************
// implementation part
//********************

template <typename T>
Array2<T> Mat2Array(cv::Mat& mat, color_channel_t color)
{
    int cols = mat.cols, rows = mat.rows;
    assert(mat.type() == 16 || mat.type() == 0);
    const std::size_t bytesperpixel { mat.elemSize() };
    const std::size_t bytesperchannel { mat.elemSize1() };
    const std::size_t channels { bytesperpixel / bytesperchannel };
    long channel = std::roundl(std::log2(static_cast<int>(color)));
    channel = std::clamp(channel, 0L, static_cast<long>(channels)-1L);
/*    std::cout << "Mat2Array(cv::Mat&, color_channel_t): \n";
    std::cout << " mat.type=" << mat.type() << " color=" << static_cast<int>(color) << " channel=" << channel << " channels=" << channels << " bytesperpixel=" << bytesperpixel << " bytesperchannel=" <<     bytesperchannel << "\n";
*/

    Array2<T> arr(cols, rows);
    if (color == color_channel_t::black) return arr;

    for (int i = 0; i < rows; i++) {
        if (bytesperchannel == 1) {
            const std::uint8_t* Mi = mat.ptr<std::uint8_t>(i);
            for (int j = 0; j < cols; j++) {
                arr(j,i) = static_cast<double>(Mi[j * channels + channel]);
            }
        }
    }
    return arr;
}

template <typename T, typename U>
cv::Mat Array2Mat(const Array2<T>& arr,
    std::function<U(const T&)> converter,
    int cv_datatype,
    bool signed_symmetry)
{
    int cols = arr.ncols(), rows = arr.nrows();
    assert(cv_datatype == CV_8UC1
        || cv_datatype == CV_8UC2
        || cv_datatype == CV_8UC3
        || cv_datatype == CV_16UC1
        || cv_datatype == CV_16UC2
        || cv_datatype == CV_16UC3);
    cv::Mat mat(rows, cols, cv_datatype);
    const std::size_t bytesperpixel { mat.elemSize() };
    const std::size_t bytesperchannel { mat.elemSize1() };
    const std::size_t channels { bytesperpixel / bytesperchannel };
/*    std::cout << "Array2Mat(const Array2<T>&, std::function<U(const T&)>, int, bool): \n";
    std::cout << " channels=" << channels << " bytesperpixel=" << bytesperpixel << " bytesperchannel=" << bytesperchannel << "\n";
*/
    for (int i = 0; i < rows; i++) {
        if (bytesperchannel == 1) {
            std::uint8_t* Mi = mat.ptr<std::uint8_t>(i);
            for (int j = 0; j < cols; j++) {
                T value;
                if (signed_symmetry) {
                    int ii = i + arr.min_sindices()[1];
                    int jj = j + arr.min_sindices()[0];
                    value = { arr.at({ jj, ii }) };
                } else {
                    value = arr.at({ j, i });
                }
                auto x = converter(value);
                if (channels == 1) {
                    Mi[j * channels] = static_cast<std::uint8_t>(x * std::numeric_limits<std::uint8_t>::max());
                } else if (channels == 3) {
                    auto colors { mapToColor<std::uint8_t>(x) };
                    Mi[j * channels] = colors.b;
                    Mi[j * channels + 1] = colors.g;
                    Mi[j * channels + 2] = colors.r;
                } else {
                    for (std::size_t ch = 0; ch < channels; ch++) {
                        Mi[j * channels + ch] = static_cast<std::uint8_t>(x * std::numeric_limits<std::uint8_t>::max());
                    }
                }
            }
        } else if (bytesperchannel == 2) {
            std::uint16_t* Mi = mat.ptr<std::uint16_t>(i);
            for (int j = 0; j < cols; j++) {
                T value;
                if (signed_symmetry) {
                    int ii = i + arr.min_sindices()[1];
                    int jj = j + arr.min_sindices()[0];
                    value = { arr.at({ jj, ii }) };
                } else
                    value = arr.at({ j, i });
                auto x = converter(value);
                if (channels == 1) {
                    Mi[j * channels] = static_cast<std::uint16_t>(x * std::numeric_limits<std::uint16_t>::max());
                } else if (channels == 3) {
                    auto colors { mapToColor<std::uint16_t>(x) };
                    Mi[j * channels] = colors.b;
                    Mi[j * channels + 1] = colors.g;
                    Mi[j * channels + 2] = colors.r;
                } else {
                    for (std::size_t ch = 0; ch < channels; ch++) {
                        Mi[j * channels + ch] = static_cast<std::uint16_t>(x * std::numeric_limits<std::uint16_t>::max());
                    }
                }
            }
        }
    }
    return mat;
}

// Function to map a double (range [0.0, 1.0]) to RGB values
// modified based on proposal from ChatGPT
template <concept_arithmetic T>
RGB<T> mapToColor(double value)
{
    // Ensure the value is clamped between 0 and 1
    value = std::clamp(value, 0., 1.);

    RGB<T> color {};

    // Simple heatmap-like color mapping (Blue to Red)
    if (value < 0.25) {
        color.r = 0;
        color.g = 0;
        color.b = static_cast<T>(std::numeric_limits<T>::max() * (4 * value)); // Blue increases as value increases
    } else if (value < 0.5) {
        color.r = 0;
        color.g = static_cast<T>(std::numeric_limits<T>::max() * (4 * value - 1)); // Green increases as value increases
        color.b = static_cast<T>(std::numeric_limits<T>::max() * (2 - 4 * value)); // Blue decreases as value increases
    } else if (value < 0.75) {
        color.r = static_cast<T>(std::numeric_limits<T>::max() * (4 * value - 2)); // Red increases
        color.g = static_cast<T>(std::numeric_limits<T>::max() * (3 - 4 * value)); // Green decreases
        color.b = 0;
    } else {
        color.r = static_cast<T>(std::numeric_limits<T>::max()); // Red stays constant after 0.75
        color.g = 0;
        color.b = static_cast<T>(std::numeric_limits<T>::max() * (4 * value - 3)); // blue increases
    }
    return color;
}

} // namespace smip
