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

#شامل <stdbool.h>
#شامل <stdint.h>

#شامل <lib/mathlib/mathlib.h>
#شامل <lib/matrix/matrix/math.hpp>

ایستا constexpr اعشاری CONSTANTS_ONE_G = 9.80665f;						// m/s^2
ایستا constexpr دوگانه CONSTANTS_RADIUS_OF_EARTH = 6371000;					// meters (m)
ایستا constexpr اعشاری  CONSTANTS_RADIUS_OF_EARTH_F = CONSTANTS_RADIUS_OF_EARTH;		// meters (m)
ایستا constexpr اعشاری CONSTANTS_EARTH_SPIN_RATE = 7.2921150e-5f;				// radians/second (rad/s)


// XXX remove
ساختار crosstrack_error_s {
	bool past_end;		// Flag indicating we are past the end of the line/arc segment
	اعشاری distance;		// Distance in meters to closest point on line/arc
	اعشاری bearing;		// Bearing in radians to closest point on line/arc
} ;


/**
 * Returns the distance to the next waypoint in meters.
 *
 * @param lat_now current position in degrees (47.1234567°, not 471234567°)
 * @param lon_now current position in degrees (8.1234567°, not 81234567°)
 * @param lat_next next waypoint position in degrees (47.1234567°, not 471234567°)
 * @param lon_next next waypoint position in degrees (8.1234567°, not 81234567°)
 */
اعشاری get_distance_to_next_waypoint(دوگانه lat_now, دوگانه lon_now, دوگانه lat_next, دوگانه lon_next);

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
تهی create_waypoint_from_سطر_and_dist(دوگانه lat_A, دوگانه lon_A, دوگانه lat_B, دوگانه lon_B, اعشاری dist,
					دوگانه *lat_target, دوگانه *lon_target);

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
تهی waypoint_from_heading_and_distance(دوگانه lat_start, دوگانه lon_start, اعشاری bearing, اعشاری dist,
					دوگانه *lat_target, دوگانه *lon_target);

/**
 * Returns the bearing to the next waypoint in radians.
 *
 * @param lat_now current position in degrees (47.1234567°, not 471234567°)
 * @param lon_now current position in degrees (8.1234567°, not 81234567°)
 * @param lat_next next waypoint position in degrees (47.1234567°, not 471234567°)
 * @param lon_next next waypoint position in degrees (8.1234567°, not 81234567°)
 */
اعشاری get_bearing_to_next_waypoint(دوگانه lat_now, دوگانه lon_now, دوگانه lat_next, دوگانه lon_next);

تهی get_vector_to_next_waypoint(دوگانه lat_now, دوگانه lon_now, دوگانه lat_next, دوگانه lon_next, اعشاری *v_n,
				 اعشاری *v_e);

تهی get_vector_to_next_waypoint_fast(دوگانه lat_now, دوگانه lon_now, دوگانه lat_next, دوگانه lon_next, اعشاری *v_n,
				      اعشاری *v_e);

تهی add_vector_to_global_position(دوگانه lat_now, دوگانه lon_now, اعشاری v_n, اعشاری v_e, دوگانه *lat_res,
				   دوگانه *lon_res);

صحیح get_distance_to_سطر(ساختار crosstrack_error_s &crosstrack_error, دوگانه lat_now, دوگانه lon_now,
			 دوگانه lat_start, دوگانه lon_start, دوگانه lat_end, دوگانه lon_end);

صحیح get_distance_to_arc(ساختار crosstrack_error_s *crosstrack_error, دوگانه lat_now, دوگانه lon_now,
			دوگانه lat_center, دوگانه lon_center,
			اعشاری radius, اعشاری arc_start_bearing, اعشاری arc_sweep);

/*
 * Calculate distance in global frame
 */
اعشاری get_distance_to_point_global_wgs84(دوگانه lat_now, دوگانه lon_now, اعشاری alt_now,
		دوگانه lat_next, دوگانه lon_next, اعشاری alt_next,
		اعشاری *dist_xy, اعشاری *dist_z);

/*
 * Calculate distance in local frame (NED)
 */
اعشاری mavlink_wpm_distance_to_point_local(اعشاری x_now, اعشاری y_now, اعشاری z_now,
		اعشاری x_next, اعشاری y_next, اعشاری z_next,
		اعشاری *dist_xy, اعشاری *dist_z);


/**
 * @brief C++ class for mapping lat/lon coordinates to local coordinated using a reference position
 */
class MapProjection final
{
خصوصی:
	uint64_t _ref_timestamp{0};
	دوگانه _ref_lat{0.0};
	دوگانه _ref_lon{0.0};
	دوگانه _ref_sin_lat{0.0};
	دوگانه _ref_cos_lat{0.0};
	bool _ref_init_done{false};

عمومی:
	/**
	 * @brief Construct a new Map Projection object
	 * The generated object will be uninitialized.
	 * To initialize, use the `initReference` function
	 */
	MapProjection() = پیشفرض;

	/**
	 * @brief Construct and initialize a new Map Projection object
	 */
	MapProjection(دوگانه lat_0, دوگانه lon_0, uint64_t timestamp = 0)
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
	تهی initReference(دوگانه lat_0, دوگانه lon_0, uint64_t timestamp = 0);

	/**
	 * @return true, if the map reference has been initialized before
	 */
	bool isInitialized() ثابت { بازگشت _ref_init_done; };

	/**
	 * @return the timestamp of the reference which the map projection was initialized with
	 */
	uint64_t getProjectionReferenceTimestamp() ثابت { بازگشت _ref_timestamp; };

	/**
	 * @return the projection reference latitude in degrees
	 */
	دوگانه getProjectionReferenceLat() ثابت { بازگشت math::degrees(_ref_lat); };

	/**
	 * @return the projection reference longitude in degrees
	 */
	دوگانه getProjectionReferenceLon() ثابت { بازگشت math::degrees(_ref_lon); };

	/**
	 * Transform a point in the geographic coordinate system to the local
	 * azimuthal equidistant plane using the projection
	 * @param lat in degrees (47.1234567°, not 471234567°)
	 * @param lon in degrees (8.1234567°, not 81234567°)
	 * @param x north
	 * @param y east
	 */
	تهی project(دوگانه lat, دوگانه lon, اعشاری &x, اعشاری &y) ثابت;

	/**
	 * Transform a point in the geographic coordinate system to the local
	 * azimuthal equidistant plane using the projection
	 * @param lat in degrees (47.1234567°, not 471234567°)
	 * @param lon in degrees (8.1234567°, not 81234567°)
	 * @return the point in local coordinates as north / east
	 */
	درونخطی matrix::Vector2f project(دوگانه lat, دوگانه lon) ثابت
	{
		matrix::Vector2f res;
		project(lat, lon, res(0), res(1));
		بازگشت res;
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
	تهی reproject(اعشاری x, اعشاری y, دوگانه &lat, دوگانه &lon) ثابت;
};
