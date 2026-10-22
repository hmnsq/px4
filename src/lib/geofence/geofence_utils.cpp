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

#અન્તર્ભાવ "geofence_utils.h"
#અન્તર્ભાવ <lib/geo/geo.h>
#અન્તર્ભાવ <lib/mathlib/mathlib.h>

નામાકાશ geofence_utils
{

નામાકાશ
{

int32_t metersToCm(દશાંશ m) { પાછા roundf(m * CM_PER_M); }

} // namespace


તાર્કિક PlannerPolygons::allocate(પૂર્ણાંક max_nodes)
{
	freeBuffers();

	જો (max_nodes > 0) {
		_x_cm = નવું int32_t[max_nodes];
		_y_cm = નવું int32_t[max_nodes];
		ગાંઠ_not_on_optimal_path = નવું તાર્કિક[max_nodes];

		જો (_x_cm != nullptr && _y_cm != nullptr && ગાંઠ_not_on_optimal_path != nullptr) {
			_max_nodes = max_nodes;

		} નહીંતર {
			freeBuffers();
		}
	}

	reset();

	પાછા _max_nodes == max_nodes;
}

શૂન્ય PlannerPolygons::freeBuffers()
{
	delete[] _x_cm;
	delete[] _y_cm;
	delete[] ગાંઠ_not_on_optimal_path;
	_x_cm = nullptr;
	_y_cm = nullptr;
	ગાંઠ_not_on_optimal_path = nullptr;
	_max_nodes = 0;
}

શૂન્ય PlannerPolygons::set_ગાંઠ(પૂર્ણાંક idx, અચલ matrix::Vector2f &p)
{
	જો (_max_nodes < 1) {
		પાછા;
	}

	idx = math::constrain(idx, 0, _max_nodes - 1);

	_x_cm[idx] = metersToCm(p(0));
	_y_cm[idx] = metersToCm(p(1));
}

PlannerPolygons::AddResult PlannerPolygons::addPolygon(અચલ matrix::Vector2f *vertices_in, પૂર્ણાંક અંક_vertices,
		તાર્કિક is_inclusion_zone, દશાંશ margin)
{
	જો (અંક_polygons >= kMaxPolygons || અંક_nodes_2 + અંક_vertices > _max_nodes) {
		પાછા AddResult::BudgetExceeded;
	}

	જો (margin < -FLT_EPSILON || અંક_vertices < 3) { પાછા AddResult::Degenerate; }

	માટે (પૂર્ણાંક i = 0; i < અંક_vertices; ++i) {
		// Reject self-intersecting (non-simple) polygons.
		અચલ matrix::Vector2f &a = vertices_in[i];
		અચલ matrix::Vector2f &b = vertices_in[(i + 1) % અંક_vertices];

		માટે (પૂર્ણાંક j = i + 2; j < અંક_vertices; ++j) {
			જો (i == 0 && j == અંક_vertices - 1) {
				ચાલુ; // (last edge, first edge) are adjacent
			}

			અચલ matrix::Vector2f &c = vertices_in[j];
			અચલ matrix::Vector2f &d = vertices_in[(j + 1) % અંક_vertices];

			જો (segmentsIntersect(
				    metersToCm(a(0)), metersToCm(a(1)), metersToCm(b(0)), metersToCm(b(1)),
				    metersToCm(c(0)), metersToCm(c(1)), metersToCm(d(0)), metersToCm(d(1))) != SegSegResult::Disjoint) {
				પાછા AddResult::Degenerate;
			}
		}

		// Reject vertices outside the usable fixed-point range
		જો (!inFixedPointRange(vertices_in[i](0)) || !inFixedPointRange(vertices_in[i](1))) {
			પાછા AddResult::OutOfRange;
		}
	}

	// Canonical orientation: walking vertices in stored order, INSIDE is on
	// the left of each edge. CCW for exclusion (bounded interior is forbidden),
	// CW for inclusion (unbounded exterior is forbidden). Reverse iff input
	// doesn't already match.
	અચલ તાર્કિક reverse = (isPolygonCCW(vertices_in, અંક_vertices) == is_inclusion_zone);

	// Index into vertices_in in stored order (accounts for possible reversal).
	auto vertex = [&](પૂર્ણાંક i) -> અચલ matrix::Vector2f & {
		પાછા vertices_in[reverse ? (અંક_vertices - 1 - i) : i];
	};

	PolygonInfo &poly = _polygons[અંક_polygons];
	poly.start_index  = અંક_nodes_2;
	poly.is_inclusion = is_inclusion_zone;

	પૂર્ણાંક out_idx = 0;

	માટે (પૂર્ણાંક i = 0; i < અંક_vertices; i++) {
		અચલ પૂર્ણાંક prev = (i + અંક_vertices - 1) % અંક_vertices;
		અચલ પૂર્ણાંક next = (i + 1) % અંક_vertices;

		અચલ matrix::Vector2f edge_in  = vertex(i) - vertex(prev);
		અચલ matrix::Vector2f edge_out = vertex(next) - vertex(i);

		અચલ દશાંશ edge_in_norm = edge_in.norm();
		અચલ દશાંશ edge_out_norm = edge_out.norm();

		જો (edge_in_norm < FLT_EPSILON || edge_out_norm < FLT_EPSILON) {
			પાછા AddResult::Degenerate;
		}

		અચલ matrix::Vector2f edge_in_normalized = edge_in / edge_in_norm;
		અચલ matrix::Vector2f edge_out_normalized = edge_out / edge_out_norm;

		// Inward normals of the two adjacent edges.
		// Canonical stored orientation: left side = forbidden region, right side = allowed region
		અચલ matrix::Vector2f n_in  = matrix::Vector2f{-edge_in_normalized(1), edge_in_normalized(0)};
		અચલ matrix::Vector2f n_out = matrix::Vector2f{-edge_out_normalized(1), edge_out_normalized(0)};

		matrix::Vector2f bisector = n_in + n_out;
		અચલ દશાંશ bisector_len  = bisector.length();

		જો (bisector_len < FLT_EPSILON) {
			પાછા AddResult::Degenerate; // antiparallel edges - polygon doubles back
		}

		અચલ matrix::Vector2f normalized_bisector = bisector / bisector_len;

		// The following bit ensures that:
		//  - The original edges are moved away from the forbidden interior by `margin`
		//  - Sharp convex corners (<60deg) are "cut off" with a new edge that is exactly
		//    `margin` away from the original vertex. For this the vertex is replaced by
		//    two new ones.

		// Only split convex corners - others are ignored, see _node_not_on_optimal_path
		અચલ તાર્કિક corner_convex = edge_in_normalized(0) * edge_out_normalized(1)
					   - edge_in_normalized(1) * edge_out_normalized(0) > 0.f;

		// Scaling needed wrt the normalised bisector to achieve parallel edges
		અચલ દશાંશ offset_scale = 2.f / bisector_len;

		// bisector_len = 2*sin(interior angle/2); split when interior angle < 60 deg <=> bisector_len < 1.
		અચલ તાર્કિક angle_sharp = bisector_len < 1;

		// If margin zero, do not split - would output the same vertex twice.
		અચલ તાર્કિક margin_nonzero = margin > FLT_EPSILON;

		// If we do not have enough space, do not split the vertex.
		// This never happens if the planner-internal node buffer is 2x the original buffer.
		અચલ તાર્કિક space_for_split_vertices = અંક_nodes_2 + out_idx + 1 < _max_nodes;

		જો (corner_convex && angle_sharp && margin_nonzero && space_for_split_vertices) {

			// Sharp corner: cut it off along the line
			//  - perpendicular to normalized_bisector
			//  - going through base := vertex(i) - normalized_bisector * margin

			અચલ matrix::Vector2f base = vertex(i) - normalized_bisector * margin;
			અચલ matrix::Vector2f perp = {-normalized_bisector(1), normalized_bisector(0)};

			// Construct s such that:
			//  base + perp*s is on the offset incoming edge
			//  base - perp*s is on the offset outgoing edge
			અચલ દશાંશ e_out_dot_b = edge_out_normalized.dot(normalized_bisector);
			અચલ દશાંશ s = margin * (1.f - n_out.dot(normalized_bisector)) / e_out_dot_b;

			set_ગાંઠ(poly.start_index + out_idx++, base + perp * s);
			set_ગાંઠ(poly.start_index + out_idx++, base - perp * s);

		} નહીંતર {
			// Regular corner or no space for split: single bisector vertex displaced by margin / sin(interior angle/2).
			set_ગાંઠ(poly.start_index + out_idx++, vertex(i) - normalized_bisector * (margin * offset_scale));
		}
	}

	અચલ પૂર્ણાંક અંક_out = out_idx;
	poly.અંક_vertices = અંક_out;

	computeBoundingBox(poly.start_index, અંક_out, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// In canonical orientation, the node is not possibly on the shortest
	// path if the vertex curves right or straight (forbidden region >= 180
	// deg)
	માટે (પૂર્ણાંક i = 0; i < અંક_out; i++) {
		અચલ પૂર્ણાંક prev = poly.start_index + (i + અંક_out - 1) % અંક_out;
		અચલ પૂર્ણાંક curr = poly.start_index + i;
		અચલ પૂર્ણાંક next = poly.start_index + (i + 1) % અંક_out;
		અચલ તાર્કિક curves_જમણું_or_straight = orient2d(_x_cm[prev], _y_cm[prev],
						      _x_cm[curr], _y_cm[curr],
						      _x_cm[next], _y_cm[next]) <= 0;
		ગાંઠ_not_on_optimal_path[curr] = curves_જમણું_or_straight;
	}

	અંક_nodes_2 += અંક_out;
	++અંક_polygons;
	પાછા AddResult::Success;
}

PlannerPolygons::AddResult PlannerPolygons::addApproxCircle(અચલ matrix::Vector2f &center, અચલ દશાંશ circle_radius,
		દશાંશ margin, અચલ તાર્કિક is_inclusion_zone)
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

	જો (અંક_polygons >= kMaxPolygons || અંક_nodes_2 + kCircleApproxVertices > _max_nodes) {
		પાછા AddResult::BudgetExceeded;
	}

	જો (circle_radius <= 0.1f) { પાછા AddResult::Degenerate; }

	// ensures that the k-gon is, w.r.t the real circle:
	//  - over-approximation of circle grown by margin, if exclusion
	//  - under-approximation of circle shrunk by margin, if inclusion
	અચલ દશાંશ k_gon_radius = is_inclusion_zone
				   ? circle_radius - margin
				   : circle_radius / cosf(M_PI_F / kCircleApproxVertices) + margin;

	જો (k_gon_radius <= FLT_EPSILON) { // Negative radius - circle empty
		પાછા AddResult::Degenerate;
	}

	જો (!inFixedPointRange(fabsf(center(0)) + k_gon_radius)
	    || !inFixedPointRange(fabsf(center(1)) + k_gon_radius)) {
		પાછા AddResult::OutOfRange;
	}

	// Canonical orientation: CCW for exclusion, CW for inclusion
	અચલ દશાંશ delta_angle = (is_inclusion_zone ? -1.f : 1.f) * 2.f * M_PI_F / kCircleApproxVertices;

	PolygonInfo &poly = _polygons[અંક_polygons];
	poly.start_index = અંક_nodes_2;
	poly.અંક_vertices = kCircleApproxVertices;
	poly.is_inclusion = is_inclusion_zone;

	માટે (પૂર્ણાંક i = 0; i < kCircleApproxVertices; i++) {
		અચલ દશાંશ angle = (i + 0.5f) * delta_angle;
		અચલ matrix::Vector2f p = center + matrix::Vector2f{
			k_gon_radius * cosf(angle), k_gon_radius * sinf(angle)
		};
		set_ગાંઠ(poly.start_index + i, p);
	}

	computeBoundingBox(poly.start_index, kCircleApproxVertices, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// For regular k-gons, all corners are either convex or reflex based on
	// is_inclusion, so the reduced visibility graph criterion is simplified
	માટે (પૂર્ણાંક i = 0; i < kCircleApproxVertices; i++) {
		ગાંઠ_not_on_optimal_path[poly.start_index + i] = is_inclusion_zone;
	}

	અંક_nodes_2 += kCircleApproxVertices;
	++અંક_polygons;
	પાછા AddResult::Success;
}

શૂન્ય PlannerPolygons::computeBoundingBox(અચલ પૂર્ણાંક start_index, અચલ પૂર્ણાંક અંક_vertices,
		int32_t &min_x, int32_t &max_x, int32_t &min_y, int32_t &max_y)
{

	// Store the bounding box, so we can do a very cheap check against the
	// bounding box, and only do the full line-polygon intersection check if
	// the line intersects the bounding box.

	min_x = max_x = _x_cm[start_index];
	min_y = max_y = _y_cm[start_index];

	માટે (પૂર્ણાંક i = 1; i < અંક_vertices; i++) {
		અચલ int32_t x = _x_cm[start_index + i];
		અચલ int32_t y = _y_cm[start_index + i];

		જો (x < min_x) { min_x = x; }

		જો (x > max_x) { max_x = x; }

		જો (y < min_y) { min_y = y; }

		જો (y > max_y) { max_y = y; }
	}
}

તાર્કિક PlannerPolygons::pointInsideInteriorCone(અચલ PolygonInfo &poly,
		int32_t px, int32_t py, પૂર્ણાંક v) અચલ
{
	અચલ પૂર્ણાંક prev = poly.start_index + ((v + poly.અંક_vertices - 1) % poly.અંક_vertices);
	અચલ પૂર્ણાંક curr = poly.start_index + v;
	અચલ પૂર્ણાંક next = poly.start_index + ((v + 1) % poly.અંક_vertices);

	અચલ પૂર્ણાંક p_ડાબું_of_incoming  = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], px, py);
	અચલ પૂર્ણાંક p_ડાબું_of_outgoing  = orient2d(_x_cm[curr], _y_cm[curr], _x_cm[next], _y_cm[next], px, py);
	અચલ પૂર્ણાંક is_convex           = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], _x_cm[next], _y_cm[next]);

	પાછા is_convex > 0  // If convex==0 (exact 180 deg "corner") the two cases are equivalent
	       ? (p_ડાબું_of_incoming > 0 && p_ડાબું_of_outgoing > 0)   // Convex: interior = intersection of two half-planes
	       : (p_ડાબું_of_incoming > 0 || p_ડાબું_of_outgoing > 0);  // Reflex: interior = union of two half-planes
}

તાર્કિક PlannerPolygons::intersectsInsideOf(અચલ PolygonInfo &poly,
		int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) અચલ
{
	// Single pass over polygon edges, doing two jobs:
	//  (1) classify the test segment vs each polygon edge. early-return on:
	//       - strict crossings (line segment - polygon edge)
	//       - interior crossing at vertex
	//  (2) accumulate midpoint winding contribution -- if (1) is inconclusive,
	//      this decides if the line is inside or outside

	પૂર્ણાંક wn = 0;
	તાર્કિક mid_on_boundary = ખોટું;

	// Rather than calculating the midpoint (mid = (s + e)/2) explicitly,
	// which will lead to rounding error for uneven numbers, we instead
	// scale all other inputs by two, reducing the range by a factor of two
	// (2^31 cm = 21400 km -> 2^30 cm = 10700 km)
	અચલ int32_t twice_mid_x = s_x + e_x;
	અચલ int32_t twice_mid_y = s_y + e_y;

	માટે (પૂર્ણાંક i = 0; i < poly.અંક_vertices; i++) {

		અચલ પૂર્ણાંક prev_idx = poly.start_index + (i + poly.અંક_vertices - 1) % poly.અંક_vertices;
		અચલ પૂર્ણાંક curr_idx = poly.start_index + i;

		અચલ int32_t ax = _x_cm[prev_idx], ay = _y_cm[prev_idx];
		અચલ int32_t bx = _x_cm[curr_idx], by = _y_cm[curr_idx];

		// (1) test segment vs polygon edge (a, b)
		પસંદગી (segmentsIntersect(ax, ay, bx, by, s_x, s_y, e_x, e_y)) {
		સ્થિતિ SegSegResult::Cross:
			પાછા સાચું;

		// skip AInsideCD - a from this segment is b from the adjacent segment
		સ્થિતિ SegSegResult::BInsideCD:

			// Polygon vertex b is exactly on the interior of the test segment.
			// Return intersecting if it pokes into the polygon interior through vertex b.
			જો (pointInsideInteriorCone(poly, s_x, s_y, i) || pointInsideInteriorCone(poly, e_x, e_y, i)) {
				પાછા સાચું;
			}

			તોડો;

		મૂળભૂત:
			તોડો;
		}

		// (2) midpoint winding contribution
		// Ref: Dan Sunday, Inclusion of a Point in a Polygon
		// https://web.archive.org/web/20130126163405/http://geomalgorithms.com/a03-_inclusion.html

		અચલ પૂર્ણાંક side = orient2d(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);

		જો (side == 0) {

			અચલ તાર્કિક mid_on_open_ab = collinearBetween(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);
			અચલ તાર્કિક mid_is_b = 2 * bx == twice_mid_x && 2 * by == twice_mid_y;

			// Skip point a - a from this segment is b from the adjacent segment
			જો (mid_on_open_ab || mid_is_b) {
				mid_on_boundary = સાચું;
			}

		} નહીંતર જો (2 * ay <= twice_mid_y) {
			જો (2 * by > twice_mid_y && side > 0) { wn++; }

		} નહીંતર જો (2 * by <= twice_mid_y && side < 0) { wn--; }
	}


	// Midpoint on boundary, segment runs along it (strict crossing detected earlier).
	// Non-intersecting in either zone.
	જો (mid_on_boundary) {
		પાછા ખોટું;
	}

	// Exclusion: violation when midpoint is inside (bounded). Inclusion: when outside (unbounded).
	// This is the only place we depart from the inside = left = forbidden canonical orientation,
	// necessary because of the point membership check.
	અચલ તાર્કિક midpoint_in_bounded_inside = (wn != 0);
	પાછા midpoint_in_bounded_inside != poly.is_inclusion;
}

તાર્કિક PlannerPolygons::intersectsAnyInside(int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) અચલ
{
	અચલ int32_t seg_min_x = s_x < e_x ? s_x : e_x;
	અચલ int32_t seg_max_x = s_x > e_x ? s_x : e_x;
	અચલ int32_t seg_min_y = s_y < e_y ? s_y : e_y;
	અચલ int32_t seg_max_y = s_y > e_y ? s_y : e_y;

	માટે (પૂર્ણાંક p = 0; p < અંક_polygons; p++) {
		અચલ PolygonInfo &poly = _polygons[p];

		જો (seg_max_x < poly.min_x || seg_min_x > poly.max_x ||
		    seg_max_y < poly.min_y || seg_min_y > poly.max_y) {
			// Segment is entirely outside this polygon.
			// For exclusion zones: outside = safe, skip.
			// For inclusion zones: outside = forbidden region, immediate violation.
			જો (poly.is_inclusion) { પાછા સાચું; }

			ચાલુ;
		}

		જો (intersectsInsideOf(poly, s_x, s_y, e_x, e_y)) {
			પાછા સાચું;
		}
	}

	પાછા ખોટું;
}

તાર્કિક PlannerPolygons::edgeBitangent(પૂર્ણાંક a, int32_t bx, int32_t by) અચલ
{
	// Find the polygon owning vertex index a. Indices not belonging to any
	// polygon (in particular the destination at index 0) are trivially bitangent.

	જો (a == destIndex()) { પાછા સાચું; }

	માટે (પૂર્ણાંક p = 0; p < અંક_polygons; p++) {
		અચલ PolygonInfo &poly = _polygons[p];

		જો (a >= poly.start_index && a < poly.start_index + poly.અંક_vertices) {
			// Define point c = a + (a-b) = 2a - b, so that ac = ab mirrored about a
			// Then: Edge is bitangent at a <=> neither ab nor ac pokes into the interior
			int32_t cx = 2 * _x_cm[a] - bx;
			int32_t cy = 2 * _y_cm[a] - by;
			પાછા !pointInsideInteriorCone(poly, bx, by, a - poly.start_index)
			       && !pointInsideInteriorCone(poly, cx, cy, a - poly.start_index);
		}
	}

	પાછા સાચું;
}

તાર્કિક PlannerPolygons::edgeBitangent(પૂર્ણાંક a, પૂર્ણાંક b) અચલ
{
	પાછા edgeBitangent(a, _x_cm[b], _y_cm[b]);
}

તાર્કિક PlannerPolygons::edgeBitangent(પૂર્ણાંક a, અચલ matrix::Vector2f &b) અચલ
{
	પાછા edgeBitangent(a, metersToCm(b(0)), metersToCm(b(1)));
}

તાર્કિક PlannerPolygons::setDestination(અચલ matrix::Vector2f &p)
{
	set_ગાંઠ(destIndex(), p);

	અચલ int32_t x_cm = metersToCm(p(0));
	અચલ int32_t y_cm = metersToCm(p(1));

	જો (intersectsAnyInside(x_cm, y_cm, x_cm, y_cm)) {
		// If the destination breaches a geofence, still use it but
		// signal to the user that it is invalid.
		પાછા ખોટું;
	}

	પાછા સાચું;
}

matrix::Vector2f PlannerPolygons::getDestination() અચલ
{
	જો (_max_nodes < 1) {
		પાછા matrix::Vector2f{NAN, NAN};
	}

	પાછા ગાંઠ_2(destIndex());
}

તાર્કિક PlannerPolygons::edgeVisible(પૂર્ણાંક a, પૂર્ણાંક b) અચલ
{
	જો (a == b) { પાછા ખોટું; }

	જો (ગાંઠ_not_on_optimal_path[a] || ગાંઠ_not_on_optimal_path[b]) { પાછા ખોટું; }

	// Only bitangent edges can be on a shortest path; this check also cheaply
	// discards edges that violate the polygon at the vertex they come from.
	જો (!edgeBitangent(a, b) || !edgeBitangent(b, a)) { પાછા ખોટું; }

	પાછા !intersectsAnyInside(_x_cm[a], _y_cm[a], _x_cm[b], _y_cm[b]);
}

તાર્કિક PlannerPolygons::edgeVisible(અચલ matrix::Vector2f &a, પૂર્ણાંક b) અચલ
{
	જો (ગાંઠ_not_on_optimal_path[b]) { પાછા ખોટું; }

	જો (!edgeBitangent(b, a)) { પાછા ખોટું; }

	પાછા !intersectsAnyInside(metersToCm(a(0)), metersToCm(a(1)), _x_cm[b], _y_cm[b]);
}

તાર્કિક PlannerPolygons::edgeVisible(અચલ matrix::Vector2f &a, અચલ matrix::Vector2f &b) અચલ
{
	પાછા !intersectsAnyInside(metersToCm(a(0)), metersToCm(a(1)), metersToCm(b(0)), metersToCm(b(1)));
}

દશાંશ PlannerPolygons::edgeCost(પૂર્ણાંક a, પૂર્ણાંક b) અચલ
{
	પાછા edgeVisible(a, b) ? (ગાંઠ_2(a) - ગાંઠ_2(b)).norm() : INFINITY;
}


તાર્કિક segmentsIntersectInclusive(અચલ matrix::Vector2d &a, અચલ matrix::Vector2d &b,
				અચલ matrix::Vector2d &c, અચલ matrix::Vector2d &d)
{
	// Reject separated bounds, including gaps between collinear segments.
	માટે (પૂર્ણાંક axis = 0; axis < 2; ++axis) {
		જો (math::max(a(axis), b(axis)) < math::min(c(axis), d(axis))
		    || math::max(c(axis), d(axis)) < math::min(a(axis), b(axis))) {
			પાછા ખોટું;
		}
	}

	અચલ matrix::Vector2d ab = b - a;
	અચલ matrix::Vector2d cd = d - c;
	અચલ દ્વિગુણ side_c = ab.cross(c - a);
	અચલ દ્વિગુણ side_d = ab.cross(d - a);
	અચલ દ્વિગુણ side_a = cd.cross(a - c);
	અચલ દ્વિગુણ side_b = cd.cross(b - c);

	// Each segment must cross or touch the line through the other segment.
	પાછા ((side_c <= 0.0 && side_d >= 0.0) || (side_c >= 0.0 && side_d <= 0.0))
	       && ((side_a <= 0.0 && side_b >= 0.0) || (side_a >= 0.0 && side_b <= 0.0));
}

દ્વિગુણ pointToSegmentDistanceSquared(અચલ matrix::Vector2d &point,
				     અચલ matrix::Vector2d &a, અચલ matrix::Vector2d &b)
{
	અચલ matrix::Vector2d direction = b - a;
	અચલ matrix::Vector2d to_point = point - a;
	અચલ દ્વિગુણ length_squared = direction.norm_squared();
	// along / length_squared locates the projection: 0 at a, 1 at b.
	અચલ દ્વિગુણ along = to_point.dot(direction);

	જો (along <= 0.0) {
		પાછા to_point.norm_squared();

	} નહીંતર જો (along >= length_squared) {
		પાછા (point - b).norm_squared();
	}

	// Squared distance to the line: |to_point x direction|^2 / |direction|^2.
	// https://mathworld.wolfram.com/2-DimensionalPoint-LineDistance.html
	અચલ દ્વિગુણ cross = to_point.cross(direction);
	પાછા cross * cross / length_squared;
}

તાર્કિક isPolygonCCW(અચલ matrix::Vector2f *vertices, પૂર્ણાંક અંક_vertices)
{
	// https://en.wikipedia.org/wiki/Shoelace_formula
	// Triangle formula
	દશાંશ signed_area_2x = 0.f;

	માટે (પૂર્ણાંક i = 0; i < અંક_vertices; i++) {
		અચલ પૂર્ણાંક j = (i + 1) % અંક_vertices;
		signed_area_2x += vertices[i](0) * vertices[j](1)
				  - vertices[j](0) * vertices[i](1);
	}

	પાછા signed_area_2x > 0.f;
}

} // namespace geofence_utils
