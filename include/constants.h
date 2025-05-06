#pragma once

#include "global.h"
#include "types.h"
#include <numbers>

namespace smip {

namespace constants {
    template <concept_floating T = double>
    static constexpr T pi = std::numbers::pi_v<T>;
    template <concept_floating T = double>
    static constexpr T c_0 { static_cast<T>(299'792'458.0) };
    template <concept_floating T = double>
    static constexpr T c_2pi { pi<T> * 2 }; /* 2*pi */
    template <concept_floating T = double>
    static constexpr T c_sqrt_2pi { static_cast<T>(2.5070831590532E+00) }; /* sqrt(2*pi) */
} // namespace consts

} // namespace smip
