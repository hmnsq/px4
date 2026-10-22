/****************************************************************************
 *
 *   Copyright (c) 2020 ECL Development Team. All rights reserved.
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

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>
#ಅನ್ತರ್ಭಾವ <math.h>
#ಅನ್ತರ್ಭಾವ <mathlib/mathlib.h>
#ಅನ್ತರ್ಭಾವ <memory>
#ಅನ್ತರ್ಭಾವ <lib/geo/geo.h>

ವರ್ಗ GeoTest : ಸಾರ್ವಜನಿಕ ::testing::Test
{
ಸಾರ್ವಜನಿಕ:
	ಶೂನ್ಯ ಗಣ_ಮೇಲೆ() override
	{
		proj.initReference(473566094 / 1e7, 85190237 / 1e7, 0);
	}

ಸಂರಕ್ಷಿತ:
	MapProjection proj;

};


TEST_F(GeoTest, reprojectProject)
{
	// GIVEN: x and y coordinates in the local cartesian frame
	ದಶಮಾಂಶ x = 0.5;
	ದಶಮಾಂಶ y = 1;
	ದ್ವಿಗುಣ lat;
	ದ್ವಿಗುಣ lon;
	proj.reproject(x, y, lat, lon);
	ದಶಮಾಂಶ x_new;
	ದಶಮಾಂಶ y_new;
	proj.project(lat, lon, x_new, y_new);
	ದ್ವಿಗುಣ lat_new;
	ದ್ವಿಗುಣ lon_new;
	proj.reproject(x_new, y_new, lat_new, lon_new);
	EXPECT_FLOAT_EQ(x, x_new);
	EXPECT_FLOAT_EQ(y, y_new);
	EXPECT_FLOAT_EQ(lat, lat_new);
	EXPECT_FLOAT_EQ(lon, lon_new);
}

TEST_F(GeoTest, projectReproject)
{
	// GIVEN: lat and lon coordinates in the geographic coordinate system
	ದ್ವಿಗುಣ lat = 47.356616973876953;
	ದ್ವಿಗುಣ lon = 8.5190505981445313;
	ದಶಮಾಂಶ x;
	ದಶಮಾಂಶ y;
	proj.project(lat, lon, x, y);
	ದ್ವಿಗುಣ lat_new;
	ದ್ವಿಗುಣ lon_new;
	proj.reproject(x, y, lat_new, lon_new);
	ದಶಮಾಂಶ x_new;
	ದಶಮಾಂಶ y_new;
	proj.project(lat_new, lon_new, x_new, y_new);
	// WHEN: apply the mapping and its inverse the output should stay the same
	EXPECT_FLOAT_EQ(x, x_new);
	EXPECT_FLOAT_EQ(y, y_new);
	EXPECT_FLOAT_EQ(lat, lat_new);
	EXPECT_FLOAT_EQ(lon, lon_new);
}

TEST_F(GeoTest, waypoint_from_heading_and_zero_ದೂರ)
{
	// GIVEN: a starting waypoint, a heading and a distance of 0
	ದ್ವಿಗುಣ lat_ಆರಂಭ = -33;
	ದ್ವಿಗುಣ lon_ಆರಂಭ = 18;
	ದಶಮಾಂಶ bearing = 0;
	ದಶಮಾಂಶ dist = 0;

	ದ್ವಿಗುಣ lat_ಗುರಿ = 0;
	ದ್ವಿಗುಣ lon_ಗುರಿ = 0;

	// WHEN: we get the next waypoint
	waypoint_from_heading_and_ದೂರ(lat_ಆರಂಭ, lon_ಆರಂಭ, bearing, dist, &lat_ಗುರಿ, &lon_ಗುರಿ);

	// THEN: it should be the same
	EXPECT_DOUBLE_EQ(lat_ಆರಂಭ, lat_ಗುರಿ);
	EXPECT_DOUBLE_EQ(lon_ಆರಂಭ, lon_ಗುರಿ);
}


TEST_F(GeoTest, waypoint_from_heading_and_negative_ದೂರ)
{
	// GIVEN: a starting waypoint, a heading and negative distance
	ದ್ವಿಗುಣ lat_ಆರಂಭ = -33;
	ದ್ವಿಗುಣ lon_ಆರಂಭ = 18;
	ದಶಮಾಂಶ bearing = 0;
	ದಶಮಾಂಶ lat_ಸ್ಥಾನಾಂತರ = -0.01f;
	ದಶಮಾಂಶ dist = CONSTANTS_RADIUS_ದ_EARTH_F * M_PI_F / 180.f * lat_ಸ್ಥಾನಾಂತರ;

	ದ್ವಿಗುಣ lat_ಗುರಿ = 0;
	ದ್ವಿಗುಣ lon_ಗುರಿ = 0;

	// WHEN: we get the next waypoint
	waypoint_from_heading_and_ದೂರ(lat_ಆರಂಭ, lon_ಆರಂಭ, bearing, dist, &lat_ಗುರಿ, &lon_ಗುರಿ);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(static_cast<ದಶಮಾಂಶ>(lat_ಆರಂಭ) + lat_ಸ್ಥಾನಾಂತರ, lat_ಗುರಿ);
	EXPECT_DOUBLE_EQ(lon_ಆರಂಭ, lon_ಗುರಿ);
}

TEST_F(GeoTest, waypoint_from_heading_and_ಧನಾತ್ಮಕ_ದೂರ)
{
	// GIVEN: a starting waypoint, a heading and positive distance
	ದ್ವಿಗುಣ lat_ಆರಂಭ = -33;
	ದ್ವಿಗುಣ lon_ಆರಂಭ = 18;
	ದಶಮಾಂಶ bearing = 0;
	ದಶಮಾಂಶ lat_ಸ್ಥಾನಾಂತರ = 0.01f;
	ದಶಮಾಂಶ dist = CONSTANTS_RADIUS_ದ_EARTH_F * M_PI_F / 180.f * lat_ಸ್ಥಾನಾಂತರ;

	ದ್ವಿಗುಣ lat_ಗುರಿ = 0;
	ದ್ವಿಗುಣ lon_ಗುರಿ = 0;

	// WHEN: we get the next waypoint
	waypoint_from_heading_and_ದೂರ(lat_ಆರಂಭ, lon_ಆರಂಭ, bearing, dist, &lat_ಗುರಿ, &lon_ಗುರಿ);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(static_cast<ದಶಮಾಂಶ>(lat_ಆರಂಭ) + lat_ಸ್ಥಾನಾಂತರ, lat_ಗುರಿ);
	EXPECT_DOUBLE_EQ(lon_ಆರಂಭ, lon_ಗುರಿ);
}

TEST_F(GeoTest, waypoint_from_ಸಾಲು_and_zero_ದೂರ)
{
	// GIVEN: a starting waypoint, a heading and a distance of 0
	ದ್ವಿಗುಣ lat_ಆರಂಭ = -33;
	ದ್ವಿಗುಣ lon_ಆರಂಭ = 18;
	ದ್ವಿಗುಣ lat_ಕೊನೆ = -32;
	ದ್ವಿಗುಣ lon_ಕೊನೆ = 18;
	ದಶಮಾಂಶ dist = 0;

	ದ್ವಿಗುಣ lat_ಗುರಿ = 0;
	ದ್ವಿಗುಣ lon_ಗುರಿ = 0;

	// WHEN: we get the next waypoint
	create_waypoint_from_ಸಾಲು_and_dist(lat_ಆರಂಭ, lon_ಆರಂಭ, lat_ಕೊನೆ, lon_ಕೊನೆ, dist, &lat_ಗುರಿ, &lon_ಗುರಿ);

	// THEN: it should be the same
	EXPECT_DOUBLE_EQ(lat_ಆರಂಭ, lat_ಗುರಿ);
	EXPECT_DOUBLE_EQ(lon_ಆರಂಭ, lon_ಗುರಿ);
}

TEST_F(GeoTest, waypoint_from_ಸಾಲು_and_ಧನಾತ್ಮಕ_ದೂರ)
{
	// GIVEN: a starting waypoint, a heading and a positive distance
	ದ್ವಿಗುಣ lat_ಆರಂಭ = -33;
	ದ್ವಿಗುಣ lon_ಆರಂಭ = 18;
	ದ್ವಿಗುಣ lat_ಸ್ಥಾನಾಂತರ = 0.01;
	ದ್ವಿಗುಣ lat_ಕೊನೆ = lat_ಆರಂಭ + lat_ಸ್ಥಾನಾಂತರ;
	ದ್ವಿಗುಣ lon_ಕೊನೆ = 18;
	ದಶಮಾಂಶ dist = CONSTANTS_RADIUS_ದ_EARTH_F * M_PI_F / 180.f * static_cast<ದಶಮಾಂಶ>(lat_ಸ್ಥಾನಾಂತರ);

	ದ್ವಿಗುಣ lat_ಗುರಿ = 0;
	ದ್ವಿಗುಣ lon_ಗುರಿ = 0;

	// WHEN: we get the next waypoint
	create_waypoint_from_ಸಾಲು_and_dist(lat_ಆರಂಭ, lon_ಆರಂಭ, lat_ಕೊನೆ, lon_ಕೊನೆ, dist, &lat_ಗುರಿ, &lon_ಗುರಿ);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(lat_ಆರಂಭ + lat_ಸ್ಥಾನಾಂತರ, lat_ಗುರಿ);
	EXPECT_DOUBLE_EQ(lon_ಆರಂಭ, lon_ಗುರಿ);
}


TEST_F(GeoTest, waypoint_from_ಸಾಲು_and_negative_ದೂರ)
{
	// GIVEN: a starting waypoint, a heading and a negative distance
	ದ್ವಿಗುಣ lat_ಆರಂಭ = -33;
	ದ್ವಿಗುಣ lon_ಆರಂಭ = 18;
	ದ್ವಿಗುಣ lat_ಸ್ಥಾನಾಂತರ = 0.01;
	ದ್ವಿಗುಣ lat_ಕೊನೆ = lat_ಆರಂಭ + lat_ಸ್ಥಾನಾಂತರ;
	ದ್ವಿಗುಣ lon_ಕೊನೆ = 18;
	ದಶಮಾಂಶ dist = -CONSTANTS_RADIUS_ದ_EARTH_F * M_PI_F / 180.f * static_cast<ದಶಮಾಂಶ>(lat_ಸ್ಥಾನಾಂತರ);

	ದ್ವಿಗುಣ lat_ಗುರಿ = 0;
	ದ್ವಿಗುಣ lon_ಗುರಿ = 0;

	// WHEN: we get the next waypoint
	create_waypoint_from_ಸಾಲು_and_dist(lat_ಆರಂಭ, lon_ಆರಂಭ, lat_ಕೊನೆ, lon_ಕೊನೆ, dist, &lat_ಗುರಿ, &lon_ಗುರಿ);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(lat_ಆರಂಭ - lat_ಸ್ಥಾನಾಂತರ, lat_ಗುರಿ);
	EXPECT_DOUBLE_EQ(lon_ಆರಂಭ, lon_ಗುರಿ);
}

TEST_F(GeoTest, get_ದೂರ_ಗಮ್ಯ_Arc_Outside_Sector)
{
	// GIVEN: an arc at a mid-latitude (so the longitude cosine scaling matters), sweeping from
	// due-North of the center (start) to due-East (end).
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ lat_center = 47.0;
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ lon_center = 8.0;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ radius = 10000.f;            // [m]
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ arc_ಆರಂಭ_bearing = 0.f;     // start point due North of the center
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ arc_sweep = M_PI_F / 2.f;    // end point due East of the center
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ mpd = 111111.0;
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ cos_lat = ಕೋಜ್ಯಾ(math::radians(lat_center));

	// The implementation locates the endpoints with an equirectangular approximation; mirror it so we
	// can predict which endpoint is nearer and at what distance.
	ಸ್ವಯಂಚಾಲಿತ endpoint = [&](ದ್ವಿಗುಣ bearing, ದ್ವಿಗುಣ & lat, ದ್ವಿಗುಣ & lon) {
		lat = lat_center + static_cast<ದ್ವಿಗುಣ>(radius) * ಕೋಜ್ಯಾ(bearing) / mpd;
		lon = lon_center + static_cast<ದ್ವಿಗುಣ>(radius) * ಜ್ಯಾ(bearing) / (mpd * cos_lat);
	};
	ದ್ವಿಗುಣ lat_ಆರಂಭ, lon_ಆರಂಭ, lat_ಕೊನೆ, lon_ಕೊನೆ;
	endpoint(static_cast<ದ್ವಿಗುಣ>(arc_ಆರಂಭ_bearing), lat_ಆರಂಭ, lon_ಆರಂಭ);
	endpoint(static_cast<ದ್ವಿಗುಣ>(arc_ಆರಂಭ_bearing + arc_sweep), lat_ಕೊನೆ, lon_ಕೊನೆ);

	crosstrack_error_s err{};
	ದ್ವಿಗುಣ lat_v, lon_v;

	// WHEN: the vehicle is outside the wedge on the start side (0.5 rad before the start bearing)
	waypoint_from_heading_and_ದೂರ(lat_center, lon_center, arc_ಆರಂಭ_bearing - 0.5f, radius, &lat_v, &lon_v);
	get_ದೂರ_ಗಮ್ಯ_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ಆರಂಭ_bearing, arc_sweep);
	// THEN: the result is the distance to the nearer (start) endpoint, not past the end
	EXPECT_LT(get_ದೂರ_ಗಮ್ಯ_ಮುಂದಿನ_waypoint(lat_v, lon_v, lat_ಆರಂಭ, lon_ಆರಂಭ),
		  get_ದೂರ_ಗಮ್ಯ_ಮುಂದಿನ_waypoint(lat_v, lon_v, lat_ಕೊನೆ, lon_ಕೊನೆ));
	EXPECT_NEAR(err.ದೂರ, get_ದೂರ_ಗಮ್ಯ_ಮುಂದಿನ_waypoint(lat_v, lon_v, lat_ಆರಂಭ, lon_ಆರಂಭ), 0.5f);
	EXPECT_FALSE(err.past_ಕೊನೆ);

	// WHEN: the vehicle is outside the wedge on the end side (0.5 rad past the end bearing)
	waypoint_from_heading_and_ದೂರ(lat_center, lon_center, arc_ಆರಂಭ_bearing + arc_sweep + 0.5f, radius, &lat_v, &lon_v);
	get_ದೂರ_ಗಮ್ಯ_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ಆರಂಭ_bearing, arc_sweep);
	// THEN: the result is the distance to the nearer (end) endpoint, past the end
	EXPECT_LT(get_ದೂರ_ಗಮ್ಯ_ಮುಂದಿನ_waypoint(lat_v, lon_v, lat_ಕೊನೆ, lon_ಕೊನೆ),
		  get_ದೂರ_ಗಮ್ಯ_ಮುಂದಿನ_waypoint(lat_v, lon_v, lat_ಆರಂಭ, lon_ಆರಂಭ));
	EXPECT_NEAR(err.ದೂರ, get_ದೂರ_ಗಮ್ಯ_ಮುಂದಿನ_waypoint(lat_v, lon_v, lat_ಕೊನೆ, lon_ಕೊನೆ), 0.5f);
	EXPECT_TRUE(err.past_ಕೊನೆ);
}

TEST_F(GeoTest, get_ದೂರ_ಗಮ್ಯ_Arc_Inside_Sector)
{
	// GIVEN: an arc sweeping from due-North (start) to due-East (end) of the center
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ lat_center = 47.0;
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ lon_center = 8.0;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ radius = 10000.f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ arc_ಆರಂಭ_bearing = 0.f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ arc_sweep = M_PI_F / 2.f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ mid_bearing = arc_ಆರಂಭ_bearing + arc_sweep / 2.f;   // pi/4, inside the wedge

	crosstrack_error_s err{};
	ದ್ವಿಗುಣ lat_v, lon_v;

	// WHEN: the vehicle is exactly on the arc (on the circle, inside the wedge). This is the case the
	// previous in-sector logic mis-classified and reported kilometers off.
	waypoint_from_heading_and_ದೂರ(lat_center, lon_center, mid_bearing, radius, &lat_v, &lon_v);
	get_ದೂರ_ಗಮ್ಯ_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ಆರಂಭ_bearing, arc_sweep);
	// THEN: distance to the arc is ~zero
	EXPECT_NEAR(err.ದೂರ, 0.f, 1.f);
	EXPECT_FALSE(err.past_ಕೊನೆ);

	// WHEN: the vehicle is inside the circle, within the wedge (half a radius from the center)
	waypoint_from_heading_and_ದೂರ(lat_center, lon_center, mid_bearing, radius / 2.f, &lat_v, &lon_v);
	get_ದೂರ_ಗಮ್ಯ_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ಆರಂಭ_bearing, arc_sweep);
	// THEN: the distance is the radial gap to the arc (~radius/2)
	EXPECT_NEAR(err.ದೂರ, radius / 2.f, 1.f);
	EXPECT_FALSE(err.past_ಕೊನೆ);

	// WHEN: the vehicle is outside the circle, within the wedge (1.5 radius from the center)
	waypoint_from_heading_and_ದೂರ(lat_center, lon_center, mid_bearing, 1.5f * radius, &lat_v, &lon_v);
	get_ದೂರ_ಗಮ್ಯ_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ಆರಂಭ_bearing, arc_sweep);
	// THEN: the distance is again the radial gap (~radius/2)
	EXPECT_NEAR(err.ದೂರ, radius / 2.f, 1.f);
	EXPECT_FALSE(err.past_ಕೊನೆ);
}

TEST_F(GeoTest, get_ದೂರ_ಗಮ್ಯ_Arc_Negative_Sweep)
{
	// GIVEN: the same NE-quadrant arc, but defined from due-East (start) sweeping negatively to North (end)
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ lat_center = 47.0;
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ lon_center = 8.0;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ radius = 10000.f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ arc_ಆರಂಭ_bearing = M_PI_F / 2.f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ arc_sweep = -M_PI_F / 2.f;

	crosstrack_error_s err{};
	ದ್ವಿಗುಣ lat_v, lon_v;

	// WHEN: the vehicle is on the arc midpoint (bearing pi/4 from the center) -> in sector
	waypoint_from_heading_and_ದೂರ(lat_center, lon_center, M_PI_F / 4.f, radius, &lat_v, &lon_v);
	get_ದೂರ_ಗಮ್ಯ_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ಆರಂಭ_bearing, arc_sweep);
	// THEN: distance to the arc is ~zero
	EXPECT_NEAR(err.ದೂರ, 0.f, 1.f);
	EXPECT_FALSE(err.past_ಕೊನೆ);

	// WHEN: the vehicle is due South of the center, far outside the wedge -> out of sector
	waypoint_from_heading_and_ದೂರ(lat_center, lon_center, M_PI_F, radius, &lat_v, &lon_v);
	get_ದೂರ_ಗಮ್ಯ_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ಆರಂಭ_bearing, arc_sweep);
	// THEN: it is NOT treated as on the arc (the old negative-sweep sector math returned ~zero here)
	EXPECT_GT(err.ದೂರ, radius);
}
