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

#అన్తర్భావ <gtest/gtest.h>
#అన్తర్భావ <math.h>
#అన్తర్భావ <mathlib/mathlib.h>
#అన్తర్భావ <memory>
#అన్తర్భావ <lib/geo/geo.h>

తరగతి GeoTest : బహిరంగ ::testing::Test
{
బహిరంగ:
	శూన్య సమితి_పైకి() override
	{
		proj.initReference(473566094 / 1e7, 85190237 / 1e7, 0);
	}

రక్షిత:
	MapProjection proj;

};


TEST_F(GeoTest, reprojectProject)
{
	// GIVEN: x and y coordinates in the local cartesian frame
	దశాంశం x = 0.5;
	దశాంశం y = 1;
	ద్విగుణ lat;
	ద్విగుణ lon;
	proj.reproject(x, y, lat, lon);
	దశాంశం x_new;
	దశాంశం y_new;
	proj.project(lat, lon, x_new, y_new);
	ద్విగుణ lat_new;
	ద్విగుణ lon_new;
	proj.reproject(x_new, y_new, lat_new, lon_new);
	EXPECT_FLOAT_EQ(x, x_new);
	EXPECT_FLOAT_EQ(y, y_new);
	EXPECT_FLOAT_EQ(lat, lat_new);
	EXPECT_FLOAT_EQ(lon, lon_new);
}

TEST_F(GeoTest, projectReproject)
{
	// GIVEN: lat and lon coordinates in the geographic coordinate system
	ద్విగుణ lat = 47.356616973876953;
	ద్విగుణ lon = 8.5190505981445313;
	దశాంశం x;
	దశాంశం y;
	proj.project(lat, lon, x, y);
	ద్విగుణ lat_new;
	ద్విగుణ lon_new;
	proj.reproject(x, y, lat_new, lon_new);
	దశాంశం x_new;
	దశాంశం y_new;
	proj.project(lat_new, lon_new, x_new, y_new);
	// WHEN: apply the mapping and its inverse the output should stay the same
	EXPECT_FLOAT_EQ(x, x_new);
	EXPECT_FLOAT_EQ(y, y_new);
	EXPECT_FLOAT_EQ(lat, lat_new);
	EXPECT_FLOAT_EQ(lon, lon_new);
}

TEST_F(GeoTest, waypoint_from_heading_and_zero_దూరం)
{
	// GIVEN: a starting waypoint, a heading and a distance of 0
	ద్విగుణ lat_ప్రారంభం = -33;
	ద్విగుణ lon_ప్రారంభం = 18;
	దశాంశం bearing = 0;
	దశాంశం dist = 0;

	ద్విగుణ lat_లక్ష్యం = 0;
	ద్విగుణ lon_లక్ష్యం = 0;

	// WHEN: we get the next waypoint
	waypoint_from_heading_and_దూరం(lat_ప్రారంభం, lon_ప్రారంభం, bearing, dist, &lat_లక్ష్యం, &lon_లక్ష్యం);

	// THEN: it should be the same
	EXPECT_DOUBLE_EQ(lat_ప్రారంభం, lat_లక్ష్యం);
	EXPECT_DOUBLE_EQ(lon_ప్రారంభం, lon_లక్ష్యం);
}


TEST_F(GeoTest, waypoint_from_heading_and_negative_దూరం)
{
	// GIVEN: a starting waypoint, a heading and negative distance
	ద్విగుణ lat_ప్రారంభం = -33;
	ద్విగుణ lon_ప్రారంభం = 18;
	దశాంశం bearing = 0;
	దశాంశం lat_స్థానభ్రంశం = -0.01f;
	దశాంశం dist = CONSTANTS_RADIUS_యొక్క_EARTH_F * M_PI_F / 180.f * lat_స్థానభ్రంశం;

	ద్విగుణ lat_లక్ష్యం = 0;
	ద్విగుణ lon_లక్ష్యం = 0;

	// WHEN: we get the next waypoint
	waypoint_from_heading_and_దూరం(lat_ప్రారంభం, lon_ప్రారంభం, bearing, dist, &lat_లక్ష్యం, &lon_లక్ష్యం);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(static_cast<దశాంశం>(lat_ప్రారంభం) + lat_స్థానభ్రంశం, lat_లక్ష్యం);
	EXPECT_DOUBLE_EQ(lon_ప్రారంభం, lon_లక్ష్యం);
}

TEST_F(GeoTest, waypoint_from_heading_and_ధనాత్మక_దూరం)
{
	// GIVEN: a starting waypoint, a heading and positive distance
	ద్విగుణ lat_ప్రారంభం = -33;
	ద్విగుణ lon_ప్రారంభం = 18;
	దశాంశం bearing = 0;
	దశాంశం lat_స్థానభ్రంశం = 0.01f;
	దశాంశం dist = CONSTANTS_RADIUS_యొక్క_EARTH_F * M_PI_F / 180.f * lat_స్థానభ్రంశం;

	ద్విగుణ lat_లక్ష్యం = 0;
	ద్విగుణ lon_లక్ష్యం = 0;

	// WHEN: we get the next waypoint
	waypoint_from_heading_and_దూరం(lat_ప్రారంభం, lon_ప్రారంభం, bearing, dist, &lat_లక్ష్యం, &lon_లక్ష్యం);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(static_cast<దశాంశం>(lat_ప్రారంభం) + lat_స్థానభ్రంశం, lat_లక్ష్యం);
	EXPECT_DOUBLE_EQ(lon_ప్రారంభం, lon_లక్ష్యం);
}

TEST_F(GeoTest, waypoint_from_పంక్తి_and_zero_దూరం)
{
	// GIVEN: a starting waypoint, a heading and a distance of 0
	ద్విగుణ lat_ప్రారంభం = -33;
	ద్విగుణ lon_ప్రారంభం = 18;
	ద్విగుణ lat_చివర = -32;
	ద్విగుణ lon_చివర = 18;
	దశాంశం dist = 0;

	ద్విగుణ lat_లక్ష్యం = 0;
	ద్విగుణ lon_లక్ష్యం = 0;

	// WHEN: we get the next waypoint
	create_waypoint_from_పంక్తి_and_dist(lat_ప్రారంభం, lon_ప్రారంభం, lat_చివర, lon_చివర, dist, &lat_లక్ష్యం, &lon_లక్ష్యం);

	// THEN: it should be the same
	EXPECT_DOUBLE_EQ(lat_ప్రారంభం, lat_లక్ష్యం);
	EXPECT_DOUBLE_EQ(lon_ప్రారంభం, lon_లక్ష్యం);
}

TEST_F(GeoTest, waypoint_from_పంక్తి_and_ధనాత్మక_దూరం)
{
	// GIVEN: a starting waypoint, a heading and a positive distance
	ద్విగుణ lat_ప్రారంభం = -33;
	ద్విగుణ lon_ప్రారంభం = 18;
	ద్విగుణ lat_స్థానభ్రంశం = 0.01;
	ద్విగుణ lat_చివర = lat_ప్రారంభం + lat_స్థానభ్రంశం;
	ద్విగుణ lon_చివర = 18;
	దశాంశం dist = CONSTANTS_RADIUS_యొక్క_EARTH_F * M_PI_F / 180.f * static_cast<దశాంశం>(lat_స్థానభ్రంశం);

	ద్విగుణ lat_లక్ష్యం = 0;
	ద్విగుణ lon_లక్ష్యం = 0;

	// WHEN: we get the next waypoint
	create_waypoint_from_పంక్తి_and_dist(lat_ప్రారంభం, lon_ప్రారంభం, lat_చివర, lon_చివర, dist, &lat_లక్ష్యం, &lon_లక్ష్యం);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(lat_ప్రారంభం + lat_స్థానభ్రంశం, lat_లక్ష్యం);
	EXPECT_DOUBLE_EQ(lon_ప్రారంభం, lon_లక్ష్యం);
}


TEST_F(GeoTest, waypoint_from_పంక్తి_and_negative_దూరం)
{
	// GIVEN: a starting waypoint, a heading and a negative distance
	ద్విగుణ lat_ప్రారంభం = -33;
	ద్విగుణ lon_ప్రారంభం = 18;
	ద్విగుణ lat_స్థానభ్రంశం = 0.01;
	ద్విగుణ lat_చివర = lat_ప్రారంభం + lat_స్థానభ్రంశం;
	ద్విగుణ lon_చివర = 18;
	దశాంశం dist = -CONSTANTS_RADIUS_యొక్క_EARTH_F * M_PI_F / 180.f * static_cast<దశాంశం>(lat_స్థానభ్రంశం);

	ద్విగుణ lat_లక్ష్యం = 0;
	ద్విగుణ lon_లక్ష్యం = 0;

	// WHEN: we get the next waypoint
	create_waypoint_from_పంక్తి_and_dist(lat_ప్రారంభం, lon_ప్రారంభం, lat_చివర, lon_చివర, dist, &lat_లక్ష్యం, &lon_లక్ష్యం);

	// THEN: it should be the same
	EXPECT_FLOAT_EQ(lat_ప్రారంభం - lat_స్థానభ్రంశం, lat_లక్ష్యం);
	EXPECT_DOUBLE_EQ(lon_ప్రారంభం, lon_లక్ష్యం);
}

TEST_F(GeoTest, get_దూరం_గమ్యం_Arc_Outside_Sector)
{
	// GIVEN: an arc at a mid-latitude (so the longitude cosine scaling matters), sweeping from
	// due-North of the center (start) to due-East (end).
	స్థిరరాశి ద్విగుణ lat_center = 47.0;
	స్థిరరాశి ద్విగుణ lon_center = 8.0;
	స్థిరరాశి దశాంశం radius = 10000.f;            // [m]
	స్థిరరాశి దశాంశం arc_ప్రారంభం_bearing = 0.f;     // start point due North of the center
	స్థిరరాశి దశాంశం arc_sweep = M_PI_F / 2.f;    // end point due East of the center
	స్థిరరాశి ద్విగుణ mpd = 111111.0;
	స్థిరరాశి ద్విగుణ cos_lat = కోజ్యా(math::radians(lat_center));

	// The implementation locates the endpoints with an equirectangular approximation; mirror it so we
	// can predict which endpoint is nearer and at what distance.
	స్వయంచాలక endpoint = [&](ద్విగుణ bearing, ద్విగుణ & lat, ద్విగుణ & lon) {
		lat = lat_center + static_cast<ద్విగుణ>(radius) * కోజ్యా(bearing) / mpd;
		lon = lon_center + static_cast<ద్విగుణ>(radius) * జ్యా(bearing) / (mpd * cos_lat);
	};
	ద్విగుణ lat_ప్రారంభం, lon_ప్రారంభం, lat_చివర, lon_చివర;
	endpoint(static_cast<ద్విగుణ>(arc_ప్రారంభం_bearing), lat_ప్రారంభం, lon_ప్రారంభం);
	endpoint(static_cast<ద్విగుణ>(arc_ప్రారంభం_bearing + arc_sweep), lat_చివర, lon_చివర);

	crosstrack_error_s err{};
	ద్విగుణ lat_v, lon_v;

	// WHEN: the vehicle is outside the wedge on the start side (0.5 rad before the start bearing)
	waypoint_from_heading_and_దూరం(lat_center, lon_center, arc_ప్రారంభం_bearing - 0.5f, radius, &lat_v, &lon_v);
	get_దూరం_గమ్యం_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ప్రారంభం_bearing, arc_sweep);
	// THEN: the result is the distance to the nearer (start) endpoint, not past the end
	EXPECT_LT(get_దూరం_గమ్యం_తదుపరి_waypoint(lat_v, lon_v, lat_ప్రారంభం, lon_ప్రారంభం),
		  get_దూరం_గమ్యం_తదుపరి_waypoint(lat_v, lon_v, lat_చివర, lon_చివర));
	EXPECT_NEAR(err.దూరం, get_దూరం_గమ్యం_తదుపరి_waypoint(lat_v, lon_v, lat_ప్రారంభం, lon_ప్రారంభం), 0.5f);
	EXPECT_FALSE(err.past_చివర);

	// WHEN: the vehicle is outside the wedge on the end side (0.5 rad past the end bearing)
	waypoint_from_heading_and_దూరం(lat_center, lon_center, arc_ప్రారంభం_bearing + arc_sweep + 0.5f, radius, &lat_v, &lon_v);
	get_దూరం_గమ్యం_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ప్రారంభం_bearing, arc_sweep);
	// THEN: the result is the distance to the nearer (end) endpoint, past the end
	EXPECT_LT(get_దూరం_గమ్యం_తదుపరి_waypoint(lat_v, lon_v, lat_చివర, lon_చివర),
		  get_దూరం_గమ్యం_తదుపరి_waypoint(lat_v, lon_v, lat_ప్రారంభం, lon_ప్రారంభం));
	EXPECT_NEAR(err.దూరం, get_దూరం_గమ్యం_తదుపరి_waypoint(lat_v, lon_v, lat_చివర, lon_చివర), 0.5f);
	EXPECT_TRUE(err.past_చివర);
}

TEST_F(GeoTest, get_దూరం_గమ్యం_Arc_Inside_Sector)
{
	// GIVEN: an arc sweeping from due-North (start) to due-East (end) of the center
	స్థిరరాశి ద్విగుణ lat_center = 47.0;
	స్థిరరాశి ద్విగుణ lon_center = 8.0;
	స్థిరరాశి దశాంశం radius = 10000.f;
	స్థిరరాశి దశాంశం arc_ప్రారంభం_bearing = 0.f;
	స్థిరరాశి దశాంశం arc_sweep = M_PI_F / 2.f;
	స్థిరరాశి దశాంశం mid_bearing = arc_ప్రారంభం_bearing + arc_sweep / 2.f;   // pi/4, inside the wedge

	crosstrack_error_s err{};
	ద్విగుణ lat_v, lon_v;

	// WHEN: the vehicle is exactly on the arc (on the circle, inside the wedge). This is the case the
	// previous in-sector logic mis-classified and reported kilometers off.
	waypoint_from_heading_and_దూరం(lat_center, lon_center, mid_bearing, radius, &lat_v, &lon_v);
	get_దూరం_గమ్యం_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ప్రారంభం_bearing, arc_sweep);
	// THEN: distance to the arc is ~zero
	EXPECT_NEAR(err.దూరం, 0.f, 1.f);
	EXPECT_FALSE(err.past_చివర);

	// WHEN: the vehicle is inside the circle, within the wedge (half a radius from the center)
	waypoint_from_heading_and_దూరం(lat_center, lon_center, mid_bearing, radius / 2.f, &lat_v, &lon_v);
	get_దూరం_గమ్యం_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ప్రారంభం_bearing, arc_sweep);
	// THEN: the distance is the radial gap to the arc (~radius/2)
	EXPECT_NEAR(err.దూరం, radius / 2.f, 1.f);
	EXPECT_FALSE(err.past_చివర);

	// WHEN: the vehicle is outside the circle, within the wedge (1.5 radius from the center)
	waypoint_from_heading_and_దూరం(lat_center, lon_center, mid_bearing, 1.5f * radius, &lat_v, &lon_v);
	get_దూరం_గమ్యం_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ప్రారంభం_bearing, arc_sweep);
	// THEN: the distance is again the radial gap (~radius/2)
	EXPECT_NEAR(err.దూరం, radius / 2.f, 1.f);
	EXPECT_FALSE(err.past_చివర);
}

TEST_F(GeoTest, get_దూరం_గమ్యం_Arc_Negative_Sweep)
{
	// GIVEN: the same NE-quadrant arc, but defined from due-East (start) sweeping negatively to North (end)
	స్థిరరాశి ద్విగుణ lat_center = 47.0;
	స్థిరరాశి ద్విగుణ lon_center = 8.0;
	స్థిరరాశి దశాంశం radius = 10000.f;
	స్థిరరాశి దశాంశం arc_ప్రారంభం_bearing = M_PI_F / 2.f;
	స్థిరరాశి దశాంశం arc_sweep = -M_PI_F / 2.f;

	crosstrack_error_s err{};
	ద్విగుణ lat_v, lon_v;

	// WHEN: the vehicle is on the arc midpoint (bearing pi/4 from the center) -> in sector
	waypoint_from_heading_and_దూరం(lat_center, lon_center, M_PI_F / 4.f, radius, &lat_v, &lon_v);
	get_దూరం_గమ్యం_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ప్రారంభం_bearing, arc_sweep);
	// THEN: distance to the arc is ~zero
	EXPECT_NEAR(err.దూరం, 0.f, 1.f);
	EXPECT_FALSE(err.past_చివర);

	// WHEN: the vehicle is due South of the center, far outside the wedge -> out of sector
	waypoint_from_heading_and_దూరం(lat_center, lon_center, M_PI_F, radius, &lat_v, &lon_v);
	get_దూరం_గమ్యం_arc(&err, lat_v, lon_v, lat_center, lon_center, radius, arc_ప్రారంభం_bearing, arc_sweep);
	// THEN: it is NOT treated as on the arc (the old negative-sweep sector math returned ~zero here)
	EXPECT_GT(err.దూరం, radius);
}
