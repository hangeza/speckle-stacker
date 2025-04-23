#include "utility.h"
#include "phasemap.h"

#if defined _WIN32
extern "C" void __declspec(dllexport) test_export() {}
#endif

namespace smip {

double get_phase_consistency(const PhaseMapElement& pme)
{
    return pme.consistency;
}

} // namespace smip
