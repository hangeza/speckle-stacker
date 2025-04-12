#include "array2.h"
#include "math_functions.h"
#include <ranges>

int array4_test(int /*argc*/, char* /*argv*/[])
{
    smip::Array_base<int> arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    smip::Array2<int> arr2 { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
    arr2.print();
    std::cout << "arr2:\n"
              << arr2 << "\n";
    static_assert(std::ranges::range<smip::Array_base<int>>);
    static_assert(std::ranges::range<smip::Array2<int>>);
    // Use ranges
    auto ref_view = std::ranges::ref_view(arr);
    for (auto val : std::views::all(arr) | std::views::transform(smip::sqr<int>)) {
        std::cout << val << " ";
    }
    std::cout << '\n';
    for (auto val : std::views::all(arr2) | std::views::transform(smip::sqr<int>)) {
        std::cout << val << " ";
    }
    std::cout << '\n';
    return 0;
}
