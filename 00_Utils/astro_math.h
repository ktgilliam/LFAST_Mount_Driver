#include <cinttypes>
#include <cmath>
// #include <map>
#include "math_util.h"
#include "libastro.h"

namespace LFAST
{
#define EARTH_ROTATIONS_PER_UT1_DAY 1.002737811906
#define SECONDS_PER_DAY 86400.0

    constexpr double SiderealRate_radpersec = (M_2_PI / SECONDS_PER_DAY) * EARTH_ROTATIONS_PER_UT1_DAY;
    // constexpr double SiderealRate_degpersec = SiderealRate_radpersec * 180.0 / M_PI;
    constexpr double SiderealRate_degpersec = (360.0 / SECONDS_PER_DAY) * EARTH_ROTATIONS_PER_UT1_DAY;
    // const double SiderealRate_radpersec = (0.000072921); //(15.041067 / 3600.0 * M_PI / 180.0)
    // #define SIDEREAL_RATE_DPS 0.004166667
}

INDI::IHorizontalCoordinates HorizontalRates_geocentric(double ha, double dec, double lat);