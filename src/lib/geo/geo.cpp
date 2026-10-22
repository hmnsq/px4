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

#அந்தர்பாவ "geo.h"

#அந்தர்பாவ <float.h>

using matrix::wrap_pi;
using matrix::wrap_2pi;

/*
 * Azimuthal Equidistant Projection
 * formulas according to: http://mathworld.wolfram.com/AzimuthalEquidistantProjection.html
 */

வெறுமை MapProjection::initReference(இரட்டை lat_0, இரட்டை lon_0, uint64_t timestamp)
{
	_ref_timestamp = timestamp;
	_ref_lat = math::radians(lat_0);
	_ref_lon = math::radians(lon_0);
	_ref_sin_lat = sin(_ref_lat);
	_ref_cos_lat = cos(_ref_lat);
	_ref_init_done = உண்மை;
}

வெறுமை MapProjection::project(இரட்டை lat, இரட்டை lon, தசம &x, தசம &y) மாறிலி
{
	மாறிலி இரட்டை lat_rad = math::radians(lat);
	மாறிலி இரட்டை lon_rad = math::radians(lon);

	மாறிலி இரட்டை sin_lat = sin(lat_rad);
	மாறிலி இரட்டை cos_lat = cos(lat_rad);

	மாறிலி இரட்டை cos_d_lon = cos(lon_rad - _ref_lon);

	மாறிலி இரட்டை arg = math::constrain(_ref_sin_lat * sin_lat + _ref_cos_lat * cos_lat * cos_d_lon, -1.0,  1.0);
	மாறிலி இரட்டை c = acos(arg);

	இரட்டை k = 1.0;

	எனில் (fabs(c) > 0) {
		k = (c / sin(c));
	}

	x = static_cast<தசம>(k * (_ref_cos_lat * sin_lat - _ref_sin_lat * cos_lat * cos_d_lon) * CONSTANTS_RADIUS_OF_EARTH);
	y = static_cast<தசம>(k * cos_lat * sin(lon_rad - _ref_lon) * CONSTANTS_RADIUS_OF_EARTH);
}

வெறுமை MapProjection::reproject(தசம x, தசம y, இரட்டை &lat, இரட்டை &lon) மாறிலி
{
	மாறிலி இரட்டை x_rad = (இரட்டை)x / CONSTANTS_RADIUS_OF_EARTH;
	மாறிலி இரட்டை y_rad = (இரட்டை)y / CONSTANTS_RADIUS_OF_EARTH;
	மாறிலி இரட்டை c = sqrt(x_rad * x_rad + y_rad * y_rad);

	எனில் (fabs(c) > 0) {
		மாறிலி இரட்டை sin_c = sin(c);
		மாறிலி இரட்டை cos_c = cos(c);

		மாறிலி இரட்டை lat_rad = asin(cos_c * _ref_sin_lat + (x_rad * sin_c * _ref_cos_lat) / c);
		மாறிலி இரட்டை lon_rad = (_ref_lon + atan2(y_rad * sin_c, c * _ref_cos_lat * cos_c - x_rad * _ref_sin_lat * sin_c));

		lat = math::degrees(lat_rad);
		lon = math::degrees(lon_rad);

	} இல்லையெனில் {
		lat = math::degrees(_ref_lat);
		lon = math::degrees(_ref_lon);
	}
}

தசம get_distance_to_next_waypoint(இரட்டை lat_now, இரட்டை lon_now, இரட்டை lat_next, இரட்டை lon_next)
{
	மாறிலி இரட்டை lat_now_rad = math::radians(lat_now);
	மாறிலி இரட்டை lat_next_rad = math::radians(lat_next);

	மாறிலி இரட்டை d_lat = lat_next_rad - lat_now_rad;
	மாறிலி இரட்டை d_lon = math::radians(lon_next) - math::radians(lon_now);

	மாறிலி இரட்டை a = sin(d_lat / 2.0) * sin(d_lat / 2.0) + sin(d_lon / 2.0) * sin(d_lon / 2.0) * cos(lat_now_rad) * cos(
				 lat_next_rad);

	மாறிலி இரட்டை c = atan2(sqrt(a), sqrt(1.0 - a));

	திரும்பு static_cast<தசம>(CONSTANTS_RADIUS_OF_EARTH * 2.0 * c);
}

வெறுமை create_waypoint_from_வரி_and_dist(இரட்டை lat_A, இரட்டை lon_A, இரட்டை lat_B, இரட்டை lon_B, தசம dist,
					இரட்டை *lat_target, இரட்டை *lon_target)
{
	எனில் (fabsf(dist) < FLT_EPSILON) {
		*lat_target = lat_A;
		*lon_target = lon_A;

	} இல்லையெனில் {
		தசம heading = get_bearing_to_next_waypoint(lat_A, lon_A, lat_B, lon_B);
		waypoint_from_heading_and_distance(lat_A, lon_A, heading, dist, lat_target, lon_target);
	}
}

வெறுமை waypoint_from_heading_and_distance(இரட்டை lat_start, இரட்டை lon_start, தசம bearing, தசம dist,
					இரட்டை *lat_target, இரட்டை *lon_target)
{
	bearing = wrap_2pi(bearing);

	இரட்டை radius_ratio = static_cast<இரட்டை>(dist) / CONSTANTS_RADIUS_OF_EARTH;

	இரட்டை lat_start_rad = math::radians(lat_start);
	இரட்டை lon_start_rad = math::radians(lon_start);

	*lat_target = asin(sin(lat_start_rad) * cos(radius_ratio) + cos(lat_start_rad) * sin(radius_ratio) * cos((
				   இரட்டை)bearing));
	*lon_target = lon_start_rad + atan2(sin((இரட்டை)bearing) * sin(radius_ratio) * cos(lat_start_rad),
					    cos(radius_ratio) - sin(lat_start_rad) * sin(*lat_target));

	*lat_target = math::degrees(*lat_target);
	*lon_target = math::degrees(*lon_target);
}

தசம get_bearing_to_next_waypoint(இரட்டை lat_now, இரட்டை lon_now, இரட்டை lat_next, இரட்டை lon_next)
{
	மாறிலி இரட்டை lat_now_rad = math::radians(lat_now);
	மாறிலி இரட்டை lat_next_rad = math::radians(lat_next);

	மாறிலி இரட்டை cos_lat_next = cos(lat_next_rad);
	மாறிலி இரட்டை d_lon = math::radians(lon_next - lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */

	மாறிலி தசம y = static_cast<தசம>(sin(d_lon) * cos_lat_next);
	மாறிலி தசம x = static_cast<தசம>(cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos_lat_next * cos(d_lon));

	திரும்பு wrap_pi(atan2f(y, x));
}

வெறுமை
get_vector_to_next_waypoint(இரட்டை lat_now, இரட்டை lon_now, இரட்டை lat_next, இரட்டை lon_next, தசம *v_n, தசம *v_e)
{
	மாறிலி இரட்டை lat_now_rad = math::radians(lat_now);
	மாறிலி இரட்டை lat_next_rad = math::radians(lat_next);
	மாறிலி இரட்டை d_lon = math::radians(lon_next) - math::radians(lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<தசம>(CONSTANTS_RADIUS_OF_EARTH * (cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos(
					  lat_next_rad) * cos(d_lon)));
	*v_e = static_cast<தசம>(CONSTANTS_RADIUS_OF_EARTH * sin(d_lon) * cos(lat_next_rad));
}

வெறுமை
get_vector_to_next_waypoint_fast(இரட்டை lat_now, இரட்டை lon_now, இரட்டை lat_next, இரட்டை lon_next, தசம *v_n,
				 தசம *v_e)
{
	இரட்டை lat_now_rad = math::radians(lat_now);
	இரட்டை lon_now_rad = math::radians(lon_now);
	இரட்டை lat_next_rad = math::radians(lat_next);
	இரட்டை lon_next_rad = math::radians(lon_next);

	இரட்டை d_lat = lat_next_rad - lat_now_rad;
	இரட்டை d_lon = lon_next_rad - lon_now_rad;

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<தசம>(CONSTANTS_RADIUS_OF_EARTH * d_lat);
	*v_e = static_cast<தசம>(CONSTANTS_RADIUS_OF_EARTH * d_lon * cos(lat_now_rad));
}

வெறுமை add_vector_to_global_position(இரட்டை lat_now, இரட்டை lon_now, தசம v_n, தசம v_e, இரட்டை *lat_res,
				   இரட்டை *lon_res)
{
	இரட்டை lat_now_rad = math::radians(lat_now);
	இரட்டை lon_now_rad = math::radians(lon_now);

	*lat_res = math::degrees(lat_now_rad + (இரட்டை)v_n / CONSTANTS_RADIUS_OF_EARTH);
	*lon_res = math::degrees(lon_now_rad + (இரட்டை)v_e / (CONSTANTS_RADIUS_OF_EARTH * cos(lat_now_rad)));
}

// Additional functions - @author Doug Weibel <douglas.weibel@colorado.edu>

முழு get_distance_to_வரி(கட்டமைப்பு crosstrack_error_s &crosstrack_error, இரட்டை lat_now, இரட்டை lon_now,
			 இரட்டை lat_start, இரட்டை lon_start, இரட்டை lat_end, இரட்டை lon_end)
{
	// This function returns the distance to the nearest point on the track line.  Distance is positive if current
	// position is right of the track and negative if left of the track as seen from a point on the track line
	// headed towards the end point.

	முழு return_மதிப்பு = -1;	// Set error flag, cleared when valid result calculated.
	crosstrack_error.past_end = பொய்;
	crosstrack_error.distance = 0.0f;
	crosstrack_error.bearing = 0.0f;

	தசம dist_to_end = get_distance_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);

	// Return error if arguments are bad
	எனில் (dist_to_end < 0.1f) {
		திரும்பு -1;
	}

	தசம bearing_end = get_bearing_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);
	தசம bearing_track = get_bearing_to_next_waypoint(lat_start, lon_start, lat_end, lon_end);
	தசம bearing_diff = wrap_pi(bearing_track - bearing_end);

	// Return past_end = true if past end point of line
	எனில் (bearing_diff > M_PI_2_F || bearing_diff < -M_PI_2_F) {
		crosstrack_error.past_end = உண்மை;
		return_மதிப்பு = 0;
		திரும்பு return_மதிப்பு;
	}

	crosstrack_error.distance = (dist_to_end) * sinf(bearing_diff);

	எனில் (sinf(bearing_diff) >= 0) {
		crosstrack_error.bearing = wrap_pi(bearing_track - M_PI_2_F);

	} இல்லையெனில் {
		crosstrack_error.bearing = wrap_pi(bearing_track + M_PI_2_F);
	}

	return_மதிப்பு = 0;

	திரும்பு return_மதிப்பு;
}

முழு get_distance_to_arc(கட்டமைப்பு crosstrack_error_s *crosstrack_error, இரட்டை lat_now, இரட்டை lon_now,
			இரட்டை lat_center, இரட்டை lon_center,
			தசம radius, தசம arc_start_bearing, தசம arc_sweep)
{
	// This function returns the distance to the nearest point on the track arc.  Distance is positive if current
	// position is right of the arc and negative if left of the arc as seen from the closest point on the arc and
	// headed towards the end point.

	// Determine if the current position is inside or outside the sector between the line from the center
	// to the arc start and the line from the center to the arc end
	மாறிலி தசம bearing_now = get_bearing_to_next_waypoint(lat_now, lon_now, lat_center, lon_center);

	முழு return_மதிப்பு = -1;		// Set error flag, cleared when valid result calculated.
	crosstrack_error->past_end = பொய்;
	crosstrack_error->distance = 0.0f;
	crosstrack_error->bearing = 0.0f;

	// Return error if arguments are bad
	எனில் (radius < 0.1f) {
		திரும்பு return_மதிப்பு;
	}

	// arc_start_bearing and arc_sweep are referenced to the center, so test membership with the
	// center->vehicle bearing. Measuring it in the direction of the sweep handles either sign and
	// any zero crossing without special cases.
	// NOLINTNEXTLINE(readability-suspicious-call-argument) intentional reverse bearing: center -> vehicle
	மாறிலி தசம bearing_center_to_now = get_bearing_to_next_waypoint(lat_center, lon_center, lat_now, lon_now);
	மாறிலி தசம bearing_from_start = wrap_2pi((bearing_center_to_now - arc_start_bearing) * (arc_sweep < 0.0f ? -1.0f :
					 1.0f));
	மாறிலி தருக்க in_sector = bearing_from_start <= fabsf(arc_sweep);

	// If in the sector then calculate distance and bearing to closest point
	எனில் (in_sector) {
		crosstrack_error->past_end = பொய்;
		தசம dist_to_center = get_distance_to_next_waypoint(lat_now, lon_now, lat_center, lon_center);

		எனில் (dist_to_center <= radius) {
			crosstrack_error->distance = radius - dist_to_center;
			crosstrack_error->bearing = bearing_now + M_PI_F;

		} இல்லையெனில் {
			crosstrack_error->distance = dist_to_center - radius;
			crosstrack_error->bearing = bearing_now;
		}

		// If out of the sector then calculate dist and bearing to start or end point

	} இல்லையெனில் {

		// Use the approximation  that 111,111 meters in the y direction is 1 degree (of latitude)
		// and 111,111 * cos(latitude) meters in the x direction is 1 degree (of longitude) to
		// calculate the position of the start and end points.  We should not be doing this often
		// as this function generally will not be called repeatedly when we are out of the sector.

		இரட்டை start_disp_x = (இரட்டை)radius * sin((இரட்டை)arc_start_bearing);
		இரட்டை start_disp_y = (இரட்டை)radius * cos((இரட்டை)arc_start_bearing);
		இரட்டை end_disp_x = (இரட்டை)radius * sin((இரட்டை)wrap_pi(arc_start_bearing + arc_sweep));
		இரட்டை end_disp_y = (இரட்டை)radius * cos((இரட்டை)wrap_pi(arc_start_bearing + arc_sweep));
		மாறிலி இரட்டை cos_lat_center = cos(math::radians(lat_center));
		இரட்டை lon_start = lon_center + start_disp_x / (111111.0 * cos_lat_center);
		இரட்டை lat_start = lat_center + start_disp_y / 111111.0;
		இரட்டை lon_end = lon_center + end_disp_x / (111111.0 * cos_lat_center);
		இரட்டை lat_end = lat_center + end_disp_y / 111111.0;
		தசம dist_to_start = get_distance_to_next_waypoint(lat_now, lon_now, lat_start, lon_start);
		தசம dist_to_end = get_distance_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);

		எனில் (dist_to_start < dist_to_end) {
			crosstrack_error->distance = dist_to_start;
			crosstrack_error->bearing = get_bearing_to_next_waypoint(lat_now, lon_now, lat_start, lon_start);

		} இல்லையெனில் {
			crosstrack_error->past_end = உண்மை;
			crosstrack_error->distance = dist_to_end;
			crosstrack_error->bearing = get_bearing_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);
		}
	}

	crosstrack_error->bearing = wrap_pi(crosstrack_error->bearing);
	return_மதிப்பு = 0;

	திரும்பு return_மதிப்பு;
}

தசம get_distance_to_point_global_wgs84(இரட்டை lat_now, இரட்டை lon_now, தசம alt_now,
		இரட்டை lat_next, இரட்டை lon_next, தசம alt_next,
		தசம *dist_xy, தசம *dist_z)
{
	இரட்டை current_x_rad = math::radians(lat_next);
	இரட்டை current_y_rad = math::radians(lon_next);
	இரட்டை x_rad = math::radians(lat_now);
	இரட்டை y_rad = math::radians(lon_now);

	இரட்டை d_lat = x_rad - current_x_rad;
	இரட்டை d_lon = y_rad - current_y_rad;

	இரட்டை a = sin(d_lat / 2.0) * sin(d_lat / 2.0) + sin(d_lon / 2.0) * sin(d_lon / 2.0) * cos(current_x_rad) * cos(x_rad);
	இரட்டை c = 2 * atan2(sqrt(a), sqrt(1 - a));

	மாறிலி தசம dxy = static_cast<தசம>(CONSTANTS_RADIUS_OF_EARTH * c);
	மாறிலி தசம dz = static_cast<தசம>(alt_now - alt_next);

	*dist_xy = fabsf(dxy);
	*dist_z = fabsf(dz);

	திரும்பு sqrtf(dxy * dxy + dz * dz);
}

தசம mavlink_wpm_distance_to_point_local(தசம x_now, தசம y_now, தசம z_now,
		தசம x_next, தசம y_next, தசம z_next,
		தசம *dist_xy, தசம *dist_z)
{
	தசம dx = x_now - x_next;
	தசம dy = y_now - y_next;
	தசம dz = z_now - z_next;

	*dist_xy = sqrtf(dx * dx + dy * dy);
	*dist_z = fabsf(dz);

	திரும்பு sqrtf(dx * dx + dy * dy + dz * dz);
}
