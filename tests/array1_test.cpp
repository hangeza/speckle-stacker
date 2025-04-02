#include "array2.h"

int array1_test(int argc, char* argv[])
{
    return 0;
    constexpr std::size_t xsize { 10 };
    constexpr std::size_t ysize { 9 };
    smip::Array2<int> arr(xsize, ysize);
    arr.print();
    assert(arr.size() == xsize * ysize);
    return 0;
}
