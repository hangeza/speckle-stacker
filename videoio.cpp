#include <opencv2/opencv.hpp>
#include <string>

#include "videoio.h"

using namespace cv;

#include <cmath>
#include <iostream>

namespace smip {

double get_phase_consistency(const PhaseMapElement& pme) { return pme.consistency; }
//double complex_phase(const std::complex<double>& a) { return (std::arg(a)+M_PI)/(M_PI*2); }


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

void save_frame(const Mat& frame, const std::string& outfilename)
{
    std::vector<int> compression_params;
    //compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    //compression_params.push_back(100);
    imwrite(outfilename, frame, compression_params);
}

} // namespace smip
