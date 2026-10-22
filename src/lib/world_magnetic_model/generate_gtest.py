#!/usr/bin/env python3
############################################################################
#
#   Copyright (c) 2020-2023 PX4 Development Team. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
# 3. Neither the name PX4 nor the names of its contributors may be
#    used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
# AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
############################################################################

ಆಮದು json
ಆಮದು sys
ಆಮದು urllib.request

SAMPLING_RES = 5
SAMPLING_MIN_LAT = -50
SAMPLING_MAX_LAT = 60
SAMPLING_MIN_LON = -180
SAMPLING_MAX_LON = 180

ಶೀರ್ಷಿಕೆ = """/****************************************************************************
 *
 *   Copyright (c) 2020-2024 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#include <gtest/gtest.h>
#include <math.h>
#include <mathlib/mathlib.h>

#include "geo_mag_declination.h"
"""

ಮುದ್ರಿಸು(ಶೀರ್ಷಿಕೆ)

ಮುದ್ರಿಸು('')

# Declination
params = urllib.parse.urlencode({'lat1': 0, 'lat2': 0, 'lon1': 0, 'lon2': 0, 'latStepSize': 1, 'lonStepSize': 1, 'magneticComponent': 'd', 'resultFormat': 'json'})
ಕೀಲಿ=sys.argv[1] # NOAA key (https://www.ngdc.noaa.gov/geomag/CalcSurvey.shtml)
f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfgrid?key=%s&%s" % (ಕೀಲಿ, params))
ದತ್ತಾಂಶ = json.loads(f.ಪಠ())


ಮುದ್ರಿಸು('TEST(GeoLookupTest, declination)\n{')
ಗಾಗಿ latitude ಒಳಗೆ ಶ್ರೇಣಿ(SAMPLING_MIN_LAT, SAMPLING_MAX_LAT+1, SAMPLING_RES):
    params = urllib.parse.urlencode({'lat1': latitude, 'lat2': latitude, 'lon1': SAMPLING_MIN_LON, 'lon2': SAMPLING_MAX_LON, 'latStepSize': 1, 'lonStepSize': SAMPLING_RES, 'magneticComponent': 'd', 'resultFormat': 'json'})
    f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfgrid?key=%s&%s" % (ಕೀಲಿ, params))
    ದತ್ತಾಂಶ = json.loads(f.ಪಠ())

    ಗಾಗಿ p ಒಳಗೆ ದತ್ತಾಂಶ['result']:
        error_deg = 1.0

        # why is this area worse?
        ಆದರೆ (-45 <= p['latitude'] <= -44) ಮತ್ತು (100 <= p['longitude'] <= 120):
            error_deg = 1.8

        ಮುದ್ರಿಸು('\tEXPECT_NEAR(get_mag_declination_degrees({}, {}), {:.1f}, {:.2f} + {});'.format(p['latitude'], p['longitude'], p['declination'], p['declination_uncertainty'], error_deg))
ಮುದ್ರಿಸು('}')

ಮುದ್ರಿಸು('')

ಮುದ್ರಿಸು('TEST(GeoLookupTest, inclination)\n{')
ಗಾಗಿ latitude ಒಳಗೆ ಶ್ರೇಣಿ(SAMPLING_MIN_LAT, SAMPLING_MAX_LAT+1, SAMPLING_RES):
    params = urllib.parse.urlencode({'lat1': latitude, 'lat2': latitude, 'lon1': SAMPLING_MIN_LON, 'lon2': SAMPLING_MAX_LON, 'latStepSize': 1, 'lonStepSize': SAMPLING_RES, 'magneticComponent': 'i', 'resultFormat': 'json'})
    f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfgrid?key=%s&%s" % (ಕೀಲಿ, params))
    ದತ್ತಾಂಶ = json.loads(f.ಪಠ())

    ಗಾಗಿ p ಒಳಗೆ ದತ್ತಾಂಶ['result']:
        error = 1.2

        # why is this area worse?
        ಆದರೆ (-45 <= p['latitude'] <= -44) ಮತ್ತು (100 <= p['longitude'] <= 120):
            error_deg = 1.5

        ಮುದ್ರಿಸು('\tEXPECT_NEAR(get_mag_inclination_degrees({}, {}), {:.1f}, {:.2f} + {});'.format(p['latitude'], p['longitude'], p['inclination'], p['inclination_uncertainty'], error))
ಮುದ್ರಿಸು('}')

ಮುದ್ರಿಸು('')

ಮುದ್ರಿಸು('TEST(GeoLookupTest, strength)\n{')
ಗಾಗಿ latitude ಒಳಗೆ ಶ್ರೇಣಿ(SAMPLING_MIN_LAT, SAMPLING_MAX_LAT+1, SAMPLING_RES):
    params = urllib.parse.urlencode({'lat1': latitude, 'lat2': latitude, 'lon1': SAMPLING_MIN_LON, 'lon2': SAMPLING_MAX_LON, 'latStepSize': 1, 'lonStepSize': SAMPLING_RES, 'magneticComponent': 'f', 'resultFormat': 'json'})
    f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfgrid?key=%s&%s" % (ಕೀಲಿ, params))
    ದತ್ತಾಂಶ = json.loads(f.ಪಠ())

    ಗಾಗಿ p ಒಳಗೆ ದತ್ತಾಂಶ['result']:
        error = 0.01

        # why is this area worse?
        ಆದರೆ (-45 <= p['latitude'] <= -35):
            error = 0.017

        ಮುದ್ರಿಸು('\tEXPECT_NEAR(get_mag_strength_tesla({}, {}) * 1e9, {:.0f}, {:.0f} + {:.0f});'.format(p['latitude'], p['longitude'], p['totalintensity'], p['totalintensity_uncertainty'], p['totalintensity'] * error))
ಮುದ್ರಿಸು('}')
