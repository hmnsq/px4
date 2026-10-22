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

#شامل <gtest/gtest.h>
#شامل <math.h>
#شامل <mathlib/mathlib.h>
#شامل <memory>
#شامل <lib/geo/geo.h>

جماعت GeoTest : عوامی ::testing::Test
{
عوامی:
	خالی سیٹ_اوپر() override
	{
		proj.initReference(473566094 / 1e7, 85190237 / 1e7, 0);
	}

محفوظ:
	MapProjection proj;

};


TEST_F(GeoTest, reprojectProject)
{
	// GIVEN: x and y coordinates in the local cartesian frame
	اعشاری x = 0.5;
	اعشاری y = 1;
	دوہرا lat;
	دوہرا lon;
	proj.reproject(x, y, lat, lon);
	اعشاری x_new;
	اعشاری y_new;
	proj.project(lat, lon, x_new, y_new);
	دوہرا lat_new;
	دوہرا lon_new;
	proj.reproject(x_new, y_new, lat_new, lon_new);
	EXPECT_FLOAT_EQ(x, x_new);
	EXPECT_FLOAT_EQ(y, y_new);
	EXPECT_FLOAT_EQ(lat, lat_new);
	EXPECT_FLOAT_EQ(lon, lon_new);
}

TEST_F(GeoTest, projectReproject)
{
	// GIVEN: lat and lon coordinates in the geographic coordinate system
	دوہرا lat = 47.356616973876953;
	دوہرا lon = 8.5190505981445313;
	اعشاری x;
	اعشاری y;
	proj.project(lat, lon, x, y);
	دوہرا lat_new;
	دوہرا lon_new;
	proj.reproject(x, y, lat_new, lon_new);
	اعشاری x_new;
	اعشاری y_new;
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
	دوہرا lat_آغاز = -33;
	دوہرا lon_آغاز = 18;
	اعشاری bearing = 0;
	اعشاری dist = 0;

	دوہرا lat_ہدف = 0;
	دوہرا lon_ہدف = 0;

	// WHEN: we get the next waypoint
	waypoint_from_heading_and_فاصلہ(lat_آغاز, lon_آغاز, bearing, dist, &lat_ہدف, &lon_ہدف);

	// THEN: it should be the same
	EXPECT_DOUBLE_EQ(lat_آغاز, lat_ہدف);
	EXPECT_DOUBLE_EQ(lon_آغاز, lon_ہدف);
}


TEST_F(GeoTest, waypoint_from_heading_and_negative_فاصلہ)
{
	// GIVEN: a starting waypoint, a heading and negative distance
	دوہرا lat_آغاز = -33;
	دوہرا lon_آغاز = 18;
	اعشاری bearing = 0;
	اعشاری lat_انحراف = -0.01f;
	اعشاری dist = CONSTANTS_RADIUS_کا_EARTH_F * M_PI_F / 180.f * lat_انحراف;

	دوہرا lat_ہدف = 0;
	دوہرا lon_ہدف = 0;

	// WHEN: we get the next waypoint
	waypoint_from_heading_and_فاصلہ(lat_آغاز, lon_آغاز, bearing, dist, &lat_ہدف, &lon_ہدف);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(static_cast<اعشاری>(lat_آغاز) + lat_انحراف, lat_ہدف);
	EXPECT_DOUBLE_EQ(lon_آغاز, lon_ہدف);
}

TEST_F(GeoTest, waypoint_from_heading_and_مثبت_فاصلہ)
{
	// GIVEN: a starting waypoint, a heading and positive distance
	دوہرا lat_آغاز = -33;
	دوہرا lon_آغاز = 18;
	اعشاری bearing = 0;
	اعشاری lat_انحراف = 0.01f;
	اعشاری dist = CONSTANTS_RADIUS_کا_EARTH_F * M_PI_F / 180.f * lat_انحراف;

	دوہرا lat_ہدف = 0;
	دوہرا lon_ہدف = 0;

	// WHEN: we get the next waypoint
	waypoint_from_heading_and_فاصلہ(lat_آغاز, lon_آغاز, bearing, dist, &lat_ہدف, &lon_ہدف);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(static_cast<اعشاری>(lat_آغاز) + lat_انحراف, lat_ہدف);
	EXPECT_DOUBLE_EQ(lon_آغاز, lon_ہدف);
}

TEST_F(GeoTest, waypoint_from_سطر_and_zero_فاصلہ)
{
	// GIVEN: a starting waypoint, a heading and a distance of 0
	دوہرا lat_آغاز = -33;
	دوہرا lon_آغاز = 18;
	دوہرا lat_انتہا = -32;
	دوہرا lon_انتہا = 18;
	اعشاری dist = 0;

	دوہرا lat_ہدف = 0;
	دوہرا lon_ہدف = 0;

	// WHEN: we get the next waypoint
	create_waypoint_from_سطر_and_dist(lat_آغاز, lon_آغاز, lat_انتہا, lon_انتہا, dist, &lat_ہدف, &lon_ہدف);

	// THEN: it should be the same
	EXPECT_DOUBLE_EQ(lat_آغاز, lat_ہدف);
	EXPECT_DOUBLE_EQ(lon_آغاز, lon_ہدف);
}

TEST_F(GeoTest, waypoint_from_سطر_and_مثبت_فاصلہ)
{
	// GIVEN: a starting waypoint, a heading and a positive distance
	دوہرا lat_آغاز = -33;
	دوہرا lon_آغاز = 18;
	دوہرا lat_انحراف = 0.01;
	دوہرا lat_انتہا = lat_آغاز + lat_انحراف;
	دوہرا lon_انتہا = 18;
	اعشاری dist = CONSTANTS_RADIUS_کا_EARTH_F * M_PI_F / 180.f * static_cast<اعشاری>(lat_انحراف);

	دوہرا lat_ہدف = 0;
	دوہرا lon_ہدف = 0;

	// WHEN: we get the next waypoint
	create_waypoint_from_سطر_and_dist(lat_آغاز, lon_آغاز, lat_انتہا, lon_انتہا, dist, &lat_ہدف, &lon_ہدف);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(lat_آغاز + lat_انحراف, lat_ہدف);
	EXPECT_DOUBLE_EQ(lon_آغاز, lon_ہدف);
}


TEST_F(GeoTest, waypoint_from_سطر_and_negative_فاصلہ)
{
	// GIVEN: a starting waypoint, a heading and a negative distance
	دوہرا lat_آغاز = -33;
	دوہرا lon_آغاز = 18;
	دوہرا lat_انحراف = 0.01;
	دوہرا lat_انتہا = lat_آغاز + lat_انحراف;
	دوہرا lon_انتہا = 18;
	اعشاری dist = -CONSTANTS_RADIUS_کا_EARTH_F * M_PI_F / 180.f * static_cast<اعشاری>(lat_انحراف);

	دوہرا lat_ہدف = 0;
	دوہرا lon_ہدف = 0;

	// WHEN: we get the next waypoint
	create_waypoint_from_سطر_and_dist(lat_آغاز, lon_آغاز, lat_انتہا, lon_انتہا, dist, &lat_ہدف, &lon_ہدف);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(lat_آغاز - lat_انحراف, lat_ہدف);
	EXPECT_DOUBLE_EQ(lon_آغاز, lon_ہدف);
}

TEST_F(GeoTest, get_فاصلہ_مقصد_Arc_Outside_Sector)
{
	// GIVEN: an arc at a mid-latitude (so the longitude cosine scaling matters), sweeping from
	// due-North of the center (start) to due-East (end).
	ثابت دوہرا lat_center = 47.0;
	ثابت دوہرا lon_center = 8.0;
	ثابت اعشاری radius = 10000.f;            // [m]
	ثابت اعشاری arc_آغاز_bearing = 0.f;     // start point due North of the center
	ثابت اعشاری arc_sweep = M_PI_F / 2.f;    // end point due East of the center
	ثابت دوہرا mpd = 111111.0;
	ثابت دوہرا cos_lat = جیب_تمام(math::radians(lat_center));

	// The implementation locates the endpoints with an equirectangular approximation; mirror it so we
	// can predict which endpoint is nearer and at what distance.
	خودکار endpoint = [&](دوہرا bearing, دوہرا & lat, دوہرا & lon) {
		lat = lat_center + static_cast<دوہرا>(radius) * جیب_تمام(bearing) / mpd;
		lon = lon_center + static_cast<دوہرا>(radius) * جیب(bearing) / (mpd * cos_lat);
	};
	دوہرا lat_آغاز, lon_آغاز, lat_انتہا, lon_انتہا;
	endpoint(static_cast<دوہرا>(arc_آغاز_bearing), lat_آغاز, lon_آغاز);
	endpoint(static_cast<دوہرا>(arc_آغاز_bearing + arc_sweep), lat_انتہا, lon_انتہا);

	crosstrack_error_s err{};
	دوہرا lat_v, lon_v;

	// WHEN: the vehicle is outside the wedge on the start side (0.5 rad before the start bearing)
	waypoint_from_heading_and_فاصلہ(lat_center, lon_center, arc_آغاز_bearing - 0.5f, radius, &lat_v, &lon_v);
	get_فاصلہ_مقصد_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_آغاز_bearing, arc_sweep);
	// THEN: the result is the distance to the nearer (start) endpoint, not past the end
	EXPECT_LT(get_فاصلہ_مقصد_اگلا_waypoint(lat_v, lon_v, lat_آغاز, lon_آغاز),
		  get_فاصلہ_مقصد_اگلا_waypoint(lat_v, lon_v, lat_انتہا, lon_انتہا));
	EXPECT_NEAR(err.فاصلہ, get_فاصلہ_مقصد_اگلا_waypoint(lat_v, lon_v, lat_آغاز, lon_آغاز), 0.5f);
	EXPECT_FALSE(err.past_انتہا);

	// WHEN: the vehicle is outside the wedge on the end side (0.5 rad past the end bearing)
	waypoint_from_heading_and_فاصلہ(lat_center, lon_center, arc_آغاز_bearing + arc_sweep + 0.5f, radius, &lat_v, &lon_v);
	get_فاصلہ_مقصد_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_آغاز_bearing, arc_sweep);
	// THEN: the result is the distance to the nearer (end) endpoint, past the end
	EXPECT_LT(get_فاصلہ_مقصد_اگلا_waypoint(lat_v, lon_v, lat_انتہا, lon_انتہا),
		  get_فاصلہ_مقصد_اگلا_waypoint(lat_v, lon_v, lat_آغاز, lon_آغاز));
	EXPECT_NEAR(err.فاصلہ, get_فاصلہ_مقصد_اگلا_waypoint(lat_v, lon_v, lat_انتہا, lon_انتہا), 0.5f);
	EXPECT_TRUE(err.past_انتہا);
}

TEST_F(GeoTest, get_فاصلہ_مقصد_Arc_Inside_Sector)
{
	// GIVEN: an arc sweeping from due-North (start) to due-East (end) of the center
	ثابت دوہرا lat_center = 47.0;
	ثابت دوہرا lon_center = 8.0;
	ثابت اعشاری radius = 10000.f;
	ثابت اعشاری arc_آغاز_bearing = 0.f;
	ثابت اعشاری arc_sweep = M_PI_F / 2.f;
	ثابت اعشاری mid_bearing = arc_آغاز_bearing + arc_sweep / 2.f;   // pi/4, inside the wedge

	crosstrack_error_s err{};
	دوہرا lat_v, lon_v;

	// WHEN: the vehicle is exactly on the arc (on the circle, inside the wedge). This is the case the
	// previous in-sector logic mis-classified and reported kilometers off.
	waypoint_from_heading_and_فاصلہ(lat_center, lon_center, mid_bearing, radius, &lat_v, &lon_v);
	get_فاصلہ_مقصد_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_آغاز_bearing, arc_sweep);
	// THEN: distance to the arc is ~zero
	EXPECT_NEAR(err.فاصلہ, 0.f, 1.f);
	EXPECT_FALSE(err.past_انتہا);

	// WHEN: the vehicle is inside the circle, within the wedge (half a radius from the center)
	waypoint_from_heading_and_فاصلہ(lat_center, lon_center, mid_bearing, radius / 2.f, &lat_v, &lon_v);
	get_فاصلہ_مقصد_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_آغاز_bearing, arc_sweep);
	// THEN: the distance is the radial gap to the arc (~radius/2)
	EXPECT_NEAR(err.فاصلہ, radius / 2.f, 1.f);
	EXPECT_FALSE(err.past_انتہا);

	// WHEN: the vehicle is outside the circle, within the wedge (1.5 radius from the center)
	waypoint_from_heading_and_فاصلہ(lat_center, lon_center, mid_bearing, 1.5f * radius, &lat_v, &lon_v);
	get_فاصلہ_مقصد_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_آغاز_bearing, arc_sweep);
	// THEN: the distance is again the radial gap (~radius/2)
	EXPECT_NEAR(err.فاصلہ, radius / 2.f, 1.f);
	EXPECT_FALSE(err.past_انتہا);
}

TEST_F(GeoTest, get_فاصلہ_مقصد_Arc_Negative_Sweep)
{
	// GIVEN: the same NE-quadrant arc, but defined from due-East (start) sweeping negatively to North (end)
	ثابت دوہرا lat_center = 47.0;
	ثابت دوہرا lon_center = 8.0;
	ثابت اعشاری radius = 10000.f;
	ثابت اعشاری arc_آغاز_bearing = M_PI_F / 2.f;
	ثابت اعشاری arc_sweep = -M_PI_F / 2.f;

	crosstrack_error_s err{};
	دوہرا lat_v, lon_v;

	// WHEN: the vehicle is on the arc midpoint (bearing pi/4 from the center) -> in sector
	waypoint_from_heading_and_فاصلہ(lat_center, lon_center, M_PI_F / 4.f, radius, &lat_v, &lon_v);
	get_فاصلہ_مقصد_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_آغاز_bearing, arc_sweep);
	// THEN: distance to the arc is ~zero
	EXPECT_NEAR(err.فاصلہ, 0.f, 1.f);
	EXPECT_FALSE(err.past_انتہا);

	// WHEN: the vehicle is due South of the center, far outside the wedge -> out of sector
	waypoint_from_heading_and_فاصلہ(lat_center, lon_center, M_PI_F, radius, &lat_v, &lon_v);
	get_فاصلہ_مقصد_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_آغاز_bearing, arc_sweep);
	// THEN: it is NOT treated as on the arc (the old negative-sweep sector math returned ~zero here)
	EXPECT_GT(err.فاصلہ, radius);
}
