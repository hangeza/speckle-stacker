#include "array2.h"

int main() 
{
    constexpr std::size_t xsize { 10 };
    constexpr std::size_t ysize { 9 };
    smip::Array2<double> arr( xsize, ysize );
    arr.print();
    assert(arr.size() == xsize*ysize);
}
