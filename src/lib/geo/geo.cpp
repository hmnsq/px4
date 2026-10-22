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

#ଅନ୍ତର୍ଭାଵ "geo.h"

#ଅନ୍ତର୍ଭାଵ <float.h>

using matrix::wrap_pi;
using matrix::wrap_2pi;

/*
 * Azimuthal Equidistant Projection
 * formulas according to: http://mathworld.wolfram.com/AzimuthalEquidistantProjection.html
 */

ଶୂନ୍ୟ MapProjection::initReference(ଦ୍ୱିଗୁଣ lat_0, ଦ୍ୱିଗୁଣ lon_0, uint64_t timestamp)
{
	_ref_timestamp = timestamp;
	_ref_lat = math::radians(lat_0);
	_ref_lon = math::radians(lon_0);
	_ref_sin_lat = sin(_ref_lat);
	_ref_cos_lat = cos(_ref_lat);
	_ref_init_done = ସତ୍ୟ;
}

ଶୂନ୍ୟ MapProjection::project(ଦ୍ୱିଗୁଣ lat, ଦ୍ୱିଗୁଣ lon, ଦଶମିକ &x, ଦଶମିକ &y) ଅଚଳ
{
	ଅଚଳ ଦ୍ୱିଗୁଣ lat_rad = math::radians(lat);
	ଅଚଳ ଦ୍ୱିଗୁଣ lon_rad = math::radians(lon);

	ଅଚଳ ଦ୍ୱିଗୁଣ sin_lat = sin(lat_rad);
	ଅଚଳ ଦ୍ୱିଗୁଣ cos_lat = cos(lat_rad);

	ଅଚଳ ଦ୍ୱିଗୁଣ cos_d_lon = cos(lon_rad - _ref_lon);

	ଅଚଳ ଦ୍ୱିଗୁଣ arg = math::constrain(_ref_sin_lat * sin_lat + _ref_cos_lat * cos_lat * cos_d_lon, -1.0,  1.0);
	ଅଚଳ ଦ୍ୱିଗୁଣ c = acos(arg);

	ଦ୍ୱିଗୁଣ k = 1.0;

	ଯଦି (fabs(c) > 0) {
		k = (c / sin(c));
	}

	x = static_cast<ଦଶମିକ>(k * (_ref_cos_lat * sin_lat - _ref_sin_lat * cos_lat * cos_d_lon) * CONSTANTS_RADIUS_OF_EARTH);
	y = static_cast<ଦଶମିକ>(k * cos_lat * sin(lon_rad - _ref_lon) * CONSTANTS_RADIUS_OF_EARTH);
}

ଶୂନ୍ୟ MapProjection::reproject(ଦଶମିକ x, ଦଶମିକ y, ଦ୍ୱିଗୁଣ &lat, ଦ୍ୱିଗୁଣ &lon) ଅଚଳ
{
	ଅଚଳ ଦ୍ୱିଗୁଣ x_rad = (ଦ୍ୱିଗୁଣ)x / CONSTANTS_RADIUS_OF_EARTH;
	ଅଚଳ ଦ୍ୱିଗୁଣ y_rad = (ଦ୍ୱିଗୁଣ)y / CONSTANTS_RADIUS_OF_EARTH;
	ଅଚଳ ଦ୍ୱିଗୁଣ c = sqrt(x_rad * x_rad + y_rad * y_rad);

	ଯଦି (fabs(c) > 0) {
		ଅଚଳ ଦ୍ୱିଗୁଣ sin_c = sin(c);
		ଅଚଳ ଦ୍ୱିଗୁଣ cos_c = cos(c);

		ଅଚଳ ଦ୍ୱିଗୁଣ lat_rad = asin(cos_c * _ref_sin_lat + (x_rad * sin_c * _ref_cos_lat) / c);
		ଅଚଳ ଦ୍ୱିଗୁଣ lon_rad = (_ref_lon + atan2(y_rad * sin_c, c * _ref_cos_lat * cos_c - x_rad * _ref_sin_lat * sin_c));

		lat = math::degrees(lat_rad);
		lon = math::degrees(lon_rad);

	} ଅନ୍ୟଥା {
		lat = math::degrees(_ref_lat);
		lon = math::degrees(_ref_lon);
	}
}

ଦଶମିକ get_distance_to_next_waypoint(ଦ୍ୱିଗୁଣ lat_now, ଦ୍ୱିଗୁଣ lon_now, ଦ୍ୱିଗୁଣ lat_next, ଦ୍ୱିଗୁଣ lon_next)
{
	ଅଚଳ ଦ୍ୱିଗୁଣ lat_now_rad = math::radians(lat_now);
	ଅଚଳ ଦ୍ୱିଗୁଣ lat_next_rad = math::radians(lat_next);

	ଅଚଳ ଦ୍ୱିଗୁଣ d_lat = lat_next_rad - lat_now_rad;
	ଅଚଳ ଦ୍ୱିଗୁଣ d_lon = math::radians(lon_next) - math::radians(lon_now);

	ଅଚଳ ଦ୍ୱିଗୁଣ a = sin(d_lat / 2.0) * sin(d_lat / 2.0) + sin(d_lon / 2.0) * sin(d_lon / 2.0) * cos(lat_now_rad) * cos(
				 lat_next_rad);

	ଅଚଳ ଦ୍ୱିଗୁଣ c = atan2(sqrt(a), sqrt(1.0 - a));

	ଫେରନ୍ତୁ static_cast<ଦଶମିକ>(CONSTANTS_RADIUS_OF_EARTH * 2.0 * c);
}

ଶୂନ୍ୟ create_waypoint_from_ଧାଡ଼ି_and_dist(ଦ୍ୱିଗୁଣ lat_A, ଦ୍ୱିଗୁଣ lon_A, ଦ୍ୱିଗୁଣ lat_B, ଦ୍ୱିଗୁଣ lon_B, ଦଶମିକ dist,
					ଦ୍ୱିଗୁଣ *lat_target, ଦ୍ୱିଗୁଣ *lon_target)
{
	ଯଦି (fabsf(dist) < FLT_EPSILON) {
		*lat_target = lat_A;
		*lon_target = lon_A;

	} ଅନ୍ୟଥା {
		ଦଶମିକ heading = get_bearing_to_next_waypoint(lat_A, lon_A, lat_B, lon_B);
		waypoint_from_heading_and_distance(lat_A, lon_A, heading, dist, lat_target, lon_target);
	}
}

ଶୂନ୍ୟ waypoint_from_heading_and_distance(ଦ୍ୱିଗୁଣ lat_start, ଦ୍ୱିଗୁଣ lon_start, ଦଶମିକ bearing, ଦଶମିକ dist,
					ଦ୍ୱିଗୁଣ *lat_target, ଦ୍ୱିଗୁଣ *lon_target)
{
	bearing = wrap_2pi(bearing);

	ଦ୍ୱିଗୁଣ radius_ratio = static_cast<ଦ୍ୱିଗୁଣ>(dist) / CONSTANTS_RADIUS_OF_EARTH;

	ଦ୍ୱିଗୁଣ lat_start_rad = math::radians(lat_start);
	ଦ୍ୱିଗୁଣ lon_start_rad = math::radians(lon_start);

	*lat_target = asin(sin(lat_start_rad) * cos(radius_ratio) + cos(lat_start_rad) * sin(radius_ratio) * cos((
				   ଦ୍ୱିଗୁଣ)bearing));
	*lon_target = lon_start_rad + atan2(sin((ଦ୍ୱିଗୁଣ)bearing) * sin(radius_ratio) * cos(lat_start_rad),
					    cos(radius_ratio) - sin(lat_start_rad) * sin(*lat_target));

	*lat_target = math::degrees(*lat_target);
	*lon_target = math::degrees(*lon_target);
}

ଦଶମିକ get_bearing_to_next_waypoint(ଦ୍ୱିଗୁଣ lat_now, ଦ୍ୱିଗୁଣ lon_now, ଦ୍ୱିଗୁଣ lat_next, ଦ୍ୱିଗୁଣ lon_next)
{
	ଅଚଳ ଦ୍ୱିଗୁଣ lat_now_rad = math::radians(lat_now);
	ଅଚଳ ଦ୍ୱିଗୁଣ lat_next_rad = math::radians(lat_next);

	ଅଚଳ ଦ୍ୱିଗୁଣ cos_lat_next = cos(lat_next_rad);
	ଅଚଳ ଦ୍ୱିଗୁଣ d_lon = math::radians(lon_next - lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */

	ଅଚଳ ଦଶମିକ y = static_cast<ଦଶମିକ>(sin(d_lon) * cos_lat_next);
	ଅଚଳ ଦଶମିକ x = static_cast<ଦଶମିକ>(cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos_lat_next * cos(d_lon));

	ଫେରନ୍ତୁ wrap_pi(atan2f(y, x));
}

ଶୂନ୍ୟ
get_vector_to_next_waypoint(ଦ୍ୱିଗୁଣ lat_now, ଦ୍ୱିଗୁଣ lon_now, ଦ୍ୱିଗୁଣ lat_next, ଦ୍ୱିଗୁଣ lon_next, ଦଶମିକ *v_n, ଦଶମିକ *v_e)
{
	ଅଚଳ ଦ୍ୱିଗୁଣ lat_now_rad = math::radians(lat_now);
	ଅଚଳ ଦ୍ୱିଗୁଣ lat_next_rad = math::radians(lat_next);
	ଅଚଳ ଦ୍ୱିଗୁଣ d_lon = math::radians(lon_next) - math::radians(lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<ଦଶମିକ>(CONSTANTS_RADIUS_OF_EARTH * (cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos(
					  lat_next_rad) * cos(d_lon)));
	*v_e = static_cast<ଦଶମିକ>(CONSTANTS_RADIUS_OF_EARTH * sin(d_lon) * cos(lat_next_rad));
}

ଶୂନ୍ୟ
get_vector_to_next_waypoint_fast(ଦ୍ୱିଗୁଣ lat_now, ଦ୍ୱିଗୁଣ lon_now, ଦ୍ୱିଗୁଣ lat_next, ଦ୍ୱିଗୁଣ lon_next, ଦଶମିକ *v_n,
				 ଦଶମିକ *v_e)
{
	ଦ୍ୱିଗୁଣ lat_now_rad = math::radians(lat_now);
	ଦ୍ୱିଗୁଣ lon_now_rad = math::radians(lon_now);
	ଦ୍ୱିଗୁଣ lat_next_rad = math::radians(lat_next);
	ଦ୍ୱିଗୁଣ lon_next_rad = math::radians(lon_next);

	ଦ୍ୱିଗୁଣ d_lat = lat_next_rad - lat_now_rad;
	ଦ୍ୱିଗୁଣ d_lon = lon_next_rad - lon_now_rad;

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<ଦଶମିକ>(CONSTANTS_RADIUS_OF_EARTH * d_lat);
	*v_e = static_cast<ଦଶମିକ>(CONSTANTS_RADIUS_OF_EARTH * d_lon * cos(lat_now_rad));
}

ଶୂନ୍ୟ add_vector_to_global_position(ଦ୍ୱିଗୁଣ lat_now, ଦ୍ୱିଗୁଣ lon_now, ଦଶମିକ v_n, ଦଶମିକ v_e, ଦ୍ୱିଗୁଣ *lat_res,
				   ଦ୍ୱିଗୁଣ *lon_res)
{
	ଦ୍ୱିଗୁଣ lat_now_rad = math::radians(lat_now);
	ଦ୍ୱିଗୁଣ lon_now_rad = math::radians(lon_now);

	*lat_res = math::degrees(lat_now_rad + (ଦ୍ୱିଗୁଣ)v_n / CONSTANTS_RADIUS_OF_EARTH);
	*lon_res = math::degrees(lon_now_rad + (ଦ୍ୱିଗୁଣ)v_e / (CONSTANTS_RADIUS_OF_EARTH * cos(lat_now_rad)));
}

// Additional functions - @author Doug Weibel <douglas.weibel@colorado.edu>

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_distance_to_ଧାଡ଼ି(ସଂରଚନା crosstrack_error_s &crosstrack_error, ଦ୍ୱିଗୁଣ lat_now, ଦ୍ୱିଗୁଣ lon_now,
			 ଦ୍ୱିଗୁଣ lat_start, ଦ୍ୱିଗୁଣ lon_start, ଦ୍ୱିଗୁଣ lat_end, ଦ୍ୱିଗୁଣ lon_end)
{
	// This function returns the distance to the nearest point on the track line.  Distance is positive if current
	// position is right of the track and negative if left of the track as seen from a point on the track line
	// headed towards the end point.

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା return_ମୂଲ୍ୟ = -1;	// Set error flag, cleared when valid result calculated.
	crosstrack_error.past_end = ମିଥ୍ୟା;
	crosstrack_error.distance = 0.0f;
	crosstrack_error.bearing = 0.0f;

	ଦଶମିକ dist_to_end = get_distance_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);

	// Return error if arguments are bad
	ଯଦି (dist_to_end < 0.1f) {
		ଫେରନ୍ତୁ -1;
	}

	ଦଶମିକ bearing_end = get_bearing_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);
	ଦଶମିକ bearing_track = get_bearing_to_next_waypoint(lat_start, lon_start, lat_end, lon_end);
	ଦଶମିକ bearing_diff = wrap_pi(bearing_track - bearing_end);

	// Return past_end = true if past end point of line
	ଯଦି (bearing_diff > M_PI_2_F || bearing_diff < -M_PI_2_F) {
		crosstrack_error.past_end = ସତ୍ୟ;
		return_ମୂଲ୍ୟ = 0;
		ଫେରନ୍ତୁ return_ମୂଲ୍ୟ;
	}

	crosstrack_error.distance = (dist_to_end) * sinf(bearing_diff);

	ଯଦି (sinf(bearing_diff) >= 0) {
		crosstrack_error.bearing = wrap_pi(bearing_track - M_PI_2_F);

	} ଅନ୍ୟଥା {
		crosstrack_error.bearing = wrap_pi(bearing_track + M_PI_2_F);
	}

	return_ମୂଲ୍ୟ = 0;

	ଫେରନ୍ତୁ return_ମୂଲ୍ୟ;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_distance_to_arc(ସଂରଚନା crosstrack_error_s *crosstrack_error, ଦ୍ୱିଗୁଣ lat_now, ଦ୍ୱିଗୁଣ lon_now,
			ଦ୍ୱିଗୁଣ lat_center, ଦ୍ୱିଗୁଣ lon_center,
			ଦଶମିକ radius, ଦଶମିକ arc_start_bearing, ଦଶମିକ arc_sweep)
{
	// This function returns the distance to the nearest point on the track arc.  Distance is positive if current
	// position is right of the arc and negative if left of the arc as seen from the closest point on the arc and
	// headed towards the end point.

	// Determine if the current position is inside or outside the sector between the line from the center
	// to the arc start and the line from the center to the arc end
	ଅଚଳ ଦଶମିକ bearing_now = get_bearing_to_next_waypoint(lat_now, lon_now, lat_center, lon_center);

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା return_ମୂଲ୍ୟ = -1;		// Set error flag, cleared when valid result calculated.
	crosstrack_error->past_end = ମିଥ୍ୟା;
	crosstrack_error->distance = 0.0f;
	crosstrack_error->bearing = 0.0f;

	// Return error if arguments are bad
	ଯଦି (radius < 0.1f) {
		ଫେରନ୍ତୁ return_ମୂଲ୍ୟ;
	}

	// arc_start_bearing and arc_sweep are referenced to the center, so test membership with the
	// center->vehicle bearing. Measuring it in the direction of the sweep handles either sign and
	// any zero crossing without special cases.
	// NOLINTNEXTLINE(readability-suspicious-call-argument) intentional reverse bearing: center -> vehicle
	ଅଚଳ ଦଶମିକ bearing_center_to_now = get_bearing_to_next_waypoint(lat_center, lon_center, lat_now, lon_now);
	ଅଚଳ ଦଶମିକ bearing_from_start = wrap_2pi((bearing_center_to_now - arc_start_bearing) * (arc_sweep < 0.0f ? -1.0f :
					 1.0f));
	ଅଚଳ ତାର୍କିକ in_sector = bearing_from_start <= fabsf(arc_sweep);

	// If in the sector then calculate distance and bearing to closest point
	ଯଦି (in_sector) {
		crosstrack_error->past_end = ମିଥ୍ୟା;
		ଦଶମିକ dist_to_center = get_distance_to_next_waypoint(lat_now, lon_now, lat_center, lon_center);

		ଯଦି (dist_to_center <= radius) {
			crosstrack_error->distance = radius - dist_to_center;
			crosstrack_error->bearing = bearing_now + M_PI_F;

		} ଅନ୍ୟଥା {
			crosstrack_error->distance = dist_to_center - radius;
			crosstrack_error->bearing = bearing_now;
		}

		// If out of the sector then calculate dist and bearing to start or end point

	} ଅନ୍ୟଥା {

		// Use the approximation  that 111,111 meters in the y direction is 1 degree (of latitude)
		// and 111,111 * cos(latitude) meters in the x direction is 1 degree (of longitude) to
		// calculate the position of the start and end points.  We should not be doing this often
		// as this function generally will not be called repeatedly when we are out of the sector.

		ଦ୍ୱିଗୁଣ start_disp_x = (ଦ୍ୱିଗୁଣ)radius * sin((ଦ୍ୱିଗୁଣ)arc_start_bearing);
		ଦ୍ୱିଗୁଣ start_disp_y = (ଦ୍ୱିଗୁଣ)radius * cos((ଦ୍ୱିଗୁଣ)arc_start_bearing);
		ଦ୍ୱିଗୁଣ end_disp_x = (ଦ୍ୱିଗୁଣ)radius * sin((ଦ୍ୱିଗୁଣ)wrap_pi(arc_start_bearing + arc_sweep));
		ଦ୍ୱିଗୁଣ end_disp_y = (ଦ୍ୱିଗୁଣ)radius * cos((ଦ୍ୱିଗୁଣ)wrap_pi(arc_start_bearing + arc_sweep));
		ଅଚଳ ଦ୍ୱିଗୁଣ cos_lat_center = cos(math::radians(lat_center));
		ଦ୍ୱିଗୁଣ lon_start = lon_center + start_disp_x / (111111.0 * cos_lat_center);
		ଦ୍ୱିଗୁଣ lat_start = lat_center + start_disp_y / 111111.0;
		ଦ୍ୱିଗୁଣ lon_end = lon_center + end_disp_x / (111111.0 * cos_lat_center);
		ଦ୍ୱିଗୁଣ lat_end = lat_center + end_disp_y / 111111.0;
		ଦଶମିକ dist_to_start = get_distance_to_next_waypoint(lat_now, lon_now, lat_start, lon_start);
		ଦଶମିକ dist_to_end = get_distance_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);

		ଯଦି (dist_to_start < dist_to_end) {
			crosstrack_error->distance = dist_to_start;
			crosstrack_error->bearing = get_bearing_to_next_waypoint(lat_now, lon_now, lat_start, lon_start);

		} ଅନ୍ୟଥା {
			crosstrack_error->past_end = ସତ୍ୟ;
			crosstrack_error->distance = dist_to_end;
			crosstrack_error->bearing = get_bearing_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);
		}
	}

	crosstrack_error->bearing = wrap_pi(crosstrack_error->bearing);
	return_ମୂଲ୍ୟ = 0;

	ଫେରନ୍ତୁ return_ମୂଲ୍ୟ;
}

ଦଶମିକ get_distance_to_point_global_wgs84(ଦ୍ୱିଗୁଣ lat_now, ଦ୍ୱିଗୁଣ lon_now, ଦଶମିକ alt_now,
		ଦ୍ୱିଗୁଣ lat_next, ଦ୍ୱିଗୁଣ lon_next, ଦଶମିକ alt_next,
		ଦଶମିକ *dist_xy, ଦଶମିକ *dist_z)
{
	ଦ୍ୱିଗୁଣ current_x_rad = math::radians(lat_next);
	ଦ୍ୱିଗୁଣ current_y_rad = math::radians(lon_next);
	ଦ୍ୱିଗୁଣ x_rad = math::radians(lat_now);
	ଦ୍ୱିଗୁଣ y_rad = math::radians(lon_now);

	ଦ୍ୱିଗୁଣ d_lat = x_rad - current_x_rad;
	ଦ୍ୱିଗୁଣ d_lon = y_rad - current_y_rad;

	ଦ୍ୱିଗୁଣ a = sin(d_lat / 2.0) * sin(d_lat / 2.0) + sin(d_lon / 2.0) * sin(d_lon / 2.0) * cos(current_x_rad) * cos(x_rad);
	ଦ୍ୱିଗୁଣ c = 2 * atan2(sqrt(a), sqrt(1 - a));

	ଅଚଳ ଦଶମିକ dxy = static_cast<ଦଶମିକ>(CONSTANTS_RADIUS_OF_EARTH * c);
	ଅଚଳ ଦଶମିକ dz = static_cast<ଦଶମିକ>(alt_now - alt_next);

	*dist_xy = fabsf(dxy);
	*dist_z = fabsf(dz);

	ଫେରନ୍ତୁ sqrtf(dxy * dxy + dz * dz);
}

ଦଶମିକ mavlink_wpm_distance_to_point_local(ଦଶମିକ x_now, ଦଶମିକ y_now, ଦଶମିକ z_now,
		ଦଶମିକ x_next, ଦଶମିକ y_next, ଦଶମିକ z_next,
		ଦଶମିକ *dist_xy, ଦଶମିକ *dist_z)
{
	ଦଶମିକ dx = x_now - x_next;
	ଦଶମିକ dy = y_now - y_next;
	ଦଶମିକ dz = z_now - z_next;

	*dist_xy = sqrtf(dx * dx + dy * dy);
	*dist_z = fabsf(dz);

	ଫେରନ୍ତୁ sqrtf(dx * dx + dy * dy + dz * dz);
}
