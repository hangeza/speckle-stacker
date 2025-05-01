#include "bispectrum.h"
#include "test_macros.h"
#include "types.h"
#include <complex>
#include <sstream>
#include <stdexcept>

using namespace smip;

using std::complex;
using std::ostringstream;

// Your fixture
template <typename T>
struct BispectrumTest {
    using TypeParam = T;
};

// Define the type list
using TestTypes = type_list<std::complex<float>, std::complex<double>>;

// Register the fixture with the types
TYPED_TEST_SUITE(BispectrumTest, TestTypes);


TEST(BispectrumTest, ConstructionTest) {
    using TypeParam = std::complex<double>;
    
    TEST_CASE("Bispectrum Construction");

    Bispectrum<TypeParam> bs( Bispectrum<TypeParam>::extents{2, 3, 4, 5});
    bs.print();
    TEST_EQUAL(bs.sizes()[0], 3);
    TEST_EQUAL(bs.sizes()[1], 3);
    TEST_EQUAL(bs.sizes()[2], 5);
    TEST_EQUAL(bs.sizes()[3], 5);
    TEST_EQUAL(bs.totalsize(), 3*3*5*5);
    TEST_CHECK(bs.size() > 0);
    TEST_EQUAL_OR_NEAR(bs.get_element(Bispectrum<TypeParam>::s_indices{0,0,0,0}), TypeParam{}); // Default initialized
}

TEST(BispectrumTest, ElementAccess) {
    using TypeParam = std::complex<double>;
    TEST_CASE("Bispectrum Element Access");
    Bispectrum<TypeParam>::extents dims = {3, 3, 3, 3};
    Bispectrum<TypeParam> b(dims);
    Bispectrum<TypeParam>::s_indices idx {-1, 1, -1, 1};
    TypeParam val {42.0, 1.};
    b.put_element(idx, val);
    TEST_EQUAL_OR_NEAR( b.get_element(idx), val );
}

TYPED_TEST(BispectrumTest, ComplexConjugation)
{
    TEST_CASE("Bispectrum Complex Conjugation");
    Bispectrum<TypeParam> B({4,4,4,4});
    TypeParam val {1.0, 2.0};
    B.put_element({-1, 2, -1, 1}, val);
    TEST_EQUAL_OR_NEAR(B.get_element({1, -2, 1, -1}), std::conj(B.get_element({-1, 2, -1, 1})));
}

TEST(BispectrumTest, ZeroSize)
{
    TEST_CASE("Bispectrum Zero Size Test");
    Bispectrum<std::complex<double>> arr;
    TEST_EQUAL(arr.size(), 0);
    TEST_THROW([[maybe_unused]] auto unused = arr.get_element({0, 0, 0, 0}), std::out_of_range);
}

TEST(BispectrumTest, ArithmeticOperators)
{
    using TypeParam = std::complex<double>;
    TEST_CASE("Bispectrum Arithmetic Operators Test");
    Bispectrum<TypeParam>::extents dims = {2,2,2,2};
    Bispectrum<TypeParam> a(dims);
    Bispectrum<TypeParam> b(dims);
    TEST_EQUAL(a.sizes(), b.sizes());

    a.put_element({0,0,0,0}, TypeParam(2.0));
    b.put_element({0,0,0,0}, TypeParam(3.0));

    a += b;
    TEST_EQUAL_OR_NEAR(a.get_element({0,0,0,0}), TypeParam(5.0));

    a -= b;
    TEST_EQUAL_OR_NEAR(a.get_element({0,0,0,0}), TypeParam(2.0));

    a *= b;
    TEST_EQUAL_OR_NEAR(a.get_element({0,0,0,0}), TypeParam(6.0));

    a /= b;
    TEST_EQUAL_OR_NEAR(a.get_element({0,0,0,0}), TypeParam(2.0));
}

TYPED_TEST(BispectrumTest, Element_Multiple_Get)
{
    TEST_CASE("Bispectrum Multiple Elements Get Test");
    typename Bispectrum<TypeParam>::extents dims = {2, 2, 2, 2};
    Bispectrum<TypeParam> b(dims);
    std::vector<typename Bispectrum<TypeParam>::s_indices> indices = {{{0,0,0,0}, {-1,1,-1,1}}};
    b.put_element(indices[0], TypeParam(1.5,1.0));
    b.put_element(indices[1], TypeParam(2.5,-1.0));
    std::vector<TypeParam> result;
    b.get_elements(indices, result);
    TEST_EQUAL(result.size(), 2);
    TEST_EQUAL_OR_NEAR(result[0], TypeParam(1.5,1.0));
    TEST_EQUAL_OR_NEAR(result[1], TypeParam(2.5,-1.0));
}

// #include <sstream>
// #include <fstream>
// #include <cstdio>
// 
// TEST_CASE(IO_Write_And_Read)
// {
//     Bispectrum<double>::extents dims = {2, 2, 2, 2};
//     Bispectrum<double> b(dims);
//     b.put_element({1,1,1,1}, 99.9);
// 
//     const std::string filename = "test_bispectrum_io.dat";
//     b.write_to_file(filename);
// 
//     Bispectrum<double> b2;
//     b2.read_from_file(filename);
// 
//     CHECK_EQUAL(b2.sizes(), dims);
//     CHECK_EQUAL(b2.get_element({1,1,1,1}), 99.9);
// 
//     std::remove(filename.c_str());
// }
// 
// TEST_CASE(Arithmetic_Assignment)
// {
//     Bispectrum<double>::extents dims = {2,2,2,2};
//     Bispectrum<double> a(dims);
//     Bispectrum<double> b(dims);
//     a.put_element({1,1,1,1}, 3.0);
//     b = a;
//     CHECK_EQUAL(b.get_element({1,1,1,1}), 3.0);
// }
// 
// TEST_CASE(Utilities)
// {
//     Bispectrum<double>::extents dims = {2,2,2,2};
//     Bispectrum<double> b(dims);
//     auto idx = b.calc_indices(0);
//     CHECK_EQUAL(idx.size(), 4);
//     CHECK_EQUAL(b.base_sizes(), dims);
// }
// 
// TEST_CASE(FFT_Accumulate_Stub)
// {
//     // Not testing internal logic, just interface
//     Bispectrum<double>::extents dims = {2,2,2,2};
//     Bispectrum<double> b(dims);
//     Array2<std::complex<double>> dummy_fft({2,2});
//     b.accumulate_from_fft(dummy_fft); // ensure it compiles and runs
// }


// TEST(Array2Test, BoundsTest) {
//     TEST_CASE("Array2 Bounds Checking");
// 
//     Array2<int> arr(2, 2);
//     arr(1, 1) = 5;
//     TEST_EQUAL(arr(1, 1),  5);
// 
//     TEST_THROW(arr(2, 0), std::out_of_range); // x out of bounds
//     TEST_THROW(arr(0, 2), std::out_of_range); // y out of bounds
// }
// 
// TEST(Array2Test, CopyTest) {
//     TEST_CASE("Array2 Copy Semantics");
// 
//     Array2<int> arr1(2, 2);
//     arr1(0, 0) = 10;
//     arr1(1, 1) = 20;
// 
//     Array2<int> arr2 = arr1; // Copy constructor
//     TEST_EQUAL(arr2(0, 0), 10);
//     TEST_EQUAL(arr2(1, 1), 20);
// 
//     Array2<int> arr3(1, 1);
//     arr3 = arr1; // Copy assignment
//     TEST_EQUAL(arr3(0, 0), 10);
//     TEST_EQUAL(arr3(1, 1), 20);
// }
// 
// TEST(Array2Test, MoveTest) {
//     TEST_CASE("Array2 Move Semantics");
// 
//     Array2<int> arr1(2, 2);
//     arr1(0, 0) = 7;
//     arr1(1, 1) = 8;
// 
//     Array2<int> arr2 = std::move(arr1); // Move constructor
//     TEST_EQUAL(arr2(0, 0), 7);
//     TEST_EQUAL(arr2(1, 1), 8);
// 
//     Array2<int> arr3(1, 1);
//     arr3 = std::move(arr2); // Move assignment
//     TEST_EQUAL(arr3(0, 0), 7);
//     TEST_EQUAL(arr3(1, 1), 8);
// }
// 
// TEST(Array2Test, ResizeTest)
// {
//     TEST_CASE("Array2 Resize");
//     Array2<int> arr(2, 2);
//     arr(0, 0) = 1;
//     arr(1, 1) = 2;
// 
//     arr = Array2<int>(3, 3, 99);
//     TEST_EQUAL(arr.xsize(), 3);
//     TEST_EQUAL(arr.ysize(), 3);
//     TEST_EQUAL(arr(0, 0), 99);
//     TEST_EQUAL(arr(1, 1), 99);
//     arr = Array2<int>(3, 3);
//     TEST_EQUAL(arr(2, 2), int{}); // New cells default-initialized
// }
// 
// TYPED_TEST(Array2Test, FillTest)
// {
//     TEST_CASE("Array2 Fill");
//     Array2<TypeParam> arr(2, 2);
//     arr = TypeParam{99};
//     TEST_EQUAL_OR_NEAR(arr(0, 0), TypeParam{99});
//     TEST_EQUAL_OR_NEAR(arr(1, 1), TypeParam{99});
// }
// 
// TEST(Array2Test, IndexTest)
// {
//     TEST_CASE("Array2 Index Operator");
//     
//     Array2<int> arr(3, 3);
//     arr(0, 0) = 1;
//     arr(1, 1) = 2;
//     arr(2, 2) = 3;
// 
//     TEST_EQUAL(arr(0, 0), 1);
//     TEST_EQUAL(arr(1, 1), 2);
//     TEST_EQUAL(arr(2, 2), 3);
// }
// 
// TEST(Array2Test, ExceptionTest)
// {
//     TEST_CASE("Array2 ExceptionHandling");
//     Array2<int> a(5, 5);
//     TEST_THROW(a(100, 100), std::out_of_range);
//     Array2<int> b; // empty
//     b.set_at(a.data(),a.size());
//     Array2<int> c(5,4); // differently sized array
//     // assignment to array which carries a reference...should throw
//     TEST_THROW(b=c, std::runtime_error);
// }
// 
// TYPED_TEST(Array2Test, AdditionOperator)
// {
//     TEST_CASE("Array2 Addition Operator");
// 
//     Array2<TypeParam> a = {
//         {1, 2},
//         {3, 4}
//     };
//     Array2<TypeParam> b = {
//         {5, 6},
//         {7, 8}
//     };
//     auto c = a + b;
// 
//     TEST_EQUAL(c.ncols(), 2);
//     TEST_EQUAL(c.nrows(), 2);
//     TEST_EQUAL_OR_NEAR(c(0, 0), TypeParam(6));
//     TEST_EQUAL_OR_NEAR(c(1, 0), TypeParam(8));
//     TEST_EQUAL_OR_NEAR(c(0, 1), TypeParam(10));
//     TEST_EQUAL_OR_NEAR(c(1, 1), TypeParam(12));
// }
// 
// TYPED_TEST(Array2Test, SubtractionOperator)
// {
//     TEST_CASE("Array2 Subtraction Operator");
// 
//     Array2<TypeParam> a = {
//         {5, 6},
//         {7, 8}
//     };
//     Array2<TypeParam> b = {
//         {1, 2},
//         {3, 4}
//     };
//     auto c = a - b;
//     TEST_EQUAL_OR_NEAR(c(0, 0), TypeParam(4));
//     TEST_EQUAL_OR_NEAR(c(1, 0), TypeParam(4));
//     TEST_EQUAL_OR_NEAR(c(0, 1), TypeParam(4));
//     TEST_EQUAL_OR_NEAR(c(1, 1), TypeParam(4));
// }
// 
// TYPED_TEST(Array2Test, MultiplicationOperator)
// {
//     TEST_CASE("Array2 Multiplication Operator");
// 
//     Array2<TypeParam> a = {
//         {1, 2},
//         {3, 4}
//     };
//     Array2<TypeParam> b = {
//         {2, 2},
//         {2, 2}
//     };
//     auto c = a * b;
//     TEST_EQUAL_OR_NEAR(c(0, 0), TypeParam(2));
//     TEST_EQUAL_OR_NEAR(c(1, 0), TypeParam(4));
//     TEST_EQUAL_OR_NEAR(c(0, 1), TypeParam(6));
//     TEST_EQUAL_OR_NEAR(c(1, 1), TypeParam(8));
// }
// 
// TYPED_TEST(Array2Test, DivisionOperator)
// {
//     TEST_CASE("Array2 Division Operator");
//     Array2<TypeParam> a = {
//         {4, 6},
//         {8, 10}
//     };
//     Array2<TypeParam> b = {
//         {2, 3},
//         {4, 5}
//     };
//     auto c = a / b;
//     TEST_EQUAL_OR_NEAR(c(0, 0), TypeParam(2));
//     TEST_EQUAL_OR_NEAR(c(1, 0), TypeParam(2));
//     TEST_EQUAL_OR_NEAR(c(0, 1), TypeParam(2));
//     TEST_EQUAL_OR_NEAR(c(1, 1), TypeParam(2));
// }
// 
// TYPED_TEST(Array2Test, ScalarMultiplication)
// {
//     TEST_CASE("Array2 Scalar Multiplication");
//     Array2<TypeParam> a = {
//         {1, 2},
//         {3, 4}
//     };
//     auto c = a * TypeParam(3);
//     TEST_EQUAL_OR_NEAR(c(0, 0), TypeParam(3));
//     TEST_EQUAL_OR_NEAR(c(1, 0), TypeParam(6));
//     TEST_EQUAL_OR_NEAR(c(0, 1), TypeParam(9));
//     TEST_EQUAL_OR_NEAR(c(1, 1), TypeParam(12));
// }
// 
// TYPED_TEST(Array2Test, ScalarDivision)
// {
//     TEST_CASE("Array2 Scalar Division");
//     Array2<TypeParam> a = {
//         {2, 4},
//         {6, 8}
//     };
//     auto c = a / TypeParam(2);
//     TEST_EQUAL_OR_NEAR(c(0, 0), TypeParam(1));
//     TEST_EQUAL_OR_NEAR(c(1, 0), TypeParam(2));
//     TEST_EQUAL_OR_NEAR(c(0, 1), TypeParam(3));
//     TEST_EQUAL_OR_NEAR(c(1, 1), TypeParam(4));
// }
// 
// TYPED_TEST(Array2Test, ScalarAddition)
// {
//     TEST_CASE("Array2 Scalar Addition");
//     Array2<TypeParam> a = {
//         {1, 2},
//         {3, 4}
//     };
//     auto c = a + TypeParam(5);
//     TEST_EQUAL_OR_NEAR(c(0, 0), TypeParam(6));
//     TEST_EQUAL_OR_NEAR(c(1, 0), TypeParam(7));
//     TEST_EQUAL_OR_NEAR(c(0, 1), TypeParam(8));
//     TEST_EQUAL_OR_NEAR(c(1, 1), TypeParam(9));
// }
// 
// TYPED_TEST(Array2Test, ScalarSubtraction)
// {
//         TEST_CASE("Array2 Scalar Subtraction");
//         Array2<TypeParam> a = {
//         {10, 9},
//         {8, 7}
//     };
//     auto c = a - TypeParam(5);
//     TEST_EQUAL_OR_NEAR(c(0, 0), TypeParam(5));
//     TEST_EQUAL_OR_NEAR(c(1, 0), TypeParam(4));
//     TEST_EQUAL_OR_NEAR(c(0, 1), TypeParam(3));
//     TEST_EQUAL_OR_NEAR(c(1, 1), TypeParam(2));
// }
// 
// TYPED_TEST(Array2Test, ConvertMethod)
// {
//     if constexpr (std::is_arithmetic_v<TypeParam>) {
//         // This block is only compiled if TypeParam is arithmetic!
//         TEST_CASE("Array2 Convert");
//         Array2<TypeParam> a = {
//             {1, 2},
//             {3, 4}
//         };
//         auto b = Array2<float>::convert<TypeParam>(a);
//         TEST_EQUAL(b.ncols(), 2);
//         TEST_EQUAL(b.nrows(), 2);
//         TEST_EQUAL_OR_NEAR(b(0, 0), 1.0f);
//         TEST_EQUAL_OR_NEAR(b(1, 0), 2.0f);
//         TEST_EQUAL_OR_NEAR(b(0, 1), 3.0f);
//         TEST_EQUAL_OR_NEAR(b(1, 1), 4.0f);
//     } else {
//         // You can optionally print that the test was skipped:
//         std::cout << "[SKIPPED] Array2 Convert for non-arithmetic type: " << type_name<TypeParam>() << "\n";
//     }
// }
// 
// TYPED_TEST(Array2Test, ImportMethod)
// {
//     if constexpr (std::is_arithmetic_v<TypeParam>) {
//         // This block is only compiled if TypeParam is arithmetic!
//         TEST_CASE("Array2 Import");
// 
//         Array2<int> a(2, 2);
//         a(0, 0) = 1;
//         a(1, 0) = 2;
//         a(0, 1) = 3;
//         a(1, 1) = 4;
// 
//         Array2<TypeParam> b;
//         b.import(a);
//         TEST_EQUAL(b.ncols(), 2);
//         TEST_EQUAL(b.nrows(), 2);
//         TEST_EQUAL_OR_NEAR(b(0, 0), TypeParam(1));
//         TEST_EQUAL_OR_NEAR(b(1, 0), TypeParam(2));
//         TEST_EQUAL_OR_NEAR(b(0, 1), TypeParam(3));
//         TEST_EQUAL_OR_NEAR(b(1, 1), TypeParam(4));
//     } else {
//         // You can optionally print that the test was skipped:
//         std::cout << "[SKIPPED] Array2 Import for non-arithmetic type: " << type_name<TypeParam>() << "\n";
//     }
// }
// 
// TEST(Array2Test, ZeroSize)
// {
//     TEST_CASE("Array2 Zero Size Test");
//     Array2<int> arr(0, 0);
//     TEST_EQUAL(arr.xsize(), 0);
//     TEST_EQUAL(arr.ysize(), 0);
//     TEST_EQUAL(arr.size(), 0);
// 
//     TEST_THROW(arr(0, 0), std::out_of_range);
// }

// TYPED_TEST(BispectrumTest, FillBenchmark)
// {
//     TEST_CASE("Bispectrum Fill Benchmark");
// 
//     constexpr int rows = 2000;
//     constexpr int cols = 2000;
// 
//     std::cout << "\n=== Performance Benchmark: Array2 vs std::vector ===\n";
//         MEASURE_TIME("Array2 fill time", {
//         Array2<TypeParam> arr(rows, cols);
//         for (int y = 0; y < rows; ++y) {
//             for (int x = 0; x < cols; ++x) {
//                 arr(x, y) = static_cast<TypeParam>(x + y);
//             }
//         }
//     });
// 
//     MEASURE_TIME("std::vector fill time", {
//         std::vector<TypeParam> vec(rows * cols);
//         for (int y = 0; y < rows; ++y) {
//             for (int x = 0; x < cols; ++x) {
//                 vec[y * cols + x] = static_cast<TypeParam>(x + y);
//             }
//         }
//     });
// }

int bispectrum_test(int /*argc*/, char* /*argv*/[])
{
    RUN_TEST(BispectrumTest, ConstructionTest);
    RUN_TEST(BispectrumTest, ZeroSize);
    RUN_TEST(BispectrumTest, ElementAccess);
    RUN_TYPED_TEST(BispectrumTest, ComplexConjugation);
    RUN_TEST(BispectrumTest, ArithmeticOperators);
    RUN_TYPED_TEST(BispectrumTest, Element_Multiple_Get);
    
//     RUN_TEST(Array2Test, CopyTest);
//     RUN_TEST(Array2Test, MoveTest);
//     RUN_TYPED_TEST(Array2Test, FillTest);
//     RUN_TEST(Array2Test, ResizeTest);
//     RUN_TEST(Array2Test, IndexTest);
//     RUN_TEST(Array2Test, BoundsTest);
//     RUN_TEST(Array2Test, ZeroSize);
//     RUN_TYPED_TEST(Array2Test, ConvertMethod);
//     RUN_TYPED_TEST(Array2Test, ImportMethod);
//     RUN_TEST(Array2Test, ExceptionTest);
// 
//     RUN_TYPED_TEST(Array2Test, AdditionOperator);
//     RUN_TYPED_TEST(Array2Test, SubtractionOperator);
//     RUN_TYPED_TEST(Array2Test, MultiplicationOperator);
//     RUN_TYPED_TEST(Array2Test, DivisionOperator);
//     RUN_TYPED_TEST(Array2Test, ScalarAddition);
//     RUN_TYPED_TEST(Array2Test, ScalarSubtraction);
//     RUN_TYPED_TEST(Array2Test, ScalarMultiplication);
//     RUN_TYPED_TEST(Array2Test, ScalarDivision);
// 
//     RUN_TYPED_TEST(Array2Test, FillBenchmark);
    
    Test::summary();
    return 0;
}
