#pragma once
#include <iostream>
#include <exception>
#include <string>
#include <cstdlib>
#include <limits>
#include <type_traits>
#include "test_type_name.h"

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

// Run the test
#define RUN_TYPED_TEST(FixtureName, TestName) \
    run_typed_test(FixtureName##_##TestName{}, FixtureName##_types{})

// Helper object to count test results
namespace Test {
    inline int pass_count = 0;
    inline int fail_count = 0;

    template <typename T>
    constexpr T test_tolerance() {
        if constexpr (std::numeric_limits<T>::is_specialized) {
            return std::numeric_limits<T>::epsilon();
        } else if constexpr (std::is_arithmetic_v<T>) {
            return std::numeric_limits<T>::epsilon();
        } else if constexpr (concept_valarray_of_arithmetic<T>) {
            return std::numeric_limits<typename std::decay_t<T>::value_type>::epsilon();
        } else if constexpr (concept_complex<T>) {
            return std::numeric_limits<typename std::decay_t<T>::value_type>::epsilon();
        }
    }

    inline void summary() {
        std::cout << "\nTest summary: " << pass_count << " passed, " << fail_count << " failed." << std::endl;
        if (fail_count > 0) {
            std::exit(1);
        }
    }
}

