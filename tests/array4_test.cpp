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

// // A test function using ranges
// void test_array() {
//     Array<int> arr(5);
// 
//     // Fill the array with values
//     for (std::size_t i = 0; i < arr.get_size(); ++i) {
//         arr[i] = static_cast<int>(i + 1);
//     }
// 
//     // Use ranges
//     auto ref_view = std::ranges::ref_view(arr);
//     for (auto& val : ref_view) {
//         std::cout << val << " ";
//     }
//     std::cout << std::endl;
// 
//     // Apply a range algorithm
//     std::ranges::for_each(ref_view, [](int val) { std::cout << val * 2 << " "; });
// }
