#pragma once

#include "smip_export.h"

namespace smip {

template <typename T>
class Array2;

struct SMIP_EXPORT PhaseMapElement {
    PhaseMapElement() = default;
    PhaseMapElement(bool a_flag);
    PhaseMapElement(bool a_flag, double a_consistency);
    double operator()() const { return consistency; }
    operator double() const { return consistency; }
    bool flag { false };
    double consistency { 0. };
};

typedef Array2<PhaseMapElement> PhaseMap;

} // namespace smip
