#include "array2.h"
#include "testconfig.h" // for TEST_DATAFILE etc.
#include <iostream>
#include <cassert>
#include <stdexcept>

using namespace smip;

int array5_test(int /*argc*/, char* /*argv*/[])
{
    try {
        std::cout << "Running array5_test...\n";

        // Basic construction and size check
        Array2<int> a(3, 4); // 3 rows, 4 cols
        assert(a.xsize() == 3);
        assert(a.ysize() == 4);
        assert(a.size() == 12);

        // Fill with values
        for (size_t i = 0; i < a.xsize(); ++i)
            for (size_t j = 0; j < a.ysize(); ++j)
                a(i, j) = static_cast<int>(i * 10 + j);

        // Check values
        for (size_t i = 0; i < a.xsize(); ++i)
            for (size_t j = 0; j < a.ysize(); ++j)
                assert(a(i, j) == static_cast<int>(i * 10 + j));

        // Test at()
        assert(a(1, 2) == 12);

//         Array2<int> b(); // empty
//         b = a;
//         assert(b.xsize() == 3);
//         assert(b.ysize() == 4);
//         assert(b.size() == 12);

        std::cout << "array5_test passed.\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << '\n';
        return 1;
    }
}
