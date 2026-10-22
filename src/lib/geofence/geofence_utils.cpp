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

#അന്തര്ഭാവ "geofence_utils.h"
#അന്തര്ഭാവ <lib/geo/geo.h>
#അന്തര്ഭാവ <lib/mathlib/mathlib.h>

നാമാകാശ geofence_utils
{

നാമാകാശ
{

int32_t metersToCm(ദശാംശം m) { തിരികെ roundf(m * CM_PER_M); }

} // namespace


തർക്കം PlannerPolygons::allocate(പൂർണ്ണസംഖ്യ max_nodes)
{
	freeBuffers();

	എങ്കിൽ (max_nodes > 0) {
		_x_cm = പുതിയ int32_t[max_nodes];
		_y_cm = പുതിയ int32_t[max_nodes];
		കണ്ണി_not_on_optimal_path = പുതിയ തർക്കം[max_nodes];

		എങ്കിൽ (_x_cm != nullptr && _y_cm != nullptr && കണ്ണി_not_on_optimal_path != nullptr) {
			_max_nodes = max_nodes;

		} അല്ലെങ്കിൽ {
			freeBuffers();
		}
	}

	reset();

	തിരികെ _max_nodes == max_nodes;
}

ശൂന്യം PlannerPolygons::freeBuffers()
{
	delete[] _x_cm;
	delete[] _y_cm;
	delete[] കണ്ണി_not_on_optimal_path;
	_x_cm = nullptr;
	_y_cm = nullptr;
	കണ്ണി_not_on_optimal_path = nullptr;
	_max_nodes = 0;
}

ശൂന്യം PlannerPolygons::set_കണ്ണി(പൂർണ്ണസംഖ്യ idx, സ്ഥിരം matrix::Vector2f &p)
{
	എങ്കിൽ (_max_nodes < 1) {
		തിരികെ;
	}

	idx = math::constrain(idx, 0, _max_nodes - 1);

	_x_cm[idx] = metersToCm(p(0));
	_y_cm[idx] = metersToCm(p(1));
}

PlannerPolygons::AddResult PlannerPolygons::addPolygon(സ്ഥിരം matrix::Vector2f *vertices_in, പൂർണ്ണസംഖ്യ അക്കം_vertices,
		തർക്കം is_inclusion_zone, ദശാംശം margin)
{
	എങ്കിൽ (അക്കം_polygons >= kMaxPolygons || അക്കം_nodes_2 + അക്കം_vertices > _max_nodes) {
		തിരികെ AddResult::BudgetExceeded;
	}

	എങ്കിൽ (margin < -FLT_EPSILON || അക്കം_vertices < 3) { തിരികെ AddResult::Degenerate; }

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_vertices; ++i) {
		// Reject self-intersecting (non-simple) polygons.
		സ്ഥിരം matrix::Vector2f &a = vertices_in[i];
		സ്ഥിരം matrix::Vector2f &b = vertices_in[(i + 1) % അക്കം_vertices];

		വേണ്ടി (പൂർണ്ണസംഖ്യ j = i + 2; j < അക്കം_vertices; ++j) {
			എങ്കിൽ (i == 0 && j == അക്കം_vertices - 1) {
				തുടരുക; // (last edge, first edge) are adjacent
			}

			സ്ഥിരം matrix::Vector2f &c = vertices_in[j];
			സ്ഥിരം matrix::Vector2f &d = vertices_in[(j + 1) % അക്കം_vertices];

			എങ്കിൽ (segmentsIntersect(
				    metersToCm(a(0)), metersToCm(a(1)), metersToCm(b(0)), metersToCm(b(1)),
				    metersToCm(c(0)), metersToCm(c(1)), metersToCm(d(0)), metersToCm(d(1))) != SegSegResult::Disjoint) {
				തിരികെ AddResult::Degenerate;
			}
		}

		// Reject vertices outside the usable fixed-point range
		എങ്കിൽ (!inFixedPointRange(vertices_in[i](0)) || !inFixedPointRange(vertices_in[i](1))) {
			തിരികെ AddResult::OutOfRange;
		}
	}

	// Canonical orientation: walking vertices in stored order, INSIDE is on
	// the left of each edge. CCW for exclusion (bounded interior is forbidden),
	// CW for inclusion (unbounded exterior is forbidden). Reverse iff input
	// doesn't already match.
	സ്ഥിരം തർക്കം reverse = (isPolygonCCW(vertices_in, അക്കം_vertices) == is_inclusion_zone);

	// Index into vertices_in in stored order (accounts for possible reversal).
	auto vertex = [&](പൂർണ്ണസംഖ്യ i) -> സ്ഥിരം matrix::Vector2f & {
		തിരികെ vertices_in[reverse ? (അക്കം_vertices - 1 - i) : i];
	};

	PolygonInfo &poly = _polygons[അക്കം_polygons];
	poly.start_index  = അക്കം_nodes_2;
	poly.is_inclusion = is_inclusion_zone;

	പൂർണ്ണസംഖ്യ out_idx = 0;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_vertices; i++) {
		സ്ഥിരം പൂർണ്ണസംഖ്യ prev = (i + അക്കം_vertices - 1) % അക്കം_vertices;
		സ്ഥിരം പൂർണ്ണസംഖ്യ next = (i + 1) % അക്കം_vertices;

		സ്ഥിരം matrix::Vector2f edge_in  = vertex(i) - vertex(prev);
		സ്ഥിരം matrix::Vector2f edge_out = vertex(next) - vertex(i);

		സ്ഥിരം ദശാംശം edge_in_norm = edge_in.norm();
		സ്ഥിരം ദശാംശം edge_out_norm = edge_out.norm();

		എങ്കിൽ (edge_in_norm < FLT_EPSILON || edge_out_norm < FLT_EPSILON) {
			തിരികെ AddResult::Degenerate;
		}

		സ്ഥിരം matrix::Vector2f edge_in_normalized = edge_in / edge_in_norm;
		സ്ഥിരം matrix::Vector2f edge_out_normalized = edge_out / edge_out_norm;

		// Inward normals of the two adjacent edges.
		// Canonical stored orientation: left side = forbidden region, right side = allowed region
		സ്ഥിരം matrix::Vector2f n_in  = matrix::Vector2f{-edge_in_normalized(1), edge_in_normalized(0)};
		സ്ഥിരം matrix::Vector2f n_out = matrix::Vector2f{-edge_out_normalized(1), edge_out_normalized(0)};

		matrix::Vector2f bisector = n_in + n_out;
		സ്ഥിരം ദശാംശം bisector_len  = bisector.length();

		എങ്കിൽ (bisector_len < FLT_EPSILON) {
			തിരികെ AddResult::Degenerate; // antiparallel edges - polygon doubles back
		}

		സ്ഥിരം matrix::Vector2f normalized_bisector = bisector / bisector_len;

		// The following bit ensures that:
		//  - The original edges are moved away from the forbidden interior by `margin`
		//  - Sharp convex corners (<60deg) are "cut off" with a new edge that is exactly
		//    `margin` away from the original vertex. For this the vertex is replaced by
		//    two new ones.

		// Only split convex corners - others are ignored, see _node_not_on_optimal_path
		സ്ഥിരം തർക്കം corner_convex = edge_in_normalized(0) * edge_out_normalized(1)
					   - edge_in_normalized(1) * edge_out_normalized(0) > 0.f;

		// Scaling needed wrt the normalised bisector to achieve parallel edges
		സ്ഥിരം ദശാംശം offset_scale = 2.f / bisector_len;

		// bisector_len = 2*sin(interior angle/2); split when interior angle < 60 deg <=> bisector_len < 1.
		സ്ഥിരം തർക്കം angle_sharp = bisector_len < 1;

		// If margin zero, do not split - would output the same vertex twice.
		സ്ഥിരം തർക്കം margin_nonzero = margin > FLT_EPSILON;

		// If we do not have enough space, do not split the vertex.
		// This never happens if the planner-internal node buffer is 2x the original buffer.
		സ്ഥിരം തർക്കം space_for_split_vertices = അക്കം_nodes_2 + out_idx + 1 < _max_nodes;

		എങ്കിൽ (corner_convex && angle_sharp && margin_nonzero && space_for_split_vertices) {

			// Sharp corner: cut it off along the line
			//  - perpendicular to normalized_bisector
			//  - going through base := vertex(i) - normalized_bisector * margin

			സ്ഥിരം matrix::Vector2f base = vertex(i) - normalized_bisector * margin;
			സ്ഥിരം matrix::Vector2f perp = {-normalized_bisector(1), normalized_bisector(0)};

			// Construct s such that:
			//  base + perp*s is on the offset incoming edge
			//  base - perp*s is on the offset outgoing edge
			സ്ഥിരം ദശാംശം e_out_dot_b = edge_out_normalized.dot(normalized_bisector);
			സ്ഥിരം ദശാംശം s = margin * (1.f - n_out.dot(normalized_bisector)) / e_out_dot_b;

			set_കണ്ണി(poly.start_index + out_idx++, base + perp * s);
			set_കണ്ണി(poly.start_index + out_idx++, base - perp * s);

		} അല്ലെങ്കിൽ {
			// Regular corner or no space for split: single bisector vertex displaced by margin / sin(interior angle/2).
			set_കണ്ണി(poly.start_index + out_idx++, vertex(i) - normalized_bisector * (margin * offset_scale));
		}
	}

	സ്ഥിരം പൂർണ്ണസംഖ്യ അക്കം_out = out_idx;
	poly.അക്കം_vertices = അക്കം_out;

	computeBoundingBox(poly.start_index, അക്കം_out, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// In canonical orientation, the node is not possibly on the shortest
	// path if the vertex curves right or straight (forbidden region >= 180
	// deg)
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_out; i++) {
		സ്ഥിരം പൂർണ്ണസംഖ്യ prev = poly.start_index + (i + അക്കം_out - 1) % അക്കം_out;
		സ്ഥിരം പൂർണ്ണസംഖ്യ curr = poly.start_index + i;
		സ്ഥിരം പൂർണ്ണസംഖ്യ next = poly.start_index + (i + 1) % അക്കം_out;
		സ്ഥിരം തർക്കം curves_വലത്_or_straight = orient2d(_x_cm[prev], _y_cm[prev],
						      _x_cm[curr], _y_cm[curr],
						      _x_cm[next], _y_cm[next]) <= 0;
		കണ്ണി_not_on_optimal_path[curr] = curves_വലത്_or_straight;
	}

	അക്കം_nodes_2 += അക്കം_out;
	++അക്കം_polygons;
	തിരികെ AddResult::Success;
}

PlannerPolygons::AddResult PlannerPolygons::addApproxCircle(സ്ഥിരം matrix::Vector2f &center, സ്ഥിരം ദശാംശം circle_radius,
		ദശാംശം margin, സ്ഥിരം തർക്കം is_inclusion_zone)
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

	എങ്കിൽ (അക്കം_polygons >= kMaxPolygons || അക്കം_nodes_2 + kCircleApproxVertices > _max_nodes) {
		തിരികെ AddResult::BudgetExceeded;
	}

	എങ്കിൽ (circle_radius <= 0.1f) { തിരികെ AddResult::Degenerate; }

	// ensures that the k-gon is, w.r.t the real circle:
	//  - over-approximation of circle grown by margin, if exclusion
	//  - under-approximation of circle shrunk by margin, if inclusion
	സ്ഥിരം ദശാംശം k_gon_radius = is_inclusion_zone
				   ? circle_radius - margin
				   : circle_radius / cosf(M_PI_F / kCircleApproxVertices) + margin;

	എങ്കിൽ (k_gon_radius <= FLT_EPSILON) { // Negative radius - circle empty
		തിരികെ AddResult::Degenerate;
	}

	എങ്കിൽ (!inFixedPointRange(fabsf(center(0)) + k_gon_radius)
	    || !inFixedPointRange(fabsf(center(1)) + k_gon_radius)) {
		തിരികെ AddResult::OutOfRange;
	}

	// Canonical orientation: CCW for exclusion, CW for inclusion
	സ്ഥിരം ദശാംശം delta_angle = (is_inclusion_zone ? -1.f : 1.f) * 2.f * M_PI_F / kCircleApproxVertices;

	PolygonInfo &poly = _polygons[അക്കം_polygons];
	poly.start_index = അക്കം_nodes_2;
	poly.അക്കം_vertices = kCircleApproxVertices;
	poly.is_inclusion = is_inclusion_zone;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < kCircleApproxVertices; i++) {
		സ്ഥിരം ദശാംശം angle = (i + 0.5f) * delta_angle;
		സ്ഥിരം matrix::Vector2f p = center + matrix::Vector2f{
			k_gon_radius * cosf(angle), k_gon_radius * sinf(angle)
		};
		set_കണ്ണി(poly.start_index + i, p);
	}

	computeBoundingBox(poly.start_index, kCircleApproxVertices, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// For regular k-gons, all corners are either convex or reflex based on
	// is_inclusion, so the reduced visibility graph criterion is simplified
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < kCircleApproxVertices; i++) {
		കണ്ണി_not_on_optimal_path[poly.start_index + i] = is_inclusion_zone;
	}

	അക്കം_nodes_2 += kCircleApproxVertices;
	++അക്കം_polygons;
	തിരികെ AddResult::Success;
}

ശൂന്യം PlannerPolygons::computeBoundingBox(സ്ഥിരം പൂർണ്ണസംഖ്യ start_index, സ്ഥിരം പൂർണ്ണസംഖ്യ അക്കം_vertices,
		int32_t &min_x, int32_t &max_x, int32_t &min_y, int32_t &max_y)
{

	// Store the bounding box, so we can do a very cheap check against the
	// bounding box, and only do the full line-polygon intersection check if
	// the line intersects the bounding box.

	min_x = max_x = _x_cm[start_index];
	min_y = max_y = _y_cm[start_index];

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < അക്കം_vertices; i++) {
		സ്ഥിരം int32_t x = _x_cm[start_index + i];
		സ്ഥിരം int32_t y = _y_cm[start_index + i];

		എങ്കിൽ (x < min_x) { min_x = x; }

		എങ്കിൽ (x > max_x) { max_x = x; }

		എങ്കിൽ (y < min_y) { min_y = y; }

		എങ്കിൽ (y > max_y) { max_y = y; }
	}
}

തർക്കം PlannerPolygons::pointInsideInteriorCone(സ്ഥിരം PolygonInfo &poly,
		int32_t px, int32_t py, പൂർണ്ണസംഖ്യ v) സ്ഥിരം
{
	സ്ഥിരം പൂർണ്ണസംഖ്യ prev = poly.start_index + ((v + poly.അക്കം_vertices - 1) % poly.അക്കം_vertices);
	സ്ഥിരം പൂർണ്ണസംഖ്യ curr = poly.start_index + v;
	സ്ഥിരം പൂർണ്ണസംഖ്യ next = poly.start_index + ((v + 1) % poly.അക്കം_vertices);

	സ്ഥിരം പൂർണ്ണസംഖ്യ p_ഇടത്_of_incoming  = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], px, py);
	സ്ഥിരം പൂർണ്ണസംഖ്യ p_ഇടത്_of_outgoing  = orient2d(_x_cm[curr], _y_cm[curr], _x_cm[next], _y_cm[next], px, py);
	സ്ഥിരം പൂർണ്ണസംഖ്യ is_convex           = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], _x_cm[next], _y_cm[next]);

	തിരികെ is_convex > 0  // If convex==0 (exact 180 deg "corner") the two cases are equivalent
	       ? (p_ഇടത്_of_incoming > 0 && p_ഇടത്_of_outgoing > 0)   // Convex: interior = intersection of two half-planes
	       : (p_ഇടത്_of_incoming > 0 || p_ഇടത്_of_outgoing > 0);  // Reflex: interior = union of two half-planes
}

തർക്കം PlannerPolygons::intersectsInsideOf(സ്ഥിരം PolygonInfo &poly,
		int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) സ്ഥിരം
{
	// Single pass over polygon edges, doing two jobs:
	//  (1) classify the test segment vs each polygon edge. early-return on:
	//       - strict crossings (line segment - polygon edge)
	//       - interior crossing at vertex
	//  (2) accumulate midpoint winding contribution -- if (1) is inconclusive,
	//      this decides if the line is inside or outside

	പൂർണ്ണസംഖ്യ wn = 0;
	തർക്കം mid_on_boundary = അസത്യം;

	// Rather than calculating the midpoint (mid = (s + e)/2) explicitly,
	// which will lead to rounding error for uneven numbers, we instead
	// scale all other inputs by two, reducing the range by a factor of two
	// (2^31 cm = 21400 km -> 2^30 cm = 10700 km)
	സ്ഥിരം int32_t twice_mid_x = s_x + e_x;
	സ്ഥിരം int32_t twice_mid_y = s_y + e_y;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < poly.അക്കം_vertices; i++) {

		സ്ഥിരം പൂർണ്ണസംഖ്യ prev_idx = poly.start_index + (i + poly.അക്കം_vertices - 1) % poly.അക്കം_vertices;
		സ്ഥിരം പൂർണ്ണസംഖ്യ curr_idx = poly.start_index + i;

		സ്ഥിരം int32_t ax = _x_cm[prev_idx], ay = _y_cm[prev_idx];
		സ്ഥിരം int32_t bx = _x_cm[curr_idx], by = _y_cm[curr_idx];

		// (1) test segment vs polygon edge (a, b)
		തിരഞ്ഞെടുപ്പ് (segmentsIntersect(ax, ay, bx, by, s_x, s_y, e_x, e_y)) {
		അവസ്ഥ SegSegResult::Cross:
			തിരികെ സത്യം;

		// skip AInsideCD - a from this segment is b from the adjacent segment
		അവസ്ഥ SegSegResult::BInsideCD:

			// Polygon vertex b is exactly on the interior of the test segment.
			// Return intersecting if it pokes into the polygon interior through vertex b.
			എങ്കിൽ (pointInsideInteriorCone(poly, s_x, s_y, i) || pointInsideInteriorCone(poly, e_x, e_y, i)) {
				തിരികെ സത്യം;
			}

			നിർത്തുക;

		സ്ഥിരസ്ഥിതി:
			നിർത്തുക;
		}

		// (2) midpoint winding contribution
		// Ref: Dan Sunday, Inclusion of a Point in a Polygon
		// https://web.archive.org/web/20130126163405/http://geomalgorithms.com/a03-_inclusion.html

		സ്ഥിരം പൂർണ്ണസംഖ്യ side = orient2d(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);

		എങ്കിൽ (side == 0) {

			സ്ഥിരം തർക്കം mid_on_open_ab = collinearBetween(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);
			സ്ഥിരം തർക്കം mid_is_b = 2 * bx == twice_mid_x && 2 * by == twice_mid_y;

			// Skip point a - a from this segment is b from the adjacent segment
			എങ്കിൽ (mid_on_open_ab || mid_is_b) {
				mid_on_boundary = സത്യം;
			}

		} അല്ലെങ്കിൽ എങ്കിൽ (2 * ay <= twice_mid_y) {
			എങ്കിൽ (2 * by > twice_mid_y && side > 0) { wn++; }

		} അല്ലെങ്കിൽ എങ്കിൽ (2 * by <= twice_mid_y && side < 0) { wn--; }
	}


	// Midpoint on boundary, segment runs along it (strict crossing detected earlier).
	// Non-intersecting in either zone.
	എങ്കിൽ (mid_on_boundary) {
		തിരികെ അസത്യം;
	}

	// Exclusion: violation when midpoint is inside (bounded). Inclusion: when outside (unbounded).
	// This is the only place we depart from the inside = left = forbidden canonical orientation,
	// necessary because of the point membership check.
	സ്ഥിരം തർക്കം midpoint_in_bounded_inside = (wn != 0);
	തിരികെ midpoint_in_bounded_inside != poly.is_inclusion;
}

തർക്കം PlannerPolygons::intersectsAnyInside(int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) സ്ഥിരം
{
	സ്ഥിരം int32_t seg_min_x = s_x < e_x ? s_x : e_x;
	സ്ഥിരം int32_t seg_max_x = s_x > e_x ? s_x : e_x;
	സ്ഥിരം int32_t seg_min_y = s_y < e_y ? s_y : e_y;
	സ്ഥിരം int32_t seg_max_y = s_y > e_y ? s_y : e_y;

	വേണ്ടി (പൂർണ്ണസംഖ്യ p = 0; p < അക്കം_polygons; p++) {
		സ്ഥിരം PolygonInfo &poly = _polygons[p];

		എങ്കിൽ (seg_max_x < poly.min_x || seg_min_x > poly.max_x ||
		    seg_max_y < poly.min_y || seg_min_y > poly.max_y) {
			// Segment is entirely outside this polygon.
			// For exclusion zones: outside = safe, skip.
			// For inclusion zones: outside = forbidden region, immediate violation.
			എങ്കിൽ (poly.is_inclusion) { തിരികെ സത്യം; }

			തുടരുക;
		}

		എങ്കിൽ (intersectsInsideOf(poly, s_x, s_y, e_x, e_y)) {
			തിരികെ സത്യം;
		}
	}

	തിരികെ അസത്യം;
}

തർക്കം PlannerPolygons::edgeBitangent(പൂർണ്ണസംഖ്യ a, int32_t bx, int32_t by) സ്ഥിരം
{
	// Find the polygon owning vertex index a. Indices not belonging to any
	// polygon (in particular the destination at index 0) are trivially bitangent.

	എങ്കിൽ (a == destIndex()) { തിരികെ സത്യം; }

	വേണ്ടി (പൂർണ്ണസംഖ്യ p = 0; p < അക്കം_polygons; p++) {
		സ്ഥിരം PolygonInfo &poly = _polygons[p];

		എങ്കിൽ (a >= poly.start_index && a < poly.start_index + poly.അക്കം_vertices) {
			// Define point c = a + (a-b) = 2a - b, so that ac = ab mirrored about a
			// Then: Edge is bitangent at a <=> neither ab nor ac pokes into the interior
			int32_t cx = 2 * _x_cm[a] - bx;
			int32_t cy = 2 * _y_cm[a] - by;
			തിരികെ !pointInsideInteriorCone(poly, bx, by, a - poly.start_index)
			       && !pointInsideInteriorCone(poly, cx, cy, a - poly.start_index);
		}
	}

	തിരികെ സത്യം;
}

തർക്കം PlannerPolygons::edgeBitangent(പൂർണ്ണസംഖ്യ a, പൂർണ്ണസംഖ്യ b) സ്ഥിരം
{
	തിരികെ edgeBitangent(a, _x_cm[b], _y_cm[b]);
}

തർക്കം PlannerPolygons::edgeBitangent(പൂർണ്ണസംഖ്യ a, സ്ഥിരം matrix::Vector2f &b) സ്ഥിരം
{
	തിരികെ edgeBitangent(a, metersToCm(b(0)), metersToCm(b(1)));
}

തർക്കം PlannerPolygons::setDestination(സ്ഥിരം matrix::Vector2f &p)
{
	set_കണ്ണി(destIndex(), p);

	സ്ഥിരം int32_t x_cm = metersToCm(p(0));
	സ്ഥിരം int32_t y_cm = metersToCm(p(1));

	എങ്കിൽ (intersectsAnyInside(x_cm, y_cm, x_cm, y_cm)) {
		// If the destination breaches a geofence, still use it but
		// signal to the user that it is invalid.
		തിരികെ അസത്യം;
	}

	തിരികെ സത്യം;
}

matrix::Vector2f PlannerPolygons::getDestination() സ്ഥിരം
{
	എങ്കിൽ (_max_nodes < 1) {
		തിരികെ matrix::Vector2f{NAN, NAN};
	}

	തിരികെ കണ്ണി_2(destIndex());
}

തർക്കം PlannerPolygons::edgeVisible(പൂർണ്ണസംഖ്യ a, പൂർണ്ണസംഖ്യ b) സ്ഥിരം
{
	എങ്കിൽ (a == b) { തിരികെ അസത്യം; }

	എങ്കിൽ (കണ്ണി_not_on_optimal_path[a] || കണ്ണി_not_on_optimal_path[b]) { തിരികെ അസത്യം; }

	// Only bitangent edges can be on a shortest path; this check also cheaply
	// discards edges that violate the polygon at the vertex they come from.
	എങ്കിൽ (!edgeBitangent(a, b) || !edgeBitangent(b, a)) { തിരികെ അസത്യം; }

	തിരികെ !intersectsAnyInside(_x_cm[a], _y_cm[a], _x_cm[b], _y_cm[b]);
}

തർക്കം PlannerPolygons::edgeVisible(സ്ഥിരം matrix::Vector2f &a, പൂർണ്ണസംഖ്യ b) സ്ഥിരം
{
	എങ്കിൽ (കണ്ണി_not_on_optimal_path[b]) { തിരികെ അസത്യം; }

	എങ്കിൽ (!edgeBitangent(b, a)) { തിരികെ അസത്യം; }

	തിരികെ !intersectsAnyInside(metersToCm(a(0)), metersToCm(a(1)), _x_cm[b], _y_cm[b]);
}

തർക്കം PlannerPolygons::edgeVisible(സ്ഥിരം matrix::Vector2f &a, സ്ഥിരം matrix::Vector2f &b) സ്ഥിരം
{
	തിരികെ !intersectsAnyInside(metersToCm(a(0)), metersToCm(a(1)), metersToCm(b(0)), metersToCm(b(1)));
}

ദശാംശം PlannerPolygons::edgeCost(പൂർണ്ണസംഖ്യ a, പൂർണ്ണസംഖ്യ b) സ്ഥിരം
{
	തിരികെ edgeVisible(a, b) ? (കണ്ണി_2(a) - കണ്ണി_2(b)).norm() : INFINITY;
}


തർക്കം segmentsIntersectInclusive(സ്ഥിരം matrix::Vector2d &a, സ്ഥിരം matrix::Vector2d &b,
				സ്ഥിരം matrix::Vector2d &c, സ്ഥിരം matrix::Vector2d &d)
{
	// Reject separated bounds, including gaps between collinear segments.
	വേണ്ടി (പൂർണ്ണസംഖ്യ axis = 0; axis < 2; ++axis) {
		എങ്കിൽ (math::max(a(axis), b(axis)) < math::min(c(axis), d(axis))
		    || math::max(c(axis), d(axis)) < math::min(a(axis), b(axis))) {
			തിരികെ അസത്യം;
		}
	}

	സ്ഥിരം matrix::Vector2d ab = b - a;
	സ്ഥിരം matrix::Vector2d cd = d - c;
	സ്ഥിരം ഇരട്ട side_c = ab.cross(c - a);
	സ്ഥിരം ഇരട്ട side_d = ab.cross(d - a);
	സ്ഥിരം ഇരട്ട side_a = cd.cross(a - c);
	സ്ഥിരം ഇരട്ട side_b = cd.cross(b - c);

	// Each segment must cross or touch the line through the other segment.
	തിരികെ ((side_c <= 0.0 && side_d >= 0.0) || (side_c >= 0.0 && side_d <= 0.0))
	       && ((side_a <= 0.0 && side_b >= 0.0) || (side_a >= 0.0 && side_b <= 0.0));
}

ഇരട്ട pointToSegmentDistanceSquared(സ്ഥിരം matrix::Vector2d &point,
				     സ്ഥിരം matrix::Vector2d &a, സ്ഥിരം matrix::Vector2d &b)
{
	സ്ഥിരം matrix::Vector2d direction = b - a;
	സ്ഥിരം matrix::Vector2d to_point = point - a;
	സ്ഥിരം ഇരട്ട length_squared = direction.norm_squared();
	// along / length_squared locates the projection: 0 at a, 1 at b.
	സ്ഥിരം ഇരട്ട along = to_point.dot(direction);

	എങ്കിൽ (along <= 0.0) {
		തിരികെ to_point.norm_squared();

	} അല്ലെങ്കിൽ എങ്കിൽ (along >= length_squared) {
		തിരികെ (point - b).norm_squared();
	}

	// Squared distance to the line: |to_point x direction|^2 / |direction|^2.
	// https://mathworld.wolfram.com/2-DimensionalPoint-LineDistance.html
	സ്ഥിരം ഇരട്ട cross = to_point.cross(direction);
	തിരികെ cross * cross / length_squared;
}

തർക്കം isPolygonCCW(സ്ഥിരം matrix::Vector2f *vertices, പൂർണ്ണസംഖ്യ അക്കം_vertices)
{
	// https://en.wikipedia.org/wiki/Shoelace_formula
	// Triangle formula
	ദശാംശം signed_area_2x = 0.f;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_vertices; i++) {
		സ്ഥിരം പൂർണ്ണസംഖ്യ j = (i + 1) % അക്കം_vertices;
		signed_area_2x += vertices[i](0) * vertices[j](1)
				  - vertices[j](0) * vertices[i](1);
	}

	തിരികെ signed_area_2x > 0.f;
}

} // namespace geofence_utils
