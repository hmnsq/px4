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

#अन्तर्भाव "geo.h"

#अन्तर्भाव <float.h>

उपयुज्य matrix::wrap_pi;
उपयुज्य matrix::wrap_2pi;

/*
 * Azimuthal Equidistant Projection
 * formulas according to: http://mathworld.wolfram.com/AzimuthalEquidistantProjection.html
 */

शून्य MapProjection::initReference(द्विगुण lat_0, द्विगुण lon_0, uint64_t timestamp)
{
	_ref_timestamp = timestamp;
	_ref_lat = math::radians(lat_0);
	_ref_lon = math::radians(lon_0);
	_ref_sin_lat = ज्या(_ref_lat);
	_ref_cos_lat = कोटिज्या(_ref_lat);
	ref_init_समाप्तम् = सत्य;
}

शून्य MapProjection::project(द्विगुण lat, द्विगुण lon, भिन्न &x, भिन्न &y) ध्रुव
{
	ध्रुव द्विगुण lat_rad = math::radians(lat);
	ध्रुव द्विगुण lon_rad = math::radians(lon);

	ध्रुव द्विगुण sin_lat = ज्या(lat_rad);
	ध्रुव द्विगुण cos_lat = कोटिज्या(lat_rad);

	ध्रुव द्विगुण cos_d_lon = कोटिज्या(lon_rad - _ref_lon);

	ध्रुव द्विगुण arg = math::constrain(_ref_sin_lat * sin_lat + _ref_cos_lat * cos_lat * cos_d_lon, -1.0,  1.0);
	ध्रुव द्विगुण c = acos(arg);

	द्विगुण k = 1.0;

	यदि (fabs(c) > 0) {
		k = (c / ज्या(c));
	}

	x = static_cast<भिन्न>(k * (_ref_cos_lat * sin_lat - _ref_sin_lat * cos_lat * cos_d_lon) * CONSTANTS_RADIUS_सम्बन्धि_EARTH);
	y = static_cast<भिन्न>(k * cos_lat * ज्या(lon_rad - _ref_lon) * CONSTANTS_RADIUS_सम्बन्धि_EARTH);
}

शून्य MapProjection::reproject(भिन्न x, भिन्न y, द्विगुण &lat, द्विगुण &lon) ध्रुव
{
	ध्रुव द्विगुण x_rad = (द्विगुण)x / CONSTANTS_RADIUS_सम्बन्धि_EARTH;
	ध्रुव द्विगुण y_rad = (द्विगुण)y / CONSTANTS_RADIUS_सम्बन्धि_EARTH;
	ध्रुव द्विगुण c = वर्गमूलम्(x_rad * x_rad + y_rad * y_rad);

	यदि (fabs(c) > 0) {
		ध्रुव द्विगुण sin_c = ज्या(c);
		ध्रुव द्विगुण cos_c = कोटिज्या(c);

		ध्रुव द्विगुण lat_rad = asin(cos_c * _ref_sin_lat + (x_rad * sin_c * _ref_cos_lat) / c);
		ध्रुव द्विगुण lon_rad = (_ref_lon + atan2(y_rad * sin_c, c * _ref_cos_lat * cos_c - x_rad * _ref_sin_lat * sin_c));

		lat = math::degrees(lat_rad);
		lon = math::degrees(lon_rad);

	} अन्यथा {
		lat = math::degrees(_ref_lat);
		lon = math::degrees(_ref_lon);
	}
}

भिन्न get_distance_गन्तव्यम्_अग्र_waypoint(द्विगुण lat_now, द्विगुण lon_now, द्विगुण lat_अग्र, द्विगुण lon_अग्र)
{
	ध्रुव द्विगुण lat_now_rad = math::radians(lat_now);
	ध्रुव द्विगुण lat_अग्र_rad = math::radians(lat_अग्र);

	ध्रुव द्विगुण d_lat = lat_अग्र_rad - lat_now_rad;
	ध्रुव द्विगुण d_lon = math::radians(lon_अग्र) - math::radians(lon_now);

	ध्रुव द्विगुण a = ज्या(d_lat / 2.0) * ज्या(d_lat / 2.0) + ज्या(d_lon / 2.0) * ज्या(d_lon / 2.0) * कोटिज्या(lat_now_rad) * कोटिज्या(
				 lat_अग्र_rad);

	ध्रुव द्विगुण c = atan2(वर्गमूलम्(a), वर्गमूलम्(1.0 - a));

	प्रत्यागम static_cast<भिन्न>(CONSTANTS_RADIUS_सम्बन्धि_EARTH * 2.0 * c);
}

शून्य create_waypoint_from_पङ्क्तिः_and_dist(द्विगुण lat_A, द्विगुण lon_A, द्विगुण lat_B, द्विगुण lon_B, भिन्न dist,
					द्विगुण *lat_target, द्विगुण *lon_target)
{
	यदि (fabsf(dist) < FLT_EPSILON) {
		*lat_target = lat_A;
		*lon_target = lon_A;

	} अन्यथा {
		भिन्न heading = get_bearing_गन्तव्यम्_अग्र_waypoint(lat_A, lon_A, lat_B, lon_B);
		waypoint_from_heading_and_distance(lat_A, lon_A, heading, dist, lat_target, lon_target);
	}
}

शून्य waypoint_from_heading_and_distance(द्विगुण lat_start, द्विगुण lon_start, भिन्न bearing, भिन्न dist,
					द्विगुण *lat_target, द्विगुण *lon_target)
{
	bearing = wrap_2pi(bearing);

	द्विगुण radius_अनुपातः = static_cast<द्विगुण>(dist) / CONSTANTS_RADIUS_सम्बन्धि_EARTH;

	द्विगुण lat_start_rad = math::radians(lat_start);
	द्विगुण lon_start_rad = math::radians(lon_start);

	*lat_target = asin(ज्या(lat_start_rad) * कोटिज्या(radius_अनुपातः) + कोटिज्या(lat_start_rad) * ज्या(radius_अनुपातः) * कोटिज्या((
				   द्विगुण)bearing));
	*lon_target = lon_start_rad + atan2(ज्या((द्विगुण)bearing) * ज्या(radius_अनुपातः) * कोटिज्या(lat_start_rad),
					    कोटिज्या(radius_अनुपातः) - ज्या(lat_start_rad) * ज्या(*lat_target));

	*lat_target = math::degrees(*lat_target);
	*lon_target = math::degrees(*lon_target);
}

भिन्न get_bearing_गन्तव्यम्_अग्र_waypoint(द्विगुण lat_now, द्विगुण lon_now, द्विगुण lat_अग्र, द्विगुण lon_अग्र)
{
	ध्रुव द्विगुण lat_now_rad = math::radians(lat_now);
	ध्रुव द्विगुण lat_अग्र_rad = math::radians(lat_अग्र);

	ध्रुव द्विगुण cos_lat_अग्र = कोटिज्या(lat_अग्र_rad);
	ध्रुव द्विगुण d_lon = math::radians(lon_अग्र - lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */

	ध्रुव भिन्न y = static_cast<भिन्न>(ज्या(d_lon) * cos_lat_अग्र);
	ध्रुव भिन्न x = static_cast<भिन्न>(कोटिज्या(lat_now_rad) * ज्या(lat_अग्र_rad) - ज्या(lat_now_rad) * cos_lat_अग्र * कोटिज्या(d_lon));

	प्रत्यागम wrap_pi(atan2f(y, x));
}

शून्य
get_सदिशम्_गन्तव्यम्_अग्र_waypoint(द्विगुण lat_now, द्विगुण lon_now, द्विगुण lat_अग्र, द्विगुण lon_अग्र, भिन्न *v_n, भिन्न *v_e)
{
	ध्रुव द्विगुण lat_now_rad = math::radians(lat_now);
	ध्रुव द्विगुण lat_अग्र_rad = math::radians(lat_अग्र);
	ध्रुव द्विगुण d_lon = math::radians(lon_अग्र) - math::radians(lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<भिन्न>(CONSTANTS_RADIUS_सम्बन्धि_EARTH * (कोटिज्या(lat_now_rad) * ज्या(lat_अग्र_rad) - ज्या(lat_now_rad) * कोटिज्या(
					  lat_अग्र_rad) * कोटिज्या(d_lon)));
	*v_e = static_cast<भिन्न>(CONSTANTS_RADIUS_सम्बन्धि_EARTH * ज्या(d_lon) * कोटिज्या(lat_अग्र_rad));
}

शून्य
get_सदिशम्_गन्तव्यम्_अग्र_waypoint_fast(द्विगुण lat_now, द्विगुण lon_now, द्विगुण lat_अग्र, द्विगुण lon_अग्र, भिन्न *v_n,
				 भिन्न *v_e)
{
	द्विगुण lat_now_rad = math::radians(lat_now);
	द्विगुण lon_now_rad = math::radians(lon_now);
	द्विगुण lat_अग्र_rad = math::radians(lat_अग्र);
	द्विगुण lon_अग्र_rad = math::radians(lon_अग्र);

	द्विगुण d_lat = lat_अग्र_rad - lat_now_rad;
	द्विगुण d_lon = lon_अग्र_rad - lon_now_rad;

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<भिन्न>(CONSTANTS_RADIUS_सम्बन्धि_EARTH * d_lat);
	*v_e = static_cast<भिन्न>(CONSTANTS_RADIUS_सम्बन्धि_EARTH * d_lon * कोटिज्या(lat_now_rad));
}

शून्य सङ्कलय_सदिशम्_गन्तव्यम्_global_position(द्विगुण lat_now, द्विगुण lon_now, भिन्न v_n, भिन्न v_e, द्विगुण *lat_res,
				   द्विगुण *lon_res)
{
	द्विगुण lat_now_rad = math::radians(lat_now);
	द्विगुण lon_now_rad = math::radians(lon_now);

	*lat_res = math::degrees(lat_now_rad + (द्विगुण)v_n / CONSTANTS_RADIUS_सम्बन्धि_EARTH);
	*lon_res = math::degrees(lon_now_rad + (द्विगुण)v_e / (CONSTANTS_RADIUS_सम्बन्धि_EARTH * कोटिज्या(lat_now_rad)));
}

// Additional functions - @author Doug Weibel <douglas.weibel@colorado.edu>

पूर्णाङ्क get_distance_गन्तव्यम्_पङ्क्तिः(संरचना crosstrack_error_s &crosstrack_error, द्विगुण lat_now, द्विगुण lon_now,
			 द्विगुण lat_start, द्विगुण lon_start, द्विगुण lat_अन्तः, द्विगुण lon_अन्तः)
{
	// This function returns the distance to the nearest point on the track line.  Distance is positive if current
	// position is right of the track and negative if left of the track as seen from a point on the track line
	// headed towards the end point.

	पूर्णाङ्क return_मानम् = -1;	// Set error flag, cleared when valid result calculated.
	crosstrack_error.past_अन्तः = असत्य;
	crosstrack_error.distance = 0.0f;
	crosstrack_error.bearing = 0.0f;

	भिन्न dist_गन्तव्यम्_अन्तः = get_distance_गन्तव्यम्_अग्र_waypoint(lat_now, lon_now, lat_अन्तः, lon_अन्तः);

	// Return error if arguments are bad
	यदि (dist_गन्तव्यम्_अन्तः < 0.1f) {
		प्रत्यागम -1;
	}

	भिन्न bearing_अन्तः = get_bearing_गन्तव्यम्_अग्र_waypoint(lat_now, lon_now, lat_अन्तः, lon_अन्तः);
	भिन्न bearing_track = get_bearing_गन्तव्यम्_अग्र_waypoint(lat_start, lon_start, lat_अन्तः, lon_अन्तः);
	भिन्न bearing_diff = wrap_pi(bearing_track - bearing_अन्तः);

	// Return past_end = true if past end point of line
	यदि (bearing_diff > M_PI_2_F || bearing_diff < -M_PI_2_F) {
		crosstrack_error.past_अन्तः = सत्य;
		return_मानम् = 0;
		प्रत्यागम return_मानम्;
	}

	crosstrack_error.distance = (dist_गन्तव्यम्_अन्तः) * sinf(bearing_diff);

	यदि (sinf(bearing_diff) >= 0) {
		crosstrack_error.bearing = wrap_pi(bearing_track - M_PI_2_F);

	} अन्यथा {
		crosstrack_error.bearing = wrap_pi(bearing_track + M_PI_2_F);
	}

	return_मानम् = 0;

	प्रत्यागम return_मानम्;
}

पूर्णाङ्क get_distance_गन्तव्यम्_arc(संरचना crosstrack_error_s *crosstrack_error, द्विगुण lat_now, द्विगुण lon_now,
			द्विगुण lat_center, द्विगुण lon_center,
			भिन्न radius, भिन्न arc_start_bearing, भिन्न arc_sweep)
{
	// This function returns the distance to the nearest point on the track arc.  Distance is positive if current
	// position is right of the arc and negative if left of the arc as seen from the closest point on the arc and
	// headed towards the end point.

	// Determine if the current position is inside or outside the sector between the line from the center
	// to the arc start and the line from the center to the arc end
	ध्रुव भिन्न bearing_now = get_bearing_गन्तव्यम्_अग्र_waypoint(lat_now, lon_now, lat_center, lon_center);

	पूर्णाङ्क return_मानम् = -1;		// Set error flag, cleared when valid result calculated.
	crosstrack_error->past_अन्तः = असत्य;
	crosstrack_error->distance = 0.0f;
	crosstrack_error->bearing = 0.0f;

	// Return error if arguments are bad
	यदि (radius < 0.1f) {
		प्रत्यागम return_मानम्;
	}

	// arc_start_bearing and arc_sweep are referenced to the center, so test membership with the
	// center->vehicle bearing. Measuring it in the direction of the sweep handles either sign and
	// any zero crossing without special cases.
	// NOLINTNEXTLINE(readability-suspicious-call-argument) intentional reverse bearing: center -> vehicle
	ध्रुव भिन्न bearing_center_गन्तव्यम्_now = get_bearing_गन्तव्यम्_अग्र_waypoint(lat_center, lon_center, lat_now, lon_now);
	ध्रुव भिन्न bearing_from_start = wrap_2pi((bearing_center_गन्तव्यम्_now - arc_start_bearing) * (arc_sweep < 0.0f ? -1.0f :
					 1.0f));
	ध्रुव तार्किक अन्तर्_sector = bearing_from_start <= fabsf(arc_sweep);

	// If in the sector then calculate distance and bearing to closest point
	यदि (अन्तर्_sector) {
		crosstrack_error->past_अन्तः = असत्य;
		भिन्न dist_गन्तव्यम्_center = get_distance_गन्तव्यम्_अग्र_waypoint(lat_now, lon_now, lat_center, lon_center);

		यदि (dist_गन्तव्यम्_center <= radius) {
			crosstrack_error->distance = radius - dist_गन्तव्यम्_center;
			crosstrack_error->bearing = bearing_now + M_PI_F;

		} अन्यथा {
			crosstrack_error->distance = dist_गन्तव्यम्_center - radius;
			crosstrack_error->bearing = bearing_now;
		}

		// If out of the sector then calculate dist and bearing to start or end point

	} अन्यथा {

		// Use the approximation  that 111,111 meters in the y direction is 1 degree (of latitude)
		// and 111,111 * cos(latitude) meters in the x direction is 1 degree (of longitude) to
		// calculate the position of the start and end points.  We should not be doing this often
		// as this function generally will not be called repeatedly when we are out of the sector.

		द्विगुण start_disp_x = (द्विगुण)radius * ज्या((द्विगुण)arc_start_bearing);
		द्विगुण start_disp_y = (द्विगुण)radius * कोटिज्या((द्विगुण)arc_start_bearing);
		द्विगुण अन्तः_disp_x = (द्विगुण)radius * ज्या((द्विगुण)wrap_pi(arc_start_bearing + arc_sweep));
		द्विगुण अन्तः_disp_y = (द्विगुण)radius * कोटिज्या((द्विगुण)wrap_pi(arc_start_bearing + arc_sweep));
		ध्रुव द्विगुण cos_lat_center = कोटिज्या(math::radians(lat_center));
		द्विगुण lon_start = lon_center + start_disp_x / (111111.0 * cos_lat_center);
		द्विगुण lat_start = lat_center + start_disp_y / 111111.0;
		द्विगुण lon_अन्तः = lon_center + अन्तः_disp_x / (111111.0 * cos_lat_center);
		द्विगुण lat_अन्तः = lat_center + अन्तः_disp_y / 111111.0;
		भिन्न dist_गन्तव्यम्_start = get_distance_गन्तव्यम्_अग्र_waypoint(lat_now, lon_now, lat_start, lon_start);
		भिन्न dist_गन्तव्यम्_अन्तः = get_distance_गन्तव्यम्_अग्र_waypoint(lat_now, lon_now, lat_अन्तः, lon_अन्तः);

		यदि (dist_गन्तव्यम्_start < dist_गन्तव्यम्_अन्तः) {
			crosstrack_error->distance = dist_गन्तव्यम्_start;
			crosstrack_error->bearing = get_bearing_गन्तव्यम्_अग्र_waypoint(lat_now, lon_now, lat_start, lon_start);

		} अन्यथा {
			crosstrack_error->past_अन्तः = सत्य;
			crosstrack_error->distance = dist_गन्तव्यम्_अन्तः;
			crosstrack_error->bearing = get_bearing_गन्तव्यम्_अग्र_waypoint(lat_now, lon_now, lat_अन्तः, lon_अन्तः);
		}
	}

	crosstrack_error->bearing = wrap_pi(crosstrack_error->bearing);
	return_मानम् = 0;

	प्रत्यागम return_मानम्;
}

भिन्न get_distance_गन्तव्यम्_point_global_wgs84(द्विगुण lat_now, द्विगुण lon_now, भिन्न alt_now,
		द्विगुण lat_अग्र, द्विगुण lon_अग्र, भिन्न alt_अग्र,
		भिन्न *dist_xy, भिन्न *dist_z)
{
	द्विगुण current_x_rad = math::radians(lat_अग्र);
	द्विगुण current_y_rad = math::radians(lon_अग्र);
	द्विगुण x_rad = math::radians(lat_now);
	द्विगुण y_rad = math::radians(lon_now);

	द्विगुण d_lat = x_rad - current_x_rad;
	द्विगुण d_lon = y_rad - current_y_rad;

	द्विगुण a = ज्या(d_lat / 2.0) * ज्या(d_lat / 2.0) + ज्या(d_lon / 2.0) * ज्या(d_lon / 2.0) * कोटिज्या(current_x_rad) * कोटिज्या(x_rad);
	द्विगुण c = 2 * atan2(वर्गमूलम्(a), वर्गमूलम्(1 - a));

	ध्रुव भिन्न dxy = static_cast<भिन्न>(CONSTANTS_RADIUS_सम्बन्धि_EARTH * c);
	ध्रुव भिन्न dz = static_cast<भिन्न>(alt_now - alt_अग्र);

	*dist_xy = fabsf(dxy);
	*dist_z = fabsf(dz);

	प्रत्यागम sqrtf(dxy * dxy + dz * dz);
}

भिन्न mavlink_wpm_distance_गन्तव्यम्_point_local(भिन्न x_now, भिन्न y_now, भिन्न z_now,
		भिन्न x_अग्र, भिन्न y_अग्र, भिन्न z_अग्र,
		भिन्न *dist_xy, भिन्न *dist_z)
{
	भिन्न dx = x_now - x_अग्र;
	भिन्न dy = y_now - y_अग्र;
	भिन्न dz = z_now - z_अग्र;

	*dist_xy = sqrtf(dx * dx + dy * dy);
	*dist_z = fabsf(dz);

	प्रत्यागम sqrtf(dx * dx + dy * dy + dz * dz);
}
