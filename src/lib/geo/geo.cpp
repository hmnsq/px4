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

#অন্তর্ভাব "geo.h"

#অন্তর্ভাব <float.h>

using matrix::wrap_pi;
using matrix::wrap_2pi;

/*
 * Azimuthal Equidistant Projection
 * formulas according to: http://mathworld.wolfram.com/AzimuthalEquidistantProjection.html
 */

শূন্য MapProjection::initReference(দ্বিগুণ lat_0, দ্বিগুণ lon_0, uint64_t timestamp)
{
	_ref_timestamp = timestamp;
	_ref_lat = math::radians(lat_0);
	_ref_lon = math::radians(lon_0);
	_ref_sin_lat = sin(_ref_lat);
	_ref_cos_lat = cos(_ref_lat);
	_ref_init_done = সত্য;
}

শূন্য MapProjection::project(দ্বিগুণ lat, দ্বিগুণ lon, ভিন্ন &x, ভিন্ন &y) ধ্রুব
{
	ধ্রুব দ্বিগুণ lat_rad = math::radians(lat);
	ধ্রুব দ্বিগুণ lon_rad = math::radians(lon);

	ধ্রুব দ্বিগুণ sin_lat = sin(lat_rad);
	ধ্রুব দ্বিগুণ cos_lat = cos(lat_rad);

	ধ্রুব দ্বিগুণ cos_d_lon = cos(lon_rad - _ref_lon);

	ধ্রুব দ্বিগুণ arg = math::constrain(_ref_sin_lat * sin_lat + _ref_cos_lat * cos_lat * cos_d_lon, -1.0,  1.0);
	ধ্রুব দ্বিগুণ c = acos(arg);

	দ্বিগুণ k = 1.0;

	যদি (fabs(c) > 0) {
		k = (c / sin(c));
	}

	x = static_cast<ভিন্ন>(k * (_ref_cos_lat * sin_lat - _ref_sin_lat * cos_lat * cos_d_lon) * CONSTANTS_RADIUS_OF_EARTH);
	y = static_cast<ভিন্ন>(k * cos_lat * sin(lon_rad - _ref_lon) * CONSTANTS_RADIUS_OF_EARTH);
}

শূন্য MapProjection::reproject(ভিন্ন x, ভিন্ন y, দ্বিগুণ &lat, দ্বিগুণ &lon) ধ্রুব
{
	ধ্রুব দ্বিগুণ x_rad = (দ্বিগুণ)x / CONSTANTS_RADIUS_OF_EARTH;
	ধ্রুব দ্বিগুণ y_rad = (দ্বিগুণ)y / CONSTANTS_RADIUS_OF_EARTH;
	ধ্রুব দ্বিগুণ c = sqrt(x_rad * x_rad + y_rad * y_rad);

	যদি (fabs(c) > 0) {
		ধ্রুব দ্বিগুণ sin_c = sin(c);
		ধ্রুব দ্বিগুণ cos_c = cos(c);

		ধ্রুব দ্বিগুণ lat_rad = asin(cos_c * _ref_sin_lat + (x_rad * sin_c * _ref_cos_lat) / c);
		ধ্রুব দ্বিগুণ lon_rad = (_ref_lon + atan2(y_rad * sin_c, c * _ref_cos_lat * cos_c - x_rad * _ref_sin_lat * sin_c));

		lat = math::degrees(lat_rad);
		lon = math::degrees(lon_rad);

	} অন্যথা {
		lat = math::degrees(_ref_lat);
		lon = math::degrees(_ref_lon);
	}
}

ভিন্ন get_distance_to_next_waypoint(দ্বিগুণ lat_now, দ্বিগুণ lon_now, দ্বিগুণ lat_next, দ্বিগুণ lon_next)
{
	ধ্রুব দ্বিগুণ lat_now_rad = math::radians(lat_now);
	ধ্রুব দ্বিগুণ lat_next_rad = math::radians(lat_next);

	ধ্রুব দ্বিগুণ d_lat = lat_next_rad - lat_now_rad;
	ধ্রুব দ্বিগুণ d_lon = math::radians(lon_next) - math::radians(lon_now);

	ধ্রুব দ্বিগুণ a = sin(d_lat / 2.0) * sin(d_lat / 2.0) + sin(d_lon / 2.0) * sin(d_lon / 2.0) * cos(lat_now_rad) * cos(
				 lat_next_rad);

	ধ্রুব দ্বিগুণ c = atan2(sqrt(a), sqrt(1.0 - a));

	প্রত্যাগম static_cast<ভিন্ন>(CONSTANTS_RADIUS_OF_EARTH * 2.0 * c);
}

শূন্য create_waypoint_from_সারি_and_dist(দ্বিগুণ lat_A, দ্বিগুণ lon_A, দ্বিগুণ lat_B, দ্বিগুণ lon_B, ভিন্ন dist,
					দ্বিগুণ *lat_target, দ্বিগুণ *lon_target)
{
	যদি (fabsf(dist) < FLT_EPSILON) {
		*lat_target = lat_A;
		*lon_target = lon_A;

	} অন্যথা {
		ভিন্ন heading = get_bearing_to_next_waypoint(lat_A, lon_A, lat_B, lon_B);
		waypoint_from_heading_and_distance(lat_A, lon_A, heading, dist, lat_target, lon_target);
	}
}

শূন্য waypoint_from_heading_and_distance(দ্বিগুণ lat_start, দ্বিগুণ lon_start, ভিন্ন bearing, ভিন্ন dist,
					দ্বিগুণ *lat_target, দ্বিগুণ *lon_target)
{
	bearing = wrap_2pi(bearing);

	দ্বিগুণ radius_ratio = static_cast<দ্বিগুণ>(dist) / CONSTANTS_RADIUS_OF_EARTH;

	দ্বিগুণ lat_start_rad = math::radians(lat_start);
	দ্বিগুণ lon_start_rad = math::radians(lon_start);

	*lat_target = asin(sin(lat_start_rad) * cos(radius_ratio) + cos(lat_start_rad) * sin(radius_ratio) * cos((
				   দ্বিগুণ)bearing));
	*lon_target = lon_start_rad + atan2(sin((দ্বিগুণ)bearing) * sin(radius_ratio) * cos(lat_start_rad),
					    cos(radius_ratio) - sin(lat_start_rad) * sin(*lat_target));

	*lat_target = math::degrees(*lat_target);
	*lon_target = math::degrees(*lon_target);
}

ভিন্ন get_bearing_to_next_waypoint(দ্বিগুণ lat_now, দ্বিগুণ lon_now, দ্বিগুণ lat_next, দ্বিগুণ lon_next)
{
	ধ্রুব দ্বিগুণ lat_now_rad = math::radians(lat_now);
	ধ্রুব দ্বিগুণ lat_next_rad = math::radians(lat_next);

	ধ্রুব দ্বিগুণ cos_lat_next = cos(lat_next_rad);
	ধ্রুব দ্বিগুণ d_lon = math::radians(lon_next - lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */

	ধ্রুব ভিন্ন y = static_cast<ভিন্ন>(sin(d_lon) * cos_lat_next);
	ধ্রুব ভিন্ন x = static_cast<ভিন্ন>(cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos_lat_next * cos(d_lon));

	প্রত্যাগম wrap_pi(atan2f(y, x));
}

শূন্য
get_vector_to_next_waypoint(দ্বিগুণ lat_now, দ্বিগুণ lon_now, দ্বিগুণ lat_next, দ্বিগুণ lon_next, ভিন্ন *v_n, ভিন্ন *v_e)
{
	ধ্রুব দ্বিগুণ lat_now_rad = math::radians(lat_now);
	ধ্রুব দ্বিগুণ lat_next_rad = math::radians(lat_next);
	ধ্রুব দ্বিগুণ d_lon = math::radians(lon_next) - math::radians(lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<ভিন্ন>(CONSTANTS_RADIUS_OF_EARTH * (cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos(
					  lat_next_rad) * cos(d_lon)));
	*v_e = static_cast<ভিন্ন>(CONSTANTS_RADIUS_OF_EARTH * sin(d_lon) * cos(lat_next_rad));
}

শূন্য
get_vector_to_next_waypoint_fast(দ্বিগুণ lat_now, দ্বিগুণ lon_now, দ্বিগুণ lat_next, দ্বিগুণ lon_next, ভিন্ন *v_n,
				 ভিন্ন *v_e)
{
	দ্বিগুণ lat_now_rad = math::radians(lat_now);
	দ্বিগুণ lon_now_rad = math::radians(lon_now);
	দ্বিগুণ lat_next_rad = math::radians(lat_next);
	দ্বিগুণ lon_next_rad = math::radians(lon_next);

	দ্বিগুণ d_lat = lat_next_rad - lat_now_rad;
	দ্বিগুণ d_lon = lon_next_rad - lon_now_rad;

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<ভিন্ন>(CONSTANTS_RADIUS_OF_EARTH * d_lat);
	*v_e = static_cast<ভিন্ন>(CONSTANTS_RADIUS_OF_EARTH * d_lon * cos(lat_now_rad));
}

শূন্য add_vector_to_global_position(দ্বিগুণ lat_now, দ্বিগুণ lon_now, ভিন্ন v_n, ভিন্ন v_e, দ্বিগুণ *lat_res,
				   দ্বিগুণ *lon_res)
{
	দ্বিগুণ lat_now_rad = math::radians(lat_now);
	দ্বিগুণ lon_now_rad = math::radians(lon_now);

	*lat_res = math::degrees(lat_now_rad + (দ্বিগুণ)v_n / CONSTANTS_RADIUS_OF_EARTH);
	*lon_res = math::degrees(lon_now_rad + (দ্বিগুণ)v_e / (CONSTANTS_RADIUS_OF_EARTH * cos(lat_now_rad)));
}

// Additional functions - @author Doug Weibel <douglas.weibel@colorado.edu>

পূর্ণাঙ্ক get_distance_to_সারি(সংরচনা crosstrack_error_s &crosstrack_error, দ্বিগুণ lat_now, দ্বিগুণ lon_now,
			 দ্বিগুণ lat_start, দ্বিগুণ lon_start, দ্বিগুণ lat_end, দ্বিগুণ lon_end)
{
	// This function returns the distance to the nearest point on the track line.  Distance is positive if current
	// position is right of the track and negative if left of the track as seen from a point on the track line
	// headed towards the end point.

	পূর্ণাঙ্ক return_মান = -1;	// Set error flag, cleared when valid result calculated.
	crosstrack_error.past_end = অসত্য;
	crosstrack_error.distance = 0.0f;
	crosstrack_error.bearing = 0.0f;

	ভিন্ন dist_to_end = get_distance_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);

	// Return error if arguments are bad
	যদি (dist_to_end < 0.1f) {
		প্রত্যাগম -1;
	}

	ভিন্ন bearing_end = get_bearing_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);
	ভিন্ন bearing_track = get_bearing_to_next_waypoint(lat_start, lon_start, lat_end, lon_end);
	ভিন্ন bearing_diff = wrap_pi(bearing_track - bearing_end);

	// Return past_end = true if past end point of line
	যদি (bearing_diff > M_PI_2_F || bearing_diff < -M_PI_2_F) {
		crosstrack_error.past_end = সত্য;
		return_মান = 0;
		প্রত্যাগম return_মান;
	}

	crosstrack_error.distance = (dist_to_end) * sinf(bearing_diff);

	যদি (sinf(bearing_diff) >= 0) {
		crosstrack_error.bearing = wrap_pi(bearing_track - M_PI_2_F);

	} অন্যথা {
		crosstrack_error.bearing = wrap_pi(bearing_track + M_PI_2_F);
	}

	return_মান = 0;

	প্রত্যাগম return_মান;
}

পূর্ণাঙ্ক get_distance_to_arc(সংরচনা crosstrack_error_s *crosstrack_error, দ্বিগুণ lat_now, দ্বিগুণ lon_now,
			দ্বিগুণ lat_center, দ্বিগুণ lon_center,
			ভিন্ন radius, ভিন্ন arc_start_bearing, ভিন্ন arc_sweep)
{
	// This function returns the distance to the nearest point on the track arc.  Distance is positive if current
	// position is right of the arc and negative if left of the arc as seen from the closest point on the arc and
	// headed towards the end point.

	// Determine if the current position is inside or outside the sector between the line from the center
	// to the arc start and the line from the center to the arc end
	ধ্রুব ভিন্ন bearing_now = get_bearing_to_next_waypoint(lat_now, lon_now, lat_center, lon_center);

	পূর্ণাঙ্ক return_মান = -1;		// Set error flag, cleared when valid result calculated.
	crosstrack_error->past_end = অসত্য;
	crosstrack_error->distance = 0.0f;
	crosstrack_error->bearing = 0.0f;

	// Return error if arguments are bad
	যদি (radius < 0.1f) {
		প্রত্যাগম return_মান;
	}

	// arc_start_bearing and arc_sweep are referenced to the center, so test membership with the
	// center->vehicle bearing. Measuring it in the direction of the sweep handles either sign and
	// any zero crossing without special cases.
	// NOLINTNEXTLINE(readability-suspicious-call-argument) intentional reverse bearing: center -> vehicle
	ধ্রুব ভিন্ন bearing_center_to_now = get_bearing_to_next_waypoint(lat_center, lon_center, lat_now, lon_now);
	ধ্রুব ভিন্ন bearing_from_start = wrap_2pi((bearing_center_to_now - arc_start_bearing) * (arc_sweep < 0.0f ? -1.0f :
					 1.0f));
	ধ্রুব তার্কিক in_sector = bearing_from_start <= fabsf(arc_sweep);

	// If in the sector then calculate distance and bearing to closest point
	যদি (in_sector) {
		crosstrack_error->past_end = অসত্য;
		ভিন্ন dist_to_center = get_distance_to_next_waypoint(lat_now, lon_now, lat_center, lon_center);

		যদি (dist_to_center <= radius) {
			crosstrack_error->distance = radius - dist_to_center;
			crosstrack_error->bearing = bearing_now + M_PI_F;

		} অন্যথা {
			crosstrack_error->distance = dist_to_center - radius;
			crosstrack_error->bearing = bearing_now;
		}

		// If out of the sector then calculate dist and bearing to start or end point

	} অন্যথা {

		// Use the approximation  that 111,111 meters in the y direction is 1 degree (of latitude)
		// and 111,111 * cos(latitude) meters in the x direction is 1 degree (of longitude) to
		// calculate the position of the start and end points.  We should not be doing this often
		// as this function generally will not be called repeatedly when we are out of the sector.

		দ্বিগুণ start_disp_x = (দ্বিগুণ)radius * sin((দ্বিগুণ)arc_start_bearing);
		দ্বিগুণ start_disp_y = (দ্বিগুণ)radius * cos((দ্বিগুণ)arc_start_bearing);
		দ্বিগুণ end_disp_x = (দ্বিগুণ)radius * sin((দ্বিগুণ)wrap_pi(arc_start_bearing + arc_sweep));
		দ্বিগুণ end_disp_y = (দ্বিগুণ)radius * cos((দ্বিগুণ)wrap_pi(arc_start_bearing + arc_sweep));
		ধ্রুব দ্বিগুণ cos_lat_center = cos(math::radians(lat_center));
		দ্বিগুণ lon_start = lon_center + start_disp_x / (111111.0 * cos_lat_center);
		দ্বিগুণ lat_start = lat_center + start_disp_y / 111111.0;
		দ্বিগুণ lon_end = lon_center + end_disp_x / (111111.0 * cos_lat_center);
		দ্বিগুণ lat_end = lat_center + end_disp_y / 111111.0;
		ভিন্ন dist_to_start = get_distance_to_next_waypoint(lat_now, lon_now, lat_start, lon_start);
		ভিন্ন dist_to_end = get_distance_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);

		যদি (dist_to_start < dist_to_end) {
			crosstrack_error->distance = dist_to_start;
			crosstrack_error->bearing = get_bearing_to_next_waypoint(lat_now, lon_now, lat_start, lon_start);

		} অন্যথা {
			crosstrack_error->past_end = সত্য;
			crosstrack_error->distance = dist_to_end;
			crosstrack_error->bearing = get_bearing_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);
		}
	}

	crosstrack_error->bearing = wrap_pi(crosstrack_error->bearing);
	return_মান = 0;

	প্রত্যাগম return_মান;
}

ভিন্ন get_distance_to_point_global_wgs84(দ্বিগুণ lat_now, দ্বিগুণ lon_now, ভিন্ন alt_now,
		দ্বিগুণ lat_next, দ্বিগুণ lon_next, ভিন্ন alt_next,
		ভিন্ন *dist_xy, ভিন্ন *dist_z)
{
	দ্বিগুণ current_x_rad = math::radians(lat_next);
	দ্বিগুণ current_y_rad = math::radians(lon_next);
	দ্বিগুণ x_rad = math::radians(lat_now);
	দ্বিগুণ y_rad = math::radians(lon_now);

	দ্বিগুণ d_lat = x_rad - current_x_rad;
	দ্বিগুণ d_lon = y_rad - current_y_rad;

	দ্বিগুণ a = sin(d_lat / 2.0) * sin(d_lat / 2.0) + sin(d_lon / 2.0) * sin(d_lon / 2.0) * cos(current_x_rad) * cos(x_rad);
	দ্বিগুণ c = 2 * atan2(sqrt(a), sqrt(1 - a));

	ধ্রুব ভিন্ন dxy = static_cast<ভিন্ন>(CONSTANTS_RADIUS_OF_EARTH * c);
	ধ্রুব ভিন্ন dz = static_cast<ভিন্ন>(alt_now - alt_next);

	*dist_xy = fabsf(dxy);
	*dist_z = fabsf(dz);

	প্রত্যাগম sqrtf(dxy * dxy + dz * dz);
}

ভিন্ন mavlink_wpm_distance_to_point_local(ভিন্ন x_now, ভিন্ন y_now, ভিন্ন z_now,
		ভিন্ন x_next, ভিন্ন y_next, ভিন্ন z_next,
		ভিন্ন *dist_xy, ভিন্ন *dist_z)
{
	ভিন্ন dx = x_now - x_next;
	ভিন্ন dy = y_now - y_next;
	ভিন্ন dz = z_now - z_next;

	*dist_xy = sqrtf(dx * dx + dy * dy);
	*dist_z = fabsf(dz);

	প্রত্যাগম sqrtf(dx * dx + dy * dy + dz * dz);
}
