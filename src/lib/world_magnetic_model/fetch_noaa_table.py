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

আনযন math
আনযন numpy
আনযন json
আনযন statistics
আনযন sys
আনযন urllib.request

SAMPLING_RES = 10
SAMPLING_MIN_LAT = -90
SAMPLING_MAX_LAT = 90
SAMPLING_MIN_LON = -180
SAMPLING_MAX_LON = 180

def constrain(n, nmin, nmax):
    ঘূৰাই max(min(nmin, n), nmax)

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

চাবি=sys.argv[1] # NOAA key (https://www.ngdc.noaa.gov/geomag/CalcSurvey.shtml)

print(header)

print('#include <stdint.h>\n')

LAT_DIM=পূর্ণাঙ্ক((SAMPLING_MAX_LAT-SAMPLING_MIN_LAT)/SAMPLING_RES)+1
LON_DIM=পূর্ণাঙ্ক((SAMPLING_MAX_LON-SAMPLING_MIN_LON)/SAMPLING_RES)+1

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
f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfwmm?key=%s&%s" % (চাবি, params))
data = json.loads(f.পঠ())

world_magnitude_model_units = data['units']

বাবে latitude অন্তঃ range(SAMPLING_MIN_LAT, SAMPLING_MAX_LAT+1, SAMPLING_RES):
    world_magnitude_model[latitude] = {}

    বাবে longitude অন্তঃ range(SAMPLING_MIN_LON, SAMPLING_MAX_LON+1, SAMPLING_RES):
        params = urllib.parse.urlencode({'lat1': latitude, 'lon1': longitude, 'lon2': SAMPLING_MAX_LON, 'resultFormat': 'json'})
        f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfwmm?key=%s&%s" % (চাবি, params))
        data = json.loads(f.পঠ())
        #print(json.dumps(data, indent = 4)) # debugging

        world_magnitude_model[latitude][longitude] = data['result'][0]
        #print(world_magnitude_model[latitude][longitude])


def print_wmm_table(চাবি_name):

    মান_min = ভিন্ন('inf')
    মান_min_lat_lon = ()

    মান_max = ভিন্ন('-inf')
    মান_max_lat_lon = ()

    বাবে latitude, lat_row অন্তঃ world_magnitude_model.বস্তুসমূহ_2():
        #print(latitude, lat_row)
        বাবে longitude, result অন্তঃ lat_row.বস্তুসমূহ_2():
            #print(result)

            মান = ভিন্ন(result[চাবি_name])

            যদি (মান > মান_max):
                মান_max = মান
                মান_max_lat_lon = (latitude, longitude)

            যদি (মান < মান_min):
                মান_min = মান
                মান_min_lat_lon = (latitude, longitude)

    # scale the values to fit into int16_t
    মান_scale_max = abs(numpy.iinfo(numpy.int16).max) / abs(মান_max)
    মান_scale_min = abs(numpy.iinfo(numpy.int16).min) / abs(মান_min)
    মান_scale = min(মান_scale_max, মান_scale_min)

    units_str = world_magnitude_model_units[চাবি_name].split(' ')[0]

    # print the table
    print('// Magnetic {} data in {:.4g} {}'.format(চাবি_name, 1.0 / মান_scale, units_str))
    print('// Model: {},'.format(data['model']))
    print('// Version: {},'.format(data['version']))
    print('// Date: {},'.format(data['result'][0]['date']))
    print('static constexpr const int16_t {}_table[{}][{}]'.format(চাবি_name, LAT_DIM, LON_DIM) + " {")
    print('\t//    LONGITUDE: ', end='')
    বাবে l অন্তঃ range(SAMPLING_MIN_LON, SAMPLING_MAX_LON+1, SAMPLING_RES):
        print('{0:6d},'.format(l), end='')
    print('')

    বাবে latitude, lat_row অন্তঃ world_magnitude_model.বস্তুসমূহ_2():
        print('\t/* LAT: {0:3d} */'.format(latitude) + ' { ', end='')
        latitude_blackout_zone = মিছা

        বাবে longitude, result অন্তঃ lat_row.বস্তুসমূহ_2():

            মান = ভিন্ন(result[চাবি_name])

            # value scaled to fit into int16_t
            মান_int = পূর্ণাঙ্ক(round(মান * মান_scale))
            print('{0:6d},'.format(মান_int), end='')

            # blackout warning at this latitude
            যত্ন:
                যদি result['warning']:
                    latitude_blackout_zone = সঁচা

            অপৱাদ:
                pass

        যদি latitude_blackout_zone:
            print(' }, // WARNING! black out zone')
        অন্যথা:
            print(' },')

    print("};")

    print('static constexpr float WMM_{}_SCALE_TO_{} = {:.9g}f;'.format(চাবি_name.upper(), units_str.upper(), 1.0 / মান_scale))
    print('static constexpr float WMM_{}_MIN_{} = {:.1f}f; // latitude: {:.0f}, longitude: {:.0f}'.format(চাবি_name.upper(), units_str.upper(), মান_min, মান_min_lat_lon[0], মান_min_lat_lon[1]))
    print('static constexpr float WMM_{}_MAX_{} = {:.1f}f; // latitude: {:.0f}, longitude: {:.0f}'.format(চাবি_name.upper(), units_str.upper(), মান_max, মান_max_lat_lon[0], মান_max_lat_lon[1]))
    print("\n")


print_wmm_table('declination')
print_wmm_table('inclination')
print_wmm_table('totalintensity')
