#include <cstdio>
#include "smip_export.h"

extern "C" SMIP_EXPORT void smip_hello() {
    std::puts("Hello from SMIP!");
}
