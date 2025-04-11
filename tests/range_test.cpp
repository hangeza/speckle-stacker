#include "array2.h"
#include "dimvector.h"
#include "math_functions.h"
#include "range.h"

int range_test(int /*argc*/, char* /*argv*/[])
{
    smip::Range<double> float_range { -4.5, 5.5 };
    smip::Range<int> int_range { -4, 5 };
    smip::Range<smip::DimVector<int, 2>> vec_range { { -3, 3 }, { 5, 10 } };
    std::cout << "range<int>: extent=" << int_range.extent() << "\n";
    std::cout << "range<double>: extent=" << float_range.extent() << "\n";
    smip::DimVector<int, 2> p { 0, 6 };
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
    std::cout << " vec_range.extent() = " << vec_range.extent() << "\n";
    for (auto dimrange : vec_range) {
        std::cout << dimrange << "; ";
    }
    std::cout << "\n";

    smip::DimVector<int, 2> low1 { -1, -2 };
    smip::DimVector<int, 2> hi1 { 2, 1 };
    smip::DimVector<int, 2> low2 { -2, -1 };
    smip::DimVector<int, 2> hi2 { 3, 2 };

    auto low = smip::DimVector<int>::merge(low1, low2);
    auto hi = smip::DimVector<int>::merge(hi1, hi2);

    smip::Range<smip::DimVector<int, 4>> vec_range2 { low, hi };
    for (auto dimrange : vec_range2) {
        std::cout << dimrange << "; ";
    }
    std::cout << "\n";
    return 0;
}
