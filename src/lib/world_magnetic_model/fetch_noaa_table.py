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

سرآیند = """/****************************************************************************
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

چاپ(سرآیند)

چاپ('#include <stdint.h>\n')

LAT_DIM=صحيح((SAMPLING_MAX_LAT-SAMPLING_MIN_LAT)/SAMPLING_RES)+1
LON_DIM=صحيح((SAMPLING_MAX_LON-SAMPLING_MIN_LON)/SAMPLING_RES)+1

چاپ('static constexpr float SAMPLING_RES = {}'.format(SAMPLING_RES) + ';')
چاپ('static constexpr float SAMPLING_MIN_LAT = {}'.format(SAMPLING_MIN_LAT) + ';')
چاپ('static constexpr float SAMPLING_MAX_LAT = {}'.format(SAMPLING_MAX_LAT) + ';')
چاپ('static constexpr float SAMPLING_MIN_LON = {}'.format(SAMPLING_MIN_LON) + ';')
چاپ('static constexpr float SAMPLING_MAX_LON = {}'.format(SAMPLING_MAX_LON) + ';')
چاپ('')
چاپ('static constexpr int LAT_DIM = {}'.format(LAT_DIM) + ';')
چاپ('static constexpr int LON_DIM = {}'.format(LON_DIM) + ';')
چاپ('\n')

چاپ('// *INDENT-OFF*\n\n\n')



# build the world magnetic model dictionary
world_magnitude_model = {} # lat/lon dictionary with grid result

params = urllib.parse.urlencode({'lat1': 0, 'lon1': 0, 'resultFormat': 'json'})
f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfwmm?key=%s&%s" % (کلید, params))
داده = json.loads(f.اقرأ())

world_magnitude_model_units = داده['units']

لكل latitude في دامنه(SAMPLING_MIN_LAT, SAMPLING_MAX_LAT+1, SAMPLING_RES):
    world_magnitude_model[latitude] = {}

    لكل longitude في دامنه(SAMPLING_MIN_LON, SAMPLING_MAX_LON+1, SAMPLING_RES):
        params = urllib.parse.urlencode({'lat1': latitude, 'lon1': longitude, 'lon2': SAMPLING_MAX_LON, 'resultFormat': 'json'})
        f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfwmm?key=%s&%s" % (کلید, params))
        داده = json.loads(f.اقرأ())
        #print(json.dumps(data, indent = 4)) # debugging

        world_magnitude_model[latitude][longitude] = داده['result'][0]
        #print(world_magnitude_model[latitude][longitude])


تعریف print_wmm_جدول(کلید_name):

    مقدار_min = عائم('inf')
    مقدار_min_lat_lon = ()

    مقدار_max = عائم('-inf')
    مقدار_max_lat_lon = ()

    لكل latitude, lat_ردیف في world_magnitude_model.اقلام_2():
        #print(latitude, lat_row)
        لكل longitude, نتیجه في lat_ردیف.اقلام_2():
            #print(result)

            مقدار = عائم(نتیجه[کلید_name])

            إذا (مقدار > مقدار_max):
                مقدار_max = مقدار
                مقدار_max_lat_lon = (latitude, longitude)

            إذا (مقدار < مقدار_min):
                مقدار_min = مقدار
                مقدار_min_lat_lon = (latitude, longitude)

    # scale the values to fit into int16_t
    مقدار_scale_max = abs(numpy.iinfo(numpy.int16).max) / abs(مقدار_max)
    مقدار_scale_min = abs(numpy.iinfo(numpy.int16).min) / abs(مقدار_min)
    مقدار_scale = min(مقدار_scale_max, مقدار_scale_min)

    units_str = world_magnitude_model_units[کلید_name].تقسیم(' ')[0]

    # print the table
    چاپ('// Magnetic {} data in {:.4g} {}'.format(کلید_name, 1.0 / مقدار_scale, units_str))
    چاپ('// Model: {},'.format(داده['model']))
    چاپ('// Version: {},'.format(داده['version']))
    چاپ('// Date: {},'.format(داده['result'][0]['date']))
    چاپ('static constexpr const int16_t {}_table[{}][{}]'.format(کلید_name, LAT_DIM, LON_DIM) + " {")
    چاپ('\t//    LONGITUDE: ', انتها='')
    لكل l في دامنه(SAMPLING_MIN_LON, SAMPLING_MAX_LON+1, SAMPLING_RES):
        چاپ('{0:6d},'.format(l), انتها='')
    چاپ('')

    لكل latitude, lat_ردیف في world_magnitude_model.اقلام_2():
        چاپ('\t/* LAT: {0:3d} */'.format(latitude) + ' { ', انتها='')
        latitude_blackout_zone = خطأ

        لكل longitude, نتیجه في lat_ردیف.اقلام_2():

            مقدار = عائم(نتیجه[کلید_name])

            # value scaled to fit into int16_t
            مقدار_int = صحيح(round(مقدار * مقدار_scale))
            چاپ('{0:6d},'.format(مقدار_int), انتها='')

            # blackout warning at this latitude
            حاول:
                إذا نتیجه['warning']:
                    latitude_blackout_zone = درست

            استثناء:
                بگذر

        إذا latitude_blackout_zone:
            چاپ(' }, // WARNING! black out zone')
        وإلا:
            چاپ(' },')

    چاپ("};")

    چاپ('static constexpr float WMM_{}_SCALE_TO_{} = {:.9g}f;'.format(کلید_name.فوقانی(), units_str.فوقانی(), 1.0 / مقدار_scale))
    چاپ('static constexpr float WMM_{}_MIN_{} = {:.1f}f; // latitude: {:.0f}, longitude: {:.0f}'.format(کلید_name.فوقانی(), units_str.فوقانی(), مقدار_min, مقدار_min_lat_lon[0], مقدار_min_lat_lon[1]))
    چاپ('static constexpr float WMM_{}_MAX_{} = {:.1f}f; // latitude: {:.0f}, longitude: {:.0f}'.format(کلید_name.فوقانی(), units_str.فوقانی(), مقدار_max, مقدار_max_lat_lon[0], مقدار_max_lat_lon[1]))
    چاپ("\n")


print_wmm_جدول('declination')
print_wmm_جدول('inclination')
print_wmm_جدول('totalintensity')
