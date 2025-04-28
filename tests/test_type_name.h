#pragma once

#include <iostream>
#include <typeinfo>

#if defined(__GNUC__) || defined(__clang__)
    #include <cxxabi.h>
    #include <memory>
#endif

// General type name function
template <typename T>
std::string type_name() {
#if defined(__GNUC__) || defined(__clang__)
    int status = 0;
    std::unique_ptr<char[], void(*)(void*)> res {
        abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status),
        std::free
    };
    return (status == 0) ? res.get() : typeid(T).name();
#else
    // On MSVC, typeid gives readable name
    return typeid(T).name();
#endif
}
