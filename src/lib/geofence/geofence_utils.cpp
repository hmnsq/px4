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

نامخلا geofence_utils
{

نامخلا
{

int32_t meters_مقصد_Cm(اعشاری m) { واپس roundf(m * CM_فی_M); }

} // namespace


منطقی PlannerPolygons::allocate(صحیح max_nodes)
{
	freeBuffers();

	اگر (max_nodes > 0) {
		_x_cm = نیا int32_t[max_nodes];
		_y_cm = نیا int32_t[max_nodes];
		گرہ_not_on_optimal_راستہ = نیا منطقی[max_nodes];

		اگر (_x_cm != خالی_اشاریہ && _y_cm != خالی_اشاریہ && گرہ_not_on_optimal_راستہ != خالی_اشاریہ) {
			_max_nodes = max_nodes;

		} ورنہ {
			freeBuffers();
		}
	}

	reset();

	واپس _max_nodes == max_nodes;
}

خالی PlannerPolygons::freeBuffers()
{
	حذف[] _x_cm;
	حذف[] _y_cm;
	حذف[] گرہ_not_on_optimal_راستہ;
	_x_cm = خالی_اشاریہ;
	_y_cm = خالی_اشاریہ;
	گرہ_not_on_optimal_راستہ = خالی_اشاریہ;
	_max_nodes = 0;
}

خالی PlannerPolygons::سیٹ_گرہ(صحیح idx, ثابت میٹرکس::Vector2f &p)
{
	اگر (_max_nodes < 1) {
		واپس;
	}

	idx = math::constrain(idx, 0, _max_nodes - 1);

	_x_cm[idx] = meters_مقصد_Cm(p(0));
	_y_cm[idx] = meters_مقصد_Cm(p(1));
}

PlannerPolygons::جوڑو_نتیجہ PlannerPolygons::جوڑو_Polygon(ثابت میٹرکس::Vector2f *vertices_اندر, صحیح عدد_vertices,
		منطقی is_inclusion_zone, اعشاری margin)
{
	اگر (عدد_polygons >= kMaxPolygons || عدد_nodes_2 + عدد_vertices > _max_nodes) {
		واپس جوڑو_نتیجہ::BudgetExceeded;
	}

	اگر (margin < -FLT_EPSILON || عدد_vertices < 3) { واپس جوڑو_نتیجہ::Degenerate; }

	برائے (صحیح i = 0; i < عدد_vertices; ++i) {
		// Reject self-intersecting (non-simple) polygons.
		ثابت میٹرکس::Vector2f &a = vertices_اندر[i];
		ثابت میٹرکس::Vector2f &b = vertices_اندر[(i + 1) % عدد_vertices];

		برائے (صحیح j = i + 2; j < عدد_vertices; ++j) {
			اگر (i == 0 && j == عدد_vertices - 1) {
				جاری; // (last edge, first edge) are adjacent
			}

			ثابت میٹرکس::Vector2f &c = vertices_اندر[j];
			ثابت میٹرکس::Vector2f &d = vertices_اندر[(j + 1) % عدد_vertices];

			اگر (segmentsIntersect(
				    meters_مقصد_Cm(a(0)), meters_مقصد_Cm(a(1)), meters_مقصد_Cm(b(0)), meters_مقصد_Cm(b(1)),
				    meters_مقصد_Cm(c(0)), meters_مقصد_Cm(c(1)), meters_مقصد_Cm(d(0)), meters_مقصد_Cm(d(1))) != Seg_Seg_نتیجہ::Disjoint) {
				واپس جوڑو_نتیجہ::Degenerate;
			}
		}

		// Reject vertices outside the usable fixed-point range
		اگر (!اندر_Fixed_نقطہ_Range(vertices_اندر[i](0)) || !اندر_Fixed_نقطہ_Range(vertices_اندر[i](1))) {
			واپس جوڑو_نتیجہ::برآمد_کا_Range;
		}
	}

	// Canonical orientation: walking vertices in stored order, INSIDE is on
	// the left of each edge. CCW for exclusion (bounded interior is forbidden),
	// CW for inclusion (unbounded exterior is forbidden). Reverse iff input
	// doesn't already match.
	ثابت منطقی الٹ = (isPolygonCCW(vertices_اندر, عدد_vertices) == is_inclusion_zone);

	// Index into vertices_in in stored order (accounts for possible reversal).
	خودکار راس = [&](صحیح i) -> ثابت میٹرکس::Vector2f & {
		واپس vertices_اندر[الٹ ? (عدد_vertices - 1 - i) : i];
	};

	PolygonInfo &poly = _polygons[عدد_polygons];
	poly.آغاز_اشاریہ  = عدد_nodes_2;
	poly.is_inclusion = is_inclusion_zone;

	صحیح برآمد_idx = 0;

	برائے (صحیح i = 0; i < عدد_vertices; i++) {
		ثابت صحیح پچھلا_2 = (i + عدد_vertices - 1) % عدد_vertices;
		ثابت صحیح اگلا = (i + 1) % عدد_vertices;

		ثابت میٹرکس::Vector2f کنارہ_اندر  = راس(i) - راس(پچھلا_2);
		ثابت میٹرکس::Vector2f کنارہ_برآمد = راس(اگلا) - راس(i);

		ثابت اعشاری کنارہ_اندر_norm = کنارہ_اندر.norm();
		ثابت اعشاری کنارہ_برآمد_norm = کنارہ_برآمد.norm();

		اگر (کنارہ_اندر_norm < FLT_EPSILON || کنارہ_برآمد_norm < FLT_EPSILON) {
			واپس جوڑو_نتیجہ::Degenerate;
		}

		ثابت میٹرکس::Vector2f کنارہ_اندر_normalized = کنارہ_اندر / کنارہ_اندر_norm;
		ثابت میٹرکس::Vector2f کنارہ_برآمد_normalized = کنارہ_برآمد / کنارہ_برآمد_norm;

		// Inward normals of the two adjacent edges.
		// Canonical stored orientation: left side = forbidden region, right side = allowed region
		ثابت میٹرکس::Vector2f n_اندر  = میٹرکس::Vector2f{-کنارہ_اندر_normalized(1), کنارہ_اندر_normalized(0)};
		ثابت میٹرکس::Vector2f n_برآمد = میٹرکس::Vector2f{-کنارہ_برآمد_normalized(1), کنارہ_برآمد_normalized(0)};

		میٹرکس::Vector2f bisector = n_اندر + n_برآمد;
		ثابت اعشاری bisector_len  = bisector.لمبائی_2();

		اگر (bisector_len < FLT_EPSILON) {
			واپس جوڑو_نتیجہ::Degenerate; // antiparallel edges - polygon doubles back
		}

		ثابت میٹرکس::Vector2f normalized_bisector = bisector / bisector_len;

		// The following bit ensures that:
		//  - The original edges are moved away from the forbidden interior by `margin`
		//  - Sharp convex corners (<60deg) are "cut off" with a new edge that is exactly
		//    `margin` away from the original vertex. For this the vertex is replaced by
		//    two new ones.

		// Only split convex corners - others are ignored, see _node_not_on_optimal_path
		ثابت منطقی کونا_convex = کنارہ_اندر_normalized(0) * کنارہ_برآمد_normalized(1)
					   - کنارہ_اندر_normalized(1) * کنارہ_برآمد_normalized(0) > 0.f;

		// Scaling needed wrt the normalised bisector to achieve parallel edges
		ثابت اعشاری انحراف_scale = 2.f / bisector_len;

		// bisector_len = 2*sin(interior angle/2); split when interior angle < 60 deg <=> bisector_len < 1.
		ثابت منطقی angle_sharp = bisector_len < 1;

		// If margin zero, do not split - would output the same vertex twice.
		ثابت منطقی margin_nonzero = margin > FLT_EPSILON;

		// If we do not have enough space, do not split the vertex.
		// This never happens if the planner-internal node buffer is 2x the original buffer.
		ثابت منطقی space_for_بانٹو_vertices = عدد_nodes_2 + برآمد_idx + 1 < _max_nodes;

		اگر (کونا_convex && angle_sharp && margin_nonzero && space_for_بانٹو_vertices) {

			// Sharp corner: cut it off along the line
			//  - perpendicular to normalized_bisector
			//  - going through base := vertex(i) - normalized_bisector * margin

			ثابت میٹرکس::Vector2f base = راس(i) - normalized_bisector * margin;
			ثابت میٹرکس::Vector2f perp = {-normalized_bisector(1), normalized_bisector(0)};

			// Construct s such that:
			//  base + perp*s is on the offset incoming edge
			//  base - perp*s is on the offset outgoing edge
			ثابت اعشاری e_برآمد_dot_b = کنارہ_برآمد_normalized.dot(normalized_bisector);
			ثابت اعشاری s = margin * (1.f - n_برآمد.dot(normalized_bisector)) / e_برآمد_dot_b;

			سیٹ_گرہ(poly.آغاز_اشاریہ + برآمد_idx++, base + perp * s);
			سیٹ_گرہ(poly.آغاز_اشاریہ + برآمد_idx++, base - perp * s);

		} ورنہ {
			// Regular corner or no space for split: single bisector vertex displaced by margin / sin(interior angle/2).
			سیٹ_گرہ(poly.آغاز_اشاریہ + برآمد_idx++, راس(i) - normalized_bisector * (margin * انحراف_scale));
		}
	}

	ثابت صحیح عدد_برآمد = برآمد_idx;
	poly.عدد_vertices = عدد_برآمد;

	computeBoundingBox(poly.آغاز_اشاریہ, عدد_برآمد, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// In canonical orientation, the node is not possibly on the shortest
	// path if the vertex curves right or straight (forbidden region >= 180
	// deg)
	برائے (صحیح i = 0; i < عدد_برآمد; i++) {
		ثابت صحیح پچھلا_2 = poly.آغاز_اشاریہ + (i + عدد_برآمد - 1) % عدد_برآمد;
		ثابت صحیح curr = poly.آغاز_اشاریہ + i;
		ثابت صحیح اگلا = poly.آغاز_اشاریہ + (i + 1) % عدد_برآمد;
		ثابت منطقی curves_دایاں_or_straight = orient2d(_x_cm[پچھلا_2], _y_cm[پچھلا_2],
						      _x_cm[curr], _y_cm[curr],
						      _x_cm[اگلا], _y_cm[اگلا]) <= 0;
		گرہ_not_on_optimal_راستہ[curr] = curves_دایاں_or_straight;
	}

	عدد_nodes_2 += عدد_برآمد;
	++عدد_polygons;
	واپس جوڑو_نتیجہ::Success;
}

PlannerPolygons::جوڑو_نتیجہ PlannerPolygons::جوڑو_Approx_Circle(ثابت میٹرکس::Vector2f &center, ثابت اعشاری circle_radius,
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

	اگر (عدد_polygons >= kMaxPolygons || عدد_nodes_2 + kCircleApproxVertices > _max_nodes) {
		واپس جوڑو_نتیجہ::BudgetExceeded;
	}

	اگر (circle_radius <= 0.1f) { واپس جوڑو_نتیجہ::Degenerate; }

	// ensures that the k-gon is, w.r.t the real circle:
	//  - over-approximation of circle grown by margin, if exclusion
	//  - under-approximation of circle shrunk by margin, if inclusion
	ثابت اعشاری k_gon_radius = is_inclusion_zone
				   ? circle_radius - margin
				   : circle_radius / cosf(M_PI_F / kCircleApproxVertices) + margin;

	اگر (k_gon_radius <= FLT_EPSILON) { // Negative radius - circle empty
		واپس جوڑو_نتیجہ::Degenerate;
	}

	اگر (!اندر_Fixed_نقطہ_Range(fabsf(center(0)) + k_gon_radius)
	    || !اندر_Fixed_نقطہ_Range(fabsf(center(1)) + k_gon_radius)) {
		واپس جوڑو_نتیجہ::برآمد_کا_Range;
	}

	// Canonical orientation: CCW for exclusion, CW for inclusion
	ثابت اعشاری delta_angle = (is_inclusion_zone ? -1.f : 1.f) * 2.f * M_PI_F / kCircleApproxVertices;

	PolygonInfo &poly = _polygons[عدد_polygons];
	poly.آغاز_اشاریہ = عدد_nodes_2;
	poly.عدد_vertices = kCircleApproxVertices;
	poly.is_inclusion = is_inclusion_zone;

	برائے (صحیح i = 0; i < kCircleApproxVertices; i++) {
		ثابت اعشاری angle = (i + 0.5f) * delta_angle;
		ثابت میٹرکس::Vector2f p = center + میٹرکس::Vector2f{
			k_gon_radius * cosf(angle), k_gon_radius * sinf(angle)
		};
		سیٹ_گرہ(poly.آغاز_اشاریہ + i, p);
	}

	computeBoundingBox(poly.آغاز_اشاریہ, kCircleApproxVertices, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// For regular k-gons, all corners are either convex or reflex based on
	// is_inclusion, so the reduced visibility graph criterion is simplified
	برائے (صحیح i = 0; i < kCircleApproxVertices; i++) {
		گرہ_not_on_optimal_راستہ[poly.آغاز_اشاریہ + i] = is_inclusion_zone;
	}

	عدد_nodes_2 += kCircleApproxVertices;
	++عدد_polygons;
	واپس جوڑو_نتیجہ::Success;
}

خالی PlannerPolygons::computeBoundingBox(ثابت صحیح آغاز_اشاریہ, ثابت صحیح عدد_vertices,
		int32_t &min_x, int32_t &max_x, int32_t &min_y, int32_t &max_y)
{

	// Store the bounding box, so we can do a very cheap check against the
	// bounding box, and only do the full line-polygon intersection check if
	// the line intersects the bounding box.

	min_x = max_x = _x_cm[آغاز_اشاریہ];
	min_y = max_y = _y_cm[آغاز_اشاریہ];

	برائے (صحیح i = 1; i < عدد_vertices; i++) {
		ثابت int32_t x = _x_cm[آغاز_اشاریہ + i];
		ثابت int32_t y = _y_cm[آغاز_اشاریہ + i];

		اگر (x < min_x) { min_x = x; }

		اگر (x > max_x) { max_x = x; }

		اگر (y < min_y) { min_y = y; }

		اگر (y > max_y) { max_y = y; }
	}
}

منطقی PlannerPolygons::نقطہ_Inside_Interior_Cone(ثابت PolygonInfo &poly,
		int32_t px, int32_t py, صحیح v) ثابت
{
	ثابت صحیح پچھلا_2 = poly.آغاز_اشاریہ + ((v + poly.عدد_vertices - 1) % poly.عدد_vertices);
	ثابت صحیح curr = poly.آغاز_اشاریہ + v;
	ثابت صحیح اگلا = poly.آغاز_اشاریہ + ((v + 1) % poly.عدد_vertices);

	ثابت صحیح p_بایاں_کا_incoming  = orient2d(_x_cm[پچھلا_2], _y_cm[پچھلا_2], _x_cm[curr], _y_cm[curr], px, py);
	ثابت صحیح p_بایاں_کا_outgoing  = orient2d(_x_cm[curr], _y_cm[curr], _x_cm[اگلا], _y_cm[اگلا], px, py);
	ثابت صحیح is_convex           = orient2d(_x_cm[پچھلا_2], _y_cm[پچھلا_2], _x_cm[curr], _y_cm[curr], _x_cm[اگلا], _y_cm[اگلا]);

	واپس is_convex > 0  // If convex==0 (exact 180 deg "corner") the two cases are equivalent
	       ? (p_بایاں_کا_incoming > 0 && p_بایاں_کا_outgoing > 0)   // Convex: interior = intersection of two half-planes
	       : (p_بایاں_کا_incoming > 0 || p_بایاں_کا_outgoing > 0);  // Reflex: interior = union of two half-planes
}

منطقی PlannerPolygons::intersects_Inside_کا(ثابت PolygonInfo &poly,
		int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ثابت
{
	// Single pass over polygon edges, doing two jobs:
	//  (1) classify the test segment vs each polygon edge. early-return on:
	//       - strict crossings (line segment - polygon edge)
	//       - interior crossing at vertex
	//  (2) accumulate midpoint winding contribution -- if (1) is inconclusive,
	//      this decides if the line is inside or outside

	صحیح wn = 0;
	منطقی mid_on_boundary = جھوٹ;

	// Rather than calculating the midpoint (mid = (s + e)/2) explicitly,
	// which will lead to rounding error for uneven numbers, we instead
	// scale all other inputs by two, reducing the range by a factor of two
	// (2^31 cm = 21400 km -> 2^30 cm = 10700 km)
	ثابت int32_t twice_mid_x = s_x + e_x;
	ثابت int32_t twice_mid_y = s_y + e_y;

	برائے (صحیح i = 0; i < poly.عدد_vertices; i++) {

		ثابت صحیح پچھلا_idx = poly.آغاز_اشاریہ + (i + poly.عدد_vertices - 1) % poly.عدد_vertices;
		ثابت صحیح curr_idx = poly.آغاز_اشاریہ + i;

		ثابت int32_t ax = _x_cm[پچھلا_idx], ay = _y_cm[پچھلا_idx];
		ثابت int32_t bx = _x_cm[curr_idx], by = _y_cm[curr_idx];

		// (1) test segment vs polygon edge (a, b)
		انتخاب (segmentsIntersect(ax, ay, bx, by, s_x, s_y, e_x, e_y)) {
		حالت Seg_Seg_نتیجہ::Cross:
			واپس سچ;

		// skip AInsideCD - a from this segment is b from the adjacent segment
		حالت Seg_Seg_نتیجہ::BInsideCD:

			// Polygon vertex b is exactly on the interior of the test segment.
			// Return intersecting if it pokes into the polygon interior through vertex b.
			اگر (نقطہ_Inside_Interior_Cone(poly, s_x, s_y, i) || نقطہ_Inside_Interior_Cone(poly, e_x, e_y, i)) {
				واپس سچ;
			}

			توڑو;

		طےشدہ:
			توڑو;
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
				mid_on_boundary = سچ;
			}

		} ورنہ اگر (2 * ay <= twice_mid_y) {
			اگر (2 * by > twice_mid_y && side > 0) { wn++; }

		} ورنہ اگر (2 * by <= twice_mid_y && side < 0) { wn--; }
	}


	// Midpoint on boundary, segment runs along it (strict crossing detected earlier).
	// Non-intersecting in either zone.
	اگر (mid_on_boundary) {
		واپس جھوٹ;
	}

	// Exclusion: violation when midpoint is inside (bounded). Inclusion: when outside (unbounded).
	// This is the only place we depart from the inside = left = forbidden canonical orientation,
	// necessary because of the point membership check.
	ثابت منطقی midpoint_اندر_bounded_inside = (wn != 0);
	واپس midpoint_اندر_bounded_inside != poly.is_inclusion;
}

منطقی PlannerPolygons::intersectsAnyInside(int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ثابت
{
	ثابت int32_t seg_min_x = s_x < e_x ? s_x : e_x;
	ثابت int32_t seg_max_x = s_x > e_x ? s_x : e_x;
	ثابت int32_t seg_min_y = s_y < e_y ? s_y : e_y;
	ثابت int32_t seg_max_y = s_y > e_y ? s_y : e_y;

	برائے (صحیح p = 0; p < عدد_polygons; p++) {
		ثابت PolygonInfo &poly = _polygons[p];

		اگر (seg_max_x < poly.min_x || seg_min_x > poly.max_x ||
		    seg_max_y < poly.min_y || seg_min_y > poly.max_y) {
			// Segment is entirely outside this polygon.
			// For exclusion zones: outside = safe, skip.
			// For inclusion zones: outside = forbidden region, immediate violation.
			اگر (poly.is_inclusion) { واپس سچ; }

			جاری;
		}

		اگر (intersects_Inside_کا(poly, s_x, s_y, e_x, e_y)) {
			واپس سچ;
		}
	}

	واپس جھوٹ;
}

منطقی PlannerPolygons::کنارہ_Bitangent(صحیح a, int32_t bx, int32_t by) ثابت
{
	// Find the polygon owning vertex index a. Indices not belonging to any
	// polygon (in particular the destination at index 0) are trivially bitangent.

	اگر (a == dest_اشاریہ()) { واپس سچ; }

	برائے (صحیح p = 0; p < عدد_polygons; p++) {
		ثابت PolygonInfo &poly = _polygons[p];

		اگر (a >= poly.آغاز_اشاریہ && a < poly.آغاز_اشاریہ + poly.عدد_vertices) {
			// Define point c = a + (a-b) = 2a - b, so that ac = ab mirrored about a
			// Then: Edge is bitangent at a <=> neither ab nor ac pokes into the interior
			int32_t cx = 2 * _x_cm[a] - bx;
			int32_t cy = 2 * _y_cm[a] - by;
			واپس !نقطہ_Inside_Interior_Cone(poly, bx, by, a - poly.آغاز_اشاریہ)
			       && !نقطہ_Inside_Interior_Cone(poly, cx, cy, a - poly.آغاز_اشاریہ);
		}
	}

	واپس سچ;
}

منطقی PlannerPolygons::کنارہ_Bitangent(صحیح a, صحیح b) ثابت
{
	واپس کنارہ_Bitangent(a, _x_cm[b], _y_cm[b]);
}

منطقی PlannerPolygons::کنارہ_Bitangent(صحیح a, ثابت میٹرکس::Vector2f &b) ثابت
{
	واپس کنارہ_Bitangent(a, meters_مقصد_Cm(b(0)), meters_مقصد_Cm(b(1)));
}

منطقی PlannerPolygons::سیٹ_Destination(ثابت میٹرکس::Vector2f &p)
{
	سیٹ_گرہ(dest_اشاریہ(), p);

	ثابت int32_t x_cm = meters_مقصد_Cm(p(0));
	ثابت int32_t y_cm = meters_مقصد_Cm(p(1));

	اگر (intersectsAnyInside(x_cm, y_cm, x_cm, y_cm)) {
		// If the destination breaches a geofence, still use it but
		// signal to the user that it is invalid.
		واپس جھوٹ;
	}

	واپس سچ;
}

میٹرکس::Vector2f PlannerPolygons::getDestination() ثابت
{
	اگر (_max_nodes < 1) {
		واپس میٹرکس::Vector2f{NAN, NAN};
	}

	واپس گرہ_2(dest_اشاریہ());
}

منطقی PlannerPolygons::کنارہ_Visible(صحیح a, صحیح b) ثابت
{
	اگر (a == b) { واپس جھوٹ; }

	اگر (گرہ_not_on_optimal_راستہ[a] || گرہ_not_on_optimal_راستہ[b]) { واپس جھوٹ; }

	// Only bitangent edges can be on a shortest path; this check also cheaply
	// discards edges that violate the polygon at the vertex they come from.
	اگر (!کنارہ_Bitangent(a, b) || !کنارہ_Bitangent(b, a)) { واپس جھوٹ; }

	واپس !intersectsAnyInside(_x_cm[a], _y_cm[a], _x_cm[b], _y_cm[b]);
}

منطقی PlannerPolygons::کنارہ_Visible(ثابت میٹرکس::Vector2f &a, صحیح b) ثابت
{
	اگر (گرہ_not_on_optimal_راستہ[b]) { واپس جھوٹ; }

	اگر (!کنارہ_Bitangent(b, a)) { واپس جھوٹ; }

	واپس !intersectsAnyInside(meters_مقصد_Cm(a(0)), meters_مقصد_Cm(a(1)), _x_cm[b], _y_cm[b]);
}

منطقی PlannerPolygons::کنارہ_Visible(ثابت میٹرکس::Vector2f &a, ثابت میٹرکس::Vector2f &b) ثابت
{
	واپس !intersectsAnyInside(meters_مقصد_Cm(a(0)), meters_مقصد_Cm(a(1)), meters_مقصد_Cm(b(0)), meters_مقصد_Cm(b(1)));
}

اعشاری PlannerPolygons::کنارہ_لاگت(صحیح a, صحیح b) ثابت
{
	واپس کنارہ_Visible(a, b) ? (گرہ_2(a) - گرہ_2(b)).norm() : INFINITY;
}


منطقی segmentsIntersectInclusive(ثابت میٹرکس::Vector2d &a, ثابت میٹرکس::Vector2d &b,
				ثابت میٹرکس::Vector2d &c, ثابت میٹرکس::Vector2d &d)
{
	// Reject separated bounds, including gaps between collinear segments.
	برائے (صحیح axis = 0; axis < 2; ++axis) {
		اگر (math::max(a(axis), b(axis)) < math::min(c(axis), d(axis))
		    || math::max(c(axis), d(axis)) < math::min(a(axis), b(axis))) {
			واپس جھوٹ;
		}
	}

	ثابت میٹرکس::Vector2d ab = b - a;
	ثابت میٹرکس::Vector2d cd = d - c;
	ثابت دوہرا side_c = ab.cross(c - a);
	ثابت دوہرا side_d = ab.cross(d - a);
	ثابت دوہرا side_a = cd.cross(a - c);
	ثابت دوہرا side_b = cd.cross(b - c);

	// Each segment must cross or touch the line through the other segment.
	واپس ((side_c <= 0.0 && side_d >= 0.0) || (side_c >= 0.0 && side_d <= 0.0))
	       && ((side_a <= 0.0 && side_b >= 0.0) || (side_a >= 0.0 && side_b <= 0.0));
}

دوہرا نقطہ_مقصد_Segment_فاصلہ_Squared(ثابت میٹرکس::Vector2d &نقطہ,
				     ثابت میٹرکس::Vector2d &a, ثابت میٹرکس::Vector2d &b)
{
	ثابت میٹرکس::Vector2d direction = b - a;
	ثابت میٹرکس::Vector2d مقصد_نقطہ = نقطہ - a;
	ثابت دوہرا لمبائی_squared = direction.norm_squared();
	// along / length_squared locates the projection: 0 at a, 1 at b.
	ثابت دوہرا along = مقصد_نقطہ.dot(direction);

	اگر (along <= 0.0) {
		واپس مقصد_نقطہ.norm_squared();

	} ورنہ اگر (along >= لمبائی_squared) {
		واپس (نقطہ - b).norm_squared();
	}

	// Squared distance to the line: |to_point x direction|^2 / |direction|^2.
	// https://mathworld.wolfram.com/2-DimensionalPoint-LineDistance.html
	ثابت دوہرا cross = مقصد_نقطہ.cross(direction);
	واپس cross * cross / لمبائی_squared;
}

منطقی isPolygonCCW(ثابت میٹرکس::Vector2f *vertices, صحیح عدد_vertices)
{
	// https://en.wikipedia.org/wiki/Shoelace_formula
	// Triangle formula
	اعشاری signed_area_2x = 0.f;

	برائے (صحیح i = 0; i < عدد_vertices; i++) {
		ثابت صحیح j = (i + 1) % عدد_vertices;
		signed_area_2x += vertices[i](0) * vertices[j](1)
				  - vertices[j](0) * vertices[i](1);
	}

	واپس signed_area_2x > 0.f;
}

} // namespace geofence_utils
