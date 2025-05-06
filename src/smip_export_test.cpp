#include "smip_export.h"
#include <cstdio>

extern "C" SMIP_EXPORT void smip_hello()
{
    std::puts("Hello from SMIP!");
}
