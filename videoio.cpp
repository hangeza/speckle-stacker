#include <opencv2/opencv.hpp>
#include <string>

#include "videoio.h"

using namespace cv;

#include <cmath>
#include <iostream>

namespace smip {
double get_phase_consistency(const PhaseMapElement& pme) { return pme.consistency; }
//double complex_phase(const std::complex<double>& a) { return (std::arg(a)+M_PI)/(M_PI*2); }
}

FrameExtractor::FrameExtractor(const std::string& filename)
    : m_filename(filename)
{
    bool success { m_cap.open(filename) };
    if (!success) {
        CV_Error(Error::StsObjectNotFound, "Can not open Video file " + filename);
    }
}

FrameExtractor::~FrameExtractor()
{
    m_cap.release();
    m_frame.release();
}

std::size_t FrameExtractor::nframes() const
{
    auto result { m_cap.get(cv::CAP_PROP_FRAME_COUNT) };
    if (result > 0)
        return static_cast<std::size_t>(result);
    else
        return 0L;
}

Mat& FrameExtractor::extract_next_frame()
{
    //try
    {
        m_cap >> m_frame; // get the next frame from video
        std::cout << "FrameExtractor::extract_next_frame(): frame=" << m_frameindex << " : " << m_frame.rows << "x" << m_frame.cols << " type=" << m_frame.type() << " elemSize=" << m_frame.elemSize() << " elemSize1=" << m_frame.elemSize1() << std::endl;
        m_frameindex++;
    }
    /*
    catch( cv::Exception& e ){
        std::cerr << e.msg << std::endl;
        //exit(1);
    }
*/
    return m_frame;
}

Array<double, 2> Mat2ArrayD(cv::Mat& mat, color_channel_t channel)
{
    int cols = mat.cols, rows = mat.rows;
    assert(mat.type() == 16 || mat.type() == 0);
    const std::size_t bytesperpixel { mat.elemSize() };
    const std::size_t bytesperchannel { mat.elemSize1() };
    const std::size_t channels { bytesperpixel / bytesperchannel };
    Array<double, 2> arr(rows, cols);
    if (mat.isContinuous()) {
        cols *= rows;
        rows = 1;
    }

    std::size_t ch = std::min(static_cast<std::size_t>(channel), channels);

    for (int i = 0; i < rows; i++) {
        if (bytesperchannel == 1) {
            const std::uint8_t* Mi = mat.ptr<std::uint8_t>(i);
            for (int j = 0; j < cols; j++) {
                arr.data().get()[i * cols + j] = Mi[j * channels + ch];
            }
        }
    }
    return arr;
}

Array<std::complex<double>, 2> Mat2ArrayC(cv::Mat& mat, color_channel_t channel)
{
    int cols = mat.cols, rows = mat.rows;
    assert(mat.type() == 16 || mat.type() == 0);
    const std::size_t bytesperpixel { mat.elemSize() };
    const std::size_t bytesperchannel { mat.elemSize1() };
    const std::size_t channels { bytesperpixel / bytesperchannel };
    Array<std::complex<double>, 2> arr(rows, cols);
    if (mat.isContinuous()) {
        cols *= rows;
        rows = 1;
    }

    std::size_t ch = std::min(static_cast<std::size_t>(channel), channels);

    for (int i = 0; i < rows; i++) {
        if (bytesperchannel == 1) {
            const std::uint8_t* Mi = mat.ptr<std::uint8_t>(i);
            for (int j = 0; j < cols; j++) {
                arr.data().get()[i * cols + j] = { static_cast<double>(Mi[j * channels + ch]), 0. };
            }
        }
    }
    return arr;
}

Mat ArrayD2Mat(const Array<double, 2>& arr, int datatype)
{
    int cols = arr.cols(), rows = arr.rows();
    assert(datatype == CV_8UC1
        || datatype == CV_8UC2
        || datatype == CV_8UC3);
    cv::Mat mat(rows, cols, datatype);
    const std::size_t bytesperpixel { mat.elemSize() };
    const std::size_t bytesperchannel { mat.elemSize1() };
    const std::size_t channels { bytesperpixel / bytesperchannel };
    if (mat.isContinuous()) {
        cols *= rows;
        rows = 1;
    }

    for (int i = 0; i < rows; i++) {
        if (bytesperchannel == 1) {
            std::uint8_t* Mi = mat.ptr<std::uint8_t>(i);
            for (int j = 0; j < cols; j++) {
                for (std::size_t ch = 0; ch < channels; ch++) {
                    Mi[j * channels + ch] = static_cast<std::uint8_t>(arr.data().get()[i * rows + j] * 255);
                }
            }
        }
    }
    return mat;
}

Mat ArrayC2Mat(const Array<std::complex<double>, 2>& arr, double (*converter)(const std::complex<double>&), int datatype)
{
    int cols = arr.cols(), rows = arr.rows();
    assert(datatype == CV_8UC1
        || datatype == CV_8UC2
        || datatype == CV_8UC3);
    cv::Mat mat(rows, cols, datatype);
    const std::size_t bytesperpixel { mat.elemSize() };
    const std::size_t bytesperchannel { mat.elemSize1() };
    const std::size_t channels { bytesperpixel / bytesperchannel };
    if (mat.isContinuous()) {
        cols *= rows;
        rows = 1;
    }

    for (int i = 0; i < rows; i++) {
        if (bytesperchannel == 1) {
            std::uint8_t* Mi = mat.ptr<std::uint8_t>(i);
            for (int j = 0; j < cols; j++) {
                auto x = converter(arr.data().get()[i * rows + j]);
                if (channels == 1) {
                    Mi[j * channels] = static_cast<std::uint8_t>(x * 255.);
                } else if (channels == 3) {
                    auto colors { mapToColor<std::uint8_t>(x) };
                    Mi[j * channels] = colors.b;
                    Mi[j * channels + 1] = colors.g;
                    Mi[j * channels + 2] = colors.r;
                } else {
                    for (std::size_t ch = 0; ch < channels; ch++) {
                        Mi[j * channels + ch] = static_cast<std::uint8_t>(x * 255.);
                    }
                }
            }
        }
    }
    return mat;
}

void save_frame(const Mat& frame, const std::string& outfilename)
{
    std::vector<int> compression_params;
    //compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    //compression_params.push_back(100);
    imwrite(outfilename, frame, compression_params);
}
