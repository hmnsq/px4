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

#அந்தர்பாவ "geofence_utils.h"
#அந்தர்பாவ <lib/geo/geo.h>
#அந்தர்பாவ <lib/mathlib/mathlib.h>

நாமாகாஶ geofence_utils
{

நாமாகாஶ
{

int32_t metersToCm(தசம m) { திரும்பு roundf(m * CM_PER_M); }

} // namespace


தருக்க PlannerPolygons::allocate(முழு max_nodes)
{
	freeBuffers();

	எனில் (max_nodes > 0) {
		_x_cm = புதிய int32_t[max_nodes];
		_y_cm = புதிய int32_t[max_nodes];
		முனை_not_on_optimal_path = புதிய தருக்க[max_nodes];

		எனில் (_x_cm != nullptr && _y_cm != nullptr && முனை_not_on_optimal_path != nullptr) {
			_max_nodes = max_nodes;

		} இல்லையெனில் {
			freeBuffers();
		}
	}

	reset();

	திரும்பு _max_nodes == max_nodes;
}

வெறுமை PlannerPolygons::freeBuffers()
{
	delete[] _x_cm;
	delete[] _y_cm;
	delete[] முனை_not_on_optimal_path;
	_x_cm = nullptr;
	_y_cm = nullptr;
	முனை_not_on_optimal_path = nullptr;
	_max_nodes = 0;
}

வெறுமை PlannerPolygons::set_முனை(முழு idx, மாறிலி matrix::Vector2f &p)
{
	எனில் (_max_nodes < 1) {
		திரும்பு;
	}

	idx = math::constrain(idx, 0, _max_nodes - 1);

	_x_cm[idx] = metersToCm(p(0));
	_y_cm[idx] = metersToCm(p(1));
}

PlannerPolygons::AddResult PlannerPolygons::addPolygon(மாறிலி matrix::Vector2f *vertices_in, முழு இலக்கம்_vertices,
		தருக்க is_inclusion_zone, தசம margin)
{
	எனில் (இலக்கம்_polygons >= kMaxPolygons || இலக்கம்_nodes_2 + இலக்கம்_vertices > _max_nodes) {
		திரும்பு AddResult::BudgetExceeded;
	}

	எனில் (margin < -FLT_EPSILON || இலக்கம்_vertices < 3) { திரும்பு AddResult::Degenerate; }

	க்கு (முழு i = 0; i < இலக்கம்_vertices; ++i) {
		// Reject self-intersecting (non-simple) polygons.
		மாறிலி matrix::Vector2f &a = vertices_in[i];
		மாறிலி matrix::Vector2f &b = vertices_in[(i + 1) % இலக்கம்_vertices];

		க்கு (முழு j = i + 2; j < இலக்கம்_vertices; ++j) {
			எனில் (i == 0 && j == இலக்கம்_vertices - 1) {
				தொடர்; // (last edge, first edge) are adjacent
			}

			மாறிலி matrix::Vector2f &c = vertices_in[j];
			மாறிலி matrix::Vector2f &d = vertices_in[(j + 1) % இலக்கம்_vertices];

			எனில் (segmentsIntersect(
				    metersToCm(a(0)), metersToCm(a(1)), metersToCm(b(0)), metersToCm(b(1)),
				    metersToCm(c(0)), metersToCm(c(1)), metersToCm(d(0)), metersToCm(d(1))) != SegSegResult::Disjoint) {
				திரும்பு AddResult::Degenerate;
			}
		}

		// Reject vertices outside the usable fixed-point range
		எனில் (!inFixedPointRange(vertices_in[i](0)) || !inFixedPointRange(vertices_in[i](1))) {
			திரும்பு AddResult::OutOfRange;
		}
	}

	// Canonical orientation: walking vertices in stored order, INSIDE is on
	// the left of each edge. CCW for exclusion (bounded interior is forbidden),
	// CW for inclusion (unbounded exterior is forbidden). Reverse iff input
	// doesn't already match.
	மாறிலி தருக்க reverse = (isPolygonCCW(vertices_in, இலக்கம்_vertices) == is_inclusion_zone);

	// Index into vertices_in in stored order (accounts for possible reversal).
	auto vertex = [&](முழு i) -> மாறிலி matrix::Vector2f & {
		திரும்பு vertices_in[reverse ? (இலக்கம்_vertices - 1 - i) : i];
	};

	PolygonInfo &poly = _polygons[இலக்கம்_polygons];
	poly.start_index  = இலக்கம்_nodes_2;
	poly.is_inclusion = is_inclusion_zone;

	முழு out_idx = 0;

	க்கு (முழு i = 0; i < இலக்கம்_vertices; i++) {
		மாறிலி முழு prev = (i + இலக்கம்_vertices - 1) % இலக்கம்_vertices;
		மாறிலி முழு next = (i + 1) % இலக்கம்_vertices;

		மாறிலி matrix::Vector2f edge_in  = vertex(i) - vertex(prev);
		மாறிலி matrix::Vector2f edge_out = vertex(next) - vertex(i);

		மாறிலி தசம edge_in_norm = edge_in.norm();
		மாறிலி தசம edge_out_norm = edge_out.norm();

		எனில் (edge_in_norm < FLT_EPSILON || edge_out_norm < FLT_EPSILON) {
			திரும்பு AddResult::Degenerate;
		}

		மாறிலி matrix::Vector2f edge_in_normalized = edge_in / edge_in_norm;
		மாறிலி matrix::Vector2f edge_out_normalized = edge_out / edge_out_norm;

		// Inward normals of the two adjacent edges.
		// Canonical stored orientation: left side = forbidden region, right side = allowed region
		மாறிலி matrix::Vector2f n_in  = matrix::Vector2f{-edge_in_normalized(1), edge_in_normalized(0)};
		மாறிலி matrix::Vector2f n_out = matrix::Vector2f{-edge_out_normalized(1), edge_out_normalized(0)};

		matrix::Vector2f bisector = n_in + n_out;
		மாறிலி தசம bisector_len  = bisector.length();

		எனில் (bisector_len < FLT_EPSILON) {
			திரும்பு AddResult::Degenerate; // antiparallel edges - polygon doubles back
		}

		மாறிலி matrix::Vector2f normalized_bisector = bisector / bisector_len;

		// The following bit ensures that:
		//  - The original edges are moved away from the forbidden interior by `margin`
		//  - Sharp convex corners (<60deg) are "cut off" with a new edge that is exactly
		//    `margin` away from the original vertex. For this the vertex is replaced by
		//    two new ones.

		// Only split convex corners - others are ignored, see _node_not_on_optimal_path
		மாறிலி தருக்க corner_convex = edge_in_normalized(0) * edge_out_normalized(1)
					   - edge_in_normalized(1) * edge_out_normalized(0) > 0.f;

		// Scaling needed wrt the normalised bisector to achieve parallel edges
		மாறிலி தசம offset_scale = 2.f / bisector_len;

		// bisector_len = 2*sin(interior angle/2); split when interior angle < 60 deg <=> bisector_len < 1.
		மாறிலி தருக்க angle_sharp = bisector_len < 1;

		// If margin zero, do not split - would output the same vertex twice.
		மாறிலி தருக்க margin_nonzero = margin > FLT_EPSILON;

		// If we do not have enough space, do not split the vertex.
		// This never happens if the planner-internal node buffer is 2x the original buffer.
		மாறிலி தருக்க space_for_split_vertices = இலக்கம்_nodes_2 + out_idx + 1 < _max_nodes;

		எனில் (corner_convex && angle_sharp && margin_nonzero && space_for_split_vertices) {

			// Sharp corner: cut it off along the line
			//  - perpendicular to normalized_bisector
			//  - going through base := vertex(i) - normalized_bisector * margin

			மாறிலி matrix::Vector2f base = vertex(i) - normalized_bisector * margin;
			மாறிலி matrix::Vector2f perp = {-normalized_bisector(1), normalized_bisector(0)};

			// Construct s such that:
			//  base + perp*s is on the offset incoming edge
			//  base - perp*s is on the offset outgoing edge
			மாறிலி தசம e_out_dot_b = edge_out_normalized.dot(normalized_bisector);
			மாறிலி தசம s = margin * (1.f - n_out.dot(normalized_bisector)) / e_out_dot_b;

			set_முனை(poly.start_index + out_idx++, base + perp * s);
			set_முனை(poly.start_index + out_idx++, base - perp * s);

		} இல்லையெனில் {
			// Regular corner or no space for split: single bisector vertex displaced by margin / sin(interior angle/2).
			set_முனை(poly.start_index + out_idx++, vertex(i) - normalized_bisector * (margin * offset_scale));
		}
	}

	மாறிலி முழு இலக்கம்_out = out_idx;
	poly.இலக்கம்_vertices = இலக்கம்_out;

	computeBoundingBox(poly.start_index, இலக்கம்_out, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// In canonical orientation, the node is not possibly on the shortest
	// path if the vertex curves right or straight (forbidden region >= 180
	// deg)
	க்கு (முழு i = 0; i < இலக்கம்_out; i++) {
		மாறிலி முழு prev = poly.start_index + (i + இலக்கம்_out - 1) % இலக்கம்_out;
		மாறிலி முழு curr = poly.start_index + i;
		மாறிலி முழு next = poly.start_index + (i + 1) % இலக்கம்_out;
		மாறிலி தருக்க curves_வலது_or_straight = orient2d(_x_cm[prev], _y_cm[prev],
						      _x_cm[curr], _y_cm[curr],
						      _x_cm[next], _y_cm[next]) <= 0;
		முனை_not_on_optimal_path[curr] = curves_வலது_or_straight;
	}

	இலக்கம்_nodes_2 += இலக்கம்_out;
	++இலக்கம்_polygons;
	திரும்பு AddResult::Success;
}

PlannerPolygons::AddResult PlannerPolygons::addApproxCircle(மாறிலி matrix::Vector2f &center, மாறிலி தசம circle_radius,
		தசம margin, மாறிலி தருக்க is_inclusion_zone)
{
	// For planning we approximate circles by regular k-gons. This adds
	// quite some nodes and additional restricted area. However, planning
	// properly around circles would be more complicated. Read these if that
	// becomes a priority regardless:
	//
	//  - https://dl.acm.org/doi/epdf/10.1145/323233.323261
	//  - https://www.sciencedirect.com/science/article/pii/S0925772106000496
	//
	// Keep in mind that the added complexity would extend beyond the
	// planner into all controllers, which would then have to also fly
	// loiter segments, not just pure waypoint sequences.

	எனில் (இலக்கம்_polygons >= kMaxPolygons || இலக்கம்_nodes_2 + kCircleApproxVertices > _max_nodes) {
		திரும்பு AddResult::BudgetExceeded;
	}

	எனில் (circle_radius <= 0.1f) { திரும்பு AddResult::Degenerate; }

	// ensures that the k-gon is, w.r.t the real circle:
	//  - over-approximation of circle grown by margin, if exclusion
	//  - under-approximation of circle shrunk by margin, if inclusion
	மாறிலி தசம k_gon_radius = is_inclusion_zone
				   ? circle_radius - margin
				   : circle_radius / cosf(M_PI_F / kCircleApproxVertices) + margin;

	எனில் (k_gon_radius <= FLT_EPSILON) { // Negative radius - circle empty
		திரும்பு AddResult::Degenerate;
	}

	எனில் (!inFixedPointRange(fabsf(center(0)) + k_gon_radius)
	    || !inFixedPointRange(fabsf(center(1)) + k_gon_radius)) {
		திரும்பு AddResult::OutOfRange;
	}

	// Canonical orientation: CCW for exclusion, CW for inclusion
	மாறிலி தசம delta_angle = (is_inclusion_zone ? -1.f : 1.f) * 2.f * M_PI_F / kCircleApproxVertices;

	PolygonInfo &poly = _polygons[இலக்கம்_polygons];
	poly.start_index = இலக்கம்_nodes_2;
	poly.இலக்கம்_vertices = kCircleApproxVertices;
	poly.is_inclusion = is_inclusion_zone;

	க்கு (முழு i = 0; i < kCircleApproxVertices; i++) {
		மாறிலி தசம angle = (i + 0.5f) * delta_angle;
		மாறிலி matrix::Vector2f p = center + matrix::Vector2f{
			k_gon_radius * cosf(angle), k_gon_radius * sinf(angle)
		};
		set_முனை(poly.start_index + i, p);
	}

	computeBoundingBox(poly.start_index, kCircleApproxVertices, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// For regular k-gons, all corners are either convex or reflex based on
	// is_inclusion, so the reduced visibility graph criterion is simplified
	க்கு (முழு i = 0; i < kCircleApproxVertices; i++) {
		முனை_not_on_optimal_path[poly.start_index + i] = is_inclusion_zone;
	}

	இலக்கம்_nodes_2 += kCircleApproxVertices;
	++இலக்கம்_polygons;
	திரும்பு AddResult::Success;
}

வெறுமை PlannerPolygons::computeBoundingBox(மாறிலி முழு start_index, மாறிலி முழு இலக்கம்_vertices,
		int32_t &min_x, int32_t &max_x, int32_t &min_y, int32_t &max_y)
{

	// Store the bounding box, so we can do a very cheap check against the
	// bounding box, and only do the full line-polygon intersection check if
	// the line intersects the bounding box.

	min_x = max_x = _x_cm[start_index];
	min_y = max_y = _y_cm[start_index];

	க்கு (முழு i = 1; i < இலக்கம்_vertices; i++) {
		மாறிலி int32_t x = _x_cm[start_index + i];
		மாறிலி int32_t y = _y_cm[start_index + i];

		எனில் (x < min_x) { min_x = x; }

		எனில் (x > max_x) { max_x = x; }

		எனில் (y < min_y) { min_y = y; }

		எனில் (y > max_y) { max_y = y; }
	}
}

தருக்க PlannerPolygons::pointInsideInteriorCone(மாறிலி PolygonInfo &poly,
		int32_t px, int32_t py, முழு v) மாறிலி
{
	மாறிலி முழு prev = poly.start_index + ((v + poly.இலக்கம்_vertices - 1) % poly.இலக்கம்_vertices);
	மாறிலி முழு curr = poly.start_index + v;
	மாறிலி முழு next = poly.start_index + ((v + 1) % poly.இலக்கம்_vertices);

	மாறிலி முழு p_இடது_of_incoming  = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], px, py);
	மாறிலி முழு p_இடது_of_outgoing  = orient2d(_x_cm[curr], _y_cm[curr], _x_cm[next], _y_cm[next], px, py);
	மாறிலி முழு is_convex           = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], _x_cm[next], _y_cm[next]);

	திரும்பு is_convex > 0  // If convex==0 (exact 180 deg "corner") the two cases are equivalent
	       ? (p_இடது_of_incoming > 0 && p_இடது_of_outgoing > 0)   // Convex: interior = intersection of two half-planes
	       : (p_இடது_of_incoming > 0 || p_இடது_of_outgoing > 0);  // Reflex: interior = union of two half-planes
}

தருக்க PlannerPolygons::intersectsInsideOf(மாறிலி PolygonInfo &poly,
		int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) மாறிலி
{
	// Single pass over polygon edges, doing two jobs:
	//  (1) classify the test segment vs each polygon edge. early-return on:
	//       - strict crossings (line segment - polygon edge)
	//       - interior crossing at vertex
	//  (2) accumulate midpoint winding contribution -- if (1) is inconclusive,
	//      this decides if the line is inside or outside

	முழு wn = 0;
	தருக்க mid_on_boundary = பொய்;

	// Rather than calculating the midpoint (mid = (s + e)/2) explicitly,
	// which will lead to rounding error for uneven numbers, we instead
	// scale all other inputs by two, reducing the range by a factor of two
	// (2^31 cm = 21400 km -> 2^30 cm = 10700 km)
	மாறிலி int32_t twice_mid_x = s_x + e_x;
	மாறிலி int32_t twice_mid_y = s_y + e_y;

	க்கு (முழு i = 0; i < poly.இலக்கம்_vertices; i++) {

		மாறிலி முழு prev_idx = poly.start_index + (i + poly.இலக்கம்_vertices - 1) % poly.இலக்கம்_vertices;
		மாறிலி முழு curr_idx = poly.start_index + i;

		மாறிலி int32_t ax = _x_cm[prev_idx], ay = _y_cm[prev_idx];
		மாறிலி int32_t bx = _x_cm[curr_idx], by = _y_cm[curr_idx];

		// (1) test segment vs polygon edge (a, b)
		தேர்வு (segmentsIntersect(ax, ay, bx, by, s_x, s_y, e_x, e_y)) {
		நிலை SegSegResult::Cross:
			திரும்பு உண்மை;

		// skip AInsideCD - a from this segment is b from the adjacent segment
		நிலை SegSegResult::BInsideCD:

			// Polygon vertex b is exactly on the interior of the test segment.
			// Return intersecting if it pokes into the polygon interior through vertex b.
			எனில் (pointInsideInteriorCone(poly, s_x, s_y, i) || pointInsideInteriorCone(poly, e_x, e_y, i)) {
				திரும்பு உண்மை;
			}

			நிறுத்து;

		இயல்பு:
			நிறுத்து;
		}

		// (2) midpoint winding contribution
		// Ref: Dan Sunday, Inclusion of a Point in a Polygon
		// https://web.archive.org/web/20130126163405/http://geomalgorithms.com/a03-_inclusion.html

		மாறிலி முழு side = orient2d(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);

		எனில் (side == 0) {

			மாறிலி தருக்க mid_on_open_ab = collinearBetween(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);
			மாறிலி தருக்க mid_is_b = 2 * bx == twice_mid_x && 2 * by == twice_mid_y;

			// Skip point a - a from this segment is b from the adjacent segment
			எனில் (mid_on_open_ab || mid_is_b) {
				mid_on_boundary = உண்மை;
			}

		} இல்லையெனில் எனில் (2 * ay <= twice_mid_y) {
			எனில் (2 * by > twice_mid_y && side > 0) { wn++; }

		} இல்லையெனில் எனில் (2 * by <= twice_mid_y && side < 0) { wn--; }
	}


	// Midpoint on boundary, segment runs along it (strict crossing detected earlier).
	// Non-intersecting in either zone.
	எனில் (mid_on_boundary) {
		திரும்பு பொய்;
	}

	// Exclusion: violation when midpoint is inside (bounded). Inclusion: when outside (unbounded).
	// This is the only place we depart from the inside = left = forbidden canonical orientation,
	// necessary because of the point membership check.
	மாறிலி தருக்க midpoint_in_bounded_inside = (wn != 0);
	திரும்பு midpoint_in_bounded_inside != poly.is_inclusion;
}

தருக்க PlannerPolygons::intersectsAnyInside(int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) மாறிலி
{
	மாறிலி int32_t seg_min_x = s_x < e_x ? s_x : e_x;
	மாறிலி int32_t seg_max_x = s_x > e_x ? s_x : e_x;
	மாறிலி int32_t seg_min_y = s_y < e_y ? s_y : e_y;
	மாறிலி int32_t seg_max_y = s_y > e_y ? s_y : e_y;

	க்கு (முழு p = 0; p < இலக்கம்_polygons; p++) {
		மாறிலி PolygonInfo &poly = _polygons[p];

		எனில் (seg_max_x < poly.min_x || seg_min_x > poly.max_x ||
		    seg_max_y < poly.min_y || seg_min_y > poly.max_y) {
			// Segment is entirely outside this polygon.
			// For exclusion zones: outside = safe, skip.
			// For inclusion zones: outside = forbidden region, immediate violation.
			எனில் (poly.is_inclusion) { திரும்பு உண்மை; }

			தொடர்;
		}

		எனில் (intersectsInsideOf(poly, s_x, s_y, e_x, e_y)) {
			திரும்பு உண்மை;
		}
	}

	திரும்பு பொய்;
}

தருக்க PlannerPolygons::edgeBitangent(முழு a, int32_t bx, int32_t by) மாறிலி
{
	// Find the polygon owning vertex index a. Indices not belonging to any
	// polygon (in particular the destination at index 0) are trivially bitangent.

	எனில் (a == destIndex()) { திரும்பு உண்மை; }

	க்கு (முழு p = 0; p < இலக்கம்_polygons; p++) {
		மாறிலி PolygonInfo &poly = _polygons[p];

		எனில் (a >= poly.start_index && a < poly.start_index + poly.இலக்கம்_vertices) {
			// Define point c = a + (a-b) = 2a - b, so that ac = ab mirrored about a
			// Then: Edge is bitangent at a <=> neither ab nor ac pokes into the interior
			int32_t cx = 2 * _x_cm[a] - bx;
			int32_t cy = 2 * _y_cm[a] - by;
			திரும்பு !pointInsideInteriorCone(poly, bx, by, a - poly.start_index)
			       && !pointInsideInteriorCone(poly, cx, cy, a - poly.start_index);
		}
	}

	திரும்பு உண்மை;
}

தருக்க PlannerPolygons::edgeBitangent(முழு a, முழு b) மாறிலி
{
	திரும்பு edgeBitangent(a, _x_cm[b], _y_cm[b]);
}

தருக்க PlannerPolygons::edgeBitangent(முழு a, மாறிலி matrix::Vector2f &b) மாறிலி
{
	திரும்பு edgeBitangent(a, metersToCm(b(0)), metersToCm(b(1)));
}

தருக்க PlannerPolygons::setDestination(மாறிலி matrix::Vector2f &p)
{
	set_முனை(destIndex(), p);

	மாறிலி int32_t x_cm = metersToCm(p(0));
	மாறிலி int32_t y_cm = metersToCm(p(1));

	எனில் (intersectsAnyInside(x_cm, y_cm, x_cm, y_cm)) {
		// If the destination breaches a geofence, still use it but
		// signal to the user that it is invalid.
		திரும்பு பொய்;
	}

	திரும்பு உண்மை;
}

matrix::Vector2f PlannerPolygons::getDestination() மாறிலி
{
	எனில் (_max_nodes < 1) {
		திரும்பு matrix::Vector2f{NAN, NAN};
	}

	திரும்பு முனை_2(destIndex());
}

தருக்க PlannerPolygons::edgeVisible(முழு a, முழு b) மாறிலி
{
	எனில் (a == b) { திரும்பு பொய்; }

	எனில் (முனை_not_on_optimal_path[a] || முனை_not_on_optimal_path[b]) { திரும்பு பொய்; }

	// Only bitangent edges can be on a shortest path; this check also cheaply
	// discards edges that violate the polygon at the vertex they come from.
	எனில் (!edgeBitangent(a, b) || !edgeBitangent(b, a)) { திரும்பு பொய்; }

	திரும்பு !intersectsAnyInside(_x_cm[a], _y_cm[a], _x_cm[b], _y_cm[b]);
}

தருக்க PlannerPolygons::edgeVisible(மாறிலி matrix::Vector2f &a, முழு b) மாறிலி
{
	எனில் (முனை_not_on_optimal_path[b]) { திரும்பு பொய்; }

	எனில் (!edgeBitangent(b, a)) { திரும்பு பொய்; }

	திரும்பு !intersectsAnyInside(metersToCm(a(0)), metersToCm(a(1)), _x_cm[b], _y_cm[b]);
}

தருக்க PlannerPolygons::edgeVisible(மாறிலி matrix::Vector2f &a, மாறிலி matrix::Vector2f &b) மாறிலி
{
	திரும்பு !intersectsAnyInside(metersToCm(a(0)), metersToCm(a(1)), metersToCm(b(0)), metersToCm(b(1)));
}

தசம PlannerPolygons::edgeCost(முழு a, முழு b) மாறிலி
{
	திரும்பு edgeVisible(a, b) ? (முனை_2(a) - முனை_2(b)).norm() : INFINITY;
}


தருக்க segmentsIntersectInclusive(மாறிலி matrix::Vector2d &a, மாறிலி matrix::Vector2d &b,
				மாறிலி matrix::Vector2d &c, மாறிலி matrix::Vector2d &d)
{
	// Reject separated bounds, including gaps between collinear segments.
	க்கு (முழு axis = 0; axis < 2; ++axis) {
		எனில் (math::max(a(axis), b(axis)) < math::min(c(axis), d(axis))
		    || math::max(c(axis), d(axis)) < math::min(a(axis), b(axis))) {
			திரும்பு பொய்;
		}
	}

	மாறிலி matrix::Vector2d ab = b - a;
	மாறிலி matrix::Vector2d cd = d - c;
	மாறிலி இரட்டை side_c = ab.cross(c - a);
	மாறிலி இரட்டை side_d = ab.cross(d - a);
	மாறிலி இரட்டை side_a = cd.cross(a - c);
	மாறிலி இரட்டை side_b = cd.cross(b - c);

	// Each segment must cross or touch the line through the other segment.
	திரும்பு ((side_c <= 0.0 && side_d >= 0.0) || (side_c >= 0.0 && side_d <= 0.0))
	       && ((side_a <= 0.0 && side_b >= 0.0) || (side_a >= 0.0 && side_b <= 0.0));
}

இரட்டை pointToSegmentDistanceSquared(மாறிலி matrix::Vector2d &point,
				     மாறிலி matrix::Vector2d &a, மாறிலி matrix::Vector2d &b)
{
	மாறிலி matrix::Vector2d direction = b - a;
	மாறிலி matrix::Vector2d to_point = point - a;
	மாறிலி இரட்டை length_squared = direction.norm_squared();
	// along / length_squared locates the projection: 0 at a, 1 at b.
	மாறிலி இரட்டை along = to_point.dot(direction);

	எனில் (along <= 0.0) {
		திரும்பு to_point.norm_squared();

	} இல்லையெனில் எனில் (along >= length_squared) {
		திரும்பு (point - b).norm_squared();
	}

	// Squared distance to the line: |to_point x direction|^2 / |direction|^2.
	// https://mathworld.wolfram.com/2-DimensionalPoint-LineDistance.html
	மாறிலி இரட்டை cross = to_point.cross(direction);
	திரும்பு cross * cross / length_squared;
}

தருக்க isPolygonCCW(மாறிலி matrix::Vector2f *vertices, முழு இலக்கம்_vertices)
{
	// https://en.wikipedia.org/wiki/Shoelace_formula
	// Triangle formula
	தசம signed_area_2x = 0.f;

	க்கு (முழு i = 0; i < இலக்கம்_vertices; i++) {
		மாறிலி முழு j = (i + 1) % இலக்கம்_vertices;
		signed_area_2x += vertices[i](0) * vertices[j](1)
				  - vertices[j](0) * vertices[i](1);
	}

	திரும்பு signed_area_2x > 0.f;
}

} // namespace geofence_utils
