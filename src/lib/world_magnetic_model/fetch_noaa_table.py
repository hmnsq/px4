#!/usr/bin/env python3
############################################################################
#
#   Copyright (c) 2020-2024 PX4 Development Team. All rights reserved.
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

ಆಮದು math
ಆಮದು numpy
ಆಮದು json
ಆಮದು statistics
ಆಮದು sys
ಆಮದು urllib.request

SAMPLING_RES = 10
SAMPLING_MIN_LAT = -90
SAMPLING_MAX_LAT = 90
SAMPLING_MIN_LON = -180
SAMPLING_MAX_LON = 180

ವ್ಯಾಖ್ಯಾನ constrain(n, nmin, nmax):
    ಹಿಂತಿರುಗಿ max(min(nmin, n), nmax)

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
"""

ಕೀಲಿ=sys.argv[1] # NOAA key (https://www.ngdc.noaa.gov/geomag/CalcSurvey.shtml)

ಮುದ್ರಿಸು(ಶೀರ್ಷಿಕೆ)

ಮುದ್ರಿಸು('#include <stdint.h>\n')

LAT_DIM=ಪೂರ್ಣಾಂಕ((SAMPLING_MAX_LAT-SAMPLING_MIN_LAT)/SAMPLING_RES)+1
LON_DIM=ಪೂರ್ಣಾಂಕ((SAMPLING_MAX_LON-SAMPLING_MIN_LON)/SAMPLING_RES)+1

ಮುದ್ರಿಸು('static constexpr float SAMPLING_RES = {}'.format(SAMPLING_RES) + ';')
ಮುದ್ರಿಸು('static constexpr float SAMPLING_MIN_LAT = {}'.format(SAMPLING_MIN_LAT) + ';')
ಮುದ್ರಿಸು('static constexpr float SAMPLING_MAX_LAT = {}'.format(SAMPLING_MAX_LAT) + ';')
ಮುದ್ರಿಸು('static constexpr float SAMPLING_MIN_LON = {}'.format(SAMPLING_MIN_LON) + ';')
ಮುದ್ರಿಸು('static constexpr float SAMPLING_MAX_LON = {}'.format(SAMPLING_MAX_LON) + ';')
ಮುದ್ರಿಸು('')
ಮುದ್ರಿಸು('static constexpr int LAT_DIM = {}'.format(LAT_DIM) + ';')
ಮುದ್ರಿಸು('static constexpr int LON_DIM = {}'.format(LON_DIM) + ';')
ಮುದ್ರಿಸು('\n')

ಮುದ್ರಿಸು('// *INDENT-OFF*\n\n\n')



# build the world magnetic model dictionary
world_magnitude_model = {} # lat/lon dictionary with grid result

params = urllib.parse.urlencode({'lat1': 0, 'lon1': 0, 'resultFormat': 'json'})
f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfwmm?key=%s&%s" % (ಕೀಲಿ, params))
ದತ್ತಾಂಶ = json.loads(f.ಪಠ())

world_magnitude_model_units = ದತ್ತಾಂಶ['units']

ಗಾಗಿ latitude ಒಳಗೆ ಶ್ರೇಣಿ(SAMPLING_MIN_LAT, SAMPLING_MAX_LAT+1, SAMPLING_RES):
    world_magnitude_model[latitude] = {}

    ಗಾಗಿ longitude ಒಳಗೆ ಶ್ರೇಣಿ(SAMPLING_MIN_LON, SAMPLING_MAX_LON+1, SAMPLING_RES):
        params = urllib.parse.urlencode({'lat1': latitude, 'lon1': longitude, 'lon2': SAMPLING_MAX_LON, 'resultFormat': 'json'})
        f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfwmm?key=%s&%s" % (ಕೀಲಿ, params))
        ದತ್ತಾಂಶ = json.loads(f.ಪಠ())
        #print(json.dumps(data, indent = 4)) # debugging

        world_magnitude_model[latitude][longitude] = ದತ್ತಾಂಶ['result'][0]
        #print(world_magnitude_model[latitude][longitude])


ವ್ಯಾಖ್ಯಾನ print_wmm_ಕೋಷ್ಟಕ(ಕೀಲಿ_name):

    ಮೌಲ್ಯ_min = ದಶಮಾಂಶ('inf')
    ಮೌಲ್ಯ_min_lat_lon = ()

    ಮೌಲ್ಯ_max = ದಶಮಾಂಶ('-inf')
    ಮೌಲ್ಯ_max_lat_lon = ()

    ಗಾಗಿ latitude, lat_ಅಡ್ಡಸಾಲು ಒಳಗೆ world_magnitude_model.ವಸ್ತುಗಳು_2():
        #print(latitude, lat_row)
        ಗಾಗಿ longitude, ಫಲಿತಾಂಶ ಒಳಗೆ lat_ಅಡ್ಡಸಾಲು.ವಸ್ತುಗಳು_2():
            #print(result)

            ಮೌಲ್ಯ = ದಶಮಾಂಶ(ಫಲಿತಾಂಶ[ಕೀಲಿ_name])

            ಆದರೆ (ಮೌಲ್ಯ > ಮೌಲ್ಯ_max):
                ಮೌಲ್ಯ_max = ಮೌಲ್ಯ
                ಮೌಲ್ಯ_max_lat_lon = (latitude, longitude)

            ಆದರೆ (ಮೌಲ್ಯ < ಮೌಲ್ಯ_min):
                ಮೌಲ್ಯ_min = ಮೌಲ್ಯ
                ಮೌಲ್ಯ_min_lat_lon = (latitude, longitude)

    # scale the values to fit into int16_t
    ಮೌಲ್ಯ_scale_max = abs(numpy.iinfo(numpy.int16).max) / abs(ಮೌಲ್ಯ_max)
    ಮೌಲ್ಯ_scale_min = abs(numpy.iinfo(numpy.int16).min) / abs(ಮೌಲ್ಯ_min)
    ಮೌಲ್ಯ_scale = min(ಮೌಲ್ಯ_scale_max, ಮೌಲ್ಯ_scale_min)

    units_str = world_magnitude_model_units[ಕೀಲಿ_name].ವಿಭಜಿಸು(' ')[0]

    # print the table
    ಮುದ್ರಿಸು('// Magnetic {} data in {:.4g} {}'.format(ಕೀಲಿ_name, 1.0 / ಮೌಲ್ಯ_scale, units_str))
    ಮುದ್ರಿಸು('// Model: {},'.format(ದತ್ತಾಂಶ['model']))
    ಮುದ್ರಿಸು('// Version: {},'.format(ದತ್ತಾಂಶ['version']))
    ಮುದ್ರಿಸು('// Date: {},'.format(ದತ್ತಾಂಶ['result'][0]['date']))
    ಮುದ್ರಿಸು('static constexpr const int16_t {}_table[{}][{}]'.format(ಕೀಲಿ_name, LAT_DIM, LON_DIM) + " {")
    ಮುದ್ರಿಸು('\t//    LONGITUDE: ', ಕೊನೆ='')
    ಗಾಗಿ l ಒಳಗೆ ಶ್ರೇಣಿ(SAMPLING_MIN_LON, SAMPLING_MAX_LON+1, SAMPLING_RES):
        ಮುದ್ರಿಸು('{0:6d},'.format(l), ಕೊನೆ='')
    ಮುದ್ರಿಸು('')

    ಗಾಗಿ latitude, lat_ಅಡ್ಡಸಾಲು ಒಳಗೆ world_magnitude_model.ವಸ್ತುಗಳು_2():
        ಮುದ್ರಿಸು('\t/* LAT: {0:3d} */'.format(latitude) + ' { ', ಕೊನೆ='')
        latitude_blackout_zone = ಸುಳ್ಳು

        ಗಾಗಿ longitude, ಫಲಿತಾಂಶ ಒಳಗೆ lat_ಅಡ್ಡಸಾಲು.ವಸ್ತುಗಳು_2():

            ಮೌಲ್ಯ = ದಶಮಾಂಶ(ಫಲಿತಾಂಶ[ಕೀಲಿ_name])

            # value scaled to fit into int16_t
            ಮೌಲ್ಯ_int = ಪೂರ್ಣಾಂಕ(round(ಮೌಲ್ಯ * ಮೌಲ್ಯ_scale))
            ಮುದ್ರಿಸು('{0:6d},'.format(ಮೌಲ್ಯ_int), ಕೊನೆ='')

            # blackout warning at this latitude
            ಪ್ರಯತ್ನಿಸು:
                ಆದರೆ ಫಲಿತಾಂಶ['warning']:
                    latitude_blackout_zone = ನಿಜ

            ಅಪವಾದ:
                ದಾಟು

        ಆದರೆ latitude_blackout_zone:
            ಮುದ್ರಿಸು(' }, // WARNING! black out zone')
        ಇಲ್ಲದಿದ್ದರೆ:
            ಮುದ್ರಿಸು(' },')

    ಮುದ್ರಿಸು("};")

    ಮುದ್ರಿಸು('static constexpr float WMM_{}_SCALE_TO_{} = {:.9g}f;'.format(ಕೀಲಿ_name.ಊರ್ಧ್ವ(), units_str.ಊರ್ಧ್ವ(), 1.0 / ಮೌಲ್ಯ_scale))
    ಮುದ್ರಿಸು('static constexpr float WMM_{}_MIN_{} = {:.1f}f; // latitude: {:.0f}, longitude: {:.0f}'.format(ಕೀಲಿ_name.ಊರ್ಧ್ವ(), units_str.ಊರ್ಧ್ವ(), ಮೌಲ್ಯ_min, ಮೌಲ್ಯ_min_lat_lon[0], ಮೌಲ್ಯ_min_lat_lon[1]))
    ಮುದ್ರಿಸು('static constexpr float WMM_{}_MAX_{} = {:.1f}f; // latitude: {:.0f}, longitude: {:.0f}'.format(ಕೀಲಿ_name.ಊರ್ಧ್ವ(), units_str.ಊರ್ಧ್ವ(), ಮೌಲ್ಯ_max, ಮೌಲ್ಯ_max_lat_lon[0], ಮೌಲ್ಯ_max_lat_lon[1]))
    ಮುದ್ರಿಸು("\n")


print_wmm_ಕೋಷ್ಟಕ('declination')
print_wmm_ಕೋಷ್ಟಕ('inclination')
print_wmm_ಕೋಷ್ಟಕ('totalintensity')
