/****************************************************************************
 *
 *   Copyright (C) 2026 PX4 Development Team. All rights reserved.
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
#అన్తర్భావ "geofence_utils.h"

ఉపయోగించి నామాకాశ మాతృక;
ఉపయోగించి SS = geofence_utils::Seg_Seg_ఫలితం;
ఉపయోగించి కూడు_ఫలితం = geofence_utils::PlannerPolygons::కూడు_ఫలితం;

// Low level primitives operating on fixed-point int32 coordinates.

TEST(GeofenceUtilsTest, Orient2d)
{
	// CCW turn -> +1, CW turn -> -1, collinear -> 0 (in front of, behind, between).
	EXPECT_EQ(1,  geofence_utils::orient2d(0, 0, 1, 0, 0, 1));
	EXPECT_EQ(-1, geofence_utils::orient2d(0, 0, 1, 0, 0, -1));
	EXPECT_EQ(0,  geofence_utils::orient2d(0, 0, 2, 2, 1, 1));
	EXPECT_EQ(0,  geofence_utils::orient2d(0, 0, 2, 2, 3, 3));
}

TEST(GeofenceUtilsTest, SegmentsCross)
{
	// Vertical ab vs horizontal cd just above the x-axis; strict interior crossing.
	EXPECT_EQ(SS::Cross, geofence_utils::segmentsIntersect(0, 0, 0, 100, -1, 1, 100, 1));

	// Crossing at very shallow angle - not a problem in fixed point
	స్థిరరాశి పూర్ణాంకం D = 10'000'000;  // 100 km
	స్థిరరాశి పూర్ణాంకం d = 1;           // 1 cm
	EXPECT_EQ(SS::Cross, geofence_utils::segmentsIntersect(0, 0, D, 0, 0, -d, D, d));
}

TEST(GeofenceUtilsTest, SegmentsEndpointOnInterior)
{
	// Endpoint c of cd lies strictly on the open ab (axis-aligned).
	EXPECT_EQ(SS::CInsideAB, geofence_utils::segmentsIntersect(0, 0, 0, 200, 0, 100, 100, 100));
	EXPECT_EQ(SS::AInsideCD, geofence_utils::segmentsIntersect(0, 100, 100, 100, 0, 0, 0, 200));

	// Same configuration on a slanted line.
	EXPECT_EQ(SS::CInsideAB, geofence_utils::segmentsIntersect(-100, 0, 100, 200, 0, 100, 100, 100));
	EXPECT_EQ(SS::AInsideCD, geofence_utils::segmentsIntersect(0, 100, 100, 100, -100, 0, 100, 200));
}

TEST(GeofenceUtilsTest, SegmentsCollinear)
{
	// Identical segments and sub-intervals are reported as Collinear, not Cross.
	EXPECT_EQ(SS::Collinear, geofence_utils::segmentsIntersect(0, 0, 2, 2, 1, 1, 2, 2));
	EXPECT_EQ(SS::Collinear, geofence_utils::segmentsIntersect(0, 0, 300, 0, 0, 0, 300, 0));
	EXPECT_EQ(SS::Collinear, geofence_utils::segmentsIntersect(1000, 1000, 4000, 2000, 1000, 1000, 4000, 2000));
	// Same supporting line, non-overlapping intervals.
	EXPECT_EQ(SS::Collinear, geofence_utils::segmentsIntersect(0, 0, 100, 0, 200, 0, 300, 0));
}

TEST(GeofenceUtilsTest, SegmentsDisjoint)
{
	// Parallel, non-collinear, disjoint.
	EXPECT_EQ(SS::Disjoint, geofence_utils::segmentsIntersect(0, 0, 300, 0, 1000, 1000, 4000, 2000));
}

నిర్మాణం SegmentIntersectionCase {
	స్థిరరాశి అక్షరం *name;
	Vector2d a, b, c, d;
	తార్కిక intersects;
};

స్థిర స్థిరరాశి SegmentIntersectionCase kSegmentIntersectionCases[] = {
	// Contact counts, even when the segments only share an endpoint or overlap along a line.
	{"Crossing", {0, 0}, {4, 4}, {0, 4}, {4, 0}, నిజం},
	{"NegativeCoordinates", {-4, -4}, {0, 0}, {-4, 0}, {0, -4}, నిజం},
	{"SharedEndpoint", {0, 0}, {4, 0}, {4, 0}, {4, 4}, నిజం},
	{"EndpointOnInterior", {0, 0}, {4, 0}, {2, 0}, {2, 4}, నిజం},
	{"CollinearOverlap", {0, 0}, {4, 0}, {2, 0}, {6, 0}, నిజం},
	{"ContainedSegment", {0, 0}, {4, 4}, {1, 1}, {3, 3}, నిజం},
	{"IdenticalSegments", {0, 0}, {4, 4}, {0, 0}, {4, 4}, నిజం},
	{"CollinearEndpointContact", {0, 0}, {2, 2}, {2, 2}, {4, 4}, నిజం},
	// A shared line or overlapping bounding boxes alone do not imply an intersection.
	{"HorizontalGap", {0, 0}, {1, 0}, {2, 0}, {3, 0}, అబద్ధం},
	{"VerticalGap", {0, 0}, {0, 1}, {0, 2}, {0, 3}, అబద్ధం},
	{"SlantedGap", {0, 0}, {1, 1}, {2, 2}, {3, 3}, అబద్ధం},
	{"ParallelWithOverlappingBounds", {0, 0}, {4, 4}, {0, 1}, {3, 4}, అబద్ధం},
	{"NonparallelWithOverlappingBounds", {0, 0}, {2, 2}, {0, 1}, {1, 3}, అబద్ధం},
	// c lies on the line through ab but past b, so only the second straddle test rejects it.
	{"EndpointOnLineBeyondSegment", {0, 0}, {4, 0}, {5, 0}, {3, 2}, అబద్ధం},
	// Equal endpoints turn a segment into a single point.
	{"PointOnInterior", {2, 2}, {2, 2}, {0, 0}, {4, 4}, నిజం},
	{"PointOnEndpoint", {0, 0}, {0, 0}, {0, 0}, {4, 4}, నిజం},
	{"PointBeyondEndpoint", {5, 5}, {5, 5}, {0, 0}, {4, 4}, అబద్ధం},
	{"PointOffLineWithinBounds", {1, 2}, {1, 2}, {0, 0}, {4, 4}, అబద్ధం},
	{"CoincidentPoints", {1, 2}, {1, 2}, {1, 2}, {1, 2}, నిజం},
	{"DistinctPoints", {1, 2}, {1, 2}, {2, 1}, {2, 1}, అబద్ధం},
	// At this latitude, 0.0000001 degrees is about 1 cm and is lost with float coordinates.
	{"LatLonCentimetreCrossing", {47.0, 8.0}, {47.0000002, 8.0}, {47.0000001, 7.9999999}, {47.0000001, 8.0000001}, నిజం},
	{"LatLonCentimetreGap", {47.0, 8.0}, {47.0000001, 8.0}, {47.0000002, 8.0}, {47.0000003, 8.0}, అబద్ధం},
	// A shallow crossing over about 76 km, with roughly 1 cm offsets at either end.
	{"LatLonShallowCrossing", {47.0, 8.0}, {47.0, 9.0}, {46.9999999, 8.0}, {47.0000001, 9.0}, నిజం},
};

తరగతి GeofenceSegmentIntersectionTest : బహిరంగ ::testing::TestWithParam<SegmentIntersectionCase>
{
};

TEST_P(GeofenceSegmentIntersectionTest, SegmentsIntersectInclusive)
{
	స్థిరరాశి స్వయంచాలక &test = GetParam();

	// Reversing endpoints or swapping the segments must not change the answer.
	కొరకు (తార్కిక తిరగేయి_ab : {అబద్ధం, నిజం}) {
		కొరకు (తార్కిక తిరగేయి_cd : {అబద్ధం, నిజం}) {
			స్థిరరాశి స్వయంచాలక &a = తిరగేయి_ab ? test.b : test.a;
			స్థిరరాశి స్వయంచాలక &b = తిరగేయి_ab ? test.a : test.b;
			స్థిరరాశి స్వయంచాలక &c = తిరగేయి_cd ? test.d : test.c;
			స్థిరరాశి స్వయంచాలక &d = తిరగేయి_cd ? test.c : test.d;

			EXPECT_EQ(test.intersects, geofence_utils::segmentsIntersectInclusive(a, b, c, d));
			EXPECT_EQ(test.intersects, geofence_utils::segmentsIntersectInclusive(c, d, a, b));
		}
	}
}

INSTANTIATE_TEST_SUITE_P(GeofenceUtils, GeofenceSegmentIntersectionTest,
			 ::testing::విలువలు_లోపల(kSegmentIntersectionCases),
			 [](స్థిరరాశి ::testing::TestParamInfo<SegmentIntersectionCase> &test_info)
{
	తిరిగి test_info.param.name;
});

నిర్మాణం బిందువు_Segment_దూరం_Case {
	స్థిరరాశి అక్షరం *name;
	Vector2d బిందువు, a, b;
	ద్విగుణ దూరం_squared;
};

స్థిర స్థిరరాశి బిందువు_Segment_దూరం_Case k_బిందువు_Segment_దూరం_Cases[] = {
	// A horizontal segment makes the nearest endpoint and perpendicular distances easy to check.
	{"BeforeStart", {-2, 3}, {0, 0}, {4, 0}, 13},
	{"AfterEnd", {6, 3}, {0, 0}, {4, 0}, 13},
	{"AboveStart", {0, 3}, {0, 0}, {4, 0}, 9},
	{"AboveInterior", {2, 3}, {0, 0}, {4, 0}, 9},
	{"OnSegment", {2, 0}, {0, 0}, {4, 0}, 0},
	{"TranslatedSegment", {12, 13}, {10, 10}, {14, 10}, 9},
	{"VerticalSegment", {-1, -2}, {-4, -4}, {-4, 0}, 9},
	{"DiagonalSegment", {2, 0}, {0, 0}, {2, 2}, 2},
	{"ZeroLength", {3, 4}, {1, 1}, {1, 1}, 13},
	{"CoincidentPoint", {1, 1}, {1, 1}, {1, 1}, 0},
	// Both segments lie on 3x + 4y = 250, exactly 50 m from the origin.
	{"CircleTangent", {0, 0}, {-30, 85}, {86, -2}, 2500},
	// Over 10 km, subtracting projected squared distances can round this tangent outside the circle.
	{"LongCircleTangent", {0, 0}, {-8030, 6085}, {318, -176}, 2500},
};

తరగతి Geofence_బిందువు_Segment_దూరం_Test : బహిరంగ ::testing::TestWithParam<బిందువు_Segment_దూరం_Case> {};

TEST_P(Geofence_బిందువు_Segment_దూరం_Test, Squared_దూరం)
{
	స్థిరరాశి స్వయంచాలక &test = GetParam();
	EXPECT_DOUBLE_EQ(test.దూరం_squared, geofence_utils::బిందువు_గమ్యం_Segment_దూరం_Squared(test.బిందువు, test.a, test.b));
	EXPECT_DOUBLE_EQ(test.దూరం_squared, geofence_utils::బిందువు_గమ్యం_Segment_దూరం_Squared(test.బిందువు, test.b, test.a));
}

INSTANTIATE_TEST_SUITE_P(GeofenceUtils, Geofence_బిందువు_Segment_దూరం_Test,
			 ::testing::విలువలు_లోపల(k_బిందువు_Segment_దూరం_Cases),
			 [](స్థిరరాశి ::testing::TestParamInfo<బిందువు_Segment_దూరం_Case> &test_info)
{
	తిరిగి test_info.param.name;
});

TEST(GeofenceUtilsTest, PolygonIsCCW)
{
	స్థిరరాశి Vector2f వర్గం[4] = {{0.f, 0.f}, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f}};
	EXPECT_TRUE(geofence_utils::isPolygonCCW(వర్గం, 4));
}

// ===========================================================================
// Segment vs polygon: each test names one geometric configuration and asserts
// the violation flag for both inclusion (outside disallowed) and exclusion
// (inside disallowed), under both polygon winding orders.
// ===========================================================================

constexpr పూర్ణాంకం kMaxN = 8;

// Check segment/polygon interior intersection under both winding direction &
// both zone types. Note that the used lineSegmentIntersectsPolygon first adds
// the polygon to the internal representation which canonicalises orientation.
// So the internal convention (left = illegal) should hold, this is also what we
// test here.
శూన్య expectSegmentVsPolygon(స్థిరరాశి Vector2f *vertices, పూర్ణాంకం n,
			    స్థిరరాశి Vector2f &a, స్థిరరాశి Vector2f &b,
			    తార్కిక intersects_inclusion, తార్కిక intersects_exclusion)
{
	ASSERT_LE(n, kMaxN);
	Vector2f rev[kMaxN];

	కొరకు (పూర్ణాంకం i = 0; i < n; ++i) { rev[i] = vertices[n - 1 - i]; }

	EXPECT_EQ(intersects_inclusion, geofence_utils::పంక్తి_Segment_Intersects_Polygon(a, b, vertices, n, నిజం));
	EXPECT_EQ(intersects_exclusion, geofence_utils::పంక్తి_Segment_Intersects_Polygon(a, b, vertices, n, అబద్ధం));
	EXPECT_EQ(intersects_inclusion, geofence_utils::పంక్తి_Segment_Intersects_Polygon(a, b, rev,      n, నిజం));
	EXPECT_EQ(intersects_exclusion, geofence_utils::పంక్తి_Segment_Intersects_Polygon(a, b, rev,      n, అబద్ధం));
}

// Convex polygon (unit square)
స్థిరరాశి Vector2f k_వర్గం[4] = {{0.f, 0.f}, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f}};

TEST(GeofenceUtilsTest, వర్గం_Segment_Outside)
{
	// Far outside: inclusion is violated (must stay inside), exclusion is fine.
	expectSegmentVsPolygon(k_వర్గం, 4, {4.f, 5.f}, {5.f, 4.f}, నిజం, అబద్ధం);
}

TEST(GeofenceUtilsTest, వర్గం_Segment_Inside)
{
	// Strict interior: exclusion violated, inclusion fine.
	expectSegmentVsPolygon(k_వర్గం, 4, {0.2f, 0.2f}, {0.6f, 0.5f}, అబద్ధం, నిజం);
}

TEST(GeofenceUtilsTest, వర్గం_Segment_Strict_దాటడం)
{
	// Interior endpoint to exterior endpoint via a single proper edge crossing.
	expectSegmentVsPolygon(k_వర్గం, 4, {0.5f, 0.5f}, {0.5f, 1.5f}, నిజం, నిజం);
}

TEST(GeofenceUtilsTest, వర్గం_Segment_Crosses_Two_అంచులు)
{
	// Exterior endpoints with two proper edge crossings.
	expectSegmentVsPolygon(k_వర్గం, 4, {0.5f, -0.5f}, {0.5f, 1.5f}, నిజం, నిజం);
}

TEST(GeofenceUtilsTest, వర్గం_Segment_అంచు_గమ్యం_అంచు_Through_Interior)
{
	// Endpoints on two opposite edges; the segment body lies in the interior.
	expectSegmentVsPolygon(k_వర్గం, 4, {0.5f, 0.f}, {0.5f, 1.f}, అబద్ధం, నిజం);
}

TEST(GeofenceUtilsTest, వర్గం_Segment_Diagonal_Through_Opposite_Vertices)
{
	// Endpoints are two opposite polygon vertices; segment lies in the interior.
	expectSegmentVsPolygon(k_వర్గం, 4, {0.f, 0.f}, {1.f, 1.f}, అబద్ధం, నిజం);
	// Segment passes through opposite vertices - both zones violated.
	expectSegmentVsPolygon(k_వర్గం, 4, {-0.5f, 0.5f}, {1.5f, 1.5f}, నిజం, నిజం);
	// Diagonal through only one vertex - still both zones.
	expectSegmentVsPolygon(k_వర్గం, 4, {2.f, 2.f}, {0.5f, 0.5f}, నిజం, నిజం);
}

TEST(GeofenceUtilsTest, వర్గం_Segment_Along_అంచు)
{
	// Segment coincides with one edge (pure graze, no strict interior or exterior).
	expectSegmentVsPolygon(k_వర్గం, 4, {0.f, 0.f}, {1.f, 0.f}, అబద్ధం, అబద్ధం);
}

TEST(GeofenceUtilsTest, వర్గం_Segment_Extends_Beyond_అంచు)
{
	// Segment overlaps edge but is longer. For exclusion this is ok
	// (non-intersecting), for inclusion this violates the outside region.
	expectSegmentVsPolygon(k_వర్గం, 4, {0.f, 0.f}, {2.f, 0.f}, నిజం, అబద్ధం);
	expectSegmentVsPolygon(k_వర్గం, 4, {-1.f, 0.f}, {2.f, 0.f}, నిజం, అబద్ధం);
}

TEST(GeofenceUtilsTest, వర్గం_Segment_Tangent_Through_శీర్షం)
{
	// Tangent line touching the (1,1) corner with both endpoints outside.
	// Inclusion violated (line outside), exclusion not (only boundary).
	// One case with midpoint = vertex, one !=
	expectSegmentVsPolygon(k_వర్గం, 4, {2.f, 0.f}, {0.f, 2.f},  నిజం, అబద్ధం);
	expectSegmentVsPolygon(k_వర్గం, 4, {2.f, 0.f}, {-1.f, 3.f}, నిజం, అబద్ధం);

	// If we nudge the point inward just a bit, we have a strict intersection
	// and both zones are violated.
	expectSegmentVsPolygon(k_వర్గం, 4, {2.f, 0.f}, {0.f, 1.98f}, నిజం, నిజం);
}






// Nonconvex polygon (L-shape with a reflex vertex at (1,1))
స్థిరరాశి Vector2f kLShape[6] = {{0.f, 0.f}, {2.f, 0.f}, {2.f, 1.f}, {1.f, 1.f}, {1.f, 2.f}, {0.f, 2.f}};

TEST(GeofenceUtilsTest, LShapeSegmentDisjointOutside)
{
	// Entirely in the notch (the reflex cone, outside the polygon).
	expectSegmentVsPolygon(kLShape, 6, {1.5f, 1.5f}, {1.8f, 1.8f}, నిజం, అబద్ధం);
}

TEST(GeofenceUtilsTest, LShapeSegmentEntirelyInside)
{
	// Horizontal line inside bottom rectangle - inclusion non-intersecting, exclusion intersecting
	expectSegmentVsPolygon(kLShape, 6, {0.5f, 0.5f}, {1.5f, 0.5f}, అబద్ధం, నిజం);

	// Skewed line inside upper rectangle - same
	expectSegmentVsPolygon(kLShape, 6, {0.3f, 1.2f}, {0.7f, 1.8f}, అబద్ధం, నిజం);
}

TEST(GeofenceUtilsTest, LShape_Segment_Crosses_Notch_అంచు)
{
	// Single proper crossing from the upper-rect interior into the notch.
	expectSegmentVsPolygon(kLShape, 6, {0.5f, 1.5f}, {1.5f, 1.5f}, నిజం, నిజం);
}

TEST(GeofenceUtilsTest, LShape_Segment_Through_Reflex_శీర్షం_Inside_గమ్యం_Inside)
{
	// Both endpoints inside, segment passes through the reflex (1,1) but
	// stays inside. Inclusion non-intersecting, exclusion intersecting.
	expectSegmentVsPolygon(kLShape, 6, {0.5f, 1.5f}, {2.f, 0.f}, అబద్ధం, నిజం);
	expectSegmentVsPolygon(kLShape, 6, {0.5f, 1.5f}, {1.5f, 0.5f}, అబద్ధం, నిజం);
}

TEST(GeofenceUtilsTest, LShape_Segment_Through_Reflex_శీర్షం_Into_Notch)
{
	// One endpoint inside the polygon, the other in the notch; the segment
	// crosses the boundary at the reflex vertex.
	expectSegmentVsPolygon(kLShape, 6, {0.5f, 0.5f}, {2.f, 2.f}, నిజం, నిజం);
}

TEST(GeofenceUtilsTest, LShapeSegmentBetweenTwoVerticesAcrossNotch)
{
	// Endpoints are two convex polygon vertices, (2,1) and (1,2). The chord
	// between them crosses the notch (outside the polygon), grazing the
	// polygon only at those two vertices.
	expectSegmentVsPolygon(kLShape, 6, {2.f, 1.f}, {1.f, 2.f}, నిజం, అబద్ధం);
}

TEST(GeofenceUtilsTest, LShape_Segment_Tangent_Through_Convex_శీర్షం)
{
	// These lines touch a vertex from outside. Inclusion -> intersection,
	// exclusion -> no intersection.

	// Tangent at (2, 0)
	expectSegmentVsPolygon(kLShape, 6, {3.f, 1.f}, {1.5f, -0.5f}, నిజం, అబద్ధం);
	// Tangent at (0, 0)
	expectSegmentVsPolygon(kLShape, 6, {-1.f, 1.f}, {1.f, -1.f}, నిజం, అబద్ధం);
	// Tangent at both (1, 2) and (2, 1)
	expectSegmentVsPolygon(kLShape, 6, {0.f, 3.f}, {3.f, 0.f}, నిజం, అబద్ధం);
}

// ===========================================================================
// addPolygon rejects input out of bounds
// ===========================================================================

TEST(GeofenceUtilsTest, కూడు_Polygon_Rejects_బహిర్గతం_యొక్క_Bounds)
{
	// Square inside of bounds, 10000 km.
	దశాంశం extent = 10000 * 1000; // km to m
	స్థిరరాశి Vector2f వర్గం[4] = {{0.f, 0.f}, {extent, 0.f}, {extent, extent}, {0.f, extent}};

	geofence_utils::PlannerPolygons polys;
	EXPECT_EQ(polys.కూడు_Polygon(వర్గం, 4, /*is_inclusion_zone=*/అబద్ధం, /*margin=*/0.f), కూడు_ఫలితం::Success);

	// Square out of bounds, 12000 km.
	extent = 12000 * 1000;
	స్థిరరాశి Vector2f వర్గం_larger[4] = {{0.f, 0.f}, {extent, 0.f}, {extent, extent}, {0.f, extent}};

	geofence_utils::PlannerPolygons polys_larger;
	EXPECT_EQ(polys_larger.కూడు_Polygon(వర్గం_larger, 4, /*is_inclusion_zone=*/అబద్ధం, /*margin=*/0.f), కూడు_ఫలితం::బహిర్గతం_యొక్క_Range);
}

TEST(GeofenceUtilsTest, కూడు_Approx_Circle_Rejects_బహిర్గతం_యొక్క_Bounds)
{
	// 100m circle at origin with 10m margin - works
	geofence_utils::PlannerPolygons polys0;
	EXPECT_EQ(
		polys0.కూడు_Approx_Circle(మాతృక::Vector2f(0, 0), 100.0f, 10.0f, అబద్ధం),
		కూడు_ఫలితం::Success
	);

	// 5000km circle -- same
	EXPECT_EQ(
		polys0.కూడు_Approx_Circle(మాతృక::Vector2f(0, 0), 5000.f * 1000.f, 10.0f, అబద్ధం),
		కూడు_ఫలితం::Success
	);

	// 10000km circle -- fails (circle is within range but approx circle not)
	EXPECT_EQ(
		polys0.కూడు_Approx_Circle(మాతృక::Vector2f(0, 0), 10000.f * 1000.f, 10.0f, అబద్ధం),
		కూడు_ఫలితం::బహిర్గతం_యొక్క_Range
	);

	// 12000km circle -- fails
	EXPECT_EQ(
		polys0.కూడు_Approx_Circle(మాతృక::Vector2f(0, 0), 12000.f * 1000.f, 10.0f, అబద్ధం),
		కూడు_ఫలితం::బహిర్గతం_యొక్క_Range
	);

	// 10000km circle, 2000 km margin (outwards, exclusion zone) -- fails
	EXPECT_EQ(
		polys0.కూడు_Approx_Circle(మాతృక::Vector2f(0, 0), 12000.f * 1000.f, 2000.f * 1000.f, అబద్ధం),
		కూడు_ఫలితం::బహిర్గతం_యొక్క_Range
	);

	// 2000km circle 10000km out - fails
	EXPECT_EQ(
		polys0.కూడు_Approx_Circle(మాతృక::Vector2f(0, 10000.f * 1000.f), 2000.f * 1000.f, 10.f, అబద్ధం),
		కూడు_ఫలితం::బహిర్గతం_యొక్క_Range
	);

	// 100km circle 10000km out - works
	EXPECT_EQ(
		polys0.కూడు_Approx_Circle(మాతృక::Vector2f(0, 10000.f * 1000.f), 100.f * 1000.f, 10.f, అబద్ధం),
		కూడు_ఫలితం::Success
	);

	// 100km circle 10000km out, 1000km margin - fails
	EXPECT_EQ(
		polys0.కూడు_Approx_Circle(మాతృక::Vector2f(0, 10000.f * 1000.f), 100.f * 1000.f, 1000.f * 1000.f, అబద్ధం),
		కూడు_ఫలితం::బహిర్గతం_యొక్క_Range
	);

}

// ===========================================================================
// addPolygon rejects non-simple input
// ===========================================================================

TEST(GeofenceUtilsTest, కూడు_Polygon_Rejects_Self_Intersecting)
{
	// Figure-eight quadrilateral: 0->1->2->3 with edges (0,1) and (2,3) crossing.
	స్థిరరాశి Vector2f figure_eight[4] = {{0.f, 0.f}, {10.f, 10.f}, {10.f, 0.f}, {0.f, 10.f}};

	geofence_utils::PlannerPolygons polys;
	EXPECT_EQ(polys.కూడు_Polygon(figure_eight, 4, /*is_inclusion_zone=*/అబద్ధం, /*margin=*/0.f), కూడు_ఫలితం::Degenerate);
}

TEST(GeofenceUtilsTest, కూడు_Polygon_స్వీకరించు_Simple_Quad)
{
	// Convex simple quadrilateral; must succeed.
	స్థిరరాశి Vector2f వర్గం[4] = {{0.f, 0.f}, {10.f, 0.f}, {10.f, 10.f}, {0.f, 10.f}};

	geofence_utils::PlannerPolygons polys;
	EXPECT_EQ(polys.కూడు_Polygon(వర్గం, 4, /*is_inclusion_zone=*/అబద్ధం, /*margin=*/0.f), కూడు_ఫలితం::Success);
}
