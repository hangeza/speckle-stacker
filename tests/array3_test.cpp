#include "array2.h"

int array3_test(int argc, char* argv[])
{
    smip::Array2<int> arr { { 1, 2, 3}, { 4, 5, 6}, { 7, 8, 9} };
    arr.print();
    std::cout << "arr:\n" << arr;
    auto shifted_arr { arr.shifted({-1,1}) };
    std::cout << "shifted arr:\n" << shifted_arr;
    auto row { shifted_arr.get_row(1) };
    std::cout << "row[1]="<<row << "\n";
    assert( (row == std::vector<int>{ 2, 3, 0}) );
    auto col { shifted_arr.get_col(1) };
    std::cout << "col[1]="<<col << "\n";
    assert( (col == std::vector<int>{ 0, 3, 6}) );
    return 0;
}
