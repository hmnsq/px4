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

#ضمن "geofence_utils.h"
#ضمن <lib/geo/geo.h>
#ضمن <lib/mathlib/mathlib.h>

نطاق geofence_utils
{

نطاق
{

int32_t metersToCm(عائم m) { أرجع roundf(m * CM_PER_M); }

} // namespace


منطقي PlannerPolygons::allocate(صحيح max_nodes)
{
	freeBuffers();

	إذا (max_nodes > 0) {
		_x_cm = جديد int32_t[max_nodes];
		_y_cm = جديد int32_t[max_nodes];
		گرێ_not_on_optimal_path = جديد منطقي[max_nodes];

		إذا (_x_cm != nullptr && _y_cm != nullptr && گرێ_not_on_optimal_path != nullptr) {
			_max_nodes = max_nodes;

		} وإلا {
			freeBuffers();
		}
	}

	reset();

	أرجع _max_nodes == max_nodes;
}

فراغ PlannerPolygons::freeBuffers()
{
	delete[] _x_cm;
	delete[] _y_cm;
	delete[] گرێ_not_on_optimal_path;
	_x_cm = nullptr;
	_y_cm = nullptr;
	گرێ_not_on_optimal_path = nullptr;
	_max_nodes = 0;
}

فراغ PlannerPolygons::set_گرێ(صحيح idx, ثابت matrix::Vector2f &p)
{
	إذا (_max_nodes < 1) {
		أرجع;
	}

	idx = math::constrain(idx, 0, _max_nodes - 1);

	_x_cm[idx] = metersToCm(p(0));
	_y_cm[idx] = metersToCm(p(1));
}

PlannerPolygons::AddResult PlannerPolygons::addPolygon(ثابت matrix::Vector2f *vertices_in, صحيح ژمارە_vertices,
		منطقي is_inclusion_zone, عائم margin)
{
	إذا (ژمارە_polygons >= kMaxPolygons || ژمارە_nodes_2 + ژمارە_vertices > _max_nodes) {
		أرجع AddResult::BudgetExceeded;
	}

	إذا (margin < -FLT_EPSILON || ژمارە_vertices < 3) { أرجع AddResult::Degenerate; }

	لكل (صحيح i = 0; i < ژمارە_vertices; ++i) {
		// Reject self-intersecting (non-simple) polygons.
		ثابت matrix::Vector2f &a = vertices_in[i];
		ثابت matrix::Vector2f &b = vertices_in[(i + 1) % ژمارە_vertices];

		لكل (صحيح j = i + 2; j < ژمارە_vertices; ++j) {
			إذا (i == 0 && j == ژمارە_vertices - 1) {
				استمر; // (last edge, first edge) are adjacent
			}

			ثابت matrix::Vector2f &c = vertices_in[j];
			ثابت matrix::Vector2f &d = vertices_in[(j + 1) % ژمارە_vertices];

			إذا (segmentsIntersect(
				    metersToCm(a(0)), metersToCm(a(1)), metersToCm(b(0)), metersToCm(b(1)),
				    metersToCm(c(0)), metersToCm(c(1)), metersToCm(d(0)), metersToCm(d(1))) != SegSegResult::Disjoint) {
				أرجع AddResult::Degenerate;
			}
		}

		// Reject vertices outside the usable fixed-point range
		إذا (!inFixedPointRange(vertices_in[i](0)) || !inFixedPointRange(vertices_in[i](1))) {
			أرجع AddResult::OutOfRange;
		}
	}

	// Canonical orientation: walking vertices in stored order, INSIDE is on
	// the left of each edge. CCW for exclusion (bounded interior is forbidden),
	// CW for inclusion (unbounded exterior is forbidden). Reverse iff input
	// doesn't already match.
	ثابت منطقي reverse = (isPolygonCCW(vertices_in, ژمارە_vertices) == is_inclusion_zone);

	// Index into vertices_in in stored order (accounts for possible reversal).
	auto vertex = [&](صحيح i) -> ثابت matrix::Vector2f & {
		أرجع vertices_in[reverse ? (ژمارە_vertices - 1 - i) : i];
	};

	PolygonInfo &poly = _polygons[ژمارە_polygons];
	poly.start_index  = ژمارە_nodes_2;
	poly.is_inclusion = is_inclusion_zone;

	صحيح out_idx = 0;

	لكل (صحيح i = 0; i < ژمارە_vertices; i++) {
		ثابت صحيح prev = (i + ژمارە_vertices - 1) % ژمارە_vertices;
		ثابت صحيح next = (i + 1) % ژمارە_vertices;

		ثابت matrix::Vector2f edge_in  = vertex(i) - vertex(prev);
		ثابت matrix::Vector2f edge_out = vertex(next) - vertex(i);

		ثابت عائم edge_in_norm = edge_in.norm();
		ثابت عائم edge_out_norm = edge_out.norm();

		إذا (edge_in_norm < FLT_EPSILON || edge_out_norm < FLT_EPSILON) {
			أرجع AddResult::Degenerate;
		}

		ثابت matrix::Vector2f edge_in_normalized = edge_in / edge_in_norm;
		ثابت matrix::Vector2f edge_out_normalized = edge_out / edge_out_norm;

		// Inward normals of the two adjacent edges.
		// Canonical stored orientation: left side = forbidden region, right side = allowed region
		ثابت matrix::Vector2f n_in  = matrix::Vector2f{-edge_in_normalized(1), edge_in_normalized(0)};
		ثابت matrix::Vector2f n_out = matrix::Vector2f{-edge_out_normalized(1), edge_out_normalized(0)};

		matrix::Vector2f bisector = n_in + n_out;
		ثابت عائم bisector_len  = bisector.length();

		إذا (bisector_len < FLT_EPSILON) {
			أرجع AddResult::Degenerate; // antiparallel edges - polygon doubles back
		}

		ثابت matrix::Vector2f normalized_bisector = bisector / bisector_len;

		// The following bit ensures that:
		//  - The original edges are moved away from the forbidden interior by `margin`
		//  - Sharp convex corners (<60deg) are "cut off" with a new edge that is exactly
		//    `margin` away from the original vertex. For this the vertex is replaced by
		//    two new ones.

		// Only split convex corners - others are ignored, see _node_not_on_optimal_path
		ثابت منطقي corner_convex = edge_in_normalized(0) * edge_out_normalized(1)
					   - edge_in_normalized(1) * edge_out_normalized(0) > 0.f;

		// Scaling needed wrt the normalised bisector to achieve parallel edges
		ثابت عائم offset_scale = 2.f / bisector_len;

		// bisector_len = 2*sin(interior angle/2); split when interior angle < 60 deg <=> bisector_len < 1.
		ثابت منطقي angle_sharp = bisector_len < 1;

		// If margin zero, do not split - would output the same vertex twice.
		ثابت منطقي margin_nonzero = margin > FLT_EPSILON;

		// If we do not have enough space, do not split the vertex.
		// This never happens if the planner-internal node buffer is 2x the original buffer.
		ثابت منطقي space_for_split_vertices = ژمارە_nodes_2 + out_idx + 1 < _max_nodes;

		إذا (corner_convex && angle_sharp && margin_nonzero && space_for_split_vertices) {

			// Sharp corner: cut it off along the line
			//  - perpendicular to normalized_bisector
			//  - going through base := vertex(i) - normalized_bisector * margin

			ثابت matrix::Vector2f base = vertex(i) - normalized_bisector * margin;
			ثابت matrix::Vector2f perp = {-normalized_bisector(1), normalized_bisector(0)};

			// Construct s such that:
			//  base + perp*s is on the offset incoming edge
			//  base - perp*s is on the offset outgoing edge
			ثابت عائم e_out_dot_b = edge_out_normalized.dot(normalized_bisector);
			ثابت عائم s = margin * (1.f - n_out.dot(normalized_bisector)) / e_out_dot_b;

			set_گرێ(poly.start_index + out_idx++, base + perp * s);
			set_گرێ(poly.start_index + out_idx++, base - perp * s);

		} وإلا {
			// Regular corner or no space for split: single bisector vertex displaced by margin / sin(interior angle/2).
			set_گرێ(poly.start_index + out_idx++, vertex(i) - normalized_bisector * (margin * offset_scale));
		}
	}

	ثابت صحيح ژمارە_out = out_idx;
	poly.ژمارە_vertices = ژمارە_out;

	computeBoundingBox(poly.start_index, ژمارە_out, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// In canonical orientation, the node is not possibly on the shortest
	// path if the vertex curves right or straight (forbidden region >= 180
	// deg)
	لكل (صحيح i = 0; i < ژمارە_out; i++) {
		ثابت صحيح prev = poly.start_index + (i + ژمارە_out - 1) % ژمارە_out;
		ثابت صحيح curr = poly.start_index + i;
		ثابت صحيح next = poly.start_index + (i + 1) % ژمارە_out;
		ثابت منطقي curves_ڕاست_or_straight = orient2d(_x_cm[prev], _y_cm[prev],
						      _x_cm[curr], _y_cm[curr],
						      _x_cm[next], _y_cm[next]) <= 0;
		گرێ_not_on_optimal_path[curr] = curves_ڕاست_or_straight;
	}

	ژمارە_nodes_2 += ژمارە_out;
	++ژمارە_polygons;
	أرجع AddResult::Success;
}

PlannerPolygons::AddResult PlannerPolygons::addApproxCircle(ثابت matrix::Vector2f &center, ثابت عائم circle_radius,
		عائم margin, ثابت منطقي is_inclusion_zone)
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

	إذا (ژمارە_polygons >= kMaxPolygons || ژمارە_nodes_2 + kCircleApproxVertices > _max_nodes) {
		أرجع AddResult::BudgetExceeded;
	}

	إذا (circle_radius <= 0.1f) { أرجع AddResult::Degenerate; }

	// ensures that the k-gon is, w.r.t the real circle:
	//  - over-approximation of circle grown by margin, if exclusion
	//  - under-approximation of circle shrunk by margin, if inclusion
	ثابت عائم k_gon_radius = is_inclusion_zone
				   ? circle_radius - margin
				   : circle_radius / cosf(M_PI_F / kCircleApproxVertices) + margin;

	إذا (k_gon_radius <= FLT_EPSILON) { // Negative radius - circle empty
		أرجع AddResult::Degenerate;
	}

	إذا (!inFixedPointRange(fabsf(center(0)) + k_gon_radius)
	    || !inFixedPointRange(fabsf(center(1)) + k_gon_radius)) {
		أرجع AddResult::OutOfRange;
	}

	// Canonical orientation: CCW for exclusion, CW for inclusion
	ثابت عائم delta_angle = (is_inclusion_zone ? -1.f : 1.f) * 2.f * M_PI_F / kCircleApproxVertices;

	PolygonInfo &poly = _polygons[ژمارە_polygons];
	poly.start_index = ژمارە_nodes_2;
	poly.ژمارە_vertices = kCircleApproxVertices;
	poly.is_inclusion = is_inclusion_zone;

	لكل (صحيح i = 0; i < kCircleApproxVertices; i++) {
		ثابت عائم angle = (i + 0.5f) * delta_angle;
		ثابت matrix::Vector2f p = center + matrix::Vector2f{
			k_gon_radius * cosf(angle), k_gon_radius * sinf(angle)
		};
		set_گرێ(poly.start_index + i, p);
	}

	computeBoundingBox(poly.start_index, kCircleApproxVertices, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// For regular k-gons, all corners are either convex or reflex based on
	// is_inclusion, so the reduced visibility graph criterion is simplified
	لكل (صحيح i = 0; i < kCircleApproxVertices; i++) {
		گرێ_not_on_optimal_path[poly.start_index + i] = is_inclusion_zone;
	}

	ژمارە_nodes_2 += kCircleApproxVertices;
	++ژمارە_polygons;
	أرجع AddResult::Success;
}

فراغ PlannerPolygons::computeBoundingBox(ثابت صحيح start_index, ثابت صحيح ژمارە_vertices,
		int32_t &min_x, int32_t &max_x, int32_t &min_y, int32_t &max_y)
{

	// Store the bounding box, so we can do a very cheap check against the
	// bounding box, and only do the full line-polygon intersection check if
	// the line intersects the bounding box.

	min_x = max_x = _x_cm[start_index];
	min_y = max_y = _y_cm[start_index];

	لكل (صحيح i = 1; i < ژمارە_vertices; i++) {
		ثابت int32_t x = _x_cm[start_index + i];
		ثابت int32_t y = _y_cm[start_index + i];

		إذا (x < min_x) { min_x = x; }

		إذا (x > max_x) { max_x = x; }

		إذا (y < min_y) { min_y = y; }

		إذا (y > max_y) { max_y = y; }
	}
}

منطقي PlannerPolygons::pointInsideInteriorCone(ثابت PolygonInfo &poly,
		int32_t px, int32_t py, صحيح v) ثابت
{
	ثابت صحيح prev = poly.start_index + ((v + poly.ژمارە_vertices - 1) % poly.ژمارە_vertices);
	ثابت صحيح curr = poly.start_index + v;
	ثابت صحيح next = poly.start_index + ((v + 1) % poly.ژمارە_vertices);

	ثابت صحيح p_چەپ_of_incoming  = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], px, py);
	ثابت صحيح p_چەپ_of_outgoing  = orient2d(_x_cm[curr], _y_cm[curr], _x_cm[next], _y_cm[next], px, py);
	ثابت صحيح is_convex           = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], _x_cm[next], _y_cm[next]);

	أرجع is_convex > 0  // If convex==0 (exact 180 deg "corner") the two cases are equivalent
	       ? (p_چەپ_of_incoming > 0 && p_چەپ_of_outgoing > 0)   // Convex: interior = intersection of two half-planes
	       : (p_چەپ_of_incoming > 0 || p_چەپ_of_outgoing > 0);  // Reflex: interior = union of two half-planes
}

منطقي PlannerPolygons::intersectsInsideOf(ثابت PolygonInfo &poly,
		int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ثابت
{
	// Single pass over polygon edges, doing two jobs:
	//  (1) classify the test segment vs each polygon edge. early-return on:
	//       - strict crossings (line segment - polygon edge)
	//       - interior crossing at vertex
	//  (2) accumulate midpoint winding contribution -- if (1) is inconclusive,
	//      this decides if the line is inside or outside

	صحيح wn = 0;
	منطقي mid_on_boundary = خطأ;

	// Rather than calculating the midpoint (mid = (s + e)/2) explicitly,
	// which will lead to rounding error for uneven numbers, we instead
	// scale all other inputs by two, reducing the range by a factor of two
	// (2^31 cm = 21400 km -> 2^30 cm = 10700 km)
	ثابت int32_t twice_mid_x = s_x + e_x;
	ثابت int32_t twice_mid_y = s_y + e_y;

	لكل (صحيح i = 0; i < poly.ژمارە_vertices; i++) {

		ثابت صحيح prev_idx = poly.start_index + (i + poly.ژمارە_vertices - 1) % poly.ژمارە_vertices;
		ثابت صحيح curr_idx = poly.start_index + i;

		ثابت int32_t ax = _x_cm[prev_idx], ay = _y_cm[prev_idx];
		ثابت int32_t bx = _x_cm[curr_idx], by = _y_cm[curr_idx];

		// (1) test segment vs polygon edge (a, b)
		بدل (segmentsIntersect(ax, ay, bx, by, s_x, s_y, e_x, e_y)) {
		حالة SegSegResult::Cross:
			أرجع true;

		// skip AInsideCD - a from this segment is b from the adjacent segment
		حالة SegSegResult::BInsideCD:

			// Polygon vertex b is exactly on the interior of the test segment.
			// Return intersecting if it pokes into the polygon interior through vertex b.
			إذا (pointInsideInteriorCone(poly, s_x, s_y, i) || pointInsideInteriorCone(poly, e_x, e_y, i)) {
				أرجع true;
			}

			اكسر;

		افتراضي:
			اكسر;
		}

		// (2) midpoint winding contribution
		// Ref: Dan Sunday, Inclusion of a Point in a Polygon
		// https://web.archive.org/web/20130126163405/http://geomalgorithms.com/a03-_inclusion.html

		ثابت صحيح side = orient2d(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);

		إذا (side == 0) {

			ثابت منطقي mid_on_open_ab = collinearBetween(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);
			ثابت منطقي mid_is_b = 2 * bx == twice_mid_x && 2 * by == twice_mid_y;

			// Skip point a - a from this segment is b from the adjacent segment
			إذا (mid_on_open_ab || mid_is_b) {
				mid_on_boundary = true;
			}

		} وإلا إذا (2 * ay <= twice_mid_y) {
			إذا (2 * by > twice_mid_y && side > 0) { wn++; }

		} وإلا إذا (2 * by <= twice_mid_y && side < 0) { wn--; }
	}


	// Midpoint on boundary, segment runs along it (strict crossing detected earlier).
	// Non-intersecting in either zone.
	إذا (mid_on_boundary) {
		أرجع خطأ;
	}

	// Exclusion: violation when midpoint is inside (bounded). Inclusion: when outside (unbounded).
	// This is the only place we depart from the inside = left = forbidden canonical orientation,
	// necessary because of the point membership check.
	ثابت منطقي midpoint_in_bounded_inside = (wn != 0);
	أرجع midpoint_in_bounded_inside != poly.is_inclusion;
}

منطقي PlannerPolygons::intersectsAnyInside(int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ثابت
{
	ثابت int32_t seg_min_x = s_x < e_x ? s_x : e_x;
	ثابت int32_t seg_max_x = s_x > e_x ? s_x : e_x;
	ثابت int32_t seg_min_y = s_y < e_y ? s_y : e_y;
	ثابت int32_t seg_max_y = s_y > e_y ? s_y : e_y;

	لكل (صحيح p = 0; p < ژمارە_polygons; p++) {
		ثابت PolygonInfo &poly = _polygons[p];

		إذا (seg_max_x < poly.min_x || seg_min_x > poly.max_x ||
		    seg_max_y < poly.min_y || seg_min_y > poly.max_y) {
			// Segment is entirely outside this polygon.
			// For exclusion zones: outside = safe, skip.
			// For inclusion zones: outside = forbidden region, immediate violation.
			إذا (poly.is_inclusion) { أرجع true; }

			استمر;
		}

		إذا (intersectsInsideOf(poly, s_x, s_y, e_x, e_y)) {
			أرجع true;
		}
	}

	أرجع خطأ;
}

منطقي PlannerPolygons::edgeBitangent(صحيح a, int32_t bx, int32_t by) ثابت
{
	// Find the polygon owning vertex index a. Indices not belonging to any
	// polygon (in particular the destination at index 0) are trivially bitangent.

	إذا (a == destIndex()) { أرجع true; }

	لكل (صحيح p = 0; p < ژمارە_polygons; p++) {
		ثابت PolygonInfo &poly = _polygons[p];

		إذا (a >= poly.start_index && a < poly.start_index + poly.ژمارە_vertices) {
			// Define point c = a + (a-b) = 2a - b, so that ac = ab mirrored about a
			// Then: Edge is bitangent at a <=> neither ab nor ac pokes into the interior
			int32_t cx = 2 * _x_cm[a] - bx;
			int32_t cy = 2 * _y_cm[a] - by;
			أرجع !pointInsideInteriorCone(poly, bx, by, a - poly.start_index)
			       && !pointInsideInteriorCone(poly, cx, cy, a - poly.start_index);
		}
	}

	أرجع true;
}

منطقي PlannerPolygons::edgeBitangent(صحيح a, صحيح b) ثابت
{
	أرجع edgeBitangent(a, _x_cm[b], _y_cm[b]);
}

منطقي PlannerPolygons::edgeBitangent(صحيح a, ثابت matrix::Vector2f &b) ثابت
{
	أرجع edgeBitangent(a, metersToCm(b(0)), metersToCm(b(1)));
}

منطقي PlannerPolygons::setDestination(ثابت matrix::Vector2f &p)
{
	set_گرێ(destIndex(), p);

	ثابت int32_t x_cm = metersToCm(p(0));
	ثابت int32_t y_cm = metersToCm(p(1));

	إذا (intersectsAnyInside(x_cm, y_cm, x_cm, y_cm)) {
		// If the destination breaches a geofence, still use it but
		// signal to the user that it is invalid.
		أرجع خطأ;
	}

	أرجع true;
}

matrix::Vector2f PlannerPolygons::getDestination() ثابت
{
	إذا (_max_nodes < 1) {
		أرجع matrix::Vector2f{NAN, NAN};
	}

	أرجع گرێ_2(destIndex());
}

منطقي PlannerPolygons::edgeVisible(صحيح a, صحيح b) ثابت
{
	إذا (a == b) { أرجع خطأ; }

	إذا (گرێ_not_on_optimal_path[a] || گرێ_not_on_optimal_path[b]) { أرجع خطأ; }

	// Only bitangent edges can be on a shortest path; this check also cheaply
	// discards edges that violate the polygon at the vertex they come from.
	إذا (!edgeBitangent(a, b) || !edgeBitangent(b, a)) { أرجع خطأ; }

	أرجع !intersectsAnyInside(_x_cm[a], _y_cm[a], _x_cm[b], _y_cm[b]);
}

منطقي PlannerPolygons::edgeVisible(ثابت matrix::Vector2f &a, صحيح b) ثابت
{
	إذا (گرێ_not_on_optimal_path[b]) { أرجع خطأ; }

	إذا (!edgeBitangent(b, a)) { أرجع خطأ; }

	أرجع !intersectsAnyInside(metersToCm(a(0)), metersToCm(a(1)), _x_cm[b], _y_cm[b]);
}

منطقي PlannerPolygons::edgeVisible(ثابت matrix::Vector2f &a, ثابت matrix::Vector2f &b) ثابت
{
	أرجع !intersectsAnyInside(metersToCm(a(0)), metersToCm(a(1)), metersToCm(b(0)), metersToCm(b(1)));
}

عائم PlannerPolygons::edgeCost(صحيح a, صحيح b) ثابت
{
	أرجع edgeVisible(a, b) ? (گرێ_2(a) - گرێ_2(b)).norm() : INFINITY;
}


منطقي segmentsIntersectInclusive(ثابت matrix::Vector2d &a, ثابت matrix::Vector2d &b,
				ثابت matrix::Vector2d &c, ثابت matrix::Vector2d &d)
{
	// Reject separated bounds, including gaps between collinear segments.
	لكل (صحيح axis = 0; axis < 2; ++axis) {
		إذا (math::max(a(axis), b(axis)) < math::min(c(axis), d(axis))
		    || math::max(c(axis), d(axis)) < math::min(a(axis), b(axis))) {
			أرجع خطأ;
		}
	}

	ثابت matrix::Vector2d ab = b - a;
	ثابت matrix::Vector2d cd = d - c;
	ثابت مزدوج side_c = ab.cross(c - a);
	ثابت مزدوج side_d = ab.cross(d - a);
	ثابت مزدوج side_a = cd.cross(a - c);
	ثابت مزدوج side_b = cd.cross(b - c);

	// Each segment must cross or touch the line through the other segment.
	أرجع ((side_c <= 0.0 && side_d >= 0.0) || (side_c >= 0.0 && side_d <= 0.0))
	       && ((side_a <= 0.0 && side_b >= 0.0) || (side_a >= 0.0 && side_b <= 0.0));
}

مزدوج pointToSegmentDistanceSquared(ثابت matrix::Vector2d &point,
				     ثابت matrix::Vector2d &a, ثابت matrix::Vector2d &b)
{
	ثابت matrix::Vector2d direction = b - a;
	ثابت matrix::Vector2d to_point = point - a;
	ثابت مزدوج length_squared = direction.norm_squared();
	// along / length_squared locates the projection: 0 at a, 1 at b.
	ثابت مزدوج along = to_point.dot(direction);

	إذا (along <= 0.0) {
		أرجع to_point.norm_squared();

	} وإلا إذا (along >= length_squared) {
		أرجع (point - b).norm_squared();
	}

	// Squared distance to the line: |to_point x direction|^2 / |direction|^2.
	// https://mathworld.wolfram.com/2-DimensionalPoint-LineDistance.html
	ثابت مزدوج cross = to_point.cross(direction);
	أرجع cross * cross / length_squared;
}

منطقي isPolygonCCW(ثابت matrix::Vector2f *vertices, صحيح ژمارە_vertices)
{
	// https://en.wikipedia.org/wiki/Shoelace_formula
	// Triangle formula
	عائم signed_area_2x = 0.f;

	لكل (صحيح i = 0; i < ژمارە_vertices; i++) {
		ثابت صحيح j = (i + 1) % ژمارە_vertices;
		signed_area_2x += vertices[i](0) * vertices[j](1)
				  - vertices[j](0) * vertices[i](1);
	}

	أرجع signed_area_2x > 0.f;
}

} // namespace geofence_utils
