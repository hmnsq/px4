/****************************************************************************
 *
 *   Copyright (c) 2023-2026 PX4 Development Team. All rights reserved.
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

#అన్తర్భావ "AdsbConflict.h"

#అన్తర్భావ <cmath>

#అన్తర్భావ <px4_platform_common/defines.h>
#అన్తర్భావ <px4_platform_common/log.h>

తార్కిక AdsbConflict::valid_wgs84_coordinates(స్థిరరాశి ద్విగుణ latitude, స్థిరరాశి ద్విగుణ longitude)
{
	తిరిగి PX4_ISFINITE(latitude) && PX4_ISFINITE(longitude)
	       && fabs(latitude) <= 90.0
	       && fabs(longitude) <= 180.0;
}

తార్కిక AdsbConflict::calculate_daa_output(స్థిరరాశి daa_ఆదానం_s &daa_ఆదానం, detect_and_avoid_s &daa_output)
{
	స్థిరరాశి transponder_report_s &transponder_report = daa_ఆదానం.transponder_report;

	స్థిరరాశి మాతృక::Vector2d traffic_lat_lon{transponder_report.lat, transponder_report.lon};

	ఒకవేళ (!valid_wgs84_coordinates(daa_ఆదానం.uav_lat_lon(0), daa_ఆదానం.uav_lat_lon(1))
	    || !valid_wgs84_coordinates(traffic_lat_lon(0), traffic_lat_lon(1))) {
		PX4_DEBUG("DAA lib: invalid coordinates");
		తిరిగి అబద్ధం;
	}

	ఒకవేళ (!PX4_ISFINITE(daa_ఆదానం.uav_alt) || !PX4_ISFINITE(transponder_report.altitude)) {
		PX4_DEBUG("DAA lib: invalid altitude");
		తిరిగి అబద్ధం;
	}

	ఒకవేళ (!(PX4_ISFINITE(transponder_report.hor_velocity) && transponder_report.hor_velocity >= 0.f
	      && PX4_ISFINITE(transponder_report.ver_velocity))) {
		PX4_DEBUG("DAA lib: invalid traffic velocity");
		తిరిగి అబద్ధం;
	}

	ఒకవేళ (!daa_ఆదానం.uav_vel_ned.isAllFinite()) {
		PX4_DEBUG("DAA lib: invalid ownship velocity");
		తిరిగి అబద్ధం;
	}

#ఒకవేళ !defined(CONFIG_NAVIGATOR_ADSB_F3442) || !CONFIG_NAVIGATOR_ADSB_F3442

	ఒకవేళ (!PX4_ISFINITE(transponder_report.heading)) {
		PX4_DEBUG("DAA lib: invalid traffic heading");
		తిరిగి అబద్ధం;
	}

#అయితే_ముగింపు // !CONFIG_NAVIGATOR_ADSB_F3442

	aircraft_state_s uav_state{};
	uav_state.lat_lon = daa_ఆదానం.uav_lat_lon;
	uav_state.altitude = daa_ఆదానం.uav_alt;
	uav_state.velocity_ned = daa_ఆదానం.uav_vel_ned;

	aircraft_state_s traffic_state{};
	traffic_state.lat_lon = traffic_lat_lon;
	traffic_state.altitude = transponder_report.altitude;
	traffic_state.heading = transponder_report.heading;

	ఒకవేళ (PX4_ISFINITE(transponder_report.heading)) {
		traffic_state.velocity_ned = మాతృక::Vector3f(
						     cosf(transponder_report.heading) * transponder_report.hor_velocity,
						     sinf(transponder_report.heading) * transponder_report.hor_velocity,
						     -transponder_report.ver_velocity);

	} లేకపోతే {
		traffic_state.velocity_ned = మాతృక::Vector3f(transponder_report.hor_velocity, 0.f, -transponder_report.ver_velocity);
	}

	daa_stats_s daa_stats{};
	daa_output.conflict_level = _daa.calculate_daa_stats(uav_state, traffic_state, daa_stats);
	daa_output.aircraft_dist = daa_stats.aircraft_dist;
	daa_output.aircraft_dist_hor = daa_stats.aircraft_dist_hor;
	daa_output.aircraft_dist_vert = daa_stats.aircraft_dist_vert;
	daa_output.expected_min_dist_సమయం = daa_stats.expected_min_dist_సమయం_sec;

	తిరిగి నిజం;
}

తార్కిక AdsbConflict::try_updating_params()
{
	తిరిగి _daa.try_setting_params();
}
