#ifndef SMIP_GLOBAL_H
#define SMIP_GLOBAL_H

#include <chrono>
#include <memory>
#include <string>

#if defined _WIN32 || defined __CYGWIN__
//  Microsoft
    #ifdef __GNUC__
        #define EXPORT __attribute__((dllexport))
        #define IMPORT __attribute__((dllimport))
    #else
    //_MSC_VER
        #define EXPORT __declspec(dllexport)
        #define IMPORT __declspec(dllimport)
    #endif
#elif defined(__GNUC__)
//  GCC
    #define EXPORT __attribute__((visibility("default")))
    #define IMPORT
    #define HIDDEN __attribute__((visibility("hidden")))
#else
//  do nothing and hope for the best?
    #define EXPORT
    #define IMPORT
    #pragma warning Unknown dynamic link import / export semantics.
#endif

#cmakedefine SMIP_COMPILING

#ifdef SMIP_COMPILING
#define SMIP_PUBLIC EXPORT
#else
#define SMIP_PUBLIC IMPORT
#endif

//#define BOOST_ENABLE_ASSERT_DEBUG_HANDLER

namespace smip::Version {
// clang-format off
constexpr int major {@PROJECT_VERSION_MAJOR@};
constexpr int minor {@PROJECT_VERSION_MINOR@};
constexpr int patch {@PROJECT_VERSION_PATCH@};
constexpr const char* additional {"@PROJECT_VERSION_ADDITIONAL@"};
// clang-format on

[[nodiscard]] auto string() -> std::string;

} // namespace smip::Version

#endif // SMIP_GLOBAL_H
