#include "bispectrum.h"
#include "test_macros.h"
#include "types.h"
#include <complex>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cstdio>

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
    Bispectrum<TypeParam>::extents dims = {3, 4, 5, 6};
    Bispectrum<TypeParam> b(dims);
    b.print();
    Bispectrum<TypeParam>::s_indices idx {-1, 1, -1, 1};
    TypeParam testval {42.0, 1.};
    b.put_element(idx, testval);
    TEST_EQUAL_OR_NEAR( b.get_element(idx), testval );
    for (auto it { b.begin() }; it != b.end(); ++it) {
        auto indices = b.calc_indices(std::distance(b.begin(), it));
        testval = TypeParam(std::distance(b.begin(), it),0.);
        *it = testval;
        auto val = b.get_element(indices);
        TEST_EQUAL_OR_NEAR( val, testval );
    }
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

TYPED_TEST(BispectrumTest, FillTest)
{
    TEST_CASE("Bispectrum Fill");
    typename Bispectrum<TypeParam>::extents dims = {3, 4, 5, 6};
    Bispectrum<TypeParam> b(dims);
    TypeParam val{42.0,-1.};
    std::fill(b.begin(), b.end(), val);
    TEST_EQUAL_OR_NEAR(b.get_element({0,0,0,0}), val);
    TEST_EQUAL_OR_NEAR(b[b.size()-1], val);
}

TYPED_TEST(BispectrumTest, IO_Write_And_Read)
{
    TEST_CASE("Bispectrum File I/O");
    typename Bispectrum<TypeParam>::extents dims = {2, 2, 2, 2};
    Bispectrum<TypeParam> b(dims);
    dims = b.sizes();
    b.put_element({-1,1,-1,1}, TypeParam(99.9, -1.));

    const std::string filename = "test_bispectrum_io.dat";
    b.write_to_file(filename);

    Bispectrum<TypeParam> b2;
    b2.read_from_file(filename);
    TEST_EQUAL(b2.sizes(), dims);
    TEST_EQUAL_OR_NEAR(b2.get_element({-1,1,-1,1}), TypeParam(99.9, -1.));

    std::remove(filename.c_str());
}

TYPED_TEST(BispectrumTest, FillBenchmark)
{
    TEST_CASE("Bispectrum Fill Benchmark");

    typename Bispectrum<TypeParam>::extents dims = {100, 100, 20, 20};
    
    std::cout << "\n=== Performance Benchmark: Bispectrum vs std::vector ===\n";
    MEASURE_TIME("Bispectrum fill time", {
        Bispectrum<TypeParam> a(dims);
        for (auto it { a.begin() }; it != a.end(); ++it) {
            auto indices = a.calc_indices(std::distance(a.begin(), it));
            auto testval = TypeParam(std::distance(a.begin(), it), -1.);
            a.put_element(indices, testval);
        }
    });

    MEASURE_TIME("std::vector fill time", {
        std::vector<TypeParam> vec(dims.product());
        for (std::size_t i = 0; i < dims.product(); ++i) {
            vec[i] = static_cast<TypeParam>(i);
        }
    });
}

/// @todo implement more, comprehensive tests
int bispectrum_test(int /*argc*/, char* /*argv*/[])
{
    RUN_TEST(BispectrumTest, ConstructionTest);
    RUN_TEST(BispectrumTest, ZeroSize);
    RUN_TEST(BispectrumTest, ElementAccess);
    RUN_TYPED_TEST(BispectrumTest, ComplexConjugation);
    RUN_TEST(BispectrumTest, ArithmeticOperators);
    RUN_TYPED_TEST(BispectrumTest, Element_Multiple_Get);
    RUN_TYPED_TEST(BispectrumTest, FillTest);
    RUN_TYPED_TEST(BispectrumTest, IO_Write_And_Read);

    RUN_TYPED_TEST(BispectrumTest, FillBenchmark);
    
    Test::summary();
    return 0;
}
