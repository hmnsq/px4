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

استورد math
استورد numpy
استورد json
استورد statistics
استورد sys
استورد urllib.request

SAMPLING_RES = 10
SAMPLING_MIN_LAT = -90
SAMPLING_MAX_LAT = 90
SAMPLING_MIN_LON = -180
SAMPLING_MAX_LON = 180

تعریف constrain(n, nmin, nmax):
    أرجع max(min(nmin, n), nmax)

سرخی = """/****************************************************************************
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

کلید=sys.argv[1] # NOAA key (https://www.ngdc.noaa.gov/geomag/CalcSurvey.shtml)

چھاپو(سرخی)

چھاپو('#include <stdint.h>\n')

LAT_DIM=صحيح((SAMPLING_MAX_LAT-SAMPLING_MIN_LAT)/SAMPLING_RES)+1
LON_DIM=صحيح((SAMPLING_MAX_LON-SAMPLING_MIN_LON)/SAMPLING_RES)+1

چھاپو('static constexpr float SAMPLING_RES = {}'.format(SAMPLING_RES) + ';')
چھاپو('static constexpr float SAMPLING_MIN_LAT = {}'.format(SAMPLING_MIN_LAT) + ';')
چھاپو('static constexpr float SAMPLING_MAX_LAT = {}'.format(SAMPLING_MAX_LAT) + ';')
چھاپو('static constexpr float SAMPLING_MIN_LON = {}'.format(SAMPLING_MIN_LON) + ';')
چھاپو('static constexpr float SAMPLING_MAX_LON = {}'.format(SAMPLING_MAX_LON) + ';')
چھاپو('')
چھاپو('static constexpr int LAT_DIM = {}'.format(LAT_DIM) + ';')
چھاپو('static constexpr int LON_DIM = {}'.format(LON_DIM) + ';')
چھاپو('\n')

چھاپو('// *INDENT-OFF*\n\n\n')



# build the world magnetic model dictionary
world_magnitude_model = {} # lat/lon dictionary with grid result

params = urllib.parse.urlencode({'lat1': 0, 'lon1': 0, 'resultFormat': 'json'})
f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfwmm?key=%s&%s" % (کلید, params))
انکڑے = json.loads(f.اقرأ())

world_magnitude_model_units = انکڑے['units']

لكل latitude في سلسلہ(SAMPLING_MIN_LAT, SAMPLING_MAX_LAT+1, SAMPLING_RES):
    world_magnitude_model[latitude] = {}

    لكل longitude في سلسلہ(SAMPLING_MIN_LON, SAMPLING_MAX_LON+1, SAMPLING_RES):
        params = urllib.parse.urlencode({'lat1': latitude, 'lon1': longitude, 'lon2': SAMPLING_MAX_LON, 'resultFormat': 'json'})
        f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfwmm?key=%s&%s" % (کلید, params))
        انکڑے = json.loads(f.اقرأ())
        #print(json.dumps(data, indent = 4)) # debugging

        world_magnitude_model[latitude][longitude] = انکڑے['result'][0]
        #print(world_magnitude_model[latitude][longitude])


تعریف print_wmm_جدول(کلید_name):

    قدر_min = عائم('inf')
    قدر_min_lat_lon = ()

    قدر_max = عائم('-inf')
    قدر_max_lat_lon = ()

    لكل latitude, lat_صف في world_magnitude_model.اشیاء_2():
        #print(latitude, lat_row)
        لكل longitude, نتیجہ في lat_صف.اشیاء_2():
            #print(result)

            قدر = عائم(نتیجہ[کلید_name])

            إذا (قدر > قدر_max):
                قدر_max = قدر
                قدر_max_lat_lon = (latitude, longitude)

            إذا (قدر < قدر_min):
                قدر_min = قدر
                قدر_min_lat_lon = (latitude, longitude)

    # scale the values to fit into int16_t
    قدر_scale_max = abs(numpy.iinfo(numpy.int16).max) / abs(قدر_max)
    قدر_scale_min = abs(numpy.iinfo(numpy.int16).min) / abs(قدر_min)
    قدر_scale = min(قدر_scale_max, قدر_scale_min)

    units_str = world_magnitude_model_units[کلید_name].ونڈو(' ')[0]

    # print the table
    چھاپو('// Magnetic {} data in {:.4g} {}'.format(کلید_name, 1.0 / قدر_scale, units_str))
    چھاپو('// Model: {},'.format(انکڑے['model']))
    چھاپو('// Version: {},'.format(انکڑے['version']))
    چھاپو('// Date: {},'.format(انکڑے['result'][0]['date']))
    چھاپو('static constexpr const int16_t {}_table[{}][{}]'.format(کلید_name, LAT_DIM, LON_DIM) + " {")
    چھاپو('\t//    LONGITUDE: ', سرا='')
    لكل l في سلسلہ(SAMPLING_MIN_LON, SAMPLING_MAX_LON+1, SAMPLING_RES):
        چھاپو('{0:6d},'.format(l), سرا='')
    چھاپو('')

    لكل latitude, lat_صف في world_magnitude_model.اشیاء_2():
        چھاپو('\t/* LAT: {0:3d} */'.format(latitude) + ' { ', سرا='')
        latitude_blackout_zone = خطأ

        لكل longitude, نتیجہ في lat_صف.اشیاء_2():

            قدر = عائم(نتیجہ[کلید_name])

            # value scaled to fit into int16_t
            قدر_int = صحيح(round(قدر * قدر_scale))
            چھاپو('{0:6d},'.format(قدر_int), سرا='')

            # blackout warning at this latitude
            حاول:
                إذا نتیجہ['warning']:
                    latitude_blackout_zone = سچ

            استثناء:
                لنگھو

        إذا latitude_blackout_zone:
            چھاپو(' }, // WARNING! black out zone')
        وإلا:
            چھاپو(' },')

    چھاپو("};")

    چھاپو('static constexpr float WMM_{}_SCALE_TO_{} = {:.9g}f;'.format(کلید_name.اتلا(), units_str.اتلا(), 1.0 / قدر_scale))
    چھاپو('static constexpr float WMM_{}_MIN_{} = {:.1f}f; // latitude: {:.0f}, longitude: {:.0f}'.format(کلید_name.اتلا(), units_str.اتلا(), قدر_min, قدر_min_lat_lon[0], قدر_min_lat_lon[1]))
    چھاپو('static constexpr float WMM_{}_MAX_{} = {:.1f}f; // latitude: {:.0f}, longitude: {:.0f}'.format(کلید_name.اتلا(), units_str.اتلا(), قدر_max, قدر_max_lat_lon[0], قدر_max_lat_lon[1]))
    چھاپو("\n")


print_wmm_جدول('declination')
print_wmm_جدول('inclination')
print_wmm_جدول('totalintensity')
