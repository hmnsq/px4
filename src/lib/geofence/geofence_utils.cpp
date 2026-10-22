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

#ଅନ୍ତର୍ଭାଵ "geofence_utils.h"
#ଅନ୍ତର୍ଭାଵ <lib/geo/geo.h>
#ଅନ୍ତର୍ଭାଵ <lib/mathlib/mathlib.h>

ନାମାକାଶ geofence_utils
{

ନାମାକାଶ
{

int32_t metersToCm(ଦଶମିକ m) { ଫେରନ୍ତୁ roundf(m * CM_PER_M); }

} // namespace


ତାର୍କିକ PlannerPolygons::allocate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_nodes)
{
	freeBuffers();

	ଯଦି (max_nodes > 0) {
		_x_cm = ନୂତନ int32_t[max_nodes];
		_y_cm = ନୂତନ int32_t[max_nodes];
		ଗଣ୍ଠି_not_on_optimal_path = ନୂତନ ତାର୍କିକ[max_nodes];

		ଯଦି (_x_cm != nullptr && _y_cm != nullptr && ଗଣ୍ଠି_not_on_optimal_path != nullptr) {
			_max_nodes = max_nodes;

		} ଅନ୍ୟଥା {
			freeBuffers();
		}
	}

	reset();

	ଫେରନ୍ତୁ _max_nodes == max_nodes;
}

ଶୂନ୍ୟ PlannerPolygons::freeBuffers()
{
	delete[] _x_cm;
	delete[] _y_cm;
	delete[] ଗଣ୍ଠି_not_on_optimal_path;
	_x_cm = nullptr;
	_y_cm = nullptr;
	ଗଣ୍ଠି_not_on_optimal_path = nullptr;
	_max_nodes = 0;
}

ଶୂନ୍ୟ PlannerPolygons::set_ଗଣ୍ଠି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ଅଚଳ matrix::Vector2f &p)
{
	ଯଦି (_max_nodes < 1) {
		ଫେରନ୍ତୁ;
	}

	idx = math::constrain(idx, 0, _max_nodes - 1);

	_x_cm[idx] = metersToCm(p(0));
	_y_cm[idx] = metersToCm(p(1));
}

PlannerPolygons::AddResult PlannerPolygons::addPolygon(ଅଚଳ matrix::Vector2f *vertices_in, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_vertices,
		ତାର୍କିକ is_inclusion_zone, ଦଶମିକ margin)
{
	ଯଦି (ଅଙ୍କ_polygons >= kMaxPolygons || ଅଙ୍କ_nodes_2 + ଅଙ୍କ_vertices > _max_nodes) {
		ଫେରନ୍ତୁ AddResult::BudgetExceeded;
	}

	ଯଦି (margin < -FLT_EPSILON || ଅଙ୍କ_vertices < 3) { ଫେରନ୍ତୁ AddResult::Degenerate; }

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_vertices; ++i) {
		// Reject self-intersecting (non-simple) polygons.
		ଅଚଳ matrix::Vector2f &a = vertices_in[i];
		ଅଚଳ matrix::Vector2f &b = vertices_in[(i + 1) % ଅଙ୍କ_vertices];

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = i + 2; j < ଅଙ୍କ_vertices; ++j) {
			ଯଦି (i == 0 && j == ଅଙ୍କ_vertices - 1) {
				ଜାରି; // (last edge, first edge) are adjacent
			}

			ଅଚଳ matrix::Vector2f &c = vertices_in[j];
			ଅଚଳ matrix::Vector2f &d = vertices_in[(j + 1) % ଅଙ୍କ_vertices];

			ଯଦି (segmentsIntersect(
				    metersToCm(a(0)), metersToCm(a(1)), metersToCm(b(0)), metersToCm(b(1)),
				    metersToCm(c(0)), metersToCm(c(1)), metersToCm(d(0)), metersToCm(d(1))) != SegSegResult::Disjoint) {
				ଫେରନ୍ତୁ AddResult::Degenerate;
			}
		}

		// Reject vertices outside the usable fixed-point range
		ଯଦି (!inFixedPointRange(vertices_in[i](0)) || !inFixedPointRange(vertices_in[i](1))) {
			ଫେରନ୍ତୁ AddResult::OutOfRange;
		}
	}

	// Canonical orientation: walking vertices in stored order, INSIDE is on
	// the left of each edge. CCW for exclusion (bounded interior is forbidden),
	// CW for inclusion (unbounded exterior is forbidden). Reverse iff input
	// doesn't already match.
	ଅଚଳ ତାର୍କିକ reverse = (isPolygonCCW(vertices_in, ଅଙ୍କ_vertices) == is_inclusion_zone);

	// Index into vertices_in in stored order (accounts for possible reversal).
	auto vertex = [&](ପୂର୍ଣ୍ଣସଂଖ୍ୟା i) -> ଅଚଳ matrix::Vector2f & {
		ଫେରନ୍ତୁ vertices_in[reverse ? (ଅଙ୍କ_vertices - 1 - i) : i];
	};

	PolygonInfo &poly = _polygons[ଅଙ୍କ_polygons];
	poly.start_index  = ଅଙ୍କ_nodes_2;
	poly.is_inclusion = is_inclusion_zone;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା out_idx = 0;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_vertices; i++) {
		ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev = (i + ଅଙ୍କ_vertices - 1) % ଅଙ୍କ_vertices;
		ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା next = (i + 1) % ଅଙ୍କ_vertices;

		ଅଚଳ matrix::Vector2f edge_in  = vertex(i) - vertex(prev);
		ଅଚଳ matrix::Vector2f edge_out = vertex(next) - vertex(i);

		ଅଚଳ ଦଶମିକ edge_in_norm = edge_in.norm();
		ଅଚଳ ଦଶମିକ edge_out_norm = edge_out.norm();

		ଯଦି (edge_in_norm < FLT_EPSILON || edge_out_norm < FLT_EPSILON) {
			ଫେରନ୍ତୁ AddResult::Degenerate;
		}

		ଅଚଳ matrix::Vector2f edge_in_normalized = edge_in / edge_in_norm;
		ଅଚଳ matrix::Vector2f edge_out_normalized = edge_out / edge_out_norm;

		// Inward normals of the two adjacent edges.
		// Canonical stored orientation: left side = forbidden region, right side = allowed region
		ଅଚଳ matrix::Vector2f n_in  = matrix::Vector2f{-edge_in_normalized(1), edge_in_normalized(0)};
		ଅଚଳ matrix::Vector2f n_out = matrix::Vector2f{-edge_out_normalized(1), edge_out_normalized(0)};

		matrix::Vector2f bisector = n_in + n_out;
		ଅଚଳ ଦଶମିକ bisector_len  = bisector.length();

		ଯଦି (bisector_len < FLT_EPSILON) {
			ଫେରନ୍ତୁ AddResult::Degenerate; // antiparallel edges - polygon doubles back
		}

		ଅଚଳ matrix::Vector2f normalized_bisector = bisector / bisector_len;

		// The following bit ensures that:
		//  - The original edges are moved away from the forbidden interior by `margin`
		//  - Sharp convex corners (<60deg) are "cut off" with a new edge that is exactly
		//    `margin` away from the original vertex. For this the vertex is replaced by
		//    two new ones.

		// Only split convex corners - others are ignored, see _node_not_on_optimal_path
		ଅଚଳ ତାର୍କିକ corner_convex = edge_in_normalized(0) * edge_out_normalized(1)
					   - edge_in_normalized(1) * edge_out_normalized(0) > 0.f;

		// Scaling needed wrt the normalised bisector to achieve parallel edges
		ଅଚଳ ଦଶମିକ offset_scale = 2.f / bisector_len;

		// bisector_len = 2*sin(interior angle/2); split when interior angle < 60 deg <=> bisector_len < 1.
		ଅଚଳ ତାର୍କିକ angle_sharp = bisector_len < 1;

		// If margin zero, do not split - would output the same vertex twice.
		ଅଚଳ ତାର୍କିକ margin_nonzero = margin > FLT_EPSILON;

		// If we do not have enough space, do not split the vertex.
		// This never happens if the planner-internal node buffer is 2x the original buffer.
		ଅଚଳ ତାର୍କିକ space_for_split_vertices = ଅଙ୍କ_nodes_2 + out_idx + 1 < _max_nodes;

		ଯଦି (corner_convex && angle_sharp && margin_nonzero && space_for_split_vertices) {

			// Sharp corner: cut it off along the line
			//  - perpendicular to normalized_bisector
			//  - going through base := vertex(i) - normalized_bisector * margin

			ଅଚଳ matrix::Vector2f base = vertex(i) - normalized_bisector * margin;
			ଅଚଳ matrix::Vector2f perp = {-normalized_bisector(1), normalized_bisector(0)};

			// Construct s such that:
			//  base + perp*s is on the offset incoming edge
			//  base - perp*s is on the offset outgoing edge
			ଅଚଳ ଦଶମିକ e_out_dot_b = edge_out_normalized.dot(normalized_bisector);
			ଅଚଳ ଦଶମିକ s = margin * (1.f - n_out.dot(normalized_bisector)) / e_out_dot_b;

			set_ଗଣ୍ଠି(poly.start_index + out_idx++, base + perp * s);
			set_ଗଣ୍ଠି(poly.start_index + out_idx++, base - perp * s);

		} ଅନ୍ୟଥା {
			// Regular corner or no space for split: single bisector vertex displaced by margin / sin(interior angle/2).
			set_ଗଣ୍ଠି(poly.start_index + out_idx++, vertex(i) - normalized_bisector * (margin * offset_scale));
		}
	}

	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_out = out_idx;
	poly.ଅଙ୍କ_vertices = ଅଙ୍କ_out;

	computeBoundingBox(poly.start_index, ଅଙ୍କ_out, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// In canonical orientation, the node is not possibly on the shortest
	// path if the vertex curves right or straight (forbidden region >= 180
	// deg)
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_out; i++) {
		ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev = poly.start_index + (i + ଅଙ୍କ_out - 1) % ଅଙ୍କ_out;
		ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା curr = poly.start_index + i;
		ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା next = poly.start_index + (i + 1) % ଅଙ୍କ_out;
		ଅଚଳ ତାର୍କିକ curves_ଡାହାଣ_or_straight = orient2d(_x_cm[prev], _y_cm[prev],
						      _x_cm[curr], _y_cm[curr],
						      _x_cm[next], _y_cm[next]) <= 0;
		ଗଣ୍ଠି_not_on_optimal_path[curr] = curves_ଡାହାଣ_or_straight;
	}

	ଅଙ୍କ_nodes_2 += ଅଙ୍କ_out;
	++ଅଙ୍କ_polygons;
	ଫେରନ୍ତୁ AddResult::Success;
}

PlannerPolygons::AddResult PlannerPolygons::addApproxCircle(ଅଚଳ matrix::Vector2f &center, ଅଚଳ ଦଶମିକ circle_radius,
		ଦଶମିକ margin, ଅଚଳ ତାର୍କିକ is_inclusion_zone)
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

	ଯଦି (ଅଙ୍କ_polygons >= kMaxPolygons || ଅଙ୍କ_nodes_2 + kCircleApproxVertices > _max_nodes) {
		ଫେରନ୍ତୁ AddResult::BudgetExceeded;
	}

	ଯଦି (circle_radius <= 0.1f) { ଫେରନ୍ତୁ AddResult::Degenerate; }

	// ensures that the k-gon is, w.r.t the real circle:
	//  - over-approximation of circle grown by margin, if exclusion
	//  - under-approximation of circle shrunk by margin, if inclusion
	ଅଚଳ ଦଶମିକ k_gon_radius = is_inclusion_zone
				   ? circle_radius - margin
				   : circle_radius / cosf(M_PI_F / kCircleApproxVertices) + margin;

	ଯଦି (k_gon_radius <= FLT_EPSILON) { // Negative radius - circle empty
		ଫେରନ୍ତୁ AddResult::Degenerate;
	}

	ଯଦି (!inFixedPointRange(fabsf(center(0)) + k_gon_radius)
	    || !inFixedPointRange(fabsf(center(1)) + k_gon_radius)) {
		ଫେରନ୍ତୁ AddResult::OutOfRange;
	}

	// Canonical orientation: CCW for exclusion, CW for inclusion
	ଅଚଳ ଦଶମିକ delta_angle = (is_inclusion_zone ? -1.f : 1.f) * 2.f * M_PI_F / kCircleApproxVertices;

	PolygonInfo &poly = _polygons[ଅଙ୍କ_polygons];
	poly.start_index = ଅଙ୍କ_nodes_2;
	poly.ଅଙ୍କ_vertices = kCircleApproxVertices;
	poly.is_inclusion = is_inclusion_zone;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < kCircleApproxVertices; i++) {
		ଅଚଳ ଦଶମିକ angle = (i + 0.5f) * delta_angle;
		ଅଚଳ matrix::Vector2f p = center + matrix::Vector2f{
			k_gon_radius * cosf(angle), k_gon_radius * sinf(angle)
		};
		set_ଗଣ୍ଠି(poly.start_index + i, p);
	}

	computeBoundingBox(poly.start_index, kCircleApproxVertices, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// For regular k-gons, all corners are either convex or reflex based on
	// is_inclusion, so the reduced visibility graph criterion is simplified
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < kCircleApproxVertices; i++) {
		ଗଣ୍ଠି_not_on_optimal_path[poly.start_index + i] = is_inclusion_zone;
	}

	ଅଙ୍କ_nodes_2 += kCircleApproxVertices;
	++ଅଙ୍କ_polygons;
	ଫେରନ୍ତୁ AddResult::Success;
}

ଶୂନ୍ୟ PlannerPolygons::computeBoundingBox(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_index, ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_vertices,
		int32_t &min_x, int32_t &max_x, int32_t &min_y, int32_t &max_y)
{

	// Store the bounding box, so we can do a very cheap check against the
	// bounding box, and only do the full line-polygon intersection check if
	// the line intersects the bounding box.

	min_x = max_x = _x_cm[start_index];
	min_y = max_y = _y_cm[start_index];

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < ଅଙ୍କ_vertices; i++) {
		ଅଚଳ int32_t x = _x_cm[start_index + i];
		ଅଚଳ int32_t y = _y_cm[start_index + i];

		ଯଦି (x < min_x) { min_x = x; }

		ଯଦି (x > max_x) { max_x = x; }

		ଯଦି (y < min_y) { min_y = y; }

		ଯଦି (y > max_y) { max_y = y; }
	}
}

ତାର୍କିକ PlannerPolygons::pointInsideInteriorCone(ଅଚଳ PolygonInfo &poly,
		int32_t px, int32_t py, ପୂର୍ଣ୍ଣସଂଖ୍ୟା v) ଅଚଳ
{
	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev = poly.start_index + ((v + poly.ଅଙ୍କ_vertices - 1) % poly.ଅଙ୍କ_vertices);
	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା curr = poly.start_index + v;
	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା next = poly.start_index + ((v + 1) % poly.ଅଙ୍କ_vertices);

	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା p_ବାମ_of_incoming  = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], px, py);
	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା p_ବାମ_of_outgoing  = orient2d(_x_cm[curr], _y_cm[curr], _x_cm[next], _y_cm[next], px, py);
	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_convex           = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], _x_cm[next], _y_cm[next]);

	ଫେରନ୍ତୁ is_convex > 0  // If convex==0 (exact 180 deg "corner") the two cases are equivalent
	       ? (p_ବାମ_of_incoming > 0 && p_ବାମ_of_outgoing > 0)   // Convex: interior = intersection of two half-planes
	       : (p_ବାମ_of_incoming > 0 || p_ବାମ_of_outgoing > 0);  // Reflex: interior = union of two half-planes
}

ତାର୍କିକ PlannerPolygons::intersectsInsideOf(ଅଚଳ PolygonInfo &poly,
		int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ଅଚଳ
{
	// Single pass over polygon edges, doing two jobs:
	//  (1) classify the test segment vs each polygon edge. early-return on:
	//       - strict crossings (line segment - polygon edge)
	//       - interior crossing at vertex
	//  (2) accumulate midpoint winding contribution -- if (1) is inconclusive,
	//      this decides if the line is inside or outside

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା wn = 0;
	ତାର୍କିକ mid_on_boundary = ମିଥ୍ୟା;

	// Rather than calculating the midpoint (mid = (s + e)/2) explicitly,
	// which will lead to rounding error for uneven numbers, we instead
	// scale all other inputs by two, reducing the range by a factor of two
	// (2^31 cm = 21400 km -> 2^30 cm = 10700 km)
	ଅଚଳ int32_t twice_mid_x = s_x + e_x;
	ଅଚଳ int32_t twice_mid_y = s_y + e_y;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < poly.ଅଙ୍କ_vertices; i++) {

		ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_idx = poly.start_index + (i + poly.ଅଙ୍କ_vertices - 1) % poly.ଅଙ୍କ_vertices;
		ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା curr_idx = poly.start_index + i;

		ଅଚଳ int32_t ax = _x_cm[prev_idx], ay = _y_cm[prev_idx];
		ଅଚଳ int32_t bx = _x_cm[curr_idx], by = _y_cm[curr_idx];

		// (1) test segment vs polygon edge (a, b)
		ବାଛ (segmentsIntersect(ax, ay, bx, by, s_x, s_y, e_x, e_y)) {
		ସ୍ଥିତି SegSegResult::Cross:
			ଫେରନ୍ତୁ ସତ୍ୟ;

		// skip AInsideCD - a from this segment is b from the adjacent segment
		ସ୍ଥିତି SegSegResult::BInsideCD:

			// Polygon vertex b is exactly on the interior of the test segment.
			// Return intersecting if it pokes into the polygon interior through vertex b.
			ଯଦି (pointInsideInteriorCone(poly, s_x, s_y, i) || pointInsideInteriorCone(poly, e_x, e_y, i)) {
				ଫେରନ୍ତୁ ସତ୍ୟ;
			}

			ଭାଙ୍ଗ;

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଭାଙ୍ଗ;
		}

		// (2) midpoint winding contribution
		// Ref: Dan Sunday, Inclusion of a Point in a Polygon
		// https://web.archive.org/web/20130126163405/http://geomalgorithms.com/a03-_inclusion.html

		ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା side = orient2d(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);

		ଯଦି (side == 0) {

			ଅଚଳ ତାର୍କିକ mid_on_open_ab = collinearBetween(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);
			ଅଚଳ ତାର୍କିକ mid_is_b = 2 * bx == twice_mid_x && 2 * by == twice_mid_y;

			// Skip point a - a from this segment is b from the adjacent segment
			ଯଦି (mid_on_open_ab || mid_is_b) {
				mid_on_boundary = ସତ୍ୟ;
			}

		} ଅନ୍ୟଥା ଯଦି (2 * ay <= twice_mid_y) {
			ଯଦି (2 * by > twice_mid_y && side > 0) { wn++; }

		} ଅନ୍ୟଥା ଯଦି (2 * by <= twice_mid_y && side < 0) { wn--; }
	}


	// Midpoint on boundary, segment runs along it (strict crossing detected earlier).
	// Non-intersecting in either zone.
	ଯଦି (mid_on_boundary) {
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	// Exclusion: violation when midpoint is inside (bounded). Inclusion: when outside (unbounded).
	// This is the only place we depart from the inside = left = forbidden canonical orientation,
	// necessary because of the point membership check.
	ଅଚଳ ତାର୍କିକ midpoint_in_bounded_inside = (wn != 0);
	ଫେରନ୍ତୁ midpoint_in_bounded_inside != poly.is_inclusion;
}

ତାର୍କିକ PlannerPolygons::intersectsAnyInside(int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ଅଚଳ
{
	ଅଚଳ int32_t seg_min_x = s_x < e_x ? s_x : e_x;
	ଅଚଳ int32_t seg_max_x = s_x > e_x ? s_x : e_x;
	ଅଚଳ int32_t seg_min_y = s_y < e_y ? s_y : e_y;
	ଅଚଳ int32_t seg_max_y = s_y > e_y ? s_y : e_y;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା p = 0; p < ଅଙ୍କ_polygons; p++) {
		ଅଚଳ PolygonInfo &poly = _polygons[p];

		ଯଦି (seg_max_x < poly.min_x || seg_min_x > poly.max_x ||
		    seg_max_y < poly.min_y || seg_min_y > poly.max_y) {
			// Segment is entirely outside this polygon.
			// For exclusion zones: outside = safe, skip.
			// For inclusion zones: outside = forbidden region, immediate violation.
			ଯଦି (poly.is_inclusion) { ଫେରନ୍ତୁ ସତ୍ୟ; }

			ଜାରି;
		}

		ଯଦି (intersectsInsideOf(poly, s_x, s_y, e_x, e_y)) {
			ଫେରନ୍ତୁ ସତ୍ୟ;
		}
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ PlannerPolygons::edgeBitangent(ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, int32_t bx, int32_t by) ଅଚଳ
{
	// Find the polygon owning vertex index a. Indices not belonging to any
	// polygon (in particular the destination at index 0) are trivially bitangent.

	ଯଦି (a == destIndex()) { ଫେରନ୍ତୁ ସତ୍ୟ; }

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା p = 0; p < ଅଙ୍କ_polygons; p++) {
		ଅଚଳ PolygonInfo &poly = _polygons[p];

		ଯଦି (a >= poly.start_index && a < poly.start_index + poly.ଅଙ୍କ_vertices) {
			// Define point c = a + (a-b) = 2a - b, so that ac = ab mirrored about a
			// Then: Edge is bitangent at a <=> neither ab nor ac pokes into the interior
			int32_t cx = 2 * _x_cm[a] - bx;
			int32_t cy = 2 * _y_cm[a] - by;
			ଫେରନ୍ତୁ !pointInsideInteriorCone(poly, bx, by, a - poly.start_index)
			       && !pointInsideInteriorCone(poly, cx, cy, a - poly.start_index);
		}
	}

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ PlannerPolygons::edgeBitangent(ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b) ଅଚଳ
{
	ଫେରନ୍ତୁ edgeBitangent(a, _x_cm[b], _y_cm[b]);
}

ତାର୍କିକ PlannerPolygons::edgeBitangent(ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, ଅଚଳ matrix::Vector2f &b) ଅଚଳ
{
	ଫେରନ୍ତୁ edgeBitangent(a, metersToCm(b(0)), metersToCm(b(1)));
}

ତାର୍କିକ PlannerPolygons::setDestination(ଅଚଳ matrix::Vector2f &p)
{
	set_ଗଣ୍ଠି(destIndex(), p);

	ଅଚଳ int32_t x_cm = metersToCm(p(0));
	ଅଚଳ int32_t y_cm = metersToCm(p(1));

	ଯଦି (intersectsAnyInside(x_cm, y_cm, x_cm, y_cm)) {
		// If the destination breaches a geofence, still use it but
		// signal to the user that it is invalid.
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

matrix::Vector2f PlannerPolygons::getDestination() ଅଚଳ
{
	ଯଦି (_max_nodes < 1) {
		ଫେରନ୍ତୁ matrix::Vector2f{NAN, NAN};
	}

	ଫେରନ୍ତୁ ଗଣ୍ଠି_2(destIndex());
}

ତାର୍କିକ PlannerPolygons::edgeVisible(ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b) ଅଚଳ
{
	ଯଦି (a == b) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }

	ଯଦି (ଗଣ୍ଠି_not_on_optimal_path[a] || ଗଣ୍ଠି_not_on_optimal_path[b]) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }

	// Only bitangent edges can be on a shortest path; this check also cheaply
	// discards edges that violate the polygon at the vertex they come from.
	ଯଦି (!edgeBitangent(a, b) || !edgeBitangent(b, a)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }

	ଫେରନ୍ତୁ !intersectsAnyInside(_x_cm[a], _y_cm[a], _x_cm[b], _y_cm[b]);
}

ତାର୍କିକ PlannerPolygons::edgeVisible(ଅଚଳ matrix::Vector2f &a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b) ଅଚଳ
{
	ଯଦି (ଗଣ୍ଠି_not_on_optimal_path[b]) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }

	ଯଦି (!edgeBitangent(b, a)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }

	ଫେରନ୍ତୁ !intersectsAnyInside(metersToCm(a(0)), metersToCm(a(1)), _x_cm[b], _y_cm[b]);
}

ତାର୍କିକ PlannerPolygons::edgeVisible(ଅଚଳ matrix::Vector2f &a, ଅଚଳ matrix::Vector2f &b) ଅଚଳ
{
	ଫେରନ୍ତୁ !intersectsAnyInside(metersToCm(a(0)), metersToCm(a(1)), metersToCm(b(0)), metersToCm(b(1)));
}

ଦଶମିକ PlannerPolygons::edgeCost(ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b) ଅଚଳ
{
	ଫେରନ୍ତୁ edgeVisible(a, b) ? (ଗଣ୍ଠି_2(a) - ଗଣ୍ଠି_2(b)).norm() : INFINITY;
}


ତାର୍କିକ segmentsIntersectInclusive(ଅଚଳ matrix::Vector2d &a, ଅଚଳ matrix::Vector2d &b,
				ଅଚଳ matrix::Vector2d &c, ଅଚଳ matrix::Vector2d &d)
{
	// Reject separated bounds, including gaps between collinear segments.
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା axis = 0; axis < 2; ++axis) {
		ଯଦି (math::max(a(axis), b(axis)) < math::min(c(axis), d(axis))
		    || math::max(c(axis), d(axis)) < math::min(a(axis), b(axis))) {
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		}
	}

	ଅଚଳ matrix::Vector2d ab = b - a;
	ଅଚଳ matrix::Vector2d cd = d - c;
	ଅଚଳ ଦ୍ୱିଗୁଣ side_c = ab.cross(c - a);
	ଅଚଳ ଦ୍ୱିଗୁଣ side_d = ab.cross(d - a);
	ଅଚଳ ଦ୍ୱିଗୁଣ side_a = cd.cross(a - c);
	ଅଚଳ ଦ୍ୱିଗୁଣ side_b = cd.cross(b - c);

	// Each segment must cross or touch the line through the other segment.
	ଫେରନ୍ତୁ ((side_c <= 0.0 && side_d >= 0.0) || (side_c >= 0.0 && side_d <= 0.0))
	       && ((side_a <= 0.0 && side_b >= 0.0) || (side_a >= 0.0 && side_b <= 0.0));
}

ଦ୍ୱିଗୁଣ pointToSegmentDistanceSquared(ଅଚଳ matrix::Vector2d &point,
				     ଅଚଳ matrix::Vector2d &a, ଅଚଳ matrix::Vector2d &b)
{
	ଅଚଳ matrix::Vector2d direction = b - a;
	ଅଚଳ matrix::Vector2d to_point = point - a;
	ଅଚଳ ଦ୍ୱିଗୁଣ length_squared = direction.norm_squared();
	// along / length_squared locates the projection: 0 at a, 1 at b.
	ଅଚଳ ଦ୍ୱିଗୁଣ along = to_point.dot(direction);

	ଯଦି (along <= 0.0) {
		ଫେରନ୍ତୁ to_point.norm_squared();

	} ଅନ୍ୟଥା ଯଦି (along >= length_squared) {
		ଫେରନ୍ତୁ (point - b).norm_squared();
	}

	// Squared distance to the line: |to_point x direction|^2 / |direction|^2.
	// https://mathworld.wolfram.com/2-DimensionalPoint-LineDistance.html
	ଅଚଳ ଦ୍ୱିଗୁଣ cross = to_point.cross(direction);
	ଫେରନ୍ତୁ cross * cross / length_squared;
}

ତାର୍କିକ isPolygonCCW(ଅଚଳ matrix::Vector2f *vertices, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_vertices)
{
	// https://en.wikipedia.org/wiki/Shoelace_formula
	// Triangle formula
	ଦଶମିକ signed_area_2x = 0.f;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_vertices; i++) {
		ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = (i + 1) % ଅଙ୍କ_vertices;
		signed_area_2x += vertices[i](0) * vertices[j](1)
				  - vertices[j](0) * vertices[i](1);
	}

	ଫେରନ୍ତୁ signed_area_2x > 0.f;
}

} // namespace geofence_utils
