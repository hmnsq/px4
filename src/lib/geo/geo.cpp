/****************************************************************************
 *
 *   Copyright (c) 2012-2021 PX4 Development Team. All rights reserved.
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

/**
 * @file geo.cpp
 *
 * Geo / math functions to perform geodesic calculations
 *
 * @author Thomas Gubler <thomasgubler@student.ethz.ch>
 * @author Julian Oes <joes@student.ethz.ch>
 * @author Lorenz Meier <lm@inf.ethz.ch>
 * @author Anton Babushkin <anton.babushkin@me.com>
 */

#అన్తర్భావ "geo.h"

#అన్తర్భావ <float.h>

ఉపయోగించి మాతృక::wrap_pi;
ఉపయోగించి మాతృక::wrap_2pi;

/*
 * Azimuthal Equidistant Projection
 * formulas according to: http://mathworld.wolfram.com/AzimuthalEquidistantProjection.html
 */

శూన్య MapProjection::initReference(ద్విగుణ lat_0, ద్విగుణ lon_0, uint64_t timestamp)
{
	_ref_timestamp = timestamp;
	_ref_lat = math::radians(lat_0);
	_ref_lon = math::radians(lon_0);
	_ref_sin_lat = జ్యా(_ref_lat);
	_ref_cos_lat = కోజ్యా(_ref_lat);
	ref_init_పూర్తి = నిజం;
}

శూన్య MapProjection::project(ద్విగుణ lat, ద్విగుణ lon, దశాంశం &x, దశాంశం &y) స్థిరరాశి
{
	స్థిరరాశి ద్విగుణ lat_rad = math::radians(lat);
	స్థిరరాశి ద్విగుణ lon_rad = math::radians(lon);

	స్థిరరాశి ద్విగుణ sin_lat = జ్యా(lat_rad);
	స్థిరరాశి ద్విగుణ cos_lat = కోజ్యా(lat_rad);

	స్థిరరాశి ద్విగుణ cos_d_lon = కోజ్యా(lon_rad - _ref_lon);

	స్థిరరాశి ద్విగుణ arg = math::constrain(_ref_sin_lat * sin_lat + _ref_cos_lat * cos_lat * cos_d_lon, -1.0,  1.0);
	స్థిరరాశి ద్విగుణ c = acos(arg);

	ద్విగుణ k = 1.0;

	ఒకవేళ (fabs(c) > 0) {
		k = (c / జ్యా(c));
	}

	x = static_cast<దశాంశం>(k * (_ref_cos_lat * sin_lat - _ref_sin_lat * cos_lat * cos_d_lon) * CONSTANTS_RADIUS_యొక్క_EARTH);
	y = static_cast<దశాంశం>(k * cos_lat * జ్యా(lon_rad - _ref_lon) * CONSTANTS_RADIUS_యొక్క_EARTH);
}

శూన్య MapProjection::reproject(దశాంశం x, దశాంశం y, ద్విగుణ &lat, ద్విగుణ &lon) స్థిరరాశి
{
	స్థిరరాశి ద్విగుణ x_rad = (ద్విగుణ)x / CONSTANTS_RADIUS_యొక్క_EARTH;
	స్థిరరాశి ద్విగుణ y_rad = (ద్విగుణ)y / CONSTANTS_RADIUS_యొక్క_EARTH;
	స్థిరరాశి ద్విగుణ c = వర్గమూలం(x_rad * x_rad + y_rad * y_rad);

	ఒకవేళ (fabs(c) > 0) {
		స్థిరరాశి ద్విగుణ sin_c = జ్యా(c);
		స్థిరరాశి ద్విగుణ cos_c = కోజ్యా(c);

		స్థిరరాశి ద్విగుణ lat_rad = asin(cos_c * _ref_sin_lat + (x_rad * sin_c * _ref_cos_lat) / c);
		స్థిరరాశి ద్విగుణ lon_rad = (_ref_lon + atan2(y_rad * sin_c, c * _ref_cos_lat * cos_c - x_rad * _ref_sin_lat * sin_c));

		lat = math::degrees(lat_rad);
		lon = math::degrees(lon_rad);

	} లేకపోతే {
		lat = math::degrees(_ref_lat);
		lon = math::degrees(_ref_lon);
	}
}

దశాంశం get_దూరం_గమ్యం_తదుపరి_waypoint(ద్విగుణ lat_now, ద్విగుణ lon_now, ద్విగుణ lat_తదుపరి, ద్విగుణ lon_తదుపరి)
{
	స్థిరరాశి ద్విగుణ lat_now_rad = math::radians(lat_now);
	స్థిరరాశి ద్విగుణ lat_తదుపరి_rad = math::radians(lat_తదుపరి);

	స్థిరరాశి ద్విగుణ d_lat = lat_తదుపరి_rad - lat_now_rad;
	స్థిరరాశి ద్విగుణ d_lon = math::radians(lon_తదుపరి) - math::radians(lon_now);

	స్థిరరాశి ద్విగుణ a = జ్యా(d_lat / 2.0) * జ్యా(d_lat / 2.0) + జ్యా(d_lon / 2.0) * జ్యా(d_lon / 2.0) * కోజ్యా(lat_now_rad) * కోజ్యా(
				 lat_తదుపరి_rad);

	స్థిరరాశి ద్విగుణ c = atan2(వర్గమూలం(a), వర్గమూలం(1.0 - a));

	తిరిగి static_cast<దశాంశం>(CONSTANTS_RADIUS_యొక్క_EARTH * 2.0 * c);
}

శూన్య create_waypoint_from_పంక్తి_and_dist(ద్విగుణ lat_A, ద్విగుణ lon_A, ద్విగుణ lat_B, ద్విగుణ lon_B, దశాంశం dist,
					ద్విగుణ *lat_లక్ష్యం, ద్విగుణ *lon_లక్ష్యం)
{
	ఒకవేళ (fabsf(dist) < FLT_EPSILON) {
		*lat_లక్ష్యం = lat_A;
		*lon_లక్ష్యం = lon_A;

	} లేకపోతే {
		దశాంశం heading = get_bearing_గమ్యం_తదుపరి_waypoint(lat_A, lon_A, lat_B, lon_B);
		waypoint_from_heading_and_దూరం(lat_A, lon_A, heading, dist, lat_లక్ష్యం, lon_లక్ష్యం);
	}
}

శూన్య waypoint_from_heading_and_దూరం(ద్విగుణ lat_ప్రారంభం, ద్విగుణ lon_ప్రారంభం, దశాంశం bearing, దశాంశం dist,
					ద్విగుణ *lat_లక్ష్యం, ద్విగుణ *lon_లక్ష్యం)
{
	bearing = wrap_2pi(bearing);

	ద్విగుణ radius_నిష్పత్తి = static_cast<ద్విగుణ>(dist) / CONSTANTS_RADIUS_యొక్క_EARTH;

	ద్విగుణ lat_ప్రారంభం_rad = math::radians(lat_ప్రారంభం);
	ద్విగుణ lon_ప్రారంభం_rad = math::radians(lon_ప్రారంభం);

	*lat_లక్ష్యం = asin(జ్యా(lat_ప్రారంభం_rad) * కోజ్యా(radius_నిష్పత్తి) + కోజ్యా(lat_ప్రారంభం_rad) * జ్యా(radius_నిష్పత్తి) * కోజ్యా((
				   ద్విగుణ)bearing));
	*lon_లక్ష్యం = lon_ప్రారంభం_rad + atan2(జ్యా((ద్విగుణ)bearing) * జ్యా(radius_నిష్పత్తి) * కోజ్యా(lat_ప్రారంభం_rad),
					    కోజ్యా(radius_నిష్పత్తి) - జ్యా(lat_ప్రారంభం_rad) * జ్యా(*lat_లక్ష్యం));

	*lat_లక్ష్యం = math::degrees(*lat_లక్ష్యం);
	*lon_లక్ష్యం = math::degrees(*lon_లక్ష్యం);
}

దశాంశం get_bearing_గమ్యం_తదుపరి_waypoint(ద్విగుణ lat_now, ద్విగుణ lon_now, ద్విగుణ lat_తదుపరి, ద్విగుణ lon_తదుపరి)
{
	స్థిరరాశి ద్విగుణ lat_now_rad = math::radians(lat_now);
	స్థిరరాశి ద్విగుణ lat_తదుపరి_rad = math::radians(lat_తదుపరి);

	స్థిరరాశి ద్విగుణ cos_lat_తదుపరి = కోజ్యా(lat_తదుపరి_rad);
	స్థిరరాశి ద్విగుణ d_lon = math::radians(lon_తదుపరి - lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */

	స్థిరరాశి దశాంశం y = static_cast<దశాంశం>(జ్యా(d_lon) * cos_lat_తదుపరి);
	స్థిరరాశి దశాంశం x = static_cast<దశాంశం>(కోజ్యా(lat_now_rad) * జ్యా(lat_తదుపరి_rad) - జ్యా(lat_now_rad) * cos_lat_తదుపరి * కోజ్యా(d_lon));

	తిరిగి wrap_pi(atan2f(y, x));
}

శూన్య
get_సదిశ_గమ్యం_తదుపరి_waypoint(ద్విగుణ lat_now, ద్విగుణ lon_now, ద్విగుణ lat_తదుపరి, ద్విగుణ lon_తదుపరి, దశాంశం *v_n, దశాంశం *v_e)
{
	స్థిరరాశి ద్విగుణ lat_now_rad = math::radians(lat_now);
	స్థిరరాశి ద్విగుణ lat_తదుపరి_rad = math::radians(lat_తదుపరి);
	స్థిరరాశి ద్విగుణ d_lon = math::radians(lon_తదుపరి) - math::radians(lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<దశాంశం>(CONSTANTS_RADIUS_యొక్క_EARTH * (కోజ్యా(lat_now_rad) * జ్యా(lat_తదుపరి_rad) - జ్యా(lat_now_rad) * కోజ్యా(
					  lat_తదుపరి_rad) * కోజ్యా(d_lon)));
	*v_e = static_cast<దశాంశం>(CONSTANTS_RADIUS_యొక్క_EARTH * జ్యా(d_lon) * కోజ్యా(lat_తదుపరి_rad));
}

శూన్య
get_సదిశ_గమ్యం_తదుపరి_waypoint_fast(ద్విగుణ lat_now, ద్విగుణ lon_now, ద్విగుణ lat_తదుపరి, ద్విగుణ lon_తదుపరి, దశాంశం *v_n,
				 దశాంశం *v_e)
{
	ద్విగుణ lat_now_rad = math::radians(lat_now);
	ద్విగుణ lon_now_rad = math::radians(lon_now);
	ద్విగుణ lat_తదుపరి_rad = math::radians(lat_తదుపరి);
	ద్విగుణ lon_తదుపరి_rad = math::radians(lon_తదుపరి);

	ద్విగుణ d_lat = lat_తదుపరి_rad - lat_now_rad;
	ద్విగుణ d_lon = lon_తదుపరి_rad - lon_now_rad;

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<దశాంశం>(CONSTANTS_RADIUS_యొక్క_EARTH * d_lat);
	*v_e = static_cast<దశాంశం>(CONSTANTS_RADIUS_యొక్క_EARTH * d_lon * కోజ్యా(lat_now_rad));
}

శూన్య కూడు_సదిశ_గమ్యం_global_position(ద్విగుణ lat_now, ద్విగుణ lon_now, దశాంశం v_n, దశాంశం v_e, ద్విగుణ *lat_res,
				   ద్విగుణ *lon_res)
{
	ద్విగుణ lat_now_rad = math::radians(lat_now);
	ద్విగుణ lon_now_rad = math::radians(lon_now);

	*lat_res = math::degrees(lat_now_rad + (ద్విగుణ)v_n / CONSTANTS_RADIUS_యొక్క_EARTH);
	*lon_res = math::degrees(lon_now_rad + (ద్విగుణ)v_e / (CONSTANTS_RADIUS_యొక్క_EARTH * కోజ్యా(lat_now_rad)));
}

// Additional functions - @author Doug Weibel <douglas.weibel@colorado.edu>

పూర్ణాంకం get_దూరం_గమ్యం_పంక్తి(నిర్మాణం crosstrack_error_s &crosstrack_error, ద్విగుణ lat_now, ద్విగుణ lon_now,
			 ద్విగుణ lat_ప్రారంభం, ద్విగుణ lon_ప్రారంభం, ద్విగుణ lat_చివర, ద్విగుణ lon_చివర)
{
	// This function returns the distance to the nearest point on the track line.  Distance is positive if current
	// position is right of the track and negative if left of the track as seen from a point on the track line
	// headed towards the end point.

	పూర్ణాంకం return_విలువ = -1;	// Set error flag, cleared when valid result calculated.
	crosstrack_error.past_చివర = అబద్ధం;
	crosstrack_error.దూరం = 0.0f;
	crosstrack_error.bearing = 0.0f;

	దశాంశం dist_గమ్యం_చివర = get_దూరం_గమ్యం_తదుపరి_waypoint(lat_now, lon_now, lat_చివర, lon_చివర);

	// Return error if arguments are bad
	ఒకవేళ (dist_గమ్యం_చివర < 0.1f) {
		తిరిగి -1;
	}

	దశాంశం bearing_చివర = get_bearing_గమ్యం_తదుపరి_waypoint(lat_now, lon_now, lat_చివర, lon_చివర);
	దశాంశం bearing_track = get_bearing_గమ్యం_తదుపరి_waypoint(lat_ప్రారంభం, lon_ప్రారంభం, lat_చివర, lon_చివర);
	దశాంశం bearing_diff = wrap_pi(bearing_track - bearing_చివర);

	// Return past_end = true if past end point of line
	ఒకవేళ (bearing_diff > M_PI_2_F || bearing_diff < -M_PI_2_F) {
		crosstrack_error.past_చివర = నిజం;
		return_విలువ = 0;
		తిరిగి return_విలువ;
	}

	crosstrack_error.దూరం = (dist_గమ్యం_చివర) * sinf(bearing_diff);

	ఒకవేళ (sinf(bearing_diff) >= 0) {
		crosstrack_error.bearing = wrap_pi(bearing_track - M_PI_2_F);

	} లేకపోతే {
		crosstrack_error.bearing = wrap_pi(bearing_track + M_PI_2_F);
	}

	return_విలువ = 0;

	తిరిగి return_విలువ;
}

పూర్ణాంకం get_దూరం_గమ్యం_arc(నిర్మాణం crosstrack_error_s *crosstrack_error, ద్విగుణ lat_now, ద్విగుణ lon_now,
			ద్విగుణ lat_center, ద్విగుణ lon_center,
			దశాంశం radius, దశాంశం arc_ప్రారంభం_bearing, దశాంశం arc_sweep)
{
	// This function returns the distance to the nearest point on the track arc.  Distance is positive if current
	// position is right of the arc and negative if left of the arc as seen from the closest point on the arc and
	// headed towards the end point.

	// Determine if the current position is inside or outside the sector between the line from the center
	// to the arc start and the line from the center to the arc end
	స్థిరరాశి దశాంశం bearing_now = get_bearing_గమ్యం_తదుపరి_waypoint(lat_now, lon_now, lat_center, lon_center);

	పూర్ణాంకం return_విలువ = -1;		// Set error flag, cleared when valid result calculated.
	crosstrack_error->past_చివర = అబద్ధం;
	crosstrack_error->దూరం = 0.0f;
	crosstrack_error->bearing = 0.0f;

	// Return error if arguments are bad
	ఒకవేళ (radius < 0.1f) {
		తిరిగి return_విలువ;
	}

	// arc_start_bearing and arc_sweep are referenced to the center, so test membership with the
	// center->vehicle bearing. Measuring it in the direction of the sweep handles either sign and
	// any zero crossing without special cases.
	// NOLINTNEXTLINE(readability-suspicious-call-argument) intentional reverse bearing: center -> vehicle
	స్థిరరాశి దశాంశం bearing_center_గమ్యం_now = get_bearing_గమ్యం_తదుపరి_waypoint(lat_center, lon_center, lat_now, lon_now);
	స్థిరరాశి దశాంశం bearing_from_ప్రారంభం = wrap_2pi((bearing_center_గమ్యం_now - arc_ప్రారంభం_bearing) * (arc_sweep < 0.0f ? -1.0f :
					 1.0f));
	స్థిరరాశి తార్కిక లోపల_sector = bearing_from_ప్రారంభం <= fabsf(arc_sweep);

	// If in the sector then calculate distance and bearing to closest point
	ఒకవేళ (లోపల_sector) {
		crosstrack_error->past_చివర = అబద్ధం;
		దశాంశం dist_గమ్యం_center = get_దూరం_గమ్యం_తదుపరి_waypoint(lat_now, lon_now, lat_center, lon_center);

		ఒకవేళ (dist_గమ్యం_center <= radius) {
			crosstrack_error->దూరం = radius - dist_గమ్యం_center;
			crosstrack_error->bearing = bearing_now + M_PI_F;

		} లేకపోతే {
			crosstrack_error->దూరం = dist_గమ్యం_center - radius;
			crosstrack_error->bearing = bearing_now;
		}

		// If out of the sector then calculate dist and bearing to start or end point

	} లేకపోతే {

		// Use the approximation  that 111,111 meters in the y direction is 1 degree (of latitude)
		// and 111,111 * cos(latitude) meters in the x direction is 1 degree (of longitude) to
		// calculate the position of the start and end points.  We should not be doing this often
		// as this function generally will not be called repeatedly when we are out of the sector.

		ద్విగుణ ప్రారంభం_disp_x = (ద్విగుణ)radius * జ్యా((ద్విగుణ)arc_ప్రారంభం_bearing);
		ద్విగుణ ప్రారంభం_disp_y = (ద్విగుణ)radius * కోజ్యా((ద్విగుణ)arc_ప్రారంభం_bearing);
		ద్విగుణ చివర_disp_x = (ద్విగుణ)radius * జ్యా((ద్విగుణ)wrap_pi(arc_ప్రారంభం_bearing + arc_sweep));
		ద్విగుణ చివర_disp_y = (ద్విగుణ)radius * కోజ్యా((ద్విగుణ)wrap_pi(arc_ప్రారంభం_bearing + arc_sweep));
		స్థిరరాశి ద్విగుణ cos_lat_center = కోజ్యా(math::radians(lat_center));
		ద్విగుణ lon_ప్రారంభం = lon_center + ప్రారంభం_disp_x / (111111.0 * cos_lat_center);
		ద్విగుణ lat_ప్రారంభం = lat_center + ప్రారంభం_disp_y / 111111.0;
		ద్విగుణ lon_చివర = lon_center + చివర_disp_x / (111111.0 * cos_lat_center);
		ద్విగుణ lat_చివర = lat_center + చివర_disp_y / 111111.0;
		దశాంశం dist_గమ్యం_ప్రారంభం = get_దూరం_గమ్యం_తదుపరి_waypoint(lat_now, lon_now, lat_ప్రారంభం, lon_ప్రారంభం);
		దశాంశం dist_గమ్యం_చివర = get_దూరం_గమ్యం_తదుపరి_waypoint(lat_now, lon_now, lat_చివర, lon_చివర);

		ఒకవేళ (dist_గమ్యం_ప్రారంభం < dist_గమ్యం_చివర) {
			crosstrack_error->దూరం = dist_గమ్యం_ప్రారంభం;
			crosstrack_error->bearing = get_bearing_గమ్యం_తదుపరి_waypoint(lat_now, lon_now, lat_ప్రారంభం, lon_ప్రారంభం);

		} లేకపోతే {
			crosstrack_error->past_చివర = నిజం;
			crosstrack_error->దూరం = dist_గమ్యం_చివర;
			crosstrack_error->bearing = get_bearing_గమ్యం_తదుపరి_waypoint(lat_now, lon_now, lat_చివర, lon_చివర);
		}
	}

	crosstrack_error->bearing = wrap_pi(crosstrack_error->bearing);
	return_విలువ = 0;

	తిరిగి return_విలువ;
}

దశాంశం get_దూరం_గమ్యం_బిందువు_global_wgs84(ద్విగుణ lat_now, ద్విగుణ lon_now, దశాంశం alt_now,
		ద్విగుణ lat_తదుపరి, ద్విగుణ lon_తదుపరి, దశాంశం alt_తదుపరి,
		దశాంశం *dist_xy, దశాంశం *dist_z)
{
	ద్విగుణ current_x_rad = math::radians(lat_తదుపరి);
	ద్విగుణ current_y_rad = math::radians(lon_తదుపరి);
	ద్విగుణ x_rad = math::radians(lat_now);
	ద్విగుణ y_rad = math::radians(lon_now);

	ద్విగుణ d_lat = x_rad - current_x_rad;
	ద్విగుణ d_lon = y_rad - current_y_rad;

	ద్విగుణ a = జ్యా(d_lat / 2.0) * జ్యా(d_lat / 2.0) + జ్యా(d_lon / 2.0) * జ్యా(d_lon / 2.0) * కోజ్యా(current_x_rad) * కోజ్యా(x_rad);
	ద్విగుణ c = 2 * atan2(వర్గమూలం(a), వర్గమూలం(1 - a));

	స్థిరరాశి దశాంశం dxy = static_cast<దశాంశం>(CONSTANTS_RADIUS_యొక్క_EARTH * c);
	స్థిరరాశి దశాంశం dz = static_cast<దశాంశం>(alt_now - alt_తదుపరి);

	*dist_xy = fabsf(dxy);
	*dist_z = fabsf(dz);

	తిరిగి sqrtf(dxy * dxy + dz * dz);
}

దశాంశం mavlink_wpm_దూరం_గమ్యం_బిందువు_local(దశాంశం x_now, దశాంశం y_now, దశాంశం z_now,
		దశాంశం x_తదుపరి, దశాంశం y_తదుపరి, దశాంశం z_తదుపరి,
		దశాంశం *dist_xy, దశాంశం *dist_z)
{
	దశాంశం dx = x_now - x_తదుపరి;
	దశాంశం dy = y_now - y_తదుపరి;
	దశాంశం dz = z_now - z_తదుపరి;

	*dist_xy = sqrtf(dx * dx + dy * dy);
	*dist_z = fabsf(dz);

	తిరిగి sqrtf(dx * dx + dy * dy + dz * dz);
}
