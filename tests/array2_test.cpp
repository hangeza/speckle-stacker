#include <complex>
#include <ranges>
#include "math_functions.h"
#include "array2.h"
#include "test_macros.h"
#include "types.h"

using namespace smip;

namespace Test {
}
// Your fixture
template <typename T>
struct Array2Test {
    using TypeParam = T;
};

// Define the type list
using TestTypes = type_list<int, std::size_t, float, double, std::complex<float>, std::complex<double>>;

// Register the fixture with the types
TYPED_TEST_SUITE(Array2Test, TestTypes);


TEST(Array2Test, ConstructionTest) {
    using TypeParam = int;
    
    TEST_CASE("Array2 Construction");

    Array2<TypeParam> arr(3, 2);
    TEST_EQUAL(arr.xsize(), 3);
    TEST_EQUAL(arr.ysize(), 2);
    TEST_EQUAL(arr.size(), 6);
    TEST_EQUAL(arr(0, 0), TypeParam{}); // Default initialized
}

TEST(Array2Test, BoundsTest) {
    TEST_CASE("Array2 Bounds Checking");

    Array2<int> arr(2, 2);
    arr(1, 1) = 5;
    TEST_EQUAL(arr(1, 1),  5);

    TEST_THROW(arr(2, 0), std::out_of_range); // x out of bounds
    TEST_THROW(arr(0, 2), std::out_of_range); // y out of bounds
}

TEST(Array2Test, CopyTest) {
    TEST_CASE("Array2 Copy Semantics");

    Array2<int> arr1(2, 2);
    arr1(0, 0) = 10;
    arr1(1, 1) = 20;

    Array2<int> arr2 = arr1; // Copy constructor
    TEST_EQUAL(arr2(0, 0), 10);
    TEST_EQUAL(arr2(1, 1), 20);

    Array2<int> arr3(1, 1);
    arr3 = arr1; // Copy assignment
    TEST_EQUAL(arr3(0, 0), 10);
    TEST_EQUAL(arr3(1, 1), 20);
}

TEST(Array2Test, MoveTest) {
    TEST_CASE("Array2 Move Semantics");

    Array2<int> arr1(2, 2);
    arr1(0, 0) = 7;
    arr1(1, 1) = 8;

    Array2<int> arr2 = std::move(arr1); // Move constructor
    TEST_EQUAL(arr2(0, 0), 7);
    TEST_EQUAL(arr2(1, 1), 8);

    Array2<int> arr3(1, 1);
    arr3 = std::move(arr2); // Move assignment
    TEST_EQUAL(arr3(0, 0), 7);
    TEST_EQUAL(arr3(1, 1), 8);
}

TEST(Array2Test, ResizeTest)
{
    TEST_CASE("Array2 Resize");
    Array2<int> arr(2, 2);
    arr(0, 0) = 1;
    arr(1, 1) = 2;

    arr = Array2<int>(3, 3, 99);
    TEST_EQUAL(arr.xsize(), 3);
    TEST_EQUAL(arr.ysize(), 3);
    TEST_EQUAL(arr(0, 0), 99);
    TEST_EQUAL(arr(1, 1), 99);
    arr = Array2<int>(3, 3);
    TEST_EQUAL(arr(2, 2), int{}); // New cells default-initialized
}

TYPED_TEST(Array2Test, FillTest)
{
    TEST_CASE("Array2 Fill");
    Array2<TypeParam> arr(2, 2);
    arr = TypeParam{99};
    TEST_EQUAL_OR_NEAR(arr(0, 0), TypeParam{99});
    TEST_EQUAL_OR_NEAR(arr(1, 1), TypeParam{99});
}

TEST(Array2Test, IndexTest)
{
    TEST_CASE("Array2 Index Operator");
    
    Array2<int> arr(3, 3);
    arr(0, 0) = 1;
    arr(1, 1) = 2;
    arr(2, 2) = 3;

    TEST_EQUAL(arr(0, 0), 1);
    TEST_EQUAL(arr(1, 1), 2);
    TEST_EQUAL(arr(2, 2), 3);
}

TEST(Array2Test, ExceptionTest)
{
    TEST_CASE("Array2 ExceptionHandling");
    Array2<int> a(5, 5);
    TEST_THROW(a(100, 100), std::out_of_range);
    Array2<int> b; // empty
    b.set_at(a.data(),a.size());
    Array2<int> c(5,4); // differently sized array
    // assignment to array which carries a reference...should throw
    TEST_THROW(b=c, std::runtime_error);
}

TYPED_TEST(Array2Test, AdditionOperator)
{
    TEST_CASE("Array2 Addition Operator");

    Array2<TypeParam> a = {
        {1, 2},
        {3, 4}
    };
    Array2<TypeParam> b = {
        {5, 6},
        {7, 8}
    };
    auto c = a + b;

    TEST_EQUAL(c.ncols(), 2);
    TEST_EQUAL(c.nrows(), 2);
    TEST_EQUAL_OR_NEAR(c(0, 0), TypeParam(6));
    TEST_EQUAL_OR_NEAR(c(1, 0), TypeParam(8));
    TEST_EQUAL_OR_NEAR(c(0, 1), TypeParam(10));
    TEST_EQUAL_OR_NEAR(c(1, 1), TypeParam(12));
}

TYPED_TEST(Array2Test, SubtractionOperator)
{
    TEST_CASE("Array2 Subtraction Operator");

    Array2<TypeParam> a = {
        {5, 6},
        {7, 8}
    };
    Array2<TypeParam> b = {
        {1, 2},
        {3, 4}
    };
    auto c = a - b;
    TEST_EQUAL_OR_NEAR(c(0, 0), TypeParam(4));
    TEST_EQUAL_OR_NEAR(c(1, 0), TypeParam(4));
    TEST_EQUAL_OR_NEAR(c(0, 1), TypeParam(4));
    TEST_EQUAL_OR_NEAR(c(1, 1), TypeParam(4));
}

TYPED_TEST(Array2Test, MultiplicationOperator)
{
    TEST_CASE("Array2 Multiplication Operator");

    Array2<TypeParam> a = {
        {1, 2},
        {3, 4}
    };
    Array2<TypeParam> b = {
        {2, 2},
        {2, 2}
    };
    auto c = a * b;
    TEST_EQUAL_OR_NEAR(c(0, 0), TypeParam(2));
    TEST_EQUAL_OR_NEAR(c(1, 0), TypeParam(4));
    TEST_EQUAL_OR_NEAR(c(0, 1), TypeParam(6));
    TEST_EQUAL_OR_NEAR(c(1, 1), TypeParam(8));
}

TYPED_TEST(Array2Test, DivisionOperator)
{
    TEST_CASE("Array2 Division Operator");
    Array2<TypeParam> a = {
        {4, 6},
        {8, 10}
    };
    Array2<TypeParam> b = {
        {2, 3},
        {4, 5}
    };
    auto c = a / b;
    TEST_EQUAL_OR_NEAR(c(0, 0), TypeParam(2));
    TEST_EQUAL_OR_NEAR(c(1, 0), TypeParam(2));
    TEST_EQUAL_OR_NEAR(c(0, 1), TypeParam(2));
    TEST_EQUAL_OR_NEAR(c(1, 1), TypeParam(2));
}

TYPED_TEST(Array2Test, ScalarMultiplication)
{
    TEST_CASE("Array2 Scalar Multiplication");
    Array2<TypeParam> a = {
        {1, 2},
        {3, 4}
    };
    auto c = a * TypeParam(3);
    TEST_EQUAL_OR_NEAR(c(0, 0), TypeParam(3));
    TEST_EQUAL_OR_NEAR(c(1, 0), TypeParam(6));
    TEST_EQUAL_OR_NEAR(c(0, 1), TypeParam(9));
    TEST_EQUAL_OR_NEAR(c(1, 1), TypeParam(12));
}

TYPED_TEST(Array2Test, ScalarDivision)
{
    TEST_CASE("Array2 Scalar Division");
    Array2<TypeParam> a = {
        {2, 4},
        {6, 8}
    };
    auto c = a / TypeParam(2);
    TEST_EQUAL_OR_NEAR(c(0, 0), TypeParam(1));
    TEST_EQUAL_OR_NEAR(c(1, 0), TypeParam(2));
    TEST_EQUAL_OR_NEAR(c(0, 1), TypeParam(3));
    TEST_EQUAL_OR_NEAR(c(1, 1), TypeParam(4));
}

TYPED_TEST(Array2Test, ScalarAddition)
{
    TEST_CASE("Array2 Scalar Addition");
    Array2<TypeParam> a = {
        {1, 2},
        {3, 4}
    };
    auto c = a + TypeParam(5);
    TEST_EQUAL_OR_NEAR(c(0, 0), TypeParam(6));
    TEST_EQUAL_OR_NEAR(c(1, 0), TypeParam(7));
    TEST_EQUAL_OR_NEAR(c(0, 1), TypeParam(8));
    TEST_EQUAL_OR_NEAR(c(1, 1), TypeParam(9));
}

TYPED_TEST(Array2Test, ScalarSubtraction)
{
        TEST_CASE("Array2 Scalar Subtraction");
        Array2<TypeParam> a = {
        {10, 9},
        {8, 7}
    };
    auto c = a - TypeParam(5);
    TEST_EQUAL_OR_NEAR(c(0, 0), TypeParam(5));
    TEST_EQUAL_OR_NEAR(c(1, 0), TypeParam(4));
    TEST_EQUAL_OR_NEAR(c(0, 1), TypeParam(3));
    TEST_EQUAL_OR_NEAR(c(1, 1), TypeParam(2));
}

TYPED_TEST(Array2Test, ConvertMethod)
{
    if constexpr (std::is_arithmetic_v<TypeParam>) {
        // This block is only compiled if TypeParam is arithmetic!
        TEST_CASE("Array2 Convert");
        Array2<TypeParam> a = {
            {1, 2},
            {3, 4}
        };
        auto b = Array2<float>::convert<TypeParam>(a);
        TEST_EQUAL(b.ncols(), 2);
        TEST_EQUAL(b.nrows(), 2);
        TEST_EQUAL_OR_NEAR(b(0, 0), 1.0f);
        TEST_EQUAL_OR_NEAR(b(1, 0), 2.0f);
        TEST_EQUAL_OR_NEAR(b(0, 1), 3.0f);
        TEST_EQUAL_OR_NEAR(b(1, 1), 4.0f);
    } else {
        // You can optionally print that the test was skipped:
        std::cout << "[SKIPPED] Array2 Convert for non-arithmetic type: " << type_name<TypeParam>() << "\n";
    }
}

TYPED_TEST(Array2Test, ImportMethod)
{
    if constexpr (std::is_arithmetic_v<TypeParam>) {
        // This block is only compiled if TypeParam is arithmetic!
        TEST_CASE("Array2 Import");

        Array2<int> a(2, 2);
        a(0, 0) = 1;
        a(1, 0) = 2;
        a(0, 1) = 3;
        a(1, 1) = 4;

        Array2<TypeParam> b;
        b.import(a);
        TEST_EQUAL(b.ncols(), 2);
        TEST_EQUAL(b.nrows(), 2);
        TEST_EQUAL_OR_NEAR(b(0, 0), TypeParam(1));
        TEST_EQUAL_OR_NEAR(b(1, 0), TypeParam(2));
        TEST_EQUAL_OR_NEAR(b(0, 1), TypeParam(3));
        TEST_EQUAL_OR_NEAR(b(1, 1), TypeParam(4));
    } else {
        // You can optionally print that the test was skipped:
        std::cout << "[SKIPPED] Array2 Import for non-arithmetic type: " << type_name<TypeParam>() << "\n";
    }
}

TEST(Array2Test, ZeroSize)
{
    TEST_CASE("Array2 Zero Size Test");
    Array2<int> arr(0, 0);
    TEST_EQUAL(arr.xsize(), 0);
    TEST_EQUAL(arr.ysize(), 0);
    TEST_EQUAL(arr.size(), 0);

    TEST_THROW(arr(0, 0), std::out_of_range);
}

TEST(Array2Test, ShiftTest)
{
    TEST_CASE("Array2 Shift Test");
    Array2<int> arr { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
    arr.print();
    std::cout << "arr:\n"
              << arr;
    auto shifted_arr { arr.shifted({ -1, 1 }) };
    std::cout << "shifted arr:\n"
              << shifted_arr;
    auto row { shifted_arr.get_row(1) };
    std::cout << "row[1]=" << row << "\n";
    TEST_EQUAL(row, std::vector<int>({ 2, 3, 0 }));
    auto col { shifted_arr.get_col(1) };
    std::cout << "col[1]=" << col << "\n";
    TEST_EQUAL(col, std::vector<int>({ 0, 3, 6 }));
}

TEST(Array2Test, RangeTest)
{
    TEST_CASE("Array2 Range Test");
    Array_base<int> arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    Array2<int> arr2 { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
    arr2.print();
    std::cout << "arr2:\n"
              << arr2 << "\n";
    TEST_CHECK(std::ranges::range<Array_base<int>>);
    TEST_CHECK(std::ranges::range<Array2<int>>);
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
}

TYPED_TEST(Array2Test, FillBenchmark)
{
    TEST_CASE("Array2 Fill Benchmark");

    constexpr int rows = 2000;
    constexpr int cols = 2000;

    std::cout << "\n=== Performance Benchmark: Array2 vs std::vector ===\n";
        MEASURE_TIME("Array2 fill time", {
        Array2<TypeParam> arr(rows, cols);
        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                arr(x, y) = static_cast<TypeParam>(x + y);
            }
        }
    });

    MEASURE_TIME("std::vector fill time", {
        std::vector<TypeParam> vec(rows * cols);
        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                vec[y * cols + x] = static_cast<TypeParam>(x + y);
            }
        }
    });
}

int array2_test(int /*argc*/, char* /*argv*/[])
{
    RUN_TEST(Array2Test, ConstructionTest);
    RUN_TEST(Array2Test, CopyTest);
    RUN_TEST(Array2Test, MoveTest);
    RUN_TYPED_TEST(Array2Test, FillTest);
    RUN_TEST(Array2Test, ResizeTest);
    RUN_TEST(Array2Test, IndexTest);
    RUN_TEST(Array2Test, BoundsTest);
    RUN_TEST(Array2Test, ZeroSize);
    RUN_TYPED_TEST(Array2Test, ConvertMethod);
    RUN_TYPED_TEST(Array2Test, ImportMethod);
    RUN_TEST(Array2Test, ExceptionTest);

    RUN_TYPED_TEST(Array2Test, AdditionOperator);
    RUN_TYPED_TEST(Array2Test, SubtractionOperator);
    RUN_TYPED_TEST(Array2Test, MultiplicationOperator);
    RUN_TYPED_TEST(Array2Test, DivisionOperator);
    RUN_TYPED_TEST(Array2Test, ScalarAddition);
    RUN_TYPED_TEST(Array2Test, ScalarSubtraction);
    RUN_TYPED_TEST(Array2Test, ScalarMultiplication);
    RUN_TYPED_TEST(Array2Test, ScalarDivision);
    RUN_TEST(Array2Test, ShiftTest);
    RUN_TEST(Array2Test, RangeTest);

    RUN_TYPED_TEST(Array2Test, FillBenchmark);
    
    Test::summary();
    return 0;
}
