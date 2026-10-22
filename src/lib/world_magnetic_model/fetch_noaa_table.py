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

ਆਯਾਤ math
ਆਯਾਤ numpy
ਆਯਾਤ json
ਆਯਾਤ statistics
ਆਯਾਤ sys
ਆਯਾਤ urllib.request

SAMPLING_RES = 10
SAMPLING_MIN_LAT = -90
SAMPLING_MAX_LAT = 90
SAMPLING_MIN_LON = -180
SAMPLING_MAX_LON = 180

ਪਰਿਭਾਸ਼ਾ constrain(n, nmin, nmax):
    ਵਾਪਸ max(min(nmin, n), nmax)

ਸਿਰਲੇਖ = """/****************************************************************************
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

ਕੁੰਜੀ=sys.argv[1] # NOAA key (https://www.ngdc.noaa.gov/geomag/CalcSurvey.shtml)

ਛਾਪੋ(ਸਿਰਲੇਖ)

ਛਾਪੋ('#include <stdint.h>\n')

LAT_DIM=ਪੂਰਨ((SAMPLING_MAX_LAT-SAMPLING_MIN_LAT)/SAMPLING_RES)+1
LON_DIM=ਪੂਰਨ((SAMPLING_MAX_LON-SAMPLING_MIN_LON)/SAMPLING_RES)+1

ਛਾਪੋ('static constexpr float SAMPLING_RES = {}'.format(SAMPLING_RES) + ';')
ਛਾਪੋ('static constexpr float SAMPLING_MIN_LAT = {}'.format(SAMPLING_MIN_LAT) + ';')
ਛਾਪੋ('static constexpr float SAMPLING_MAX_LAT = {}'.format(SAMPLING_MAX_LAT) + ';')
ਛਾਪੋ('static constexpr float SAMPLING_MIN_LON = {}'.format(SAMPLING_MIN_LON) + ';')
ਛਾਪੋ('static constexpr float SAMPLING_MAX_LON = {}'.format(SAMPLING_MAX_LON) + ';')
ਛਾਪੋ('')
ਛਾਪੋ('static constexpr int LAT_DIM = {}'.format(LAT_DIM) + ';')
ਛਾਪੋ('static constexpr int LON_DIM = {}'.format(LON_DIM) + ';')
ਛਾਪੋ('\n')

ਛਾਪੋ('// *INDENT-OFF*\n\n\n')



# build the world magnetic model dictionary
world_magnitude_model = {} # lat/lon dictionary with grid result

params = urllib.parse.urlencode({'lat1': 0, 'lon1': 0, 'resultFormat': 'json'})
f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfwmm?key=%s&%s" % (ਕੁੰਜੀ, params))
ਅੰਕੜੇ = json.loads(f.ਪਠ())

world_magnitude_model_units = ਅੰਕੜੇ['units']

ਲਈ latitude ਵਿੱਚ ਸੀਮਾ(SAMPLING_MIN_LAT, SAMPLING_MAX_LAT+1, SAMPLING_RES):
    world_magnitude_model[latitude] = {}

    ਲਈ longitude ਵਿੱਚ ਸੀਮਾ(SAMPLING_MIN_LON, SAMPLING_MAX_LON+1, SAMPLING_RES):
        params = urllib.parse.urlencode({'lat1': latitude, 'lon1': longitude, 'lon2': SAMPLING_MAX_LON, 'resultFormat': 'json'})
        f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfwmm?key=%s&%s" % (ਕੁੰਜੀ, params))
        ਅੰਕੜੇ = json.loads(f.ਪਠ())
        #print(json.dumps(data, indent = 4)) # debugging

        world_magnitude_model[latitude][longitude] = ਅੰਕੜੇ['result'][0]
        #print(world_magnitude_model[latitude][longitude])


ਪਰਿਭਾਸ਼ਾ print_wmm_ਸਾਰਣੀ(ਕੁੰਜੀ_name):

    ਮੁੱਲ_min = ਦਸ਼ਮਲਵ('inf')
    ਮੁੱਲ_min_lat_lon = ()

    ਮੁੱਲ_max = ਦਸ਼ਮਲਵ('-inf')
    ਮੁੱਲ_max_lat_lon = ()

    ਲਈ latitude, lat_ਪੰਕਤੀ ਵਿੱਚ world_magnitude_model.ਚੀਜ਼ਾਂ_2():
        #print(latitude, lat_row)
        ਲਈ longitude, ਨਤੀਜਾ ਵਿੱਚ lat_ਪੰਕਤੀ.ਚੀਜ਼ਾਂ_2():
            #print(result)

            ਮੁੱਲ = ਦਸ਼ਮਲਵ(ਨਤੀਜਾ[ਕੁੰਜੀ_name])

            ਜੇ (ਮੁੱਲ > ਮੁੱਲ_max):
                ਮੁੱਲ_max = ਮੁੱਲ
                ਮੁੱਲ_max_lat_lon = (latitude, longitude)

            ਜੇ (ਮੁੱਲ < ਮੁੱਲ_min):
                ਮੁੱਲ_min = ਮੁੱਲ
                ਮੁੱਲ_min_lat_lon = (latitude, longitude)

    # scale the values to fit into int16_t
    ਮੁੱਲ_scale_max = abs(numpy.iinfo(numpy.int16).max) / abs(ਮੁੱਲ_max)
    ਮੁੱਲ_scale_min = abs(numpy.iinfo(numpy.int16).min) / abs(ਮੁੱਲ_min)
    ਮੁੱਲ_scale = min(ਮੁੱਲ_scale_max, ਮੁੱਲ_scale_min)

    units_str = world_magnitude_model_units[ਕੁੰਜੀ_name].ਵੰਡੋ(' ')[0]

    # print the table
    ਛਾਪੋ('// Magnetic {} data in {:.4g} {}'.format(ਕੁੰਜੀ_name, 1.0 / ਮੁੱਲ_scale, units_str))
    ਛਾਪੋ('// Model: {},'.format(ਅੰਕੜੇ['model']))
    ਛਾਪੋ('// Version: {},'.format(ਅੰਕੜੇ['version']))
    ਛਾਪੋ('// Date: {},'.format(ਅੰਕੜੇ['result'][0]['date']))
    ਛਾਪੋ('static constexpr const int16_t {}_table[{}][{}]'.format(ਕੁੰਜੀ_name, LAT_DIM, LON_DIM) + " {")
    ਛਾਪੋ('\t//    LONGITUDE: ', ਸਿਰਾ='')
    ਲਈ l ਵਿੱਚ ਸੀਮਾ(SAMPLING_MIN_LON, SAMPLING_MAX_LON+1, SAMPLING_RES):
        ਛਾਪੋ('{0:6d},'.format(l), ਸਿਰਾ='')
    ਛਾਪੋ('')

    ਲਈ latitude, lat_ਪੰਕਤੀ ਵਿੱਚ world_magnitude_model.ਚੀਜ਼ਾਂ_2():
        ਛਾਪੋ('\t/* LAT: {0:3d} */'.format(latitude) + ' { ', ਸਿਰਾ='')
        latitude_blackout_zone = ਝੂਠ

        ਲਈ longitude, ਨਤੀਜਾ ਵਿੱਚ lat_ਪੰਕਤੀ.ਚੀਜ਼ਾਂ_2():

            ਮੁੱਲ = ਦਸ਼ਮਲਵ(ਨਤੀਜਾ[ਕੁੰਜੀ_name])

            # value scaled to fit into int16_t
            ਮੁੱਲ_int = ਪੂਰਨ(round(ਮੁੱਲ * ਮੁੱਲ_scale))
            ਛਾਪੋ('{0:6d},'.format(ਮੁੱਲ_int), ਸਿਰਾ='')

            # blackout warning at this latitude
            ਕੋਸ਼ਿਸ਼:
                ਜੇ ਨਤੀਜਾ['warning']:
                    latitude_blackout_zone = ਸੱਚ

            ਅਪਵਾਦ:
                ਲੰਘੋ

        ਜੇ latitude_blackout_zone:
            ਛਾਪੋ(' }, // WARNING! black out zone')
        ਨਹੀਂਤਾਂ:
            ਛਾਪੋ(' },')

    ਛਾਪੋ("};")

    ਛਾਪੋ('static constexpr float WMM_{}_SCALE_TO_{} = {:.9g}f;'.format(ਕੁੰਜੀ_name.ਉੱਤਲਾ(), units_str.ਉੱਤਲਾ(), 1.0 / ਮੁੱਲ_scale))
    ਛਾਪੋ('static constexpr float WMM_{}_MIN_{} = {:.1f}f; // latitude: {:.0f}, longitude: {:.0f}'.format(ਕੁੰਜੀ_name.ਉੱਤਲਾ(), units_str.ਉੱਤਲਾ(), ਮੁੱਲ_min, ਮੁੱਲ_min_lat_lon[0], ਮੁੱਲ_min_lat_lon[1]))
    ਛਾਪੋ('static constexpr float WMM_{}_MAX_{} = {:.1f}f; // latitude: {:.0f}, longitude: {:.0f}'.format(ਕੁੰਜੀ_name.ਉੱਤਲਾ(), units_str.ਉੱਤਲਾ(), ਮੁੱਲ_max, ਮੁੱਲ_max_lat_lon[0], ਮੁੱਲ_max_lat_lon[1]))
    ਛਾਪੋ("\n")


print_wmm_ਸਾਰਣੀ('declination')
print_wmm_ਸਾਰਣੀ('inclination')
print_wmm_ਸਾਰਣੀ('totalintensity')
