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

#ಅನ್ತರ್ಭಾವ "geo.h"

#ಅನ್ತರ್ಭಾವ <float.h>

using matrix::wrap_pi;
using matrix::wrap_2pi;

/*
 * Azimuthal Equidistant Projection
 * formulas according to: http://mathworld.wolfram.com/AzimuthalEquidistantProjection.html
 */

ಶೂನ್ಯ MapProjection::initReference(ದ್ವಿಗುಣ lat_0, ದ್ವಿಗುಣ lon_0, uint64_t timestamp)
{
	_ref_timestamp = timestamp;
	_ref_lat = math::radians(lat_0);
	_ref_lon = math::radians(lon_0);
	_ref_sin_lat = sin(_ref_lat);
	_ref_cos_lat = cos(_ref_lat);
	_ref_init_done = ಸತ್ಯ;
}

ಶೂನ್ಯ MapProjection::project(ದ್ವಿಗುಣ lat, ದ್ವಿಗುಣ lon, ಭಿನ್ನ &x, ಭಿನ್ನ &y) ಧ್ರುವ
{
	ಧ್ರುವ ದ್ವಿಗುಣ lat_rad = math::radians(lat);
	ಧ್ರುವ ದ್ವಿಗುಣ lon_rad = math::radians(lon);

	ಧ್ರುವ ದ್ವಿಗುಣ sin_lat = sin(lat_rad);
	ಧ್ರುವ ದ್ವಿಗುಣ cos_lat = cos(lat_rad);

	ಧ್ರುವ ದ್ವಿಗುಣ cos_d_lon = cos(lon_rad - _ref_lon);

	ಧ್ರುವ ದ್ವಿಗುಣ arg = math::constrain(_ref_sin_lat * sin_lat + _ref_cos_lat * cos_lat * cos_d_lon, -1.0,  1.0);
	ಧ್ರುವ ದ್ವಿಗುಣ c = acos(arg);

	ದ್ವಿಗುಣ k = 1.0;

	ಯದಿ (fabs(c) > 0) {
		k = (c / sin(c));
	}

	x = static_cast<ಭಿನ್ನ>(k * (_ref_cos_lat * sin_lat - _ref_sin_lat * cos_lat * cos_d_lon) * CONSTANTS_RADIUS_OF_EARTH);
	y = static_cast<ಭಿನ್ನ>(k * cos_lat * sin(lon_rad - _ref_lon) * CONSTANTS_RADIUS_OF_EARTH);
}

ಶೂನ್ಯ MapProjection::reproject(ಭಿನ್ನ x, ಭಿನ್ನ y, ದ್ವಿಗುಣ &lat, ದ್ವಿಗುಣ &lon) ಧ್ರುವ
{
	ಧ್ರುವ ದ್ವಿಗುಣ x_rad = (ದ್ವಿಗುಣ)x / CONSTANTS_RADIUS_OF_EARTH;
	ಧ್ರುವ ದ್ವಿಗುಣ y_rad = (ದ್ವಿಗುಣ)y / CONSTANTS_RADIUS_OF_EARTH;
	ಧ್ರುವ ದ್ವಿಗುಣ c = sqrt(x_rad * x_rad + y_rad * y_rad);

	ಯದಿ (fabs(c) > 0) {
		ಧ್ರುವ ದ್ವಿಗುಣ sin_c = sin(c);
		ಧ್ರುವ ದ್ವಿಗುಣ cos_c = cos(c);

		ಧ್ರುವ ದ್ವಿಗುಣ lat_rad = asin(cos_c * _ref_sin_lat + (x_rad * sin_c * _ref_cos_lat) / c);
		ಧ್ರುವ ದ್ವಿಗುಣ lon_rad = (_ref_lon + atan2(y_rad * sin_c, c * _ref_cos_lat * cos_c - x_rad * _ref_sin_lat * sin_c));

		lat = math::degrees(lat_rad);
		lon = math::degrees(lon_rad);

	} ಅನ್ಯಥಾ {
		lat = math::degrees(_ref_lat);
		lon = math::degrees(_ref_lon);
	}
}

ಭಿನ್ನ get_distance_to_next_waypoint(ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now, ದ್ವಿಗುಣ lat_next, ದ್ವಿಗುಣ lon_next)
{
	ಧ್ರುವ ದ್ವಿಗುಣ lat_now_rad = math::radians(lat_now);
	ಧ್ರುವ ದ್ವಿಗುಣ lat_next_rad = math::radians(lat_next);

	ಧ್ರುವ ದ್ವಿಗುಣ d_lat = lat_next_rad - lat_now_rad;
	ಧ್ರುವ ದ್ವಿಗುಣ d_lon = math::radians(lon_next) - math::radians(lon_now);

	ಧ್ರುವ ದ್ವಿಗುಣ a = sin(d_lat / 2.0) * sin(d_lat / 2.0) + sin(d_lon / 2.0) * sin(d_lon / 2.0) * cos(lat_now_rad) * cos(
				 lat_next_rad);

	ಧ್ರುವ ದ್ವಿಗುಣ c = atan2(sqrt(a), sqrt(1.0 - a));

	ಪ್ರತ್ಯಾಗಮ static_cast<ಭಿನ್ನ>(CONSTANTS_RADIUS_OF_EARTH * 2.0 * c);
}

ಶೂನ್ಯ create_waypoint_from_ಸಾಲು_and_dist(ದ್ವಿಗುಣ lat_A, ದ್ವಿಗುಣ lon_A, ದ್ವಿಗುಣ lat_B, ದ್ವಿಗುಣ lon_B, ಭಿನ್ನ dist,
					ದ್ವಿಗುಣ *lat_target, ದ್ವಿಗುಣ *lon_target)
{
	ಯದಿ (fabsf(dist) < FLT_EPSILON) {
		*lat_target = lat_A;
		*lon_target = lon_A;

	} ಅನ್ಯಥಾ {
		ಭಿನ್ನ heading = get_bearing_to_next_waypoint(lat_A, lon_A, lat_B, lon_B);
		waypoint_from_heading_and_distance(lat_A, lon_A, heading, dist, lat_target, lon_target);
	}
}

ಶೂನ್ಯ waypoint_from_heading_and_distance(ದ್ವಿಗುಣ lat_start, ದ್ವಿಗುಣ lon_start, ಭಿನ್ನ bearing, ಭಿನ್ನ dist,
					ದ್ವಿಗುಣ *lat_target, ದ್ವಿಗುಣ *lon_target)
{
	bearing = wrap_2pi(bearing);

	ದ್ವಿಗುಣ radius_ratio = static_cast<ದ್ವಿಗುಣ>(dist) / CONSTANTS_RADIUS_OF_EARTH;

	ದ್ವಿಗುಣ lat_start_rad = math::radians(lat_start);
	ದ್ವಿಗುಣ lon_start_rad = math::radians(lon_start);

	*lat_target = asin(sin(lat_start_rad) * cos(radius_ratio) + cos(lat_start_rad) * sin(radius_ratio) * cos((
				   ದ್ವಿಗುಣ)bearing));
	*lon_target = lon_start_rad + atan2(sin((ದ್ವಿಗುಣ)bearing) * sin(radius_ratio) * cos(lat_start_rad),
					    cos(radius_ratio) - sin(lat_start_rad) * sin(*lat_target));

	*lat_target = math::degrees(*lat_target);
	*lon_target = math::degrees(*lon_target);
}

ಭಿನ್ನ get_bearing_to_next_waypoint(ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now, ದ್ವಿಗುಣ lat_next, ದ್ವಿಗುಣ lon_next)
{
	ಧ್ರುವ ದ್ವಿಗುಣ lat_now_rad = math::radians(lat_now);
	ಧ್ರುವ ದ್ವಿಗುಣ lat_next_rad = math::radians(lat_next);

	ಧ್ರುವ ದ್ವಿಗುಣ cos_lat_next = cos(lat_next_rad);
	ಧ್ರುವ ದ್ವಿಗುಣ d_lon = math::radians(lon_next - lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */

	ಧ್ರುವ ಭಿನ್ನ y = static_cast<ಭಿನ್ನ>(sin(d_lon) * cos_lat_next);
	ಧ್ರುವ ಭಿನ್ನ x = static_cast<ಭಿನ್ನ>(cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos_lat_next * cos(d_lon));

	ಪ್ರತ್ಯಾಗಮ wrap_pi(atan2f(y, x));
}

ಶೂನ್ಯ
get_vector_to_next_waypoint(ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now, ದ್ವಿಗುಣ lat_next, ದ್ವಿಗುಣ lon_next, ಭಿನ್ನ *v_n, ಭಿನ್ನ *v_e)
{
	ಧ್ರುವ ದ್ವಿಗುಣ lat_now_rad = math::radians(lat_now);
	ಧ್ರುವ ದ್ವಿಗುಣ lat_next_rad = math::radians(lat_next);
	ಧ್ರುವ ದ್ವಿಗುಣ d_lon = math::radians(lon_next) - math::radians(lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<ಭಿನ್ನ>(CONSTANTS_RADIUS_OF_EARTH * (cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos(
					  lat_next_rad) * cos(d_lon)));
	*v_e = static_cast<ಭಿನ್ನ>(CONSTANTS_RADIUS_OF_EARTH * sin(d_lon) * cos(lat_next_rad));
}

ಶೂನ್ಯ
get_vector_to_next_waypoint_fast(ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now, ದ್ವಿಗುಣ lat_next, ದ್ವಿಗುಣ lon_next, ಭಿನ್ನ *v_n,
				 ಭಿನ್ನ *v_e)
{
	ದ್ವಿಗುಣ lat_now_rad = math::radians(lat_now);
	ದ್ವಿಗುಣ lon_now_rad = math::radians(lon_now);
	ದ್ವಿಗುಣ lat_next_rad = math::radians(lat_next);
	ದ್ವಿಗುಣ lon_next_rad = math::radians(lon_next);

	ದ್ವಿಗುಣ d_lat = lat_next_rad - lat_now_rad;
	ದ್ವಿಗುಣ d_lon = lon_next_rad - lon_now_rad;

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<ಭಿನ್ನ>(CONSTANTS_RADIUS_OF_EARTH * d_lat);
	*v_e = static_cast<ಭಿನ್ನ>(CONSTANTS_RADIUS_OF_EARTH * d_lon * cos(lat_now_rad));
}

ಶೂನ್ಯ add_vector_to_global_position(ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now, ಭಿನ್ನ v_n, ಭಿನ್ನ v_e, ದ್ವಿಗುಣ *lat_res,
				   ದ್ವಿಗುಣ *lon_res)
{
	ದ್ವಿಗುಣ lat_now_rad = math::radians(lat_now);
	ದ್ವಿಗುಣ lon_now_rad = math::radians(lon_now);

	*lat_res = math::degrees(lat_now_rad + (ದ್ವಿಗುಣ)v_n / CONSTANTS_RADIUS_OF_EARTH);
	*lon_res = math::degrees(lon_now_rad + (ದ್ವಿಗುಣ)v_e / (CONSTANTS_RADIUS_OF_EARTH * cos(lat_now_rad)));
}

// Additional functions - @author Doug Weibel <douglas.weibel@colorado.edu>

ಪೂರ್ಣಾಙ್ಕ get_distance_to_ಸಾಲು(ಸಂರಚನಾ crosstrack_error_s &crosstrack_error, ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now,
			 ದ್ವಿಗುಣ lat_start, ದ್ವಿಗುಣ lon_start, ದ್ವಿಗುಣ lat_end, ದ್ವಿಗುಣ lon_end)
{
	// This function returns the distance to the nearest point on the track line.  Distance is positive if current
	// position is right of the track and negative if left of the track as seen from a point on the track line
	// headed towards the end point.

	ಪೂರ್ಣಾಙ್ಕ return_ಮೌಲ್ಯ = -1;	// Set error flag, cleared when valid result calculated.
	crosstrack_error.past_end = ಅಸತ್ಯ;
	crosstrack_error.distance = 0.0f;
	crosstrack_error.bearing = 0.0f;

	ಭಿನ್ನ dist_to_end = get_distance_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);

	// Return error if arguments are bad
	ಯದಿ (dist_to_end < 0.1f) {
		ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಭಿನ್ನ bearing_end = get_bearing_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);
	ಭಿನ್ನ bearing_track = get_bearing_to_next_waypoint(lat_start, lon_start, lat_end, lon_end);
	ಭಿನ್ನ bearing_diff = wrap_pi(bearing_track - bearing_end);

	// Return past_end = true if past end point of line
	ಯದಿ (bearing_diff > M_PI_2_F || bearing_diff < -M_PI_2_F) {
		crosstrack_error.past_end = ಸತ್ಯ;
		return_ಮೌಲ್ಯ = 0;
		ಪ್ರತ್ಯಾಗಮ return_ಮೌಲ್ಯ;
	}

	crosstrack_error.distance = (dist_to_end) * sinf(bearing_diff);

	ಯದಿ (sinf(bearing_diff) >= 0) {
		crosstrack_error.bearing = wrap_pi(bearing_track - M_PI_2_F);

	} ಅನ್ಯಥಾ {
		crosstrack_error.bearing = wrap_pi(bearing_track + M_PI_2_F);
	}

	return_ಮೌಲ್ಯ = 0;

	ಪ್ರತ್ಯಾಗಮ return_ಮೌಲ್ಯ;
}

ಪೂರ್ಣಾಙ್ಕ get_distance_to_arc(ಸಂರಚನಾ crosstrack_error_s *crosstrack_error, ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now,
			ದ್ವಿಗುಣ lat_center, ದ್ವಿಗುಣ lon_center,
			ಭಿನ್ನ radius, ಭಿನ್ನ arc_start_bearing, ಭಿನ್ನ arc_sweep)
{
	// This function returns the distance to the nearest point on the track arc.  Distance is positive if current
	// position is right of the arc and negative if left of the arc as seen from the closest point on the arc and
	// headed towards the end point.

	// Determine if the current position is inside or outside the sector between the line from the center
	// to the arc start and the line from the center to the arc end
	ಧ್ರುವ ಭಿನ್ನ bearing_now = get_bearing_to_next_waypoint(lat_now, lon_now, lat_center, lon_center);

	ಪೂರ್ಣಾಙ್ಕ return_ಮೌಲ್ಯ = -1;		// Set error flag, cleared when valid result calculated.
	crosstrack_error->past_end = ಅಸತ್ಯ;
	crosstrack_error->distance = 0.0f;
	crosstrack_error->bearing = 0.0f;

	// Return error if arguments are bad
	ಯದಿ (radius < 0.1f) {
		ಪ್ರತ್ಯಾಗಮ return_ಮೌಲ್ಯ;
	}

	// arc_start_bearing and arc_sweep are referenced to the center, so test membership with the
	// center->vehicle bearing. Measuring it in the direction of the sweep handles either sign and
	// any zero crossing without special cases.
	// NOLINTNEXTLINE(readability-suspicious-call-argument) intentional reverse bearing: center -> vehicle
	ಧ್ರುವ ಭಿನ್ನ bearing_center_to_now = get_bearing_to_next_waypoint(lat_center, lon_center, lat_now, lon_now);
	ಧ್ರುವ ಭಿನ್ನ bearing_from_start = wrap_2pi((bearing_center_to_now - arc_start_bearing) * (arc_sweep < 0.0f ? -1.0f :
					 1.0f));
	ಧ್ರುವ ತಾರ್ಕಿಕ in_sector = bearing_from_start <= fabsf(arc_sweep);

	// If in the sector then calculate distance and bearing to closest point
	ಯದಿ (in_sector) {
		crosstrack_error->past_end = ಅಸತ್ಯ;
		ಭಿನ್ನ dist_to_center = get_distance_to_next_waypoint(lat_now, lon_now, lat_center, lon_center);

		ಯದಿ (dist_to_center <= radius) {
			crosstrack_error->distance = radius - dist_to_center;
			crosstrack_error->bearing = bearing_now + M_PI_F;

		} ಅನ್ಯಥಾ {
			crosstrack_error->distance = dist_to_center - radius;
			crosstrack_error->bearing = bearing_now;
		}

		// If out of the sector then calculate dist and bearing to start or end point

	} ಅನ್ಯಥಾ {

		// Use the approximation  that 111,111 meters in the y direction is 1 degree (of latitude)
		// and 111,111 * cos(latitude) meters in the x direction is 1 degree (of longitude) to
		// calculate the position of the start and end points.  We should not be doing this often
		// as this function generally will not be called repeatedly when we are out of the sector.

		ದ್ವಿಗುಣ start_disp_x = (ದ್ವಿಗುಣ)radius * sin((ದ್ವಿಗುಣ)arc_start_bearing);
		ದ್ವಿಗುಣ start_disp_y = (ದ್ವಿಗುಣ)radius * cos((ದ್ವಿಗುಣ)arc_start_bearing);
		ದ್ವಿಗುಣ end_disp_x = (ದ್ವಿಗುಣ)radius * sin((ದ್ವಿಗುಣ)wrap_pi(arc_start_bearing + arc_sweep));
		ದ್ವಿಗುಣ end_disp_y = (ದ್ವಿಗುಣ)radius * cos((ದ್ವಿಗುಣ)wrap_pi(arc_start_bearing + arc_sweep));
		ಧ್ರುವ ದ್ವಿಗುಣ cos_lat_center = cos(math::radians(lat_center));
		ದ್ವಿಗುಣ lon_start = lon_center + start_disp_x / (111111.0 * cos_lat_center);
		ದ್ವಿಗುಣ lat_start = lat_center + start_disp_y / 111111.0;
		ದ್ವಿಗುಣ lon_end = lon_center + end_disp_x / (111111.0 * cos_lat_center);
		ದ್ವಿಗುಣ lat_end = lat_center + end_disp_y / 111111.0;
		ಭಿನ್ನ dist_to_start = get_distance_to_next_waypoint(lat_now, lon_now, lat_start, lon_start);
		ಭಿನ್ನ dist_to_end = get_distance_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);

		ಯದಿ (dist_to_start < dist_to_end) {
			crosstrack_error->distance = dist_to_start;
			crosstrack_error->bearing = get_bearing_to_next_waypoint(lat_now, lon_now, lat_start, lon_start);

		} ಅನ್ಯಥಾ {
			crosstrack_error->past_end = ಸತ್ಯ;
			crosstrack_error->distance = dist_to_end;
			crosstrack_error->bearing = get_bearing_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);
		}
	}

	crosstrack_error->bearing = wrap_pi(crosstrack_error->bearing);
	return_ಮೌಲ್ಯ = 0;

	ಪ್ರತ್ಯಾಗಮ return_ಮೌಲ್ಯ;
}

ಭಿನ್ನ get_distance_to_point_global_wgs84(ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now, ಭಿನ್ನ alt_now,
		ದ್ವಿಗುಣ lat_next, ದ್ವಿಗುಣ lon_next, ಭಿನ್ನ alt_next,
		ಭಿನ್ನ *dist_xy, ಭಿನ್ನ *dist_z)
{
	ದ್ವಿಗುಣ current_x_rad = math::radians(lat_next);
	ದ್ವಿಗುಣ current_y_rad = math::radians(lon_next);
	ದ್ವಿಗುಣ x_rad = math::radians(lat_now);
	ದ್ವಿಗುಣ y_rad = math::radians(lon_now);

	ದ್ವಿಗುಣ d_lat = x_rad - current_x_rad;
	ದ್ವಿಗುಣ d_lon = y_rad - current_y_rad;

	ದ್ವಿಗುಣ a = sin(d_lat / 2.0) * sin(d_lat / 2.0) + sin(d_lon / 2.0) * sin(d_lon / 2.0) * cos(current_x_rad) * cos(x_rad);
	ದ್ವಿಗುಣ c = 2 * atan2(sqrt(a), sqrt(1 - a));

	ಧ್ರುವ ಭಿನ್ನ dxy = static_cast<ಭಿನ್ನ>(CONSTANTS_RADIUS_OF_EARTH * c);
	ಧ್ರುವ ಭಿನ್ನ dz = static_cast<ಭಿನ್ನ>(alt_now - alt_next);

	*dist_xy = fabsf(dxy);
	*dist_z = fabsf(dz);

	ಪ್ರತ್ಯಾಗಮ sqrtf(dxy * dxy + dz * dz);
}

ಭಿನ್ನ mavlink_wpm_distance_to_point_local(ಭಿನ್ನ x_now, ಭಿನ್ನ y_now, ಭಿನ್ನ z_now,
		ಭಿನ್ನ x_next, ಭಿನ್ನ y_next, ಭಿನ್ನ z_next,
		ಭಿನ್ನ *dist_xy, ಭಿನ್ನ *dist_z)
{
	ಭಿನ್ನ dx = x_now - x_next;
	ಭಿನ್ನ dy = y_now - y_next;
	ಭಿನ್ನ dz = z_now - z_next;

	*dist_xy = sqrtf(dx * dx + dy * dy);
	*dist_z = fabsf(dz);

	ಪ್ರತ್ಯಾಗಮ sqrtf(dx * dx + dy * dy + dz * dz);
}
