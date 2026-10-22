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

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mathlib/mathlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <memory>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/geo/geo.h>

ਵਰਗ GeoTest : ਜਨਤਕ ::testing::Test
{
ਜਨਤਕ:
	ਖਾਲੀ ਸੈੱਟ_ਉੱਪਰ() override
	{
		proj.initReference(473566094 / 1e7, 85190237 / 1e7, 0);
	}

ਸੁਰੱਖਿਅਤ:
	MapProjection proj;

};


TEST_F(GeoTest, reprojectProject)
{
	// GIVEN: x and y coordinates in the local cartesian frame
	ਦਸ਼ਮਲਵ x = 0.5;
	ਦਸ਼ਮਲਵ y = 1;
	ਦੁੱਗਣਾ lat;
	ਦੁੱਗਣਾ lon;
	proj.reproject(x, y, lat, lon);
	ਦਸ਼ਮਲਵ x_new;
	ਦਸ਼ਮਲਵ y_new;
	proj.project(lat, lon, x_new, y_new);
	ਦੁੱਗਣਾ lat_new;
	ਦੁੱਗਣਾ lon_new;
	proj.reproject(x_new, y_new, lat_new, lon_new);
	EXPECT_FLOAT_EQ(x, x_new);
	EXPECT_FLOAT_EQ(y, y_new);
	EXPECT_FLOAT_EQ(lat, lat_new);
	EXPECT_FLOAT_EQ(lon, lon_new);
}

TEST_F(GeoTest, projectReproject)
{
	// GIVEN: lat and lon coordinates in the geographic coordinate system
	ਦੁੱਗਣਾ lat = 47.356616973876953;
	ਦੁੱਗਣਾ lon = 8.5190505981445313;
	ਦਸ਼ਮਲਵ x;
	ਦਸ਼ਮਲਵ y;
	proj.project(lat, lon, x, y);
	ਦੁੱਗਣਾ lat_new;
	ਦੁੱਗਣਾ lon_new;
	proj.reproject(x, y, lat_new, lon_new);
	ਦਸ਼ਮਲਵ x_new;
	ਦਸ਼ਮਲਵ y_new;
	proj.project(lat_new, lon_new, x_new, y_new);
	// WHEN: apply the mapping and its inverse the output should stay the same
	EXPECT_FLOAT_EQ(x, x_new);
	EXPECT_FLOAT_EQ(y, y_new);
	EXPECT_FLOAT_EQ(lat, lat_new);
	EXPECT_FLOAT_EQ(lon, lon_new);
}

TEST_F(GeoTest, waypoint_from_heading_and_zero_ਦੂਰੀ)
{
	// GIVEN: a starting waypoint, a heading and a distance of 0
	ਦੁੱਗਣਾ lat_ਸ਼ੁਰੂ = -33;
	ਦੁੱਗਣਾ lon_ਸ਼ੁਰੂ = 18;
	ਦਸ਼ਮਲਵ bearing = 0;
	ਦਸ਼ਮਲਵ dist = 0;

	ਦੁੱਗਣਾ lat_ਨਿਸ਼ਾਨਾ = 0;
	ਦੁੱਗਣਾ lon_ਨਿਸ਼ਾਨਾ = 0;

	// WHEN: we get the next waypoint
	waypoint_from_heading_and_ਦੂਰੀ(lat_ਸ਼ੁਰੂ, lon_ਸ਼ੁਰੂ, bearing, dist, &lat_ਨਿਸ਼ਾਨਾ, &lon_ਨਿਸ਼ਾਨਾ);

	// THEN: it should be the same
	EXPECT_DOUBLE_EQ(lat_ਸ਼ੁਰੂ, lat_ਨਿਸ਼ਾਨਾ);
	EXPECT_DOUBLE_EQ(lon_ਸ਼ੁਰੂ, lon_ਨਿਸ਼ਾਨਾ);
}


TEST_F(GeoTest, waypoint_from_heading_and_negative_ਦੂਰੀ)
{
	// GIVEN: a starting waypoint, a heading and negative distance
	ਦੁੱਗਣਾ lat_ਸ਼ੁਰੂ = -33;
	ਦੁੱਗਣਾ lon_ਸ਼ੁਰੂ = 18;
	ਦਸ਼ਮਲਵ bearing = 0;
	ਦਸ਼ਮਲਵ lat_ਵਿੱਥ = -0.01f;
	ਦਸ਼ਮਲਵ dist = CONSTANTS_RADIUS_ਦਾ_EARTH_F * M_PI_F / 180.f * lat_ਵਿੱਥ;

	ਦੁੱਗਣਾ lat_ਨਿਸ਼ਾਨਾ = 0;
	ਦੁੱਗਣਾ lon_ਨਿਸ਼ਾਨਾ = 0;

	// WHEN: we get the next waypoint
	waypoint_from_heading_and_ਦੂਰੀ(lat_ਸ਼ੁਰੂ, lon_ਸ਼ੁਰੂ, bearing, dist, &lat_ਨਿਸ਼ਾਨਾ, &lon_ਨਿਸ਼ਾਨਾ);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(static_cast<ਦਸ਼ਮਲਵ>(lat_ਸ਼ੁਰੂ) + lat_ਵਿੱਥ, lat_ਨਿਸ਼ਾਨਾ);
	EXPECT_DOUBLE_EQ(lon_ਸ਼ੁਰੂ, lon_ਨਿਸ਼ਾਨਾ);
}

TEST_F(GeoTest, waypoint_from_heading_and_ਸਕਾਰਾਤਮਕ_ਦੂਰੀ)
{
	// GIVEN: a starting waypoint, a heading and positive distance
	ਦੁੱਗਣਾ lat_ਸ਼ੁਰੂ = -33;
	ਦੁੱਗਣਾ lon_ਸ਼ੁਰੂ = 18;
	ਦਸ਼ਮਲਵ bearing = 0;
	ਦਸ਼ਮਲਵ lat_ਵਿੱਥ = 0.01f;
	ਦਸ਼ਮਲਵ dist = CONSTANTS_RADIUS_ਦਾ_EARTH_F * M_PI_F / 180.f * lat_ਵਿੱਥ;

	ਦੁੱਗਣਾ lat_ਨਿਸ਼ਾਨਾ = 0;
	ਦੁੱਗਣਾ lon_ਨਿਸ਼ਾਨਾ = 0;

	// WHEN: we get the next waypoint
	waypoint_from_heading_and_ਦੂਰੀ(lat_ਸ਼ੁਰੂ, lon_ਸ਼ੁਰੂ, bearing, dist, &lat_ਨਿਸ਼ਾਨਾ, &lon_ਨਿਸ਼ਾਨਾ);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(static_cast<ਦਸ਼ਮਲਵ>(lat_ਸ਼ੁਰੂ) + lat_ਵਿੱਥ, lat_ਨਿਸ਼ਾਨਾ);
	EXPECT_DOUBLE_EQ(lon_ਸ਼ੁਰੂ, lon_ਨਿਸ਼ਾਨਾ);
}

TEST_F(GeoTest, waypoint_from_ਸਤਰ_and_zero_ਦੂਰੀ)
{
	// GIVEN: a starting waypoint, a heading and a distance of 0
	ਦੁੱਗਣਾ lat_ਸ਼ੁਰੂ = -33;
	ਦੁੱਗਣਾ lon_ਸ਼ੁਰੂ = 18;
	ਦੁੱਗਣਾ lat_ਸਿਰਾ = -32;
	ਦੁੱਗਣਾ lon_ਸਿਰਾ = 18;
	ਦਸ਼ਮਲਵ dist = 0;

	ਦੁੱਗਣਾ lat_ਨਿਸ਼ਾਨਾ = 0;
	ਦੁੱਗਣਾ lon_ਨਿਸ਼ਾਨਾ = 0;

	// WHEN: we get the next waypoint
	create_waypoint_from_ਸਤਰ_and_dist(lat_ਸ਼ੁਰੂ, lon_ਸ਼ੁਰੂ, lat_ਸਿਰਾ, lon_ਸਿਰਾ, dist, &lat_ਨਿਸ਼ਾਨਾ, &lon_ਨਿਸ਼ਾਨਾ);

	// THEN: it should be the same
	EXPECT_DOUBLE_EQ(lat_ਸ਼ੁਰੂ, lat_ਨਿਸ਼ਾਨਾ);
	EXPECT_DOUBLE_EQ(lon_ਸ਼ੁਰੂ, lon_ਨਿਸ਼ਾਨਾ);
}

TEST_F(GeoTest, waypoint_from_ਸਤਰ_and_ਸਕਾਰਾਤਮਕ_ਦੂਰੀ)
{
	// GIVEN: a starting waypoint, a heading and a positive distance
	ਦੁੱਗਣਾ lat_ਸ਼ੁਰੂ = -33;
	ਦੁੱਗਣਾ lon_ਸ਼ੁਰੂ = 18;
	ਦੁੱਗਣਾ lat_ਵਿੱਥ = 0.01;
	ਦੁੱਗਣਾ lat_ਸਿਰਾ = lat_ਸ਼ੁਰੂ + lat_ਵਿੱਥ;
	ਦੁੱਗਣਾ lon_ਸਿਰਾ = 18;
	ਦਸ਼ਮਲਵ dist = CONSTANTS_RADIUS_ਦਾ_EARTH_F * M_PI_F / 180.f * static_cast<ਦਸ਼ਮਲਵ>(lat_ਵਿੱਥ);

	ਦੁੱਗਣਾ lat_ਨਿਸ਼ਾਨਾ = 0;
	ਦੁੱਗਣਾ lon_ਨਿਸ਼ਾਨਾ = 0;

	// WHEN: we get the next waypoint
	create_waypoint_from_ਸਤਰ_and_dist(lat_ਸ਼ੁਰੂ, lon_ਸ਼ੁਰੂ, lat_ਸਿਰਾ, lon_ਸਿਰਾ, dist, &lat_ਨਿਸ਼ਾਨਾ, &lon_ਨਿਸ਼ਾਨਾ);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(lat_ਸ਼ੁਰੂ + lat_ਵਿੱਥ, lat_ਨਿਸ਼ਾਨਾ);
	EXPECT_DOUBLE_EQ(lon_ਸ਼ੁਰੂ, lon_ਨਿਸ਼ਾਨਾ);
}


TEST_F(GeoTest, waypoint_from_ਸਤਰ_and_negative_ਦੂਰੀ)
{
	// GIVEN: a starting waypoint, a heading and a negative distance
	ਦੁੱਗਣਾ lat_ਸ਼ੁਰੂ = -33;
	ਦੁੱਗਣਾ lon_ਸ਼ੁਰੂ = 18;
	ਦੁੱਗਣਾ lat_ਵਿੱਥ = 0.01;
	ਦੁੱਗਣਾ lat_ਸਿਰਾ = lat_ਸ਼ੁਰੂ + lat_ਵਿੱਥ;
	ਦੁੱਗਣਾ lon_ਸਿਰਾ = 18;
	ਦਸ਼ਮਲਵ dist = -CONSTANTS_RADIUS_ਦਾ_EARTH_F * M_PI_F / 180.f * static_cast<ਦਸ਼ਮਲਵ>(lat_ਵਿੱਥ);

	ਦੁੱਗਣਾ lat_ਨਿਸ਼ਾਨਾ = 0;
	ਦੁੱਗਣਾ lon_ਨਿਸ਼ਾਨਾ = 0;

	// WHEN: we get the next waypoint
	create_waypoint_from_ਸਤਰ_and_dist(lat_ਸ਼ੁਰੂ, lon_ਸ਼ੁਰੂ, lat_ਸਿਰਾ, lon_ਸਿਰਾ, dist, &lat_ਨਿਸ਼ਾਨਾ, &lon_ਨਿਸ਼ਾਨਾ);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(lat_ਸ਼ੁਰੂ - lat_ਵਿੱਥ, lat_ਨਿਸ਼ਾਨਾ);
	EXPECT_DOUBLE_EQ(lon_ਸ਼ੁਰੂ, lon_ਨਿਸ਼ਾਨਾ);
}

TEST_F(GeoTest, get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_Arc_Outside_Sector)
{
	// GIVEN: an arc at a mid-latitude (so the longitude cosine scaling matters), sweeping from
	// due-North of the center (start) to due-East (end).
	ਅਚਲ ਦੁੱਗਣਾ lat_center = 47.0;
	ਅਚਲ ਦੁੱਗਣਾ lon_center = 8.0;
	ਅਚਲ ਦਸ਼ਮਲਵ radius = 10000.f;            // [m]
	ਅਚਲ ਦਸ਼ਮਲਵ arc_ਸ਼ੁਰੂ_bearing = 0.f;     // start point due North of the center
	ਅਚਲ ਦਸ਼ਮਲਵ arc_sweep = M_PI_F / 2.f;    // end point due East of the center
	ਅਚਲ ਦੁੱਗਣਾ mpd = 111111.0;
	ਅਚਲ ਦੁੱਗਣਾ cos_lat = ਕੋਜੀਵਾ(math::radians(lat_center));

	// The implementation locates the endpoints with an equirectangular approximation; mirror it so we
	// can predict which endpoint is nearer and at what distance.
	ਸਵੈਚਾਲਿਤ endpoint = [&](ਦੁੱਗਣਾ bearing, ਦੁੱਗਣਾ & lat, ਦੁੱਗਣਾ & lon) {
		lat = lat_center + static_cast<ਦੁੱਗਣਾ>(radius) * ਕੋਜੀਵਾ(bearing) / mpd;
		lon = lon_center + static_cast<ਦੁੱਗਣਾ>(radius) * ਜੀਵਾ(bearing) / (mpd * cos_lat);
	};
	ਦੁੱਗਣਾ lat_ਸ਼ੁਰੂ, lon_ਸ਼ੁਰੂ, lat_ਸਿਰਾ, lon_ਸਿਰਾ;
	endpoint(static_cast<ਦੁੱਗਣਾ>(arc_ਸ਼ੁਰੂ_bearing), lat_ਸ਼ੁਰੂ, lon_ਸ਼ੁਰੂ);
	endpoint(static_cast<ਦੁੱਗਣਾ>(arc_ਸ਼ੁਰੂ_bearing + arc_sweep), lat_ਸਿਰਾ, lon_ਸਿਰਾ);

	crosstrack_error_s err{};
	ਦੁੱਗਣਾ lat_v, lon_v;

	// WHEN: the vehicle is outside the wedge on the start side (0.5 rad before the start bearing)
	waypoint_from_heading_and_ਦੂਰੀ(lat_center, lon_center, arc_ਸ਼ੁਰੂ_bearing - 0.5f, radius, &lat_v, &lon_v);
	get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ਸ਼ੁਰੂ_bearing, arc_sweep);
	// THEN: the result is the distance to the nearer (start) endpoint, not past the end
	EXPECT_LT(get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(lat_v, lon_v, lat_ਸ਼ੁਰੂ, lon_ਸ਼ੁਰੂ),
		  get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(lat_v, lon_v, lat_ਸਿਰਾ, lon_ਸਿਰਾ));
	EXPECT_NEAR(err.ਦੂਰੀ, get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(lat_v, lon_v, lat_ਸ਼ੁਰੂ, lon_ਸ਼ੁਰੂ), 0.5f);
	EXPECT_FALSE(err.past_ਸਿਰਾ);

	// WHEN: the vehicle is outside the wedge on the end side (0.5 rad past the end bearing)
	waypoint_from_heading_and_ਦੂਰੀ(lat_center, lon_center, arc_ਸ਼ੁਰੂ_bearing + arc_sweep + 0.5f, radius, &lat_v, &lon_v);
	get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ਸ਼ੁਰੂ_bearing, arc_sweep);
	// THEN: the result is the distance to the nearer (end) endpoint, past the end
	EXPECT_LT(get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(lat_v, lon_v, lat_ਸਿਰਾ, lon_ਸਿਰਾ),
		  get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(lat_v, lon_v, lat_ਸ਼ੁਰੂ, lon_ਸ਼ੁਰੂ));
	EXPECT_NEAR(err.ਦੂਰੀ, get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਅਗਲਾ_waypoint(lat_v, lon_v, lat_ਸਿਰਾ, lon_ਸਿਰਾ), 0.5f);
	EXPECT_TRUE(err.past_ਸਿਰਾ);
}

TEST_F(GeoTest, get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_Arc_Inside_Sector)
{
	// GIVEN: an arc sweeping from due-North (start) to due-East (end) of the center
	ਅਚਲ ਦੁੱਗਣਾ lat_center = 47.0;
	ਅਚਲ ਦੁੱਗਣਾ lon_center = 8.0;
	ਅਚਲ ਦਸ਼ਮਲਵ radius = 10000.f;
	ਅਚਲ ਦਸ਼ਮਲਵ arc_ਸ਼ੁਰੂ_bearing = 0.f;
	ਅਚਲ ਦਸ਼ਮਲਵ arc_sweep = M_PI_F / 2.f;
	ਅਚਲ ਦਸ਼ਮਲਵ mid_bearing = arc_ਸ਼ੁਰੂ_bearing + arc_sweep / 2.f;   // pi/4, inside the wedge

	crosstrack_error_s err{};
	ਦੁੱਗਣਾ lat_v, lon_v;

	// WHEN: the vehicle is exactly on the arc (on the circle, inside the wedge). This is the case the
	// previous in-sector logic mis-classified and reported kilometers off.
	waypoint_from_heading_and_ਦੂਰੀ(lat_center, lon_center, mid_bearing, radius, &lat_v, &lon_v);
	get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ਸ਼ੁਰੂ_bearing, arc_sweep);
	// THEN: distance to the arc is ~zero
	EXPECT_NEAR(err.ਦੂਰੀ, 0.f, 1.f);
	EXPECT_FALSE(err.past_ਸਿਰਾ);

	// WHEN: the vehicle is inside the circle, within the wedge (half a radius from the center)
	waypoint_from_heading_and_ਦੂਰੀ(lat_center, lon_center, mid_bearing, radius / 2.f, &lat_v, &lon_v);
	get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ਸ਼ੁਰੂ_bearing, arc_sweep);
	// THEN: the distance is the radial gap to the arc (~radius/2)
	EXPECT_NEAR(err.ਦੂਰੀ, radius / 2.f, 1.f);
	EXPECT_FALSE(err.past_ਸਿਰਾ);

	// WHEN: the vehicle is outside the circle, within the wedge (1.5 radius from the center)
	waypoint_from_heading_and_ਦੂਰੀ(lat_center, lon_center, mid_bearing, 1.5f * radius, &lat_v, &lon_v);
	get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ਸ਼ੁਰੂ_bearing, arc_sweep);
	// THEN: the distance is again the radial gap (~radius/2)
	EXPECT_NEAR(err.ਦੂਰੀ, radius / 2.f, 1.f);
	EXPECT_FALSE(err.past_ਸਿਰਾ);
}

TEST_F(GeoTest, get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_Arc_Negative_Sweep)
{
	// GIVEN: the same NE-quadrant arc, but defined from due-East (start) sweeping negatively to North (end)
	ਅਚਲ ਦੁੱਗਣਾ lat_center = 47.0;
	ਅਚਲ ਦੁੱਗਣਾ lon_center = 8.0;
	ਅਚਲ ਦਸ਼ਮਲਵ radius = 10000.f;
	ਅਚਲ ਦਸ਼ਮਲਵ arc_ਸ਼ੁਰੂ_bearing = M_PI_F / 2.f;
	ਅਚਲ ਦਸ਼ਮਲਵ arc_sweep = -M_PI_F / 2.f;

	crosstrack_error_s err{};
	ਦੁੱਗਣਾ lat_v, lon_v;

	// WHEN: the vehicle is on the arc midpoint (bearing pi/4 from the center) -> in sector
	waypoint_from_heading_and_ਦੂਰੀ(lat_center, lon_center, M_PI_F / 4.f, radius, &lat_v, &lon_v);
	get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ਸ਼ੁਰੂ_bearing, arc_sweep);
	// THEN: distance to the arc is ~zero
	EXPECT_NEAR(err.ਦੂਰੀ, 0.f, 1.f);
	EXPECT_FALSE(err.past_ਸਿਰਾ);

	// WHEN: the vehicle is due South of the center, far outside the wedge -> out of sector
	waypoint_from_heading_and_ਦੂਰੀ(lat_center, lon_center, M_PI_F, radius, &lat_v, &lon_v);
	get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ਸ਼ੁਰੂ_bearing, arc_sweep);
	// THEN: it is NOT treated as on the arc (the old negative-sweep sector math returned ~zero here)
	EXPECT_GT(err.ਦੂਰੀ, radius);
}
