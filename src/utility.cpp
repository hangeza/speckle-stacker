#include "utility.h"
#include "phasemap.h"

extern "C" SMIP_PUBLIC void test_export() {}

namespace smip {

double get_phase_consistency(const PhaseMapElement& pme)
{
    return pme.consistency;
}

} // namespace smip
