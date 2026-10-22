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

#ضمن "geo.h"

#ضمن <float.h>

using matrix::wrap_pi;
using matrix::wrap_2pi;

/*
 * Azimuthal Equidistant Projection
 * formulas according to: http://mathworld.wolfram.com/AzimuthalEquidistantProjection.html
 */

فراغ MapProjection::initReference(مزدوج lat_0, مزدوج lon_0, uint64_t timestamp)
{
	_ref_timestamp = timestamp;
	_ref_lat = math::radians(lat_0);
	_ref_lon = math::radians(lon_0);
	_ref_sin_lat = sin(_ref_lat);
	_ref_cos_lat = cos(_ref_lat);
	_ref_init_done = ܫܪܪܐ;
}

فراغ MapProjection::project(مزدوج lat, مزدوج lon, عائم &x, عائم &y) ثابت
{
	ثابت مزدوج lat_rad = math::radians(lat);
	ثابت مزدوج lon_rad = math::radians(lon);

	ثابت مزدوج sin_lat = sin(lat_rad);
	ثابت مزدوج cos_lat = cos(lat_rad);

	ثابت مزدوج cos_d_lon = cos(lon_rad - _ref_lon);

	ثابت مزدوج arg = math::constrain(_ref_sin_lat * sin_lat + _ref_cos_lat * cos_lat * cos_d_lon, -1.0,  1.0);
	ثابت مزدوج c = acos(arg);

	مزدوج k = 1.0;

	ܐܢ (fabs(c) > 0) {
		k = (c / sin(c));
	}

	x = static_cast<عائم>(k * (_ref_cos_lat * sin_lat - _ref_sin_lat * cos_lat * cos_d_lon) * CONSTANTS_RADIUS_OF_EARTH);
	y = static_cast<عائم>(k * cos_lat * sin(lon_rad - _ref_lon) * CONSTANTS_RADIUS_OF_EARTH);
}

فراغ MapProjection::reproject(عائم x, عائم y, مزدوج &lat, مزدوج &lon) ثابت
{
	ثابت مزدوج x_rad = (مزدوج)x / CONSTANTS_RADIUS_OF_EARTH;
	ثابت مزدوج y_rad = (مزدوج)y / CONSTANTS_RADIUS_OF_EARTH;
	ثابت مزدوج c = sqrt(x_rad * x_rad + y_rad * y_rad);

	ܐܢ (fabs(c) > 0) {
		ثابت مزدوج sin_c = sin(c);
		ثابت مزدوج cos_c = cos(c);

		ثابت مزدوج lat_rad = asin(cos_c * _ref_sin_lat + (x_rad * sin_c * _ref_cos_lat) / c);
		ثابت مزدوج lon_rad = (_ref_lon + atan2(y_rad * sin_c, c * _ref_cos_lat * cos_c - x_rad * _ref_sin_lat * sin_c));

		lat = math::degrees(lat_rad);
		lon = math::degrees(lon_rad);

	} ܐܚܪܢܐ {
		lat = math::degrees(_ref_lat);
		lon = math::degrees(_ref_lon);
	}
}

عائم get_distance_to_next_waypoint(مزدوج lat_now, مزدوج lon_now, مزدوج lat_next, مزدوج lon_next)
{
	ثابت مزدوج lat_now_rad = math::radians(lat_now);
	ثابت مزدوج lat_next_rad = math::radians(lat_next);

	ثابت مزدوج d_lat = lat_next_rad - lat_now_rad;
	ثابت مزدوج d_lon = math::radians(lon_next) - math::radians(lon_now);

	ثابت مزدوج a = sin(d_lat / 2.0) * sin(d_lat / 2.0) + sin(d_lon / 2.0) * sin(d_lon / 2.0) * cos(lat_now_rad) * cos(
				 lat_next_rad);

	ثابت مزدوج c = atan2(sqrt(a), sqrt(1.0 - a));

	ܦܢܐ static_cast<عائم>(CONSTANTS_RADIUS_OF_EARTH * 2.0 * c);
}

فراغ create_waypoint_from_line_and_dist(مزدوج lat_A, مزدوج lon_A, مزدوج lat_B, مزدوج lon_B, عائم dist,
					مزدوج *lat_target, مزدوج *lon_target)
{
	ܐܢ (fabsf(dist) < FLT_EPSILON) {
		*lat_target = lat_A;
		*lon_target = lon_A;

	} ܐܚܪܢܐ {
		عائم heading = get_bearing_to_next_waypoint(lat_A, lon_A, lat_B, lon_B);
		waypoint_from_heading_and_distance(lat_A, lon_A, heading, dist, lat_target, lon_target);
	}
}

فراغ waypoint_from_heading_and_distance(مزدوج lat_start, مزدوج lon_start, عائم bearing, عائم dist,
					مزدوج *lat_target, مزدوج *lon_target)
{
	bearing = wrap_2pi(bearing);

	مزدوج radius_ratio = static_cast<مزدوج>(dist) / CONSTANTS_RADIUS_OF_EARTH;

	مزدوج lat_start_rad = math::radians(lat_start);
	مزدوج lon_start_rad = math::radians(lon_start);

	*lat_target = asin(sin(lat_start_rad) * cos(radius_ratio) + cos(lat_start_rad) * sin(radius_ratio) * cos((
				   مزدوج)bearing));
	*lon_target = lon_start_rad + atan2(sin((مزدوج)bearing) * sin(radius_ratio) * cos(lat_start_rad),
					    cos(radius_ratio) - sin(lat_start_rad) * sin(*lat_target));

	*lat_target = math::degrees(*lat_target);
	*lon_target = math::degrees(*lon_target);
}

عائم get_bearing_to_next_waypoint(مزدوج lat_now, مزدوج lon_now, مزدوج lat_next, مزدوج lon_next)
{
	ثابت مزدوج lat_now_rad = math::radians(lat_now);
	ثابت مزدوج lat_next_rad = math::radians(lat_next);

	ثابت مزدوج cos_lat_next = cos(lat_next_rad);
	ثابت مزدوج d_lon = math::radians(lon_next - lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */

	ثابت عائم y = static_cast<عائم>(sin(d_lon) * cos_lat_next);
	ثابت عائم x = static_cast<عائم>(cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos_lat_next * cos(d_lon));

	ܦܢܐ wrap_pi(atan2f(y, x));
}

فراغ
get_vector_to_next_waypoint(مزدوج lat_now, مزدوج lon_now, مزدوج lat_next, مزدوج lon_next, عائم *v_n, عائم *v_e)
{
	ثابت مزدوج lat_now_rad = math::radians(lat_now);
	ثابت مزدوج lat_next_rad = math::radians(lat_next);
	ثابت مزدوج d_lon = math::radians(lon_next) - math::radians(lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<عائم>(CONSTANTS_RADIUS_OF_EARTH * (cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos(
					  lat_next_rad) * cos(d_lon)));
	*v_e = static_cast<عائم>(CONSTANTS_RADIUS_OF_EARTH * sin(d_lon) * cos(lat_next_rad));
}

فراغ
get_vector_to_next_waypoint_fast(مزدوج lat_now, مزدوج lon_now, مزدوج lat_next, مزدوج lon_next, عائم *v_n,
				 عائم *v_e)
{
	مزدوج lat_now_rad = math::radians(lat_now);
	مزدوج lon_now_rad = math::radians(lon_now);
	مزدوج lat_next_rad = math::radians(lat_next);
	مزدوج lon_next_rad = math::radians(lon_next);

	مزدوج d_lat = lat_next_rad - lat_now_rad;
	مزدوج d_lon = lon_next_rad - lon_now_rad;

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<عائم>(CONSTANTS_RADIUS_OF_EARTH * d_lat);
	*v_e = static_cast<عائم>(CONSTANTS_RADIUS_OF_EARTH * d_lon * cos(lat_now_rad));
}

فراغ add_vector_to_global_position(مزدوج lat_now, مزدوج lon_now, عائم v_n, عائم v_e, مزدوج *lat_res,
				   مزدوج *lon_res)
{
	مزدوج lat_now_rad = math::radians(lat_now);
	مزدوج lon_now_rad = math::radians(lon_now);

	*lat_res = math::degrees(lat_now_rad + (مزدوج)v_n / CONSTANTS_RADIUS_OF_EARTH);
	*lon_res = math::degrees(lon_now_rad + (مزدوج)v_e / (CONSTANTS_RADIUS_OF_EARTH * cos(lat_now_rad)));
}

// Additional functions - @author Doug Weibel <douglas.weibel@colorado.edu>

صحيح get_distance_to_line(بنية crosstrack_error_s &crosstrack_error, مزدوج lat_now, مزدوج lon_now,
			 مزدوج lat_start, مزدوج lon_start, مزدوج lat_end, مزدوج lon_end)
{
	// This function returns the distance to the nearest point on the track line.  Distance is positive if current
	// position is right of the track and negative if left of the track as seen from a point on the track line
	// headed towards the end point.

	صحيح return_value = -1;	// Set error flag, cleared when valid result calculated.
	crosstrack_error.past_end = ܕܓܠܐ;
	crosstrack_error.distance = 0.0f;
	crosstrack_error.bearing = 0.0f;

	عائم dist_to_end = get_distance_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);

	// Return error if arguments are bad
	ܐܢ (dist_to_end < 0.1f) {
		ܦܢܐ -1;
	}

	عائم bearing_end = get_bearing_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);
	عائم bearing_track = get_bearing_to_next_waypoint(lat_start, lon_start, lat_end, lon_end);
	عائم bearing_diff = wrap_pi(bearing_track - bearing_end);

	// Return past_end = true if past end point of line
	ܐܢ (bearing_diff > M_PI_2_F || bearing_diff < -M_PI_2_F) {
		crosstrack_error.past_end = ܫܪܪܐ;
		return_value = 0;
		ܦܢܐ return_value;
	}

	crosstrack_error.distance = (dist_to_end) * sinf(bearing_diff);

	ܐܢ (sinf(bearing_diff) >= 0) {
		crosstrack_error.bearing = wrap_pi(bearing_track - M_PI_2_F);

	} ܐܚܪܢܐ {
		crosstrack_error.bearing = wrap_pi(bearing_track + M_PI_2_F);
	}

	return_value = 0;

	ܦܢܐ return_value;
}

صحيح get_distance_to_arc(بنية crosstrack_error_s *crosstrack_error, مزدوج lat_now, مزدوج lon_now,
			مزدوج lat_center, مزدوج lon_center,
			عائم radius, عائم arc_start_bearing, عائم arc_sweep)
{
	// This function returns the distance to the nearest point on the track arc.  Distance is positive if current
	// position is right of the arc and negative if left of the arc as seen from the closest point on the arc and
	// headed towards the end point.

	// Determine if the current position is inside or outside the sector between the line from the center
	// to the arc start and the line from the center to the arc end
	ثابت عائم bearing_now = get_bearing_to_next_waypoint(lat_now, lon_now, lat_center, lon_center);

	صحيح return_value = -1;		// Set error flag, cleared when valid result calculated.
	crosstrack_error->past_end = ܕܓܠܐ;
	crosstrack_error->distance = 0.0f;
	crosstrack_error->bearing = 0.0f;

	// Return error if arguments are bad
	ܐܢ (radius < 0.1f) {
		ܦܢܐ return_value;
	}

	// arc_start_bearing and arc_sweep are referenced to the center, so test membership with the
	// center->vehicle bearing. Measuring it in the direction of the sweep handles either sign and
	// any zero crossing without special cases.
	// NOLINTNEXTLINE(readability-suspicious-call-argument) intentional reverse bearing: center -> vehicle
	ثابت عائم bearing_center_to_now = get_bearing_to_next_waypoint(lat_center, lon_center, lat_now, lon_now);
	ثابت عائم bearing_from_start = wrap_2pi((bearing_center_to_now - arc_start_bearing) * (arc_sweep < 0.0f ? -1.0f :
					 1.0f));
	ثابت منطقي in_sector = bearing_from_start <= fabsf(arc_sweep);

	// If in the sector then calculate distance and bearing to closest point
	ܐܢ (in_sector) {
		crosstrack_error->past_end = ܕܓܠܐ;
		عائم dist_to_center = get_distance_to_next_waypoint(lat_now, lon_now, lat_center, lon_center);

		ܐܢ (dist_to_center <= radius) {
			crosstrack_error->distance = radius - dist_to_center;
			crosstrack_error->bearing = bearing_now + M_PI_F;

		} ܐܚܪܢܐ {
			crosstrack_error->distance = dist_to_center - radius;
			crosstrack_error->bearing = bearing_now;
		}

		// If out of the sector then calculate dist and bearing to start or end point

	} ܐܚܪܢܐ {

		// Use the approximation  that 111,111 meters in the y direction is 1 degree (of latitude)
		// and 111,111 * cos(latitude) meters in the x direction is 1 degree (of longitude) to
		// calculate the position of the start and end points.  We should not be doing this often
		// as this function generally will not be called repeatedly when we are out of the sector.

		مزدوج start_disp_x = (مزدوج)radius * sin((مزدوج)arc_start_bearing);
		مزدوج start_disp_y = (مزدوج)radius * cos((مزدوج)arc_start_bearing);
		مزدوج end_disp_x = (مزدوج)radius * sin((مزدوج)wrap_pi(arc_start_bearing + arc_sweep));
		مزدوج end_disp_y = (مزدوج)radius * cos((مزدوج)wrap_pi(arc_start_bearing + arc_sweep));
		ثابت مزدوج cos_lat_center = cos(math::radians(lat_center));
		مزدوج lon_start = lon_center + start_disp_x / (111111.0 * cos_lat_center);
		مزدوج lat_start = lat_center + start_disp_y / 111111.0;
		مزدوج lon_end = lon_center + end_disp_x / (111111.0 * cos_lat_center);
		مزدوج lat_end = lat_center + end_disp_y / 111111.0;
		عائم dist_to_start = get_distance_to_next_waypoint(lat_now, lon_now, lat_start, lon_start);
		عائم dist_to_end = get_distance_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);

		ܐܢ (dist_to_start < dist_to_end) {
			crosstrack_error->distance = dist_to_start;
			crosstrack_error->bearing = get_bearing_to_next_waypoint(lat_now, lon_now, lat_start, lon_start);

		} ܐܚܪܢܐ {
			crosstrack_error->past_end = ܫܪܪܐ;
			crosstrack_error->distance = dist_to_end;
			crosstrack_error->bearing = get_bearing_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);
		}
	}

	crosstrack_error->bearing = wrap_pi(crosstrack_error->bearing);
	return_value = 0;

	ܦܢܐ return_value;
}

عائم get_distance_to_point_global_wgs84(مزدوج lat_now, مزدوج lon_now, عائم alt_now,
		مزدوج lat_next, مزدوج lon_next, عائم alt_next,
		عائم *dist_xy, عائم *dist_z)
{
	مزدوج current_x_rad = math::radians(lat_next);
	مزدوج current_y_rad = math::radians(lon_next);
	مزدوج x_rad = math::radians(lat_now);
	مزدوج y_rad = math::radians(lon_now);

	مزدوج d_lat = x_rad - current_x_rad;
	مزدوج d_lon = y_rad - current_y_rad;

	مزدوج a = sin(d_lat / 2.0) * sin(d_lat / 2.0) + sin(d_lon / 2.0) * sin(d_lon / 2.0) * cos(current_x_rad) * cos(x_rad);
	مزدوج c = 2 * atan2(sqrt(a), sqrt(1 - a));

	ثابت عائم dxy = static_cast<عائم>(CONSTANTS_RADIUS_OF_EARTH * c);
	ثابت عائم dz = static_cast<عائم>(alt_now - alt_next);

	*dist_xy = fabsf(dxy);
	*dist_z = fabsf(dz);

	ܦܢܐ sqrtf(dxy * dxy + dz * dz);
}

عائم mavlink_wpm_distance_to_point_local(عائم x_now, عائم y_now, عائم z_now,
		عائم x_next, عائم y_next, عائم z_next,
		عائم *dist_xy, عائم *dist_z)
{
	عائم dx = x_now - x_next;
	عائم dy = y_now - y_next;
	عائم dz = z_now - z_next;

	*dist_xy = sqrtf(dx * dx + dy * dy);
	*dist_z = fabsf(dz);

	ܦܢܐ sqrtf(dx * dx + dy * dy + dz * dz);
}
