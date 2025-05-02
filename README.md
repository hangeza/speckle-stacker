# Speckle Stacker
Speckle Masking Image Processing (SMIP) library and command line interface (CLI) application named speckle stacker for image processing of astronomical image series (videos) using the speckle masking technique calculating the original source distribution undisturbed by atmospheric turbulence.

Example of what SMIP can do:

| ![Video sequence of HU940](/data/hu940ani/hu940ani.gif) | *Video sequence of binary star WDS 19055+3352 (HU 940)<br/> disturbed by atmospheric turbulence<br/> (source: http://www.astrosurf.com/hfosaf/uk/speckle10.htm)<br/> mag:9.2/9.8, sep = 0.32 arcs, texp: 7ms (40 frames)* |
|--|--|
| ![Sum image of HU940 over 40 frames](/data/hu940ani/sum_image_falsecolor.png) | *Naive picture sum over 40 frames of the sequence<br/>(false color)* |
| ![Speckle reco image of HU940 from 40 frames](/data/hu940ani/reco_image_falsecolor.png) | *Speckle reconstructed image<br/>(false color)* | 

## Build from source
### Dependencies
The SMIP library and the executable cli rely on OpenCV and FFTW3. Both must be available at compile time. For building, CMake is required as well as the usual build tools. 
The CMake control file checks for availability of OpenCV and FFTW3 as well as for a sufficiently recent version of the compiler (min. gcc-10 or clang-12).
Install the required packages before build under linux with:

`sudo apt install libopencv-dev fftw3-dev cmake`

Under Windows the mingw64 environment is required for successful build. Install libopencv-dev through e.g. Chocolatey. The FFTW3 binaries can be installed following the instructions from https://fftw.org/install/windows.html.

### Building
Now, build and install the project:
- checkout the main branch of this git repository: `git clone https://github.com/hangeza/speckle-stacker.git`
- in the project dir `mkdir build && cd build`
- `cmake ../`
- `make`

After successful build the library should be found in subfolder output/lib and the cli executable as well as the unit test executable under bin/.
