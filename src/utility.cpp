#include "utility.h"
#include "phasemap.h"

namespace smip {

double get_phase_consistency(const PhaseMapElement& pme)
{
    return pme.consistency;
}

} // namespace smip
