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

#pragma once

#ಅನ್ತರ್ಭಾವ <stdbool.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>

#ಅನ್ತರ್ಭಾವ <lib/mathlib/mathlib.h>
#ಅನ್ತರ್ಭಾವ <lib/matrix/matrix/math.hpp>

ಸ್ಥಿರ constexpr ಭಿನ್ನ CONSTANTS_ONE_G = 9.80665f;						// m/s^2
ಸ್ಥಿರ constexpr ದ್ವಿಗುಣ CONSTANTS_RADIUS_OF_EARTH = 6371000;					// meters (m)
ಸ್ಥಿರ constexpr ಭಿನ್ನ  CONSTANTS_RADIUS_OF_EARTH_F = CONSTANTS_RADIUS_OF_EARTH;		// meters (m)
ಸ್ಥಿರ constexpr ಭಿನ್ನ CONSTANTS_EARTH_SPIN_RATE = 7.2921150e-5f;				// radians/second (rad/s)


// XXX remove
ಸಂರಚನಾ crosstrack_error_s {
	bool past_end;		// Flag indicating we are past the end of the line/arc segment
	ಭಿನ್ನ distance;		// Distance in meters to closest point on line/arc
	ಭಿನ್ನ bearing;		// Bearing in radians to closest point on line/arc
} ;


/**
 * Returns the distance to the next waypoint in meters.
 *
 * @param lat_now current position in degrees (47.1234567°, not 471234567°)
 * @param lon_now current position in degrees (8.1234567°, not 81234567°)
 * @param lat_next next waypoint position in degrees (47.1234567°, not 471234567°)
 * @param lon_next next waypoint position in degrees (8.1234567°, not 81234567°)
 */
ಭಿನ್ನ get_distance_to_next_waypoint(ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now, ದ್ವಿಗುಣ lat_next, ದ್ವಿಗುಣ lon_next);

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
ಶೂನ್ಯ create_waypoint_from_ಸಾಲು_and_dist(ದ್ವಿಗುಣ lat_A, ದ್ವಿಗುಣ lon_A, ದ್ವಿಗುಣ lat_B, ದ್ವಿಗುಣ lon_B, ಭಿನ್ನ dist,
					ದ್ವಿಗುಣ *lat_target, ದ್ವಿಗುಣ *lon_target);

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
ಶೂನ್ಯ waypoint_from_heading_and_distance(ದ್ವಿಗುಣ lat_start, ದ್ವಿಗುಣ lon_start, ಭಿನ್ನ bearing, ಭಿನ್ನ dist,
					ದ್ವಿಗುಣ *lat_target, ದ್ವಿಗುಣ *lon_target);

/**
 * Returns the bearing to the next waypoint in radians.
 *
 * @param lat_now current position in degrees (47.1234567°, not 471234567°)
 * @param lon_now current position in degrees (8.1234567°, not 81234567°)
 * @param lat_next next waypoint position in degrees (47.1234567°, not 471234567°)
 * @param lon_next next waypoint position in degrees (8.1234567°, not 81234567°)
 */
ಭಿನ್ನ get_bearing_to_next_waypoint(ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now, ದ್ವಿಗುಣ lat_next, ದ್ವಿಗುಣ lon_next);

ಶೂನ್ಯ get_vector_to_next_waypoint(ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now, ದ್ವಿಗುಣ lat_next, ದ್ವಿಗುಣ lon_next, ಭಿನ್ನ *v_n,
				 ಭಿನ್ನ *v_e);

ಶೂನ್ಯ get_vector_to_next_waypoint_fast(ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now, ದ್ವಿಗುಣ lat_next, ದ್ವಿಗುಣ lon_next, ಭಿನ್ನ *v_n,
				      ಭಿನ್ನ *v_e);

ಶೂನ್ಯ add_vector_to_global_position(ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now, ಭಿನ್ನ v_n, ಭಿನ್ನ v_e, ದ್ವಿಗುಣ *lat_res,
				   ದ್ವಿಗುಣ *lon_res);

ಪೂರ್ಣಾಙ್ಕ get_distance_to_ಸಾಲು(ಸಂರಚನಾ crosstrack_error_s &crosstrack_error, ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now,
			 ದ್ವಿಗುಣ lat_start, ದ್ವಿಗುಣ lon_start, ದ್ವಿಗುಣ lat_end, ದ್ವಿಗುಣ lon_end);

ಪೂರ್ಣಾಙ್ಕ get_distance_to_arc(ಸಂರಚನಾ crosstrack_error_s *crosstrack_error, ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now,
			ದ್ವಿಗುಣ lat_center, ದ್ವಿಗುಣ lon_center,
			ಭಿನ್ನ radius, ಭಿನ್ನ arc_start_bearing, ಭಿನ್ನ arc_sweep);

/*
 * Calculate distance in global frame
 */
ಭಿನ್ನ get_distance_to_point_global_wgs84(ದ್ವಿಗುಣ lat_now, ದ್ವಿಗುಣ lon_now, ಭಿನ್ನ alt_now,
		ದ್ವಿಗುಣ lat_next, ದ್ವಿಗುಣ lon_next, ಭಿನ್ನ alt_next,
		ಭಿನ್ನ *dist_xy, ಭಿನ್ನ *dist_z);

/*
 * Calculate distance in local frame (NED)
 */
ಭಿನ್ನ mavlink_wpm_distance_to_point_local(ಭಿನ್ನ x_now, ಭಿನ್ನ y_now, ಭಿನ್ನ z_now,
		ಭಿನ್ನ x_next, ಭಿನ್ನ y_next, ಭಿನ್ನ z_next,
		ಭಿನ್ನ *dist_xy, ಭಿನ್ನ *dist_z);


/**
 * @brief C++ class for mapping lat/lon coordinates to local coordinated using a reference position
 */
class MapProjection final
{
ಗುಪ್ತ:
	uint64_t _ref_timestamp{0};
	ದ್ವಿಗುಣ _ref_lat{0.0};
	ದ್ವಿಗುಣ _ref_lon{0.0};
	ದ್ವಿಗುಣ _ref_sin_lat{0.0};
	ದ್ವಿಗುಣ _ref_cos_lat{0.0};
	bool _ref_init_done{false};

ಸಾರ್ವಜನಿಕ:
	/**
	 * @brief Construct a new Map Projection object
	 * The generated object will be uninitialized.
	 * To initialize, use the `initReference` function
	 */
	MapProjection() = ಸ್ವಭಾವ;

	/**
	 * @brief Construct and initialize a new Map Projection object
	 */
	MapProjection(ದ್ವಿಗುಣ lat_0, ದ್ವಿಗುಣ lon_0, uint64_t timestamp = 0)
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
	ಶೂನ್ಯ initReference(ದ್ವಿಗುಣ lat_0, ದ್ವಿಗುಣ lon_0, uint64_t timestamp = 0);

	/**
	 * @return true, if the map reference has been initialized before
	 */
	bool isInitialized() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _ref_init_done; };

	/**
	 * @return the timestamp of the reference which the map projection was initialized with
	 */
	uint64_t getProjectionReferenceTimestamp() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _ref_timestamp; };

	/**
	 * @return the projection reference latitude in degrees
	 */
	ದ್ವಿಗುಣ getProjectionReferenceLat() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ math::degrees(_ref_lat); };

	/**
	 * @return the projection reference longitude in degrees
	 */
	ದ್ವಿಗುಣ getProjectionReferenceLon() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ math::degrees(_ref_lon); };

	/**
	 * Transform a point in the geographic coordinate system to the local
	 * azimuthal equidistant plane using the projection
	 * @param lat in degrees (47.1234567°, not 471234567°)
	 * @param lon in degrees (8.1234567°, not 81234567°)
	 * @param x north
	 * @param y east
	 */
	ಶೂನ್ಯ project(ದ್ವಿಗುಣ lat, ದ್ವಿಗುಣ lon, ಭಿನ್ನ &x, ಭಿನ್ನ &y) ಧ್ರುವ;

	/**
	 * Transform a point in the geographic coordinate system to the local
	 * azimuthal equidistant plane using the projection
	 * @param lat in degrees (47.1234567°, not 471234567°)
	 * @param lon in degrees (8.1234567°, not 81234567°)
	 * @return the point in local coordinates as north / east
	 */
	ಅನ್ತಃಸ್ಥ matrix::Vector2f project(ದ್ವಿಗುಣ lat, ದ್ವಿಗುಣ lon) ಧ್ರುವ
	{
		matrix::Vector2f res;
		project(lat, lon, res(0), res(1));
		ಪ್ರತ್ಯಾಗಮ res;
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
	ಶೂನ್ಯ reproject(ಭಿನ್ನ x, ಭಿನ್ನ y, ದ್ವಿಗುಣ &lat, ದ್ವಿಗುಣ &lon) ಧ್ರುವ;
};
