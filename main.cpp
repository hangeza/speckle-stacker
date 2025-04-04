#include <cstdio>
#include <getopt.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>

#include <complex>
#include <fstream>
#include <functional>
#include <iterator>
#include <numeric>
#include <stdexcept>
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

#include "array2.h"
#include "bispectrum.h"
#include "crosscorrel.h"
#include "log.h"
#include "phasemap.h"
#include "phasereco.h"
#include "point.h"
#include "rect.h"
#include "types.h"
#include "videoio.h"
#include "window_function.h"

using namespace smip;

void Usage(const char* progname)
{
    using namespace std;
    cout << "Speckle Masking Image Processing v1.0" << endl;
    cout << "2002-2012, 2025 HG Zaunick <hg.zaunick@gmx.de>" << endl;
    cout << endl;
    cout << "   Usage :  " << std::string(progname) << " [nrpbcvh?] <source root>" << endl;
    cout << "    available options:" << endl;
    cout << "     -n   --nrframes    <pics>    :   process at most number of <pics> frames" << endl;
    cout << "                                      default : all frames" << endl;
    cout << "     -r   --refframe    <index>   :   index of reference frame (default : first found)" << endl;
    cout << "     -p   --recoradius  <pixels>  :   radius of phase reconstruction (default : 2 * bispectrum extent)" << endl;
    cout << "     -b   --bdepth       <pixels>  :   bispectrum extent (3rd and 4th dimension) (default : 20)" << endl;
    cout << "     -c   --channel     <r|g|b|i> :   color channel (default: i)" << endl;
    cout << "          --calcsum               :   calculate picture sum and shifted sum (default)" << endl;
    cout << "          --no-calcsum            :   do not calculate picture sum and shifted sum" << endl;
    cout << "          --specklemasking        :   perform speckle masking (default)" << endl;
    cout << "          --no-specklemasking     :   do not perform speckle masking" << endl;
    cout << "     -k   --croppos <l:t>         :   fixed crop box position (default: 0:0)" << endl;
    cout << "     -s   --cropsize <w:h>        :   crop box size (default: total image)" << endl;
    cout << "     -f   --follow                :   follow object defined by the ref frame:  shifts the crop box" << endl;
    cout << "                                       for each frame such that the object defined by the reference frame" << endl;
    cout << "                                       is always centered. if no crop box position is specified" << endl;
    cout << "                                       by --croppos, the brightest object within the ref. image is" << endl;
    cout << "                                       automatically selected and the crop box centered around it." << endl;
    cout << "     -v   --verbose               :   increase verbosity level" << endl;
    cout << "          --version               :   display version and exit" << endl;
    cout << "     -h -?  --help                :   help (this screen)" << endl;
    cout << endl;
}

int main(int argc, char* argv[])
{
    const char* progname = argv[0];

    log::system::setup(
        log::Level::Info,
        [](int c) { exit(c); },
        std::cerr);

    log::info() << "Speckle Masking Image Processing";
    log::info() << "v1.0 (c) GPL v2.0 2002-2012, 2025 HG Zaunick (hg.zaunick@gmx.de)";

    std::size_t max_frames { 10000 };
    std::size_t ref_frame { 0 };
    std::size_t bispectrum_depth { 20 };
    std::size_t reco_radius = bispectrum_depth * 2;
    std::uint8_t color_channel { color_channel_t::white };
    Rect<std::size_t> crop_rect {};
    int swSpeckleMasking { 1 };
    int swCalcSum { 1 };
    int swShowVersion { 0 };
    std::size_t verbose { 0 };

    // evaluate command line options
    for (char ch {}; ch != -1;) {
        static struct option long_options[] = {
            // These options set a flag.
            //{"verbose", no_argument,       &verbose_flag, 1},
            //{"brief",   no_argument,       &verbose_flag, 0},
            // These options don't set a flag.
            //  We distinguish them by their indices.
            { "verbose", no_argument, 0, 'v' },
            { "bdepth", required_argument, 0, 'b' },
            { "recoradius", required_argument, 0, 'p' },
            { "refframe", required_argument, 0, 'r' },
            { "nrframes", required_argument, 0, 'n' },
            { "channel", required_argument, 0, 'c' },
            { "croppos", required_argument, 0, 'k' },
            { "cropsize", required_argument, 0, 's' },
            { "help", no_argument, 0, 'h' },
            { "version", no_argument, &swShowVersion, 1 },
            { "no-calcsum", no_argument, &swCalcSum, 0 },
            { "calcsum", no_argument, &swCalcSum, 1 },
            { "no-specklemasking", no_argument, &swSpeckleMasking, 0 },
            { "specklemasking", no_argument, &swSpeckleMasking, 1 },
            { 0, 0, 0, 0 }
        };
        // getopt_long stores the option index here.
        int option_index { 0 };

        ch = getopt_long(argc, argv, "vn:r:p:b:c:h?k:s:",
            long_options, &option_index);

        std::istringstream istr;
        switch (ch) {
        case 'v':
            log::debug() << "verbose";
            verbose++;
            break;
        case 'n':
            log::debug() << "number of frames: " << optarg;
            max_frames = strtoul(optarg, NULL, 10);
            break;
        case 'r':
            log::debug() << "reference frame: " << optarg;
            ref_frame = strtoul(optarg, NULL, 10);
            break;
        case 'p':
            log::debug() << "radius of reconstructed phase disc: " << optarg;
            reco_radius = strtoul(optarg, NULL, 10);
            break;
        case 'b':
            log::debug() << "bispectrum size (dims 3 & 4): " << optarg;
            bispectrum_depth = strtoul(optarg, NULL, 10);
            break;
        case 'k':
            istr.str(std::string(optarg));
            int _a, _b;
            _a = _b = -1;
            char _c;
            istr >> _a >> _c >> _b;
            if (_a < 0 || _b <= 0)
                throw std::range_error("invalid crop box pos arguments");
            crop_rect += { static_cast<std::size_t>(_a), static_cast<std::size_t>(_b) };
            log::debug() << "crop box offset (l:t): " << crop_rect.topleft;
            break;
        case 's':
            istr.str(std::string(optarg));
            int a, b;
            a = b = -1;
            char c;
            istr >> a >> c >> b;
            if (a <= 0 || b <= 0)
                throw std::range_error("invalid crop box size arguments");
            crop_rect.set_size({ static_cast<std::size_t>(a), static_cast<std::size_t>(b) });
            log::debug() << "crop box size (w:h): " << crop_rect.width() << "," << crop_rect.height();
            break;
        case 'c':
            //cout<<"color channel : "<<optarg<<endl;
            if (std::string(optarg) != "") {
                std::string stdstr = std::string(optarg);
                for (std::size_t i { 0 }; i < stdstr.size(); ++i) {
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
                log::debug() << "color channel(s) : (r,g,b) = (" << (color_channel & color_channel_t::red) / color_channel_t::red
                             << "," << (color_channel & color_channel_t::green) / color_channel_t::green
                             << "," << (color_channel & color_channel_t::blue) / color_channel_t::blue << ")";
            }
            break;
        case 'h':
        case '?':
            Usage(progname);
            exit(0);
        default:
            break;
        }
    }

    if (swShowVersion) {
        //         log::info() << "v1.0";
        exit(0);
    };

    switch (verbose) {
    case 0:
        break;
    case 1:
        log::system::level() = log::Level::Info;
        break;
    case 2:
    default:
        log::system::level() = log::Level::Debug;
    }
    argc -= optind;
    argv += optind;

    if (argc != 1) {
        Usage(progname);
        exit(0);
    }
    std::string filename(*argv);

    Array2<complex_t> powerspec;
    Array2<complex_t> indata;
    Bispectrum<bispec_complex_t> bispectrum;
    Array2<complex_t> phases;

    FrameExtractor fe(filename);
    log::info() << "opening video file " << filename;
    if (!fe.is_valid()) {
        log::critical(-1) << "file open error: " << fe.filename();
        //         exit(-1);
    }
    const std::size_t nframes { std::min(fe.nframes(), max_frames) };
    log::info() << "opened video file: " << fe.nframes() << " frames";
    log::notice() << "using " << nframes << "/" << fe.nframes() << " frames";
    log::info() << "creating sum, power spectra and accumulating bispectrum of all frames";
    log::info() << "reading first (reference) frame";
    indata = Mat2Array<complex_t>(fe.extract_next_frame());
    log::debug() << "frame data:";
    if (log::system::level() >= log::Level::Debug)
        indata.print();

    log::debug() << "creating bispectrum with size [" << indata.ncols() << " " << indata.nrows() << " " << bispectrum_depth << " " << bispectrum_depth << "]";
    bispectrum = Bispectrum<bispec_complex_t>({ indata.ncols(), indata.nrows(), bispectrum_depth, bispectrum_depth });
    if (log::system::level() >= log::Level::Debug)
        bispectrum.print();
    fftw_plan forward_plan = fftw_plan_dft_2d(indata.nrows(), indata.ncols(),
        reinterpret_cast<fftw_complex*>(indata.data().get()),
        reinterpret_cast<fftw_complex*>(indata.data().get()),
        FFTW_FORWARD, FFTW_ESTIMATE);

    log::info() << "adding frame to sum image";
    // first, create empty sumarray with frame size
    Array2<double> sumarray(indata.ncols(), indata.nrows());
    // element-wise conversion from complex indata frame to real sumarray
    // sumarray = indata;
    std::transform(indata.begin(), indata.end(), sumarray.begin(), [](const complex_t& c) { return c.real(); });
    // set up cross correlation object with first frame as reference frame
    CrossCorrelation<double> cross_correl(sumarray);

    log::info() << "executing fft";
    fftw_execute(forward_plan);
    log::info() << "accumulating fft to mean bispectrum";
    bispectrum.accumulate_from_fft(indata);
    std::transform(indata.begin(), indata.end(), indata.begin(),
        [](const complex_t& val) {
            return complex_t { std::norm(val), 0. };
        });
    log::info() << "adding power spectrum to mean power spectrum";
    powerspec = indata;

    while (fe.current_frame() < nframes) {
        log::info() << "reading frame " << fe.current_frame() + 1 << "/" << nframes;
        // the following two lines circumvent the move operation
        // which would alter indata's storage address
        indata = complex_t {};
        indata += Mat2Array<complex_t>(fe.extract_next_frame());
        //std::cout << "indata address: " << std::hex << indata.data() << std::dec << "\n";
        // calculate shift of frame wrt ref frame through cross correlation
        auto xyshift = cross_correl(Array2<double>::convert<std::complex<double>>(indata, complex_abs<double>));
        log::info() << "relative shift wrt ref frame: [x,y] = " << xyshift;
        log::info() << "adding back-shifted frame to sum image";
        // add back-shifted frame to sum image
        xyshift = -xyshift;
        sumarray += Array2<double>::convert<std::complex<double>>(indata, complex_abs<double>).shifted(xyshift);
        log::info() << "executing fft";
        fftw_execute(forward_plan);
        log::info() << "accumulating fft to mean bispectrum";
        bispectrum.accumulate_from_fft(indata);
        log::info() << "creating power spectrum from fft";
        std::transform(indata.begin(), indata.end(), indata.begin(),
            [](const complex_t& val) {
                return complex_t { std::norm(val), 0. };
            });
        log::info() << "adding power spectrum to mean power spectrum";
        powerspec += indata;
    }
    log::info() << "normalizing sum image";
    sumarray /= nframes;
    log::info() << "normalizing bispectrum";
    powerspec /= nframes * powerspec.size();
    log::info() << "normalizing power spectrum";
    bispectrum /= bispec_complex_t(nframes, 0.);
    log::notice() << "writing bispectrum to file 'bispectrum.dat'";
    bispectrum.write_to_file("bispectrum.dat");
    fftw_destroy_plan(forward_plan);

    log::info() << "reconstructing fourier phases from bispectrum";
    PhaseMap pm;
    phases = reconstruct_phases<complex_t, bispec_complex_t>(bispectrum, indata.ncols(), indata.nrows(), reco_radius, &pm);
    if (log::system::level() >= log::Level::Debug) {
        log::debug() << "sumarray:";
        sumarray.print();
        log::debug() << "power spectrum:";
        powerspec.print();
        log::info() << "phases:";
        phases.print();
    }
    log::info() << "applying window function to phase map";
    Hann<complex_t> window_f(powerspec.ncols(), powerspec.nrows(), reco_radius * 2);
    phases *= window_f;
    Array2<complex_t> result_image(powerspec.ncols(), powerspec.nrows());
    log::info() << "calculating sqrt of power spectrum";
    std::transform(powerspec.begin(), powerspec.end(), result_image.begin(),
        [](const complex_t& val) {
            return complex_t { std::sqrt(val.real()), 0. };
        });
    log::info() << "combining power spectrum with phases";
    result_image *= phases;

    fftw_plan reverse_plan = fftw_plan_dft_2d(result_image.nrows(), result_image.ncols(),
        reinterpret_cast<fftw_complex*>(result_image.data().get()),
        reinterpret_cast<fftw_complex*>(result_image.data().get()),
        FFTW_BACKWARD, FFTW_ESTIMATE);

    log::notice() << "fft back transform of combined spectrum";
    fftw_execute(reverse_plan);

    if (log::system::level() >= log::Level::Debug) {
        log::debug() << "reconstructed image:";
        result_image.print();
    }
    fftw_destroy_plan(reverse_plan);

    auto abscomp = [](const std::complex<double>& a, const std::complex<double>& b) {
        return (std::abs(a) < std::abs(b));
    };
    auto minmax_d = std::minmax_element(sumarray.begin(), sumarray.end());
    if (log::system::level() >= log::Level::Debug) {
        log::debug() << "sum image: min=" << *(minmax_d.first) << " max=" << *(minmax_d.second);
    }
    auto minmax = std::minmax_element(powerspec.begin(), powerspec.end(), abscomp);
    if (log::system::level() >= log::Level::Debug) {
        log::debug() << "power spectrum: min=" << std::abs(*(minmax.first)) << " max=" << std::abs(*(minmax.second));
    }
    auto normfact { std::abs(*(minmax.second)) / 1000. };
    powerspec /= normfact;
    minmax = std::minmax_element(phases.begin(), phases.end(), abscomp);
    if (log::system::level() >= log::Level::Debug) {
        log::debug() << "phases: min=" << std::abs(*(minmax.first)) << " max=" << std::abs(*(minmax.second));
    }
    minmax = std::minmax_element(result_image.begin(), result_image.end(), abscomp);
    if (log::system::level() >= log::Level::Debug) {
        log::debug() << "reconstructed image: min=" << std::abs(*(minmax.first)) << " max=" << std::abs(*(minmax.second));
    }
    normfact = { std::abs(*(minmax.second)) };
    result_image /= complex_t { normfact, 0. };

    sumarray /= 255.;

    save_frame(Array2Mat<double, double>(sumarray, std::fabs<double>, CV_16UC3, false), "sum_image_falsecolor.png");
    save_frame(Array2Mat<double, double>(sumarray, std::fabs<double>, CV_16U, false), "sum_image.png");
    save_frame(Array2Mat<complex_t, double>(phases, complex_phase<double>, CV_16UC3), "phases_falsecolor.png");
    save_frame(Array2Mat<complex_t, double>(phases, complex_phase<double>, CV_16U), "phases.png");
    save_frame(Array2Mat<PhaseMapElement, double>(pm, get_phase_consistency, CV_16UC3), "phasecons.png");
    save_frame(Array2Mat<complex_t, double>(powerspec, complex_abs<double>, CV_16UC3), "powerspec_falsecolor.png");
    save_frame(Array2Mat<complex_t, double>(powerspec, complex_abs<double>, CV_16U), "powerspec.png");
    save_frame(Array2Mat<complex_t, double>(result_image, complex_abs<double>, CV_16UC3), "reco_image_falsecolor.png");
    save_frame(Array2Mat<complex_t, double>(result_image, complex_abs<double>, CV_16U), "reco_image.png");
    if (log::system::level() >= log::Level::Debug) {
        cv::namedWindow("Display Sum Image", cv::WINDOW_AUTOSIZE);
        cv::namedWindow("Display FFT Image", cv::WINDOW_AUTOSIZE);
        cv::namedWindow("Display Phases Image", cv::WINDOW_AUTOSIZE);
        cv::namedWindow("Display PhaseCons Image", cv::WINDOW_AUTOSIZE);
        cv::namedWindow("Display Reco Image", cv::WINDOW_AUTOSIZE);
        cv::imshow("Display Sum Image", Array2Mat<double, double>(sumarray, std::fabs<double>, CV_8UC3, false));
        cv::imshow("Display FFT Image", Array2Mat<complex_t, double>(powerspec, complex_abs<double>, CV_8UC3));
        cv::imshow("Display Phases Image", Array2Mat<complex_t, double>(phases, complex_phase<double>, CV_8UC3));
        cv::imshow("Display PhaseCons Image", Array2Mat<PhaseMapElement, double>(pm, get_phase_consistency, CV_8UC3));
        cv::imshow("Display Reco Image", Array2Mat<complex_t, double>(result_image, complex_abs<double>, CV_8UC3));
        cv::waitKey(0);
    }
}
