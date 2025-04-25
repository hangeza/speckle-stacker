#include "utility.h"
#include "phasemap.h"

#ifdef _WIN32
extern "C" void __attribute__((dllexport)) __attribute__((visibility("default"))) test_export() {}
#endif

namespace smip {

#ifdef _WIN32
void force_link() {
    test_export();
}
#endif

double get_phase_consistency(const PhaseMapElement& pme)
{
    return pme.consistency;
}

} // namespace smip
