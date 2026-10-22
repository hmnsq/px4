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

int32_t meters_منزل_Cm(عائم m) { أرجع roundf(m * CM_في_M); }

} // namespace


منطقي PlannerPolygons::allocate(صحيح max_nodes)
{
	freeBuffers();

	إذا (max_nodes > 0) {
		_x_cm = جديد int32_t[max_nodes];
		_y_cm = جديد int32_t[max_nodes];
		ڳنڍ_not_on_optimal_رستو = جديد منطقي[max_nodes];

		إذا (_x_cm != خالي_اشارو && _y_cm != خالي_اشارو && ڳنڍ_not_on_optimal_رستو != خالي_اشارو) {
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
	ميٽيو[] _x_cm;
	ميٽيو[] _y_cm;
	ميٽيو[] ڳنڍ_not_on_optimal_رستو;
	_x_cm = خالي_اشارو;
	_y_cm = خالي_اشارو;
	ڳنڍ_not_on_optimal_رستو = خالي_اشارو;
	_max_nodes = 0;
}

فراغ PlannerPolygons::سيٽ_ڳنڍ(صحيح idx, ثابت ميٽرڪس::Vector2f &p)
{
	إذا (_max_nodes < 1) {
		أرجع;
	}

	idx = math::constrain(idx, 0, _max_nodes - 1);

	_x_cm[idx] = meters_منزل_Cm(p(0));
	_y_cm[idx] = meters_منزل_Cm(p(1));
}

PlannerPolygons::جوڙيو_نتيجو PlannerPolygons::جوڙيو_Polygon(ثابت ميٽرڪس::Vector2f *vertices_اندريون, صحيح انگ_vertices,
		منطقي is_inclusion_zone, عائم margin)
{
	إذا (انگ_polygons >= kMaxPolygons || انگ_nodes_2 + انگ_vertices > _max_nodes) {
		أرجع جوڙيو_نتيجو::BudgetExceeded;
	}

	إذا (margin < -FLT_EPSILON || انگ_vertices < 3) { أرجع جوڙيو_نتيجو::Degenerate; }

	لكل (صحيح i = 0; i < انگ_vertices; ++i) {
		// Reject self-intersecting (non-simple) polygons.
		ثابت ميٽرڪس::Vector2f &a = vertices_اندريون[i];
		ثابت ميٽرڪس::Vector2f &b = vertices_اندريون[(i + 1) % انگ_vertices];

		لكل (صحيح j = i + 2; j < انگ_vertices; ++j) {
			إذا (i == 0 && j == انگ_vertices - 1) {
				استمر; // (last edge, first edge) are adjacent
			}

			ثابت ميٽرڪس::Vector2f &c = vertices_اندريون[j];
			ثابت ميٽرڪس::Vector2f &d = vertices_اندريون[(j + 1) % انگ_vertices];

			إذا (segmentsIntersect(
				    meters_منزل_Cm(a(0)), meters_منزل_Cm(a(1)), meters_منزل_Cm(b(0)), meters_منزل_Cm(b(1)),
				    meters_منزل_Cm(c(0)), meters_منزل_Cm(c(1)), meters_منزل_Cm(d(0)), meters_منزل_Cm(d(1))) != Seg_Seg_نتيجو::Disjoint) {
				أرجع جوڙيو_نتيجو::Degenerate;
			}
		}

		// Reject vertices outside the usable fixed-point range
		إذا (!اندريون_Fixed_نقطو_Range(vertices_اندريون[i](0)) || !اندريون_Fixed_نقطو_Range(vertices_اندريون[i](1))) {
			أرجع جوڙيو_نتيجو::ٻاهريون_جو_Range;
		}
	}

	// Canonical orientation: walking vertices in stored order, INSIDE is on
	// the left of each edge. CCW for exclusion (bounded interior is forbidden),
	// CW for inclusion (unbounded exterior is forbidden). Reverse iff input
	// doesn't already match.
	ثابت منطقي ابتو = (isPolygonCCW(vertices_اندريون, انگ_vertices) == is_inclusion_zone);

	// Index into vertices_in in stored order (accounts for possible reversal).
	خودڪار راس = [&](صحيح i) -> ثابت ميٽرڪس::Vector2f & {
		أرجع vertices_اندريون[ابتو ? (انگ_vertices - 1 - i) : i];
	};

	PolygonInfo &poly = _polygons[انگ_polygons];
	poly.شروعات_اشاريو  = انگ_nodes_2;
	poly.is_inclusion = is_inclusion_zone;

	صحيح ٻاهريون_idx = 0;

	لكل (صحيح i = 0; i < انگ_vertices; i++) {
		ثابت صحيح prev = (i + انگ_vertices - 1) % انگ_vertices;
		ثابت صحيح ايندڙ = (i + 1) % انگ_vertices;

		ثابت ميٽرڪس::Vector2f ڪنارو_اندريون  = راس(i) - راس(prev);
		ثابت ميٽرڪس::Vector2f ڪنارو_ٻاهريون = راس(ايندڙ) - راس(i);

		ثابت عائم ڪنارو_اندريون_norm = ڪنارو_اندريون.norm();
		ثابت عائم ڪنارو_ٻاهريون_norm = ڪنارو_ٻاهريون.norm();

		إذا (ڪنارو_اندريون_norm < FLT_EPSILON || ڪنارو_ٻاهريون_norm < FLT_EPSILON) {
			أرجع جوڙيو_نتيجو::Degenerate;
		}

		ثابت ميٽرڪس::Vector2f ڪنارو_اندريون_normalized = ڪنارو_اندريون / ڪنارو_اندريون_norm;
		ثابت ميٽرڪس::Vector2f ڪنارو_ٻاهريون_normalized = ڪنارو_ٻاهريون / ڪنارو_ٻاهريون_norm;

		// Inward normals of the two adjacent edges.
		// Canonical stored orientation: left side = forbidden region, right side = allowed region
		ثابت ميٽرڪس::Vector2f n_اندريون  = ميٽرڪس::Vector2f{-ڪنارو_اندريون_normalized(1), ڪنارو_اندريون_normalized(0)};
		ثابت ميٽرڪس::Vector2f n_ٻاهريون = ميٽرڪس::Vector2f{-ڪنارو_ٻاهريون_normalized(1), ڪنارو_ٻاهريون_normalized(0)};

		ميٽرڪس::Vector2f bisector = n_اندريون + n_ٻاهريون;
		ثابت عائم bisector_len  = bisector.ڊيگهائي();

		إذا (bisector_len < FLT_EPSILON) {
			أرجع جوڙيو_نتيجو::Degenerate; // antiparallel edges - polygon doubles back
		}

		ثابت ميٽرڪس::Vector2f normalized_bisector = bisector / bisector_len;

		// The following bit ensures that:
		//  - The original edges are moved away from the forbidden interior by `margin`
		//  - Sharp convex corners (<60deg) are "cut off" with a new edge that is exactly
		//    `margin` away from the original vertex. For this the vertex is replaced by
		//    two new ones.

		// Only split convex corners - others are ignored, see _node_not_on_optimal_path
		ثابت منطقي ڪنڊ_convex = ڪنارو_اندريون_normalized(0) * ڪنارو_ٻاهريون_normalized(1)
					   - ڪنارو_اندريون_normalized(1) * ڪنارو_ٻاهريون_normalized(0) > 0.f;

		// Scaling needed wrt the normalised bisector to achieve parallel edges
		ثابت عائم وٿي_scale = 2.f / bisector_len;

		// bisector_len = 2*sin(interior angle/2); split when interior angle < 60 deg <=> bisector_len < 1.
		ثابت منطقي angle_sharp = bisector_len < 1;

		// If margin zero, do not split - would output the same vertex twice.
		ثابت منطقي margin_nonzero = margin > FLT_EPSILON;

		// If we do not have enough space, do not split the vertex.
		// This never happens if the planner-internal node buffer is 2x the original buffer.
		ثابت منطقي space_for_ورهايو_vertices = انگ_nodes_2 + ٻاهريون_idx + 1 < _max_nodes;

		إذا (ڪنڊ_convex && angle_sharp && margin_nonzero && space_for_ورهايو_vertices) {

			// Sharp corner: cut it off along the line
			//  - perpendicular to normalized_bisector
			//  - going through base := vertex(i) - normalized_bisector * margin

			ثابت ميٽرڪس::Vector2f base = راس(i) - normalized_bisector * margin;
			ثابت ميٽرڪس::Vector2f perp = {-normalized_bisector(1), normalized_bisector(0)};

			// Construct s such that:
			//  base + perp*s is on the offset incoming edge
			//  base - perp*s is on the offset outgoing edge
			ثابت عائم e_ٻاهريون_dot_b = ڪنارو_ٻاهريون_normalized.dot(normalized_bisector);
			ثابت عائم s = margin * (1.f - n_ٻاهريون.dot(normalized_bisector)) / e_ٻاهريون_dot_b;

			سيٽ_ڳنڍ(poly.شروعات_اشاريو + ٻاهريون_idx++, base + perp * s);
			سيٽ_ڳنڍ(poly.شروعات_اشاريو + ٻاهريون_idx++, base - perp * s);

		} وإلا {
			// Regular corner or no space for split: single bisector vertex displaced by margin / sin(interior angle/2).
			سيٽ_ڳنڍ(poly.شروعات_اشاريو + ٻاهريون_idx++, راس(i) - normalized_bisector * (margin * وٿي_scale));
		}
	}

	ثابت صحيح انگ_ٻاهريون = ٻاهريون_idx;
	poly.انگ_vertices = انگ_ٻاهريون;

	computeBoundingBox(poly.شروعات_اشاريو, انگ_ٻاهريون, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// In canonical orientation, the node is not possibly on the shortest
	// path if the vertex curves right or straight (forbidden region >= 180
	// deg)
	لكل (صحيح i = 0; i < انگ_ٻاهريون; i++) {
		ثابت صحيح prev = poly.شروعات_اشاريو + (i + انگ_ٻاهريون - 1) % انگ_ٻاهريون;
		ثابت صحيح curr = poly.شروعات_اشاريو + i;
		ثابت صحيح ايندڙ = poly.شروعات_اشاريو + (i + 1) % انگ_ٻاهريون;
		ثابت منطقي curves_ساڄو_or_straight = orient2d(_x_cm[prev], _y_cm[prev],
						      _x_cm[curr], _y_cm[curr],
						      _x_cm[ايندڙ], _y_cm[ايندڙ]) <= 0;
		ڳنڍ_not_on_optimal_رستو[curr] = curves_ساڄو_or_straight;
	}

	انگ_nodes_2 += انگ_ٻاهريون;
	++انگ_polygons;
	أرجع جوڙيو_نتيجو::Success;
}

PlannerPolygons::جوڙيو_نتيجو PlannerPolygons::جوڙيو_Approx_Circle(ثابت ميٽرڪس::Vector2f &center, ثابت عائم circle_radius,
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

	إذا (انگ_polygons >= kMaxPolygons || انگ_nodes_2 + kCircleApproxVertices > _max_nodes) {
		أرجع جوڙيو_نتيجو::BudgetExceeded;
	}

	إذا (circle_radius <= 0.1f) { أرجع جوڙيو_نتيجو::Degenerate; }

	// ensures that the k-gon is, w.r.t the real circle:
	//  - over-approximation of circle grown by margin, if exclusion
	//  - under-approximation of circle shrunk by margin, if inclusion
	ثابت عائم k_gon_radius = is_inclusion_zone
				   ? circle_radius - margin
				   : circle_radius / cosf(M_PI_F / kCircleApproxVertices) + margin;

	إذا (k_gon_radius <= FLT_EPSILON) { // Negative radius - circle empty
		أرجع جوڙيو_نتيجو::Degenerate;
	}

	إذا (!اندريون_Fixed_نقطو_Range(fabsf(center(0)) + k_gon_radius)
	    || !اندريون_Fixed_نقطو_Range(fabsf(center(1)) + k_gon_radius)) {
		أرجع جوڙيو_نتيجو::ٻاهريون_جو_Range;
	}

	// Canonical orientation: CCW for exclusion, CW for inclusion
	ثابت عائم delta_angle = (is_inclusion_zone ? -1.f : 1.f) * 2.f * M_PI_F / kCircleApproxVertices;

	PolygonInfo &poly = _polygons[انگ_polygons];
	poly.شروعات_اشاريو = انگ_nodes_2;
	poly.انگ_vertices = kCircleApproxVertices;
	poly.is_inclusion = is_inclusion_zone;

	لكل (صحيح i = 0; i < kCircleApproxVertices; i++) {
		ثابت عائم angle = (i + 0.5f) * delta_angle;
		ثابت ميٽرڪس::Vector2f p = center + ميٽرڪس::Vector2f{
			k_gon_radius * cosf(angle), k_gon_radius * sinf(angle)
		};
		سيٽ_ڳنڍ(poly.شروعات_اشاريو + i, p);
	}

	computeBoundingBox(poly.شروعات_اشاريو, kCircleApproxVertices, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// For regular k-gons, all corners are either convex or reflex based on
	// is_inclusion, so the reduced visibility graph criterion is simplified
	لكل (صحيح i = 0; i < kCircleApproxVertices; i++) {
		ڳنڍ_not_on_optimal_رستو[poly.شروعات_اشاريو + i] = is_inclusion_zone;
	}

	انگ_nodes_2 += kCircleApproxVertices;
	++انگ_polygons;
	أرجع جوڙيو_نتيجو::Success;
}

فراغ PlannerPolygons::computeBoundingBox(ثابت صحيح شروعات_اشاريو, ثابت صحيح انگ_vertices,
		int32_t &min_x, int32_t &max_x, int32_t &min_y, int32_t &max_y)
{

	// Store the bounding box, so we can do a very cheap check against the
	// bounding box, and only do the full line-polygon intersection check if
	// the line intersects the bounding box.

	min_x = max_x = _x_cm[شروعات_اشاريو];
	min_y = max_y = _y_cm[شروعات_اشاريو];

	لكل (صحيح i = 1; i < انگ_vertices; i++) {
		ثابت int32_t x = _x_cm[شروعات_اشاريو + i];
		ثابت int32_t y = _y_cm[شروعات_اشاريو + i];

		إذا (x < min_x) { min_x = x; }

		إذا (x > max_x) { max_x = x; }

		إذا (y < min_y) { min_y = y; }

		إذا (y > max_y) { max_y = y; }
	}
}

منطقي PlannerPolygons::نقطو_Inside_Interior_Cone(ثابت PolygonInfo &poly,
		int32_t px, int32_t py, صحيح v) ثابت
{
	ثابت صحيح prev = poly.شروعات_اشاريو + ((v + poly.انگ_vertices - 1) % poly.انگ_vertices);
	ثابت صحيح curr = poly.شروعات_اشاريو + v;
	ثابت صحيح ايندڙ = poly.شروعات_اشاريو + ((v + 1) % poly.انگ_vertices);

	ثابت صحيح p_کاٻو_جو_incoming  = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], px, py);
	ثابت صحيح p_کاٻو_جو_outgoing  = orient2d(_x_cm[curr], _y_cm[curr], _x_cm[ايندڙ], _y_cm[ايندڙ], px, py);
	ثابت صحيح is_convex           = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], _x_cm[ايندڙ], _y_cm[ايندڙ]);

	أرجع is_convex > 0  // If convex==0 (exact 180 deg "corner") the two cases are equivalent
	       ? (p_کاٻو_جو_incoming > 0 && p_کاٻو_جو_outgoing > 0)   // Convex: interior = intersection of two half-planes
	       : (p_کاٻو_جو_incoming > 0 || p_کاٻو_جو_outgoing > 0);  // Reflex: interior = union of two half-planes
}

منطقي PlannerPolygons::intersects_Inside_جو(ثابت PolygonInfo &poly,
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

	لكل (صحيح i = 0; i < poly.انگ_vertices; i++) {

		ثابت صحيح prev_idx = poly.شروعات_اشاريو + (i + poly.انگ_vertices - 1) % poly.انگ_vertices;
		ثابت صحيح curr_idx = poly.شروعات_اشاريو + i;

		ثابت int32_t ax = _x_cm[prev_idx], ay = _y_cm[prev_idx];
		ثابت int32_t bx = _x_cm[curr_idx], by = _y_cm[curr_idx];

		// (1) test segment vs polygon edge (a, b)
		بدل (segmentsIntersect(ax, ay, bx, by, s_x, s_y, e_x, e_y)) {
		حالة Seg_Seg_نتيجو::Cross:
			أرجع سچ;

		// skip AInsideCD - a from this segment is b from the adjacent segment
		حالة Seg_Seg_نتيجو::BInsideCD:

			// Polygon vertex b is exactly on the interior of the test segment.
			// Return intersecting if it pokes into the polygon interior through vertex b.
			إذا (نقطو_Inside_Interior_Cone(poly, s_x, s_y, i) || نقطو_Inside_Interior_Cone(poly, e_x, e_y, i)) {
				أرجع سچ;
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
				mid_on_boundary = سچ;
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
	ثابت منطقي midpoint_اندريون_bounded_inside = (wn != 0);
	أرجع midpoint_اندريون_bounded_inside != poly.is_inclusion;
}

منطقي PlannerPolygons::intersectsAnyInside(int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ثابت
{
	ثابت int32_t seg_min_x = s_x < e_x ? s_x : e_x;
	ثابت int32_t seg_max_x = s_x > e_x ? s_x : e_x;
	ثابت int32_t seg_min_y = s_y < e_y ? s_y : e_y;
	ثابت int32_t seg_max_y = s_y > e_y ? s_y : e_y;

	لكل (صحيح p = 0; p < انگ_polygons; p++) {
		ثابت PolygonInfo &poly = _polygons[p];

		إذا (seg_max_x < poly.min_x || seg_min_x > poly.max_x ||
		    seg_max_y < poly.min_y || seg_min_y > poly.max_y) {
			// Segment is entirely outside this polygon.
			// For exclusion zones: outside = safe, skip.
			// For inclusion zones: outside = forbidden region, immediate violation.
			إذا (poly.is_inclusion) { أرجع سچ; }

			استمر;
		}

		إذا (intersects_Inside_جو(poly, s_x, s_y, e_x, e_y)) {
			أرجع سچ;
		}
	}

	أرجع خطأ;
}

منطقي PlannerPolygons::ڪنارو_Bitangent(صحيح a, int32_t bx, int32_t by) ثابت
{
	// Find the polygon owning vertex index a. Indices not belonging to any
	// polygon (in particular the destination at index 0) are trivially bitangent.

	إذا (a == dest_اشاريو()) { أرجع سچ; }

	لكل (صحيح p = 0; p < انگ_polygons; p++) {
		ثابت PolygonInfo &poly = _polygons[p];

		إذا (a >= poly.شروعات_اشاريو && a < poly.شروعات_اشاريو + poly.انگ_vertices) {
			// Define point c = a + (a-b) = 2a - b, so that ac = ab mirrored about a
			// Then: Edge is bitangent at a <=> neither ab nor ac pokes into the interior
			int32_t cx = 2 * _x_cm[a] - bx;
			int32_t cy = 2 * _y_cm[a] - by;
			أرجع !نقطو_Inside_Interior_Cone(poly, bx, by, a - poly.شروعات_اشاريو)
			       && !نقطو_Inside_Interior_Cone(poly, cx, cy, a - poly.شروعات_اشاريو);
		}
	}

	أرجع سچ;
}

منطقي PlannerPolygons::ڪنارو_Bitangent(صحيح a, صحيح b) ثابت
{
	أرجع ڪنارو_Bitangent(a, _x_cm[b], _y_cm[b]);
}

منطقي PlannerPolygons::ڪنارو_Bitangent(صحيح a, ثابت ميٽرڪس::Vector2f &b) ثابت
{
	أرجع ڪنارو_Bitangent(a, meters_منزل_Cm(b(0)), meters_منزل_Cm(b(1)));
}

منطقي PlannerPolygons::سيٽ_Destination(ثابت ميٽرڪس::Vector2f &p)
{
	سيٽ_ڳنڍ(dest_اشاريو(), p);

	ثابت int32_t x_cm = meters_منزل_Cm(p(0));
	ثابت int32_t y_cm = meters_منزل_Cm(p(1));

	إذا (intersectsAnyInside(x_cm, y_cm, x_cm, y_cm)) {
		// If the destination breaches a geofence, still use it but
		// signal to the user that it is invalid.
		أرجع خطأ;
	}

	أرجع سچ;
}

ميٽرڪس::Vector2f PlannerPolygons::getDestination() ثابت
{
	إذا (_max_nodes < 1) {
		أرجع ميٽرڪس::Vector2f{NAN, NAN};
	}

	أرجع ڳنڍ_2(dest_اشاريو());
}

منطقي PlannerPolygons::ڪنارو_Visible(صحيح a, صحيح b) ثابت
{
	إذا (a == b) { أرجع خطأ; }

	إذا (ڳنڍ_not_on_optimal_رستو[a] || ڳنڍ_not_on_optimal_رستو[b]) { أرجع خطأ; }

	// Only bitangent edges can be on a shortest path; this check also cheaply
	// discards edges that violate the polygon at the vertex they come from.
	إذا (!ڪنارو_Bitangent(a, b) || !ڪنارو_Bitangent(b, a)) { أرجع خطأ; }

	أرجع !intersectsAnyInside(_x_cm[a], _y_cm[a], _x_cm[b], _y_cm[b]);
}

منطقي PlannerPolygons::ڪنارو_Visible(ثابت ميٽرڪس::Vector2f &a, صحيح b) ثابت
{
	إذا (ڳنڍ_not_on_optimal_رستو[b]) { أرجع خطأ; }

	إذا (!ڪنارو_Bitangent(b, a)) { أرجع خطأ; }

	أرجع !intersectsAnyInside(meters_منزل_Cm(a(0)), meters_منزل_Cm(a(1)), _x_cm[b], _y_cm[b]);
}

منطقي PlannerPolygons::ڪنارو_Visible(ثابت ميٽرڪس::Vector2f &a, ثابت ميٽرڪس::Vector2f &b) ثابت
{
	أرجع !intersectsAnyInside(meters_منزل_Cm(a(0)), meters_منزل_Cm(a(1)), meters_منزل_Cm(b(0)), meters_منزل_Cm(b(1)));
}

عائم PlannerPolygons::ڪنارو_لاڳت(صحيح a, صحيح b) ثابت
{
	أرجع ڪنارو_Visible(a, b) ? (ڳنڍ_2(a) - ڳنڍ_2(b)).norm() : INFINITY;
}


منطقي segmentsIntersectInclusive(ثابت ميٽرڪس::Vector2d &a, ثابت ميٽرڪس::Vector2d &b,
				ثابت ميٽرڪس::Vector2d &c, ثابت ميٽرڪس::Vector2d &d)
{
	// Reject separated bounds, including gaps between collinear segments.
	لكل (صحيح axis = 0; axis < 2; ++axis) {
		إذا (math::max(a(axis), b(axis)) < math::min(c(axis), d(axis))
		    || math::max(c(axis), d(axis)) < math::min(a(axis), b(axis))) {
			أرجع خطأ;
		}
	}

	ثابت ميٽرڪس::Vector2d ab = b - a;
	ثابت ميٽرڪس::Vector2d cd = d - c;
	ثابت مزدوج side_c = ab.cross(c - a);
	ثابت مزدوج side_d = ab.cross(d - a);
	ثابت مزدوج side_a = cd.cross(a - c);
	ثابت مزدوج side_b = cd.cross(b - c);

	// Each segment must cross or touch the line through the other segment.
	أرجع ((side_c <= 0.0 && side_d >= 0.0) || (side_c >= 0.0 && side_d <= 0.0))
	       && ((side_a <= 0.0 && side_b >= 0.0) || (side_a >= 0.0 && side_b <= 0.0));
}

مزدوج نقطو_منزل_Segment_مفاصلو_Squared(ثابت ميٽرڪس::Vector2d &نقطو,
				     ثابت ميٽرڪس::Vector2d &a, ثابت ميٽرڪس::Vector2d &b)
{
	ثابت ميٽرڪس::Vector2d direction = b - a;
	ثابت ميٽرڪس::Vector2d منزل_نقطو = نقطو - a;
	ثابت مزدوج ڊيگهائي_squared = direction.norm_squared();
	// along / length_squared locates the projection: 0 at a, 1 at b.
	ثابت مزدوج along = منزل_نقطو.dot(direction);

	إذا (along <= 0.0) {
		أرجع منزل_نقطو.norm_squared();

	} وإلا إذا (along >= ڊيگهائي_squared) {
		أرجع (نقطو - b).norm_squared();
	}

	// Squared distance to the line: |to_point x direction|^2 / |direction|^2.
	// https://mathworld.wolfram.com/2-DimensionalPoint-LineDistance.html
	ثابت مزدوج cross = منزل_نقطو.cross(direction);
	أرجع cross * cross / ڊيگهائي_squared;
}

منطقي isPolygonCCW(ثابت ميٽرڪس::Vector2f *vertices, صحيح انگ_vertices)
{
	// https://en.wikipedia.org/wiki/Shoelace_formula
	// Triangle formula
	عائم signed_area_2x = 0.f;

	لكل (صحيح i = 0; i < انگ_vertices; i++) {
		ثابت صحيح j = (i + 1) % انگ_vertices;
		signed_area_2x += vertices[i](0) * vertices[j](1)
				  - vertices[j](0) * vertices[i](1);
	}

	أرجع signed_area_2x > 0.f;
}

} // namespace geofence_utils
