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

#شامل "geofence_utils.h"
#شامل <lib/geo/geo.h>
#شامل <lib/mathlib/mathlib.h>

فضاینام geofence_utils
{

فضاینام
{

int32_t metersToCm(اعشاری m) { بازگشت roundf(m * CM_PER_M); }

} // namespace


منطقی PlannerPolygons::allocate(صحیح max_nodes)
{
	freeBuffers();

	اگر (max_nodes > 0) {
		_x_cm = جدید int32_t[max_nodes];
		_y_cm = جدید int32_t[max_nodes];
		گره_not_on_optimal_path = جدید منطقی[max_nodes];

		اگر (_x_cm != nullptr && _y_cm != nullptr && گره_not_on_optimal_path != nullptr) {
			_max_nodes = max_nodes;

		} وگرنه {
			freeBuffers();
		}
	}

	reset();

	بازگشت _max_nodes == max_nodes;
}

تهی PlannerPolygons::freeBuffers()
{
	delete[] _x_cm;
	delete[] _y_cm;
	delete[] گره_not_on_optimal_path;
	_x_cm = nullptr;
	_y_cm = nullptr;
	گره_not_on_optimal_path = nullptr;
	_max_nodes = 0;
}

تهی PlannerPolygons::set_گره(صحیح idx, ثابت matrix::Vector2f &p)
{
	اگر (_max_nodes < 1) {
		بازگشت;
	}

	idx = math::constrain(idx, 0, _max_nodes - 1);

	_x_cm[idx] = metersToCm(p(0));
	_y_cm[idx] = metersToCm(p(1));
}

PlannerPolygons::AddResult PlannerPolygons::addPolygon(ثابت matrix::Vector2f *vertices_in, صحیح رقم_vertices,
		منطقی is_inclusion_zone, اعشاری margin)
{
	اگر (رقم_polygons >= kMaxPolygons || رقم_nodes_2 + رقم_vertices > _max_nodes) {
		بازگشت AddResult::BudgetExceeded;
	}

	اگر (margin < -FLT_EPSILON || رقم_vertices < 3) { بازگشت AddResult::Degenerate; }

	برای (صحیح i = 0; i < رقم_vertices; ++i) {
		// Reject self-intersecting (non-simple) polygons.
		ثابت matrix::Vector2f &a = vertices_in[i];
		ثابت matrix::Vector2f &b = vertices_in[(i + 1) % رقم_vertices];

		برای (صحیح j = i + 2; j < رقم_vertices; ++j) {
			اگر (i == 0 && j == رقم_vertices - 1) {
				ادامه; // (last edge, first edge) are adjacent
			}

			ثابت matrix::Vector2f &c = vertices_in[j];
			ثابت matrix::Vector2f &d = vertices_in[(j + 1) % رقم_vertices];

			اگر (segmentsIntersect(
				    metersToCm(a(0)), metersToCm(a(1)), metersToCm(b(0)), metersToCm(b(1)),
				    metersToCm(c(0)), metersToCm(c(1)), metersToCm(d(0)), metersToCm(d(1))) != SegSegResult::Disjoint) {
				بازگشت AddResult::Degenerate;
			}
		}

		// Reject vertices outside the usable fixed-point range
		اگر (!inFixedPointRange(vertices_in[i](0)) || !inFixedPointRange(vertices_in[i](1))) {
			بازگشت AddResult::OutOfRange;
		}
	}

	// Canonical orientation: walking vertices in stored order, INSIDE is on
	// the left of each edge. CCW for exclusion (bounded interior is forbidden),
	// CW for inclusion (unbounded exterior is forbidden). Reverse iff input
	// doesn't already match.
	ثابت منطقی reverse = (isPolygonCCW(vertices_in, رقم_vertices) == is_inclusion_zone);

	// Index into vertices_in in stored order (accounts for possible reversal).
	auto vertex = [&](صحیح i) -> ثابت matrix::Vector2f & {
		بازگشت vertices_in[reverse ? (رقم_vertices - 1 - i) : i];
	};

	PolygonInfo &poly = _polygons[رقم_polygons];
	poly.start_index  = رقم_nodes_2;
	poly.is_inclusion = is_inclusion_zone;

	صحیح out_idx = 0;

	برای (صحیح i = 0; i < رقم_vertices; i++) {
		ثابت صحیح prev = (i + رقم_vertices - 1) % رقم_vertices;
		ثابت صحیح next = (i + 1) % رقم_vertices;

		ثابت matrix::Vector2f edge_in  = vertex(i) - vertex(prev);
		ثابت matrix::Vector2f edge_out = vertex(next) - vertex(i);

		ثابت اعشاری edge_in_norm = edge_in.norm();
		ثابت اعشاری edge_out_norm = edge_out.norm();

		اگر (edge_in_norm < FLT_EPSILON || edge_out_norm < FLT_EPSILON) {
			بازگشت AddResult::Degenerate;
		}

		ثابت matrix::Vector2f edge_in_normalized = edge_in / edge_in_norm;
		ثابت matrix::Vector2f edge_out_normalized = edge_out / edge_out_norm;

		// Inward normals of the two adjacent edges.
		// Canonical stored orientation: left side = forbidden region, right side = allowed region
		ثابت matrix::Vector2f n_in  = matrix::Vector2f{-edge_in_normalized(1), edge_in_normalized(0)};
		ثابت matrix::Vector2f n_out = matrix::Vector2f{-edge_out_normalized(1), edge_out_normalized(0)};

		matrix::Vector2f bisector = n_in + n_out;
		ثابت اعشاری bisector_len  = bisector.length();

		اگر (bisector_len < FLT_EPSILON) {
			بازگشت AddResult::Degenerate; // antiparallel edges - polygon doubles back
		}

		ثابت matrix::Vector2f normalized_bisector = bisector / bisector_len;

		// The following bit ensures that:
		//  - The original edges are moved away from the forbidden interior by `margin`
		//  - Sharp convex corners (<60deg) are "cut off" with a new edge that is exactly
		//    `margin` away from the original vertex. For this the vertex is replaced by
		//    two new ones.

		// Only split convex corners - others are ignored, see _node_not_on_optimal_path
		ثابت منطقی corner_convex = edge_in_normalized(0) * edge_out_normalized(1)
					   - edge_in_normalized(1) * edge_out_normalized(0) > 0.f;

		// Scaling needed wrt the normalised bisector to achieve parallel edges
		ثابت اعشاری offset_scale = 2.f / bisector_len;

		// bisector_len = 2*sin(interior angle/2); split when interior angle < 60 deg <=> bisector_len < 1.
		ثابت منطقی angle_sharp = bisector_len < 1;

		// If margin zero, do not split - would output the same vertex twice.
		ثابت منطقی margin_nonzero = margin > FLT_EPSILON;

		// If we do not have enough space, do not split the vertex.
		// This never happens if the planner-internal node buffer is 2x the original buffer.
		ثابت منطقی space_for_split_vertices = رقم_nodes_2 + out_idx + 1 < _max_nodes;

		اگر (corner_convex && angle_sharp && margin_nonzero && space_for_split_vertices) {

			// Sharp corner: cut it off along the line
			//  - perpendicular to normalized_bisector
			//  - going through base := vertex(i) - normalized_bisector * margin

			ثابت matrix::Vector2f base = vertex(i) - normalized_bisector * margin;
			ثابت matrix::Vector2f perp = {-normalized_bisector(1), normalized_bisector(0)};

			// Construct s such that:
			//  base + perp*s is on the offset incoming edge
			//  base - perp*s is on the offset outgoing edge
			ثابت اعشاری e_out_dot_b = edge_out_normalized.dot(normalized_bisector);
			ثابت اعشاری s = margin * (1.f - n_out.dot(normalized_bisector)) / e_out_dot_b;

			set_گره(poly.start_index + out_idx++, base + perp * s);
			set_گره(poly.start_index + out_idx++, base - perp * s);

		} وگرنه {
			// Regular corner or no space for split: single bisector vertex displaced by margin / sin(interior angle/2).
			set_گره(poly.start_index + out_idx++, vertex(i) - normalized_bisector * (margin * offset_scale));
		}
	}

	ثابت صحیح رقم_out = out_idx;
	poly.رقم_vertices = رقم_out;

	computeBoundingBox(poly.start_index, رقم_out, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// In canonical orientation, the node is not possibly on the shortest
	// path if the vertex curves right or straight (forbidden region >= 180
	// deg)
	برای (صحیح i = 0; i < رقم_out; i++) {
		ثابت صحیح prev = poly.start_index + (i + رقم_out - 1) % رقم_out;
		ثابت صحیح curr = poly.start_index + i;
		ثابت صحیح next = poly.start_index + (i + 1) % رقم_out;
		ثابت منطقی curves_راست_or_straight = orient2d(_x_cm[prev], _y_cm[prev],
						      _x_cm[curr], _y_cm[curr],
						      _x_cm[next], _y_cm[next]) <= 0;
		گره_not_on_optimal_path[curr] = curves_راست_or_straight;
	}

	رقم_nodes_2 += رقم_out;
	++رقم_polygons;
	بازگشت AddResult::Success;
}

PlannerPolygons::AddResult PlannerPolygons::addApproxCircle(ثابت matrix::Vector2f &center, ثابت اعشاری circle_radius,
		اعشاری margin, ثابت منطقی is_inclusion_zone)
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

	اگر (رقم_polygons >= kMaxPolygons || رقم_nodes_2 + kCircleApproxVertices > _max_nodes) {
		بازگشت AddResult::BudgetExceeded;
	}

	اگر (circle_radius <= 0.1f) { بازگشت AddResult::Degenerate; }

	// ensures that the k-gon is, w.r.t the real circle:
	//  - over-approximation of circle grown by margin, if exclusion
	//  - under-approximation of circle shrunk by margin, if inclusion
	ثابت اعشاری k_gon_radius = is_inclusion_zone
				   ? circle_radius - margin
				   : circle_radius / cosf(M_PI_F / kCircleApproxVertices) + margin;

	اگر (k_gon_radius <= FLT_EPSILON) { // Negative radius - circle empty
		بازگشت AddResult::Degenerate;
	}

	اگر (!inFixedPointRange(fabsf(center(0)) + k_gon_radius)
	    || !inFixedPointRange(fabsf(center(1)) + k_gon_radius)) {
		بازگشت AddResult::OutOfRange;
	}

	// Canonical orientation: CCW for exclusion, CW for inclusion
	ثابت اعشاری delta_angle = (is_inclusion_zone ? -1.f : 1.f) * 2.f * M_PI_F / kCircleApproxVertices;

	PolygonInfo &poly = _polygons[رقم_polygons];
	poly.start_index = رقم_nodes_2;
	poly.رقم_vertices = kCircleApproxVertices;
	poly.is_inclusion = is_inclusion_zone;

	برای (صحیح i = 0; i < kCircleApproxVertices; i++) {
		ثابت اعشاری angle = (i + 0.5f) * delta_angle;
		ثابت matrix::Vector2f p = center + matrix::Vector2f{
			k_gon_radius * cosf(angle), k_gon_radius * sinf(angle)
		};
		set_گره(poly.start_index + i, p);
	}

	computeBoundingBox(poly.start_index, kCircleApproxVertices, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// For regular k-gons, all corners are either convex or reflex based on
	// is_inclusion, so the reduced visibility graph criterion is simplified
	برای (صحیح i = 0; i < kCircleApproxVertices; i++) {
		گره_not_on_optimal_path[poly.start_index + i] = is_inclusion_zone;
	}

	رقم_nodes_2 += kCircleApproxVertices;
	++رقم_polygons;
	بازگشت AddResult::Success;
}

تهی PlannerPolygons::computeBoundingBox(ثابت صحیح start_index, ثابت صحیح رقم_vertices,
		int32_t &min_x, int32_t &max_x, int32_t &min_y, int32_t &max_y)
{

	// Store the bounding box, so we can do a very cheap check against the
	// bounding box, and only do the full line-polygon intersection check if
	// the line intersects the bounding box.

	min_x = max_x = _x_cm[start_index];
	min_y = max_y = _y_cm[start_index];

	برای (صحیح i = 1; i < رقم_vertices; i++) {
		ثابت int32_t x = _x_cm[start_index + i];
		ثابت int32_t y = _y_cm[start_index + i];

		اگر (x < min_x) { min_x = x; }

		اگر (x > max_x) { max_x = x; }

		اگر (y < min_y) { min_y = y; }

		اگر (y > max_y) { max_y = y; }
	}
}

منطقی PlannerPolygons::pointInsideInteriorCone(ثابت PolygonInfo &poly,
		int32_t px, int32_t py, صحیح v) ثابت
{
	ثابت صحیح prev = poly.start_index + ((v + poly.رقم_vertices - 1) % poly.رقم_vertices);
	ثابت صحیح curr = poly.start_index + v;
	ثابت صحیح next = poly.start_index + ((v + 1) % poly.رقم_vertices);

	ثابت صحیح p_چپ_of_incoming  = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], px, py);
	ثابت صحیح p_چپ_of_outgoing  = orient2d(_x_cm[curr], _y_cm[curr], _x_cm[next], _y_cm[next], px, py);
	ثابت صحیح is_convex           = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], _x_cm[next], _y_cm[next]);

	بازگشت is_convex > 0  // If convex==0 (exact 180 deg "corner") the two cases are equivalent
	       ? (p_چپ_of_incoming > 0 && p_چپ_of_outgoing > 0)   // Convex: interior = intersection of two half-planes
	       : (p_چپ_of_incoming > 0 || p_چپ_of_outgoing > 0);  // Reflex: interior = union of two half-planes
}

منطقی PlannerPolygons::intersectsInsideOf(ثابت PolygonInfo &poly,
		int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ثابت
{
	// Single pass over polygon edges, doing two jobs:
	//  (1) classify the test segment vs each polygon edge. early-return on:
	//       - strict crossings (line segment - polygon edge)
	//       - interior crossing at vertex
	//  (2) accumulate midpoint winding contribution -- if (1) is inconclusive,
	//      this decides if the line is inside or outside

	صحیح wn = 0;
	منطقی mid_on_boundary = نادرست;

	// Rather than calculating the midpoint (mid = (s + e)/2) explicitly,
	// which will lead to rounding error for uneven numbers, we instead
	// scale all other inputs by two, reducing the range by a factor of two
	// (2^31 cm = 21400 km -> 2^30 cm = 10700 km)
	ثابت int32_t twice_mid_x = s_x + e_x;
	ثابت int32_t twice_mid_y = s_y + e_y;

	برای (صحیح i = 0; i < poly.رقم_vertices; i++) {

		ثابت صحیح prev_idx = poly.start_index + (i + poly.رقم_vertices - 1) % poly.رقم_vertices;
		ثابت صحیح curr_idx = poly.start_index + i;

		ثابت int32_t ax = _x_cm[prev_idx], ay = _y_cm[prev_idx];
		ثابت int32_t bx = _x_cm[curr_idx], by = _y_cm[curr_idx];

		// (1) test segment vs polygon edge (a, b)
		انتخاب (segmentsIntersect(ax, ay, bx, by, s_x, s_y, e_x, e_y)) {
		حالت SegSegResult::Cross:
			بازگشت درست;

		// skip AInsideCD - a from this segment is b from the adjacent segment
		حالت SegSegResult::BInsideCD:

			// Polygon vertex b is exactly on the interior of the test segment.
			// Return intersecting if it pokes into the polygon interior through vertex b.
			اگر (pointInsideInteriorCone(poly, s_x, s_y, i) || pointInsideInteriorCone(poly, e_x, e_y, i)) {
				بازگشت درست;
			}

			بشکن;

		پیشفرض:
			بشکن;
		}

		// (2) midpoint winding contribution
		// Ref: Dan Sunday, Inclusion of a Point in a Polygon
		// https://web.archive.org/web/20130126163405/http://geomalgorithms.com/a03-_inclusion.html

		ثابت صحیح side = orient2d(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);

		اگر (side == 0) {

			ثابت منطقی mid_on_open_ab = collinearBetween(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);
			ثابت منطقی mid_is_b = 2 * bx == twice_mid_x && 2 * by == twice_mid_y;

			// Skip point a - a from this segment is b from the adjacent segment
			اگر (mid_on_open_ab || mid_is_b) {
				mid_on_boundary = درست;
			}

		} وگرنه اگر (2 * ay <= twice_mid_y) {
			اگر (2 * by > twice_mid_y && side > 0) { wn++; }

		} وگرنه اگر (2 * by <= twice_mid_y && side < 0) { wn--; }
	}


	// Midpoint on boundary, segment runs along it (strict crossing detected earlier).
	// Non-intersecting in either zone.
	اگر (mid_on_boundary) {
		بازگشت نادرست;
	}

	// Exclusion: violation when midpoint is inside (bounded). Inclusion: when outside (unbounded).
	// This is the only place we depart from the inside = left = forbidden canonical orientation,
	// necessary because of the point membership check.
	ثابت منطقی midpoint_in_bounded_inside = (wn != 0);
	بازگشت midpoint_in_bounded_inside != poly.is_inclusion;
}

منطقی PlannerPolygons::intersectsAnyInside(int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ثابت
{
	ثابت int32_t seg_min_x = s_x < e_x ? s_x : e_x;
	ثابت int32_t seg_max_x = s_x > e_x ? s_x : e_x;
	ثابت int32_t seg_min_y = s_y < e_y ? s_y : e_y;
	ثابت int32_t seg_max_y = s_y > e_y ? s_y : e_y;

	برای (صحیح p = 0; p < رقم_polygons; p++) {
		ثابت PolygonInfo &poly = _polygons[p];

		اگر (seg_max_x < poly.min_x || seg_min_x > poly.max_x ||
		    seg_max_y < poly.min_y || seg_min_y > poly.max_y) {
			// Segment is entirely outside this polygon.
			// For exclusion zones: outside = safe, skip.
			// For inclusion zones: outside = forbidden region, immediate violation.
			اگر (poly.is_inclusion) { بازگشت درست; }

			ادامه;
		}

		اگر (intersectsInsideOf(poly, s_x, s_y, e_x, e_y)) {
			بازگشت درست;
		}
	}

	بازگشت نادرست;
}

منطقی PlannerPolygons::edgeBitangent(صحیح a, int32_t bx, int32_t by) ثابت
{
	// Find the polygon owning vertex index a. Indices not belonging to any
	// polygon (in particular the destination at index 0) are trivially bitangent.

	اگر (a == destIndex()) { بازگشت درست; }

	برای (صحیح p = 0; p < رقم_polygons; p++) {
		ثابت PolygonInfo &poly = _polygons[p];

		اگر (a >= poly.start_index && a < poly.start_index + poly.رقم_vertices) {
			// Define point c = a + (a-b) = 2a - b, so that ac = ab mirrored about a
			// Then: Edge is bitangent at a <=> neither ab nor ac pokes into the interior
			int32_t cx = 2 * _x_cm[a] - bx;
			int32_t cy = 2 * _y_cm[a] - by;
			بازگشت !pointInsideInteriorCone(poly, bx, by, a - poly.start_index)
			       && !pointInsideInteriorCone(poly, cx, cy, a - poly.start_index);
		}
	}

	بازگشت درست;
}

منطقی PlannerPolygons::edgeBitangent(صحیح a, صحیح b) ثابت
{
	بازگشت edgeBitangent(a, _x_cm[b], _y_cm[b]);
}

منطقی PlannerPolygons::edgeBitangent(صحیح a, ثابت matrix::Vector2f &b) ثابت
{
	بازگشت edgeBitangent(a, metersToCm(b(0)), metersToCm(b(1)));
}

منطقی PlannerPolygons::setDestination(ثابت matrix::Vector2f &p)
{
	set_گره(destIndex(), p);

	ثابت int32_t x_cm = metersToCm(p(0));
	ثابت int32_t y_cm = metersToCm(p(1));

	اگر (intersectsAnyInside(x_cm, y_cm, x_cm, y_cm)) {
		// If the destination breaches a geofence, still use it but
		// signal to the user that it is invalid.
		بازگشت نادرست;
	}

	بازگشت درست;
}

matrix::Vector2f PlannerPolygons::getDestination() ثابت
{
	اگر (_max_nodes < 1) {
		بازگشت matrix::Vector2f{NAN, NAN};
	}

	بازگشت گره_2(destIndex());
}

منطقی PlannerPolygons::edgeVisible(صحیح a, صحیح b) ثابت
{
	اگر (a == b) { بازگشت نادرست; }

	اگر (گره_not_on_optimal_path[a] || گره_not_on_optimal_path[b]) { بازگشت نادرست; }

	// Only bitangent edges can be on a shortest path; this check also cheaply
	// discards edges that violate the polygon at the vertex they come from.
	اگر (!edgeBitangent(a, b) || !edgeBitangent(b, a)) { بازگشت نادرست; }

	بازگشت !intersectsAnyInside(_x_cm[a], _y_cm[a], _x_cm[b], _y_cm[b]);
}

منطقی PlannerPolygons::edgeVisible(ثابت matrix::Vector2f &a, صحیح b) ثابت
{
	اگر (گره_not_on_optimal_path[b]) { بازگشت نادرست; }

	اگر (!edgeBitangent(b, a)) { بازگشت نادرست; }

	بازگشت !intersectsAnyInside(metersToCm(a(0)), metersToCm(a(1)), _x_cm[b], _y_cm[b]);
}

منطقی PlannerPolygons::edgeVisible(ثابت matrix::Vector2f &a, ثابت matrix::Vector2f &b) ثابت
{
	بازگشت !intersectsAnyInside(metersToCm(a(0)), metersToCm(a(1)), metersToCm(b(0)), metersToCm(b(1)));
}

اعشاری PlannerPolygons::edgeCost(صحیح a, صحیح b) ثابت
{
	بازگشت edgeVisible(a, b) ? (گره_2(a) - گره_2(b)).norm() : INFINITY;
}


منطقی segmentsIntersectInclusive(ثابت matrix::Vector2d &a, ثابت matrix::Vector2d &b,
				ثابت matrix::Vector2d &c, ثابت matrix::Vector2d &d)
{
	// Reject separated bounds, including gaps between collinear segments.
	برای (صحیح axis = 0; axis < 2; ++axis) {
		اگر (math::max(a(axis), b(axis)) < math::min(c(axis), d(axis))
		    || math::max(c(axis), d(axis)) < math::min(a(axis), b(axis))) {
			بازگشت نادرست;
		}
	}

	ثابت matrix::Vector2d ab = b - a;
	ثابت matrix::Vector2d cd = d - c;
	ثابت دوگانه side_c = ab.cross(c - a);
	ثابت دوگانه side_d = ab.cross(d - a);
	ثابت دوگانه side_a = cd.cross(a - c);
	ثابت دوگانه side_b = cd.cross(b - c);

	// Each segment must cross or touch the line through the other segment.
	بازگشت ((side_c <= 0.0 && side_d >= 0.0) || (side_c >= 0.0 && side_d <= 0.0))
	       && ((side_a <= 0.0 && side_b >= 0.0) || (side_a >= 0.0 && side_b <= 0.0));
}

دوگانه pointToSegmentDistanceSquared(ثابت matrix::Vector2d &point,
				     ثابت matrix::Vector2d &a, ثابت matrix::Vector2d &b)
{
	ثابت matrix::Vector2d direction = b - a;
	ثابت matrix::Vector2d to_point = point - a;
	ثابت دوگانه length_squared = direction.norm_squared();
	// along / length_squared locates the projection: 0 at a, 1 at b.
	ثابت دوگانه along = to_point.dot(direction);

	اگر (along <= 0.0) {
		بازگشت to_point.norm_squared();

	} وگرنه اگر (along >= length_squared) {
		بازگشت (point - b).norm_squared();
	}

	// Squared distance to the line: |to_point x direction|^2 / |direction|^2.
	// https://mathworld.wolfram.com/2-DimensionalPoint-LineDistance.html
	ثابت دوگانه cross = to_point.cross(direction);
	بازگشت cross * cross / length_squared;
}

منطقی isPolygonCCW(ثابت matrix::Vector2f *vertices, صحیح رقم_vertices)
{
	// https://en.wikipedia.org/wiki/Shoelace_formula
	// Triangle formula
	اعشاری signed_area_2x = 0.f;

	برای (صحیح i = 0; i < رقم_vertices; i++) {
		ثابت صحیح j = (i + 1) % رقم_vertices;
		signed_area_2x += vertices[i](0) * vertices[j](1)
				  - vertices[j](0) * vertices[i](1);
	}

	بازگشت signed_area_2x > 0.f;
}

} // namespace geofence_utils
