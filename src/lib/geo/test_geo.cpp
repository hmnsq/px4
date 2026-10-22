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

#ضمن <gtest/gtest.h>
#ضمن <math.h>
#ضمن <mathlib/mathlib.h>
#ضمن <memory>
#ضمن <lib/geo/geo.h>

صنف GeoTest : عام ::testing::Test
{
عام:
	فراغ سیٹ_اتے() override
	{
		proj.initReference(473566094 / 1e7, 85190237 / 1e7, 0);
	}

محمي:
	MapProjection proj;

};


TEST_F(GeoTest, reprojectProject)
{
	// GIVEN: x and y coordinates in the local cartesian frame
	عائم x = 0.5;
	عائم y = 1;
	مزدوج lat;
	مزدوج lon;
	proj.reproject(x, y, lat, lon);
	عائم x_new;
	عائم y_new;
	proj.project(lat, lon, x_new, y_new);
	مزدوج lat_new;
	مزدوج lon_new;
	proj.reproject(x_new, y_new, lat_new, lon_new);
	EXPECT_FLOAT_EQ(x, x_new);
	EXPECT_FLOAT_EQ(y, y_new);
	EXPECT_FLOAT_EQ(lat, lat_new);
	EXPECT_FLOAT_EQ(lon, lon_new);
}

TEST_F(GeoTest, projectReproject)
{
	// GIVEN: lat and lon coordinates in the geographic coordinate system
	مزدوج lat = 47.356616973876953;
	مزدوج lon = 8.5190505981445313;
	عائم x;
	عائم y;
	proj.project(lat, lon, x, y);
	مزدوج lat_new;
	مزدوج lon_new;
	proj.reproject(x, y, lat_new, lon_new);
	عائم x_new;
	عائم y_new;
	proj.project(lat_new, lon_new, x_new, y_new);
	// WHEN: apply the mapping and its inverse the output should stay the same
	EXPECT_FLOAT_EQ(x, x_new);
	EXPECT_FLOAT_EQ(y, y_new);
	EXPECT_FLOAT_EQ(lat, lat_new);
	EXPECT_FLOAT_EQ(lon, lon_new);
}

TEST_F(GeoTest, waypoint_from_heading_and_zero_فاصلہ)
{
	// GIVEN: a starting waypoint, a heading and a distance of 0
	مزدوج lat_شروع = -33;
	مزدوج lon_شروع = 18;
	عائم bearing = 0;
	عائم dist = 0;

	مزدوج lat_نشانہ = 0;
	مزدوج lon_نشانہ = 0;

	// WHEN: we get the next waypoint
	waypoint_from_heading_and_فاصلہ(lat_شروع, lon_شروع, bearing, dist, &lat_نشانہ, &lon_نشانہ);

	// THEN: it should be the same
	EXPECT_DOUBLE_EQ(lat_شروع, lat_نشانہ);
	EXPECT_DOUBLE_EQ(lon_شروع, lon_نشانہ);
}


TEST_F(GeoTest, waypoint_from_heading_and_negative_فاصلہ)
{
	// GIVEN: a starting waypoint, a heading and negative distance
	مزدوج lat_شروع = -33;
	مزدوج lon_شروع = 18;
	عائم bearing = 0;
	عائم lat_وتھ = -0.01f;
	عائم dist = CONSTANTS_RADIUS_دا_EARTH_F * M_PI_F / 180.f * lat_وتھ;

	مزدوج lat_نشانہ = 0;
	مزدوج lon_نشانہ = 0;

	// WHEN: we get the next waypoint
	waypoint_from_heading_and_فاصلہ(lat_شروع, lon_شروع, bearing, dist, &lat_نشانہ, &lon_نشانہ);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(static_cast<عائم>(lat_شروع) + lat_وتھ, lat_نشانہ);
	EXPECT_DOUBLE_EQ(lon_شروع, lon_نشانہ);
}

TEST_F(GeoTest, waypoint_from_heading_and_مثبت_فاصلہ)
{
	// GIVEN: a starting waypoint, a heading and positive distance
	مزدوج lat_شروع = -33;
	مزدوج lon_شروع = 18;
	عائم bearing = 0;
	عائم lat_وتھ = 0.01f;
	عائم dist = CONSTANTS_RADIUS_دا_EARTH_F * M_PI_F / 180.f * lat_وتھ;

	مزدوج lat_نشانہ = 0;
	مزدوج lon_نشانہ = 0;

	// WHEN: we get the next waypoint
	waypoint_from_heading_and_فاصلہ(lat_شروع, lon_شروع, bearing, dist, &lat_نشانہ, &lon_نشانہ);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(static_cast<عائم>(lat_شروع) + lat_وتھ, lat_نشانہ);
	EXPECT_DOUBLE_EQ(lon_شروع, lon_نشانہ);
}

TEST_F(GeoTest, waypoint_from_سطر_and_zero_فاصلہ)
{
	// GIVEN: a starting waypoint, a heading and a distance of 0
	مزدوج lat_شروع = -33;
	مزدوج lon_شروع = 18;
	مزدوج lat_سرا = -32;
	مزدوج lon_سرا = 18;
	عائم dist = 0;

	مزدوج lat_نشانہ = 0;
	مزدوج lon_نشانہ = 0;

	// WHEN: we get the next waypoint
	create_waypoint_from_سطر_and_dist(lat_شروع, lon_شروع, lat_سرا, lon_سرا, dist, &lat_نشانہ, &lon_نشانہ);

	// THEN: it should be the same
	EXPECT_DOUBLE_EQ(lat_شروع, lat_نشانہ);
	EXPECT_DOUBLE_EQ(lon_شروع, lon_نشانہ);
}

TEST_F(GeoTest, waypoint_from_سطر_and_مثبت_فاصلہ)
{
	// GIVEN: a starting waypoint, a heading and a positive distance
	مزدوج lat_شروع = -33;
	مزدوج lon_شروع = 18;
	مزدوج lat_وتھ = 0.01;
	مزدوج lat_سرا = lat_شروع + lat_وتھ;
	مزدوج lon_سرا = 18;
	عائم dist = CONSTANTS_RADIUS_دا_EARTH_F * M_PI_F / 180.f * static_cast<عائم>(lat_وتھ);

	مزدوج lat_نشانہ = 0;
	مزدوج lon_نشانہ = 0;

	// WHEN: we get the next waypoint
	create_waypoint_from_سطر_and_dist(lat_شروع, lon_شروع, lat_سرا, lon_سرا, dist, &lat_نشانہ, &lon_نشانہ);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(lat_شروع + lat_وتھ, lat_نشانہ);
	EXPECT_DOUBLE_EQ(lon_شروع, lon_نشانہ);
}


TEST_F(GeoTest, waypoint_from_سطر_and_negative_فاصلہ)
{
	// GIVEN: a starting waypoint, a heading and a negative distance
	مزدوج lat_شروع = -33;
	مزدوج lon_شروع = 18;
	مزدوج lat_وتھ = 0.01;
	مزدوج lat_سرا = lat_شروع + lat_وتھ;
	مزدوج lon_سرا = 18;
	عائم dist = -CONSTANTS_RADIUS_دا_EARTH_F * M_PI_F / 180.f * static_cast<عائم>(lat_وتھ);

	مزدوج lat_نشانہ = 0;
	مزدوج lon_نشانہ = 0;

	// WHEN: we get the next waypoint
	create_waypoint_from_سطر_and_dist(lat_شروع, lon_شروع, lat_سرا, lon_سرا, dist, &lat_نشانہ, &lon_نشانہ);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(lat_شروع - lat_وتھ, lat_نشانہ);
	EXPECT_DOUBLE_EQ(lon_شروع, lon_نشانہ);
}

TEST_F(GeoTest, get_فاصلہ_منزل_Arc_Outside_Sector)
{
	// GIVEN: an arc at a mid-latitude (so the longitude cosine scaling matters), sweeping from
	// due-North of the center (start) to due-East (end).
	ثابت مزدوج lat_center = 47.0;
	ثابت مزدوج lon_center = 8.0;
	ثابت عائم radius = 10000.f;            // [m]
	ثابت عائم arc_شروع_bearing = 0.f;     // start point due North of the center
	ثابت عائم arc_sweep = M_PI_F / 2.f;    // end point due East of the center
	ثابت مزدوج mpd = 111111.0;
	ثابت مزدوج cos_lat = جیب_تمام(math::radians(lat_center));

	// The implementation locates the endpoints with an equirectangular approximation; mirror it so we
	// can predict which endpoint is nearer and at what distance.
	خودکار endpoint = [&](مزدوج bearing, مزدوج & lat, مزدوج & lon) {
		lat = lat_center + static_cast<مزدوج>(radius) * جیب_تمام(bearing) / mpd;
		lon = lon_center + static_cast<مزدوج>(radius) * جیب(bearing) / (mpd * cos_lat);
	};
	مزدوج lat_شروع, lon_شروع, lat_سرا, lon_سرا;
	endpoint(static_cast<مزدوج>(arc_شروع_bearing), lat_شروع, lon_شروع);
	endpoint(static_cast<مزدوج>(arc_شروع_bearing + arc_sweep), lat_سرا, lon_سرا);

	crosstrack_error_s err{};
	مزدوج lat_v, lon_v;

	// WHEN: the vehicle is outside the wedge on the start side (0.5 rad before the start bearing)
	waypoint_from_heading_and_فاصلہ(lat_center, lon_center, arc_شروع_bearing - 0.5f, radius, &lat_v, &lon_v);
	get_فاصلہ_منزل_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_شروع_bearing, arc_sweep);
	// THEN: the result is the distance to the nearer (start) endpoint, not past the end
	EXPECT_LT(get_فاصلہ_منزل_اگلا_waypoint(lat_v, lon_v, lat_شروع, lon_شروع),
		  get_فاصلہ_منزل_اگلا_waypoint(lat_v, lon_v, lat_سرا, lon_سرا));
	EXPECT_NEAR(err.فاصلہ, get_فاصلہ_منزل_اگلا_waypoint(lat_v, lon_v, lat_شروع, lon_شروع), 0.5f);
	EXPECT_FALSE(err.past_سرا);

	// WHEN: the vehicle is outside the wedge on the end side (0.5 rad past the end bearing)
	waypoint_from_heading_and_فاصلہ(lat_center, lon_center, arc_شروع_bearing + arc_sweep + 0.5f, radius, &lat_v, &lon_v);
	get_فاصلہ_منزل_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_شروع_bearing, arc_sweep);
	// THEN: the result is the distance to the nearer (end) endpoint, past the end
	EXPECT_LT(get_فاصلہ_منزل_اگلا_waypoint(lat_v, lon_v, lat_سرا, lon_سرا),
		  get_فاصلہ_منزل_اگلا_waypoint(lat_v, lon_v, lat_شروع, lon_شروع));
	EXPECT_NEAR(err.فاصلہ, get_فاصلہ_منزل_اگلا_waypoint(lat_v, lon_v, lat_سرا, lon_سرا), 0.5f);
	EXPECT_TRUE(err.past_سرا);
}

TEST_F(GeoTest, get_فاصلہ_منزل_Arc_Inside_Sector)
{
	// GIVEN: an arc sweeping from due-North (start) to due-East (end) of the center
	ثابت مزدوج lat_center = 47.0;
	ثابت مزدوج lon_center = 8.0;
	ثابت عائم radius = 10000.f;
	ثابت عائم arc_شروع_bearing = 0.f;
	ثابت عائم arc_sweep = M_PI_F / 2.f;
	ثابت عائم mid_bearing = arc_شروع_bearing + arc_sweep / 2.f;   // pi/4, inside the wedge

	crosstrack_error_s err{};
	مزدوج lat_v, lon_v;

	// WHEN: the vehicle is exactly on the arc (on the circle, inside the wedge). This is the case the
	// previous in-sector logic mis-classified and reported kilometers off.
	waypoint_from_heading_and_فاصلہ(lat_center, lon_center, mid_bearing, radius, &lat_v, &lon_v);
	get_فاصلہ_منزل_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_شروع_bearing, arc_sweep);
	// THEN: distance to the arc is ~zero
	EXPECT_NEAR(err.فاصلہ, 0.f, 1.f);
	EXPECT_FALSE(err.past_سرا);

	// WHEN: the vehicle is inside the circle, within the wedge (half a radius from the center)
	waypoint_from_heading_and_فاصلہ(lat_center, lon_center, mid_bearing, radius / 2.f, &lat_v, &lon_v);
	get_فاصلہ_منزل_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_شروع_bearing, arc_sweep);
	// THEN: the distance is the radial gap to the arc (~radius/2)
	EXPECT_NEAR(err.فاصلہ, radius / 2.f, 1.f);
	EXPECT_FALSE(err.past_سرا);

	// WHEN: the vehicle is outside the circle, within the wedge (1.5 radius from the center)
	waypoint_from_heading_and_فاصلہ(lat_center, lon_center, mid_bearing, 1.5f * radius, &lat_v, &lon_v);
	get_فاصلہ_منزل_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_شروع_bearing, arc_sweep);
	// THEN: the distance is again the radial gap (~radius/2)
	EXPECT_NEAR(err.فاصلہ, radius / 2.f, 1.f);
	EXPECT_FALSE(err.past_سرا);
}

TEST_F(GeoTest, get_فاصلہ_منزل_Arc_Negative_Sweep)
{
	// GIVEN: the same NE-quadrant arc, but defined from due-East (start) sweeping negatively to North (end)
	ثابت مزدوج lat_center = 47.0;
	ثابت مزدوج lon_center = 8.0;
	ثابت عائم radius = 10000.f;
	ثابت عائم arc_شروع_bearing = M_PI_F / 2.f;
	ثابت عائم arc_sweep = -M_PI_F / 2.f;

	crosstrack_error_s err{};
	مزدوج lat_v, lon_v;

	// WHEN: the vehicle is on the arc midpoint (bearing pi/4 from the center) -> in sector
	waypoint_from_heading_and_فاصلہ(lat_center, lon_center, M_PI_F / 4.f, radius, &lat_v, &lon_v);
	get_فاصلہ_منزل_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_شروع_bearing, arc_sweep);
	// THEN: distance to the arc is ~zero
	EXPECT_NEAR(err.فاصلہ, 0.f, 1.f);
	EXPECT_FALSE(err.past_سرا);

	// WHEN: the vehicle is due South of the center, far outside the wedge -> out of sector
	waypoint_from_heading_and_فاصلہ(lat_center, lon_center, M_PI_F, radius, &lat_v, &lon_v);
	get_فاصلہ_منزل_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_شروع_bearing, arc_sweep);
	// THEN: it is NOT treated as on the arc (the old negative-sweep sector math returned ~zero here)
	EXPECT_GT(err.فاصلہ, radius);
}
