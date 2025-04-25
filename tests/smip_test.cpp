#include "array2.h"
#include <iostream>
#include <filesystem>

int smip_test(int /*argc*/, char* /*argv*/[])
{
    std::cout << "Running from: " << std::filesystem::current_path() << "\n";
    std::cout << "Checking for DLL: libsmip.dll = " 
              << (std::filesystem::exists("libsmip.dll") ? "FOUND" : "NOT FOUND") 
              << std::endl;
    return 0;
}
