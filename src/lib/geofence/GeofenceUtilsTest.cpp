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

#অন্তর্ভাব <gtest/gtest.h>
#অন্তর্ভাব "geofence_utils.h"

using নামাকাশ matrix;
using SS = geofence_utils::Seg_Seg_ফলাফল;
using Add_ফলাফল = geofence_utils::PlannerPolygons::Add_ফলাফল;

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
	ধ্রুবক পূর্ণসংখ্যা D = 10'000'000;  // 100 km
	ধ্রুবক পূর্ণসংখ্যা d = 1;           // 1 cm
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

কাঠামো SegmentIntersectionCase {
	ধ্রুবক অক্ষর *name;
	Vector2d a, b, c, d;
	যৌক্তিক intersects;
};

স্থির ধ্রুবক SegmentIntersectionCase kSegmentIntersectionCases[] = {
	// Contact counts, even when the segments only share an endpoint or overlap along a line.
	{"Crossing", {0, 0}, {4, 4}, {0, 4}, {4, 0}, সত্য},
	{"NegativeCoordinates", {-4, -4}, {0, 0}, {-4, 0}, {0, -4}, সত্য},
	{"SharedEndpoint", {0, 0}, {4, 0}, {4, 0}, {4, 4}, সত্য},
	{"EndpointOnInterior", {0, 0}, {4, 0}, {2, 0}, {2, 4}, সত্য},
	{"CollinearOverlap", {0, 0}, {4, 0}, {2, 0}, {6, 0}, সত্য},
	{"ContainedSegment", {0, 0}, {4, 4}, {1, 1}, {3, 3}, সত্য},
	{"IdenticalSegments", {0, 0}, {4, 4}, {0, 0}, {4, 4}, সত্য},
	{"CollinearEndpointContact", {0, 0}, {2, 2}, {2, 2}, {4, 4}, সত্য},
	// A shared line or overlapping bounding boxes alone do not imply an intersection.
	{"HorizontalGap", {0, 0}, {1, 0}, {2, 0}, {3, 0}, মিথ্যা},
	{"VerticalGap", {0, 0}, {0, 1}, {0, 2}, {0, 3}, মিথ্যা},
	{"SlantedGap", {0, 0}, {1, 1}, {2, 2}, {3, 3}, মিথ্যা},
	{"ParallelWithOverlappingBounds", {0, 0}, {4, 4}, {0, 1}, {3, 4}, মিথ্যা},
	{"NonparallelWithOverlappingBounds", {0, 0}, {2, 2}, {0, 1}, {1, 3}, মিথ্যা},
	// c lies on the line through ab but past b, so only the second straddle test rejects it.
	{"EndpointOnLineBeyondSegment", {0, 0}, {4, 0}, {5, 0}, {3, 2}, মিথ্যা},
	// Equal endpoints turn a segment into a single point.
	{"PointOnInterior", {2, 2}, {2, 2}, {0, 0}, {4, 4}, সত্য},
	{"PointOnEndpoint", {0, 0}, {0, 0}, {0, 0}, {4, 4}, সত্য},
	{"PointBeyondEndpoint", {5, 5}, {5, 5}, {0, 0}, {4, 4}, মিথ্যা},
	{"PointOffLineWithinBounds", {1, 2}, {1, 2}, {0, 0}, {4, 4}, মিথ্যা},
	{"CoincidentPoints", {1, 2}, {1, 2}, {1, 2}, {1, 2}, সত্য},
	{"DistinctPoints", {1, 2}, {1, 2}, {2, 1}, {2, 1}, মিথ্যা},
	// At this latitude, 0.0000001 degrees is about 1 cm and is lost with float coordinates.
	{"LatLonCentimetreCrossing", {47.0, 8.0}, {47.0000002, 8.0}, {47.0000001, 7.9999999}, {47.0000001, 8.0000001}, সত্য},
	{"LatLonCentimetreGap", {47.0, 8.0}, {47.0000001, 8.0}, {47.0000002, 8.0}, {47.0000003, 8.0}, মিথ্যা},
	// A shallow crossing over about 76 km, with roughly 1 cm offsets at either end.
	{"LatLonShallowCrossing", {47.0, 8.0}, {47.0, 9.0}, {46.9999999, 8.0}, {47.0000001, 9.0}, সত্য},
};

শ্রেণী GeofenceSegmentIntersectionTest : সর্বজনীন ::testing::TestWithParam<SegmentIntersectionCase>
{
};

TEST_P(GeofenceSegmentIntersectionTest, SegmentsIntersectInclusive)
{
	ধ্রুবক auto &test = GetParam();

	// Reversing endpoints or swapping the segments must not change the answer.
	জন্য (যৌক্তিক reverse_ab : {মিথ্যা, সত্য}) {
		জন্য (যৌক্তিক reverse_cd : {মিথ্যা, সত্য}) {
			ধ্রুবক auto &a = reverse_ab ? test.b : test.a;
			ধ্রুবক auto &b = reverse_ab ? test.a : test.b;
			ধ্রুবক auto &c = reverse_cd ? test.d : test.c;
			ধ্রুবক auto &d = reverse_cd ? test.c : test.d;

			EXPECT_EQ(test.intersects, geofence_utils::segmentsIntersectInclusive(a, b, c, d));
			EXPECT_EQ(test.intersects, geofence_utils::segmentsIntersectInclusive(c, d, a, b));
		}
	}
}

INSTANTIATE_TEST_SUITE_P(GeofenceUtils, GeofenceSegmentIntersectionTest,
			 ::testing::মানসমূহ_In(kSegmentIntersectionCases),
			 [](ধ্রুবক ::testing::TestParamInfo<SegmentIntersectionCase> &test_info)
{
	ফেরত test_info.param.name;
});

কাঠামো PointSegmentDistanceCase {
	ধ্রুবক অক্ষর *name;
	Vector2d point, a, b;
	দ্বিগুণ distance_squared;
};

স্থির ধ্রুবক PointSegmentDistanceCase kPointSegmentDistanceCases[] = {
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

শ্রেণী GeofencePointSegmentDistanceTest : সর্বজনীন ::testing::TestWithParam<PointSegmentDistanceCase> {};

TEST_P(GeofencePointSegmentDistanceTest, SquaredDistance)
{
	ধ্রুবক auto &test = GetParam();
	EXPECT_DOUBLE_EQ(test.distance_squared, geofence_utils::pointToSegmentDistanceSquared(test.point, test.a, test.b));
	EXPECT_DOUBLE_EQ(test.distance_squared, geofence_utils::pointToSegmentDistanceSquared(test.point, test.b, test.a));
}

INSTANTIATE_TEST_SUITE_P(GeofenceUtils, GeofencePointSegmentDistanceTest,
			 ::testing::মানসমূহ_In(kPointSegmentDistanceCases),
			 [](ধ্রুবক ::testing::TestParamInfo<PointSegmentDistanceCase> &test_info)
{
	ফেরত test_info.param.name;
});

TEST(GeofenceUtilsTest, PolygonIsCCW)
{
	ধ্রুবক Vector2f square[4] = {{0.f, 0.f}, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f}};
	EXPECT_TRUE(geofence_utils::isPolygonCCW(square, 4));
}

// ===========================================================================
// Segment vs polygon: each test names one geometric configuration and asserts
// the violation flag for both inclusion (outside disallowed) and exclusion
// (inside disallowed), under both polygon winding orders.
// ===========================================================================

constexpr পূর্ণসংখ্যা kMaxN = 8;

// Check segment/polygon interior intersection under both winding direction &
// both zone types. Note that the used lineSegmentIntersectsPolygon first adds
// the polygon to the internal representation which canonicalises orientation.
// So the internal convention (left = illegal) should hold, this is also what we
// test here.
শূন্য expectSegmentVsPolygon(ধ্রুবক Vector2f *vertices, পূর্ণসংখ্যা n,
			    ধ্রুবক Vector2f &a, ধ্রুবক Vector2f &b,
			    যৌক্তিক intersects_inclusion, যৌক্তিক intersects_exclusion)
{
	ASSERT_LE(n, kMaxN);
	Vector2f rev[kMaxN];

	জন্য (পূর্ণসংখ্যা i = 0; i < n; ++i) { rev[i] = vertices[n - 1 - i]; }

	EXPECT_EQ(intersects_inclusion, geofence_utils::সারি_Segment_Intersects_Polygon(a, b, vertices, n, সত্য));
	EXPECT_EQ(intersects_exclusion, geofence_utils::সারি_Segment_Intersects_Polygon(a, b, vertices, n, মিথ্যা));
	EXPECT_EQ(intersects_inclusion, geofence_utils::সারি_Segment_Intersects_Polygon(a, b, rev,      n, সত্য));
	EXPECT_EQ(intersects_exclusion, geofence_utils::সারি_Segment_Intersects_Polygon(a, b, rev,      n, মিথ্যা));
}

// Convex polygon (unit square)
ধ্রুবক Vector2f kSquare[4] = {{0.f, 0.f}, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f}};

TEST(GeofenceUtilsTest, SquareSegmentOutside)
{
	// Far outside: inclusion is violated (must stay inside), exclusion is fine.
	expectSegmentVsPolygon(kSquare, 4, {4.f, 5.f}, {5.f, 4.f}, সত্য, মিথ্যা);
}

TEST(GeofenceUtilsTest, SquareSegmentInside)
{
	// Strict interior: exclusion violated, inclusion fine.
	expectSegmentVsPolygon(kSquare, 4, {0.2f, 0.2f}, {0.6f, 0.5f}, মিথ্যা, সত্য);
}

TEST(GeofenceUtilsTest, SquareSegmentStrictCrossing)
{
	// Interior endpoint to exterior endpoint via a single proper edge crossing.
	expectSegmentVsPolygon(kSquare, 4, {0.5f, 0.5f}, {0.5f, 1.5f}, সত্য, সত্য);
}

TEST(GeofenceUtilsTest, SquareSegmentCrossesTwoEdges)
{
	// Exterior endpoints with two proper edge crossings.
	expectSegmentVsPolygon(kSquare, 4, {0.5f, -0.5f}, {0.5f, 1.5f}, সত্য, সত্য);
}

TEST(GeofenceUtilsTest, SquareSegmentEdgeToEdgeThroughInterior)
{
	// Endpoints on two opposite edges; the segment body lies in the interior.
	expectSegmentVsPolygon(kSquare, 4, {0.5f, 0.f}, {0.5f, 1.f}, মিথ্যা, সত্য);
}

TEST(GeofenceUtilsTest, SquareSegmentDiagonalThroughOppositeVertices)
{
	// Endpoints are two opposite polygon vertices; segment lies in the interior.
	expectSegmentVsPolygon(kSquare, 4, {0.f, 0.f}, {1.f, 1.f}, মিথ্যা, সত্য);
	// Segment passes through opposite vertices - both zones violated.
	expectSegmentVsPolygon(kSquare, 4, {-0.5f, 0.5f}, {1.5f, 1.5f}, সত্য, সত্য);
	// Diagonal through only one vertex - still both zones.
	expectSegmentVsPolygon(kSquare, 4, {2.f, 2.f}, {0.5f, 0.5f}, সত্য, সত্য);
}

TEST(GeofenceUtilsTest, SquareSegmentAlongEdge)
{
	// Segment coincides with one edge (pure graze, no strict interior or exterior).
	expectSegmentVsPolygon(kSquare, 4, {0.f, 0.f}, {1.f, 0.f}, মিথ্যা, মিথ্যা);
}

TEST(GeofenceUtilsTest, SquareSegmentExtendsBeyondEdge)
{
	// Segment overlaps edge but is longer. For exclusion this is ok
	// (non-intersecting), for inclusion this violates the outside region.
	expectSegmentVsPolygon(kSquare, 4, {0.f, 0.f}, {2.f, 0.f}, সত্য, মিথ্যা);
	expectSegmentVsPolygon(kSquare, 4, {-1.f, 0.f}, {2.f, 0.f}, সত্য, মিথ্যা);
}

TEST(GeofenceUtilsTest, SquareSegmentTangentThroughVertex)
{
	// Tangent line touching the (1,1) corner with both endpoints outside.
	// Inclusion violated (line outside), exclusion not (only boundary).
	// One case with midpoint = vertex, one !=
	expectSegmentVsPolygon(kSquare, 4, {2.f, 0.f}, {0.f, 2.f},  সত্য, মিথ্যা);
	expectSegmentVsPolygon(kSquare, 4, {2.f, 0.f}, {-1.f, 3.f}, সত্য, মিথ্যা);

	// If we nudge the point inward just a bit, we have a strict intersection
	// and both zones are violated.
	expectSegmentVsPolygon(kSquare, 4, {2.f, 0.f}, {0.f, 1.98f}, সত্য, সত্য);
}






// Nonconvex polygon (L-shape with a reflex vertex at (1,1))
ধ্রুবক Vector2f kLShape[6] = {{0.f, 0.f}, {2.f, 0.f}, {2.f, 1.f}, {1.f, 1.f}, {1.f, 2.f}, {0.f, 2.f}};

TEST(GeofenceUtilsTest, LShapeSegmentDisjointOutside)
{
	// Entirely in the notch (the reflex cone, outside the polygon).
	expectSegmentVsPolygon(kLShape, 6, {1.5f, 1.5f}, {1.8f, 1.8f}, সত্য, মিথ্যা);
}

TEST(GeofenceUtilsTest, LShapeSegmentEntirelyInside)
{
	// Horizontal line inside bottom rectangle - inclusion non-intersecting, exclusion intersecting
	expectSegmentVsPolygon(kLShape, 6, {0.5f, 0.5f}, {1.5f, 0.5f}, মিথ্যা, সত্য);

	// Skewed line inside upper rectangle - same
	expectSegmentVsPolygon(kLShape, 6, {0.3f, 1.2f}, {0.7f, 1.8f}, মিথ্যা, সত্য);
}

TEST(GeofenceUtilsTest, LShapeSegmentCrossesNotchEdge)
{
	// Single proper crossing from the upper-rect interior into the notch.
	expectSegmentVsPolygon(kLShape, 6, {0.5f, 1.5f}, {1.5f, 1.5f}, সত্য, সত্য);
}

TEST(GeofenceUtilsTest, LShapeSegmentThroughReflexVertexInsideToInside)
{
	// Both endpoints inside, segment passes through the reflex (1,1) but
	// stays inside. Inclusion non-intersecting, exclusion intersecting.
	expectSegmentVsPolygon(kLShape, 6, {0.5f, 1.5f}, {2.f, 0.f}, মিথ্যা, সত্য);
	expectSegmentVsPolygon(kLShape, 6, {0.5f, 1.5f}, {1.5f, 0.5f}, মিথ্যা, সত্য);
}

TEST(GeofenceUtilsTest, LShapeSegmentThroughReflexVertexIntoNotch)
{
	// One endpoint inside the polygon, the other in the notch; the segment
	// crosses the boundary at the reflex vertex.
	expectSegmentVsPolygon(kLShape, 6, {0.5f, 0.5f}, {2.f, 2.f}, সত্য, সত্য);
}

TEST(GeofenceUtilsTest, LShapeSegmentBetweenTwoVerticesAcrossNotch)
{
	// Endpoints are two convex polygon vertices, (2,1) and (1,2). The chord
	// between them crosses the notch (outside the polygon), grazing the
	// polygon only at those two vertices.
	expectSegmentVsPolygon(kLShape, 6, {2.f, 1.f}, {1.f, 2.f}, সত্য, মিথ্যা);
}

TEST(GeofenceUtilsTest, LShapeSegmentTangentThroughConvexVertex)
{
	// These lines touch a vertex from outside. Inclusion -> intersection,
	// exclusion -> no intersection.

	// Tangent at (2, 0)
	expectSegmentVsPolygon(kLShape, 6, {3.f, 1.f}, {1.5f, -0.5f}, সত্য, মিথ্যা);
	// Tangent at (0, 0)
	expectSegmentVsPolygon(kLShape, 6, {-1.f, 1.f}, {1.f, -1.f}, সত্য, মিথ্যা);
	// Tangent at both (1, 2) and (2, 1)
	expectSegmentVsPolygon(kLShape, 6, {0.f, 3.f}, {3.f, 0.f}, সত্য, মিথ্যা);
}

// ===========================================================================
// addPolygon rejects input out of bounds
// ===========================================================================

TEST(GeofenceUtilsTest, AddPolygonRejectsOutOfBounds)
{
	// Square inside of bounds, 10000 km.
	দশমিক extent = 10000 * 1000; // km to m
	ধ্রুবক Vector2f square[4] = {{0.f, 0.f}, {extent, 0.f}, {extent, extent}, {0.f, extent}};

	geofence_utils::PlannerPolygons polys;
	EXPECT_EQ(polys.addPolygon(square, 4, /*is_inclusion_zone=*/মিথ্যা, /*margin=*/0.f), Add_ফলাফল::Success);

	// Square out of bounds, 12000 km.
	extent = 12000 * 1000;
	ধ্রুবক Vector2f square_larger[4] = {{0.f, 0.f}, {extent, 0.f}, {extent, extent}, {0.f, extent}};

	geofence_utils::PlannerPolygons polys_larger;
	EXPECT_EQ(polys_larger.addPolygon(square_larger, 4, /*is_inclusion_zone=*/মিথ্যা, /*margin=*/0.f), Add_ফলাফল::OutOfRange);
}

TEST(GeofenceUtilsTest, AddApproxCircleRejectsOutOfBounds)
{
	// 100m circle at origin with 10m margin - works
	geofence_utils::PlannerPolygons polys0;
	EXPECT_EQ(
		polys0.addApproxCircle(matrix::Vector2f(0, 0), 100.0f, 10.0f, মিথ্যা),
		Add_ফলাফল::Success
	);

	// 5000km circle -- same
	EXPECT_EQ(
		polys0.addApproxCircle(matrix::Vector2f(0, 0), 5000.f * 1000.f, 10.0f, মিথ্যা),
		Add_ফলাফল::Success
	);

	// 10000km circle -- fails (circle is within range but approx circle not)
	EXPECT_EQ(
		polys0.addApproxCircle(matrix::Vector2f(0, 0), 10000.f * 1000.f, 10.0f, মিথ্যা),
		Add_ফলাফল::OutOfRange
	);

	// 12000km circle -- fails
	EXPECT_EQ(
		polys0.addApproxCircle(matrix::Vector2f(0, 0), 12000.f * 1000.f, 10.0f, মিথ্যা),
		Add_ফলাফল::OutOfRange
	);

	// 10000km circle, 2000 km margin (outwards, exclusion zone) -- fails
	EXPECT_EQ(
		polys0.addApproxCircle(matrix::Vector2f(0, 0), 12000.f * 1000.f, 2000.f * 1000.f, মিথ্যা),
		Add_ফলাফল::OutOfRange
	);

	// 2000km circle 10000km out - fails
	EXPECT_EQ(
		polys0.addApproxCircle(matrix::Vector2f(0, 10000.f * 1000.f), 2000.f * 1000.f, 10.f, মিথ্যা),
		Add_ফলাফল::OutOfRange
	);

	// 100km circle 10000km out - works
	EXPECT_EQ(
		polys0.addApproxCircle(matrix::Vector2f(0, 10000.f * 1000.f), 100.f * 1000.f, 10.f, মিথ্যা),
		Add_ফলাফল::Success
	);

	// 100km circle 10000km out, 1000km margin - fails
	EXPECT_EQ(
		polys0.addApproxCircle(matrix::Vector2f(0, 10000.f * 1000.f), 100.f * 1000.f, 1000.f * 1000.f, মিথ্যা),
		Add_ফলাফল::OutOfRange
	);

}

// ===========================================================================
// addPolygon rejects non-simple input
// ===========================================================================

TEST(GeofenceUtilsTest, AddPolygonRejectsSelfIntersecting)
{
	// Figure-eight quadrilateral: 0->1->2->3 with edges (0,1) and (2,3) crossing.
	ধ্রুবক Vector2f figure_eight[4] = {{0.f, 0.f}, {10.f, 10.f}, {10.f, 0.f}, {0.f, 10.f}};

	geofence_utils::PlannerPolygons polys;
	EXPECT_EQ(polys.addPolygon(figure_eight, 4, /*is_inclusion_zone=*/মিথ্যা, /*margin=*/0.f), Add_ফলাফল::Degenerate);
}

TEST(GeofenceUtilsTest, AddPolygonAcceptsSimpleQuad)
{
	// Convex simple quadrilateral; must succeed.
	ধ্রুবক Vector2f square[4] = {{0.f, 0.f}, {10.f, 0.f}, {10.f, 10.f}, {0.f, 10.f}};

	geofence_utils::PlannerPolygons polys;
	EXPECT_EQ(polys.addPolygon(square, 4, /*is_inclusion_zone=*/মিথ্যা, /*margin=*/0.f), Add_ফলাফল::Success);
}
