#include "array2.h"
#include "math_functions.h"
#include "range.h"
#include "dimvector.h"

int range_test(int /*argc*/, char* /*argv*/[])
{
    smip::Range<double> float_range{-4.5, 5.5};
    smip::Range<int> int_range{-4, 5};
    smip::Range<smip::DimVector<int,2>> vec_range { {-3, 3}, {5, 10}  };
    std::cout << "range<int>: extent=" << int_range.extent() << "\n";
    std::cout << "range<double>: extent=" << float_range.extent() << "\n";
    smip::DimVector<int,2> p { 0, 6 };
    std::cout << p << " is in range of " << vec_range << " : " << vec_range.contains(p) << "\n";
    assert(vec_range.contains(p));
    p = { -4, 6 };
    std::cout << p << " is in range of " << vec_range << " : " << vec_range.contains(p) << "\n";
    assert(!vec_range.contains(p));
    p = { 0, 11 };
    std::cout << p << " is in range of " << vec_range << " : " << vec_range.contains(p) << "\n";
    assert(!vec_range.contains(p));
    p = { -4, 11 };
    std::cout << p << " is in range of " << vec_range << " : " << vec_range.contains(p) << "\n";
    assert(!vec_range.contains(p));
    return 0;
}
