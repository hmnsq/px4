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

దిగుమతి math
దిగుమతి numpy
దిగుమతి json
దిగుమతి statistics
దిగుమతి sys
దిగుమతి urllib.request

SAMPLING_RES = 10
SAMPLING_MIN_LAT = -90
SAMPLING_MAX_LAT = 90
SAMPLING_MIN_LON = -180
SAMPLING_MAX_LON = 180

నిర్వచనం constrain(n, nmin, nmax):
    తిరిగి max(min(nmin, n), nmax)

శీర్షిక = """/****************************************************************************
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

కీలకం=sys.argv[1] # NOAA key (https://www.ngdc.noaa.gov/geomag/CalcSurvey.shtml)

ముద్రించు(శీర్షిక)

ముద్రించు('#include <stdint.h>\n')

LAT_DIM=పూర్ణాంకం((SAMPLING_MAX_LAT-SAMPLING_MIN_LAT)/SAMPLING_RES)+1
LON_DIM=పూర్ణాంకం((SAMPLING_MAX_LON-SAMPLING_MIN_LON)/SAMPLING_RES)+1

ముద్రించు('static constexpr float SAMPLING_RES = {}'.format(SAMPLING_RES) + ';')
ముద్రించు('static constexpr float SAMPLING_MIN_LAT = {}'.format(SAMPLING_MIN_LAT) + ';')
ముద్రించు('static constexpr float SAMPLING_MAX_LAT = {}'.format(SAMPLING_MAX_LAT) + ';')
ముద్రించు('static constexpr float SAMPLING_MIN_LON = {}'.format(SAMPLING_MIN_LON) + ';')
ముద్రించు('static constexpr float SAMPLING_MAX_LON = {}'.format(SAMPLING_MAX_LON) + ';')
ముద్రించు('')
ముద్రించు('static constexpr int LAT_DIM = {}'.format(LAT_DIM) + ';')
ముద్రించు('static constexpr int LON_DIM = {}'.format(LON_DIM) + ';')
ముద్రించు('\n')

ముద్రించు('// *INDENT-OFF*\n\n\n')



# build the world magnetic model dictionary
world_magnitude_model = {} # lat/lon dictionary with grid result

params = urllib.parse.urlencode({'lat1': 0, 'lon1': 0, 'resultFormat': 'json'})
f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfwmm?key=%s&%s" % (కీలకం, params))
దత్తాంశం = json.loads(f.పఠ())

world_magnitude_model_units = దత్తాంశం['units']

కొరకు latitude లో శ్రేణి(SAMPLING_MIN_LAT, SAMPLING_MAX_LAT+1, SAMPLING_RES):
    world_magnitude_model[latitude] = {}

    కొరకు longitude లో శ్రేణి(SAMPLING_MIN_LON, SAMPLING_MAX_LON+1, SAMPLING_RES):
        params = urllib.parse.urlencode({'lat1': latitude, 'lon1': longitude, 'lon2': SAMPLING_MAX_LON, 'resultFormat': 'json'})
        f = urllib.request.urlopen("https://www.ngdc.noaa.gov/geomag-web/calculators/calculateIgrfwmm?key=%s&%s" % (కీలకం, params))
        దత్తాంశం = json.loads(f.పఠ())
        #print(json.dumps(data, indent = 4)) # debugging

        world_magnitude_model[latitude][longitude] = దత్తాంశం['result'][0]
        #print(world_magnitude_model[latitude][longitude])


నిర్వచనం print_wmm_పట్టిక(కీలకం_name):

    విలువ_min = దశాంశం('inf')
    విలువ_min_lat_lon = ()

    విలువ_max = దశాంశం('-inf')
    విలువ_max_lat_lon = ()

    కొరకు latitude, lat_అడ్డువరుస లో world_magnitude_model.వస్తువులు_2():
        #print(latitude, lat_row)
        కొరకు longitude, ఫలితం లో lat_అడ్డువరుస.వస్తువులు_2():
            #print(result)

            విలువ = దశాంశం(ఫలితం[కీలకం_name])

            ఒకవేళ (విలువ > విలువ_max):
                విలువ_max = విలువ
                విలువ_max_lat_lon = (latitude, longitude)

            ఒకవేళ (విలువ < విలువ_min):
                విలువ_min = విలువ
                విలువ_min_lat_lon = (latitude, longitude)

    # scale the values to fit into int16_t
    విలువ_scale_max = abs(numpy.iinfo(numpy.int16).max) / abs(విలువ_max)
    విలువ_scale_min = abs(numpy.iinfo(numpy.int16).min) / abs(విలువ_min)
    విలువ_scale = min(విలువ_scale_max, విలువ_scale_min)

    units_str = world_magnitude_model_units[కీలకం_name].విభజించు(' ')[0]

    # print the table
    ముద్రించు('// Magnetic {} data in {:.4g} {}'.format(కీలకం_name, 1.0 / విలువ_scale, units_str))
    ముద్రించు('// Model: {},'.format(దత్తాంశం['model']))
    ముద్రించు('// Version: {},'.format(దత్తాంశం['version']))
    ముద్రించు('// Date: {},'.format(దత్తాంశం['result'][0]['date']))
    ముద్రించు('static constexpr const int16_t {}_table[{}][{}]'.format(కీలకం_name, LAT_DIM, LON_DIM) + " {")
    ముద్రించు('\t//    LONGITUDE: ', చివర='')
    కొరకు l లో శ్రేణి(SAMPLING_MIN_LON, SAMPLING_MAX_LON+1, SAMPLING_RES):
        ముద్రించు('{0:6d},'.format(l), చివర='')
    ముద్రించు('')

    కొరకు latitude, lat_అడ్డువరుస లో world_magnitude_model.వస్తువులు_2():
        ముద్రించు('\t/* LAT: {0:3d} */'.format(latitude) + ' { ', చివర='')
        latitude_blackout_zone = అబద్ధం

        కొరకు longitude, ఫలితం లో lat_అడ్డువరుస.వస్తువులు_2():

            విలువ = దశాంశం(ఫలితం[కీలకం_name])

            # value scaled to fit into int16_t
            విలువ_int = పూర్ణాంకం(round(విలువ * విలువ_scale))
            ముద్రించు('{0:6d},'.format(విలువ_int), చివర='')

            # blackout warning at this latitude
            ప్రయత్నించు:
                ఒకవేళ ఫలితం['warning']:
                    latitude_blackout_zone = నిజం

            మినహాయింపు:
                దాటు

        ఒకవేళ latitude_blackout_zone:
            ముద్రించు(' }, // WARNING! black out zone')
        లేకపోతే:
            ముద్రించు(' },')

    ముద్రించు("};")

    ముద్రించు('static constexpr float WMM_{}_SCALE_TO_{} = {:.9g}f;'.format(కీలకం_name.పై(), units_str.పై(), 1.0 / విలువ_scale))
    ముద్రించు('static constexpr float WMM_{}_MIN_{} = {:.1f}f; // latitude: {:.0f}, longitude: {:.0f}'.format(కీలకం_name.పై(), units_str.పై(), విలువ_min, విలువ_min_lat_lon[0], విలువ_min_lat_lon[1]))
    ముద్రించు('static constexpr float WMM_{}_MAX_{} = {:.1f}f; // latitude: {:.0f}, longitude: {:.0f}'.format(కీలకం_name.పై(), units_str.పై(), విలువ_max, విలువ_max_lat_lon[0], విలువ_max_lat_lon[1]))
    ముద్రించు("\n")


print_wmm_పట్టిక('declination')
print_wmm_పట్టిక('inclination')
print_wmm_పట్టిక('totalintensity')
