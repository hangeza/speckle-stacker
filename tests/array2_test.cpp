#include "array2.h"

int array2_test(int argc, char* argv[])
{
    smip::Array2<int> arr { { 1, 2, 3}, { 4, 5, 6}, { 7, 8, 9} };
    arr.print();
    auto shifted_arr { arr.shifted({-1,1}) };
    auto row { shifted_arr.get_row(1) };
    //assert( (row == std::vector<int>{ 2, 3, 0}) );
    //auto col { shifted_arr.get_col(1) };
    //assert( (col == std::vector<int>{ 0, 3, 6}) );
    return 0;
}
