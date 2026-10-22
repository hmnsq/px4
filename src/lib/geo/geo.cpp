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

استعمال ميٽرڪس::wrap_pi;
استعمال ميٽرڪس::wrap_2pi;

/*
 * Azimuthal Equidistant Projection
 * formulas according to: http://mathworld.wolfram.com/AzimuthalEquidistantProjection.html
 */

فراغ MapProjection::initReference(مزدوج lat_0, مزدوج lon_0, uint64_t timestamp)
{
	_ref_timestamp = timestamp;
	_ref_lat = math::radians(lat_0);
	_ref_lon = math::radians(lon_0);
	_ref_sin_lat = جيب(_ref_lat);
	_ref_cos_lat = جيب_تمام(_ref_lat);
	ref_init_پورو = سچ;
}

فراغ MapProjection::project(مزدوج lat, مزدوج lon, عائم &x, عائم &y) ثابت
{
	ثابت مزدوج lat_rad = math::radians(lat);
	ثابت مزدوج lon_rad = math::radians(lon);

	ثابت مزدوج sin_lat = جيب(lat_rad);
	ثابت مزدوج cos_lat = جيب_تمام(lat_rad);

	ثابت مزدوج cos_d_lon = جيب_تمام(lon_rad - _ref_lon);

	ثابت مزدوج arg = math::constrain(_ref_sin_lat * sin_lat + _ref_cos_lat * cos_lat * cos_d_lon, -1.0,  1.0);
	ثابت مزدوج c = acos(arg);

	مزدوج k = 1.0;

	إذا (fabs(c) > 0) {
		k = (c / جيب(c));
	}

	x = static_cast<عائم>(k * (_ref_cos_lat * sin_lat - _ref_sin_lat * cos_lat * cos_d_lon) * CONSTANTS_RADIUS_جو_EARTH);
	y = static_cast<عائم>(k * cos_lat * جيب(lon_rad - _ref_lon) * CONSTANTS_RADIUS_جو_EARTH);
}

فراغ MapProjection::reproject(عائم x, عائم y, مزدوج &lat, مزدوج &lon) ثابت
{
	ثابت مزدوج x_rad = (مزدوج)x / CONSTANTS_RADIUS_جو_EARTH;
	ثابت مزدوج y_rad = (مزدوج)y / CONSTANTS_RADIUS_جو_EARTH;
	ثابت مزدوج c = جذر(x_rad * x_rad + y_rad * y_rad);

	إذا (fabs(c) > 0) {
		ثابت مزدوج sin_c = جيب(c);
		ثابت مزدوج cos_c = جيب_تمام(c);

		ثابت مزدوج lat_rad = asin(cos_c * _ref_sin_lat + (x_rad * sin_c * _ref_cos_lat) / c);
		ثابت مزدوج lon_rad = (_ref_lon + atan2(y_rad * sin_c, c * _ref_cos_lat * cos_c - x_rad * _ref_sin_lat * sin_c));

		lat = math::degrees(lat_rad);
		lon = math::degrees(lon_rad);

	} وإلا {
		lat = math::degrees(_ref_lat);
		lon = math::degrees(_ref_lon);
	}
}

عائم get_مفاصلو_منزل_ايندڙ_waypoint(مزدوج lat_now, مزدوج lon_now, مزدوج lat_ايندڙ, مزدوج lon_ايندڙ)
{
	ثابت مزدوج lat_now_rad = math::radians(lat_now);
	ثابت مزدوج lat_ايندڙ_rad = math::radians(lat_ايندڙ);

	ثابت مزدوج d_lat = lat_ايندڙ_rad - lat_now_rad;
	ثابت مزدوج d_lon = math::radians(lon_ايندڙ) - math::radians(lon_now);

	ثابت مزدوج a = جيب(d_lat / 2.0) * جيب(d_lat / 2.0) + جيب(d_lon / 2.0) * جيب(d_lon / 2.0) * جيب_تمام(lat_now_rad) * جيب_تمام(
				 lat_ايندڙ_rad);

	ثابت مزدوج c = atan2(جذر(a), جذر(1.0 - a));

	أرجع static_cast<عائم>(CONSTANTS_RADIUS_جو_EARTH * 2.0 * c);
}

فراغ create_waypoint_from_سٽ_and_dist(مزدوج lat_A, مزدوج lon_A, مزدوج lat_B, مزدوج lon_B, عائم dist,
					مزدوج *lat_هدف, مزدوج *lon_هدف)
{
	إذا (fabsf(dist) < FLT_EPSILON) {
		*lat_هدف = lat_A;
		*lon_هدف = lon_A;

	} وإلا {
		عائم heading = get_bearing_منزل_ايندڙ_waypoint(lat_A, lon_A, lat_B, lon_B);
		waypoint_from_heading_and_مفاصلو(lat_A, lon_A, heading, dist, lat_هدف, lon_هدف);
	}
}

فراغ waypoint_from_heading_and_مفاصلو(مزدوج lat_شروعات, مزدوج lon_شروعات, عائم bearing, عائم dist,
					مزدوج *lat_هدف, مزدوج *lon_هدف)
{
	bearing = wrap_2pi(bearing);

	مزدوج radius_تناسب = static_cast<مزدوج>(dist) / CONSTANTS_RADIUS_جو_EARTH;

	مزدوج lat_شروعات_rad = math::radians(lat_شروعات);
	مزدوج lon_شروعات_rad = math::radians(lon_شروعات);

	*lat_هدف = asin(جيب(lat_شروعات_rad) * جيب_تمام(radius_تناسب) + جيب_تمام(lat_شروعات_rad) * جيب(radius_تناسب) * جيب_تمام((
				   مزدوج)bearing));
	*lon_هدف = lon_شروعات_rad + atan2(جيب((مزدوج)bearing) * جيب(radius_تناسب) * جيب_تمام(lat_شروعات_rad),
					    جيب_تمام(radius_تناسب) - جيب(lat_شروعات_rad) * جيب(*lat_هدف));

	*lat_هدف = math::degrees(*lat_هدف);
	*lon_هدف = math::degrees(*lon_هدف);
}

عائم get_bearing_منزل_ايندڙ_waypoint(مزدوج lat_now, مزدوج lon_now, مزدوج lat_ايندڙ, مزدوج lon_ايندڙ)
{
	ثابت مزدوج lat_now_rad = math::radians(lat_now);
	ثابت مزدوج lat_ايندڙ_rad = math::radians(lat_ايندڙ);

	ثابت مزدوج cos_lat_ايندڙ = جيب_تمام(lat_ايندڙ_rad);
	ثابت مزدوج d_lon = math::radians(lon_ايندڙ - lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */

	ثابت عائم y = static_cast<عائم>(جيب(d_lon) * cos_lat_ايندڙ);
	ثابت عائم x = static_cast<عائم>(جيب_تمام(lat_now_rad) * جيب(lat_ايندڙ_rad) - جيب(lat_now_rad) * cos_lat_ايندڙ * جيب_تمام(d_lon));

	أرجع wrap_pi(atan2f(y, x));
}

فراغ
get_ويڪٽر_منزل_ايندڙ_waypoint(مزدوج lat_now, مزدوج lon_now, مزدوج lat_ايندڙ, مزدوج lon_ايندڙ, عائم *v_n, عائم *v_e)
{
	ثابت مزدوج lat_now_rad = math::radians(lat_now);
	ثابت مزدوج lat_ايندڙ_rad = math::radians(lat_ايندڙ);
	ثابت مزدوج d_lon = math::radians(lon_ايندڙ) - math::radians(lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<عائم>(CONSTANTS_RADIUS_جو_EARTH * (جيب_تمام(lat_now_rad) * جيب(lat_ايندڙ_rad) - جيب(lat_now_rad) * جيب_تمام(
					  lat_ايندڙ_rad) * جيب_تمام(d_lon)));
	*v_e = static_cast<عائم>(CONSTANTS_RADIUS_جو_EARTH * جيب(d_lon) * جيب_تمام(lat_ايندڙ_rad));
}

فراغ
get_ويڪٽر_منزل_ايندڙ_waypoint_fast(مزدوج lat_now, مزدوج lon_now, مزدوج lat_ايندڙ, مزدوج lon_ايندڙ, عائم *v_n,
				 عائم *v_e)
{
	مزدوج lat_now_rad = math::radians(lat_now);
	مزدوج lon_now_rad = math::radians(lon_now);
	مزدوج lat_ايندڙ_rad = math::radians(lat_ايندڙ);
	مزدوج lon_ايندڙ_rad = math::radians(lon_ايندڙ);

	مزدوج d_lat = lat_ايندڙ_rad - lat_now_rad;
	مزدوج d_lon = lon_ايندڙ_rad - lon_now_rad;

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<عائم>(CONSTANTS_RADIUS_جو_EARTH * d_lat);
	*v_e = static_cast<عائم>(CONSTANTS_RADIUS_جو_EARTH * d_lon * جيب_تمام(lat_now_rad));
}

فراغ جوڙيو_ويڪٽر_منزل_global_position(مزدوج lat_now, مزدوج lon_now, عائم v_n, عائم v_e, مزدوج *lat_res,
				   مزدوج *lon_res)
{
	مزدوج lat_now_rad = math::radians(lat_now);
	مزدوج lon_now_rad = math::radians(lon_now);

	*lat_res = math::degrees(lat_now_rad + (مزدوج)v_n / CONSTANTS_RADIUS_جو_EARTH);
	*lon_res = math::degrees(lon_now_rad + (مزدوج)v_e / (CONSTANTS_RADIUS_جو_EARTH * جيب_تمام(lat_now_rad)));
}

// Additional functions - @author Doug Weibel <douglas.weibel@colorado.edu>

صحيح get_مفاصلو_منزل_سٽ(بنية crosstrack_error_s &crosstrack_error, مزدوج lat_now, مزدوج lon_now,
			 مزدوج lat_شروعات, مزدوج lon_شروعات, مزدوج lat_پڇاڙي, مزدوج lon_پڇاڙي)
{
	// This function returns the distance to the nearest point on the track line.  Distance is positive if current
	// position is right of the track and negative if left of the track as seen from a point on the track line
	// headed towards the end point.

	صحيح return_قدر = -1;	// Set error flag, cleared when valid result calculated.
	crosstrack_error.past_پڇاڙي = خطأ;
	crosstrack_error.مفاصلو = 0.0f;
	crosstrack_error.bearing = 0.0f;

	عائم dist_منزل_پڇاڙي = get_مفاصلو_منزل_ايندڙ_waypoint(lat_now, lon_now, lat_پڇاڙي, lon_پڇاڙي);

	// Return error if arguments are bad
	إذا (dist_منزل_پڇاڙي < 0.1f) {
		أرجع -1;
	}

	عائم bearing_پڇاڙي = get_bearing_منزل_ايندڙ_waypoint(lat_now, lon_now, lat_پڇاڙي, lon_پڇاڙي);
	عائم bearing_track = get_bearing_منزل_ايندڙ_waypoint(lat_شروعات, lon_شروعات, lat_پڇاڙي, lon_پڇاڙي);
	عائم bearing_diff = wrap_pi(bearing_track - bearing_پڇاڙي);

	// Return past_end = true if past end point of line
	إذا (bearing_diff > M_PI_2_F || bearing_diff < -M_PI_2_F) {
		crosstrack_error.past_پڇاڙي = سچ;
		return_قدر = 0;
		أرجع return_قدر;
	}

	crosstrack_error.مفاصلو = (dist_منزل_پڇاڙي) * sinf(bearing_diff);

	إذا (sinf(bearing_diff) >= 0) {
		crosstrack_error.bearing = wrap_pi(bearing_track - M_PI_2_F);

	} وإلا {
		crosstrack_error.bearing = wrap_pi(bearing_track + M_PI_2_F);
	}

	return_قدر = 0;

	أرجع return_قدر;
}

صحيح get_مفاصلو_منزل_arc(بنية crosstrack_error_s *crosstrack_error, مزدوج lat_now, مزدوج lon_now,
			مزدوج lat_center, مزدوج lon_center,
			عائم radius, عائم arc_شروعات_bearing, عائم arc_sweep)
{
	// This function returns the distance to the nearest point on the track arc.  Distance is positive if current
	// position is right of the arc and negative if left of the arc as seen from the closest point on the arc and
	// headed towards the end point.

	// Determine if the current position is inside or outside the sector between the line from the center
	// to the arc start and the line from the center to the arc end
	ثابت عائم bearing_now = get_bearing_منزل_ايندڙ_waypoint(lat_now, lon_now, lat_center, lon_center);

	صحيح return_قدر = -1;		// Set error flag, cleared when valid result calculated.
	crosstrack_error->past_پڇاڙي = خطأ;
	crosstrack_error->مفاصلو = 0.0f;
	crosstrack_error->bearing = 0.0f;

	// Return error if arguments are bad
	إذا (radius < 0.1f) {
		أرجع return_قدر;
	}

	// arc_start_bearing and arc_sweep are referenced to the center, so test membership with the
	// center->vehicle bearing. Measuring it in the direction of the sweep handles either sign and
	// any zero crossing without special cases.
	// NOLINTNEXTLINE(readability-suspicious-call-argument) intentional reverse bearing: center -> vehicle
	ثابت عائم bearing_center_منزل_now = get_bearing_منزل_ايندڙ_waypoint(lat_center, lon_center, lat_now, lon_now);
	ثابت عائم bearing_from_شروعات = wrap_2pi((bearing_center_منزل_now - arc_شروعات_bearing) * (arc_sweep < 0.0f ? -1.0f :
					 1.0f));
	ثابت منطقي اندريون_sector = bearing_from_شروعات <= fabsf(arc_sweep);

	// If in the sector then calculate distance and bearing to closest point
	إذا (اندريون_sector) {
		crosstrack_error->past_پڇاڙي = خطأ;
		عائم dist_منزل_center = get_مفاصلو_منزل_ايندڙ_waypoint(lat_now, lon_now, lat_center, lon_center);

		إذا (dist_منزل_center <= radius) {
			crosstrack_error->مفاصلو = radius - dist_منزل_center;
			crosstrack_error->bearing = bearing_now + M_PI_F;

		} وإلا {
			crosstrack_error->مفاصلو = dist_منزل_center - radius;
			crosstrack_error->bearing = bearing_now;
		}

		// If out of the sector then calculate dist and bearing to start or end point

	} وإلا {

		// Use the approximation  that 111,111 meters in the y direction is 1 degree (of latitude)
		// and 111,111 * cos(latitude) meters in the x direction is 1 degree (of longitude) to
		// calculate the position of the start and end points.  We should not be doing this often
		// as this function generally will not be called repeatedly when we are out of the sector.

		مزدوج شروعات_disp_x = (مزدوج)radius * جيب((مزدوج)arc_شروعات_bearing);
		مزدوج شروعات_disp_y = (مزدوج)radius * جيب_تمام((مزدوج)arc_شروعات_bearing);
		مزدوج پڇاڙي_disp_x = (مزدوج)radius * جيب((مزدوج)wrap_pi(arc_شروعات_bearing + arc_sweep));
		مزدوج پڇاڙي_disp_y = (مزدوج)radius * جيب_تمام((مزدوج)wrap_pi(arc_شروعات_bearing + arc_sweep));
		ثابت مزدوج cos_lat_center = جيب_تمام(math::radians(lat_center));
		مزدوج lon_شروعات = lon_center + شروعات_disp_x / (111111.0 * cos_lat_center);
		مزدوج lat_شروعات = lat_center + شروعات_disp_y / 111111.0;
		مزدوج lon_پڇاڙي = lon_center + پڇاڙي_disp_x / (111111.0 * cos_lat_center);
		مزدوج lat_پڇاڙي = lat_center + پڇاڙي_disp_y / 111111.0;
		عائم dist_منزل_شروعات = get_مفاصلو_منزل_ايندڙ_waypoint(lat_now, lon_now, lat_شروعات, lon_شروعات);
		عائم dist_منزل_پڇاڙي = get_مفاصلو_منزل_ايندڙ_waypoint(lat_now, lon_now, lat_پڇاڙي, lon_پڇاڙي);

		إذا (dist_منزل_شروعات < dist_منزل_پڇاڙي) {
			crosstrack_error->مفاصلو = dist_منزل_شروعات;
			crosstrack_error->bearing = get_bearing_منزل_ايندڙ_waypoint(lat_now, lon_now, lat_شروعات, lon_شروعات);

		} وإلا {
			crosstrack_error->past_پڇاڙي = سچ;
			crosstrack_error->مفاصلو = dist_منزل_پڇاڙي;
			crosstrack_error->bearing = get_bearing_منزل_ايندڙ_waypoint(lat_now, lon_now, lat_پڇاڙي, lon_پڇاڙي);
		}
	}

	crosstrack_error->bearing = wrap_pi(crosstrack_error->bearing);
	return_قدر = 0;

	أرجع return_قدر;
}

عائم get_مفاصلو_منزل_نقطو_global_wgs84(مزدوج lat_now, مزدوج lon_now, عائم alt_now,
		مزدوج lat_ايندڙ, مزدوج lon_ايندڙ, عائم alt_ايندڙ,
		عائم *dist_xy, عائم *dist_z)
{
	مزدوج current_x_rad = math::radians(lat_ايندڙ);
	مزدوج current_y_rad = math::radians(lon_ايندڙ);
	مزدوج x_rad = math::radians(lat_now);
	مزدوج y_rad = math::radians(lon_now);

	مزدوج d_lat = x_rad - current_x_rad;
	مزدوج d_lon = y_rad - current_y_rad;

	مزدوج a = جيب(d_lat / 2.0) * جيب(d_lat / 2.0) + جيب(d_lon / 2.0) * جيب(d_lon / 2.0) * جيب_تمام(current_x_rad) * جيب_تمام(x_rad);
	مزدوج c = 2 * atan2(جذر(a), جذر(1 - a));

	ثابت عائم dxy = static_cast<عائم>(CONSTANTS_RADIUS_جو_EARTH * c);
	ثابت عائم dz = static_cast<عائم>(alt_now - alt_ايندڙ);

	*dist_xy = fabsf(dxy);
	*dist_z = fabsf(dz);

	أرجع sqrtf(dxy * dxy + dz * dz);
}

عائم mavlink_wpm_مفاصلو_منزل_نقطو_local(عائم x_now, عائم y_now, عائم z_now,
		عائم x_ايندڙ, عائم y_ايندڙ, عائم z_ايندڙ,
		عائم *dist_xy, عائم *dist_z)
{
	عائم dx = x_now - x_ايندڙ;
	عائم dy = y_now - y_ايندڙ;
	عائم dz = z_now - z_ايندڙ;

	*dist_xy = sqrtf(dx * dx + dy * dy);
	*dist_z = fabsf(dz);

	أرجع sqrtf(dx * dx + dy * dy + dz * dz);
}
