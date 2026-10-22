/****************************************************************************
 *
 *   Copyright (c) 2013-2026 PX4 Development Team. All rights reserved.
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
 * @file geofence_utils.h
 * Pure geometry functions for geofence point-in-polygon and point-in-circle tests.
 */

#pragma once

#ଅନ୍ତର୍ଭାଵ <cmath>
#ଅନ୍ତର୍ଭାଵ <cstdint>
#ଅନ୍ତର୍ଭାଵ <cstdlib>

#ଅନ୍ତର୍ଭାଵ <matrix/math.hpp>

#ଅନ୍ତର୍ଭାଵ <dataman/dataman.h>

namespace geofence_utils
{

ସ୍ଥିର constexpr ଦଶମିକ CM_PER_M  = 100.f;

// True iff `m_value` (metres) fits into the planner's fixed-point
// representation. intersectsInsideOf doubles cm coordinates internally, so the
// usable range is 2^30 cm = 10,737 km from the planner's reference.
ଧାଡ଼ିଭିତରେ bool inFixedPointRange(ଦଶମିକ m_ମୂଲ୍ୟ) { ଫେରନ୍ତୁ fabsf(m_ମୂଲ୍ୟ) < (1u << 30) / CM_PER_M; }

/**
 * Sign of the signed area of triangle abc -- the fundamental 2D orientation
 * predicate. Returns:
 *   +1, if c is left of a->b (CCW turn)
 *   -1, if c is right of a->b (CW turn)
 *    0, if collinear
 *
 * Refs:
 *
 * Fundamental ideas, usage of orient2d for higher level geometry:
 * Lecture Notes on Geometric Robustness, Jonathan Richard Shewchuk
 * https://perso.uclouvain.be/jean-francois.remacle/LMECA2170/robnotes.pdf
 *
 * The fact that in fixed-point orient2d is automatically exact:
 * Computational Geometry in C, Joseph O'Rourke
 * https://www.science.smith.edu/~jorourke/books/compgeom.html
 *
 */
ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା orient2d(int32_t ax, int32_t ay,
		    int32_t bx, int32_t by,
		    int32_t cx, int32_t cy)
{
	ଅଚଳ int64_t det = (static_cast<int64_t>(bx) - ax) * (static_cast<int64_t>(cy) - ay)
			    - (static_cast<int64_t>(by) - ay) * (static_cast<int64_t>(cx) - ax);

	ଯଦି (det > 0) { ଫେରନ୍ତୁ  1; }

	ଯଦି (det < 0) { ଫେରନ୍ତୁ -1; }

	ଫେରନ୍ତୁ 0;
}

/**
 * For a, b, c collinear (orient2d == 0), is c on the open segment ab?
 */
ଧାଡ଼ିଭିତରେ bool collinearBetween(int32_t ax, int32_t ay, int32_t bx, int32_t by, int32_t cx, int32_t cy)
{
	ଯଦି (std::abs(ax - bx) >= std::abs(ay - by)) {
		ଫେରନ୍ତୁ (ax < cx && cx < bx) || (bx < cx && cx < ax);

	} ଅନ୍ୟଥା {
		ଫେରନ୍ତୁ (ay < cy && cy < by) || (by < cy && cy < ay);
	}
}

/**
 * Classification of the relative position of two segments ab and cd.
 *
 *   Cross       proper interior crossing (each segment strictly straddles
 *               the other's supporting line)
 *   AInsideCD   a strictly between c and d (open interior of segment cd)
 *   BInsideCD   b strictly between c and d
 *   CInsideAB   c strictly between a and b
 *   DInsideAB   d strictly between a and b
 *   Collinear   ab and cd lie on the same supporting line (overlap or not)
 *   Disjoint    none of the above
 *
 * Ref: SegSegInt implementation from:
 * Computational Geometry in C, Joseph O'Rourke
 * https://www.science.smith.edu/~jorourke/books/compgeom.html
 */
ଗଣନା class SegSegResult {
	Disjoint,
	Cross,
	AInsideCD, BInsideCD,
	CInsideAB, DInsideAB,
	Collinear,
};

ଧାଡ଼ିଭିତରେ SegSegResult segmentsIntersect(int32_t ax, int32_t ay, int32_t bx, int32_t by,
				      int32_t cx, int32_t cy, int32_t dx, int32_t dy)
{
	// These two early returns are logically not necessary (would catch the
	// case in the end) but improve performance, as most segment pairs are
	// expected to be completely disjoint. If both c and d are strictly on
	// the same side of a-b (first check, second vice versa) we can already
	// conclude the segments are disjoint. This does not detect _all_
	// disjoint cases so the final return stays.

	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା o1 = orient2d(ax, ay, bx, by, cx, cy);
	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା o2 = orient2d(ax, ay, bx, by, dx, dy);

	ଯଦି (o1 != 0 && o1 == o2) { ଫେରନ୍ତୁ SegSegResult::Disjoint; }

	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା o3 = orient2d(cx, cy, dx, dy, ax, ay);
	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା o4 = orient2d(cx, cy, dx, dy, bx, by);

	ଯଦି (o3 != 0 && o3 == o4) { ଫେରନ୍ତୁ SegSegResult::Disjoint; }

	ଯଦି (o1 && o2 && o3 && o4 && o1 != o2 && o3 != o4) { ଫେରନ୍ତୁ SegSegResult::Cross; }

	ଯଦି (!o1 && !o2 && !o3 && !o4) { ଫେରନ୍ତୁ SegSegResult::Collinear; }

	// Endpoint strictly on the open interior of the other segment iff:
	//  - orient2d is zero (point on the supporting line)
	//  - the point lies strictly between the other two via collinearBetween
	ଯଦି (o3 == 0 && collinearBetween(cx, cy, dx, dy, ax, ay)) { ଫେରନ୍ତୁ SegSegResult::AInsideCD; }

	ଯଦି (o4 == 0 && collinearBetween(cx, cy, dx, dy, bx, by)) { ଫେରନ୍ତୁ SegSegResult::BInsideCD; }

	ଯଦି (o1 == 0 && collinearBetween(ax, ay, bx, by, cx, cy)) { ଫେରନ୍ତୁ SegSegResult::CInsideAB; }

	ଯଦି (o2 == 0 && collinearBetween(ax, ay, bx, by, dx, dy)) { ଫେରନ୍ତୁ SegSegResult::DInsideAB; }

	ଫେରନ୍ତୁ SegSegResult::Disjoint;
}

/**
 * Return true if segments ab and cd share any point, including endpoints and overlap.
 * Zero-length segments are treated as points. Coordinates must be finite and in the same plane.
 * For latitude/longitude coordinates, longitudes must use one continuous interval.
 */
bool segmentsIntersectInclusive(ଅଚଳ matrix::Vector2d &a, ଅଚଳ matrix::Vector2d &b,
				ଅଚଳ matrix::Vector2d &c, ଅଚଳ matrix::Vector2d &d);

/**
 * Squared distance from a point to the closest point on segment ab, including its endpoints.
 * Use finite Cartesian coordinates in the same units. A zero-length segment is treated as a point.
 */
ଦ୍ୱିଗୁଣ pointToSegmentDistanceSquared(ଅଚଳ matrix::Vector2d &point,
				     ଅଚଳ matrix::Vector2d &a, ଅଚଳ matrix::Vector2d &b);

/**
 * Check if a polygon's vertices are ordered counter-clockwise using the shoelace formula.
 * Works in local Cartesian coordinates. Returns false for CW or zero-area polygons.
 *
 * @param vertices      polygon vertices in local frame
 * @param num_vertices  number of vertices
 * @return true if the polygon is counter-clockwise
 */
bool isPolygonCCW(ଅଚଳ matrix::Vector2f *vertices, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_vertices);

/**
 * Upper bound on the number of vertices addPolygon() emits for a simple polygon:
 *  - Sharp convex corners (interior angle < 60 deg) are split in two vertices
 *  - Interior angles of a simple polygon sum to (n - 2) * 180 deg
 *  - With k corners below 60 deg and the rest below 360 deg, we have
 *     (n - 2) * 180 < k * 60 + (n - k) * 360
 *     =>  k < (180 n + 360) / 300
 * so k <= ceil(.) - 1, computed as (a - 1) / b in integer arithmetic.

 * Degenerate input (n < 3) is rejected by addPolygon and emits nothing, so
 * the bound is 0.
 */
constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxVerticesAfterSplitting(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_vertices)
{
	ଫେରନ୍ତୁ ଅଙ୍କ_vertices < 3 ? 0 : ଅଙ୍କ_vertices + (180 * ଅଙ୍କ_vertices + 360 - 1) / 300;
}


/**
 * Fence geometry cache for the geofence avoidance planner. Owns a flat
 * int32-cm node buffer that holds polygon vertices, circle k-gon vertices,
 * and one destination slot.
 *
 * Polygons are stored in canonical orientation: the polygon INSIDE is always to
 * the left of each edge (CCW for exclusion zones, CW for inclusion zones).
 *
 * Typical lifecycle:
 *   1. addPolygon / addApproxCircle  (one per geofence zone)
 *   2. setDestination(p)             (called whenever destination changes)
 *   3. edgeCost(a, b) / edgeVisible  (queried during planning)
 */
class PlannerPolygons
{
ସର୍ବସାଧାରଣ:

	// Default node capacity. 100 nodes was found to limit the full update
	// on geofence / margin change to about 20 ms.
	ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା kDefaultMaxNodes = 100;
	ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା kMaxPolygons = 16;
	ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା kCircleApproxVertices = 8;

	// Number of nodes needed to hold the largest geofence storable in dataman.
	// Worst case:
	//  - kMaxPolygons-1 circles (most nodes per dataman item)
	//  - one remaining polygon using all remaining fence points, with max amount of split vertices
	//  - 1 destination slot
	// If the allocated node capacity >= kMaxNodesForAnyStorableFence, we always have enough space.
	// Otherwise we may fail with AddResult::BudgetExceeded.
	ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା kMaxNodesForAnyStorableFence =
		(kMaxPolygons - 1) * kCircleApproxVertices
		+ maxVerticesAfterSplitting(DM_ଚାବି_FENCE_POINTS_MAX - (kMaxPolygons - 1))
		+ 1;

	// Return code specifying why a polygon was not added.
	ଗଣନା class AddResult {
		Success,
		BudgetExceeded,  // node or polygon buffer full (allocated node capacity / kMaxPolygons)
		OutOfRange,      // vertex or circle extent outside usable fixed-point range
		Degenerate,      // <3 vertices, zero-length/antiparallel edge, self-intersecting, empty circle, negative margin
	};

	explicit PlannerPolygons(ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_nodes = kDefaultMaxNodes) { allocate(max_nodes); }
	~PlannerPolygons() { freeBuffers(); }

	PlannerPolygons(ଅଚଳ PlannerPolygons &) = delete;
	PlannerPolygons &operator=(ଅଚଳ PlannerPolygons &) = delete;

	// (Re-)allocate the node buffers for max_nodes nodes (including the destination slot) and reset.
	// Returns false if the allocation failed, in which case the capacity is 0 and every add fails.
	bool allocate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_nodes);

	// Slot 0 is always reserved for the destination; polygon vertices start at index 1.
	ଶୂନ୍ୟ reset()
	{
		ଅଙ୍କ_polygons = 0;

		ଯଦି (_max_nodes < 1) {
			ଅଙ୍କ_nodes_2 = 0;
			ଫେରନ୍ତୁ;
		}

		ଅଙ୍କ_nodes_2 = 1;
		_x_cm[0] = 0;
		_y_cm[0] = 0;
		ଗଣ୍ଠି_not_on_optimal_path[0] = false;
	}

	// Append a polygon:
	//  - canonicalize orientation,
	//  - offset each vertex outward (exclusion) or inward (inclusion) by `margin` meters,
	//  - quantize to cm, append as nodes.
	AddResult addPolygon(ଅଚଳ matrix::Vector2f *vertices_in, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_vertices,
			     bool is_inclusion_zone, ଦଶମିକ margin = 0.f);

	// Append an approximate circle (k-gon over/underapproximation).
	//  - shrink/expand according to margin and is_inclusion_zone
	//  - quantize to cm, append as nodes.
	AddResult addApproxCircle(ଅଚଳ matrix::Vector2f &center, ଅଚଳ ଦଶମିକ radius, ଦଶମିକ margin, ଅଚଳ bool is_inclusion_zone);

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_Nodes() ଅଚଳ { ଫେରନ୍ତୁ ଅଙ୍କ_nodes_2; }

	// Number of polygon nodes (excludes the destination at index 0).
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_Vertices() ଅଚଳ { ଫେରନ୍ତୁ ଅଙ୍କ_nodes_2 - 1; }

	matrix::Vector2f ଗଣ୍ଠି_2(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx) ଅଚଳ
	{
		ଫେରନ୍ତୁ matrix::Vector2f{_x_cm[idx] / CM_PER_M, _y_cm[idx] / CM_PER_M};
	}

	// Update the destination position. Always safe to call after reset().
	// Returns false if the destination violates the geofence, but still updates destionation.
	bool setDestination(ଅଚଳ matrix::Vector2f &p);

	// Get the current destination position.
	matrix::Vector2f getDestination() ଅଚଳ;

	// Destination is always at index 0; polygon vertices start at index 1.
	ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା destIndex() { ଫେରନ୍ତୁ 0; }

	// True if the segment (a, b) crosses no geofence interior.
	bool edgeVisible(ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b) ଅଚଳ;
	bool edgeVisible(ଅଚଳ matrix::Vector2f &a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b) ଅଚଳ;          // One float endpoint (e.g. vehicle position).
	bool edgeVisible(ଅଚଳ matrix::Vector2f &a, ଅଚଳ matrix::Vector2f &b) ଅଚଳ; // Both float endpoints.

	// Traversal cost for edge (a, b): INFINITY if the edge crosses any fence,
	// Euclidean distance (metres) otherwise.
	ଦଶମିକ edgeCost(ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b) ଅଚଳ;

ବ୍ୟକ୍ତିଗତ:
	ଶୂନ୍ୟ freeBuffers();

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା _max_nodes{0};
	int32_t *_x_cm{nullptr};
	int32_t *_y_cm{nullptr};

	ସଂରଚନା PolygonInfo {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_index;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_vertices;
		bool is_inclusion;
		int32_t min_x, max_x, min_y, max_y; // Bouding box
	};

	PolygonInfo _polygons[kMaxPolygons];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_polygons{0};

	/**
	 * Reduced visibility graph: we can exclude some vertices a priori
	 * because they cannot be part of the shortest path. For convex
	 * inclusion corners or reflex exclusion corners at node B, the path
	 *   (A -> B -> C)
	 * is a detour compared to
	 *   (A -> just before B -> just after B -> C)
	 * and so it cannot be globally optimal.
	 */
	bool *ଗଣ୍ଠି_not_on_optimal_path{nullptr};
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_nodes_2{0};

	/**
	 * Line-segment vs polygon intersection check.
	 *
	 * Only detects intersection with the _interior_ of the polygon in
	 * canonical orientation, i.e. the forbidden region. Intersection with
	 * only the boundary is allowed (necessary for shortest path to be well
	 * defined)
	 */
	bool intersectsInsideOf(ଅଚଳ PolygonInfo &poly, int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ଅଚଳ;

	/**
	 * Return true iff the line segment s-e intersects the inside of _any_ stored polygon.
	 */
	bool intersectsAnyInside(int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ଅଚଳ;

	/**
	 * Consider extending the triangle poly[v-1] -> poly[v] -> poly[v+1]
	 * outward from poly[v] (the inside always being left of the points).
	 * This cone is the local interior of the forbidden region at the given
	 * vertex.
	 *
	 * Returns true if the point p is in the interior of that cone, which is
	 * the case iff the line segment poly[v] -> p pierces the interior at
	 * vertex curr.
	 */
	bool pointInsideInteriorCone(ଅଚଳ PolygonInfo &poly, int32_t px, int32_t py, ପୂର୍ଣ୍ଣସଂଖ୍ୟା v) ଅଚଳ;

	/**
	 * Return true if the edge ab is bitangent at a: neither the edge nor
	 * its mirror about `a` enters the forbidden interior there.
	 *
	 * Only bitangent edges can be part of a shortest path [*]. Checking
	 * this first in edgeVisible also throws out invalid (interior-entering)
	 * edges before the general intersection checks.
	 *
	 * * Stephen LaValle, 2006: Planning Algorithms, 6.2.4 - Shortest-Path Roadmaps
	 */
	bool edgeBitangent(ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, int32_t bx, int32_t by) ଅଚଳ;
	bool edgeBitangent(ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b) ଅଚଳ;                        // Other endpoint is node index b.
	bool edgeBitangent(ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, ଅଚଳ matrix::Vector2f &b) ଅଚଳ;    // Other endpoint is an arbitrary position.

	/**
	 * Update {min,max}_{x,y} with the min/max coordinates of the polygon with vertices
	 * [start_index, start_index + num_vertices).
	 */
	ଶୂନ୍ୟ computeBoundingBox(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_index, ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_vertices,
				int32_t &min_x, int32_t &max_x, int32_t &min_y, int32_t &max_y);

	// Write float-metre coordinates into an existing node slot (used internally by
	// addPolygon, addApproxCircle, and setDestination).
	ଶୂନ୍ୟ set_ଗଣ୍ଠି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ଅଚଳ matrix::Vector2f &p);

};

// Convenience for unit tests and one-shot callers: build a transient
// PlannerPolygons with the given polygon and query the segment.
ଧାଡ଼ିଭିତରେ bool ଧାଡ଼ି_Segment_Intersects_Polygon(ଅଚଳ matrix::Vector2f &start, ଅଚଳ matrix::Vector2f &end,
		ଅଚଳ matrix::Vector2f *vertices, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_vertices, bool is_inclusion_zone)
{
	PlannerPolygons polys{maxVerticesAfterSplitting(ଅଙ୍କ_vertices) + 1};

	ଯଦି (polys.addPolygon(vertices, ଅଙ୍କ_vertices, is_inclusion_zone) != PlannerPolygons::AddResult::Success) {
		ଫେରନ୍ତୁ false;
	}

	ଫେରନ୍ତୁ !polys.edgeVisible(start, end);
}

} // namespace geofence_utils
