#include <cstdio>
#include <getopt.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>

#include <fstream>
#include <functional>
#include <iterator>
#include <numeric>
#include <unistd.h> // for getopt()
#include <vector>

#include <fftw3.h>

#include "opencv2/core.hpp"
#include "opencv2/core/base.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/video.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/core/mat.hpp>

#include "bispectrum.h"
#include "multidimarray.h"
#include "phasemap.h"
#include "phasereco.h"
#include "videoio.h"
#include "window_function.h"
#include "log.h"
#include "point.h"
#include "rect.h"


typedef std::complex<double> complex_t;
typedef std::complex<float> bispec_complex_t;

using namespace std;

void Usage(const char* progname)
{
    cout<<"Speckle Masking Image Processing v1.0"<<endl;
    cout<<"2002-2012, 2025 HG Zaunick <hg.zaunick@gmx.de>"<<endl;
    cout<<endl;
    cout<<"   Usage :  "<<std::string(progname)<<" [nrpbcvh?] <source root>"<<endl;
    cout<<"    available options:"<<endl;
    cout<<"     -n   --nrframes    <pics>    :   process at most number of <pics> frames"<<endl;
    cout<<"                                      default : all files in folder"<<endl;
    cout<<"     -r   --refframe    <index>   :   index of reference frame (default : first found)"<<endl;
    cout<<"     -p   --recoradius  <pixels>  :   radius of phase reconstruction (default : 2 * cispectrum extent)"<<endl;
    cout<<"     -b   --bdepth       <pixels>  :   bispectrum extent (3rd and 4th dimension) (default : 20)"<<endl;
    cout<<"     -c   --channel     <r|g|b|i> :   color channel (default: i)"<<endl;
    cout<<"          --calcsum               :   calculate picture sum and shifted sum (default)"<<endl;
    cout<<"          --no-calcsum            :   do not calculate picture sum and shifted sum"<<endl;
    cout<<"          --specklemasking        :   perform speckle masking (default)"<<endl;
    cout<<"          --no-specklemasking     :   do not perform speckle masking"<<endl;
    cout<<"     -k   --croppos <l:t>         :   fixed crop box position (default: 0:0)"<<endl;
    cout<<"     -s   --cropsize <w:h>        :   crop box size (default: total image)"<<endl;
    cout<<"     -f   --follow                :   follow object defined by the ref frame:  shifts the crop box"<<endl;
    cout<<"                                       for each frame such that the object defined by the reference frame"<<endl;
    cout<<"                                       is always centered. if no crop box position is specified"<<endl;
    cout<<"                                       by --croppos, the brightest object within the ref. image is"<<endl;
    cout<<"                                       automatically selected and the crop box centered around it."<<endl;
    cout<<"     -v   --verbose               :   increase verbosity level"<<endl;
    cout<<"          --version               :   display version and exit"<<endl;
    cout<<"     -h -?  --help                :   help (this screen)"<<endl;
    cout<<endl;
}

int main(int argc, char* argv[])
{
    const char* progname = argv[0];

    smip::log::system::setup(
        smip::log::Level::Info,
        [](int c) { exit(c); },
        std::cerr);

    smip::log::info() << "Speckle Masking Image Processing";
    smip::log::info() << "v1.0 (c) GPL v2.0 2002-2012, 2025 HG Zaunick (hg.zaunick@gmx.de)";

    std::size_t max_frames { 400 };
    std::size_t ref_frame { 0 };
    std::size_t bispectrum_depth { 15 };
    std::size_t reco_radius = bispectrum_depth * 2;
    std::uint8_t color_channel { color_channel_t::white };
    Point<std::size_t> crop_pos {};
    Rect<std::size_t> crop_rect {};
    int swSpeckleMasking { 1 };
    int swCalcSum { 1 };
    std::size_t verbose { 0 };
    

    // evaluate command line options
    for (char ch {}; ch != -1;) {
        static struct option long_options[] =
            {
                // These options set a flag.
                //{"verbose", no_argument,       &verbose_flag, 1},
                //{"brief",   no_argument,       &verbose_flag, 0},
                // These options don't set a flag.
                //  We distinguish them by their indices.
                {"bdepth",  required_argument, 0, 'b'},
                {"recoradius",  required_argument, 0, 'p'},
                {"refframe",  required_argument, 0, 'r'},
                {"nrframes",    required_argument, 0, 'n'},
                {"channel",  required_argument, 0, 'c'},
                {"croppos",  required_argument, 0, 'k'},
                {"cropsize",  required_argument, 0, 's'},
                {"verbose", no_argument,       0, 'v'},
                {"help", no_argument,       0, 'h'},
                {"version", no_argument,       0},
                {"no-calcsum", no_argument,       &swCalcSum, 0},
                {"calcsum", no_argument,       &swCalcSum, 1},
                {"no-specklemasking", no_argument,       &swSpeckleMasking, 0},
                {"specklemasking", no_argument,       &swSpeckleMasking, 1},
                {0, 0, 0, 0}
            };
        // getopt_long stores the option index here.
        int option_index { 0 };
     
        ch = getopt_long (argc, argv, "vn:r:p:b:c:h?k:s:",
                            long_options, &option_index); 
     
        std::string stdstr;
        std::istringstream istr;
        switch (ch) {
            case 'v':
                smip::log::debug() << "verbose";
                verbose++;
                break;
            case 'n':
                smip::log::debug() << "number of frames: " << optarg;
                max_frames = strtoul(optarg,NULL,10);
                break;
            case 'r':
                smip::log::debug() << "reference frame: " << optarg;
                ref_frame = strtoul(optarg,NULL,10);
                break;
            case 'p':
                smip::log::debug() << "radius of reconstructed phase disc: " << optarg;
                reco_radius = strtoul(optarg,NULL,10);
                break;
            case 'b':
                smip::log::debug() << "bispectrum size (dims 3 & 4): " << optarg;
                bispectrum_depth = strtoul(optarg,NULL,10);
                break;
            case 'k':
                stdstr=std::string(optarg);
                istr.str(stdstr);
                int _a,_b;
                _a=_b=-1;
                char _c;
                istr>>_a>>_c>>_b;
                if (_a<=0) break;
                if (_b<=0) _b=_a;
                crop_pos = { _a, _b };
                smip::log::debug() << "crop box offset (l:t): " << crop_pos;
                break;
            case 's':
                stdstr=std::string(optarg);
                istr.str(stdstr);
                int a,b;
                a =-1; b=-1;
                char c;
                istr>>a>>c>>b;
                if (a<=0) break;
                if (b<=0) b=a;
                crop_rect = { {0,0}, {a, b} };
                smip::log::debug() << "crop box size (w:h): " << crop_rect.width() << "," << crop_rect.height();
                break;
            case 'c':
                //cout<<"color channel : "<<optarg<<endl;
                if (optarg!="") {
                    stdstr=std::string(optarg);
                    //std::string str(optarg);
                    for (int i=0; i<stdstr.size(); ++i)
                    {
                        switch (stdstr[i]) {
                            case 'r':
                                color_channel = color_channel_t::red;
                                break;
                            case 'g':
                                color_channel = color_channel_t::green;
                                break;
                            case 'b':
                                color_channel = color_channel_t::blue;
                                break;
                            case 'i':
                                color_channel = color_channel_t::white;
                                break;
                            default:
                                break;
                        }
                    }
                    smip::log::debug() << "color channel(s) : (r,g,b) = (" << (color_channel & color_channel_t::red) / color_channel_t::red
                    << "," <<(color_channel & color_channel_t::green) / color_channel_t::green
                    << "," << (color_channel & color_channel_t::blue) / color_channel_t::blue << ")";
                }
                break;
            case 'h':
            case '?':  Usage(progname); exit(0);
            default: break;
        }
    }
   
    argc -= optind;
    argv += optind;

    if (argc!=1) { Usage(progname); exit(0);}
    std::string filename(*argv);
    
    Array<complex_t, 2> sumarray, powerspec;
    Array<complex_t, 2> indata;
    Bispectrum<bispec_complex_t> bispectrum;
    Array<complex_t, 2> phases;
    FrameExtractor fe(filename);
    smip::log::info() << "opening video file " << filename;
    if (!fe.is_valid()) {
        smip::log::critical(-1) << "file open error: " << fe.filename();
        //exit(-1);
    }
    const std::size_t nframes { std::min(fe.nframes(), max_frames) };
    smip::log::notice() << "opened video file: " << fe.nframes() << " frames";
    smip::log::notice() << "using " << nframes << "/" << fe.nframes() << " frames";
    smip::log::info() << "reading first (reference) frame";
    indata = Mat2Array<complex_t>(fe.extract_next_frame());
    smip::log::debug() << "frame data:";
    if (smip::log::system::level() >= smip::log::Level::Debug) indata.print();

    smip::log::debug() << "creating bispectrum with size [" << indata.cols() << " " << indata.rows() << " " << bispectrum_depth << " " << bispectrum_depth << "]";
    bispectrum = Bispectrum<bispec_complex_t>({ indata.cols(), indata.rows(), bispectrum_depth, bispectrum_depth });
    if (smip::log::system::level() >= smip::log::Level::Debug) bispectrum.print();
    fftw_plan forward_plan = fftw_plan_dft_2d(indata.cols(), indata.rows(),
        reinterpret_cast<fftw_complex*>(indata.data().get()),
        reinterpret_cast<fftw_complex*>(indata.data().get()),
        FFTW_FORWARD, FFTW_ESTIMATE);
    smip::log::info() << "adding frame to sum image";
    sumarray = indata;
    smip::log::info() << "executing fft";
    fftw_execute(forward_plan);
    smip::log::info() << "accumulating fft to mean bispectrum";
    bispectrum.accumulate_from_fft(indata);
    std::transform(indata.begin(), indata.end(), indata.begin(),
        [](const complex_t& val) {
            return complex_t { std::norm(val), 0. };
        });
    smip::log::info() << "adding power spectrum to mean power spectrum";
    powerspec = indata;

    while (fe.current_frame() < nframes) {
        smip::log::info() << "reading frame " << fe.current_frame() + 1 << "/" << nframes;
        indata = Mat2Array<complex_t>(fe.extract_next_frame());
        //std::cout << "indata address: " << std::hex << indata.data() << std::dec << "\n";
        smip::log::info() << "adding frame to sum image";
        sumarray += indata;
        smip::log::info() << "executing fft";
        fftw_execute(forward_plan);
        //std::cout<<"fftw result = "<<forward_plan<<std::endl;
        smip::log::info() << "accumulating fft to mean bispectrum";
        bispectrum.accumulate_from_fft(indata);
        smip::log::info() << "creating power spectrum from fft";
        std::transform(indata.begin(), indata.end(), indata.begin(),
            [](const complex_t& val) {
                return complex_t { std::norm(val), 0. };
            });
        smip::log::info() << "adding power spectrum to mean power spectrum";
        powerspec += indata;
    }
    sumarray /= 1.0 * nframes;
    smip::log::info() << "normalizing bispectrum";
    powerspec /= complex_t(nframes * powerspec.NrElements(), 0.);
    smip::log::info() << "normalizing power spectrum";
    bispectrum /= bispec_complex_t(nframes, 0.);
    smip::log::notice() << "writing bispectrum to file 'bispectrum.dat'";
    bispectrum.write_to_file("bispectrum.dat");
    fftw_destroy_plan(forward_plan);

    smip::log::notice() << "reconstructing fourier phases from bispectrum";
    PhaseMap pm;
    phases = reconstruct_phases<complex_t, bispec_complex_t>(bispectrum, indata.cols(), indata.rows(), reco_radius, &pm);
    if (smip::log::system::level() >= smip::log::Level::Debug) {
        smip::log::debug() << "sumarray:";
        sumarray.print();
        smip::log::debug() << "power spectrum:";
        powerspec.print();
        smip::log::info() << "phases:";
        phases.print();
    }
    smip::log::notice() << "applying window function to phase map";
    Hann<complex_t> window_f(powerspec.rows(), powerspec.cols(), 2 * reco_radius);
    phases *= window_f;
    Array<complex_t, 2> result_image(powerspec.rows(), powerspec.cols());
    smip::log::info() << "calculating sqrt of power spectrum";
    std::transform(powerspec.begin(), powerspec.end(), result_image.begin(),
        [](const complex_t& val) {
            return complex_t { std::sqrt(val.real()), 0. };
        });
    smip::log::notice() << "combining powerspectrum with phases";
    result_image *= phases;

    fftw_plan reverse_plan = fftw_plan_dft_2d(result_image.cols(), result_image.rows(),
        reinterpret_cast<fftw_complex*>(result_image.data().get()),
        reinterpret_cast<fftw_complex*>(result_image.data().get()),
        FFTW_BACKWARD, FFTW_ESTIMATE);

    smip::log::notice() << "fft back transform of combined spectrum";
    fftw_execute(reverse_plan);

    if (smip::log::system::level() >= smip::log::Level::Debug) {
        smip::log::debug() << "reconstructed image:";
        result_image.print();
    }
    fftw_destroy_plan(reverse_plan);

    auto abscomp = [](const std::complex<double>& a, const std::complex<double>& b) {
        return (std::abs(a) < std::abs(b));
    };
    auto minmax = std::minmax_element(sumarray.begin(), sumarray.end(), abscomp);
    if (smip::log::system::level() >= smip::log::Level::Debug) {
        smip::log::debug() << "sum image: min=" << std::abs(*(minmax.first)) << " max=" << std::abs(*(minmax.second));
    }
    minmax = std::minmax_element(powerspec.begin(), powerspec.end(), abscomp);
    if (smip::log::system::level() >= smip::log::Level::Debug) {
        smip::log::debug() << "power spectrum: min=" << std::abs(*(minmax.first)) << " max=" << std::abs(*(minmax.second));
    }
    auto normfact { std::abs(*(minmax.second)) / 1000. };
    powerspec /= complex_t { normfact, 0. };
    minmax = std::minmax_element(phases.begin(), phases.end(), abscomp);
    if (smip::log::system::level() >= smip::log::Level::Debug) {
        smip::log::debug() << "phases: min=" << std::abs(*(minmax.first)) << " max=" << std::abs(*(minmax.second));
    }
    minmax = std::minmax_element(result_image.begin(), result_image.end(), abscomp);
    if (smip::log::system::level() >= smip::log::Level::Debug) {
        smip::log::debug() << "reconstructed image: min=" << std::abs(*(minmax.first)) << " max=" << std::abs(*(minmax.second));
    }
    normfact = { std::abs(*(minmax.second)) };
    result_image /= complex_t { normfact, 0. };

    sumarray /= 255.;

    cv::namedWindow("Display Sum Image", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Display FFT Image", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Display Phases Image", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Display PhaseCons Image", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Display Reco Image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Sum Image", Array2Mat(sumarray, std::abs<double>, CV_8UC3, false));
    cv::imshow("Display FFT Image", Array2Mat(powerspec, std::abs<double>, CV_8UC3));
    cv::imshow("Display Phases Image", Array2Mat(phases, smip::complex_phase<double>, CV_8UC3));
    cv::imshow("Display PhaseCons Image", Array2Mat<PhaseMapElement, double>(pm, smip::get_phase_consistency, CV_8UC3));
    cv::imshow("Display Reco Image", Array2Mat(result_image, std::abs<double>, CV_8UC3));
    save_frame(Array2Mat(sumarray, std::abs<double>, CV_16UC3, false), "sum_image_falsecolor.png");
    save_frame(Array2Mat(sumarray, std::abs<double>, CV_16U, false), "sum_image.png");
    save_frame(Array2Mat(phases, smip::complex_phase<double>, CV_16UC3), "phases_falsecolor.png");
    save_frame(Array2Mat(phases, smip::complex_phase<double>, CV_16U), "phases.png");
    save_frame(Array2Mat<PhaseMapElement, double>(pm, smip::get_phase_consistency, CV_16UC3), "phasecons.png");
    save_frame(Array2Mat(powerspec, std::abs<double>, CV_16UC3), "powerspec_falsecolor.png");
    save_frame(Array2Mat(powerspec, std::abs<double>, CV_16U), "powerspec.png");
    save_frame(Array2Mat(result_image, std::abs<double>, CV_16UC3), "reco_image_falsecolor.png");
    save_frame(Array2Mat(result_image, std::abs<double>, CV_16U), "reco_image.png");
    cv::waitKey(0);

    return 0;
}
