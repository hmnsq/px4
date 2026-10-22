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

#અન્તર્ભાવ "geo.h"

#અન્તર્ભાવ <float.h>

using matrix::wrap_pi;
using matrix::wrap_2pi;

/*
 * Azimuthal Equidistant Projection
 * formulas according to: http://mathworld.wolfram.com/AzimuthalEquidistantProjection.html
 */

શૂન્ય MapProjection::initReference(દ્વિગુણ lat_0, દ્વિગુણ lon_0, uint64_t timestamp)
{
	_ref_timestamp = timestamp;
	_ref_lat = math::radians(lat_0);
	_ref_lon = math::radians(lon_0);
	_ref_sin_lat = sin(_ref_lat);
	_ref_cos_lat = cos(_ref_lat);
	_ref_init_done = સાચું;
}

શૂન્ય MapProjection::project(દ્વિગુણ lat, દ્વિગુણ lon, દશાંશ &x, દશાંશ &y) અચલ
{
	અચલ દ્વિગુણ lat_rad = math::radians(lat);
	અચલ દ્વિગુણ lon_rad = math::radians(lon);

	અચલ દ્વિગુણ sin_lat = sin(lat_rad);
	અચલ દ્વિગુણ cos_lat = cos(lat_rad);

	અચલ દ્વિગુણ cos_d_lon = cos(lon_rad - _ref_lon);

	અચલ દ્વિગુણ arg = math::constrain(_ref_sin_lat * sin_lat + _ref_cos_lat * cos_lat * cos_d_lon, -1.0,  1.0);
	અચલ દ્વિગુણ c = acos(arg);

	દ્વિગુણ k = 1.0;

	જો (fabs(c) > 0) {
		k = (c / sin(c));
	}

	x = static_cast<દશાંશ>(k * (_ref_cos_lat * sin_lat - _ref_sin_lat * cos_lat * cos_d_lon) * CONSTANTS_RADIUS_OF_EARTH);
	y = static_cast<દશાંશ>(k * cos_lat * sin(lon_rad - _ref_lon) * CONSTANTS_RADIUS_OF_EARTH);
}

શૂન્ય MapProjection::reproject(દશાંશ x, દશાંશ y, દ્વિગુણ &lat, દ્વિગુણ &lon) અચલ
{
	અચલ દ્વિગુણ x_rad = (દ્વિગુણ)x / CONSTANTS_RADIUS_OF_EARTH;
	અચલ દ્વિગુણ y_rad = (દ્વિગુણ)y / CONSTANTS_RADIUS_OF_EARTH;
	અચલ દ્વિગુણ c = sqrt(x_rad * x_rad + y_rad * y_rad);

	જો (fabs(c) > 0) {
		અચલ દ્વિગુણ sin_c = sin(c);
		અચલ દ્વિગુણ cos_c = cos(c);

		અચલ દ્વિગુણ lat_rad = asin(cos_c * _ref_sin_lat + (x_rad * sin_c * _ref_cos_lat) / c);
		અચલ દ્વિગુણ lon_rad = (_ref_lon + atan2(y_rad * sin_c, c * _ref_cos_lat * cos_c - x_rad * _ref_sin_lat * sin_c));

		lat = math::degrees(lat_rad);
		lon = math::degrees(lon_rad);

	} નહીંતર {
		lat = math::degrees(_ref_lat);
		lon = math::degrees(_ref_lon);
	}
}

દશાંશ get_distance_to_next_waypoint(દ્વિગુણ lat_now, દ્વિગુણ lon_now, દ્વિગુણ lat_next, દ્વિગુણ lon_next)
{
	અચલ દ્વિગુણ lat_now_rad = math::radians(lat_now);
	અચલ દ્વિગુણ lat_next_rad = math::radians(lat_next);

	અચલ દ્વિગુણ d_lat = lat_next_rad - lat_now_rad;
	અચલ દ્વિગુણ d_lon = math::radians(lon_next) - math::radians(lon_now);

	અચલ દ્વિગુણ a = sin(d_lat / 2.0) * sin(d_lat / 2.0) + sin(d_lon / 2.0) * sin(d_lon / 2.0) * cos(lat_now_rad) * cos(
				 lat_next_rad);

	અચલ દ્વિગુણ c = atan2(sqrt(a), sqrt(1.0 - a));

	પાછા static_cast<દશાંશ>(CONSTANTS_RADIUS_OF_EARTH * 2.0 * c);
}

શૂન્ય create_waypoint_from_લીટી_and_dist(દ્વિગુણ lat_A, દ્વિગુણ lon_A, દ્વિગુણ lat_B, દ્વિગુણ lon_B, દશાંશ dist,
					દ્વિગુણ *lat_target, દ્વિગુણ *lon_target)
{
	જો (fabsf(dist) < FLT_EPSILON) {
		*lat_target = lat_A;
		*lon_target = lon_A;

	} નહીંતર {
		દશાંશ heading = get_bearing_to_next_waypoint(lat_A, lon_A, lat_B, lon_B);
		waypoint_from_heading_and_distance(lat_A, lon_A, heading, dist, lat_target, lon_target);
	}
}

શૂન્ય waypoint_from_heading_and_distance(દ્વિગુણ lat_start, દ્વિગુણ lon_start, દશાંશ bearing, દશાંશ dist,
					દ્વિગુણ *lat_target, દ્વિગુણ *lon_target)
{
	bearing = wrap_2pi(bearing);

	દ્વિગુણ radius_ratio = static_cast<દ્વિગુણ>(dist) / CONSTANTS_RADIUS_OF_EARTH;

	દ્વિગુણ lat_start_rad = math::radians(lat_start);
	દ્વિગુણ lon_start_rad = math::radians(lon_start);

	*lat_target = asin(sin(lat_start_rad) * cos(radius_ratio) + cos(lat_start_rad) * sin(radius_ratio) * cos((
				   દ્વિગુણ)bearing));
	*lon_target = lon_start_rad + atan2(sin((દ્વિગુણ)bearing) * sin(radius_ratio) * cos(lat_start_rad),
					    cos(radius_ratio) - sin(lat_start_rad) * sin(*lat_target));

	*lat_target = math::degrees(*lat_target);
	*lon_target = math::degrees(*lon_target);
}

દશાંશ get_bearing_to_next_waypoint(દ્વિગુણ lat_now, દ્વિગુણ lon_now, દ્વિગુણ lat_next, દ્વિગુણ lon_next)
{
	અચલ દ્વિગુણ lat_now_rad = math::radians(lat_now);
	અચલ દ્વિગુણ lat_next_rad = math::radians(lat_next);

	અચલ દ્વિગુણ cos_lat_next = cos(lat_next_rad);
	અચલ દ્વિગુણ d_lon = math::radians(lon_next - lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */

	અચલ દશાંશ y = static_cast<દશાંશ>(sin(d_lon) * cos_lat_next);
	અચલ દશાંશ x = static_cast<દશાંશ>(cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos_lat_next * cos(d_lon));

	પાછા wrap_pi(atan2f(y, x));
}

શૂન્ય
get_vector_to_next_waypoint(દ્વિગુણ lat_now, દ્વિગુણ lon_now, દ્વિગુણ lat_next, દ્વિગુણ lon_next, દશાંશ *v_n, દશાંશ *v_e)
{
	અચલ દ્વિગુણ lat_now_rad = math::radians(lat_now);
	અચલ દ્વિગુણ lat_next_rad = math::radians(lat_next);
	અચલ દ્વિગુણ d_lon = math::radians(lon_next) - math::radians(lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<દશાંશ>(CONSTANTS_RADIUS_OF_EARTH * (cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos(
					  lat_next_rad) * cos(d_lon)));
	*v_e = static_cast<દશાંશ>(CONSTANTS_RADIUS_OF_EARTH * sin(d_lon) * cos(lat_next_rad));
}

શૂન્ય
get_vector_to_next_waypoint_fast(દ્વિગુણ lat_now, દ્વિગુણ lon_now, દ્વિગુણ lat_next, દ્વિગુણ lon_next, દશાંશ *v_n,
				 દશાંશ *v_e)
{
	દ્વિગુણ lat_now_rad = math::radians(lat_now);
	દ્વિગુણ lon_now_rad = math::radians(lon_now);
	દ્વિગુણ lat_next_rad = math::radians(lat_next);
	દ્વિગુણ lon_next_rad = math::radians(lon_next);

	દ્વિગુણ d_lat = lat_next_rad - lat_now_rad;
	દ્વિગુણ d_lon = lon_next_rad - lon_now_rad;

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<દશાંશ>(CONSTANTS_RADIUS_OF_EARTH * d_lat);
	*v_e = static_cast<દશાંશ>(CONSTANTS_RADIUS_OF_EARTH * d_lon * cos(lat_now_rad));
}

શૂન્ય add_vector_to_global_position(દ્વિગુણ lat_now, દ્વિગુણ lon_now, દશાંશ v_n, દશાંશ v_e, દ્વિગુણ *lat_res,
				   દ્વિગુણ *lon_res)
{
	દ્વિગુણ lat_now_rad = math::radians(lat_now);
	દ્વિગુણ lon_now_rad = math::radians(lon_now);

	*lat_res = math::degrees(lat_now_rad + (દ્વિગુણ)v_n / CONSTANTS_RADIUS_OF_EARTH);
	*lon_res = math::degrees(lon_now_rad + (દ્વિગુણ)v_e / (CONSTANTS_RADIUS_OF_EARTH * cos(lat_now_rad)));
}

// Additional functions - @author Doug Weibel <douglas.weibel@colorado.edu>

પૂર્ણાંક get_distance_to_લીટી(સંરચના crosstrack_error_s &crosstrack_error, દ્વિગુણ lat_now, દ્વિગુણ lon_now,
			 દ્વિગુણ lat_start, દ્વિગુણ lon_start, દ્વિગુણ lat_end, દ્વિગુણ lon_end)
{
	// This function returns the distance to the nearest point on the track line.  Distance is positive if current
	// position is right of the track and negative if left of the track as seen from a point on the track line
	// headed towards the end point.

	પૂર્ણાંક return_કિંમત = -1;	// Set error flag, cleared when valid result calculated.
	crosstrack_error.past_end = ખોટું;
	crosstrack_error.distance = 0.0f;
	crosstrack_error.bearing = 0.0f;

	દશાંશ dist_to_end = get_distance_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);

	// Return error if arguments are bad
	જો (dist_to_end < 0.1f) {
		પાછા -1;
	}

	દશાંશ bearing_end = get_bearing_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);
	દશાંશ bearing_track = get_bearing_to_next_waypoint(lat_start, lon_start, lat_end, lon_end);
	દશાંશ bearing_diff = wrap_pi(bearing_track - bearing_end);

	// Return past_end = true if past end point of line
	જો (bearing_diff > M_PI_2_F || bearing_diff < -M_PI_2_F) {
		crosstrack_error.past_end = સાચું;
		return_કિંમત = 0;
		પાછા return_કિંમત;
	}

	crosstrack_error.distance = (dist_to_end) * sinf(bearing_diff);

	જો (sinf(bearing_diff) >= 0) {
		crosstrack_error.bearing = wrap_pi(bearing_track - M_PI_2_F);

	} નહીંતર {
		crosstrack_error.bearing = wrap_pi(bearing_track + M_PI_2_F);
	}

	return_કિંમત = 0;

	પાછા return_કિંમત;
}

પૂર્ણાંક get_distance_to_arc(સંરચના crosstrack_error_s *crosstrack_error, દ્વિગુણ lat_now, દ્વિગુણ lon_now,
			દ્વિગુણ lat_center, દ્વિગુણ lon_center,
			દશાંશ radius, દશાંશ arc_start_bearing, દશાંશ arc_sweep)
{
	// This function returns the distance to the nearest point on the track arc.  Distance is positive if current
	// position is right of the arc and negative if left of the arc as seen from the closest point on the arc and
	// headed towards the end point.

	// Determine if the current position is inside or outside the sector between the line from the center
	// to the arc start and the line from the center to the arc end
	અચલ દશાંશ bearing_now = get_bearing_to_next_waypoint(lat_now, lon_now, lat_center, lon_center);

	પૂર્ણાંક return_કિંમત = -1;		// Set error flag, cleared when valid result calculated.
	crosstrack_error->past_end = ખોટું;
	crosstrack_error->distance = 0.0f;
	crosstrack_error->bearing = 0.0f;

	// Return error if arguments are bad
	જો (radius < 0.1f) {
		પાછા return_કિંમત;
	}

	// arc_start_bearing and arc_sweep are referenced to the center, so test membership with the
	// center->vehicle bearing. Measuring it in the direction of the sweep handles either sign and
	// any zero crossing without special cases.
	// NOLINTNEXTLINE(readability-suspicious-call-argument) intentional reverse bearing: center -> vehicle
	અચલ દશાંશ bearing_center_to_now = get_bearing_to_next_waypoint(lat_center, lon_center, lat_now, lon_now);
	અચલ દશાંશ bearing_from_start = wrap_2pi((bearing_center_to_now - arc_start_bearing) * (arc_sweep < 0.0f ? -1.0f :
					 1.0f));
	અચલ તાર્કિક in_sector = bearing_from_start <= fabsf(arc_sweep);

	// If in the sector then calculate distance and bearing to closest point
	જો (in_sector) {
		crosstrack_error->past_end = ખોટું;
		દશાંશ dist_to_center = get_distance_to_next_waypoint(lat_now, lon_now, lat_center, lon_center);

		જો (dist_to_center <= radius) {
			crosstrack_error->distance = radius - dist_to_center;
			crosstrack_error->bearing = bearing_now + M_PI_F;

		} નહીંતર {
			crosstrack_error->distance = dist_to_center - radius;
			crosstrack_error->bearing = bearing_now;
		}

		// If out of the sector then calculate dist and bearing to start or end point

	} નહીંતર {

		// Use the approximation  that 111,111 meters in the y direction is 1 degree (of latitude)
		// and 111,111 * cos(latitude) meters in the x direction is 1 degree (of longitude) to
		// calculate the position of the start and end points.  We should not be doing this often
		// as this function generally will not be called repeatedly when we are out of the sector.

		દ્વિગુણ start_disp_x = (દ્વિગુણ)radius * sin((દ્વિગુણ)arc_start_bearing);
		દ્વિગુણ start_disp_y = (દ્વિગુણ)radius * cos((દ્વિગુણ)arc_start_bearing);
		દ્વિગુણ end_disp_x = (દ્વિગુણ)radius * sin((દ્વિગુણ)wrap_pi(arc_start_bearing + arc_sweep));
		દ્વિગુણ end_disp_y = (દ્વિગુણ)radius * cos((દ્વિગુણ)wrap_pi(arc_start_bearing + arc_sweep));
		અચલ દ્વિગુણ cos_lat_center = cos(math::radians(lat_center));
		દ્વિગુણ lon_start = lon_center + start_disp_x / (111111.0 * cos_lat_center);
		દ્વિગુણ lat_start = lat_center + start_disp_y / 111111.0;
		દ્વિગુણ lon_end = lon_center + end_disp_x / (111111.0 * cos_lat_center);
		દ્વિગુણ lat_end = lat_center + end_disp_y / 111111.0;
		દશાંશ dist_to_start = get_distance_to_next_waypoint(lat_now, lon_now, lat_start, lon_start);
		દશાંશ dist_to_end = get_distance_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);

		જો (dist_to_start < dist_to_end) {
			crosstrack_error->distance = dist_to_start;
			crosstrack_error->bearing = get_bearing_to_next_waypoint(lat_now, lon_now, lat_start, lon_start);

		} નહીંતર {
			crosstrack_error->past_end = સાચું;
			crosstrack_error->distance = dist_to_end;
			crosstrack_error->bearing = get_bearing_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);
		}
	}

	crosstrack_error->bearing = wrap_pi(crosstrack_error->bearing);
	return_કિંમત = 0;

	પાછા return_કિંમત;
}

દશાંશ get_distance_to_point_global_wgs84(દ્વિગુણ lat_now, દ્વિગુણ lon_now, દશાંશ alt_now,
		દ્વિગુણ lat_next, દ્વિગુણ lon_next, દશાંશ alt_next,
		દશાંશ *dist_xy, દશાંશ *dist_z)
{
	દ્વિગુણ current_x_rad = math::radians(lat_next);
	દ્વિગુણ current_y_rad = math::radians(lon_next);
	દ્વિગુણ x_rad = math::radians(lat_now);
	દ્વિગુણ y_rad = math::radians(lon_now);

	દ્વિગુણ d_lat = x_rad - current_x_rad;
	દ્વિગુણ d_lon = y_rad - current_y_rad;

	દ્વિગુણ a = sin(d_lat / 2.0) * sin(d_lat / 2.0) + sin(d_lon / 2.0) * sin(d_lon / 2.0) * cos(current_x_rad) * cos(x_rad);
	દ્વિગુણ c = 2 * atan2(sqrt(a), sqrt(1 - a));

	અચલ દશાંશ dxy = static_cast<દશાંશ>(CONSTANTS_RADIUS_OF_EARTH * c);
	અચલ દશાંશ dz = static_cast<દશાંશ>(alt_now - alt_next);

	*dist_xy = fabsf(dxy);
	*dist_z = fabsf(dz);

	પાછા sqrtf(dxy * dxy + dz * dz);
}

દશાંશ mavlink_wpm_distance_to_point_local(દશાંશ x_now, દશાંશ y_now, દશાંશ z_now,
		દશાંશ x_next, દશાંશ y_next, દશાંશ z_next,
		દશાંશ *dist_xy, દશાંશ *dist_z)
{
	દશાંશ dx = x_now - x_next;
	દશાંશ dy = y_now - y_next;
	દશાંશ dz = z_now - z_next;

	*dist_xy = sqrtf(dx * dx + dy * dy);
	*dist_z = fabsf(dz);

	પાછા sqrtf(dx * dx + dy * dy + dz * dz);
}
