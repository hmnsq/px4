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

#ଅନ୍ତର୍ଭାଵ <stdbool.h>
#ଅନ୍ତର୍ଭାଵ <stdint.h>

#ଅନ୍ତର୍ଭାଵ <lib/mathlib/mathlib.h>
#ଅନ୍ତର୍ଭାଵ <lib/matrix/matrix/math.hpp>

ସ୍ଥିର constexpr ଦଶମିକ CONSTANTS_ONE_G = 9.80665f;						// m/s^2
ସ୍ଥିର constexpr ଦ୍ୱିଗୁଣ CONSTANTS_RADIUS_OF_EARTH = 6371000;					// meters (m)
ସ୍ଥିର constexpr ଦଶମିକ  CONSTANTS_RADIUS_OF_EARTH_F = CONSTANTS_RADIUS_OF_EARTH;		// meters (m)
ସ୍ଥିର constexpr ଦଶମିକ CONSTANTS_EARTH_SPIN_RATE = 7.2921150e-5f;				// radians/second (rad/s)


// XXX remove
ସଂରଚନା crosstrack_error_s {
	bool past_end;		// Flag indicating we are past the end of the line/arc segment
	ଦଶମିକ distance;		// Distance in meters to closest point on line/arc
	ଦଶମିକ bearing;		// Bearing in radians to closest point on line/arc
} ;


/**
 * Returns the distance to the next waypoint in meters.
 *
 * @param lat_now current position in degrees (47.1234567°, not 471234567°)
 * @param lon_now current position in degrees (8.1234567°, not 81234567°)
 * @param lat_next next waypoint position in degrees (47.1234567°, not 471234567°)
 * @param lon_next next waypoint position in degrees (8.1234567°, not 81234567°)
 */
ଦଶମିକ get_distance_to_next_waypoint(ଦ୍ୱିଗୁଣ lat_now, ଦ୍ୱିଗୁଣ lon_now, ଦ୍ୱିଗୁଣ lat_next, ଦ୍ୱିଗୁଣ lon_next);

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
ଶୂନ୍ୟ create_waypoint_from_ଧାଡ଼ି_and_dist(ଦ୍ୱିଗୁଣ lat_A, ଦ୍ୱିଗୁଣ lon_A, ଦ୍ୱିଗୁଣ lat_B, ଦ୍ୱିଗୁଣ lon_B, ଦଶମିକ dist,
					ଦ୍ୱିଗୁଣ *lat_target, ଦ୍ୱିଗୁଣ *lon_target);

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
ଶୂନ୍ୟ waypoint_from_heading_and_distance(ଦ୍ୱିଗୁଣ lat_start, ଦ୍ୱିଗୁଣ lon_start, ଦଶମିକ bearing, ଦଶମିକ dist,
					ଦ୍ୱିଗୁଣ *lat_target, ଦ୍ୱିଗୁଣ *lon_target);

/**
 * Returns the bearing to the next waypoint in radians.
 *
 * @param lat_now current position in degrees (47.1234567°, not 471234567°)
 * @param lon_now current position in degrees (8.1234567°, not 81234567°)
 * @param lat_next next waypoint position in degrees (47.1234567°, not 471234567°)
 * @param lon_next next waypoint position in degrees (8.1234567°, not 81234567°)
 */
ଦଶମିକ get_bearing_to_next_waypoint(ଦ୍ୱିଗୁଣ lat_now, ଦ୍ୱିଗୁଣ lon_now, ଦ୍ୱିଗୁଣ lat_next, ଦ୍ୱିଗୁଣ lon_next);

ଶୂନ୍ୟ get_vector_to_next_waypoint(ଦ୍ୱିଗୁଣ lat_now, ଦ୍ୱିଗୁଣ lon_now, ଦ୍ୱିଗୁଣ lat_next, ଦ୍ୱିଗୁଣ lon_next, ଦଶମିକ *v_n,
				 ଦଶମିକ *v_e);

ଶୂନ୍ୟ get_vector_to_next_waypoint_fast(ଦ୍ୱିଗୁଣ lat_now, ଦ୍ୱିଗୁଣ lon_now, ଦ୍ୱିଗୁଣ lat_next, ଦ୍ୱିଗୁଣ lon_next, ଦଶମିକ *v_n,
				      ଦଶମିକ *v_e);

ଶୂନ୍ୟ add_vector_to_global_position(ଦ୍ୱିଗୁଣ lat_now, ଦ୍ୱିଗୁଣ lon_now, ଦଶମିକ v_n, ଦଶମିକ v_e, ଦ୍ୱିଗୁଣ *lat_res,
				   ଦ୍ୱିଗୁଣ *lon_res);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_distance_to_ଧାଡ଼ି(ସଂରଚନା crosstrack_error_s &crosstrack_error, ଦ୍ୱିଗୁଣ lat_now, ଦ୍ୱିଗୁଣ lon_now,
			 ଦ୍ୱିଗୁଣ lat_start, ଦ୍ୱିଗୁଣ lon_start, ଦ୍ୱିଗୁଣ lat_end, ଦ୍ୱିଗୁଣ lon_end);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_distance_to_arc(ସଂରଚନା crosstrack_error_s *crosstrack_error, ଦ୍ୱିଗୁଣ lat_now, ଦ୍ୱିଗୁଣ lon_now,
			ଦ୍ୱିଗୁଣ lat_center, ଦ୍ୱିଗୁଣ lon_center,
			ଦଶମିକ radius, ଦଶମିକ arc_start_bearing, ଦଶମିକ arc_sweep);

/*
 * Calculate distance in global frame
 */
ଦଶମିକ get_distance_to_point_global_wgs84(ଦ୍ୱିଗୁଣ lat_now, ଦ୍ୱିଗୁଣ lon_now, ଦଶମିକ alt_now,
		ଦ୍ୱିଗୁଣ lat_next, ଦ୍ୱିଗୁଣ lon_next, ଦଶମିକ alt_next,
		ଦଶମିକ *dist_xy, ଦଶମିକ *dist_z);

/*
 * Calculate distance in local frame (NED)
 */
ଦଶମିକ mavlink_wpm_distance_to_point_local(ଦଶମିକ x_now, ଦଶମିକ y_now, ଦଶମିକ z_now,
		ଦଶମିକ x_next, ଦଶମିକ y_next, ଦଶମିକ z_next,
		ଦଶମିକ *dist_xy, ଦଶମିକ *dist_z);


/**
 * @brief C++ class for mapping lat/lon coordinates to local coordinated using a reference position
 */
class MapProjection final
{
ବ୍ୟକ୍ତିଗତ:
	uint64_t _ref_timestamp{0};
	ଦ୍ୱିଗୁଣ _ref_lat{0.0};
	ଦ୍ୱିଗୁଣ _ref_lon{0.0};
	ଦ୍ୱିଗୁଣ _ref_sin_lat{0.0};
	ଦ୍ୱିଗୁଣ _ref_cos_lat{0.0};
	bool _ref_init_done{false};

ସର୍ବସାଧାରଣ:
	/**
	 * @brief Construct a new Map Projection object
	 * The generated object will be uninitialized.
	 * To initialize, use the `initReference` function
	 */
	MapProjection() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	/**
	 * @brief Construct and initialize a new Map Projection object
	 */
	MapProjection(ଦ୍ୱିଗୁଣ lat_0, ଦ୍ୱିଗୁଣ lon_0, uint64_t timestamp = 0)
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
	ଶୂନ୍ୟ initReference(ଦ୍ୱିଗୁଣ lat_0, ଦ୍ୱିଗୁଣ lon_0, uint64_t timestamp = 0);

	/**
	 * @return true, if the map reference has been initialized before
	 */
	bool isInitialized() ଅଚଳ { ଫେରନ୍ତୁ _ref_init_done; };

	/**
	 * @return the timestamp of the reference which the map projection was initialized with
	 */
	uint64_t getProjectionReferenceTimestamp() ଅଚଳ { ଫେରନ୍ତୁ _ref_timestamp; };

	/**
	 * @return the projection reference latitude in degrees
	 */
	ଦ୍ୱିଗୁଣ getProjectionReferenceLat() ଅଚଳ { ଫେରନ୍ତୁ math::degrees(_ref_lat); };

	/**
	 * @return the projection reference longitude in degrees
	 */
	ଦ୍ୱିଗୁଣ getProjectionReferenceLon() ଅଚଳ { ଫେରନ୍ତୁ math::degrees(_ref_lon); };

	/**
	 * Transform a point in the geographic coordinate system to the local
	 * azimuthal equidistant plane using the projection
	 * @param lat in degrees (47.1234567°, not 471234567°)
	 * @param lon in degrees (8.1234567°, not 81234567°)
	 * @param x north
	 * @param y east
	 */
	ଶୂନ୍ୟ project(ଦ୍ୱିଗୁଣ lat, ଦ୍ୱିଗୁଣ lon, ଦଶମିକ &x, ଦଶମିକ &y) ଅଚଳ;

	/**
	 * Transform a point in the geographic coordinate system to the local
	 * azimuthal equidistant plane using the projection
	 * @param lat in degrees (47.1234567°, not 471234567°)
	 * @param lon in degrees (8.1234567°, not 81234567°)
	 * @return the point in local coordinates as north / east
	 */
	ଧାଡ଼ିଭିତରେ matrix::Vector2f project(ଦ୍ୱିଗୁଣ lat, ଦ୍ୱିଗୁଣ lon) ଅଚଳ
	{
		matrix::Vector2f res;
		project(lat, lon, res(0), res(1));
		ଫେରନ୍ତୁ res;
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
	ଶୂନ୍ୟ reproject(ଦଶମିକ x, ଦଶମିକ y, ଦ୍ୱିଗୁଣ &lat, ଦ୍ୱିଗୁଣ &lon) ଅଚଳ;
};
