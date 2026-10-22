/****************************************************************************
 *
 *   Copyright (C) 2012-2021 PX4 Development Team. All rights reserved.
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
 * @file geo.h
 *
 * Definition of geo / math functions to perform geodesic calculations
 *
 * @author Thomas Gubler <thomasgubler@student.ethz.ch>
 * @author Julian Oes <joes@student.ethz.ch>
 * @author Lorenz Meier <lm@inf.ethz.ch>
 * @author Anton Babushkin <anton.babushkin@me.com>
 * Additional functions - @author Doug Weibel <douglas.weibel@colorado.edu>
 */

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <stdbool.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <lib/mathlib/mathlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/matrix/matrix/math.hpp>

ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ CONSTANTS_ONE_G = 9.80665f;						// m/s^2
ਸਥਿਰ constexpr ਦੁੱਗਣਾ CONSTANTS_RADIUS_ਦਾ_EARTH = 6371000;					// meters (m)
ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ  CONSTANTS_RADIUS_ਦਾ_EARTH_F = CONSTANTS_RADIUS_ਦਾ_EARTH;		// meters (m)
ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ CONSTANTS_EARTH_SPIN_ਦਰ = 7.2921150e-5f;				// radians/second (rad/s)


// XXX remove
ਬਣਤਰ crosstrack_error_s {
	bool past_ਸਿਰਾ;		// Flag indicating we are past the end of the line/arc segment
	ਦਸ਼ਮਲਵ ਦੂਰੀ;		// Distance in meters to closest point on line/arc
	ਦਸ਼ਮਲਵ bearing;		// Bearing in radians to closest point on line/arc
} ;


/**
 * Returns the distance to the next waypoint in meters.
 *
 * @param lat_now current position in degrees (47.1234567°, not 471234567°)
 * @param lon_now current position in degrees (8.1234567°, not 81234567°)
 * @param lat_next next waypoint position in degrees (47.1234567°, not 471234567°)
 * @param lon_next next waypoint position in degrees (8.1234567°, not 81234567°)
 */
ਦਸ਼ਮਲਵ get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(ਦੁੱਗਣਾ lat_now, ਦੁੱਗਣਾ lon_now, ਦੁੱਗਣਾ lat_ਅਗਲਾ, ਦੁੱਗਣਾ lon_ਅਗਲਾ);

/**
 * Creates a new waypoint C on the line of two given waypoints (A, B) at certain distance
 * from waypoint A
 *
 * @param lat_A waypoint A latitude in degrees (47.1234567°, not 471234567°)
 * @param lon_A waypoint A longitude in degrees (8.1234567°, not 81234567°)
 * @param lat_B waypoint B latitude in degrees (47.1234567°, not 471234567°)
 * @param lon_B waypoint B longitude in degrees (8.1234567°, not 81234567°)
 * @param dist distance of target waypoint from waypoint A in meters (can be negative)
 * @param lat_target latitude of target waypoint C in degrees (47.1234567°, not 471234567°)
 * @param lon_target longitude of target waypoint C in degrees (47.1234567°, not 471234567°)
 */
ਖਾਲੀ create_waypoint_from_ਸਤਰ_and_dist(ਦੁੱਗਣਾ lat_A, ਦੁੱਗਣਾ lon_A, ਦੁੱਗਣਾ lat_B, ਦੁੱਗਣਾ lon_B, ਦਸ਼ਮਲਵ dist,
					ਦੁੱਗਣਾ *lat_ਨਿਸ਼ਾਨਾ, ਦੁੱਗਣਾ *lon_ਨਿਸ਼ਾਨਾ);

/**
 * Creates a waypoint from given waypoint, distance and bearing
 * see http://www.movable-type.co.uk/scripts/latlong.html
 *
 * @param lat_start latitude of starting waypoint in degrees (47.1234567°, not 471234567°)
 * @param lon_start longitude of starting waypoint in degrees (8.1234567°, not 81234567°)
 * @param bearing in rad
 * @param distance in meters
 * @param lat_target latitude of target waypoint in degrees (47.1234567°, not 471234567°)
 * @param lon_target longitude of target waypoint in degrees (47.1234567°, not 471234567°)
 */
ਖਾਲੀ waypoint_from_heading_and_ਦੂਰੀ(ਦੁੱਗਣਾ lat_ਸ਼ੁਰੂ, ਦੁੱਗਣਾ lon_ਸ਼ੁਰੂ, ਦਸ਼ਮਲਵ bearing, ਦਸ਼ਮਲਵ dist,
					ਦੁੱਗਣਾ *lat_ਨਿਸ਼ਾਨਾ, ਦੁੱਗਣਾ *lon_ਨਿਸ਼ਾਨਾ);

/**
 * Returns the bearing to the next waypoint in radians.
 *
 * @param lat_now current position in degrees (47.1234567°, not 471234567°)
 * @param lon_now current position in degrees (8.1234567°, not 81234567°)
 * @param lat_next next waypoint position in degrees (47.1234567°, not 471234567°)
 * @param lon_next next waypoint position in degrees (8.1234567°, not 81234567°)
 */
ਦਸ਼ਮਲਵ get_bearing_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(ਦੁੱਗਣਾ lat_now, ਦੁੱਗਣਾ lon_now, ਦੁੱਗਣਾ lat_ਅਗਲਾ, ਦੁੱਗਣਾ lon_ਅਗਲਾ);

ਖਾਲੀ get_ਸਦਿਸ਼_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(ਦੁੱਗਣਾ lat_now, ਦੁੱਗਣਾ lon_now, ਦੁੱਗਣਾ lat_ਅਗਲਾ, ਦੁੱਗਣਾ lon_ਅਗਲਾ, ਦਸ਼ਮਲਵ *v_n,
				 ਦਸ਼ਮਲਵ *v_e);

ਖਾਲੀ get_ਸਦਿਸ਼_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint_fast(ਦੁੱਗਣਾ lat_now, ਦੁੱਗਣਾ lon_now, ਦੁੱਗਣਾ lat_ਅਗਲਾ, ਦੁੱਗਣਾ lon_ਅਗਲਾ, ਦਸ਼ਮਲਵ *v_n,
				      ਦਸ਼ਮਲਵ *v_e);

ਖਾਲੀ ਜੋੜੋ_ਸਦਿਸ਼_ਮੰਜ਼ਿਲ_global_position(ਦੁੱਗਣਾ lat_now, ਦੁੱਗਣਾ lon_now, ਦਸ਼ਮਲਵ v_n, ਦਸ਼ਮਲਵ v_e, ਦੁੱਗਣਾ *lat_res,
				   ਦੁੱਗਣਾ *lon_res);

ਪੂਰਨ get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਸਤਰ(ਬਣਤਰ crosstrack_error_s &crosstrack_error, ਦੁੱਗਣਾ lat_now, ਦੁੱਗਣਾ lon_now,
			 ਦੁੱਗਣਾ lat_ਸ਼ੁਰੂ, ਦੁੱਗਣਾ lon_ਸ਼ੁਰੂ, ਦੁੱਗਣਾ lat_ਸਿਰਾ, ਦੁੱਗਣਾ lon_ਸਿਰਾ);

ਪੂਰਨ get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_arc(ਬਣਤਰ crosstrack_error_s *crosstrack_error, ਦੁੱਗਣਾ lat_now, ਦੁੱਗਣਾ lon_now,
			ਦੁੱਗਣਾ lat_center, ਦੁੱਗਣਾ lon_center,
			ਦਸ਼ਮਲਵ radius, ਦਸ਼ਮਲਵ arc_ਸ਼ੁਰੂ_bearing, ਦਸ਼ਮਲਵ arc_sweep);

/*
 * Calculate distance in global frame
 */
ਦਸ਼ਮਲਵ get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਬਿੰਦੂ_global_wgs84(ਦੁੱਗਣਾ lat_now, ਦੁੱਗਣਾ lon_now, ਦਸ਼ਮਲਵ alt_now,
		ਦੁੱਗਣਾ lat_ਅਗਲਾ, ਦੁੱਗਣਾ lon_ਅਗਲਾ, ਦਸ਼ਮਲਵ alt_ਅਗਲਾ,
		ਦਸ਼ਮਲਵ *dist_xy, ਦਸ਼ਮਲਵ *dist_z);

/*
 * Calculate distance in local frame (NED)
 */
ਦਸ਼ਮਲਵ mavlink_wpm_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਬਿੰਦੂ_local(ਦਸ਼ਮਲਵ x_now, ਦਸ਼ਮਲਵ y_now, ਦਸ਼ਮਲਵ z_now,
		ਦਸ਼ਮਲਵ x_ਅਗਲਾ, ਦਸ਼ਮਲਵ y_ਅਗਲਾ, ਦਸ਼ਮਲਵ z_ਅਗਲਾ,
		ਦਸ਼ਮਲਵ *dist_xy, ਦਸ਼ਮਲਵ *dist_z);


/**
 * @brief C++ class for mapping lat/lon coordinates to local coordinated using a reference position
 */
class MapProjection final
{
ਨਿਜੀ:
	uint64_t _ref_timestamp{0};
	ਦੁੱਗਣਾ _ref_lat{0.0};
	ਦੁੱਗਣਾ _ref_lon{0.0};
	ਦੁੱਗਣਾ _ref_sin_lat{0.0};
	ਦੁੱਗਣਾ _ref_cos_lat{0.0};
	bool ref_init_ਪੂਰਾ{false};

ਜਨਤਕ:
	/**
	 * @brief Construct a new Map Projection object
	 * The generated object will be uninitialized.
	 * To initialize, use the `initReference` function
	 */
	MapProjection() = ਮੂਲ;

	/**
	 * @brief Construct and initialize a new Map Projection object
	 */
	MapProjection(ਦੁੱਗਣਾ lat_0, ਦੁੱਗਣਾ lon_0, uint64_t timestamp = 0)
	{
		initReference(lat_0, lon_0, timestamp);
	}

	/**
	 * Initialize the map transformation
	 *
	 * Initializes the transformation between the geographic coordinate system and
	 * the azimuthal equidistant plane
	 * @param lat in degrees (47.1234567°, not 471234567°)
	 * @param lon in degrees (8.1234567°, not 81234567°)
	 */
	ਖਾਲੀ initReference(ਦੁੱਗਣਾ lat_0, ਦੁੱਗਣਾ lon_0, uint64_t timestamp = 0);

	/**
	 * @return true, if the map reference has been initialized before
	 */
	bool isInitialized() ਅਚਲ { ਵਾਪਸ ref_init_ਪੂਰਾ; };

	/**
	 * @return the timestamp of the reference which the map projection was initialized with
	 */
	uint64_t getProjectionReferenceTimestamp() ਅਚਲ { ਵਾਪਸ _ref_timestamp; };

	/**
	 * @return the projection reference latitude in degrees
	 */
	ਦੁੱਗਣਾ getProjectionReferenceLat() ਅਚਲ { ਵਾਪਸ math::degrees(_ref_lat); };

	/**
	 * @return the projection reference longitude in degrees
	 */
	ਦੁੱਗਣਾ getProjectionReferenceLon() ਅਚਲ { ਵਾਪਸ math::degrees(_ref_lon); };

	/**
	 * Transform a point in the geographic coordinate system to the local
	 * azimuthal equidistant plane using the projection
	 * @param lat in degrees (47.1234567°, not 471234567°)
	 * @param lon in degrees (8.1234567°, not 81234567°)
	 * @param x north
	 * @param y east
	 */
	ਖਾਲੀ project(ਦੁੱਗਣਾ lat, ਦੁੱਗਣਾ lon, ਦਸ਼ਮਲਵ &x, ਦਸ਼ਮਲਵ &y) ਅਚਲ;

	/**
	 * Transform a point in the geographic coordinate system to the local
	 * azimuthal equidistant plane using the projection
	 * @param lat in degrees (47.1234567°, not 471234567°)
	 * @param lon in degrees (8.1234567°, not 81234567°)
	 * @return the point in local coordinates as north / east
	 */
	ਸਤਰਅੰਦਰ ਮੈਟ੍ਰਿਕਸ::Vector2f project(ਦੁੱਗਣਾ lat, ਦੁੱਗਣਾ lon) ਅਚਲ
	{
		ਮੈਟ੍ਰਿਕਸ::Vector2f res;
		project(lat, lon, res(0), res(1));
		ਵਾਪਸ res;
	}

	/**
	 * Transform a point in the local azimuthal equidistant plane to the
	 * geographic coordinate system using the projection
	 *
	 * @param x north
	 * @param y east
	 * @param lat in degrees (47.1234567°, not 471234567°)
	 * @param lon in degrees (8.1234567°, not 81234567°)
	 */
	ਖਾਲੀ reproject(ਦਸ਼ਮਲਵ x, ਦਸ਼ਮਲਵ y, ਦੁੱਗਣਾ &lat, ਦੁੱਗਣਾ &lon) ਅਚਲ;
};
