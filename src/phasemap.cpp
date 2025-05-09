#include <cmath>
#include <complex>
#include <cstdlib>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#include "array2.h"
#include "phasemap.h"

namespace smip {

PhaseMapElement::PhaseMapElement(bool a_flag)
    : flag(a_flag)
{
}

PhaseMapElement::PhaseMapElement(bool a_flag, double a_consistency)
    : flag(a_flag)
    , consistency(a_consistency)
{
}

} // namespace smip
