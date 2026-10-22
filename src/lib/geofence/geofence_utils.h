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

#অন্তর্ভাৱ <cmath>
#অন্তর্ভাৱ <cstdint>
#অন্তর্ভাৱ <cstdlib>

#অন্তর্ভাৱ <matrix/math.hpp>

#অন্তর্ভাৱ <dataman/dataman.h>

namespace geofence_utils
{

স্থির constexpr ভিন্ন CM_PER_M  = 100.f;

// True iff `m_value` (metres) fits into the planner's fixed-point
// representation. intersectsInsideOf doubles cm coordinates internally, so the
// usable range is 2^30 cm = 10,737 km from the planner's reference.
অন্তঃস্থ bool inFixedPointRange(ভিন্ন m_মান) { ঘূৰাই fabsf(m_মান) < (1u << 30) / CM_PER_M; }

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
অন্তঃস্থ পূর্ণাঙ্ক orient2d(int32_t ax, int32_t ay,
		    int32_t bx, int32_t by,
		    int32_t cx, int32_t cy)
{
	ধ্রুৱ int64_t det = (static_cast<int64_t>(bx) - ax) * (static_cast<int64_t>(cy) - ay)
			    - (static_cast<int64_t>(by) - ay) * (static_cast<int64_t>(cx) - ax);

	যদি (det > 0) { ঘূৰাই  1; }

	যদি (det < 0) { ঘূৰাই -1; }

	ঘূৰাই 0;
}

/**
 * For a, b, c collinear (orient2d == 0), is c on the open segment ab?
 */
অন্তঃস্থ bool collinearBetween(int32_t ax, int32_t ay, int32_t bx, int32_t by, int32_t cx, int32_t cy)
{
	যদি (std::abs(ax - bx) >= std::abs(ay - by)) {
		ঘূৰাই (ax < cx && cx < bx) || (bx < cx && cx < ax);

	} অন্যথা {
		ঘূৰাই (ay < cy && cy < by) || (by < cy && cy < ay);
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
গণনা class SegSegResult {
	Disjoint,
	Cross,
	AInsideCD, BInsideCD,
	CInsideAB, DInsideAB,
	Collinear,
};

অন্তঃস্থ SegSegResult segmentsIntersect(int32_t ax, int32_t ay, int32_t bx, int32_t by,
				      int32_t cx, int32_t cy, int32_t dx, int32_t dy)
{
	// These two early returns are logically not necessary (would catch the
	// case in the end) but improve performance, as most segment pairs are
	// expected to be completely disjoint. If both c and d are strictly on
	// the same side of a-b (first check, second vice versa) we can already
	// conclude the segments are disjoint. This does not detect _all_
	// disjoint cases so the final return stays.

	ধ্রুৱ পূর্ণাঙ্ক o1 = orient2d(ax, ay, bx, by, cx, cy);
	ধ্রুৱ পূর্ণাঙ্ক o2 = orient2d(ax, ay, bx, by, dx, dy);

	যদি (o1 != 0 && o1 == o2) { ঘূৰাই SegSegResult::Disjoint; }

	ধ্রুৱ পূর্ণাঙ্ক o3 = orient2d(cx, cy, dx, dy, ax, ay);
	ধ্রুৱ পূর্ণাঙ্ক o4 = orient2d(cx, cy, dx, dy, bx, by);

	যদি (o3 != 0 && o3 == o4) { ঘূৰাই SegSegResult::Disjoint; }

	যদি (o1 && o2 && o3 && o4 && o1 != o2 && o3 != o4) { ঘূৰাই SegSegResult::Cross; }

	যদি (!o1 && !o2 && !o3 && !o4) { ঘূৰাই SegSegResult::Collinear; }

	// Endpoint strictly on the open interior of the other segment iff:
	//  - orient2d is zero (point on the supporting line)
	//  - the point lies strictly between the other two via collinearBetween
	যদি (o3 == 0 && collinearBetween(cx, cy, dx, dy, ax, ay)) { ঘূৰাই SegSegResult::AInsideCD; }

	যদি (o4 == 0 && collinearBetween(cx, cy, dx, dy, bx, by)) { ঘূৰাই SegSegResult::BInsideCD; }

	যদি (o1 == 0 && collinearBetween(ax, ay, bx, by, cx, cy)) { ঘূৰাই SegSegResult::CInsideAB; }

	যদি (o2 == 0 && collinearBetween(ax, ay, bx, by, dx, dy)) { ঘূৰাই SegSegResult::DInsideAB; }

	ঘূৰাই SegSegResult::Disjoint;
}

/**
 * Return true if segments ab and cd share any point, including endpoints and overlap.
 * Zero-length segments are treated as points. Coordinates must be finite and in the same plane.
 * For latitude/longitude coordinates, longitudes must use one continuous interval.
 */
bool segmentsIntersectInclusive(ধ্রুৱ matrix::Vector2d &a, ধ্রুৱ matrix::Vector2d &b,
				ধ্রুৱ matrix::Vector2d &c, ধ্রুৱ matrix::Vector2d &d);

/**
 * Squared distance from a point to the closest point on segment ab, including its endpoints.
 * Use finite Cartesian coordinates in the same units. A zero-length segment is treated as a point.
 */
দ্ৱিগুণ pointToSegmentDistanceSquared(ধ্রুৱ matrix::Vector2d &point,
				     ধ্রুৱ matrix::Vector2d &a, ধ্রুৱ matrix::Vector2d &b);

/**
 * Check if a polygon's vertices are ordered counter-clockwise using the shoelace formula.
 * Works in local Cartesian coordinates. Returns false for CW or zero-area polygons.
 *
 * @param vertices      polygon vertices in local frame
 * @param num_vertices  number of vertices
 * @return true if the polygon is counter-clockwise
 */
bool isPolygonCCW(ধ্রুৱ matrix::Vector2f *vertices, পূর্ণাঙ্ক অঙ্ক_vertices);

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
constexpr পূর্ণাঙ্ক maxVerticesAfterSplitting(পূর্ণাঙ্ক অঙ্ক_vertices)
{
	ঘূৰাই অঙ্ক_vertices < 3 ? 0 : অঙ্ক_vertices + (180 * অঙ্ক_vertices + 360 - 1) / 300;
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
সার্ৱজনিক:

	// Default node capacity. 100 nodes was found to limit the full update
	// on geofence / margin change to about 20 ms.
	স্থির constexpr পূর্ণাঙ্ক kDefaultMaxNodes = 100;
	স্থির constexpr পূর্ণাঙ্ক kMaxPolygons = 16;
	স্থির constexpr পূর্ণাঙ্ক kCircleApproxVertices = 8;

	// Number of nodes needed to hold the largest geofence storable in dataman.
	// Worst case:
	//  - kMaxPolygons-1 circles (most nodes per dataman item)
	//  - one remaining polygon using all remaining fence points, with max amount of split vertices
	//  - 1 destination slot
	// If the allocated node capacity >= kMaxNodesForAnyStorableFence, we always have enough space.
	// Otherwise we may fail with AddResult::BudgetExceeded.
	স্থির constexpr পূর্ণাঙ্ক kMaxNodesForAnyStorableFence =
		(kMaxPolygons - 1) * kCircleApproxVertices
		+ maxVerticesAfterSplitting(DM_চাবি_FENCE_POINTS_MAX - (kMaxPolygons - 1))
		+ 1;

	// Return code specifying why a polygon was not added.
	গণনা class AddResult {
		Success,
		BudgetExceeded,  // node or polygon buffer full (allocated node capacity / kMaxPolygons)
		OutOfRange,      // vertex or circle extent outside usable fixed-point range
		Degenerate,      // <3 vertices, zero-length/antiparallel edge, self-intersecting, empty circle, negative margin
	};

	explicit PlannerPolygons(পূর্ণাঙ্ক max_nodes = kDefaultMaxNodes) { allocate(max_nodes); }
	~PlannerPolygons() { freeBuffers(); }

	PlannerPolygons(ধ্রুৱ PlannerPolygons &) = delete;
	PlannerPolygons &operator=(ধ্রুৱ PlannerPolygons &) = delete;

	// (Re-)allocate the node buffers for max_nodes nodes (including the destination slot) and reset.
	// Returns false if the allocation failed, in which case the capacity is 0 and every add fails.
	bool allocate(পূর্ণাঙ্ক max_nodes);

	// Slot 0 is always reserved for the destination; polygon vertices start at index 1.
	শূন্য reset()
	{
		অঙ্ক_polygons = 0;

		যদি (_max_nodes < 1) {
			অঙ্ক_nodes_2 = 0;
			ঘূৰাই;
		}

		অঙ্ক_nodes_2 = 1;
		_x_cm[0] = 0;
		_y_cm[0] = 0;
		গ্ৰন্থি_not_on_optimal_path[0] = false;
	}

	// Append a polygon:
	//  - canonicalize orientation,
	//  - offset each vertex outward (exclusion) or inward (inclusion) by `margin` meters,
	//  - quantize to cm, append as nodes.
	AddResult addPolygon(ধ্রুৱ matrix::Vector2f *vertices_in, পূর্ণাঙ্ক অঙ্ক_vertices,
			     bool is_inclusion_zone, ভিন্ন margin = 0.f);

	// Append an approximate circle (k-gon over/underapproximation).
	//  - shrink/expand according to margin and is_inclusion_zone
	//  - quantize to cm, append as nodes.
	AddResult addApproxCircle(ধ্রুৱ matrix::Vector2f &center, ধ্রুৱ ভিন্ন radius, ভিন্ন margin, ধ্রুৱ bool is_inclusion_zone);

	পূর্ণাঙ্ক অঙ্ক_Nodes() ধ্রুৱ { ঘূৰাই অঙ্ক_nodes_2; }

	// Number of polygon nodes (excludes the destination at index 0).
	পূর্ণাঙ্ক অঙ্ক_Vertices() ধ্রুৱ { ঘূৰাই অঙ্ক_nodes_2 - 1; }

	matrix::Vector2f গ্ৰন্থি_2(পূর্ণাঙ্ক idx) ধ্রুৱ
	{
		ঘূৰাই matrix::Vector2f{_x_cm[idx] / CM_PER_M, _y_cm[idx] / CM_PER_M};
	}

	// Update the destination position. Always safe to call after reset().
	// Returns false if the destination violates the geofence, but still updates destionation.
	bool setDestination(ধ্রুৱ matrix::Vector2f &p);

	// Get the current destination position.
	matrix::Vector2f getDestination() ধ্রুৱ;

	// Destination is always at index 0; polygon vertices start at index 1.
	স্থির constexpr পূর্ণাঙ্ক destIndex() { ঘূৰাই 0; }

	// True if the segment (a, b) crosses no geofence interior.
	bool edgeVisible(পূর্ণাঙ্ক a, পূর্ণাঙ্ক b) ধ্রুৱ;
	bool edgeVisible(ধ্রুৱ matrix::Vector2f &a, পূর্ণাঙ্ক b) ধ্রুৱ;          // One float endpoint (e.g. vehicle position).
	bool edgeVisible(ধ্রুৱ matrix::Vector2f &a, ধ্রুৱ matrix::Vector2f &b) ধ্রুৱ; // Both float endpoints.

	// Traversal cost for edge (a, b): INFINITY if the edge crosses any fence,
	// Euclidean distance (metres) otherwise.
	ভিন্ন edgeCost(পূর্ণাঙ্ক a, পূর্ণাঙ্ক b) ধ্রুৱ;

গুপ্ত:
	শূন্য freeBuffers();

	পূর্ণাঙ্ক _max_nodes{0};
	int32_t *_x_cm{nullptr};
	int32_t *_y_cm{nullptr};

	সংরচনা PolygonInfo {
		পূর্ণাঙ্ক start_index;
		পূর্ণাঙ্ক অঙ্ক_vertices;
		bool is_inclusion;
		int32_t min_x, max_x, min_y, max_y; // Bouding box
	};

	PolygonInfo _polygons[kMaxPolygons];
	পূর্ণাঙ্ক অঙ্ক_polygons{0};

	/**
	 * Reduced visibility graph: we can exclude some vertices a priori
	 * because they cannot be part of the shortest path. For convex
	 * inclusion corners or reflex exclusion corners at node B, the path
	 *   (A -> B -> C)
	 * is a detour compared to
	 *   (A -> just before B -> just after B -> C)
	 * and so it cannot be globally optimal.
	 */
	bool *গ্ৰন্থি_not_on_optimal_path{nullptr};
	পূর্ণাঙ্ক অঙ্ক_nodes_2{0};

	/**
	 * Line-segment vs polygon intersection check.
	 *
	 * Only detects intersection with the _interior_ of the polygon in
	 * canonical orientation, i.e. the forbidden region. Intersection with
	 * only the boundary is allowed (necessary for shortest path to be well
	 * defined)
	 */
	bool intersectsInsideOf(ধ্রুৱ PolygonInfo &poly, int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ধ্রুৱ;

	/**
	 * Return true iff the line segment s-e intersects the inside of _any_ stored polygon.
	 */
	bool intersectsAnyInside(int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ধ্রুৱ;

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
	bool pointInsideInteriorCone(ধ্রুৱ PolygonInfo &poly, int32_t px, int32_t py, পূর্ণাঙ্ক v) ধ্রুৱ;

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
	bool edgeBitangent(পূর্ণাঙ্ক a, int32_t bx, int32_t by) ধ্রুৱ;
	bool edgeBitangent(পূর্ণাঙ্ক a, পূর্ণাঙ্ক b) ধ্রুৱ;                        // Other endpoint is node index b.
	bool edgeBitangent(পূর্ণাঙ্ক a, ধ্রুৱ matrix::Vector2f &b) ধ্রুৱ;    // Other endpoint is an arbitrary position.

	/**
	 * Update {min,max}_{x,y} with the min/max coordinates of the polygon with vertices
	 * [start_index, start_index + num_vertices).
	 */
	শূন্য computeBoundingBox(ধ্রুৱ পূর্ণাঙ্ক start_index, ধ্রুৱ পূর্ণাঙ্ক অঙ্ক_vertices,
				int32_t &min_x, int32_t &max_x, int32_t &min_y, int32_t &max_y);

	// Write float-metre coordinates into an existing node slot (used internally by
	// addPolygon, addApproxCircle, and setDestination).
	শূন্য set_গ্ৰন্থি(পূর্ণাঙ্ক idx, ধ্রুৱ matrix::Vector2f &p);

};

// Convenience for unit tests and one-shot callers: build a transient
// PlannerPolygons with the given polygon and query the segment.
অন্তঃস্থ bool সাৰি_Segment_Intersects_Polygon(ধ্রুৱ matrix::Vector2f &start, ধ্রুৱ matrix::Vector2f &end,
		ধ্রুৱ matrix::Vector2f *vertices, পূর্ণাঙ্ক অঙ্ক_vertices, bool is_inclusion_zone)
{
	PlannerPolygons polys{maxVerticesAfterSplitting(অঙ্ক_vertices) + 1};

	যদি (polys.addPolygon(vertices, অঙ্ক_vertices, is_inclusion_zone) != PlannerPolygons::AddResult::Success) {
		ঘূৰাই false;
	}

	ঘূৰাই !polys.edgeVisible(start, end);
}

} // namespace geofence_utils
