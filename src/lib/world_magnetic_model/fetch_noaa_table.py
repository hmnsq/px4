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

आनयन math
आनयन numpy
आनयन json
आनयन statistics
आनयन sys
आनयन urllib.request

SAMPLING_RES = 10
SAMPLING_MIN_LAT = -90
SAMPLING_MAX_LAT = 90
SAMPLING_MIN_LON = -180
SAMPLING_MAX_LON = 180

def constrain(n, nmin, nmax):
    प्रत्यागम max(min(nmin, n), nmax)

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

किल्ली=sys.argv[1] # NOAA key (https://www.ngdc.noaa.gov/geomag/CalcSurvey.shtml)

print(header)

print('#include <stdint.h>\n')

LAT_DIM=पूर्णाङ्क((SAMPLING_MAX_LAT-SAMPLING_MIN_LAT)/SAMPLING_RES)+1
LON_DIM=पूर्णाङ्क((SAMPLING_MAX_LON-SAMPLING_MIN_LON)/SAMPLING_RES)+1

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
f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfwmm?key=%s&%s" % (किल्ली, params))
data = json.loads(f.पठ())

world_magnitude_model_units = data['units']

प्रति latitude अन्तः range(SAMPLING_MIN_LAT, SAMPLING_MAX_LAT+1, SAMPLING_RES):
    world_magnitude_model[latitude] = {}

    प्रति longitude अन्तः range(SAMPLING_MIN_LON, SAMPLING_MAX_LON+1, SAMPLING_RES):
        params = urllib.parse.urlencode({'lat1': latitude, 'lon1': longitude, 'lon2': SAMPLING_MAX_LON, 'resultFormat': 'json'})
        f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfwmm?key=%s&%s" % (किल्ली, params))
        data = json.loads(f.पठ())
        #print(json.dumps(data, indent = 4)) # debugging

        world_magnitude_model[latitude][longitude] = data['result'][0]
        #print(world_magnitude_model[latitude][longitude])


def print_wmm_table(किल्ली_name):

    मूल्य_min = भिन्न('inf')
    मूल्य_min_lat_lon = ()

    मूल्य_max = भिन्न('-inf')
    मूल्य_max_lat_lon = ()

    प्रति latitude, lat_row अन्तः world_magnitude_model.वस्तुसंच_2():
        #print(latitude, lat_row)
        प्रति longitude, result अन्तः lat_row.वस्तुसंच_2():
            #print(result)

            मूल्य = भिन्न(result[किल्ली_name])

            यदि (मूल्य > मूल्य_max):
                मूल्य_max = मूल्य
                मूल्य_max_lat_lon = (latitude, longitude)

            यदि (मूल्य < मूल्य_min):
                मूल्य_min = मूल्य
                मूल्य_min_lat_lon = (latitude, longitude)

    # scale the values to fit into int16_t
    मूल्य_scale_max = abs(numpy.iinfo(numpy.int16).max) / abs(मूल्य_max)
    मूल्य_scale_min = abs(numpy.iinfo(numpy.int16).min) / abs(मूल्य_min)
    मूल्य_scale = min(मूल्य_scale_max, मूल्य_scale_min)

    units_str = world_magnitude_model_units[किल्ली_name].split(' ')[0]

    # print the table
    print('// Magnetic {} data in {:.4g} {}'.format(किल्ली_name, 1.0 / मूल्य_scale, units_str))
    print('// Model: {},'.format(data['model']))
    print('// Version: {},'.format(data['version']))
    print('// Date: {},'.format(data['result'][0]['date']))
    print('static constexpr const int16_t {}_table[{}][{}]'.format(किल्ली_name, LAT_DIM, LON_DIM) + " {")
    print('\t//    LONGITUDE: ', end='')
    प्रति l अन्तः range(SAMPLING_MIN_LON, SAMPLING_MAX_LON+1, SAMPLING_RES):
        print('{0:6d},'.format(l), end='')
    print('')

    प्रति latitude, lat_row अन्तः world_magnitude_model.वस्तुसंच_2():
        print('\t/* LAT: {0:3d} */'.format(latitude) + ' { ', end='')
        latitude_blackout_zone = असत्य

        प्रति longitude, result अन्तः lat_row.वस्तुसंच_2():

            मूल्य = भिन्न(result[किल्ली_name])

            # value scaled to fit into int16_t
            मूल्य_int = पूर्णाङ्क(round(मूल्य * मूल्य_scale))
            print('{0:6d},'.format(मूल्य_int), end='')

            # blackout warning at this latitude
            यत्न:
                यदि result['warning']:
                    latitude_blackout_zone = सत्य

            अपवाद:
                pass

        यदि latitude_blackout_zone:
            print(' }, // WARNING! black out zone')
        अन्यथा:
            print(' },')

    print("};")

    print('static constexpr float WMM_{}_SCALE_TO_{} = {:.9g}f;'.format(किल्ली_name.upper(), units_str.upper(), 1.0 / मूल्य_scale))
    print('static constexpr float WMM_{}_MIN_{} = {:.1f}f; // latitude: {:.0f}, longitude: {:.0f}'.format(किल्ली_name.upper(), units_str.upper(), मूल्य_min, मूल्य_min_lat_lon[0], मूल्य_min_lat_lon[1]))
    print('static constexpr float WMM_{}_MAX_{} = {:.1f}f; // latitude: {:.0f}, longitude: {:.0f}'.format(किल्ली_name.upper(), units_str.upper(), मूल्य_max, मूल्य_max_lat_lon[0], मूल्य_max_lat_lon[1]))
    print("\n")


print_wmm_table('declination')
print_wmm_table('inclination')
print_wmm_table('totalintensity')
