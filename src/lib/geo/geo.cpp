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

استعمال میٹرکس::wrap_pi;
استعمال میٹرکس::wrap_2pi;

/*
 * Azimuthal Equidistant Projection
 * formulas according to: http://mathworld.wolfram.com/AzimuthalEquidistantProjection.html
 */

خالی MapProjection::initReference(دوہرا lat_0, دوہرا lon_0, uint64_t timestamp)
{
	_ref_timestamp = timestamp;
	_ref_lat = math::radians(lat_0);
	_ref_lon = math::radians(lon_0);
	_ref_sin_lat = جیب(_ref_lat);
	_ref_cos_lat = جیب_تمام(_ref_lat);
	ref_init_مکمل = سچ;
}

خالی MapProjection::project(دوہرا lat, دوہرا lon, اعشاری &x, اعشاری &y) ثابت
{
	ثابت دوہرا lat_rad = math::radians(lat);
	ثابت دوہرا lon_rad = math::radians(lon);

	ثابت دوہرا sin_lat = جیب(lat_rad);
	ثابت دوہرا cos_lat = جیب_تمام(lat_rad);

	ثابت دوہرا cos_d_lon = جیب_تمام(lon_rad - _ref_lon);

	ثابت دوہرا arg = math::constrain(_ref_sin_lat * sin_lat + _ref_cos_lat * cos_lat * cos_d_lon, -1.0,  1.0);
	ثابت دوہرا c = acos(arg);

	دوہرا k = 1.0;

	اگر (fabs(c) > 0) {
		k = (c / جیب(c));
	}

	x = static_cast<اعشاری>(k * (_ref_cos_lat * sin_lat - _ref_sin_lat * cos_lat * cos_d_lon) * CONSTANTS_RADIUS_کا_EARTH);
	y = static_cast<اعشاری>(k * cos_lat * جیب(lon_rad - _ref_lon) * CONSTANTS_RADIUS_کا_EARTH);
}

خالی MapProjection::reproject(اعشاری x, اعشاری y, دوہرا &lat, دوہرا &lon) ثابت
{
	ثابت دوہرا x_rad = (دوہرا)x / CONSTANTS_RADIUS_کا_EARTH;
	ثابت دوہرا y_rad = (دوہرا)y / CONSTANTS_RADIUS_کا_EARTH;
	ثابت دوہرا c = جذر(x_rad * x_rad + y_rad * y_rad);

	اگر (fabs(c) > 0) {
		ثابت دوہرا sin_c = جیب(c);
		ثابت دوہرا cos_c = جیب_تمام(c);

		ثابت دوہرا lat_rad = asin(cos_c * _ref_sin_lat + (x_rad * sin_c * _ref_cos_lat) / c);
		ثابت دوہرا lon_rad = (_ref_lon + atan2(y_rad * sin_c, c * _ref_cos_lat * cos_c - x_rad * _ref_sin_lat * sin_c));

		lat = math::degrees(lat_rad);
		lon = math::degrees(lon_rad);

	} ورنہ {
		lat = math::degrees(_ref_lat);
		lon = math::degrees(_ref_lon);
	}
}

اعشاری get_فاصلہ_مقصد_اگلا_waypoint(دوہرا lat_now, دوہرا lon_now, دوہرا lat_اگلا, دوہرا lon_اگلا)
{
	ثابت دوہرا lat_now_rad = math::radians(lat_now);
	ثابت دوہرا lat_اگلا_rad = math::radians(lat_اگلا);

	ثابت دوہرا d_lat = lat_اگلا_rad - lat_now_rad;
	ثابت دوہرا d_lon = math::radians(lon_اگلا) - math::radians(lon_now);

	ثابت دوہرا a = جیب(d_lat / 2.0) * جیب(d_lat / 2.0) + جیب(d_lon / 2.0) * جیب(d_lon / 2.0) * جیب_تمام(lat_now_rad) * جیب_تمام(
				 lat_اگلا_rad);

	ثابت دوہرا c = atan2(جذر(a), جذر(1.0 - a));

	واپس static_cast<اعشاری>(CONSTANTS_RADIUS_کا_EARTH * 2.0 * c);
}

خالی create_waypoint_from_سطر_and_dist(دوہرا lat_A, دوہرا lon_A, دوہرا lat_B, دوہرا lon_B, اعشاری dist,
					دوہرا *lat_ہدف, دوہرا *lon_ہدف)
{
	اگر (fabsf(dist) < FLT_EPSILON) {
		*lat_ہدف = lat_A;
		*lon_ہدف = lon_A;

	} ورنہ {
		اعشاری heading = get_bearing_مقصد_اگلا_waypoint(lat_A, lon_A, lat_B, lon_B);
		waypoint_from_heading_and_فاصلہ(lat_A, lon_A, heading, dist, lat_ہدف, lon_ہدف);
	}
}

خالی waypoint_from_heading_and_فاصلہ(دوہرا lat_آغاز, دوہرا lon_آغاز, اعشاری bearing, اعشاری dist,
					دوہرا *lat_ہدف, دوہرا *lon_ہدف)
{
	bearing = wrap_2pi(bearing);

	دوہرا radius_تناسب = static_cast<دوہرا>(dist) / CONSTANTS_RADIUS_کا_EARTH;

	دوہرا lat_آغاز_rad = math::radians(lat_آغاز);
	دوہرا lon_آغاز_rad = math::radians(lon_آغاز);

	*lat_ہدف = asin(جیب(lat_آغاز_rad) * جیب_تمام(radius_تناسب) + جیب_تمام(lat_آغاز_rad) * جیب(radius_تناسب) * جیب_تمام((
				   دوہرا)bearing));
	*lon_ہدف = lon_آغاز_rad + atan2(جیب((دوہرا)bearing) * جیب(radius_تناسب) * جیب_تمام(lat_آغاز_rad),
					    جیب_تمام(radius_تناسب) - جیب(lat_آغاز_rad) * جیب(*lat_ہدف));

	*lat_ہدف = math::degrees(*lat_ہدف);
	*lon_ہدف = math::degrees(*lon_ہدف);
}

اعشاری get_bearing_مقصد_اگلا_waypoint(دوہرا lat_now, دوہرا lon_now, دوہرا lat_اگلا, دوہرا lon_اگلا)
{
	ثابت دوہرا lat_now_rad = math::radians(lat_now);
	ثابت دوہرا lat_اگلا_rad = math::radians(lat_اگلا);

	ثابت دوہرا cos_lat_اگلا = جیب_تمام(lat_اگلا_rad);
	ثابت دوہرا d_lon = math::radians(lon_اگلا - lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */

	ثابت اعشاری y = static_cast<اعشاری>(جیب(d_lon) * cos_lat_اگلا);
	ثابت اعشاری x = static_cast<اعشاری>(جیب_تمام(lat_now_rad) * جیب(lat_اگلا_rad) - جیب(lat_now_rad) * cos_lat_اگلا * جیب_تمام(d_lon));

	واپس wrap_pi(atan2f(y, x));
}

خالی
get_سمتیہ_مقصد_اگلا_waypoint(دوہرا lat_now, دوہرا lon_now, دوہرا lat_اگلا, دوہرا lon_اگلا, اعشاری *v_n, اعشاری *v_e)
{
	ثابت دوہرا lat_now_rad = math::radians(lat_now);
	ثابت دوہرا lat_اگلا_rad = math::radians(lat_اگلا);
	ثابت دوہرا d_lon = math::radians(lon_اگلا) - math::radians(lon_now);

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<اعشاری>(CONSTANTS_RADIUS_کا_EARTH * (جیب_تمام(lat_now_rad) * جیب(lat_اگلا_rad) - جیب(lat_now_rad) * جیب_تمام(
					  lat_اگلا_rad) * جیب_تمام(d_lon)));
	*v_e = static_cast<اعشاری>(CONSTANTS_RADIUS_کا_EARTH * جیب(d_lon) * جیب_تمام(lat_اگلا_rad));
}

خالی
get_سمتیہ_مقصد_اگلا_waypoint_fast(دوہرا lat_now, دوہرا lon_now, دوہرا lat_اگلا, دوہرا lon_اگلا, اعشاری *v_n,
				 اعشاری *v_e)
{
	دوہرا lat_now_rad = math::radians(lat_now);
	دوہرا lon_now_rad = math::radians(lon_now);
	دوہرا lat_اگلا_rad = math::radians(lat_اگلا);
	دوہرا lon_اگلا_rad = math::radians(lon_اگلا);

	دوہرا d_lat = lat_اگلا_rad - lat_now_rad;
	دوہرا d_lon = lon_اگلا_rad - lon_now_rad;

	/* conscious mix of double and float trig function to maximize speed and efficiency */
	*v_n = static_cast<اعشاری>(CONSTANTS_RADIUS_کا_EARTH * d_lat);
	*v_e = static_cast<اعشاری>(CONSTANTS_RADIUS_کا_EARTH * d_lon * جیب_تمام(lat_now_rad));
}

خالی جوڑو_سمتیہ_مقصد_global_position(دوہرا lat_now, دوہرا lon_now, اعشاری v_n, اعشاری v_e, دوہرا *lat_res,
				   دوہرا *lon_res)
{
	دوہرا lat_now_rad = math::radians(lat_now);
	دوہرا lon_now_rad = math::radians(lon_now);

	*lat_res = math::degrees(lat_now_rad + (دوہرا)v_n / CONSTANTS_RADIUS_کا_EARTH);
	*lon_res = math::degrees(lon_now_rad + (دوہرا)v_e / (CONSTANTS_RADIUS_کا_EARTH * جیب_تمام(lat_now_rad)));
}

// Additional functions - @author Doug Weibel <douglas.weibel@colorado.edu>

صحیح get_فاصلہ_مقصد_سطر(ساخت crosstrack_error_s &crosstrack_error, دوہرا lat_now, دوہرا lon_now,
			 دوہرا lat_آغاز, دوہرا lon_آغاز, دوہرا lat_انتہا, دوہرا lon_انتہا)
{
	// This function returns the distance to the nearest point on the track line.  Distance is positive if current
	// position is right of the track and negative if left of the track as seen from a point on the track line
	// headed towards the end point.

	صحیح return_قدر = -1;	// Set error flag, cleared when valid result calculated.
	crosstrack_error.past_انتہا = جھوٹ;
	crosstrack_error.فاصلہ = 0.0f;
	crosstrack_error.bearing = 0.0f;

	اعشاری dist_مقصد_انتہا = get_فاصلہ_مقصد_اگلا_waypoint(lat_now, lon_now, lat_انتہا, lon_انتہا);

	// Return error if arguments are bad
	اگر (dist_مقصد_انتہا < 0.1f) {
		واپس -1;
	}

	اعشاری bearing_انتہا = get_bearing_مقصد_اگلا_waypoint(lat_now, lon_now, lat_انتہا, lon_انتہا);
	اعشاری bearing_track = get_bearing_مقصد_اگلا_waypoint(lat_آغاز, lon_آغاز, lat_انتہا, lon_انتہا);
	اعشاری bearing_diff = wrap_pi(bearing_track - bearing_انتہا);

	// Return past_end = true if past end point of line
	اگر (bearing_diff > M_PI_2_F || bearing_diff < -M_PI_2_F) {
		crosstrack_error.past_انتہا = سچ;
		return_قدر = 0;
		واپس return_قدر;
	}

	crosstrack_error.فاصلہ = (dist_مقصد_انتہا) * sinf(bearing_diff);

	اگر (sinf(bearing_diff) >= 0) {
		crosstrack_error.bearing = wrap_pi(bearing_track - M_PI_2_F);

	} ورنہ {
		crosstrack_error.bearing = wrap_pi(bearing_track + M_PI_2_F);
	}

	return_قدر = 0;

	واپس return_قدر;
}

صحیح get_فاصلہ_مقصد_arc(ساخت crosstrack_error_s *crosstrack_error, دوہرا lat_now, دوہرا lon_now,
			دوہرا lat_center, دوہرا lon_center,
			اعشاری radius, اعشاری arc_آغاز_bearing, اعشاری arc_sweep)
{
	// This function returns the distance to the nearest point on the track arc.  Distance is positive if current
	// position is right of the arc and negative if left of the arc as seen from the closest point on the arc and
	// headed towards the end point.

	// Determine if the current position is inside or outside the sector between the line from the center
	// to the arc start and the line from the center to the arc end
	ثابت اعشاری bearing_now = get_bearing_مقصد_اگلا_waypoint(lat_now, lon_now, lat_center, lon_center);

	صحیح return_قدر = -1;		// Set error flag, cleared when valid result calculated.
	crosstrack_error->past_انتہا = جھوٹ;
	crosstrack_error->فاصلہ = 0.0f;
	crosstrack_error->bearing = 0.0f;

	// Return error if arguments are bad
	اگر (radius < 0.1f) {
		واپس return_قدر;
	}

	// arc_start_bearing and arc_sweep are referenced to the center, so test membership with the
	// center->vehicle bearing. Measuring it in the direction of the sweep handles either sign and
	// any zero crossing without special cases.
	// NOLINTNEXTLINE(readability-suspicious-call-argument) intentional reverse bearing: center -> vehicle
	ثابت اعشاری bearing_center_مقصد_now = get_bearing_مقصد_اگلا_waypoint(lat_center, lon_center, lat_now, lon_now);
	ثابت اعشاری bearing_from_آغاز = wrap_2pi((bearing_center_مقصد_now - arc_آغاز_bearing) * (arc_sweep < 0.0f ? -1.0f :
					 1.0f));
	ثابت منطقی اندر_sector = bearing_from_آغاز <= fabsf(arc_sweep);

	// If in the sector then calculate distance and bearing to closest point
	اگر (اندر_sector) {
		crosstrack_error->past_انتہا = جھوٹ;
		اعشاری dist_مقصد_center = get_فاصلہ_مقصد_اگلا_waypoint(lat_now, lon_now, lat_center, lon_center);

		اگر (dist_مقصد_center <= radius) {
			crosstrack_error->فاصلہ = radius - dist_مقصد_center;
			crosstrack_error->bearing = bearing_now + M_PI_F;

		} ورنہ {
			crosstrack_error->فاصلہ = dist_مقصد_center - radius;
			crosstrack_error->bearing = bearing_now;
		}

		// If out of the sector then calculate dist and bearing to start or end point

	} ورنہ {

		// Use the approximation  that 111,111 meters in the y direction is 1 degree (of latitude)
		// and 111,111 * cos(latitude) meters in the x direction is 1 degree (of longitude) to
		// calculate the position of the start and end points.  We should not be doing this often
		// as this function generally will not be called repeatedly when we are out of the sector.

		دوہرا آغاز_disp_x = (دوہرا)radius * جیب((دوہرا)arc_آغاز_bearing);
		دوہرا آغاز_disp_y = (دوہرا)radius * جیب_تمام((دوہرا)arc_آغاز_bearing);
		دوہرا انتہا_disp_x = (دوہرا)radius * جیب((دوہرا)wrap_pi(arc_آغاز_bearing + arc_sweep));
		دوہرا انتہا_disp_y = (دوہرا)radius * جیب_تمام((دوہرا)wrap_pi(arc_آغاز_bearing + arc_sweep));
		ثابت دوہرا cos_lat_center = جیب_تمام(math::radians(lat_center));
		دوہرا lon_آغاز = lon_center + آغاز_disp_x / (111111.0 * cos_lat_center);
		دوہرا lat_آغاز = lat_center + آغاز_disp_y / 111111.0;
		دوہرا lon_انتہا = lon_center + انتہا_disp_x / (111111.0 * cos_lat_center);
		دوہرا lat_انتہا = lat_center + انتہا_disp_y / 111111.0;
		اعشاری dist_مقصد_آغاز = get_فاصلہ_مقصد_اگلا_waypoint(lat_now, lon_now, lat_آغاز, lon_آغاز);
		اعشاری dist_مقصد_انتہا = get_فاصلہ_مقصد_اگلا_waypoint(lat_now, lon_now, lat_انتہا, lon_انتہا);

		اگر (dist_مقصد_آغاز < dist_مقصد_انتہا) {
			crosstrack_error->فاصلہ = dist_مقصد_آغاز;
			crosstrack_error->bearing = get_bearing_مقصد_اگلا_waypoint(lat_now, lon_now, lat_آغاز, lon_آغاز);

		} ورنہ {
			crosstrack_error->past_انتہا = سچ;
			crosstrack_error->فاصلہ = dist_مقصد_انتہا;
			crosstrack_error->bearing = get_bearing_مقصد_اگلا_waypoint(lat_now, lon_now, lat_انتہا, lon_انتہا);
		}
	}

	crosstrack_error->bearing = wrap_pi(crosstrack_error->bearing);
	return_قدر = 0;

	واپس return_قدر;
}

اعشاری get_فاصلہ_مقصد_نقطہ_global_wgs84(دوہرا lat_now, دوہرا lon_now, اعشاری alt_now,
		دوہرا lat_اگلا, دوہرا lon_اگلا, اعشاری alt_اگلا,
		اعشاری *dist_xy, اعشاری *dist_z)
{
	دوہرا current_x_rad = math::radians(lat_اگلا);
	دوہرا current_y_rad = math::radians(lon_اگلا);
	دوہرا x_rad = math::radians(lat_now);
	دوہرا y_rad = math::radians(lon_now);

	دوہرا d_lat = x_rad - current_x_rad;
	دوہرا d_lon = y_rad - current_y_rad;

	دوہرا a = جیب(d_lat / 2.0) * جیب(d_lat / 2.0) + جیب(d_lon / 2.0) * جیب(d_lon / 2.0) * جیب_تمام(current_x_rad) * جیب_تمام(x_rad);
	دوہرا c = 2 * atan2(جذر(a), جذر(1 - a));

	ثابت اعشاری dxy = static_cast<اعشاری>(CONSTANTS_RADIUS_کا_EARTH * c);
	ثابت اعشاری dz = static_cast<اعشاری>(alt_now - alt_اگلا);

	*dist_xy = fabsf(dxy);
	*dist_z = fabsf(dz);

	واپس sqrtf(dxy * dxy + dz * dz);
}

اعشاری mavlink_wpm_فاصلہ_مقصد_نقطہ_local(اعشاری x_now, اعشاری y_now, اعشاری z_now,
		اعشاری x_اگلا, اعشاری y_اگلا, اعشاری z_اگلا,
		اعشاری *dist_xy, اعشاری *dist_z)
{
	اعشاری dx = x_now - x_اگلا;
	اعشاری dy = y_now - y_اگلا;
	اعشاری dz = z_now - z_اگلا;

	*dist_xy = sqrtf(dx * dx + dy * dy);
	*dist_z = fabsf(dz);

	واپس sqrtf(dx * dx + dy * dy + dz * dz);
}
