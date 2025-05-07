# Speckle Stacker

**Speckle Stacker** is a C++ application and SMIP (Speckle Masking Image Processing) library designed to reconstruct high-resolution astronomical images from short-exposure video frames. By employing speckle masking techniques, the software mitigates the blurring effects of atmospheric turbulence, enabling astronomical imaging close to the diffraction limit of the optical instrument.

---

## 📖 Project Overview

This project targets astronomers, astrophotographers, and image processing enthusiasts aiming to enhance ground-based telescope imagery. It processes input video data, applies fft, bispectrum accumulation, phase revovery, windowing and fft back transform resulting in a reconstructed deblurred object image close to the diffraction limit of the telescope without atmospheric distortions, depending on the quality and amount of video frames.

---

## 🔬 Scientific Background

### What is Speckle Imaging?

Speckle imaging is a high-resolution astronomical technique that involves capturing numerous short-exposure images to "freeze" atmospheric distortions. These images are then processed to reconstruct a deblurred representation of the observed object.  

### Speckle Masking Technique

Speckle masking is a form of speckle interferometry that utilizes the bispectrum (or closure phases) of short-exposure images. By analyzing the statistical properties of speckle patterns, it reconstructs high-resolution images, overcoming limitations imposed by atmospheric turbulence.  
[More on Speckle Imaging and Speckle Masking](https://en.wikipedia.org/wiki/Speckle_imaging)

---

## 🌌 What SMIP Can Do

| ![Video sequence of HU940](/data/hu940ani/hu940ani.gif) | *Video sequence of binary star WDS 19055+3352 (HU 940)<br/> disturbed by atmospheric turbulence<br/> (source: http://www.astrosurf.com/hfosaf/uk/speckle10.htm)<br/> mag:9.2/9.8, sep = 0.32 arcs, texp: 7ms (40 frames)* |
|--|--|
| ![Sum image of HU940 over 40 frames](/data/hu940ani/sum_image_falsecolor.png) | *Naive picture sum over 40 frames of the sequence<br/>(false color)* |
| ![Speckle reco image of HU940 from 40 frames](/data/hu940ani/reco_image_falsecolor.png) | *Speckle reconstructed image (false color)<br/> with the two clearly separated components* |

---

## 🚀 Features

- **Bispectrum Analysis**: Implements bispectral analysis for image reconstruction.
- **Phase Retrieval**: Uses closure phase algorithm to recover object phases from bispectrum.
- **Cross Correlation**: Provides functions to calculate cross and auto correlations of arrays.
- **Shift and Add (SAA)**: Provides processing of picture sums and shifted picture sums based on cross correlation.
- **CLI Tool & Library**: Offers both an end-user interface and reusable code modules.
- **Cross-platform Build Support**: Works under Linux and Windows (MinGW).
- **Image Output**: Generates multiple diagnostic and reconstruction visualizations.

---

## 🛠️ Build From Source

### Dependencies

- OpenCV (>= 4.x)
- FFTW3 (tested with 3.3.5)
- CMake (>= 3.10)
- C++20-compatible compiler (GCC ≥ 10 or Clang ≥ 12)

#### Linux Installation

```bash
sudo apt install libopencv-dev fftw3-dev cmake
```

#### Windows Installation (MinGW64)

- Install `libopencv-dev` (e.g. via [Chocolatey](https://chocolatey.org/))
- Install FFTW3 library following instructions from: https://fftw.org/install/windows.html

### Build Instructions

```bash
git clone https://github.com/hangeza/speckle-stacker.git
cd speckle-stacker
mkdir build && cd build
cmake ..
make
```

- The CLI executable will be located in `build/bin/smip-cli`
- Library will be under `build/output/lib`

---

## 🧪 CLI Usage

The CLI processes videos or GIFs to reconstruct high-resolution images using speckle masking.

### Reproduce the Example Above

```bash
bin/smip-cli -v -b 32 -p 64 -c r ../data/hu940ani/hu940ani.gif
```

*(Assuming execution from `build` folder)*

### Output

- Sum image
- Power spectrum
- Phase map
- Phase consistency map
- Reconstructed image (grayscale & false color, 16-bit)

Future versions will support exporting to HDF5 and FITS file formats.

---

## 🧠 Planned Enhancements

- FITS & HDF5 export formats
- OpenMP utilization
- GPU acceleration (CUDA/OpenCL)
- Image analysis algorithms (noise, SNR, entropy etc.)
- Adaptive frame selection (lucky imaging mode based on image analysis)

---

## 🤝 Contributing

Contributions are highly welcome! To propose changes:

1. Fork the repository.
2. Create a feature branch: `git checkout -b feature-xyz`
3. Commit and push your changes.
4. Open a pull request with a description.

---

## 📄 License

This project is licensed under the GNU General Public License v2.0.  
See the [LICENSE](LICENSE) file for details.

---

## 📚 References

- [Speckle Imaging - Wikipedia](https://en.wikipedia.org/wiki/Speckle_imaging)
- [Lucky Imaging - Wikipedia](https://en.wikipedia.org/wiki/Lucky_imaging)
- [Speckle Interferometry of Binary Stars (AMOS 2021)](https://amostech.com/TechnicalPapers/2021/Poster/Tavenner.pdf)
- [G. Weigelt - Modified astronomical speckle interferometry “speckle masking”](https://doi.org/10.1016/0030-4018(77)90077-3)
