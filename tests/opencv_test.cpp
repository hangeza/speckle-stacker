#include "array2.h"
#include "testconfig.h"
#include "videoio.h"
#include <iostream>
#include <random>

int opencv_test(int /*argc*/, char* /*argv*/[])
{
    const std::string filename { smip::test::datafile };
    smip::FrameExtractor fe(filename);
    std::cout << "opening video file " << filename << "\n";
    if (!fe.is_valid()) {
        std::cerr << "file open error: " << fe.filename() << "\n";
        exit(-1);
    }
    const std::size_t nframes { fe.nframes() };
    std::cout << "opened video file: " << nframes << " frames\n";
    assert(nframes == 40);
    return 0;
}
