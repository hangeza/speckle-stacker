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

//typedef float float_t;
typedef std::complex<double> complex_t;
typedef std::complex<float> bispec_complex_t;

int main(int argc, char* argv[])
{
    const std::string filename { "/home/hgz/Videos/venus1.m4v" };

    constexpr std::size_t max_frames { 400 };
    constexpr std::size_t bispectrum_depth { 15 };
    constexpr std::size_t reco_radius = bispectrum_depth * 2;

    Array<complex_t, 2> sumarray, powerspec;
    Array<complex_t, 2> indata;
    Bispectrum<bispec_complex_t> bispectrum;
    Array<complex_t, 2> phases;
    FrameExtractor fe(filename);
    std::cout << "opening video file " << filename << "\n";
    if (!fe.is_valid()) {
        std::cerr << "file open error: " << fe.filename() << "\n";
        exit(-1);
    }
    const std::size_t nframes { std::min(fe.nframes(), max_frames) };
    std::cout << "opened video file: " << fe.nframes() << " frames\n";
    std::cout << "using " << nframes << "/" << fe.nframes() << " frames\n";
    std::cout << "reading first (reference) frame \n";
    indata = Mat2Array<complex_t>(fe.extract_next_frame());
    std::cout << "frame data:\n";
    indata.print();

    std::cout << "creating bispectrum with size [" << indata.cols() << " " << indata.rows() << " " << bispectrum_depth << " " << bispectrum_depth << "]\n";
    bispectrum = Bispectrum<bispec_complex_t>({ indata.cols(), indata.rows(), bispectrum_depth, bispectrum_depth });
    bispectrum.print();
    fftw_plan forward_plan = fftw_plan_dft_2d(indata.cols(), indata.rows(),
        reinterpret_cast<fftw_complex*>(indata.data().get()),
        reinterpret_cast<fftw_complex*>(indata.data().get()),
        FFTW_FORWARD, FFTW_ESTIMATE);
    std::cout << "adding frame to sum image\n";
    sumarray = indata;
    std::cout << "executing fft\n";
    fftw_execute(forward_plan);
    std::cout << "accumulating fft to mean bispectrum\n";
    bispectrum.accumulate_from_fft(indata);
    std::transform(indata.begin(), indata.end(), indata.begin(),
        [](const complex_t& val) {
            return complex_t { std::norm(val), 0. };
        });
    std::cout << "adding power spectrum to mean power spectrum\n";
    powerspec = indata;

    while (fe.current_frame() < nframes) {
        std::cout << "reading frame " << fe.current_frame() + 1 << "/" << nframes << "\n";
        indata = Mat2Array<complex_t>(fe.extract_next_frame());
        //std::cout << "indata address: " << std::hex << indata.data() << std::dec << "\n";
        std::cout << "adding frame to sum image\n";
        sumarray += indata;
        std::cout << "executing fft\n";
        fftw_execute(forward_plan);
        //std::cout<<"fftw result = "<<forward_plan<<std::endl;
        std::cout << "accumulating fft to mean bispectrum\n";
        bispectrum.accumulate_from_fft(indata);
        std::cout << "creating power spectrum from fft\n";
        std::transform(indata.begin(), indata.end(), indata.begin(),
            [](const complex_t& val) {
                return complex_t { std::norm(val), 0. };
            });
        std::cout << "adding power spectrum to mean power spectrum\n";
        powerspec += indata;
    }
    sumarray /= 1.0 * nframes;
    std::cout << "normalizing bispectrum\n";
    powerspec /= complex_t(nframes * powerspec.NrElements(), 0.);
    std::cout << "normalizing power spectrum\n";
    bispectrum /= bispec_complex_t(nframes, 0.);
    std::cout << "writing bispectrum to file 'bispectrum.dat'\n";
    bispectrum.write_to_file("bispectrum.dat");
    fftw_destroy_plan(forward_plan);

    std::cout << "reconstructing fourier phases from bispectrum\n";
    PhaseMap pm;
    phases = reconstruct_phases<complex_t, bispec_complex_t>(bispectrum, indata.cols(), indata.rows(), reco_radius, &pm);
    std::cout << "sumarray: \n";
    sumarray.print();
    std::cout << "power spectrum: \n";
    powerspec.print();
    std::cout << "phases:\n";
    phases.print();

    std::cout << "applying window function to phase map\n";
    Hann<complex_t, 2 * reco_radius> window_f(powerspec.rows(), powerspec.cols());
    phases *= window_f;
    Array<complex_t, 2> result_image(powerspec.rows(), powerspec.cols());
    std::cout << "calculating sqrt of power spectrum\n";
    std::transform(powerspec.begin(), powerspec.end(), result_image.begin(),
        [](const complex_t& val) {
            return complex_t { std::sqrt(val.real()), 0. };
        });
    std::cout << "combining powerspectrum with phases\n";
    result_image *= phases;

    fftw_plan reverse_plan = fftw_plan_dft_2d(result_image.cols(), result_image.rows(),
        reinterpret_cast<fftw_complex*>(result_image.data().get()),
        reinterpret_cast<fftw_complex*>(result_image.data().get()),
        FFTW_BACKWARD, FFTW_ESTIMATE);

    std::cout << "fft back transform of combined spectrum\n";
    fftw_execute(reverse_plan);

    std::cout << "reconstructed image:\n";
    result_image.print();

    fftw_destroy_plan(reverse_plan);

    auto abscomp = [](const std::complex<double>& a, const std::complex<double>& b) {
        return (std::abs(a) < std::abs(b));
    };
    auto minmax = std::minmax_element(sumarray.begin(), sumarray.end(), abscomp);
    std::cout << "sum image: min=" << std::abs(*(minmax.first)) << " max=" << std::abs(*(minmax.second)) << std::endl;
    minmax = std::minmax_element(powerspec.begin(), powerspec.end(), abscomp);
    std::cout << "power spectrum: min=" << std::abs(*(minmax.first)) << " max=" << std::abs(*(minmax.second)) << std::endl;
    auto normfact { std::abs(*(minmax.second)) / 1000. };
    powerspec /= complex_t { normfact, 0. };
    minmax = std::minmax_element(phases.begin(), phases.end(), abscomp);
    std::cout << "phases: min=" << std::abs(*(minmax.first)) << " max=" << std::abs(*(minmax.second)) << std::endl;
    minmax = std::minmax_element(result_image.begin(), result_image.end(), abscomp);
    std::cout << "reconstructed image: min=" << std::abs(*(minmax.first)) << " max=" << std::abs(*(minmax.second)) << std::endl;
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
