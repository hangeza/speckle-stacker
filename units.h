#pragma once
#include "global.h"
#include "constants.h"

namespace smip {

namespace units {
static constexpr double giga = 1.0e9;
static constexpr double mega = 1.0e6;
static constexpr double kilo = 1.0e3;

static constexpr double centi = 1.0e-2;
static constexpr double milli = 1.0e-3;
static constexpr double micro = 1.0e-6;
static constexpr double nano  = 1.0e-9;

static constexpr double radian = 1.0;
static constexpr double degree = (constants::pi / 180.0) * radian;

static constexpr double meter     = 1.0;
static constexpr double kilometer = kilo * meter;

static constexpr double nanosecond = 1.0;
static constexpr double second     = nanosecond / nano;
} // namespace units

} // namespace smip
