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

ಆನಯನ math
ಆನಯನ numpy
ಆನಯನ json
ಆನಯನ statistics
ಆನಯನ sys
ಆನಯನ urllib.request

SAMPLING_RES = 10
SAMPLING_MIN_LAT = -90
SAMPLING_MAX_LAT = 90
SAMPLING_MIN_LON = -180
SAMPLING_MAX_LON = 180

def constrain(n, nmin, nmax):
    ಪ್ರತ್ಯಾಗಮ max(min(nmin, n), nmax)

header = """/****************************************************************************
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

print(header)

print('#include <stdint.h>\n')

LAT_DIM=ಪೂರ್ಣಾಙ್ಕ((SAMPLING_MAX_LAT-SAMPLING_MIN_LAT)/SAMPLING_RES)+1
LON_DIM=ಪೂರ್ಣಾಙ್ಕ((SAMPLING_MAX_LON-SAMPLING_MIN_LON)/SAMPLING_RES)+1

print('static constexpr float SAMPLING_RES = {}'.format(SAMPLING_RES) + ';')
print('static constexpr float SAMPLING_MIN_LAT = {}'.format(SAMPLING_MIN_LAT) + ';')
print('static constexpr float SAMPLING_MAX_LAT = {}'.format(SAMPLING_MAX_LAT) + ';')
print('static constexpr float SAMPLING_MIN_LON = {}'.format(SAMPLING_MIN_LON) + ';')
print('static constexpr float SAMPLING_MAX_LON = {}'.format(SAMPLING_MAX_LON) + ';')
print('')
print('static constexpr int LAT_DIM = {}'.format(LAT_DIM) + ';')
print('static constexpr int LON_DIM = {}'.format(LON_DIM) + ';')
print('\n')

print('// *INDENT-OFF*\n\n\n')



# build the world magnetic model dictionary
world_magnitude_model = {} # lat/lon dictionary with grid result

params = urllib.parse.urlencode({'lat1': 0, 'lon1': 0, 'resultFormat': 'json'})
f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfwmm?key=%s&%s" % (ಕೀಲಿ, params))
data = json.loads(f.ಪಠ())

world_magnitude_model_units = data['units']

ಪ್ರತಿ latitude ಅನ್ತಃ range(SAMPLING_MIN_LAT, SAMPLING_MAX_LAT+1, SAMPLING_RES):
    world_magnitude_model[latitude] = {}

    ಪ್ರತಿ longitude ಅನ್ತಃ range(SAMPLING_MIN_LON, SAMPLING_MAX_LON+1, SAMPLING_RES):
        params = urllib.parse.urlencode({'lat1': latitude, 'lon1': longitude, 'lon2': SAMPLING_MAX_LON, 'resultFormat': 'json'})
        f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfwmm?key=%s&%s" % (ಕೀಲಿ, params))
        data = json.loads(f.ಪಠ())
        #print(json.dumps(data, indent = 4)) # debugging

        world_magnitude_model[latitude][longitude] = data['result'][0]
        #print(world_magnitude_model[latitude][longitude])


def print_wmm_table(ಕೀಲಿ_name):

    ಮೌಲ್ಯ_min = ಭಿನ್ನ('inf')
    ಮೌಲ್ಯ_min_lat_lon = ()

    ಮೌಲ್ಯ_max = ಭಿನ್ನ('-inf')
    ಮೌಲ್ಯ_max_lat_lon = ()

    ಪ್ರತಿ latitude, lat_row ಅನ್ತಃ world_magnitude_model.ವಸ್ತುಗಳು_2():
        #print(latitude, lat_row)
        ಪ್ರತಿ longitude, result ಅನ್ತಃ lat_row.ವಸ್ತುಗಳು_2():
            #print(result)

            ಮೌಲ್ಯ = ಭಿನ್ನ(result[ಕೀಲಿ_name])

            ಯದಿ (ಮೌಲ್ಯ > ಮೌಲ್ಯ_max):
                ಮೌಲ್ಯ_max = ಮೌಲ್ಯ
                ಮೌಲ್ಯ_max_lat_lon = (latitude, longitude)

            ಯದಿ (ಮೌಲ್ಯ < ಮೌಲ್ಯ_min):
                ಮೌಲ್ಯ_min = ಮೌಲ್ಯ
                ಮೌಲ್ಯ_min_lat_lon = (latitude, longitude)

    # scale the values to fit into int16_t
    ಮೌಲ್ಯ_scale_max = abs(numpy.iinfo(numpy.int16).max) / abs(ಮೌಲ್ಯ_max)
    ಮೌಲ್ಯ_scale_min = abs(numpy.iinfo(numpy.int16).min) / abs(ಮೌಲ್ಯ_min)
    ಮೌಲ್ಯ_scale = min(ಮೌಲ್ಯ_scale_max, ಮೌಲ್ಯ_scale_min)

    units_str = world_magnitude_model_units[ಕೀಲಿ_name].split(' ')[0]

    # print the table
    print('// Magnetic {} data in {:.4g} {}'.format(ಕೀಲಿ_name, 1.0 / ಮೌಲ್ಯ_scale, units_str))
    print('// Model: {},'.format(data['model']))
    print('// Version: {},'.format(data['version']))
    print('// Date: {},'.format(data['result'][0]['date']))
    print('static constexpr const int16_t {}_table[{}][{}]'.format(ಕೀಲಿ_name, LAT_DIM, LON_DIM) + " {")
    print('\t//    LONGITUDE: ', end='')
    ಪ್ರತಿ l ಅನ್ತಃ range(SAMPLING_MIN_LON, SAMPLING_MAX_LON+1, SAMPLING_RES):
        print('{0:6d},'.format(l), end='')
    print('')

    ಪ್ರತಿ latitude, lat_row ಅನ್ತಃ world_magnitude_model.ವಸ್ತುಗಳು_2():
        print('\t/* LAT: {0:3d} */'.format(latitude) + ' { ', end='')
        latitude_blackout_zone = ಅಸತ್ಯ

        ಪ್ರತಿ longitude, result ಅನ್ತಃ lat_row.ವಸ್ತುಗಳು_2():

            ಮೌಲ್ಯ = ಭಿನ್ನ(result[ಕೀಲಿ_name])

            # value scaled to fit into int16_t
            ಮೌಲ್ಯ_int = ಪೂರ್ಣಾಙ್ಕ(round(ಮೌಲ್ಯ * ಮೌಲ್ಯ_scale))
            print('{0:6d},'.format(ಮೌಲ್ಯ_int), end='')

            # blackout warning at this latitude
            ಯತ್ನ:
                ಯದಿ result['warning']:
                    latitude_blackout_zone = ಸತ್ಯ

            ಅಪವಾದ:
                pass

        ಯದಿ latitude_blackout_zone:
            print(' }, // WARNING! black out zone')
        ಅನ್ಯಥಾ:
            print(' },')

    print("};")

    print('static constexpr float WMM_{}_SCALE_TO_{} = {:.9g}f;'.format(ಕೀಲಿ_name.upper(), units_str.upper(), 1.0 / ಮೌಲ್ಯ_scale))
    print('static constexpr float WMM_{}_MIN_{} = {:.1f}f; // latitude: {:.0f}, longitude: {:.0f}'.format(ಕೀಲಿ_name.upper(), units_str.upper(), ಮೌಲ್ಯ_min, ಮೌಲ್ಯ_min_lat_lon[0], ಮೌಲ್ಯ_min_lat_lon[1]))
    print('static constexpr float WMM_{}_MAX_{} = {:.1f}f; // latitude: {:.0f}, longitude: {:.0f}'.format(ಕೀಲಿ_name.upper(), units_str.upper(), ಮೌಲ್ಯ_max, ಮೌಲ್ಯ_max_lat_lon[0], ಮೌಲ್ಯ_max_lat_lon[1]))
    print("\n")


print_wmm_table('declination')
print_wmm_table('inclination')
print_wmm_table('totalintensity')
