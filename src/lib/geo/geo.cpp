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

#ಅನ್ತರ್ಭಾವ "geo.h"

#ಅನ್ತರ್ಭಾವ <float.h>

ಬಳಸಿ ಮಾತೃಕೆ::wrap_pi;
ಬಳಸಿ ಮಾತೃಕೆ::wrap_2pi;

/*
 * Azimuthal Equidistant Projection
 * formulas according to: http://mathworld.wolfram.com/AzimuthalEquidistantProjection.html
 */

ಶೂನ್ಯ MapProjection::initReference(ದ್ವಿಗುಣ lat_0, ದ್ವಿಗುಣ lon_0, uint64_t timestamp)
{
	_ref_timestamp = timestamp;
	_ref_lat = math::radians(lat_0);
	_ref_lon = math::radians(lon_0);
	_ref_sin_lat = ಜ್ಯಾ(_ref_lat);
	_ref_cos_lat = ಕೋಜ್ಯಾ(_ref_lat);
	ref_init_ಮುಗಿದ = ನಿಜ;
}

ಶೂನ್ಯ MapProjection::project(ದ್ವಿಗುಣ lat, ದ್ವಿಗುಣ lon, ದಶಮಾಂಶ &x, ದಶಮಾಂಶ &y) ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ lat_rad = math::radians(lat);
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ lon_rad = math::radians(lon);

	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ sin_lat = ಜ್ಯಾ(lat_rad);
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ cos_lat = ಕೋಜ್ಯಾ(lat_rad);

	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ cos_d_lon = ಕೋಜ್ಯಾ(lon_rad - _ref_lon);

	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ arg = math::constrain(_ref_sin_lat * sin_lat + _ref_cos_lat * cos_lat * cos_d_lon, -1.0,  1.0);
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ c = acos(arg);

	ದ್ವಿಗುಣ k = 1.0;

	ಆದರೆ (fabs(c) > 0) {
		k = (c / ಜ್ಯಾ(c));
	}

	x = static_cast<ದಶಮಾಂಶ>(k * (_ref_cos_lat * sin_lat - _ref_sin_lat * cos_lat * cos_d_lon) * CONSTANTS_RADIUS_ದ_EARTH);
	y = static_cast<ದಶಮಾಂಶ>(k * cos_lat * ಜ್ಯಾ(lon_rad - _ref_lon) * CONSTANTS_RADIUS_ದ_EARTH);
}

ಶೂನ್ಯ MapProjection::reproject(ದಶಮಾಂಶ x, ದಶಮಾಂಶ y, ದ್ವಿಗುಣ &lat, ದ್ವಿಗುಣ &lon) ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ x_rad = (ದ್ವಿಗುಣ)x / CONSTANTS_RADIUS_ದ_EARTH;
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ y_rad = (ದ್ವಿಗುಣ)y / CONSTANTS_RADIUS_ದ_EARTH;
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ c = ವರ್ಗಮೂಲ(x_rad * x_rad + y_rad * y_rad);

	ಆದರೆ (fabs(c) > 0) {
		ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ sin_c = ಜ್ಯಾ(c);
		ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ cos_c = ಕೋಜ್ಯಾ(c);

		ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ lat_rad = asin(cos_c * _ref_sin_lat + (x_rad * sin_c * _ref_cos_lat) / c);
		ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ lon_rad = (_ref_lon + atan2(y_rad * sin_c, c * _ref_cos_lat * cos_c - x_rad * _ref_sin_lat * sin_c));

		lat = math::degrees(lat_rad);
		lon = math::degrees(lon_rad);

	} ಇಲ್ಲದಿದ್ದರೆ {
		lat = math::degrees(_ref_lat);
		lon = math::degrees(_ref_lon);
	}
}

ದಶಮಾಂಶ get_ದೂರ_ಗಮ್ಯ_ಮುಂದಿನ_waypoint(ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now, ದ್ವಿಗುಣ lat_ಮುಂದಿನ, ದ್ವಿಗುಣ lon_ಮುಂದಿನ)
{
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ lat_now_rad = math::radians(lat_now);
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ lat_ಮುಂದಿನ_rad = math::radians(lat_ಮುಂದಿನ);

	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ d_lat = lat_ಮುಂದಿನ_rad - lat_now_rad;
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ d_lon = math::radians(lon_ಮುಂದಿನ) - math::radians(lon_now);

	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ a = ಜ್ಯಾ(d_lat / 2.0) * ಜ್ಯಾ(d_lat / 2.0) + ಜ್ಯಾ(d_lon / 2.0) * ಜ್ಯಾ(d_lon / 2.0) * ಕೋಜ್ಯಾ(lat_now_rad) * ಕೋಜ್ಯಾ(
				 lat_ಮುಂದಿನ_rad);

	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ c = atan2(ವರ್ಗಮೂಲ(a), ವರ್ಗಮೂಲ(1.0 - a));

	ಹಿಂತಿರುಗಿ static_cast<ದಶಮಾಂಶ>(CONSTANTS_RADIUS_ದ_EARTH * 2.0 * c);
}

ಶೂನ್ಯ create_waypoint_from_ಸಾಲು_and_dist(ದ್ವಿಗುಣ lat_A, ದ್ವಿಗುಣ lon_A, ದ್ವಿಗುಣ lat_B, ದ್ವಿಗುಣ lon_B, ದಶಮಾಂಶ dist,
					ದ್ವಿಗುಣ *lat_ಗುರಿ, ದ್ವಿಗುಣ *lon_ಗುರಿ)
{
	ಆದರೆ (fabsf(dist) < FLT_EPSILON) {
		*lat_ಗುರಿ = lat_A;
		*lon_ಗುರಿ = lon_A;

	} ಇಲ್ಲದಿದ್ದರೆ {
		ದಶಮಾಂಶ heading = get_bearing_ಗಮ್ಯ_ಮುಂದಿನ_waypoint(lat_A, lon_A, lat_B, lon_B);
		waypoint_from_heading_and_ದೂರ(lat_A, lon_A, heading, dist, lat_ಗುರಿ, lon_ಗುರಿ);
	}
}

ಶೂನ್ಯ waypoint_from_heading_and_ದೂರ(ದ್ವಿಗುಣ lat_ಆರಂಭ, ದ್ವಿಗುಣ lon_ಆರಂಭ, ದಶಮಾಂಶ bearing, ದಶಮಾಂಶ dist,
					ದ್ವಿಗುಣ *lat_ಗುರಿ, ದ್ವಿಗುಣ *lon_ಗುರಿ)
{
	bearing = wrap_2pi(bearing);

	ದ್ವಿಗುಣ radius_ಅನುಪಾತ = static_cast<ದ್ವಿಗುಣ>(dist) / CONSTANTS_RADIUS_ದ_EARTH;

	ದ್ವಿಗುಣ lat_ಆರಂಭ_rad = math::radians(lat_ಆರಂಭ);
	ದ್ವಿಗುಣ lon_ಆರಂಭ_rad = math::radians(lon_ಆರಂಭ);

	*lat_ಗುರಿ = asin(ಜ್ಯಾ(lat_ಆರಂಭ_rad) * ಕೋಜ್ಯಾ(radius_ಅನುಪಾತ) + ಕೋಜ್ಯಾ(lat_ಆರಂಭ_rad) * ಜ್ಯಾ(radius_ಅನುಪಾತ) * ಕೋಜ್ಯಾ((
				   ದ್ವಿಗುಣ)bearing));
	*lon_ಗುರಿ = lon_ಆರಂಭ_rad + atan2(ಜ್ಯಾ((ದ್ವಿಗುಣ)bearing) * ಜ್ಯಾ(radius_ಅನುಪಾತ) * ಕೋಜ್ಯಾ(lat_ಆರಂಭ_rad),
					    ಕೋಜ್ಯಾ(radius_ಅನುಪಾತ) - ಜ್ಯಾ(lat_ಆರಂಭ_rad) * ಜ್ಯಾ(*lat_ಗುರಿ));

	*lat_ಗುರಿ = math::degrees(*lat_ಗುರಿ);
	*lon_ಗುರಿ = math::degrees(*lon_ಗುರಿ);
}

ದಶಮಾಂಶ get_bearing_ಗಮ್ಯ_ಮುಂದಿನ_waypoint(ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now, ದ್ವಿಗುಣ lat_ಮುಂದಿನ, ದ್ವಿಗುಣ lon_ಮುಂದಿನ)
{
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ lat_now_rad = math::radians(lat_now);
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ lat_ಮುಂದಿನ_rad = math::radians(lat_ಮುಂದಿನ);

	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ cos_lat_ಮುಂದಿನ = ಕೋಜ್ಯಾ(lat_ಮುಂದಿನ_rad);
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ d_lon = math::radians(lon_ಮುಂದಿನ - lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ y = static_cast<ದಶಮಾಂಶ>(ಜ್ಯಾ(d_lon) * cos_lat_ಮುಂದಿನ);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ x = static_cast<ದಶಮಾಂಶ>(ಕೋಜ್ಯಾ(lat_now_rad) * ಜ್ಯಾ(lat_ಮುಂದಿನ_rad) - ಜ್ಯಾ(lat_now_rad) * cos_lat_ಮುಂದಿನ * ಕೋಜ್ಯಾ(d_lon));

	ಹಿಂತಿರುಗಿ wrap_pi(atan2f(y, x));
}

ಶೂನ್ಯ
get_ಸದಿಶ_ಗಮ್ಯ_ಮುಂದಿನ_waypoint(ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now, ದ್ವಿಗುಣ lat_ಮುಂದಿನ, ದ್ವಿಗುಣ lon_ಮುಂದಿನ, ದಶಮಾಂಶ *v_n, ದಶಮಾಂಶ *v_e)
{
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ lat_now_rad = math::radians(lat_now);
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ lat_ಮುಂದಿನ_rad = math::radians(lat_ಮುಂದಿನ);
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ d_lon = math::radians(lon_ಮುಂದಿನ) - math::radians(lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<ದಶಮಾಂಶ>(CONSTANTS_RADIUS_ದ_EARTH * (ಕೋಜ್ಯಾ(lat_now_rad) * ಜ್ಯಾ(lat_ಮುಂದಿನ_rad) - ಜ್ಯಾ(lat_now_rad) * ಕೋಜ್ಯಾ(
					  lat_ಮುಂದಿನ_rad) * ಕೋಜ್ಯಾ(d_lon)));
	*v_e = static_cast<ದಶಮಾಂಶ>(CONSTANTS_RADIUS_ದ_EARTH * ಜ್ಯಾ(d_lon) * ಕೋಜ್ಯಾ(lat_ಮುಂದಿನ_rad));
}

ಶೂನ್ಯ
get_ಸದಿಶ_ಗಮ್ಯ_ಮುಂದಿನ_waypoint_fast(ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now, ದ್ವಿಗುಣ lat_ಮುಂದಿನ, ದ್ವಿಗುಣ lon_ಮುಂದಿನ, ದಶಮಾಂಶ *v_n,
				 ದಶಮಾಂಶ *v_e)
{
	ದ್ವಿಗುಣ lat_now_rad = math::radians(lat_now);
	ದ್ವಿಗುಣ lon_now_rad = math::radians(lon_now);
	ದ್ವಿಗುಣ lat_ಮುಂದಿನ_rad = math::radians(lat_ಮುಂದಿನ);
	ದ್ವಿಗುಣ lon_ಮುಂದಿನ_rad = math::radians(lon_ಮುಂದಿನ);

	ದ್ವಿಗುಣ d_lat = lat_ಮುಂದಿನ_rad - lat_now_rad;
	ದ್ವಿಗುಣ d_lon = lon_ಮುಂದಿನ_rad - lon_now_rad;

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<ದಶಮಾಂಶ>(CONSTANTS_RADIUS_ದ_EARTH * d_lat);
	*v_e = static_cast<ದಶಮಾಂಶ>(CONSTANTS_RADIUS_ದ_EARTH * d_lon * ಕೋಜ್ಯಾ(lat_now_rad));
}

ಶೂನ್ಯ ಕೂಡಿಸು_ಸದಿಶ_ಗಮ್ಯ_global_position(ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now, ದಶಮಾಂಶ v_n, ದಶಮಾಂಶ v_e, ದ್ವಿಗುಣ *lat_res,
				   ದ್ವಿಗುಣ *lon_res)
{
	ದ್ವಿಗುಣ lat_now_rad = math::radians(lat_now);
	ದ್ವಿಗುಣ lon_now_rad = math::radians(lon_now);

	*lat_res = math::degrees(lat_now_rad + (ದ್ವಿಗುಣ)v_n / CONSTANTS_RADIUS_ದ_EARTH);
	*lon_res = math::degrees(lon_now_rad + (ದ್ವಿಗುಣ)v_e / (CONSTANTS_RADIUS_ದ_EARTH * ಕೋಜ್ಯಾ(lat_now_rad)));
}

// Additional functions - @author Doug Weibel <douglas.weibel@colorado.edu>

ಪೂರ್ಣಾಂಕ get_ದೂರ_ಗಮ್ಯ_ಸಾಲು(ರಚನೆ crosstrack_error_s &crosstrack_error, ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now,
			 ದ್ವಿಗುಣ lat_ಆರಂಭ, ದ್ವಿಗುಣ lon_ಆರಂಭ, ದ್ವಿಗುಣ lat_ಕೊನೆ, ದ್ವಿಗುಣ lon_ಕೊನೆ)
{
	// This function returns the distance to the nearest point on the track line.  Distance is positive if current
	// position is right of the track and negative if left of the track as seen from a point on the track line
	// headed towards the end point.

	ಪೂರ್ಣಾಂಕ return_ಮೌಲ್ಯ = -1;	// Set error flag, cleared when valid result calculated.
	crosstrack_error.past_ಕೊನೆ = ಸುಳ್ಳು;
	crosstrack_error.ದೂರ = 0.0f;
	crosstrack_error.bearing = 0.0f;

	ದಶಮಾಂಶ dist_ಗಮ್ಯ_ಕೊನೆ = get_ದೂರ_ಗಮ್ಯ_ಮುಂದಿನ_waypoint(lat_now, lon_now, lat_ಕೊನೆ, lon_ಕೊನೆ);

	// Return error if arguments are bad
	ಆದರೆ (dist_ಗಮ್ಯ_ಕೊನೆ < 0.1f) {
		ಹಿಂತಿರುಗಿ -1;
	}

	ದಶಮಾಂಶ bearing_ಕೊನೆ = get_bearing_ಗಮ್ಯ_ಮುಂದಿನ_waypoint(lat_now, lon_now, lat_ಕೊನೆ, lon_ಕೊನೆ);
	ದಶಮಾಂಶ bearing_track = get_bearing_ಗಮ್ಯ_ಮುಂದಿನ_waypoint(lat_ಆರಂಭ, lon_ಆರಂಭ, lat_ಕೊನೆ, lon_ಕೊನೆ);
	ದಶಮಾಂಶ bearing_diff = wrap_pi(bearing_track - bearing_ಕೊನೆ);

	// Return past_end = true if past end point of line
	ಆದರೆ (bearing_diff > M_PI_2_F || bearing_diff < -M_PI_2_F) {
		crosstrack_error.past_ಕೊನೆ = ನಿಜ;
		return_ಮೌಲ್ಯ = 0;
		ಹಿಂತಿರುಗಿ return_ಮೌಲ್ಯ;
	}

	crosstrack_error.ದೂರ = (dist_ಗಮ್ಯ_ಕೊನೆ) * sinf(bearing_diff);

	ಆದರೆ (sinf(bearing_diff) >= 0) {
		crosstrack_error.bearing = wrap_pi(bearing_track - M_PI_2_F);

	} ಇಲ್ಲದಿದ್ದರೆ {
		crosstrack_error.bearing = wrap_pi(bearing_track + M_PI_2_F);
	}

	return_ಮೌಲ್ಯ = 0;

	ಹಿಂತಿರುಗಿ return_ಮೌಲ್ಯ;
}

ಪೂರ್ಣಾಂಕ get_ದೂರ_ಗಮ್ಯ_arc(ರಚನೆ crosstrack_error_s *crosstrack_error, ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now,
			ದ್ವಿಗುಣ lat_center, ದ್ವಿಗುಣ lon_center,
			ದಶಮಾಂಶ radius, ದಶಮಾಂಶ arc_ಆರಂಭ_bearing, ದಶಮಾಂಶ arc_sweep)
{
	// This function returns the distance to the nearest point on the track arc.  Distance is positive if current
	// position is right of the arc and negative if left of the arc as seen from the closest point on the arc and
	// headed towards the end point.

	// Determine if the current position is inside or outside the sector between the line from the center
	// to the arc start and the line from the center to the arc end
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ bearing_now = get_bearing_ಗಮ್ಯ_ಮುಂದಿನ_waypoint(lat_now, lon_now, lat_center, lon_center);

	ಪೂರ್ಣಾಂಕ return_ಮೌಲ್ಯ = -1;		// Set error flag, cleared when valid result calculated.
	crosstrack_error->past_ಕೊನೆ = ಸುಳ್ಳು;
	crosstrack_error->ದೂರ = 0.0f;
	crosstrack_error->bearing = 0.0f;

	// Return error if arguments are bad
	ಆದರೆ (radius < 0.1f) {
		ಹಿಂತಿರುಗಿ return_ಮೌಲ್ಯ;
	}

	// arc_start_bearing and arc_sweep are referenced to the center, so test membership with the
	// center->vehicle bearing. Measuring it in the direction of the sweep handles either sign and
	// any zero crossing without special cases.
	// NOLINTNEXTLINE(readability-suspicious-call-argument) intentional reverse bearing: center -> vehicle
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ bearing_center_ಗಮ್ಯ_now = get_bearing_ಗಮ್ಯ_ಮುಂದಿನ_waypoint(lat_center, lon_center, lat_now, lon_now);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ bearing_from_ಆರಂಭ = wrap_2pi((bearing_center_ಗಮ್ಯ_now - arc_ಆರಂಭ_bearing) * (arc_sweep < 0.0f ? -1.0f :
					 1.0f));
	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ ಒಳ_sector = bearing_from_ಆರಂಭ <= fabsf(arc_sweep);

	// If in the sector then calculate distance and bearing to closest point
	ಆದರೆ (ಒಳ_sector) {
		crosstrack_error->past_ಕೊನೆ = ಸುಳ್ಳು;
		ದಶಮಾಂಶ dist_ಗಮ್ಯ_center = get_ದೂರ_ಗಮ್ಯ_ಮುಂದಿನ_waypoint(lat_now, lon_now, lat_center, lon_center);

		ಆದರೆ (dist_ಗಮ್ಯ_center <= radius) {
			crosstrack_error->ದೂರ = radius - dist_ಗಮ್ಯ_center;
			crosstrack_error->bearing = bearing_now + M_PI_F;

		} ಇಲ್ಲದಿದ್ದರೆ {
			crosstrack_error->ದೂರ = dist_ಗಮ್ಯ_center - radius;
			crosstrack_error->bearing = bearing_now;
		}

		// If out of the sector then calculate dist and bearing to start or end point

	} ಇಲ್ಲದಿದ್ದರೆ {

		// Use the approximation  that 111,111 meters in the y direction is 1 degree (of latitude)
		// and 111,111 * cos(latitude) meters in the x direction is 1 degree (of longitude) to
		// calculate the position of the start and end points.  We should not be doing this often
		// as this function generally will not be called repeatedly when we are out of the sector.

		ದ್ವಿಗುಣ ಆರಂಭ_disp_x = (ದ್ವಿಗುಣ)radius * ಜ್ಯಾ((ದ್ವಿಗುಣ)arc_ಆರಂಭ_bearing);
		ದ್ವಿಗುಣ ಆರಂಭ_disp_y = (ದ್ವಿಗುಣ)radius * ಕೋಜ್ಯಾ((ದ್ವಿಗುಣ)arc_ಆರಂಭ_bearing);
		ದ್ವಿಗುಣ ಕೊನೆ_disp_x = (ದ್ವಿಗುಣ)radius * ಜ್ಯಾ((ದ್ವಿಗುಣ)wrap_pi(arc_ಆರಂಭ_bearing + arc_sweep));
		ದ್ವಿಗುಣ ಕೊನೆ_disp_y = (ದ್ವಿಗುಣ)radius * ಕೋಜ್ಯಾ((ದ್ವಿಗುಣ)wrap_pi(arc_ಆರಂಭ_bearing + arc_sweep));
		ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ cos_lat_center = ಕೋಜ್ಯಾ(math::radians(lat_center));
		ದ್ವಿಗುಣ lon_ಆರಂಭ = lon_center + ಆರಂಭ_disp_x / (111111.0 * cos_lat_center);
		ದ್ವಿಗುಣ lat_ಆರಂಭ = lat_center + ಆರಂಭ_disp_y / 111111.0;
		ದ್ವಿಗುಣ lon_ಕೊನೆ = lon_center + ಕೊನೆ_disp_x / (111111.0 * cos_lat_center);
		ದ್ವಿಗುಣ lat_ಕೊನೆ = lat_center + ಕೊನೆ_disp_y / 111111.0;
		ದಶಮಾಂಶ dist_ಗಮ್ಯ_ಆರಂಭ = get_ದೂರ_ಗಮ್ಯ_ಮುಂದಿನ_waypoint(lat_now, lon_now, lat_ಆರಂಭ, lon_ಆರಂಭ);
		ದಶಮಾಂಶ dist_ಗಮ್ಯ_ಕೊನೆ = get_ದೂರ_ಗಮ್ಯ_ಮುಂದಿನ_waypoint(lat_now, lon_now, lat_ಕೊನೆ, lon_ಕೊನೆ);

		ಆದರೆ (dist_ಗಮ್ಯ_ಆರಂಭ < dist_ಗಮ್ಯ_ಕೊನೆ) {
			crosstrack_error->ದೂರ = dist_ಗಮ್ಯ_ಆರಂಭ;
			crosstrack_error->bearing = get_bearing_ಗಮ್ಯ_ಮುಂದಿನ_waypoint(lat_now, lon_now, lat_ಆರಂಭ, lon_ಆರಂಭ);

		} ಇಲ್ಲದಿದ್ದರೆ {
			crosstrack_error->past_ಕೊನೆ = ನಿಜ;
			crosstrack_error->ದೂರ = dist_ಗಮ್ಯ_ಕೊನೆ;
			crosstrack_error->bearing = get_bearing_ಗಮ್ಯ_ಮುಂದಿನ_waypoint(lat_now, lon_now, lat_ಕೊನೆ, lon_ಕೊನೆ);
		}
	}

	crosstrack_error->bearing = wrap_pi(crosstrack_error->bearing);
	return_ಮೌಲ್ಯ = 0;

	ಹಿಂತಿರುಗಿ return_ಮೌಲ್ಯ;
}

ದಶಮಾಂಶ get_ದೂರ_ಗಮ್ಯ_ಬಿಂದು_global_wgs84(ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now, ದಶಮಾಂಶ alt_now,
		ದ್ವಿಗುಣ lat_ಮುಂದಿನ, ದ್ವಿಗುಣ lon_ಮುಂದಿನ, ದಶಮಾಂಶ alt_ಮುಂದಿನ,
		ದಶಮಾಂಶ *dist_xy, ದಶಮಾಂಶ *dist_z)
{
	ದ್ವಿಗುಣ current_x_rad = math::radians(lat_ಮುಂದಿನ);
	ದ್ವಿಗುಣ current_y_rad = math::radians(lon_ಮುಂದಿನ);
	ದ್ವಿಗುಣ x_rad = math::radians(lat_now);
	ದ್ವಿಗುಣ y_rad = math::radians(lon_now);

	ದ್ವಿಗುಣ d_lat = x_rad - current_x_rad;
	ದ್ವಿಗುಣ d_lon = y_rad - current_y_rad;

	ದ್ವಿಗುಣ a = ಜ್ಯಾ(d_lat / 2.0) * ಜ್ಯಾ(d_lat / 2.0) + ಜ್ಯಾ(d_lon / 2.0) * ಜ್ಯಾ(d_lon / 2.0) * ಕೋಜ್ಯಾ(current_x_rad) * ಕೋಜ್ಯಾ(x_rad);
	ದ್ವಿಗುಣ c = 2 * atan2(ವರ್ಗಮೂಲ(a), ವರ್ಗಮೂಲ(1 - a));

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dxy = static_cast<ದಶಮಾಂಶ>(CONSTANTS_RADIUS_ದ_EARTH * c);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dz = static_cast<ದಶಮಾಂಶ>(alt_now - alt_ಮುಂದಿನ);

	*dist_xy = fabsf(dxy);
	*dist_z = fabsf(dz);

	ಹಿಂತಿರುಗಿ sqrtf(dxy * dxy + dz * dz);
}

ದಶಮಾಂಶ mavlink_wpm_ದೂರ_ಗಮ್ಯ_ಬಿಂದು_local(ದಶಮಾಂಶ x_now, ದಶಮಾಂಶ y_now, ದಶಮಾಂಶ z_now,
		ದಶಮಾಂಶ x_ಮುಂದಿನ, ದಶಮಾಂಶ y_ಮುಂದಿನ, ದಶಮಾಂಶ z_ಮುಂದಿನ,
		ದಶಮಾಂಶ *dist_xy, ದಶಮಾಂಶ *dist_z)
{
	ದಶಮಾಂಶ dx = x_now - x_ಮುಂದಿನ;
	ದಶಮಾಂಶ dy = y_now - y_ಮುಂದಿನ;
	ದಶಮಾಂಶ dz = z_now - z_ಮುಂದಿನ;

	*dist_xy = sqrtf(dx * dx + dy * dy);
	*dist_z = fabsf(dz);

	ಹಿಂತಿರುಗಿ sqrtf(dx * dx + dy * dy + dz * dz);
}
