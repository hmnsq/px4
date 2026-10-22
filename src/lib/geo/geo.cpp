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

#شامل "geo.h"

#شامل <float.h>

using matrix::wrap_pi;
using matrix::wrap_2pi;

/*
 * Azimuthal Equidistant Projection
 * formulas according to: http://mathworld.wolfram.com/AzimuthalEquidistantProjection.html
 */

تهی MapProjection::initReference(دوگانه lat_0, دوگانه lon_0, uint64_t timestamp)
{
	_ref_timestamp = timestamp;
	_ref_lat = math::radians(lat_0);
	_ref_lon = math::radians(lon_0);
	_ref_sin_lat = sin(_ref_lat);
	_ref_cos_lat = cos(_ref_lat);
	_ref_init_done = درست;
}

تهی MapProjection::project(دوگانه lat, دوگانه lon, اعشاری &x, اعشاری &y) ثابت
{
	ثابت دوگانه lat_rad = math::radians(lat);
	ثابت دوگانه lon_rad = math::radians(lon);

	ثابت دوگانه sin_lat = sin(lat_rad);
	ثابت دوگانه cos_lat = cos(lat_rad);

	ثابت دوگانه cos_d_lon = cos(lon_rad - _ref_lon);

	ثابت دوگانه arg = math::constrain(_ref_sin_lat * sin_lat + _ref_cos_lat * cos_lat * cos_d_lon, -1.0,  1.0);
	ثابت دوگانه c = acos(arg);

	دوگانه k = 1.0;

	اگر (fabs(c) > 0) {
		k = (c / sin(c));
	}

	x = static_cast<اعشاری>(k * (_ref_cos_lat * sin_lat - _ref_sin_lat * cos_lat * cos_d_lon) * CONSTANTS_RADIUS_OF_EARTH);
	y = static_cast<اعشاری>(k * cos_lat * sin(lon_rad - _ref_lon) * CONSTANTS_RADIUS_OF_EARTH);
}

تهی MapProjection::reproject(اعشاری x, اعشاری y, دوگانه &lat, دوگانه &lon) ثابت
{
	ثابت دوگانه x_rad = (دوگانه)x / CONSTANTS_RADIUS_OF_EARTH;
	ثابت دوگانه y_rad = (دوگانه)y / CONSTANTS_RADIUS_OF_EARTH;
	ثابت دوگانه c = sqrt(x_rad * x_rad + y_rad * y_rad);

	اگر (fabs(c) > 0) {
		ثابت دوگانه sin_c = sin(c);
		ثابت دوگانه cos_c = cos(c);

		ثابت دوگانه lat_rad = asin(cos_c * _ref_sin_lat + (x_rad * sin_c * _ref_cos_lat) / c);
		ثابت دوگانه lon_rad = (_ref_lon + atan2(y_rad * sin_c, c * _ref_cos_lat * cos_c - x_rad * _ref_sin_lat * sin_c));

		lat = math::degrees(lat_rad);
		lon = math::degrees(lon_rad);

	} وگرنه {
		lat = math::degrees(_ref_lat);
		lon = math::degrees(_ref_lon);
	}
}

اعشاری get_distance_to_next_waypoint(دوگانه lat_now, دوگانه lon_now, دوگانه lat_next, دوگانه lon_next)
{
	ثابت دوگانه lat_now_rad = math::radians(lat_now);
	ثابت دوگانه lat_next_rad = math::radians(lat_next);

	ثابت دوگانه d_lat = lat_next_rad - lat_now_rad;
	ثابت دوگانه d_lon = math::radians(lon_next) - math::radians(lon_now);

	ثابت دوگانه a = sin(d_lat / 2.0) * sin(d_lat / 2.0) + sin(d_lon / 2.0) * sin(d_lon / 2.0) * cos(lat_now_rad) * cos(
				 lat_next_rad);

	ثابت دوگانه c = atan2(sqrt(a), sqrt(1.0 - a));

	بازگشت static_cast<اعشاری>(CONSTANTS_RADIUS_OF_EARTH * 2.0 * c);
}

تهی create_waypoint_from_سطر_and_dist(دوگانه lat_A, دوگانه lon_A, دوگانه lat_B, دوگانه lon_B, اعشاری dist,
					دوگانه *lat_target, دوگانه *lon_target)
{
	اگر (fabsf(dist) < FLT_EPSILON) {
		*lat_target = lat_A;
		*lon_target = lon_A;

	} وگرنه {
		اعشاری heading = get_bearing_to_next_waypoint(lat_A, lon_A, lat_B, lon_B);
		waypoint_from_heading_and_distance(lat_A, lon_A, heading, dist, lat_target, lon_target);
	}
}

تهی waypoint_from_heading_and_distance(دوگانه lat_start, دوگانه lon_start, اعشاری bearing, اعشاری dist,
					دوگانه *lat_target, دوگانه *lon_target)
{
	bearing = wrap_2pi(bearing);

	دوگانه radius_ratio = static_cast<دوگانه>(dist) / CONSTANTS_RADIUS_OF_EARTH;

	دوگانه lat_start_rad = math::radians(lat_start);
	دوگانه lon_start_rad = math::radians(lon_start);

	*lat_target = asin(sin(lat_start_rad) * cos(radius_ratio) + cos(lat_start_rad) * sin(radius_ratio) * cos((
				   دوگانه)bearing));
	*lon_target = lon_start_rad + atan2(sin((دوگانه)bearing) * sin(radius_ratio) * cos(lat_start_rad),
					    cos(radius_ratio) - sin(lat_start_rad) * sin(*lat_target));

	*lat_target = math::degrees(*lat_target);
	*lon_target = math::degrees(*lon_target);
}

اعشاری get_bearing_to_next_waypoint(دوگانه lat_now, دوگانه lon_now, دوگانه lat_next, دوگانه lon_next)
{
	ثابت دوگانه lat_now_rad = math::radians(lat_now);
	ثابت دوگانه lat_next_rad = math::radians(lat_next);

	ثابت دوگانه cos_lat_next = cos(lat_next_rad);
	ثابت دوگانه d_lon = math::radians(lon_next - lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */

	ثابت اعشاری y = static_cast<اعشاری>(sin(d_lon) * cos_lat_next);
	ثابت اعشاری x = static_cast<اعشاری>(cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos_lat_next * cos(d_lon));

	بازگشت wrap_pi(atan2f(y, x));
}

تهی
get_vector_to_next_waypoint(دوگانه lat_now, دوگانه lon_now, دوگانه lat_next, دوگانه lon_next, اعشاری *v_n, اعشاری *v_e)
{
	ثابت دوگانه lat_now_rad = math::radians(lat_now);
	ثابت دوگانه lat_next_rad = math::radians(lat_next);
	ثابت دوگانه d_lon = math::radians(lon_next) - math::radians(lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<اعشاری>(CONSTANTS_RADIUS_OF_EARTH * (cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos(
					  lat_next_rad) * cos(d_lon)));
	*v_e = static_cast<اعشاری>(CONSTANTS_RADIUS_OF_EARTH * sin(d_lon) * cos(lat_next_rad));
}

تهی
get_vector_to_next_waypoint_fast(دوگانه lat_now, دوگانه lon_now, دوگانه lat_next, دوگانه lon_next, اعشاری *v_n,
				 اعشاری *v_e)
{
	دوگانه lat_now_rad = math::radians(lat_now);
	دوگانه lon_now_rad = math::radians(lon_now);
	دوگانه lat_next_rad = math::radians(lat_next);
	دوگانه lon_next_rad = math::radians(lon_next);

	دوگانه d_lat = lat_next_rad - lat_now_rad;
	دوگانه d_lon = lon_next_rad - lon_now_rad;

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<اعشاری>(CONSTANTS_RADIUS_OF_EARTH * d_lat);
	*v_e = static_cast<اعشاری>(CONSTANTS_RADIUS_OF_EARTH * d_lon * cos(lat_now_rad));
}

تهی add_vector_to_global_position(دوگانه lat_now, دوگانه lon_now, اعشاری v_n, اعشاری v_e, دوگانه *lat_res,
				   دوگانه *lon_res)
{
	دوگانه lat_now_rad = math::radians(lat_now);
	دوگانه lon_now_rad = math::radians(lon_now);

	*lat_res = math::degrees(lat_now_rad + (دوگانه)v_n / CONSTANTS_RADIUS_OF_EARTH);
	*lon_res = math::degrees(lon_now_rad + (دوگانه)v_e / (CONSTANTS_RADIUS_OF_EARTH * cos(lat_now_rad)));
}

// Additional functions - @author Doug Weibel <douglas.weibel@colorado.edu>

صحیح get_distance_to_سطر(ساختار crosstrack_error_s &crosstrack_error, دوگانه lat_now, دوگانه lon_now,
			 دوگانه lat_start, دوگانه lon_start, دوگانه lat_end, دوگانه lon_end)
{
	// This function returns the distance to the nearest point on the track line.  Distance is positive if current
	// position is right of the track and negative if left of the track as seen from a point on the track line
	// headed towards the end point.

	صحیح return_مقدار = -1;	// Set error flag, cleared when valid result calculated.
	crosstrack_error.past_end = نادرست;
	crosstrack_error.distance = 0.0f;
	crosstrack_error.bearing = 0.0f;

	اعشاری dist_to_end = get_distance_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);

	// Return error if arguments are bad
	اگر (dist_to_end < 0.1f) {
		بازگشت -1;
	}

	اعشاری bearing_end = get_bearing_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);
	اعشاری bearing_track = get_bearing_to_next_waypoint(lat_start, lon_start, lat_end, lon_end);
	اعشاری bearing_diff = wrap_pi(bearing_track - bearing_end);

	// Return past_end = true if past end point of line
	اگر (bearing_diff > M_PI_2_F || bearing_diff < -M_PI_2_F) {
		crosstrack_error.past_end = درست;
		return_مقدار = 0;
		بازگشت return_مقدار;
	}

	crosstrack_error.distance = (dist_to_end) * sinf(bearing_diff);

	اگر (sinf(bearing_diff) >= 0) {
		crosstrack_error.bearing = wrap_pi(bearing_track - M_PI_2_F);

	} وگرنه {
		crosstrack_error.bearing = wrap_pi(bearing_track + M_PI_2_F);
	}

	return_مقدار = 0;

	بازگشت return_مقدار;
}

صحیح get_distance_to_arc(ساختار crosstrack_error_s *crosstrack_error, دوگانه lat_now, دوگانه lon_now,
			دوگانه lat_center, دوگانه lon_center,
			اعشاری radius, اعشاری arc_start_bearing, اعشاری arc_sweep)
{
	// This function returns the distance to the nearest point on the track arc.  Distance is positive if current
	// position is right of the arc and negative if left of the arc as seen from the closest point on the arc and
	// headed towards the end point.

	// Determine if the current position is inside or outside the sector between the line from the center
	// to the arc start and the line from the center to the arc end
	ثابت اعشاری bearing_now = get_bearing_to_next_waypoint(lat_now, lon_now, lat_center, lon_center);

	صحیح return_مقدار = -1;		// Set error flag, cleared when valid result calculated.
	crosstrack_error->past_end = نادرست;
	crosstrack_error->distance = 0.0f;
	crosstrack_error->bearing = 0.0f;

	// Return error if arguments are bad
	اگر (radius < 0.1f) {
		بازگشت return_مقدار;
	}

	// arc_start_bearing and arc_sweep are referenced to the center, so test membership with the
	// center->vehicle bearing. Measuring it in the direction of the sweep handles either sign and
	// any zero crossing without special cases.
	// NOLINTNEXTLINE(readability-suspicious-call-argument) intentional reverse bearing: center -> vehicle
	ثابت اعشاری bearing_center_to_now = get_bearing_to_next_waypoint(lat_center, lon_center, lat_now, lon_now);
	ثابت اعشاری bearing_from_start = wrap_2pi((bearing_center_to_now - arc_start_bearing) * (arc_sweep < 0.0f ? -1.0f :
					 1.0f));
	ثابت منطقی in_sector = bearing_from_start <= fabsf(arc_sweep);

	// If in the sector then calculate distance and bearing to closest point
	اگر (in_sector) {
		crosstrack_error->past_end = نادرست;
		اعشاری dist_to_center = get_distance_to_next_waypoint(lat_now, lon_now, lat_center, lon_center);

		اگر (dist_to_center <= radius) {
			crosstrack_error->distance = radius - dist_to_center;
			crosstrack_error->bearing = bearing_now + M_PI_F;

		} وگرنه {
			crosstrack_error->distance = dist_to_center - radius;
			crosstrack_error->bearing = bearing_now;
		}

		// If out of the sector then calculate dist and bearing to start or end point

	} وگرنه {

		// Use the approximation  that 111,111 meters in the y direction is 1 degree (of latitude)
		// and 111,111 * cos(latitude) meters in the x direction is 1 degree (of longitude) to
		// calculate the position of the start and end points.  We should not be doing this often
		// as this function generally will not be called repeatedly when we are out of the sector.

		دوگانه start_disp_x = (دوگانه)radius * sin((دوگانه)arc_start_bearing);
		دوگانه start_disp_y = (دوگانه)radius * cos((دوگانه)arc_start_bearing);
		دوگانه end_disp_x = (دوگانه)radius * sin((دوگانه)wrap_pi(arc_start_bearing + arc_sweep));
		دوگانه end_disp_y = (دوگانه)radius * cos((دوگانه)wrap_pi(arc_start_bearing + arc_sweep));
		ثابت دوگانه cos_lat_center = cos(math::radians(lat_center));
		دوگانه lon_start = lon_center + start_disp_x / (111111.0 * cos_lat_center);
		دوگانه lat_start = lat_center + start_disp_y / 111111.0;
		دوگانه lon_end = lon_center + end_disp_x / (111111.0 * cos_lat_center);
		دوگانه lat_end = lat_center + end_disp_y / 111111.0;
		اعشاری dist_to_start = get_distance_to_next_waypoint(lat_now, lon_now, lat_start, lon_start);
		اعشاری dist_to_end = get_distance_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);

		اگر (dist_to_start < dist_to_end) {
			crosstrack_error->distance = dist_to_start;
			crosstrack_error->bearing = get_bearing_to_next_waypoint(lat_now, lon_now, lat_start, lon_start);

		} وگرنه {
			crosstrack_error->past_end = درست;
			crosstrack_error->distance = dist_to_end;
			crosstrack_error->bearing = get_bearing_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);
		}
	}

	crosstrack_error->bearing = wrap_pi(crosstrack_error->bearing);
	return_مقدار = 0;

	بازگشت return_مقدار;
}

اعشاری get_distance_to_point_global_wgs84(دوگانه lat_now, دوگانه lon_now, اعشاری alt_now,
		دوگانه lat_next, دوگانه lon_next, اعشاری alt_next,
		اعشاری *dist_xy, اعشاری *dist_z)
{
	دوگانه current_x_rad = math::radians(lat_next);
	دوگانه current_y_rad = math::radians(lon_next);
	دوگانه x_rad = math::radians(lat_now);
	دوگانه y_rad = math::radians(lon_now);

	دوگانه d_lat = x_rad - current_x_rad;
	دوگانه d_lon = y_rad - current_y_rad;

	دوگانه a = sin(d_lat / 2.0) * sin(d_lat / 2.0) + sin(d_lon / 2.0) * sin(d_lon / 2.0) * cos(current_x_rad) * cos(x_rad);
	دوگانه c = 2 * atan2(sqrt(a), sqrt(1 - a));

	ثابت اعشاری dxy = static_cast<اعشاری>(CONSTANTS_RADIUS_OF_EARTH * c);
	ثابت اعشاری dz = static_cast<اعشاری>(alt_now - alt_next);

	*dist_xy = fabsf(dxy);
	*dist_z = fabsf(dz);

	بازگشت sqrtf(dxy * dxy + dz * dz);
}

اعشاری mavlink_wpm_distance_to_point_local(اعشاری x_now, اعشاری y_now, اعشاری z_now,
		اعشاری x_next, اعشاری y_next, اعشاری z_next,
		اعشاری *dist_xy, اعشاری *dist_z)
{
	اعشاری dx = x_now - x_next;
	اعشاری dy = y_now - y_next;
	اعشاری dz = z_now - z_next;

	*dist_xy = sqrtf(dx * dx + dy * dy);
	*dist_z = fabsf(dz);

	بازگشت sqrtf(dx * dx + dy * dy + dz * dz);
}
