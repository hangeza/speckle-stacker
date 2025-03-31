#include "array2.h"

int array2_test(int argc, char* argv[])
{
    smip::Array2<int> arr { { 1, 2, 3}, { 4, 5, 6}, { 7, 8, 9} };
    arr.print();
    return 0;
    std::cout << "arr:\n" << arr;
    auto row { arr.get_row(1) };
    std::cout << "row[1]="<<row << "\n";
    assert( (row == std::vector<int>{ 4, 5, 6}) );
    auto col { arr.get_col(1) };
    std::cout << "col[1]="<<col << "\n";
    assert( (col == std::vector<int>{ 2, 5, 8}) );
    return 0;
}
