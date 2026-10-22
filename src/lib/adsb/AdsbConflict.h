/****************************************************************************
 *
 *   Copyright (c) 2012-2026 PX4 Development Team. All rights reserved.
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

#సూచన once

// Provides CONFIG_NAVIGATOR_ADSB_F3442 for the standard selection.
#అన్తర్భావ <px4_boardconfig.h>

#ఒకవేళ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
#అన్తర్భావ "f3442_standard/DaaF3442.h"
#లేకపోతే
#అన్తర్భావ "crosstrack_standard/DaaCrosstrack.h"
#అయితే_ముగింపు

#అన్తర్భావ <matrix/math.hpp>

#అన్తర్భావ <uORB/topics/detect_and_avoid.h>
#అన్తర్భావ <uORB/topics/transponder_report.h>

నిర్మాణం daa_ఆదానం_s {
	మాతృక::Vector2d uav_lat_lon{};
	దశాంశం uav_alt{0.f};
	మాతృక::Vector3f uav_vel_ned{};
	transponder_report_s transponder_report{};
};

class AdsbConflict
{
బహిరంగ:
	AdsbConflict() = అప్రమేయ;
	~AdsbConflict() = అప్రమేయ;

	/**
	 * @brief Validate ownship + transponder inputs and compute the DAA output.
	 * Returns false on non-finite inputs or when the built standard needs a heading the report does not provide.
	*/
	bool calculate_daa_output(స్థిరరాశి daa_ఆదానం_s &daa_ఆదానం, detect_and_avoid_s &daa_output);

	bool try_updating_params();

	స్థిర bool valid_wgs84_coordinates(స్థిరరాశి ద్విగుణ latitude, స్థిరరాశి ద్విగుణ longitude);

అంతరంగ:
#ఒకవేళ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	DaaF3442 _daa;
#లేకపోతే
	DaaCrosstrack _daa;
#అయితే_ముగింపు
};
