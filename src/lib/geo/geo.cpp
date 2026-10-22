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

#ਅਨ੍ਤਰ੍ਭਾਵ "geo.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <float.h>

ਵਰਤੋਂ ਮੈਟ੍ਰਿਕਸ::wrap_pi;
ਵਰਤੋਂ ਮੈਟ੍ਰਿਕਸ::wrap_2pi;

/*
 * Azimuthal Equidistant Projection
 * formulas according to: http://mathworld.wolfram.com/AzimuthalEquidistantProjection.html
 */

ਖਾਲੀ MapProjection::initReference(ਦੁੱਗਣਾ lat_0, ਦੁੱਗਣਾ lon_0, uint64_t timestamp)
{
	_ref_timestamp = timestamp;
	_ref_lat = math::radians(lat_0);
	_ref_lon = math::radians(lon_0);
	_ref_sin_lat = ਜੀਵਾ(_ref_lat);
	_ref_cos_lat = ਕੋਜੀਵਾ(_ref_lat);
	ref_init_ਪੂਰਾ = ਸੱਚ;
}

ਖਾਲੀ MapProjection::project(ਦੁੱਗਣਾ lat, ਦੁੱਗਣਾ lon, ਦਸ਼ਮਲਵ &x, ਦਸ਼ਮਲਵ &y) ਅਚਲ
{
	ਅਚਲ ਦੁੱਗਣਾ lat_rad = math::radians(lat);
	ਅਚਲ ਦੁੱਗਣਾ lon_rad = math::radians(lon);

	ਅਚਲ ਦੁੱਗਣਾ sin_lat = ਜੀਵਾ(lat_rad);
	ਅਚਲ ਦੁੱਗਣਾ cos_lat = ਕੋਜੀਵਾ(lat_rad);

	ਅਚਲ ਦੁੱਗਣਾ cos_d_lon = ਕੋਜੀਵਾ(lon_rad - _ref_lon);

	ਅਚਲ ਦੁੱਗਣਾ arg = math::constrain(_ref_sin_lat * sin_lat + _ref_cos_lat * cos_lat * cos_d_lon, -1.0,  1.0);
	ਅਚਲ ਦੁੱਗਣਾ c = acos(arg);

	ਦੁੱਗਣਾ k = 1.0;

	ਜੇ (fabs(c) > 0) {
		k = (c / ਜੀਵਾ(c));
	}

	x = static_cast<ਦਸ਼ਮਲਵ>(k * (_ref_cos_lat * sin_lat - _ref_sin_lat * cos_lat * cos_d_lon) * CONSTANTS_RADIUS_ਦਾ_EARTH);
	y = static_cast<ਦਸ਼ਮਲਵ>(k * cos_lat * ਜੀਵਾ(lon_rad - _ref_lon) * CONSTANTS_RADIUS_ਦਾ_EARTH);
}

ਖਾਲੀ MapProjection::reproject(ਦਸ਼ਮਲਵ x, ਦਸ਼ਮਲਵ y, ਦੁੱਗਣਾ &lat, ਦੁੱਗਣਾ &lon) ਅਚਲ
{
	ਅਚਲ ਦੁੱਗਣਾ x_rad = (ਦੁੱਗਣਾ)x / CONSTANTS_RADIUS_ਦਾ_EARTH;
	ਅਚਲ ਦੁੱਗਣਾ y_rad = (ਦੁੱਗਣਾ)y / CONSTANTS_RADIUS_ਦਾ_EARTH;
	ਅਚਲ ਦੁੱਗਣਾ c = ਵਰਗਮੂਲ(x_rad * x_rad + y_rad * y_rad);

	ਜੇ (fabs(c) > 0) {
		ਅਚਲ ਦੁੱਗਣਾ sin_c = ਜੀਵਾ(c);
		ਅਚਲ ਦੁੱਗਣਾ cos_c = ਕੋਜੀਵਾ(c);

		ਅਚਲ ਦੁੱਗਣਾ lat_rad = asin(cos_c * _ref_sin_lat + (x_rad * sin_c * _ref_cos_lat) / c);
		ਅਚਲ ਦੁੱਗਣਾ lon_rad = (_ref_lon + atan2(y_rad * sin_c, c * _ref_cos_lat * cos_c - x_rad * _ref_sin_lat * sin_c));

		lat = math::degrees(lat_rad);
		lon = math::degrees(lon_rad);

	} ਨਹੀਂਤਾਂ {
		lat = math::degrees(_ref_lat);
		lon = math::degrees(_ref_lon);
	}
}

ਦਸ਼ਮਲਵ get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(ਦੁੱਗਣਾ lat_now, ਦੁੱਗਣਾ lon_now, ਦੁੱਗਣਾ lat_ਅਗਲਾ, ਦੁੱਗਣਾ lon_ਅਗਲਾ)
{
	ਅਚਲ ਦੁੱਗਣਾ lat_now_rad = math::radians(lat_now);
	ਅਚਲ ਦੁੱਗਣਾ lat_ਅਗਲਾ_rad = math::radians(lat_ਅਗਲਾ);

	ਅਚਲ ਦੁੱਗਣਾ d_lat = lat_ਅਗਲਾ_rad - lat_now_rad;
	ਅਚਲ ਦੁੱਗਣਾ d_lon = math::radians(lon_ਅਗਲਾ) - math::radians(lon_now);

	ਅਚਲ ਦੁੱਗਣਾ a = ਜੀਵਾ(d_lat / 2.0) * ਜੀਵਾ(d_lat / 2.0) + ਜੀਵਾ(d_lon / 2.0) * ਜੀਵਾ(d_lon / 2.0) * ਕੋਜੀਵਾ(lat_now_rad) * ਕੋਜੀਵਾ(
				 lat_ਅਗਲਾ_rad);

	ਅਚਲ ਦੁੱਗਣਾ c = atan2(ਵਰਗਮੂਲ(a), ਵਰਗਮੂਲ(1.0 - a));

	ਵਾਪਸ static_cast<ਦਸ਼ਮਲਵ>(CONSTANTS_RADIUS_ਦਾ_EARTH * 2.0 * c);
}

ਖਾਲੀ create_waypoint_from_ਸਤਰ_and_dist(ਦੁੱਗਣਾ lat_A, ਦੁੱਗਣਾ lon_A, ਦੁੱਗਣਾ lat_B, ਦੁੱਗਣਾ lon_B, ਦਸ਼ਮਲਵ dist,
					ਦੁੱਗਣਾ *lat_ਨਿਸ਼ਾਨਾ, ਦੁੱਗਣਾ *lon_ਨਿਸ਼ਾਨਾ)
{
	ਜੇ (fabsf(dist) < FLT_EPSILON) {
		*lat_ਨਿਸ਼ਾਨਾ = lat_A;
		*lon_ਨਿਸ਼ਾਨਾ = lon_A;

	} ਨਹੀਂਤਾਂ {
		ਦਸ਼ਮਲਵ heading = get_bearing_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(lat_A, lon_A, lat_B, lon_B);
		waypoint_from_heading_and_ਦੂਰੀ(lat_A, lon_A, heading, dist, lat_ਨਿਸ਼ਾਨਾ, lon_ਨਿਸ਼ਾਨਾ);
	}
}

ਖਾਲੀ waypoint_from_heading_and_ਦੂਰੀ(ਦੁੱਗਣਾ lat_ਸ਼ੁਰੂ, ਦੁੱਗਣਾ lon_ਸ਼ੁਰੂ, ਦਸ਼ਮਲਵ bearing, ਦਸ਼ਮਲਵ dist,
					ਦੁੱਗਣਾ *lat_ਨਿਸ਼ਾਨਾ, ਦੁੱਗਣਾ *lon_ਨਿਸ਼ਾਨਾ)
{
	bearing = wrap_2pi(bearing);

	ਦੁੱਗਣਾ radius_ਅਨੁਪਾਤ = static_cast<ਦੁੱਗਣਾ>(dist) / CONSTANTS_RADIUS_ਦਾ_EARTH;

	ਦੁੱਗਣਾ lat_ਸ਼ੁਰੂ_rad = math::radians(lat_ਸ਼ੁਰੂ);
	ਦੁੱਗਣਾ lon_ਸ਼ੁਰੂ_rad = math::radians(lon_ਸ਼ੁਰੂ);

	*lat_ਨਿਸ਼ਾਨਾ = asin(ਜੀਵਾ(lat_ਸ਼ੁਰੂ_rad) * ਕੋਜੀਵਾ(radius_ਅਨੁਪਾਤ) + ਕੋਜੀਵਾ(lat_ਸ਼ੁਰੂ_rad) * ਜੀਵਾ(radius_ਅਨੁਪਾਤ) * ਕੋਜੀਵਾ((
				   ਦੁੱਗਣਾ)bearing));
	*lon_ਨਿਸ਼ਾਨਾ = lon_ਸ਼ੁਰੂ_rad + atan2(ਜੀਵਾ((ਦੁੱਗਣਾ)bearing) * ਜੀਵਾ(radius_ਅਨੁਪਾਤ) * ਕੋਜੀਵਾ(lat_ਸ਼ੁਰੂ_rad),
					    ਕੋਜੀਵਾ(radius_ਅਨੁਪਾਤ) - ਜੀਵਾ(lat_ਸ਼ੁਰੂ_rad) * ਜੀਵਾ(*lat_ਨਿਸ਼ਾਨਾ));

	*lat_ਨਿਸ਼ਾਨਾ = math::degrees(*lat_ਨਿਸ਼ਾਨਾ);
	*lon_ਨਿਸ਼ਾਨਾ = math::degrees(*lon_ਨਿਸ਼ਾਨਾ);
}

ਦਸ਼ਮਲਵ get_bearing_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(ਦੁੱਗਣਾ lat_now, ਦੁੱਗਣਾ lon_now, ਦੁੱਗਣਾ lat_ਅਗਲਾ, ਦੁੱਗਣਾ lon_ਅਗਲਾ)
{
	ਅਚਲ ਦੁੱਗਣਾ lat_now_rad = math::radians(lat_now);
	ਅਚਲ ਦੁੱਗਣਾ lat_ਅਗਲਾ_rad = math::radians(lat_ਅਗਲਾ);

	ਅਚਲ ਦੁੱਗਣਾ cos_lat_ਅਗਲਾ = ਕੋਜੀਵਾ(lat_ਅਗਲਾ_rad);
	ਅਚਲ ਦੁੱਗਣਾ d_lon = math::radians(lon_ਅਗਲਾ - lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */

	ਅਚਲ ਦਸ਼ਮਲਵ y = static_cast<ਦਸ਼ਮਲਵ>(ਜੀਵਾ(d_lon) * cos_lat_ਅਗਲਾ);
	ਅਚਲ ਦਸ਼ਮਲਵ x = static_cast<ਦਸ਼ਮਲਵ>(ਕੋਜੀਵਾ(lat_now_rad) * ਜੀਵਾ(lat_ਅਗਲਾ_rad) - ਜੀਵਾ(lat_now_rad) * cos_lat_ਅਗਲਾ * ਕੋਜੀਵਾ(d_lon));

	ਵਾਪਸ wrap_pi(atan2f(y, x));
}

ਖਾਲੀ
get_ਸਦਿਸ਼_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(ਦੁੱਗਣਾ lat_now, ਦੁੱਗਣਾ lon_now, ਦੁੱਗਣਾ lat_ਅਗਲਾ, ਦੁੱਗਣਾ lon_ਅਗਲਾ, ਦਸ਼ਮਲਵ *v_n, ਦਸ਼ਮਲਵ *v_e)
{
	ਅਚਲ ਦੁੱਗਣਾ lat_now_rad = math::radians(lat_now);
	ਅਚਲ ਦੁੱਗਣਾ lat_ਅਗਲਾ_rad = math::radians(lat_ਅਗਲਾ);
	ਅਚਲ ਦੁੱਗਣਾ d_lon = math::radians(lon_ਅਗਲਾ) - math::radians(lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<ਦਸ਼ਮਲਵ>(CONSTANTS_RADIUS_ਦਾ_EARTH * (ਕੋਜੀਵਾ(lat_now_rad) * ਜੀਵਾ(lat_ਅਗਲਾ_rad) - ਜੀਵਾ(lat_now_rad) * ਕੋਜੀਵਾ(
					  lat_ਅਗਲਾ_rad) * ਕੋਜੀਵਾ(d_lon)));
	*v_e = static_cast<ਦਸ਼ਮਲਵ>(CONSTANTS_RADIUS_ਦਾ_EARTH * ਜੀਵਾ(d_lon) * ਕੋਜੀਵਾ(lat_ਅਗਲਾ_rad));
}

ਖਾਲੀ
get_ਸਦਿਸ਼_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint_fast(ਦੁੱਗਣਾ lat_now, ਦੁੱਗਣਾ lon_now, ਦੁੱਗਣਾ lat_ਅਗਲਾ, ਦੁੱਗਣਾ lon_ਅਗਲਾ, ਦਸ਼ਮਲਵ *v_n,
				 ਦਸ਼ਮਲਵ *v_e)
{
	ਦੁੱਗਣਾ lat_now_rad = math::radians(lat_now);
	ਦੁੱਗਣਾ lon_now_rad = math::radians(lon_now);
	ਦੁੱਗਣਾ lat_ਅਗਲਾ_rad = math::radians(lat_ਅਗਲਾ);
	ਦੁੱਗਣਾ lon_ਅਗਲਾ_rad = math::radians(lon_ਅਗਲਾ);

	ਦੁੱਗਣਾ d_lat = lat_ਅਗਲਾ_rad - lat_now_rad;
	ਦੁੱਗਣਾ d_lon = lon_ਅਗਲਾ_rad - lon_now_rad;

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<ਦਸ਼ਮਲਵ>(CONSTANTS_RADIUS_ਦਾ_EARTH * d_lat);
	*v_e = static_cast<ਦਸ਼ਮਲਵ>(CONSTANTS_RADIUS_ਦਾ_EARTH * d_lon * ਕੋਜੀਵਾ(lat_now_rad));
}

ਖਾਲੀ ਜੋੜੋ_ਸਦਿਸ਼_ਮੰਜ਼ਿਲ_global_position(ਦੁੱਗਣਾ lat_now, ਦੁੱਗਣਾ lon_now, ਦਸ਼ਮਲਵ v_n, ਦਸ਼ਮਲਵ v_e, ਦੁੱਗਣਾ *lat_res,
				   ਦੁੱਗਣਾ *lon_res)
{
	ਦੁੱਗਣਾ lat_now_rad = math::radians(lat_now);
	ਦੁੱਗਣਾ lon_now_rad = math::radians(lon_now);

	*lat_res = math::degrees(lat_now_rad + (ਦੁੱਗਣਾ)v_n / CONSTANTS_RADIUS_ਦਾ_EARTH);
	*lon_res = math::degrees(lon_now_rad + (ਦੁੱਗਣਾ)v_e / (CONSTANTS_RADIUS_ਦਾ_EARTH * ਕੋਜੀਵਾ(lat_now_rad)));
}

// Additional functions - @author Doug Weibel <douglas.weibel@colorado.edu>

ਪੂਰਨ get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਸਤਰ(ਬਣਤਰ crosstrack_error_s &crosstrack_error, ਦੁੱਗਣਾ lat_now, ਦੁੱਗਣਾ lon_now,
			 ਦੁੱਗਣਾ lat_ਸ਼ੁਰੂ, ਦੁੱਗਣਾ lon_ਸ਼ੁਰੂ, ਦੁੱਗਣਾ lat_ਸਿਰਾ, ਦੁੱਗਣਾ lon_ਸਿਰਾ)
{
	// This function returns the distance to the nearest point on the track line.  Distance is positive if current
	// position is right of the track and negative if left of the track as seen from a point on the track line
	// headed towards the end point.

	ਪੂਰਨ return_ਮੁੱਲ = -1;	// Set error flag, cleared when valid result calculated.
	crosstrack_error.past_ਸਿਰਾ = ਝੂਠ;
	crosstrack_error.ਦੂਰੀ = 0.0f;
	crosstrack_error.bearing = 0.0f;

	ਦਸ਼ਮਲਵ dist_ਮੰਜ਼ਿਲ_ਸਿਰਾ = get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(lat_now, lon_now, lat_ਸਿਰਾ, lon_ਸਿਰਾ);

	// Return error if arguments are bad
	ਜੇ (dist_ਮੰਜ਼ਿਲ_ਸਿਰਾ < 0.1f) {
		ਵਾਪਸ -1;
	}

	ਦਸ਼ਮਲਵ bearing_ਸਿਰਾ = get_bearing_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(lat_now, lon_now, lat_ਸਿਰਾ, lon_ਸਿਰਾ);
	ਦਸ਼ਮਲਵ bearing_track = get_bearing_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(lat_ਸ਼ੁਰੂ, lon_ਸ਼ੁਰੂ, lat_ਸਿਰਾ, lon_ਸਿਰਾ);
	ਦਸ਼ਮਲਵ bearing_diff = wrap_pi(bearing_track - bearing_ਸਿਰਾ);

	// Return past_end = true if past end point of line
	ਜੇ (bearing_diff > M_PI_2_F || bearing_diff < -M_PI_2_F) {
		crosstrack_error.past_ਸਿਰਾ = ਸੱਚ;
		return_ਮੁੱਲ = 0;
		ਵਾਪਸ return_ਮੁੱਲ;
	}

	crosstrack_error.ਦੂਰੀ = (dist_ਮੰਜ਼ਿਲ_ਸਿਰਾ) * sinf(bearing_diff);

	ਜੇ (sinf(bearing_diff) >= 0) {
		crosstrack_error.bearing = wrap_pi(bearing_track - M_PI_2_F);

	} ਨਹੀਂਤਾਂ {
		crosstrack_error.bearing = wrap_pi(bearing_track + M_PI_2_F);
	}

	return_ਮੁੱਲ = 0;

	ਵਾਪਸ return_ਮੁੱਲ;
}

ਪੂਰਨ get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_arc(ਬਣਤਰ crosstrack_error_s *crosstrack_error, ਦੁੱਗਣਾ lat_now, ਦੁੱਗਣਾ lon_now,
			ਦੁੱਗਣਾ lat_center, ਦੁੱਗਣਾ lon_center,
			ਦਸ਼ਮਲਵ radius, ਦਸ਼ਮਲਵ arc_ਸ਼ੁਰੂ_bearing, ਦਸ਼ਮਲਵ arc_sweep)
{
	// This function returns the distance to the nearest point on the track arc.  Distance is positive if current
	// position is right of the arc and negative if left of the arc as seen from the closest point on the arc and
	// headed towards the end point.

	// Determine if the current position is inside or outside the sector between the line from the center
	// to the arc start and the line from the center to the arc end
	ਅਚਲ ਦਸ਼ਮਲਵ bearing_now = get_bearing_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(lat_now, lon_now, lat_center, lon_center);

	ਪੂਰਨ return_ਮੁੱਲ = -1;		// Set error flag, cleared when valid result calculated.
	crosstrack_error->past_ਸਿਰਾ = ਝੂਠ;
	crosstrack_error->ਦੂਰੀ = 0.0f;
	crosstrack_error->bearing = 0.0f;

	// Return error if arguments are bad
	ਜੇ (radius < 0.1f) {
		ਵਾਪਸ return_ਮੁੱਲ;
	}

	// arc_start_bearing and arc_sweep are referenced to the center, so test membership with the
	// center->vehicle bearing. Measuring it in the direction of the sweep handles either sign and
	// any zero crossing without special cases.
	// NOLINTNEXTLINE(readability-suspicious-call-argument) intentional reverse bearing: center -> vehicle
	ਅਚਲ ਦਸ਼ਮਲਵ bearing_center_ਮੰਜ਼ਿਲ_now = get_bearing_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(lat_center, lon_center, lat_now, lon_now);
	ਅਚਲ ਦਸ਼ਮਲਵ bearing_from_ਸ਼ੁਰੂ = wrap_2pi((bearing_center_ਮੰਜ਼ਿਲ_now - arc_ਸ਼ੁਰੂ_bearing) * (arc_sweep < 0.0f ? -1.0f :
					 1.0f));
	ਅਚਲ ਤਾਰਕਿਕ ਵਿੱਚ_sector = bearing_from_ਸ਼ੁਰੂ <= fabsf(arc_sweep);

	// If in the sector then calculate distance and bearing to closest point
	ਜੇ (ਵਿੱਚ_sector) {
		crosstrack_error->past_ਸਿਰਾ = ਝੂਠ;
		ਦਸ਼ਮਲਵ dist_ਮੰਜ਼ਿਲ_center = get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(lat_now, lon_now, lat_center, lon_center);

		ਜੇ (dist_ਮੰਜ਼ਿਲ_center <= radius) {
			crosstrack_error->ਦੂਰੀ = radius - dist_ਮੰਜ਼ਿਲ_center;
			crosstrack_error->bearing = bearing_now + M_PI_F;

		} ਨਹੀਂਤਾਂ {
			crosstrack_error->ਦੂਰੀ = dist_ਮੰਜ਼ਿਲ_center - radius;
			crosstrack_error->bearing = bearing_now;
		}

		// If out of the sector then calculate dist and bearing to start or end point

	} ਨਹੀਂਤਾਂ {

		// Use the approximation  that 111,111 meters in the y direction is 1 degree (of latitude)
		// and 111,111 * cos(latitude) meters in the x direction is 1 degree (of longitude) to
		// calculate the position of the start and end points.  We should not be doing this often
		// as this function generally will not be called repeatedly when we are out of the sector.

		ਦੁੱਗਣਾ ਸ਼ੁਰੂ_disp_x = (ਦੁੱਗਣਾ)radius * ਜੀਵਾ((ਦੁੱਗਣਾ)arc_ਸ਼ੁਰੂ_bearing);
		ਦੁੱਗਣਾ ਸ਼ੁਰੂ_disp_y = (ਦੁੱਗਣਾ)radius * ਕੋਜੀਵਾ((ਦੁੱਗਣਾ)arc_ਸ਼ੁਰੂ_bearing);
		ਦੁੱਗਣਾ ਸਿਰਾ_disp_x = (ਦੁੱਗਣਾ)radius * ਜੀਵਾ((ਦੁੱਗਣਾ)wrap_pi(arc_ਸ਼ੁਰੂ_bearing + arc_sweep));
		ਦੁੱਗਣਾ ਸਿਰਾ_disp_y = (ਦੁੱਗਣਾ)radius * ਕੋਜੀਵਾ((ਦੁੱਗਣਾ)wrap_pi(arc_ਸ਼ੁਰੂ_bearing + arc_sweep));
		ਅਚਲ ਦੁੱਗਣਾ cos_lat_center = ਕੋਜੀਵਾ(math::radians(lat_center));
		ਦੁੱਗਣਾ lon_ਸ਼ੁਰੂ = lon_center + ਸ਼ੁਰੂ_disp_x / (111111.0 * cos_lat_center);
		ਦੁੱਗਣਾ lat_ਸ਼ੁਰੂ = lat_center + ਸ਼ੁਰੂ_disp_y / 111111.0;
		ਦੁੱਗਣਾ lon_ਸਿਰਾ = lon_center + ਸਿਰਾ_disp_x / (111111.0 * cos_lat_center);
		ਦੁੱਗਣਾ lat_ਸਿਰਾ = lat_center + ਸਿਰਾ_disp_y / 111111.0;
		ਦਸ਼ਮਲਵ dist_ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ = get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(lat_now, lon_now, lat_ਸ਼ੁਰੂ, lon_ਸ਼ੁਰੂ);
		ਦਸ਼ਮਲਵ dist_ਮੰਜ਼ਿਲ_ਸਿਰਾ = get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(lat_now, lon_now, lat_ਸਿਰਾ, lon_ਸਿਰਾ);

		ਜੇ (dist_ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ < dist_ਮੰਜ਼ਿਲ_ਸਿਰਾ) {
			crosstrack_error->ਦੂਰੀ = dist_ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ;
			crosstrack_error->bearing = get_bearing_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(lat_now, lon_now, lat_ਸ਼ੁਰੂ, lon_ਸ਼ੁਰੂ);

		} ਨਹੀਂਤਾਂ {
			crosstrack_error->past_ਸਿਰਾ = ਸੱਚ;
			crosstrack_error->ਦੂਰੀ = dist_ਮੰਜ਼ਿਲ_ਸਿਰਾ;
			crosstrack_error->bearing = get_bearing_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(lat_now, lon_now, lat_ਸਿਰਾ, lon_ਸਿਰਾ);
		}
	}

	crosstrack_error->bearing = wrap_pi(crosstrack_error->bearing);
	return_ਮੁੱਲ = 0;

	ਵਾਪਸ return_ਮੁੱਲ;
}

ਦਸ਼ਮਲਵ get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਬਿੰਦੂ_global_wgs84(ਦੁੱਗਣਾ lat_now, ਦੁੱਗਣਾ lon_now, ਦਸ਼ਮਲਵ alt_now,
		ਦੁੱਗਣਾ lat_ਅਗਲਾ, ਦੁੱਗਣਾ lon_ਅਗਲਾ, ਦਸ਼ਮਲਵ alt_ਅਗਲਾ,
		ਦਸ਼ਮਲਵ *dist_xy, ਦਸ਼ਮਲਵ *dist_z)
{
	ਦੁੱਗਣਾ current_x_rad = math::radians(lat_ਅਗਲਾ);
	ਦੁੱਗਣਾ current_y_rad = math::radians(lon_ਅਗਲਾ);
	ਦੁੱਗਣਾ x_rad = math::radians(lat_now);
	ਦੁੱਗਣਾ y_rad = math::radians(lon_now);

	ਦੁੱਗਣਾ d_lat = x_rad - current_x_rad;
	ਦੁੱਗਣਾ d_lon = y_rad - current_y_rad;

	ਦੁੱਗਣਾ a = ਜੀਵਾ(d_lat / 2.0) * ਜੀਵਾ(d_lat / 2.0) + ਜੀਵਾ(d_lon / 2.0) * ਜੀਵਾ(d_lon / 2.0) * ਕੋਜੀਵਾ(current_x_rad) * ਕੋਜੀਵਾ(x_rad);
	ਦੁੱਗਣਾ c = 2 * atan2(ਵਰਗਮੂਲ(a), ਵਰਗਮੂਲ(1 - a));

	ਅਚਲ ਦਸ਼ਮਲਵ dxy = static_cast<ਦਸ਼ਮਲਵ>(CONSTANTS_RADIUS_ਦਾ_EARTH * c);
	ਅਚਲ ਦਸ਼ਮਲਵ dz = static_cast<ਦਸ਼ਮਲਵ>(alt_now - alt_ਅਗਲਾ);

	*dist_xy = fabsf(dxy);
	*dist_z = fabsf(dz);

	ਵਾਪਸ sqrtf(dxy * dxy + dz * dz);
}

ਦਸ਼ਮਲਵ mavlink_wpm_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਬਿੰਦੂ_local(ਦਸ਼ਮਲਵ x_now, ਦਸ਼ਮਲਵ y_now, ਦਸ਼ਮਲਵ z_now,
		ਦਸ਼ਮਲਵ x_ਅਗਲਾ, ਦਸ਼ਮਲਵ y_ਅਗਲਾ, ਦਸ਼ਮਲਵ z_ਅਗਲਾ,
		ਦਸ਼ਮਲਵ *dist_xy, ਦਸ਼ਮਲਵ *dist_z)
{
	ਦਸ਼ਮਲਵ dx = x_now - x_ਅਗਲਾ;
	ਦਸ਼ਮਲਵ dy = y_now - y_ਅਗਲਾ;
	ਦਸ਼ਮਲਵ dz = z_now - z_ਅਗਲਾ;

	*dist_xy = sqrtf(dx * dx + dy * dy);
	*dist_z = fabsf(dz);

	ਵਾਪਸ sqrtf(dx * dx + dy * dy + dz * dz);
}
