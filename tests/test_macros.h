#pragma once
#include <iostream>
#include <exception>
#include <string>
#include <cstdlib>
#include <limits>
#include <type_traits>
#include <cmath>
#include <chrono>
#include "test_type_name.h"


// Helper object to count test results
namespace Test {
    inline int pass_count = 0;
    inline int fail_count = 0;

    template <typename T>
    constexpr double test_tolerance() {
        if constexpr (std::is_integral_v<T>) {
            return double{};
        } else if constexpr (std::is_arithmetic_v<T>) {
            return static_cast<double>(std::numeric_limits<T>::epsilon());
        } else if constexpr (std::numeric_limits<T>::is_specialized) {
            return static_cast<double>(std::numeric_limits<T>::epsilon());
        } else if constexpr (concept_valarray_of_arithmetic<T>) {
            return std::numeric_limits<typename std::decay_t<T>::value_type>::epsilon();
        } else if constexpr (concept_complex<T>) {
            return static_cast<double>(std::numeric_limits<typename std::decay_t<T>::value_type>::epsilon());
        }
    }

    inline void summary() {
        std::cout << "\nTest summary: " << pass_count << " passed, " << fail_count << " failed." << std::endl;
        if (fail_count > 0) {
            std::exit(1);
        }
    }
}


// Macro to define a test case with output
#define TEST_CASE(name) \
    do { \
        std::cout << "\n=== Test Case: " << (name) << " ===\n"; \
    } while (0)

// Macro for a general test check
#define TEST_CHECK(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "[FAILED] " << __FILE__ << ":" << __LINE__ << ": " \
                      << "Test check failed: " << #expr << std::endl; \
            ++Test::fail_count; \
        } else { \
            ++Test::pass_count; \
        } \
    } while (0)

// Macro to check equality
#define TEST_EQUAL(a, b) \
    do { \
        if (!((a) == (b))) { \
            std::cerr << "[FAILED] " << __FILE__ << ":" << __LINE__ << ": " \
                      << "Test equality failed: " << #a << " == " << #b << " (got " << (a) << " vs " << (b) << ")" << std::endl; \
            ++Test::fail_count; \
        } else { \
            ++Test::pass_count; \
        } \
    } while (0)

// Macro to check that an expression throws a given exception
#define TEST_THROW(expr, ex_type) \
    do { \
        bool thrown = false; \
        try { \
            expr; \
        } catch (const ex_type&) { \
            thrown = true; \
        } catch (const std::exception& e) { \
            std::cerr << "[FAILED] " << __FILE__ << ":" << __LINE__ \
                      << ": Test throw failed: " << #expr \
                      << " threw wrong exception type (" << e.what() << ")\n"; \
        } catch (...) { \
            std::cerr << "[FAILED] " << __FILE__ << ":" << __LINE__ \
                      << ": Test throw failed: " << #expr << " threw unknown exception type\n"; \
        } \
        if (!thrown) { \
            std::cerr << "[FAILED] " << __FILE__ << ":" << __LINE__ \
                      << ": Test throw failed: " << #expr \
                      << " did not throw " << #ex_type << "\n"; \
            ++Test::fail_count; \
        } else { \
            ++Test::pass_count; \
        } \
    } while (0)

// Macro to check that two floating-point numbers are close within a tolerance
#define TEST_NEAR(a, b, tol) \
    do { \
        auto _a = (a); \
        auto _b = (b); \
        auto _tol = (tol); \
        if (!((_a > _b - _tol) && (_a < _b + _tol))) { \
            auto diff = (_a > _b) ? (_a - _b) : (_b - _a); \
            std::cerr << "[FAILED] " << __FILE__ << ":" << __LINE__ \
                      << ": Test near failed: " << #a << " ~= " << #b \
                      << " within tolerance " << _tol \
                      << " (got " << _a << " vs " << _b \
                      << ", diff = " << diff << ")\n"; \
            ++Test::fail_count; \
        } else { \
            ++Test::pass_count; \
        } \
    } while (0)
// Define a fixture test case
#define TEST_FIXTURE(fixture_class, name) \
    do { \
        std::cout << "\n=== Test Fixture: " << (name) << " ===\n"; \
        fixture_class _fixture; \
        _fixture.run(); \
    } while (0)

#define TEST(FixtureName, TestName) \
    struct FixtureName##_##TestName { \
        void operator()() const; \
    }; \
    void FixtureName##_##TestName::operator()() const

// --------- Typed Test Infrastructure ----------

// Helper for type lists
template <typename... Ts>
struct type_list {};

// Register the types
#define TYPED_TEST_SUITE(FixtureName, TypeListName) \
    using FixtureName##_types = TypeListName

// Define the typed test
#define TYPED_TEST(FixtureName, TestName) \
    struct FixtureName##_##TestName { \
        template <typename TypeParam> \
        void operator()() const; \
    }; \
    template <typename TypeParam> \
    void FixtureName##_##TestName::operator()() const


// Internal helper: apply a test to all types
// template <typename... Types, typename TestFunc>
// void run_typed_test(TestFunc test_func, type_list<Types...>) {
//     (test_func.template operator()<Types>(), ...);
// }

template <typename... Types, typename TestFunc>
void run_typed_test(TestFunc test_func, type_list<Types...>) {
    ((
        std::cout << "\n--- Running for type: " << type_name<Types>() << "\n",
        test_func.template operator()<Types>()
    ), ...);
}

// Run the typed test
#define RUN_TYPED_TEST(FixtureName, TestName) \
    run_typed_test(FixtureName##_##TestName{}, FixtureName##_types{})

// Run the simple (non-typed) test
#define RUN_TEST(FixtureName, TestName) \
    FixtureName##_##TestName{}()
    
template<typename T>
void test_equal_or_near(const T& a, const T& b, const char* expr_a, const char* expr_b, const char* file, int line) {
    if constexpr (!std::is_integral_v<T>) {
        // Use a tolerance for non-integral types
        constexpr auto tol = Test::test_tolerance<T>();
        double diff = std::abs(a-b);
        if (diff > tol) {
            std::cerr << "[FAILED] " << file << ":" << line << ": "
                      << "Test near failed: " << expr_a << " ~= " << expr_b
                      << " (got " << a << " vs " << b << ", tolerance " << tol << ")" << std::endl;
            ++Test::fail_count;
        } else {
            ++Test::pass_count;
        }
    } else {
        // Exact check for integers
        if (!(a == b)) {
            std::cerr << "[FAILED] " << file << ":" << line << ": "
                      << "Test equality failed: " << expr_a << " == " << expr_b
                      << " (got " << a << " vs " << b << ")" << std::endl;
            ++Test::fail_count;
        } else {
            ++Test::pass_count;
        }
    }
}

// Convenience macro to capture expressions nicely
#define TEST_EQUAL_OR_NEAR(a, b) \
    test_equal_or_near((a), (b), #a, #b, __FILE__, __LINE__)

#define MEASURE_TIME(label, code_block)                         \
    do {                                                        \
        auto start = std::chrono::high_resolution_clock::now(); \
        code_block                                               \
        auto end = std::chrono::high_resolution_clock::now();   \
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); \
        std::cout << (label) << ": " << ms << " ms\n";           \
    } while (0)

