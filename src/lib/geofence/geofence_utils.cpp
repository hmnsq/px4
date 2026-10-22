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

#ಅನ್ತರ್ಭಾವ "geofence_utils.h"
#ಅನ್ತರ್ಭಾವ <lib/geo/geo.h>
#ಅನ್ತರ್ಭಾವ <lib/mathlib/mathlib.h>

ನಾಮಾಕಾಶ geofence_utils
{

ನಾಮಾಕಾಶ
{

int32_t metersToCm(ಭಿನ್ನ m) { ಪ್ರತ್ಯಾಗಮ roundf(m * CM_PER_M); }

} // namespace


ತಾರ್ಕಿಕ PlannerPolygons::allocate(ಪೂರ್ಣಾಙ್ಕ max_nodes)
{
	freeBuffers();

	ಯದಿ (max_nodes > 0) {
		_x_cm = ನೂತನ int32_t[max_nodes];
		_y_cm = ನೂತನ int32_t[max_nodes];
		ಗಂಟು_not_on_optimal_path = ನೂತನ ತಾರ್ಕಿಕ[max_nodes];

		ಯದಿ (_x_cm != nullptr && _y_cm != nullptr && ಗಂಟು_not_on_optimal_path != nullptr) {
			_max_nodes = max_nodes;

		} ಅನ್ಯಥಾ {
			freeBuffers();
		}
	}

	reset();

	ಪ್ರತ್ಯಾಗಮ _max_nodes == max_nodes;
}

ಶೂನ್ಯ PlannerPolygons::freeBuffers()
{
	delete[] _x_cm;
	delete[] _y_cm;
	delete[] ಗಂಟು_not_on_optimal_path;
	_x_cm = nullptr;
	_y_cm = nullptr;
	ಗಂಟು_not_on_optimal_path = nullptr;
	_max_nodes = 0;
}

ಶೂನ್ಯ PlannerPolygons::set_ಗಂಟು(ಪೂರ್ಣಾಙ್ಕ idx, ಧ್ರುವ matrix::Vector2f &p)
{
	ಯದಿ (_max_nodes < 1) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	idx = math::constrain(idx, 0, _max_nodes - 1);

	_x_cm[idx] = metersToCm(p(0));
	_y_cm[idx] = metersToCm(p(1));
}

PlannerPolygons::AddResult PlannerPolygons::addPolygon(ಧ್ರುವ matrix::Vector2f *vertices_in, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_vertices,
		ತಾರ್ಕಿಕ is_inclusion_zone, ಭಿನ್ನ margin)
{
	ಯದಿ (ಅಂಕೆ_polygons >= kMaxPolygons || ಅಂಕೆ_nodes_2 + ಅಂಕೆ_vertices > _max_nodes) {
		ಪ್ರತ್ಯಾಗಮ AddResult::BudgetExceeded;
	}

	ಯದಿ (margin < -FLT_EPSILON || ಅಂಕೆ_vertices < 3) { ಪ್ರತ್ಯಾಗಮ AddResult::Degenerate; }

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_vertices; ++i) {
		// Reject self-intersecting (non-simple) polygons.
		ಧ್ರುವ matrix::Vector2f &a = vertices_in[i];
		ಧ್ರುವ matrix::Vector2f &b = vertices_in[(i + 1) % ಅಂಕೆ_vertices];

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = i + 2; j < ಅಂಕೆ_vertices; ++j) {
			ಯದಿ (i == 0 && j == ಅಂಕೆ_vertices - 1) {
				ಅನುವರ್ತ; // (last edge, first edge) are adjacent
			}

			ಧ್ರುವ matrix::Vector2f &c = vertices_in[j];
			ಧ್ರುವ matrix::Vector2f &d = vertices_in[(j + 1) % ಅಂಕೆ_vertices];

			ಯದಿ (segmentsIntersect(
				    metersToCm(a(0)), metersToCm(a(1)), metersToCm(b(0)), metersToCm(b(1)),
				    metersToCm(c(0)), metersToCm(c(1)), metersToCm(d(0)), metersToCm(d(1))) != SegSegResult::Disjoint) {
				ಪ್ರತ್ಯಾಗಮ AddResult::Degenerate;
			}
		}

		// Reject vertices outside the usable fixed-point range
		ಯದಿ (!inFixedPointRange(vertices_in[i](0)) || !inFixedPointRange(vertices_in[i](1))) {
			ಪ್ರತ್ಯಾಗಮ AddResult::OutOfRange;
		}
	}

	// Canonical orientation: walking vertices in stored order, INSIDE is on
	// the left of each edge. CCW for exclusion (bounded interior is forbidden),
	// CW for inclusion (unbounded exterior is forbidden). Reverse iff input
	// doesn't already match.
	ಧ್ರುವ ತಾರ್ಕಿಕ reverse = (isPolygonCCW(vertices_in, ಅಂಕೆ_vertices) == is_inclusion_zone);

	// Index into vertices_in in stored order (accounts for possible reversal).
	auto vertex = [&](ಪೂರ್ಣಾಙ್ಕ i) -> ಧ್ರುವ matrix::Vector2f & {
		ಪ್ರತ್ಯಾಗಮ vertices_in[reverse ? (ಅಂಕೆ_vertices - 1 - i) : i];
	};

	PolygonInfo &poly = _polygons[ಅಂಕೆ_polygons];
	poly.start_index  = ಅಂಕೆ_nodes_2;
	poly.is_inclusion = is_inclusion_zone;

	ಪೂರ್ಣಾಙ್ಕ out_idx = 0;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_vertices; i++) {
		ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ prev = (i + ಅಂಕೆ_vertices - 1) % ಅಂಕೆ_vertices;
		ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ next = (i + 1) % ಅಂಕೆ_vertices;

		ಧ್ರುವ matrix::Vector2f edge_in  = vertex(i) - vertex(prev);
		ಧ್ರುವ matrix::Vector2f edge_out = vertex(next) - vertex(i);

		ಧ್ರುವ ಭಿನ್ನ edge_in_norm = edge_in.norm();
		ಧ್ರುವ ಭಿನ್ನ edge_out_norm = edge_out.norm();

		ಯದಿ (edge_in_norm < FLT_EPSILON || edge_out_norm < FLT_EPSILON) {
			ಪ್ರತ್ಯಾಗಮ AddResult::Degenerate;
		}

		ಧ್ರುವ matrix::Vector2f edge_in_normalized = edge_in / edge_in_norm;
		ಧ್ರುವ matrix::Vector2f edge_out_normalized = edge_out / edge_out_norm;

		// Inward normals of the two adjacent edges.
		// Canonical stored orientation: left side = forbidden region, right side = allowed region
		ಧ್ರುವ matrix::Vector2f n_in  = matrix::Vector2f{-edge_in_normalized(1), edge_in_normalized(0)};
		ಧ್ರುವ matrix::Vector2f n_out = matrix::Vector2f{-edge_out_normalized(1), edge_out_normalized(0)};

		matrix::Vector2f bisector = n_in + n_out;
		ಧ್ರುವ ಭಿನ್ನ bisector_len  = bisector.length();

		ಯದಿ (bisector_len < FLT_EPSILON) {
			ಪ್ರತ್ಯಾಗಮ AddResult::Degenerate; // antiparallel edges - polygon doubles back
		}

		ಧ್ರುವ matrix::Vector2f normalized_bisector = bisector / bisector_len;

		// The following bit ensures that:
		//  - The original edges are moved away from the forbidden interior by `margin`
		//  - Sharp convex corners (<60deg) are "cut off" with a new edge that is exactly
		//    `margin` away from the original vertex. For this the vertex is replaced by
		//    two new ones.

		// Only split convex corners - others are ignored, see _node_not_on_optimal_path
		ಧ್ರುವ ತಾರ್ಕಿಕ corner_convex = edge_in_normalized(0) * edge_out_normalized(1)
					   - edge_in_normalized(1) * edge_out_normalized(0) > 0.f;

		// Scaling needed wrt the normalised bisector to achieve parallel edges
		ಧ್ರುವ ಭಿನ್ನ offset_scale = 2.f / bisector_len;

		// bisector_len = 2*sin(interior angle/2); split when interior angle < 60 deg <=> bisector_len < 1.
		ಧ್ರುವ ತಾರ್ಕಿಕ angle_sharp = bisector_len < 1;

		// If margin zero, do not split - would output the same vertex twice.
		ಧ್ರುವ ತಾರ್ಕಿಕ margin_nonzero = margin > FLT_EPSILON;

		// If we do not have enough space, do not split the vertex.
		// This never happens if the planner-internal node buffer is 2x the original buffer.
		ಧ್ರುವ ತಾರ್ಕಿಕ space_for_split_vertices = ಅಂಕೆ_nodes_2 + out_idx + 1 < _max_nodes;

		ಯದಿ (corner_convex && angle_sharp && margin_nonzero && space_for_split_vertices) {

			// Sharp corner: cut it off along the line
			//  - perpendicular to normalized_bisector
			//  - going through base := vertex(i) - normalized_bisector * margin

			ಧ್ರುವ matrix::Vector2f base = vertex(i) - normalized_bisector * margin;
			ಧ್ರುವ matrix::Vector2f perp = {-normalized_bisector(1), normalized_bisector(0)};

			// Construct s such that:
			//  base + perp*s is on the offset incoming edge
			//  base - perp*s is on the offset outgoing edge
			ಧ್ರುವ ಭಿನ್ನ e_out_dot_b = edge_out_normalized.dot(normalized_bisector);
			ಧ್ರುವ ಭಿನ್ನ s = margin * (1.f - n_out.dot(normalized_bisector)) / e_out_dot_b;

			set_ಗಂಟು(poly.start_index + out_idx++, base + perp * s);
			set_ಗಂಟು(poly.start_index + out_idx++, base - perp * s);

		} ಅನ್ಯಥಾ {
			// Regular corner or no space for split: single bisector vertex displaced by margin / sin(interior angle/2).
			set_ಗಂಟು(poly.start_index + out_idx++, vertex(i) - normalized_bisector * (margin * offset_scale));
		}
	}

	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_out = out_idx;
	poly.ಅಂಕೆ_vertices = ಅಂಕೆ_out;

	computeBoundingBox(poly.start_index, ಅಂಕೆ_out, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// In canonical orientation, the node is not possibly on the shortest
	// path if the vertex curves right or straight (forbidden region >= 180
	// deg)
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_out; i++) {
		ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ prev = poly.start_index + (i + ಅಂಕೆ_out - 1) % ಅಂಕೆ_out;
		ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ curr = poly.start_index + i;
		ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ next = poly.start_index + (i + 1) % ಅಂಕೆ_out;
		ಧ್ರುವ ತಾರ್ಕಿಕ curves_ಬಲ_or_straight = orient2d(_x_cm[prev], _y_cm[prev],
						      _x_cm[curr], _y_cm[curr],
						      _x_cm[next], _y_cm[next]) <= 0;
		ಗಂಟು_not_on_optimal_path[curr] = curves_ಬಲ_or_straight;
	}

	ಅಂಕೆ_nodes_2 += ಅಂಕೆ_out;
	++ಅಂಕೆ_polygons;
	ಪ್ರತ್ಯಾಗಮ AddResult::Success;
}

PlannerPolygons::AddResult PlannerPolygons::addApproxCircle(ಧ್ರುವ matrix::Vector2f &center, ಧ್ರುವ ಭಿನ್ನ circle_radius,
		ಭಿನ್ನ margin, ಧ್ರುವ ತಾರ್ಕಿಕ is_inclusion_zone)
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

	ಯದಿ (ಅಂಕೆ_polygons >= kMaxPolygons || ಅಂಕೆ_nodes_2 + kCircleApproxVertices > _max_nodes) {
		ಪ್ರತ್ಯಾಗಮ AddResult::BudgetExceeded;
	}

	ಯದಿ (circle_radius <= 0.1f) { ಪ್ರತ್ಯಾಗಮ AddResult::Degenerate; }

	// ensures that the k-gon is, w.r.t the real circle:
	//  - over-approximation of circle grown by margin, if exclusion
	//  - under-approximation of circle shrunk by margin, if inclusion
	ಧ್ರುವ ಭಿನ್ನ k_gon_radius = is_inclusion_zone
				   ? circle_radius - margin
				   : circle_radius / cosf(M_PI_F / kCircleApproxVertices) + margin;

	ಯದಿ (k_gon_radius <= FLT_EPSILON) { // Negative radius - circle empty
		ಪ್ರತ್ಯಾಗಮ AddResult::Degenerate;
	}

	ಯದಿ (!inFixedPointRange(fabsf(center(0)) + k_gon_radius)
	    || !inFixedPointRange(fabsf(center(1)) + k_gon_radius)) {
		ಪ್ರತ್ಯಾಗಮ AddResult::OutOfRange;
	}

	// Canonical orientation: CCW for exclusion, CW for inclusion
	ಧ್ರುವ ಭಿನ್ನ delta_angle = (is_inclusion_zone ? -1.f : 1.f) * 2.f * M_PI_F / kCircleApproxVertices;

	PolygonInfo &poly = _polygons[ಅಂಕೆ_polygons];
	poly.start_index = ಅಂಕೆ_nodes_2;
	poly.ಅಂಕೆ_vertices = kCircleApproxVertices;
	poly.is_inclusion = is_inclusion_zone;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < kCircleApproxVertices; i++) {
		ಧ್ರುವ ಭಿನ್ನ angle = (i + 0.5f) * delta_angle;
		ಧ್ರುವ matrix::Vector2f p = center + matrix::Vector2f{
			k_gon_radius * cosf(angle), k_gon_radius * sinf(angle)
		};
		set_ಗಂಟು(poly.start_index + i, p);
	}

	computeBoundingBox(poly.start_index, kCircleApproxVertices, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// For regular k-gons, all corners are either convex or reflex based on
	// is_inclusion, so the reduced visibility graph criterion is simplified
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < kCircleApproxVertices; i++) {
		ಗಂಟು_not_on_optimal_path[poly.start_index + i] = is_inclusion_zone;
	}

	ಅಂಕೆ_nodes_2 += kCircleApproxVertices;
	++ಅಂಕೆ_polygons;
	ಪ್ರತ್ಯಾಗಮ AddResult::Success;
}

ಶೂನ್ಯ PlannerPolygons::computeBoundingBox(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ start_index, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_vertices,
		int32_t &min_x, int32_t &max_x, int32_t &min_y, int32_t &max_y)
{

	// Store the bounding box, so we can do a very cheap check against the
	// bounding box, and only do the full line-polygon intersection check if
	// the line intersects the bounding box.

	min_x = max_x = _x_cm[start_index];
	min_y = max_y = _y_cm[start_index];

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < ಅಂಕೆ_vertices; i++) {
		ಧ್ರುವ int32_t x = _x_cm[start_index + i];
		ಧ್ರುವ int32_t y = _y_cm[start_index + i];

		ಯದಿ (x < min_x) { min_x = x; }

		ಯದಿ (x > max_x) { max_x = x; }

		ಯದಿ (y < min_y) { min_y = y; }

		ಯದಿ (y > max_y) { max_y = y; }
	}
}

ತಾರ್ಕಿಕ PlannerPolygons::pointInsideInteriorCone(ಧ್ರುವ PolygonInfo &poly,
		int32_t px, int32_t py, ಪೂರ್ಣಾಙ್ಕ v) ಧ್ರುವ
{
	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ prev = poly.start_index + ((v + poly.ಅಂಕೆ_vertices - 1) % poly.ಅಂಕೆ_vertices);
	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ curr = poly.start_index + v;
	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ next = poly.start_index + ((v + 1) % poly.ಅಂಕೆ_vertices);

	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ p_ಎಡ_of_incoming  = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], px, py);
	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ p_ಎಡ_of_outgoing  = orient2d(_x_cm[curr], _y_cm[curr], _x_cm[next], _y_cm[next], px, py);
	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ is_convex           = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], _x_cm[next], _y_cm[next]);

	ಪ್ರತ್ಯಾಗಮ is_convex > 0  // If convex==0 (exact 180 deg "corner") the two cases are equivalent
	       ? (p_ಎಡ_of_incoming > 0 && p_ಎಡ_of_outgoing > 0)   // Convex: interior = intersection of two half-planes
	       : (p_ಎಡ_of_incoming > 0 || p_ಎಡ_of_outgoing > 0);  // Reflex: interior = union of two half-planes
}

ತಾರ್ಕಿಕ PlannerPolygons::intersectsInsideOf(ಧ್ರುವ PolygonInfo &poly,
		int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ಧ್ರುವ
{
	// Single pass over polygon edges, doing two jobs:
	//  (1) classify the test segment vs each polygon edge. early-return on:
	//       - strict crossings (line segment - polygon edge)
	//       - interior crossing at vertex
	//  (2) accumulate midpoint winding contribution -- if (1) is inconclusive,
	//      this decides if the line is inside or outside

	ಪೂರ್ಣಾಙ್ಕ wn = 0;
	ತಾರ್ಕಿಕ mid_on_boundary = ಅಸತ್ಯ;

	// Rather than calculating the midpoint (mid = (s + e)/2) explicitly,
	// which will lead to rounding error for uneven numbers, we instead
	// scale all other inputs by two, reducing the range by a factor of two
	// (2^31 cm = 21400 km -> 2^30 cm = 10700 km)
	ಧ್ರುವ int32_t twice_mid_x = s_x + e_x;
	ಧ್ರುವ int32_t twice_mid_y = s_y + e_y;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < poly.ಅಂಕೆ_vertices; i++) {

		ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ prev_idx = poly.start_index + (i + poly.ಅಂಕೆ_vertices - 1) % poly.ಅಂಕೆ_vertices;
		ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ curr_idx = poly.start_index + i;

		ಧ್ರುವ int32_t ax = _x_cm[prev_idx], ay = _y_cm[prev_idx];
		ಧ್ರುವ int32_t bx = _x_cm[curr_idx], by = _y_cm[curr_idx];

		// (1) test segment vs polygon edge (a, b)
		ವಿಕಲ್ಪ (segmentsIntersect(ax, ay, bx, by, s_x, s_y, e_x, e_y)) {
		ಪಕ್ಷ SegSegResult::Cross:
			ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;

		// skip AInsideCD - a from this segment is b from the adjacent segment
		ಪಕ್ಷ SegSegResult::BInsideCD:

			// Polygon vertex b is exactly on the interior of the test segment.
			// Return intersecting if it pokes into the polygon interior through vertex b.
			ಯದಿ (pointInsideInteriorCone(poly, s_x, s_y, i) || pointInsideInteriorCone(poly, e_x, e_y, i)) {
				ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
			}

			ಭಙ್ಗ;

		ಸ್ವಭಾವ:
			ಭಙ್ಗ;
		}

		// (2) midpoint winding contribution
		// Ref: Dan Sunday, Inclusion of a Point in a Polygon
		// https://web.archive.org/web/20130126163405/http://geomalgorithms.com/a03-_inclusion.html

		ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ side = orient2d(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);

		ಯದಿ (side == 0) {

			ಧ್ರುವ ತಾರ್ಕಿಕ mid_on_open_ab = collinearBetween(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);
			ಧ್ರುವ ತಾರ್ಕಿಕ mid_is_b = 2 * bx == twice_mid_x && 2 * by == twice_mid_y;

			// Skip point a - a from this segment is b from the adjacent segment
			ಯದಿ (mid_on_open_ab || mid_is_b) {
				mid_on_boundary = ಸತ್ಯ;
			}

		} ಅನ್ಯಥಾ ಯದಿ (2 * ay <= twice_mid_y) {
			ಯದಿ (2 * by > twice_mid_y && side > 0) { wn++; }

		} ಅನ್ಯಥಾ ಯದಿ (2 * by <= twice_mid_y && side < 0) { wn--; }
	}


	// Midpoint on boundary, segment runs along it (strict crossing detected earlier).
	// Non-intersecting in either zone.
	ಯದಿ (mid_on_boundary) {
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	// Exclusion: violation when midpoint is inside (bounded). Inclusion: when outside (unbounded).
	// This is the only place we depart from the inside = left = forbidden canonical orientation,
	// necessary because of the point membership check.
	ಧ್ರುವ ತಾರ್ಕಿಕ midpoint_in_bounded_inside = (wn != 0);
	ಪ್ರತ್ಯಾಗಮ midpoint_in_bounded_inside != poly.is_inclusion;
}

ತಾರ್ಕಿಕ PlannerPolygons::intersectsAnyInside(int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ಧ್ರುವ
{
	ಧ್ರುವ int32_t seg_min_x = s_x < e_x ? s_x : e_x;
	ಧ್ರುವ int32_t seg_max_x = s_x > e_x ? s_x : e_x;
	ಧ್ರುವ int32_t seg_min_y = s_y < e_y ? s_y : e_y;
	ಧ್ರುವ int32_t seg_max_y = s_y > e_y ? s_y : e_y;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ p = 0; p < ಅಂಕೆ_polygons; p++) {
		ಧ್ರುವ PolygonInfo &poly = _polygons[p];

		ಯದಿ (seg_max_x < poly.min_x || seg_min_x > poly.max_x ||
		    seg_max_y < poly.min_y || seg_min_y > poly.max_y) {
			// Segment is entirely outside this polygon.
			// For exclusion zones: outside = safe, skip.
			// For inclusion zones: outside = forbidden region, immediate violation.
			ಯದಿ (poly.is_inclusion) { ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ; }

			ಅನುವರ್ತ;
		}

		ಯದಿ (intersectsInsideOf(poly, s_x, s_y, e_x, e_y)) {
			ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
		}
	}

	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ತಾರ್ಕಿಕ PlannerPolygons::edgeBitangent(ಪೂರ್ಣಾಙ್ಕ a, int32_t bx, int32_t by) ಧ್ರುವ
{
	// Find the polygon owning vertex index a. Indices not belonging to any
	// polygon (in particular the destination at index 0) are trivially bitangent.

	ಯದಿ (a == destIndex()) { ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ; }

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ p = 0; p < ಅಂಕೆ_polygons; p++) {
		ಧ್ರುವ PolygonInfo &poly = _polygons[p];

		ಯದಿ (a >= poly.start_index && a < poly.start_index + poly.ಅಂಕೆ_vertices) {
			// Define point c = a + (a-b) = 2a - b, so that ac = ab mirrored about a
			// Then: Edge is bitangent at a <=> neither ab nor ac pokes into the interior
			int32_t cx = 2 * _x_cm[a] - bx;
			int32_t cy = 2 * _y_cm[a] - by;
			ಪ್ರತ್ಯಾಗಮ !pointInsideInteriorCone(poly, bx, by, a - poly.start_index)
			       && !pointInsideInteriorCone(poly, cx, cy, a - poly.start_index);
		}
	}

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ PlannerPolygons::edgeBitangent(ಪೂರ್ಣಾಙ್ಕ a, ಪೂರ್ಣಾಙ್ಕ b) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ edgeBitangent(a, _x_cm[b], _y_cm[b]);
}

ತಾರ್ಕಿಕ PlannerPolygons::edgeBitangent(ಪೂರ್ಣಾಙ್ಕ a, ಧ್ರುವ matrix::Vector2f &b) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ edgeBitangent(a, metersToCm(b(0)), metersToCm(b(1)));
}

ತಾರ್ಕಿಕ PlannerPolygons::setDestination(ಧ್ರುವ matrix::Vector2f &p)
{
	set_ಗಂಟು(destIndex(), p);

	ಧ್ರುವ int32_t x_cm = metersToCm(p(0));
	ಧ್ರುವ int32_t y_cm = metersToCm(p(1));

	ಯದಿ (intersectsAnyInside(x_cm, y_cm, x_cm, y_cm)) {
		// If the destination breaches a geofence, still use it but
		// signal to the user that it is invalid.
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

matrix::Vector2f PlannerPolygons::getDestination() ಧ್ರುವ
{
	ಯದಿ (_max_nodes < 1) {
		ಪ್ರತ್ಯಾಗಮ matrix::Vector2f{NAN, NAN};
	}

	ಪ್ರತ್ಯಾಗಮ ಗಂಟು_2(destIndex());
}

ತಾರ್ಕಿಕ PlannerPolygons::edgeVisible(ಪೂರ್ಣಾಙ್ಕ a, ಪೂರ್ಣಾಙ್ಕ b) ಧ್ರುವ
{
	ಯದಿ (a == b) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }

	ಯದಿ (ಗಂಟು_not_on_optimal_path[a] || ಗಂಟು_not_on_optimal_path[b]) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }

	// Only bitangent edges can be on a shortest path; this check also cheaply
	// discards edges that violate the polygon at the vertex they come from.
	ಯದಿ (!edgeBitangent(a, b) || !edgeBitangent(b, a)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }

	ಪ್ರತ್ಯಾಗಮ !intersectsAnyInside(_x_cm[a], _y_cm[a], _x_cm[b], _y_cm[b]);
}

ತಾರ್ಕಿಕ PlannerPolygons::edgeVisible(ಧ್ರುವ matrix::Vector2f &a, ಪೂರ್ಣಾಙ್ಕ b) ಧ್ರುವ
{
	ಯದಿ (ಗಂಟು_not_on_optimal_path[b]) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }

	ಯದಿ (!edgeBitangent(b, a)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }

	ಪ್ರತ್ಯಾಗಮ !intersectsAnyInside(metersToCm(a(0)), metersToCm(a(1)), _x_cm[b], _y_cm[b]);
}

ತಾರ್ಕಿಕ PlannerPolygons::edgeVisible(ಧ್ರುವ matrix::Vector2f &a, ಧ್ರುವ matrix::Vector2f &b) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ !intersectsAnyInside(metersToCm(a(0)), metersToCm(a(1)), metersToCm(b(0)), metersToCm(b(1)));
}

ಭಿನ್ನ PlannerPolygons::edgeCost(ಪೂರ್ಣಾಙ್ಕ a, ಪೂರ್ಣಾಙ್ಕ b) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ edgeVisible(a, b) ? (ಗಂಟು_2(a) - ಗಂಟು_2(b)).norm() : INFINITY;
}


ತಾರ್ಕಿಕ segmentsIntersectInclusive(ಧ್ರುವ matrix::Vector2d &a, ಧ್ರುವ matrix::Vector2d &b,
				ಧ್ರುವ matrix::Vector2d &c, ಧ್ರುವ matrix::Vector2d &d)
{
	// Reject separated bounds, including gaps between collinear segments.
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ axis = 0; axis < 2; ++axis) {
		ಯದಿ (math::max(a(axis), b(axis)) < math::min(c(axis), d(axis))
		    || math::max(c(axis), d(axis)) < math::min(a(axis), b(axis))) {
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		}
	}

	ಧ್ರುವ matrix::Vector2d ab = b - a;
	ಧ್ರುವ matrix::Vector2d cd = d - c;
	ಧ್ರುವ ದ್ವಿಗುಣ side_c = ab.cross(c - a);
	ಧ್ರುವ ದ್ವಿಗುಣ side_d = ab.cross(d - a);
	ಧ್ರುವ ದ್ವಿಗುಣ side_a = cd.cross(a - c);
	ಧ್ರುವ ದ್ವಿಗುಣ side_b = cd.cross(b - c);

	// Each segment must cross or touch the line through the other segment.
	ಪ್ರತ್ಯಾಗಮ ((side_c <= 0.0 && side_d >= 0.0) || (side_c >= 0.0 && side_d <= 0.0))
	       && ((side_a <= 0.0 && side_b >= 0.0) || (side_a >= 0.0 && side_b <= 0.0));
}

ದ್ವಿಗುಣ pointToSegmentDistanceSquared(ಧ್ರುವ matrix::Vector2d &point,
				     ಧ್ರುವ matrix::Vector2d &a, ಧ್ರುವ matrix::Vector2d &b)
{
	ಧ್ರುವ matrix::Vector2d direction = b - a;
	ಧ್ರುವ matrix::Vector2d to_point = point - a;
	ಧ್ರುವ ದ್ವಿಗುಣ length_squared = direction.norm_squared();
	// along / length_squared locates the projection: 0 at a, 1 at b.
	ಧ್ರುವ ದ್ವಿಗುಣ along = to_point.dot(direction);

	ಯದಿ (along <= 0.0) {
		ಪ್ರತ್ಯಾಗಮ to_point.norm_squared();

	} ಅನ್ಯಥಾ ಯದಿ (along >= length_squared) {
		ಪ್ರತ್ಯಾಗಮ (point - b).norm_squared();
	}

	// Squared distance to the line: |to_point x direction|^2 / |direction|^2.
	// https://mathworld.wolfram.com/2-DimensionalPoint-LineDistance.html
	ಧ್ರುವ ದ್ವಿಗುಣ cross = to_point.cross(direction);
	ಪ್ರತ್ಯಾಗಮ cross * cross / length_squared;
}

ತಾರ್ಕಿಕ isPolygonCCW(ಧ್ರುವ matrix::Vector2f *vertices, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_vertices)
{
	// https://en.wikipedia.org/wiki/Shoelace_formula
	// Triangle formula
	ಭಿನ್ನ signed_area_2x = 0.f;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_vertices; i++) {
		ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ j = (i + 1) % ಅಂಕೆ_vertices;
		signed_area_2x += vertices[i](0) * vertices[j](1)
				  - vertices[j](0) * vertices[i](1);
	}

	ಪ್ರತ್ಯಾಗಮ signed_area_2x > 0.f;
}

} // namespace geofence_utils
