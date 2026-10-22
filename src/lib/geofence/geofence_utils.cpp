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

#অন্তর্ভাব "geofence_utils.h"
#অন্তর্ভাব <lib/geo/geo.h>
#অন্তর্ভাব <lib/mathlib/mathlib.h>

নামাকাশ geofence_utils
{

নামাকাশ
{

int32_t metersToCm(দশমিক m) { ফেরত roundf(m * CM_PER_M); }

} // namespace


যৌক্তিক PlannerPolygons::allocate(পূর্ণসংখ্যা max_nodes)
{
	freeBuffers();

	যদি (max_nodes > 0) {
		_x_cm = নতুন int32_t[max_nodes];
		_y_cm = নতুন int32_t[max_nodes];
		গ্রন্থি_not_on_optimal_path = নতুন যৌক্তিক[max_nodes];

		যদি (_x_cm != nullptr && _y_cm != nullptr && গ্রন্থি_not_on_optimal_path != nullptr) {
			_max_nodes = max_nodes;

		} অন্যথা {
			freeBuffers();
		}
	}

	reset();

	ফেরত _max_nodes == max_nodes;
}

শূন্য PlannerPolygons::freeBuffers()
{
	delete[] _x_cm;
	delete[] _y_cm;
	delete[] গ্রন্থি_not_on_optimal_path;
	_x_cm = nullptr;
	_y_cm = nullptr;
	গ্রন্থি_not_on_optimal_path = nullptr;
	_max_nodes = 0;
}

শূন্য PlannerPolygons::set_গ্রন্থি(পূর্ণসংখ্যা idx, ধ্রুবক matrix::Vector2f &p)
{
	যদি (_max_nodes < 1) {
		ফেরত;
	}

	idx = math::constrain(idx, 0, _max_nodes - 1);

	_x_cm[idx] = metersToCm(p(0));
	_y_cm[idx] = metersToCm(p(1));
}

PlannerPolygons::Add_ফলাফল PlannerPolygons::addPolygon(ধ্রুবক matrix::Vector2f *vertices_in, পূর্ণসংখ্যা অঙ্ক_vertices,
		যৌক্তিক is_inclusion_zone, দশমিক margin)
{
	যদি (অঙ্ক_polygons >= kMaxPolygons || অঙ্ক_nodes_2 + অঙ্ক_vertices > _max_nodes) {
		ফেরত Add_ফলাফল::BudgetExceeded;
	}

	যদি (margin < -FLT_EPSILON || অঙ্ক_vertices < 3) { ফেরত Add_ফলাফল::Degenerate; }

	জন্য (পূর্ণসংখ্যা i = 0; i < অঙ্ক_vertices; ++i) {
		// Reject self-intersecting (non-simple) polygons.
		ধ্রুবক matrix::Vector2f &a = vertices_in[i];
		ধ্রুবক matrix::Vector2f &b = vertices_in[(i + 1) % অঙ্ক_vertices];

		জন্য (পূর্ণসংখ্যা j = i + 2; j < অঙ্ক_vertices; ++j) {
			যদি (i == 0 && j == অঙ্ক_vertices - 1) {
				চালু; // (last edge, first edge) are adjacent
			}

			ধ্রুবক matrix::Vector2f &c = vertices_in[j];
			ধ্রুবক matrix::Vector2f &d = vertices_in[(j + 1) % অঙ্ক_vertices];

			যদি (segmentsIntersect(
				    metersToCm(a(0)), metersToCm(a(1)), metersToCm(b(0)), metersToCm(b(1)),
				    metersToCm(c(0)), metersToCm(c(1)), metersToCm(d(0)), metersToCm(d(1))) != Seg_Seg_ফলাফল::Disjoint) {
				ফেরত Add_ফলাফল::Degenerate;
			}
		}

		// Reject vertices outside the usable fixed-point range
		যদি (!inFixedPointRange(vertices_in[i](0)) || !inFixedPointRange(vertices_in[i](1))) {
			ফেরত Add_ফলাফল::OutOfRange;
		}
	}

	// Canonical orientation: walking vertices in stored order, INSIDE is on
	// the left of each edge. CCW for exclusion (bounded interior is forbidden),
	// CW for inclusion (unbounded exterior is forbidden). Reverse iff input
	// doesn't already match.
	ধ্রুবক যৌক্তিক reverse = (isPolygonCCW(vertices_in, অঙ্ক_vertices) == is_inclusion_zone);

	// Index into vertices_in in stored order (accounts for possible reversal).
	auto vertex = [&](পূর্ণসংখ্যা i) -> ধ্রুবক matrix::Vector2f & {
		ফেরত vertices_in[reverse ? (অঙ্ক_vertices - 1 - i) : i];
	};

	PolygonInfo &poly = _polygons[অঙ্ক_polygons];
	poly.start_সূচক  = অঙ্ক_nodes_2;
	poly.is_inclusion = is_inclusion_zone;

	পূর্ণসংখ্যা out_idx = 0;

	জন্য (পূর্ণসংখ্যা i = 0; i < অঙ্ক_vertices; i++) {
		ধ্রুবক পূর্ণসংখ্যা পূর্ব = (i + অঙ্ক_vertices - 1) % অঙ্ক_vertices;
		ধ্রুবক পূর্ণসংখ্যা পরবর্তী = (i + 1) % অঙ্ক_vertices;

		ধ্রুবক matrix::Vector2f edge_in  = vertex(i) - vertex(পূর্ব);
		ধ্রুবক matrix::Vector2f edge_out = vertex(পরবর্তী) - vertex(i);

		ধ্রুবক দশমিক edge_in_norm = edge_in.norm();
		ধ্রুবক দশমিক edge_out_norm = edge_out.norm();

		যদি (edge_in_norm < FLT_EPSILON || edge_out_norm < FLT_EPSILON) {
			ফেরত Add_ফলাফল::Degenerate;
		}

		ধ্রুবক matrix::Vector2f edge_in_normalized = edge_in / edge_in_norm;
		ধ্রুবক matrix::Vector2f edge_out_normalized = edge_out / edge_out_norm;

		// Inward normals of the two adjacent edges.
		// Canonical stored orientation: left side = forbidden region, right side = allowed region
		ধ্রুবক matrix::Vector2f n_in  = matrix::Vector2f{-edge_in_normalized(1), edge_in_normalized(0)};
		ধ্রুবক matrix::Vector2f n_out = matrix::Vector2f{-edge_out_normalized(1), edge_out_normalized(0)};

		matrix::Vector2f bisector = n_in + n_out;
		ধ্রুবক দশমিক bisector_len  = bisector.দৈর্ঘ্য();

		যদি (bisector_len < FLT_EPSILON) {
			ফেরত Add_ফলাফল::Degenerate; // antiparallel edges - polygon doubles back
		}

		ধ্রুবক matrix::Vector2f normalized_bisector = bisector / bisector_len;

		// The following bit ensures that:
		//  - The original edges are moved away from the forbidden interior by `margin`
		//  - Sharp convex corners (<60deg) are "cut off" with a new edge that is exactly
		//    `margin` away from the original vertex. For this the vertex is replaced by
		//    two new ones.

		// Only split convex corners - others are ignored, see _node_not_on_optimal_path
		ধ্রুবক যৌক্তিক corner_convex = edge_in_normalized(0) * edge_out_normalized(1)
					   - edge_in_normalized(1) * edge_out_normalized(0) > 0.f;

		// Scaling needed wrt the normalised bisector to achieve parallel edges
		ধ্রুবক দশমিক offset_scale = 2.f / bisector_len;

		// bisector_len = 2*sin(interior angle/2); split when interior angle < 60 deg <=> bisector_len < 1.
		ধ্রুবক যৌক্তিক angle_sharp = bisector_len < 1;

		// If margin zero, do not split - would output the same vertex twice.
		ধ্রুবক যৌক্তিক margin_nonzero = margin > FLT_EPSILON;

		// If we do not have enough space, do not split the vertex.
		// This never happens if the planner-internal node buffer is 2x the original buffer.
		ধ্রুবক যৌক্তিক space_for_split_vertices = অঙ্ক_nodes_2 + out_idx + 1 < _max_nodes;

		যদি (corner_convex && angle_sharp && margin_nonzero && space_for_split_vertices) {

			// Sharp corner: cut it off along the line
			//  - perpendicular to normalized_bisector
			//  - going through base := vertex(i) - normalized_bisector * margin

			ধ্রুবক matrix::Vector2f base = vertex(i) - normalized_bisector * margin;
			ধ্রুবক matrix::Vector2f perp = {-normalized_bisector(1), normalized_bisector(0)};

			// Construct s such that:
			//  base + perp*s is on the offset incoming edge
			//  base - perp*s is on the offset outgoing edge
			ধ্রুবক দশমিক e_out_dot_b = edge_out_normalized.dot(normalized_bisector);
			ধ্রুবক দশমিক s = margin * (1.f - n_out.dot(normalized_bisector)) / e_out_dot_b;

			set_গ্রন্থি(poly.start_সূচক + out_idx++, base + perp * s);
			set_গ্রন্থি(poly.start_সূচক + out_idx++, base - perp * s);

		} অন্যথা {
			// Regular corner or no space for split: single bisector vertex displaced by margin / sin(interior angle/2).
			set_গ্রন্থি(poly.start_সূচক + out_idx++, vertex(i) - normalized_bisector * (margin * offset_scale));
		}
	}

	ধ্রুবক পূর্ণসংখ্যা অঙ্ক_out = out_idx;
	poly.অঙ্ক_vertices = অঙ্ক_out;

	computeBoundingBox(poly.start_সূচক, অঙ্ক_out, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// In canonical orientation, the node is not possibly on the shortest
	// path if the vertex curves right or straight (forbidden region >= 180
	// deg)
	জন্য (পূর্ণসংখ্যা i = 0; i < অঙ্ক_out; i++) {
		ধ্রুবক পূর্ণসংখ্যা পূর্ব = poly.start_সূচক + (i + অঙ্ক_out - 1) % অঙ্ক_out;
		ধ্রুবক পূর্ণসংখ্যা curr = poly.start_সূচক + i;
		ধ্রুবক পূর্ণসংখ্যা পরবর্তী = poly.start_সূচক + (i + 1) % অঙ্ক_out;
		ধ্রুবক যৌক্তিক curves_ডান_or_straight = orient2d(_x_cm[পূর্ব], _y_cm[পূর্ব],
						      _x_cm[curr], _y_cm[curr],
						      _x_cm[পরবর্তী], _y_cm[পরবর্তী]) <= 0;
		গ্রন্থি_not_on_optimal_path[curr] = curves_ডান_or_straight;
	}

	অঙ্ক_nodes_2 += অঙ্ক_out;
	++অঙ্ক_polygons;
	ফেরত Add_ফলাফল::Success;
}

PlannerPolygons::Add_ফলাফল PlannerPolygons::addApproxCircle(ধ্রুবক matrix::Vector2f &center, ধ্রুবক দশমিক circle_radius,
		দশমিক margin, ধ্রুবক যৌক্তিক is_inclusion_zone)
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

	যদি (অঙ্ক_polygons >= kMaxPolygons || অঙ্ক_nodes_2 + kCircleApproxVertices > _max_nodes) {
		ফেরত Add_ফলাফল::BudgetExceeded;
	}

	যদি (circle_radius <= 0.1f) { ফেরত Add_ফলাফল::Degenerate; }

	// ensures that the k-gon is, w.r.t the real circle:
	//  - over-approximation of circle grown by margin, if exclusion
	//  - under-approximation of circle shrunk by margin, if inclusion
	ধ্রুবক দশমিক k_gon_radius = is_inclusion_zone
				   ? circle_radius - margin
				   : circle_radius / cosf(M_PI_F / kCircleApproxVertices) + margin;

	যদি (k_gon_radius <= FLT_EPSILON) { // Negative radius - circle empty
		ফেরত Add_ফলাফল::Degenerate;
	}

	যদি (!inFixedPointRange(fabsf(center(0)) + k_gon_radius)
	    || !inFixedPointRange(fabsf(center(1)) + k_gon_radius)) {
		ফেরত Add_ফলাফল::OutOfRange;
	}

	// Canonical orientation: CCW for exclusion, CW for inclusion
	ধ্রুবক দশমিক delta_angle = (is_inclusion_zone ? -1.f : 1.f) * 2.f * M_PI_F / kCircleApproxVertices;

	PolygonInfo &poly = _polygons[অঙ্ক_polygons];
	poly.start_সূচক = অঙ্ক_nodes_2;
	poly.অঙ্ক_vertices = kCircleApproxVertices;
	poly.is_inclusion = is_inclusion_zone;

	জন্য (পূর্ণসংখ্যা i = 0; i < kCircleApproxVertices; i++) {
		ধ্রুবক দশমিক angle = (i + 0.5f) * delta_angle;
		ধ্রুবক matrix::Vector2f p = center + matrix::Vector2f{
			k_gon_radius * cosf(angle), k_gon_radius * sinf(angle)
		};
		set_গ্রন্থি(poly.start_সূচক + i, p);
	}

	computeBoundingBox(poly.start_সূচক, kCircleApproxVertices, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// For regular k-gons, all corners are either convex or reflex based on
	// is_inclusion, so the reduced visibility graph criterion is simplified
	জন্য (পূর্ণসংখ্যা i = 0; i < kCircleApproxVertices; i++) {
		গ্রন্থি_not_on_optimal_path[poly.start_সূচক + i] = is_inclusion_zone;
	}

	অঙ্ক_nodes_2 += kCircleApproxVertices;
	++অঙ্ক_polygons;
	ফেরত Add_ফলাফল::Success;
}

শূন্য PlannerPolygons::computeBoundingBox(ধ্রুবক পূর্ণসংখ্যা start_সূচক, ধ্রুবক পূর্ণসংখ্যা অঙ্ক_vertices,
		int32_t &min_x, int32_t &max_x, int32_t &min_y, int32_t &max_y)
{

	// Store the bounding box, so we can do a very cheap check against the
	// bounding box, and only do the full line-polygon intersection check if
	// the line intersects the bounding box.

	min_x = max_x = _x_cm[start_সূচক];
	min_y = max_y = _y_cm[start_সূচক];

	জন্য (পূর্ণসংখ্যা i = 1; i < অঙ্ক_vertices; i++) {
		ধ্রুবক int32_t x = _x_cm[start_সূচক + i];
		ধ্রুবক int32_t y = _y_cm[start_সূচক + i];

		যদি (x < min_x) { min_x = x; }

		যদি (x > max_x) { max_x = x; }

		যদি (y < min_y) { min_y = y; }

		যদি (y > max_y) { max_y = y; }
	}
}

যৌক্তিক PlannerPolygons::pointInsideInteriorCone(ধ্রুবক PolygonInfo &poly,
		int32_t px, int32_t py, পূর্ণসংখ্যা v) ধ্রুবক
{
	ধ্রুবক পূর্ণসংখ্যা পূর্ব = poly.start_সূচক + ((v + poly.অঙ্ক_vertices - 1) % poly.অঙ্ক_vertices);
	ধ্রুবক পূর্ণসংখ্যা curr = poly.start_সূচক + v;
	ধ্রুবক পূর্ণসংখ্যা পরবর্তী = poly.start_সূচক + ((v + 1) % poly.অঙ্ক_vertices);

	ধ্রুবক পূর্ণসংখ্যা p_বাম_of_incoming  = orient2d(_x_cm[পূর্ব], _y_cm[পূর্ব], _x_cm[curr], _y_cm[curr], px, py);
	ধ্রুবক পূর্ণসংখ্যা p_বাম_of_outgoing  = orient2d(_x_cm[curr], _y_cm[curr], _x_cm[পরবর্তী], _y_cm[পরবর্তী], px, py);
	ধ্রুবক পূর্ণসংখ্যা is_convex           = orient2d(_x_cm[পূর্ব], _y_cm[পূর্ব], _x_cm[curr], _y_cm[curr], _x_cm[পরবর্তী], _y_cm[পরবর্তী]);

	ফেরত is_convex > 0  // If convex==0 (exact 180 deg "corner") the two cases are equivalent
	       ? (p_বাম_of_incoming > 0 && p_বাম_of_outgoing > 0)   // Convex: interior = intersection of two half-planes
	       : (p_বাম_of_incoming > 0 || p_বাম_of_outgoing > 0);  // Reflex: interior = union of two half-planes
}

যৌক্তিক PlannerPolygons::intersectsInsideOf(ধ্রুবক PolygonInfo &poly,
		int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ধ্রুবক
{
	// Single pass over polygon edges, doing two jobs:
	//  (1) classify the test segment vs each polygon edge. early-return on:
	//       - strict crossings (line segment - polygon edge)
	//       - interior crossing at vertex
	//  (2) accumulate midpoint winding contribution -- if (1) is inconclusive,
	//      this decides if the line is inside or outside

	পূর্ণসংখ্যা wn = 0;
	যৌক্তিক mid_on_boundary = মিথ্যা;

	// Rather than calculating the midpoint (mid = (s + e)/2) explicitly,
	// which will lead to rounding error for uneven numbers, we instead
	// scale all other inputs by two, reducing the range by a factor of two
	// (2^31 cm = 21400 km -> 2^30 cm = 10700 km)
	ধ্রুবক int32_t twice_mid_x = s_x + e_x;
	ধ্রুবক int32_t twice_mid_y = s_y + e_y;

	জন্য (পূর্ণসংখ্যা i = 0; i < poly.অঙ্ক_vertices; i++) {

		ধ্রুবক পূর্ণসংখ্যা পূর্ব_idx = poly.start_সূচক + (i + poly.অঙ্ক_vertices - 1) % poly.অঙ্ক_vertices;
		ধ্রুবক পূর্ণসংখ্যা curr_idx = poly.start_সূচক + i;

		ধ্রুবক int32_t ax = _x_cm[পূর্ব_idx], ay = _y_cm[পূর্ব_idx];
		ধ্রুবক int32_t bx = _x_cm[curr_idx], by = _y_cm[curr_idx];

		// (1) test segment vs polygon edge (a, b)
		নির্বাচন (segmentsIntersect(ax, ay, bx, by, s_x, s_y, e_x, e_y)) {
		ক্ষেত্র Seg_Seg_ফলাফল::Cross:
			ফেরত সত্য;

		// skip AInsideCD - a from this segment is b from the adjacent segment
		ক্ষেত্র Seg_Seg_ফলাফল::BInsideCD:

			// Polygon vertex b is exactly on the interior of the test segment.
			// Return intersecting if it pokes into the polygon interior through vertex b.
			যদি (pointInsideInteriorCone(poly, s_x, s_y, i) || pointInsideInteriorCone(poly, e_x, e_y, i)) {
				ফেরত সত্য;
			}

			বিরতি;

		পূর্বনির্ধারিত:
			বিরতি;
		}

		// (2) midpoint winding contribution
		// Ref: Dan Sunday, Inclusion of a Point in a Polygon
		// https://web.archive.org/web/20130126163405/http://geomalgorithms.com/a03-_inclusion.html

		ধ্রুবক পূর্ণসংখ্যা side = orient2d(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);

		যদি (side == 0) {

			ধ্রুবক যৌক্তিক mid_on_open_ab = collinearBetween(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);
			ধ্রুবক যৌক্তিক mid_is_b = 2 * bx == twice_mid_x && 2 * by == twice_mid_y;

			// Skip point a - a from this segment is b from the adjacent segment
			যদি (mid_on_open_ab || mid_is_b) {
				mid_on_boundary = সত্য;
			}

		} অন্যথা যদি (2 * ay <= twice_mid_y) {
			যদি (2 * by > twice_mid_y && side > 0) { wn++; }

		} অন্যথা যদি (2 * by <= twice_mid_y && side < 0) { wn--; }
	}


	// Midpoint on boundary, segment runs along it (strict crossing detected earlier).
	// Non-intersecting in either zone.
	যদি (mid_on_boundary) {
		ফেরত মিথ্যা;
	}

	// Exclusion: violation when midpoint is inside (bounded). Inclusion: when outside (unbounded).
	// This is the only place we depart from the inside = left = forbidden canonical orientation,
	// necessary because of the point membership check.
	ধ্রুবক যৌক্তিক midpoint_in_bounded_inside = (wn != 0);
	ফেরত midpoint_in_bounded_inside != poly.is_inclusion;
}

যৌক্তিক PlannerPolygons::intersectsAnyInside(int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ধ্রুবক
{
	ধ্রুবক int32_t seg_min_x = s_x < e_x ? s_x : e_x;
	ধ্রুবক int32_t seg_max_x = s_x > e_x ? s_x : e_x;
	ধ্রুবক int32_t seg_min_y = s_y < e_y ? s_y : e_y;
	ধ্রুবক int32_t seg_max_y = s_y > e_y ? s_y : e_y;

	জন্য (পূর্ণসংখ্যা p = 0; p < অঙ্ক_polygons; p++) {
		ধ্রুবক PolygonInfo &poly = _polygons[p];

		যদি (seg_max_x < poly.min_x || seg_min_x > poly.max_x ||
		    seg_max_y < poly.min_y || seg_min_y > poly.max_y) {
			// Segment is entirely outside this polygon.
			// For exclusion zones: outside = safe, skip.
			// For inclusion zones: outside = forbidden region, immediate violation.
			যদি (poly.is_inclusion) { ফেরত সত্য; }

			চালু;
		}

		যদি (intersectsInsideOf(poly, s_x, s_y, e_x, e_y)) {
			ফেরত সত্য;
		}
	}

	ফেরত মিথ্যা;
}

যৌক্তিক PlannerPolygons::edgeBitangent(পূর্ণসংখ্যা a, int32_t bx, int32_t by) ধ্রুবক
{
	// Find the polygon owning vertex index a. Indices not belonging to any
	// polygon (in particular the destination at index 0) are trivially bitangent.

	যদি (a == dest_সূচক()) { ফেরত সত্য; }

	জন্য (পূর্ণসংখ্যা p = 0; p < অঙ্ক_polygons; p++) {
		ধ্রুবক PolygonInfo &poly = _polygons[p];

		যদি (a >= poly.start_সূচক && a < poly.start_সূচক + poly.অঙ্ক_vertices) {
			// Define point c = a + (a-b) = 2a - b, so that ac = ab mirrored about a
			// Then: Edge is bitangent at a <=> neither ab nor ac pokes into the interior
			int32_t cx = 2 * _x_cm[a] - bx;
			int32_t cy = 2 * _y_cm[a] - by;
			ফেরত !pointInsideInteriorCone(poly, bx, by, a - poly.start_সূচক)
			       && !pointInsideInteriorCone(poly, cx, cy, a - poly.start_সূচক);
		}
	}

	ফেরত সত্য;
}

যৌক্তিক PlannerPolygons::edgeBitangent(পূর্ণসংখ্যা a, পূর্ণসংখ্যা b) ধ্রুবক
{
	ফেরত edgeBitangent(a, _x_cm[b], _y_cm[b]);
}

যৌক্তিক PlannerPolygons::edgeBitangent(পূর্ণসংখ্যা a, ধ্রুবক matrix::Vector2f &b) ধ্রুবক
{
	ফেরত edgeBitangent(a, metersToCm(b(0)), metersToCm(b(1)));
}

যৌক্তিক PlannerPolygons::setDestination(ধ্রুবক matrix::Vector2f &p)
{
	set_গ্রন্থি(dest_সূচক(), p);

	ধ্রুবক int32_t x_cm = metersToCm(p(0));
	ধ্রুবক int32_t y_cm = metersToCm(p(1));

	যদি (intersectsAnyInside(x_cm, y_cm, x_cm, y_cm)) {
		// If the destination breaches a geofence, still use it but
		// signal to the user that it is invalid.
		ফেরত মিথ্যা;
	}

	ফেরত সত্য;
}

matrix::Vector2f PlannerPolygons::getDestination() ধ্রুবক
{
	যদি (_max_nodes < 1) {
		ফেরত matrix::Vector2f{NAN, NAN};
	}

	ফেরত গ্রন্থি_2(dest_সূচক());
}

যৌক্তিক PlannerPolygons::edgeVisible(পূর্ণসংখ্যা a, পূর্ণসংখ্যা b) ধ্রুবক
{
	যদি (a == b) { ফেরত মিথ্যা; }

	যদি (গ্রন্থি_not_on_optimal_path[a] || গ্রন্থি_not_on_optimal_path[b]) { ফেরত মিথ্যা; }

	// Only bitangent edges can be on a shortest path; this check also cheaply
	// discards edges that violate the polygon at the vertex they come from.
	যদি (!edgeBitangent(a, b) || !edgeBitangent(b, a)) { ফেরত মিথ্যা; }

	ফেরত !intersectsAnyInside(_x_cm[a], _y_cm[a], _x_cm[b], _y_cm[b]);
}

যৌক্তিক PlannerPolygons::edgeVisible(ধ্রুবক matrix::Vector2f &a, পূর্ণসংখ্যা b) ধ্রুবক
{
	যদি (গ্রন্থি_not_on_optimal_path[b]) { ফেরত মিথ্যা; }

	যদি (!edgeBitangent(b, a)) { ফেরত মিথ্যা; }

	ফেরত !intersectsAnyInside(metersToCm(a(0)), metersToCm(a(1)), _x_cm[b], _y_cm[b]);
}

যৌক্তিক PlannerPolygons::edgeVisible(ধ্রুবক matrix::Vector2f &a, ধ্রুবক matrix::Vector2f &b) ধ্রুবক
{
	ফেরত !intersectsAnyInside(metersToCm(a(0)), metersToCm(a(1)), metersToCm(b(0)), metersToCm(b(1)));
}

দশমিক PlannerPolygons::edgeCost(পূর্ণসংখ্যা a, পূর্ণসংখ্যা b) ধ্রুবক
{
	ফেরত edgeVisible(a, b) ? (গ্রন্থি_2(a) - গ্রন্থি_2(b)).norm() : INFINITY;
}


যৌক্তিক segmentsIntersectInclusive(ধ্রুবক matrix::Vector2d &a, ধ্রুবক matrix::Vector2d &b,
				ধ্রুবক matrix::Vector2d &c, ধ্রুবক matrix::Vector2d &d)
{
	// Reject separated bounds, including gaps between collinear segments.
	জন্য (পূর্ণসংখ্যা axis = 0; axis < 2; ++axis) {
		যদি (math::max(a(axis), b(axis)) < math::min(c(axis), d(axis))
		    || math::max(c(axis), d(axis)) < math::min(a(axis), b(axis))) {
			ফেরত মিথ্যা;
		}
	}

	ধ্রুবক matrix::Vector2d ab = b - a;
	ধ্রুবক matrix::Vector2d cd = d - c;
	ধ্রুবক দ্বিগুণ side_c = ab.cross(c - a);
	ধ্রুবক দ্বিগুণ side_d = ab.cross(d - a);
	ধ্রুবক দ্বিগুণ side_a = cd.cross(a - c);
	ধ্রুবক দ্বিগুণ side_b = cd.cross(b - c);

	// Each segment must cross or touch the line through the other segment.
	ফেরত ((side_c <= 0.0 && side_d >= 0.0) || (side_c >= 0.0 && side_d <= 0.0))
	       && ((side_a <= 0.0 && side_b >= 0.0) || (side_a >= 0.0 && side_b <= 0.0));
}

দ্বিগুণ pointToSegmentDistanceSquared(ধ্রুবক matrix::Vector2d &point,
				     ধ্রুবক matrix::Vector2d &a, ধ্রুবক matrix::Vector2d &b)
{
	ধ্রুবক matrix::Vector2d direction = b - a;
	ধ্রুবক matrix::Vector2d to_point = point - a;
	ধ্রুবক দ্বিগুণ দৈর্ঘ্য_squared = direction.norm_squared();
	// along / length_squared locates the projection: 0 at a, 1 at b.
	ধ্রুবক দ্বিগুণ along = to_point.dot(direction);

	যদি (along <= 0.0) {
		ফেরত to_point.norm_squared();

	} অন্যথা যদি (along >= দৈর্ঘ্য_squared) {
		ফেরত (point - b).norm_squared();
	}

	// Squared distance to the line: |to_point x direction|^2 / |direction|^2.
	// https://mathworld.wolfram.com/2-DimensionalPoint-LineDistance.html
	ধ্রুবক দ্বিগুণ cross = to_point.cross(direction);
	ফেরত cross * cross / দৈর্ঘ্য_squared;
}

যৌক্তিক isPolygonCCW(ধ্রুবক matrix::Vector2f *vertices, পূর্ণসংখ্যা অঙ্ক_vertices)
{
	// https://en.wikipedia.org/wiki/Shoelace_formula
	// Triangle formula
	দশমিক signed_area_2x = 0.f;

	জন্য (পূর্ণসংখ্যা i = 0; i < অঙ্ক_vertices; i++) {
		ধ্রুবক পূর্ণসংখ্যা j = (i + 1) % অঙ্ক_vertices;
		signed_area_2x += vertices[i](0) * vertices[j](1)
				  - vertices[j](0) * vertices[i](1);
	}

	ফেরত signed_area_2x > 0.f;
}

} // namespace geofence_utils
