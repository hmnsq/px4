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

#അന്തര്ഭാവ "geo.h"

#അന്തര്ഭാവ <float.h>

using matrix::wrap_pi;
using matrix::wrap_2pi;

/*
 * Azimuthal Equidistant Projection
 * formulas according to: http://mathworld.wolfram.com/AzimuthalEquidistantProjection.html
 */

ശൂന്യം MapProjection::initReference(ഇരട്ട lat_0, ഇരട്ട lon_0, uint64_t timestamp)
{
	_ref_timestamp = timestamp;
	_ref_lat = math::radians(lat_0);
	_ref_lon = math::radians(lon_0);
	_ref_sin_lat = sin(_ref_lat);
	_ref_cos_lat = cos(_ref_lat);
	_ref_init_done = സത്യം;
}

ശൂന്യം MapProjection::project(ഇരട്ട lat, ഇരട്ട lon, ദശാംശം &x, ദശാംശം &y) സ്ഥിരം
{
	സ്ഥിരം ഇരട്ട lat_rad = math::radians(lat);
	സ്ഥിരം ഇരട്ട lon_rad = math::radians(lon);

	സ്ഥിരം ഇരട്ട sin_lat = sin(lat_rad);
	സ്ഥിരം ഇരട്ട cos_lat = cos(lat_rad);

	സ്ഥിരം ഇരട്ട cos_d_lon = cos(lon_rad - _ref_lon);

	സ്ഥിരം ഇരട്ട arg = math::constrain(_ref_sin_lat * sin_lat + _ref_cos_lat * cos_lat * cos_d_lon, -1.0,  1.0);
	സ്ഥിരം ഇരട്ട c = acos(arg);

	ഇരട്ട k = 1.0;

	എങ്കിൽ (fabs(c) > 0) {
		k = (c / sin(c));
	}

	x = static_cast<ദശാംശം>(k * (_ref_cos_lat * sin_lat - _ref_sin_lat * cos_lat * cos_d_lon) * CONSTANTS_RADIUS_OF_EARTH);
	y = static_cast<ദശാംശം>(k * cos_lat * sin(lon_rad - _ref_lon) * CONSTANTS_RADIUS_OF_EARTH);
}

ശൂന്യം MapProjection::reproject(ദശാംശം x, ദശാംശം y, ഇരട്ട &lat, ഇരട്ട &lon) സ്ഥിരം
{
	സ്ഥിരം ഇരട്ട x_rad = (ഇരട്ട)x / CONSTANTS_RADIUS_OF_EARTH;
	സ്ഥിരം ഇരട്ട y_rad = (ഇരട്ട)y / CONSTANTS_RADIUS_OF_EARTH;
	സ്ഥിരം ഇരട്ട c = sqrt(x_rad * x_rad + y_rad * y_rad);

	എങ്കിൽ (fabs(c) > 0) {
		സ്ഥിരം ഇരട്ട sin_c = sin(c);
		സ്ഥിരം ഇരട്ട cos_c = cos(c);

		സ്ഥിരം ഇരട്ട lat_rad = asin(cos_c * _ref_sin_lat + (x_rad * sin_c * _ref_cos_lat) / c);
		സ്ഥിരം ഇരട്ട lon_rad = (_ref_lon + atan2(y_rad * sin_c, c * _ref_cos_lat * cos_c - x_rad * _ref_sin_lat * sin_c));

		lat = math::degrees(lat_rad);
		lon = math::degrees(lon_rad);

	} അല്ലെങ്കിൽ {
		lat = math::degrees(_ref_lat);
		lon = math::degrees(_ref_lon);
	}
}

ദശാംശം get_distance_to_next_waypoint(ഇരട്ട lat_now, ഇരട്ട lon_now, ഇരട്ട lat_next, ഇരട്ട lon_next)
{
	സ്ഥിരം ഇരട്ട lat_now_rad = math::radians(lat_now);
	സ്ഥിരം ഇരട്ട lat_next_rad = math::radians(lat_next);

	സ്ഥിരം ഇരട്ട d_lat = lat_next_rad - lat_now_rad;
	സ്ഥിരം ഇരട്ട d_lon = math::radians(lon_next) - math::radians(lon_now);

	സ്ഥിരം ഇരട്ട a = sin(d_lat / 2.0) * sin(d_lat / 2.0) + sin(d_lon / 2.0) * sin(d_lon / 2.0) * cos(lat_now_rad) * cos(
				 lat_next_rad);

	സ്ഥിരം ഇരട്ട c = atan2(sqrt(a), sqrt(1.0 - a));

	തിരികെ static_cast<ദശാംശം>(CONSTANTS_RADIUS_OF_EARTH * 2.0 * c);
}

ശൂന്യം create_waypoint_from_വരി_and_dist(ഇരട്ട lat_A, ഇരട്ട lon_A, ഇരട്ട lat_B, ഇരട്ട lon_B, ദശാംശം dist,
					ഇരട്ട *lat_target, ഇരട്ട *lon_target)
{
	എങ്കിൽ (fabsf(dist) < FLT_EPSILON) {
		*lat_target = lat_A;
		*lon_target = lon_A;

	} അല്ലെങ്കിൽ {
		ദശാംശം heading = get_bearing_to_next_waypoint(lat_A, lon_A, lat_B, lon_B);
		waypoint_from_heading_and_distance(lat_A, lon_A, heading, dist, lat_target, lon_target);
	}
}

ശൂന്യം waypoint_from_heading_and_distance(ഇരട്ട lat_start, ഇരട്ട lon_start, ദശാംശം bearing, ദശാംശം dist,
					ഇരട്ട *lat_target, ഇരട്ട *lon_target)
{
	bearing = wrap_2pi(bearing);

	ഇരട്ട radius_ratio = static_cast<ഇരട്ട>(dist) / CONSTANTS_RADIUS_OF_EARTH;

	ഇരട്ട lat_start_rad = math::radians(lat_start);
	ഇരട്ട lon_start_rad = math::radians(lon_start);

	*lat_target = asin(sin(lat_start_rad) * cos(radius_ratio) + cos(lat_start_rad) * sin(radius_ratio) * cos((
				   ഇരട്ട)bearing));
	*lon_target = lon_start_rad + atan2(sin((ഇരട്ട)bearing) * sin(radius_ratio) * cos(lat_start_rad),
					    cos(radius_ratio) - sin(lat_start_rad) * sin(*lat_target));

	*lat_target = math::degrees(*lat_target);
	*lon_target = math::degrees(*lon_target);
}

ദശാംശം get_bearing_to_next_waypoint(ഇരട്ട lat_now, ഇരട്ട lon_now, ഇരട്ട lat_next, ഇരട്ട lon_next)
{
	സ്ഥിരം ഇരട്ട lat_now_rad = math::radians(lat_now);
	സ്ഥിരം ഇരട്ട lat_next_rad = math::radians(lat_next);

	സ്ഥിരം ഇരട്ട cos_lat_next = cos(lat_next_rad);
	സ്ഥിരം ഇരട്ട d_lon = math::radians(lon_next - lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */

	സ്ഥിരം ദശാംശം y = static_cast<ദശാംശം>(sin(d_lon) * cos_lat_next);
	സ്ഥിരം ദശാംശം x = static_cast<ദശാംശം>(cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos_lat_next * cos(d_lon));

	തിരികെ wrap_pi(atan2f(y, x));
}

ശൂന്യം
get_vector_to_next_waypoint(ഇരട്ട lat_now, ഇരട്ട lon_now, ഇരട്ട lat_next, ഇരട്ട lon_next, ദശാംശം *v_n, ദശാംശം *v_e)
{
	സ്ഥിരം ഇരട്ട lat_now_rad = math::radians(lat_now);
	സ്ഥിരം ഇരട്ട lat_next_rad = math::radians(lat_next);
	സ്ഥിരം ഇരട്ട d_lon = math::radians(lon_next) - math::radians(lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<ദശാംശം>(CONSTANTS_RADIUS_OF_EARTH * (cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos(
					  lat_next_rad) * cos(d_lon)));
	*v_e = static_cast<ദശാംശം>(CONSTANTS_RADIUS_OF_EARTH * sin(d_lon) * cos(lat_next_rad));
}

ശൂന്യം
get_vector_to_next_waypoint_fast(ഇരട്ട lat_now, ഇരട്ട lon_now, ഇരട്ട lat_next, ഇരട്ട lon_next, ദശാംശം *v_n,
				 ദശാംശം *v_e)
{
	ഇരട്ട lat_now_rad = math::radians(lat_now);
	ഇരട്ട lon_now_rad = math::radians(lon_now);
	ഇരട്ട lat_next_rad = math::radians(lat_next);
	ഇരട്ട lon_next_rad = math::radians(lon_next);

	ഇരട്ട d_lat = lat_next_rad - lat_now_rad;
	ഇരട്ട d_lon = lon_next_rad - lon_now_rad;

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<ദശാംശം>(CONSTANTS_RADIUS_OF_EARTH * d_lat);
	*v_e = static_cast<ദശാംശം>(CONSTANTS_RADIUS_OF_EARTH * d_lon * cos(lat_now_rad));
}

ശൂന്യം add_vector_to_global_position(ഇരട്ട lat_now, ഇരട്ട lon_now, ദശാംശം v_n, ദശാംശം v_e, ഇരട്ട *lat_res,
				   ഇരട്ട *lon_res)
{
	ഇരട്ട lat_now_rad = math::radians(lat_now);
	ഇരട്ട lon_now_rad = math::radians(lon_now);

	*lat_res = math::degrees(lat_now_rad + (ഇരട്ട)v_n / CONSTANTS_RADIUS_OF_EARTH);
	*lon_res = math::degrees(lon_now_rad + (ഇരട്ട)v_e / (CONSTANTS_RADIUS_OF_EARTH * cos(lat_now_rad)));
}

// Additional functions - @author Doug Weibel <douglas.weibel@colorado.edu>

പൂർണ്ണസംഖ്യ get_distance_to_വരി(ഘടന crosstrack_error_s &crosstrack_error, ഇരട്ട lat_now, ഇരട്ട lon_now,
			 ഇരട്ട lat_start, ഇരട്ട lon_start, ഇരട്ട lat_end, ഇരട്ട lon_end)
{
	// This function returns the distance to the nearest point on the track line.  Distance is positive if current
	// position is right of the track and negative if left of the track as seen from a point on the track line
	// headed towards the end point.

	പൂർണ്ണസംഖ്യ return_മൂല്യം = -1;	// Set error flag, cleared when valid result calculated.
	crosstrack_error.past_end = അസത്യം;
	crosstrack_error.distance = 0.0f;
	crosstrack_error.bearing = 0.0f;

	ദശാംശം dist_to_end = get_distance_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);

	// Return error if arguments are bad
	എങ്കിൽ (dist_to_end < 0.1f) {
		തിരികെ -1;
	}

	ദശാംശം bearing_end = get_bearing_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);
	ദശാംശം bearing_track = get_bearing_to_next_waypoint(lat_start, lon_start, lat_end, lon_end);
	ദശാംശം bearing_diff = wrap_pi(bearing_track - bearing_end);

	// Return past_end = true if past end point of line
	എങ്കിൽ (bearing_diff > M_PI_2_F || bearing_diff < -M_PI_2_F) {
		crosstrack_error.past_end = സത്യം;
		return_മൂല്യം = 0;
		തിരികെ return_മൂല്യം;
	}

	crosstrack_error.distance = (dist_to_end) * sinf(bearing_diff);

	എങ്കിൽ (sinf(bearing_diff) >= 0) {
		crosstrack_error.bearing = wrap_pi(bearing_track - M_PI_2_F);

	} അല്ലെങ്കിൽ {
		crosstrack_error.bearing = wrap_pi(bearing_track + M_PI_2_F);
	}

	return_മൂല്യം = 0;

	തിരികെ return_മൂല്യം;
}

പൂർണ്ണസംഖ്യ get_distance_to_arc(ഘടന crosstrack_error_s *crosstrack_error, ഇരട്ട lat_now, ഇരട്ട lon_now,
			ഇരട്ട lat_center, ഇരട്ട lon_center,
			ദശാംശം radius, ദശാംശം arc_start_bearing, ദശാംശം arc_sweep)
{
	// This function returns the distance to the nearest point on the track arc.  Distance is positive if current
	// position is right of the arc and negative if left of the arc as seen from the closest point on the arc and
	// headed towards the end point.

	// Determine if the current position is inside or outside the sector between the line from the center
	// to the arc start and the line from the center to the arc end
	സ്ഥിരം ദശാംശം bearing_now = get_bearing_to_next_waypoint(lat_now, lon_now, lat_center, lon_center);

	പൂർണ്ണസംഖ്യ return_മൂല്യം = -1;		// Set error flag, cleared when valid result calculated.
	crosstrack_error->past_end = അസത്യം;
	crosstrack_error->distance = 0.0f;
	crosstrack_error->bearing = 0.0f;

	// Return error if arguments are bad
	എങ്കിൽ (radius < 0.1f) {
		തിരികെ return_മൂല്യം;
	}

	// arc_start_bearing and arc_sweep are referenced to the center, so test membership with the
	// center->vehicle bearing. Measuring it in the direction of the sweep handles either sign and
	// any zero crossing without special cases.
	// NOLINTNEXTLINE(readability-suspicious-call-argument) intentional reverse bearing: center -> vehicle
	സ്ഥിരം ദശാംശം bearing_center_to_now = get_bearing_to_next_waypoint(lat_center, lon_center, lat_now, lon_now);
	സ്ഥിരം ദശാംശം bearing_from_start = wrap_2pi((bearing_center_to_now - arc_start_bearing) * (arc_sweep < 0.0f ? -1.0f :
					 1.0f));
	സ്ഥിരം തർക്കം in_sector = bearing_from_start <= fabsf(arc_sweep);

	// If in the sector then calculate distance and bearing to closest point
	എങ്കിൽ (in_sector) {
		crosstrack_error->past_end = അസത്യം;
		ദശാംശം dist_to_center = get_distance_to_next_waypoint(lat_now, lon_now, lat_center, lon_center);

		എങ്കിൽ (dist_to_center <= radius) {
			crosstrack_error->distance = radius - dist_to_center;
			crosstrack_error->bearing = bearing_now + M_PI_F;

		} അല്ലെങ്കിൽ {
			crosstrack_error->distance = dist_to_center - radius;
			crosstrack_error->bearing = bearing_now;
		}

		// If out of the sector then calculate dist and bearing to start or end point

	} അല്ലെങ്കിൽ {

		// Use the approximation  that 111,111 meters in the y direction is 1 degree (of latitude)
		// and 111,111 * cos(latitude) meters in the x direction is 1 degree (of longitude) to
		// calculate the position of the start and end points.  We should not be doing this often
		// as this function generally will not be called repeatedly when we are out of the sector.

		ഇരട്ട start_disp_x = (ഇരട്ട)radius * sin((ഇരട്ട)arc_start_bearing);
		ഇരട്ട start_disp_y = (ഇരട്ട)radius * cos((ഇരട്ട)arc_start_bearing);
		ഇരട്ട end_disp_x = (ഇരട്ട)radius * sin((ഇരട്ട)wrap_pi(arc_start_bearing + arc_sweep));
		ഇരട്ട end_disp_y = (ഇരട്ട)radius * cos((ഇരട്ട)wrap_pi(arc_start_bearing + arc_sweep));
		സ്ഥിരം ഇരട്ട cos_lat_center = cos(math::radians(lat_center));
		ഇരട്ട lon_start = lon_center + start_disp_x / (111111.0 * cos_lat_center);
		ഇരട്ട lat_start = lat_center + start_disp_y / 111111.0;
		ഇരട്ട lon_end = lon_center + end_disp_x / (111111.0 * cos_lat_center);
		ഇരട്ട lat_end = lat_center + end_disp_y / 111111.0;
		ദശാംശം dist_to_start = get_distance_to_next_waypoint(lat_now, lon_now, lat_start, lon_start);
		ദശാംശം dist_to_end = get_distance_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);

		എങ്കിൽ (dist_to_start < dist_to_end) {
			crosstrack_error->distance = dist_to_start;
			crosstrack_error->bearing = get_bearing_to_next_waypoint(lat_now, lon_now, lat_start, lon_start);

		} അല്ലെങ്കിൽ {
			crosstrack_error->past_end = സത്യം;
			crosstrack_error->distance = dist_to_end;
			crosstrack_error->bearing = get_bearing_to_next_waypoint(lat_now, lon_now, lat_end, lon_end);
		}
	}

	crosstrack_error->bearing = wrap_pi(crosstrack_error->bearing);
	return_മൂല്യം = 0;

	തിരികെ return_മൂല്യം;
}

ദശാംശം get_distance_to_point_global_wgs84(ഇരട്ട lat_now, ഇരട്ട lon_now, ദശാംശം alt_now,
		ഇരട്ട lat_next, ഇരട്ട lon_next, ദശാംശം alt_next,
		ദശാംശം *dist_xy, ദശാംശം *dist_z)
{
	ഇരട്ട current_x_rad = math::radians(lat_next);
	ഇരട്ട current_y_rad = math::radians(lon_next);
	ഇരട്ട x_rad = math::radians(lat_now);
	ഇരട്ട y_rad = math::radians(lon_now);

	ഇരട്ട d_lat = x_rad - current_x_rad;
	ഇരട്ട d_lon = y_rad - current_y_rad;

	ഇരട്ട a = sin(d_lat / 2.0) * sin(d_lat / 2.0) + sin(d_lon / 2.0) * sin(d_lon / 2.0) * cos(current_x_rad) * cos(x_rad);
	ഇരട്ട c = 2 * atan2(sqrt(a), sqrt(1 - a));

	സ്ഥിരം ദശാംശം dxy = static_cast<ദശാംശം>(CONSTANTS_RADIUS_OF_EARTH * c);
	സ്ഥിരം ദശാംശം dz = static_cast<ദശാംശം>(alt_now - alt_next);

	*dist_xy = fabsf(dxy);
	*dist_z = fabsf(dz);

	തിരികെ sqrtf(dxy * dxy + dz * dz);
}

ദശാംശം mavlink_wpm_distance_to_point_local(ദശാംശം x_now, ദശാംശം y_now, ദശാംശം z_now,
		ദശാംശം x_next, ദശാംശം y_next, ദശാംശം z_next,
		ദശാംശം *dist_xy, ദശാംശം *dist_z)
{
	ദശാംശം dx = x_now - x_next;
	ദശാംശം dy = y_now - y_next;
	ദശാംശം dz = z_now - z_next;

	*dist_xy = sqrtf(dx * dx + dy * dy);
	*dist_z = fabsf(dz);

	തിരികെ sqrtf(dx * dx + dy * dy + dz * dz);
}
