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

ورتوں میٹرکس::wrap_pi;
ورتوں میٹرکس::wrap_2pi;

/*
 * Azimuthal Equidistant Projection
 * formulas according to: http://mathworld.wolfram.com/AzimuthalEquidistantProjection.html
 */

فراغ MapProjection::initReference(مزدوج lat_0, مزدوج lon_0, uint64_t timestamp)
{
	_ref_timestamp = timestamp;
	_ref_lat = math::radians(lat_0);
	_ref_lon = math::radians(lon_0);
	_ref_sin_lat = جیب(_ref_lat);
	_ref_cos_lat = جیب_تمام(_ref_lat);
	ref_init_پورا = سچ;
}

فراغ MapProjection::project(مزدوج lat, مزدوج lon, عائم &x, عائم &y) ثابت
{
	ثابت مزدوج lat_rad = math::radians(lat);
	ثابت مزدوج lon_rad = math::radians(lon);

	ثابت مزدوج sin_lat = جیب(lat_rad);
	ثابت مزدوج cos_lat = جیب_تمام(lat_rad);

	ثابت مزدوج cos_d_lon = جیب_تمام(lon_rad - _ref_lon);

	ثابت مزدوج arg = math::constrain(_ref_sin_lat * sin_lat + _ref_cos_lat * cos_lat * cos_d_lon, -1.0,  1.0);
	ثابت مزدوج c = acos(arg);

	مزدوج k = 1.0;

	إذا (fabs(c) > 0) {
		k = (c / جیب(c));
	}

	x = static_cast<عائم>(k * (_ref_cos_lat * sin_lat - _ref_sin_lat * cos_lat * cos_d_lon) * CONSTANTS_RADIUS_دا_EARTH);
	y = static_cast<عائم>(k * cos_lat * جیب(lon_rad - _ref_lon) * CONSTANTS_RADIUS_دا_EARTH);
}

فراغ MapProjection::reproject(عائم x, عائم y, مزدوج &lat, مزدوج &lon) ثابت
{
	ثابت مزدوج x_rad = (مزدوج)x / CONSTANTS_RADIUS_دا_EARTH;
	ثابت مزدوج y_rad = (مزدوج)y / CONSTANTS_RADIUS_دا_EARTH;
	ثابت مزدوج c = جذر(x_rad * x_rad + y_rad * y_rad);

	إذا (fabs(c) > 0) {
		ثابت مزدوج sin_c = جیب(c);
		ثابت مزدوج cos_c = جیب_تمام(c);

		ثابت مزدوج lat_rad = asin(cos_c * _ref_sin_lat + (x_rad * sin_c * _ref_cos_lat) / c);
		ثابت مزدوج lon_rad = (_ref_lon + atan2(y_rad * sin_c, c * _ref_cos_lat * cos_c - x_rad * _ref_sin_lat * sin_c));

		lat = math::degrees(lat_rad);
		lon = math::degrees(lon_rad);

	} وإلا {
		lat = math::degrees(_ref_lat);
		lon = math::degrees(_ref_lon);
	}
}

عائم get_فاصلہ_منزل_اگلا_waypoint(مزدوج lat_now, مزدوج lon_now, مزدوج lat_اگلا, مزدوج lon_اگلا)
{
	ثابت مزدوج lat_now_rad = math::radians(lat_now);
	ثابت مزدوج lat_اگلا_rad = math::radians(lat_اگلا);

	ثابت مزدوج d_lat = lat_اگلا_rad - lat_now_rad;
	ثابت مزدوج d_lon = math::radians(lon_اگلا) - math::radians(lon_now);

	ثابت مزدوج a = جیب(d_lat / 2.0) * جیب(d_lat / 2.0) + جیب(d_lon / 2.0) * جیب(d_lon / 2.0) * جیب_تمام(lat_now_rad) * جیب_تمام(
				 lat_اگلا_rad);

	ثابت مزدوج c = atan2(جذر(a), جذر(1.0 - a));

	أرجع static_cast<عائم>(CONSTANTS_RADIUS_دا_EARTH * 2.0 * c);
}

فراغ create_waypoint_from_سطر_and_dist(مزدوج lat_A, مزدوج lon_A, مزدوج lat_B, مزدوج lon_B, عائم dist,
					مزدوج *lat_نشانہ, مزدوج *lon_نشانہ)
{
	إذا (fabsf(dist) < FLT_EPSILON) {
		*lat_نشانہ = lat_A;
		*lon_نشانہ = lon_A;

	} وإلا {
		عائم heading = get_bearing_منزل_اگلا_waypoint(lat_A, lon_A, lat_B, lon_B);
		waypoint_from_heading_and_فاصلہ(lat_A, lon_A, heading, dist, lat_نشانہ, lon_نشانہ);
	}
}

فراغ waypoint_from_heading_and_فاصلہ(مزدوج lat_شروع, مزدوج lon_شروع, عائم bearing, عائم dist,
					مزدوج *lat_نشانہ, مزدوج *lon_نشانہ)
{
	bearing = wrap_2pi(bearing);

	مزدوج radius_تناسب = static_cast<مزدوج>(dist) / CONSTANTS_RADIUS_دا_EARTH;

	مزدوج lat_شروع_rad = math::radians(lat_شروع);
	مزدوج lon_شروع_rad = math::radians(lon_شروع);

	*lat_نشانہ = asin(جیب(lat_شروع_rad) * جیب_تمام(radius_تناسب) + جیب_تمام(lat_شروع_rad) * جیب(radius_تناسب) * جیب_تمام((
				   مزدوج)bearing));
	*lon_نشانہ = lon_شروع_rad + atan2(جیب((مزدوج)bearing) * جیب(radius_تناسب) * جیب_تمام(lat_شروع_rad),
					    جیب_تمام(radius_تناسب) - جیب(lat_شروع_rad) * جیب(*lat_نشانہ));

	*lat_نشانہ = math::degrees(*lat_نشانہ);
	*lon_نشانہ = math::degrees(*lon_نشانہ);
}

عائم get_bearing_منزل_اگلا_waypoint(مزدوج lat_now, مزدوج lon_now, مزدوج lat_اگلا, مزدوج lon_اگلا)
{
	ثابت مزدوج lat_now_rad = math::radians(lat_now);
	ثابت مزدوج lat_اگلا_rad = math::radians(lat_اگلا);

	ثابت مزدوج cos_lat_اگلا = جیب_تمام(lat_اگلا_rad);
	ثابت مزدوج d_lon = math::radians(lon_اگلا - lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */

	ثابت عائم y = static_cast<عائم>(جیب(d_lon) * cos_lat_اگلا);
	ثابت عائم x = static_cast<عائم>(جیب_تمام(lat_now_rad) * جیب(lat_اگلا_rad) - جیب(lat_now_rad) * cos_lat_اگلا * جیب_تمام(d_lon));

	أرجع wrap_pi(atan2f(y, x));
}

فراغ
get_سمتیہ_منزل_اگلا_waypoint(مزدوج lat_now, مزدوج lon_now, مزدوج lat_اگلا, مزدوج lon_اگلا, عائم *v_n, عائم *v_e)
{
	ثابت مزدوج lat_now_rad = math::radians(lat_now);
	ثابت مزدوج lat_اگلا_rad = math::radians(lat_اگلا);
	ثابت مزدوج d_lon = math::radians(lon_اگلا) - math::radians(lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<عائم>(CONSTANTS_RADIUS_دا_EARTH * (جیب_تمام(lat_now_rad) * جیب(lat_اگلا_rad) - جیب(lat_now_rad) * جیب_تمام(
					  lat_اگلا_rad) * جیب_تمام(d_lon)));
	*v_e = static_cast<عائم>(CONSTANTS_RADIUS_دا_EARTH * جیب(d_lon) * جیب_تمام(lat_اگلا_rad));
}

فراغ
get_سمتیہ_منزل_اگلا_waypoint_fast(مزدوج lat_now, مزدوج lon_now, مزدوج lat_اگلا, مزدوج lon_اگلا, عائم *v_n,
				 عائم *v_e)
{
	مزدوج lat_now_rad = math::radians(lat_now);
	مزدوج lon_now_rad = math::radians(lon_now);
	مزدوج lat_اگلا_rad = math::radians(lat_اگلا);
	مزدوج lon_اگلا_rad = math::radians(lon_اگلا);

	مزدوج d_lat = lat_اگلا_rad - lat_now_rad;
	مزدوج d_lon = lon_اگلا_rad - lon_now_rad;

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<عائم>(CONSTANTS_RADIUS_دا_EARTH * d_lat);
	*v_e = static_cast<عائم>(CONSTANTS_RADIUS_دا_EARTH * d_lon * جیب_تمام(lat_now_rad));
}

فراغ جوڑو_سمتیہ_منزل_global_position(مزدوج lat_now, مزدوج lon_now, عائم v_n, عائم v_e, مزدوج *lat_res,
				   مزدوج *lon_res)
{
	مزدوج lat_now_rad = math::radians(lat_now);
	مزدوج lon_now_rad = math::radians(lon_now);

	*lat_res = math::degrees(lat_now_rad + (مزدوج)v_n / CONSTANTS_RADIUS_دا_EARTH);
	*lon_res = math::degrees(lon_now_rad + (مزدوج)v_e / (CONSTANTS_RADIUS_دا_EARTH * جیب_تمام(lat_now_rad)));
}

// Additional functions - @author Doug Weibel <douglas.weibel@colorado.edu>

صحيح get_فاصلہ_منزل_سطر(بنية crosstrack_error_s &crosstrack_error, مزدوج lat_now, مزدوج lon_now,
			 مزدوج lat_شروع, مزدوج lon_شروع, مزدوج lat_سرا, مزدوج lon_سرا)
{
	// This function returns the distance to the nearest point on the track line.  Distance is positive if current
	// position is right of the track and negative if left of the track as seen from a point on the track line
	// headed towards the end point.

	صحيح return_قدر = -1;	// Set error flag, cleared when valid result calculated.
	crosstrack_error.past_سرا = خطأ;
	crosstrack_error.فاصلہ = 0.0f;
	crosstrack_error.bearing = 0.0f;

	عائم dist_منزل_سرا = get_فاصلہ_منزل_اگلا_waypoint(lat_now, lon_now, lat_سرا, lon_سرا);

	// Return error if arguments are bad
	إذا (dist_منزل_سرا < 0.1f) {
		أرجع -1;
	}

	عائم bearing_سرا = get_bearing_منزل_اگلا_waypoint(lat_now, lon_now, lat_سرا, lon_سرا);
	عائم bearing_track = get_bearing_منزل_اگلا_waypoint(lat_شروع, lon_شروع, lat_سرا, lon_سرا);
	عائم bearing_diff = wrap_pi(bearing_track - bearing_سرا);

	// Return past_end = true if past end point of line
	إذا (bearing_diff > M_PI_2_F || bearing_diff < -M_PI_2_F) {
		crosstrack_error.past_سرا = سچ;
		return_قدر = 0;
		أرجع return_قدر;
	}

	crosstrack_error.فاصلہ = (dist_منزل_سرا) * sinf(bearing_diff);

	إذا (sinf(bearing_diff) >= 0) {
		crosstrack_error.bearing = wrap_pi(bearing_track - M_PI_2_F);

	} وإلا {
		crosstrack_error.bearing = wrap_pi(bearing_track + M_PI_2_F);
	}

	return_قدر = 0;

	أرجع return_قدر;
}

صحيح get_فاصلہ_منزل_arc(بنية crosstrack_error_s *crosstrack_error, مزدوج lat_now, مزدوج lon_now,
			مزدوج lat_center, مزدوج lon_center,
			عائم radius, عائم arc_شروع_bearing, عائم arc_sweep)
{
	// This function returns the distance to the nearest point on the track arc.  Distance is positive if current
	// position is right of the arc and negative if left of the arc as seen from the closest point on the arc and
	// headed towards the end point.

	// Determine if the current position is inside or outside the sector between the line from the center
	// to the arc start and the line from the center to the arc end
	ثابت عائم bearing_now = get_bearing_منزل_اگلا_waypoint(lat_now, lon_now, lat_center, lon_center);

	صحيح return_قدر = -1;		// Set error flag, cleared when valid result calculated.
	crosstrack_error->past_سرا = خطأ;
	crosstrack_error->فاصلہ = 0.0f;
	crosstrack_error->bearing = 0.0f;

	// Return error if arguments are bad
	إذا (radius < 0.1f) {
		أرجع return_قدر;
	}

	// arc_start_bearing and arc_sweep are referenced to the center, so test membership with the
	// center->vehicle bearing. Measuring it in the direction of the sweep handles either sign and
	// any zero crossing without special cases.
	// NOLINTNEXTLINE(readability-suspicious-call-argument) intentional reverse bearing: center -> vehicle
	ثابت عائم bearing_center_منزل_now = get_bearing_منزل_اگلا_waypoint(lat_center, lon_center, lat_now, lon_now);
	ثابت عائم bearing_from_شروع = wrap_2pi((bearing_center_منزل_now - arc_شروع_bearing) * (arc_sweep < 0.0f ? -1.0f :
					 1.0f));
	ثابت منطقي وچ_sector = bearing_from_شروع <= fabsf(arc_sweep);

	// If in the sector then calculate distance and bearing to closest point
	إذا (وچ_sector) {
		crosstrack_error->past_سرا = خطأ;
		عائم dist_منزل_center = get_فاصلہ_منزل_اگلا_waypoint(lat_now, lon_now, lat_center, lon_center);

		إذا (dist_منزل_center <= radius) {
			crosstrack_error->فاصلہ = radius - dist_منزل_center;
			crosstrack_error->bearing = bearing_now + M_PI_F;

		} وإلا {
			crosstrack_error->فاصلہ = dist_منزل_center - radius;
			crosstrack_error->bearing = bearing_now;
		}

		// If out of the sector then calculate dist and bearing to start or end point

	} وإلا {

		// Use the approximation  that 111,111 meters in the y direction is 1 degree (of latitude)
		// and 111,111 * cos(latitude) meters in the x direction is 1 degree (of longitude) to
		// calculate the position of the start and end points.  We should not be doing this often
		// as this function generally will not be called repeatedly when we are out of the sector.

		مزدوج شروع_disp_x = (مزدوج)radius * جیب((مزدوج)arc_شروع_bearing);
		مزدوج شروع_disp_y = (مزدوج)radius * جیب_تمام((مزدوج)arc_شروع_bearing);
		مزدوج سرا_disp_x = (مزدوج)radius * جیب((مزدوج)wrap_pi(arc_شروع_bearing + arc_sweep));
		مزدوج سرا_disp_y = (مزدوج)radius * جیب_تمام((مزدوج)wrap_pi(arc_شروع_bearing + arc_sweep));
		ثابت مزدوج cos_lat_center = جیب_تمام(math::radians(lat_center));
		مزدوج lon_شروع = lon_center + شروع_disp_x / (111111.0 * cos_lat_center);
		مزدوج lat_شروع = lat_center + شروع_disp_y / 111111.0;
		مزدوج lon_سرا = lon_center + سرا_disp_x / (111111.0 * cos_lat_center);
		مزدوج lat_سرا = lat_center + سرا_disp_y / 111111.0;
		عائم dist_منزل_شروع = get_فاصلہ_منزل_اگلا_waypoint(lat_now, lon_now, lat_شروع, lon_شروع);
		عائم dist_منزل_سرا = get_فاصلہ_منزل_اگلا_waypoint(lat_now, lon_now, lat_سرا, lon_سرا);

		إذا (dist_منزل_شروع < dist_منزل_سرا) {
			crosstrack_error->فاصلہ = dist_منزل_شروع;
			crosstrack_error->bearing = get_bearing_منزل_اگلا_waypoint(lat_now, lon_now, lat_شروع, lon_شروع);

		} وإلا {
			crosstrack_error->past_سرا = سچ;
			crosstrack_error->فاصلہ = dist_منزل_سرا;
			crosstrack_error->bearing = get_bearing_منزل_اگلا_waypoint(lat_now, lon_now, lat_سرا, lon_سرا);
		}
	}

	crosstrack_error->bearing = wrap_pi(crosstrack_error->bearing);
	return_قدر = 0;

	أرجع return_قدر;
}

عائم get_فاصلہ_منزل_نقطہ_global_wgs84(مزدوج lat_now, مزدوج lon_now, عائم alt_now,
		مزدوج lat_اگلا, مزدوج lon_اگلا, عائم alt_اگلا,
		عائم *dist_xy, عائم *dist_z)
{
	مزدوج current_x_rad = math::radians(lat_اگلا);
	مزدوج current_y_rad = math::radians(lon_اگلا);
	مزدوج x_rad = math::radians(lat_now);
	مزدوج y_rad = math::radians(lon_now);

	مزدوج d_lat = x_rad - current_x_rad;
	مزدوج d_lon = y_rad - current_y_rad;

	مزدوج a = جیب(d_lat / 2.0) * جیب(d_lat / 2.0) + جیب(d_lon / 2.0) * جیب(d_lon / 2.0) * جیب_تمام(current_x_rad) * جیب_تمام(x_rad);
	مزدوج c = 2 * atan2(جذر(a), جذر(1 - a));

	ثابت عائم dxy = static_cast<عائم>(CONSTANTS_RADIUS_دا_EARTH * c);
	ثابت عائم dz = static_cast<عائم>(alt_now - alt_اگلا);

	*dist_xy = fabsf(dxy);
	*dist_z = fabsf(dz);

	أرجع sqrtf(dxy * dxy + dz * dz);
}

عائم mavlink_wpm_فاصلہ_منزل_نقطہ_local(عائم x_now, عائم y_now, عائم z_now,
		عائم x_اگلا, عائم y_اگلا, عائم z_اگلا,
		عائم *dist_xy, عائم *dist_z)
{
	عائم dx = x_now - x_اگلا;
	عائم dy = y_now - y_اگلا;
	عائم dz = z_now - z_اگلا;

	*dist_xy = sqrtf(dx * dx + dy * dy);
	*dist_z = fabsf(dz);

	أرجع sqrtf(dx * dx + dy * dy + dz * dz);
}
