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

परिभाषा constrain(n, nmin, nmax):
    प्रत्यागम max(min(nmin, n), nmax)

शीर्षकम् = """/****************************************************************************
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

कुञ्चिका=sys.argv[1] # NOAA key (https://www.ngdc.noaa.gov/geomag/CalcSurvey.shtml)

मुद्रय(शीर्षकम्)

मुद्रय('#include <stdint.h>\n')

LAT_DIM=पूर्णाङ्क((SAMPLING_MAX_LAT-SAMPLING_MIN_LAT)/SAMPLING_RES)+1
LON_DIM=पूर्णाङ्क((SAMPLING_MAX_LON-SAMPLING_MIN_LON)/SAMPLING_RES)+1

मुद्रय('static constexpr float SAMPLING_RES = {}'.format(SAMPLING_RES) + ';')
मुद्रय('static constexpr float SAMPLING_MIN_LAT = {}'.format(SAMPLING_MIN_LAT) + ';')
मुद्रय('static constexpr float SAMPLING_MAX_LAT = {}'.format(SAMPLING_MAX_LAT) + ';')
मुद्रय('static constexpr float SAMPLING_MIN_LON = {}'.format(SAMPLING_MIN_LON) + ';')
मुद्रय('static constexpr float SAMPLING_MAX_LON = {}'.format(SAMPLING_MAX_LON) + ';')
मुद्रय('')
मुद्रय('static constexpr int LAT_DIM = {}'.format(LAT_DIM) + ';')
मुद्रय('static constexpr int LON_DIM = {}'.format(LON_DIM) + ';')
मुद्रय('\n')

मुद्रय('// *INDENT-OFF*\n\n\n')



# build the world magnetic model dictionary
world_magnitude_model = {} # lat/lon dictionary with grid result

params = urllib.parse.urlencode({'lat1': 0, 'lon1': 0, 'resultFormat': 'json'})
f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfwmm?key=%s&%s" % (कुञ्चिका, params))
दत्त = json.loads(f.पठ())

world_magnitude_model_units = दत्त['units']

प्रति latitude अन्तः परिसरः(SAMPLING_MIN_LAT, SAMPLING_MAX_LAT+1, SAMPLING_RES):
    world_magnitude_model[latitude] = {}

    प्रति longitude अन्तः परिसरः(SAMPLING_MIN_LON, SAMPLING_MAX_LON+1, SAMPLING_RES):
        params = urllib.parse.urlencode({'lat1': latitude, 'lon1': longitude, 'lon2': SAMPLING_MAX_LON, 'resultFormat': 'json'})
        f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfwmm?key=%s&%s" % (कुञ्चिका, params))
        दत्त = json.loads(f.पठ())
        #print(json.dumps(data, indent = 4)) # debugging

        world_magnitude_model[latitude][longitude] = दत्त['result'][0]
        #print(world_magnitude_model[latitude][longitude])


परिभाषा print_wmm_सारणी(कुञ्चिका_name):

    मानम्_min = भिन्न('inf')
    मानम्_min_lat_lon = ()

    मानम्_max = भिन्न('-inf')
    मानम्_max_lat_lon = ()

    प्रति latitude, lat_row अन्तः world_magnitude_model.वस्तूनि_2():
        #print(latitude, lat_row)
        प्रति longitude, परिणाम अन्तः lat_row.वस्तूनि_2():
            #print(result)

            मानम् = भिन्न(परिणाम[कुञ्चिका_name])

            यदि (मानम् > मानम्_max):
                मानम्_max = मानम्
                मानम्_max_lat_lon = (latitude, longitude)

            यदि (मानम् < मानम्_min):
                मानम्_min = मानम्
                मानम्_min_lat_lon = (latitude, longitude)

    # scale the values to fit into int16_t
    मानम्_scale_max = abs(numpy.iinfo(numpy.int16).max) / abs(मानम्_max)
    मानम्_scale_min = abs(numpy.iinfo(numpy.int16).min) / abs(मानम्_min)
    मानम्_scale = min(मानम्_scale_max, मानम्_scale_min)

    units_str = world_magnitude_model_units[कुञ्चिका_name].विभज(' ')[0]

    # print the table
    मुद्रय('// Magnetic {} data in {:.4g} {}'.format(कुञ्चिका_name, 1.0 / मानम्_scale, units_str))
    मुद्रय('// Model: {},'.format(दत्त['model']))
    मुद्रय('// Version: {},'.format(दत्त['version']))
    मुद्रय('// Date: {},'.format(दत्त['result'][0]['date']))
    मुद्रय('static constexpr const int16_t {}_table[{}][{}]'.format(कुञ्चिका_name, LAT_DIM, LON_DIM) + " {")
    मुद्रय('\t//    LONGITUDE: ', अन्तः_2='')
    प्रति l अन्तः परिसरः(SAMPLING_MIN_LON, SAMPLING_MAX_LON+1, SAMPLING_RES):
        मुद्रय('{0:6d},'.format(l), अन्तः_2='')
    मुद्रय('')

    प्रति latitude, lat_row अन्तः world_magnitude_model.वस्तूनि_2():
        मुद्रय('\t/* LAT: {0:3d} */'.format(latitude) + ' { ', अन्तः_2='')
        latitude_blackout_zone = असत्य

        प्रति longitude, परिणाम अन्तः lat_row.वस्तूनि_2():

            मानम् = भिन्न(परिणाम[कुञ्चिका_name])

            # value scaled to fit into int16_t
            मानम्_int = पूर्णाङ्क(round(मानम् * मानम्_scale))
            मुद्रय('{0:6d},'.format(मानम्_int), अन्तः_2='')

            # blackout warning at this latitude
            यत्न:
                यदि परिणाम['warning']:
                    latitude_blackout_zone = सत्य

            अपवाद:
                अतिक्रम

        यदि latitude_blackout_zone:
            मुद्रय(' }, // WARNING! black out zone')
        अन्यथा:
            मुद्रय(' },')

    मुद्रय("};")

    मुद्रय('static constexpr float WMM_{}_SCALE_TO_{} = {:.9g}f;'.format(कुञ्चिका_name.ऊर्ध्वस्थम्(), units_str.ऊर्ध्वस्थम्(), 1.0 / मानम्_scale))
    मुद्रय('static constexpr float WMM_{}_MIN_{} = {:.1f}f; // latitude: {:.0f}, longitude: {:.0f}'.format(कुञ्चिका_name.ऊर्ध्वस्थम्(), units_str.ऊर्ध्वस्थम्(), मानम्_min, मानम्_min_lat_lon[0], मानम्_min_lat_lon[1]))
    मुद्रय('static constexpr float WMM_{}_MAX_{} = {:.1f}f; // latitude: {:.0f}, longitude: {:.0f}'.format(कुञ्चिका_name.ऊर्ध्वस्थम्(), units_str.ऊर्ध्वस्थम्(), मानम्_max, मानम्_max_lat_lon[0], मानम्_max_lat_lon[1]))
    मुद्रय("\n")


print_wmm_सारणी('declination')
print_wmm_सारणी('inclination')
print_wmm_सारणी('totalintensity')
