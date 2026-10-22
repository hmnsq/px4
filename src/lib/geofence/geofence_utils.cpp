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

#అన్తర్భావ "geofence_utils.h"
#అన్తర్భావ <lib/geo/geo.h>
#అన్తర్భావ <lib/mathlib/mathlib.h>

నామాకాశ geofence_utils
{

నామాకాశ
{

int32_t meters_గమ్యం_Cm(దశాంశం m) { తిరిగి roundf(m * CM_చొప్పున_M); }

} // namespace


తార్కిక PlannerPolygons::allocate(పూర్ణాంకం max_nodes)
{
	freeBuffers();

	ఒకవేళ (max_nodes > 0) {
		_x_cm = కొత్త int32_t[max_nodes];
		_y_cm = కొత్త int32_t[max_nodes];
		కణుపు_not_on_optimal_మార్గం = కొత్త తార్కిక[max_nodes];

		ఒకవేళ (_x_cm != శూన్యసూచిక && _y_cm != శూన్యసూచిక && కణుపు_not_on_optimal_మార్గం != శూన్యసూచిక) {
			_max_nodes = max_nodes;

		} లేకపోతే {
			freeBuffers();
		}
	}

	reset();

	తిరిగి _max_nodes == max_nodes;
}

శూన్య PlannerPolygons::freeBuffers()
{
	తొలగించు[] _x_cm;
	తొలగించు[] _y_cm;
	తొలగించు[] కణుపు_not_on_optimal_మార్గం;
	_x_cm = శూన్యసూచిక;
	_y_cm = శూన్యసూచిక;
	కణుపు_not_on_optimal_మార్గం = శూన్యసూచిక;
	_max_nodes = 0;
}

శూన్య PlannerPolygons::సమితి_కణుపు(పూర్ణాంకం idx, స్థిరరాశి మాతృక::Vector2f &p)
{
	ఒకవేళ (_max_nodes < 1) {
		తిరిగి;
	}

	idx = math::constrain(idx, 0, _max_nodes - 1);

	_x_cm[idx] = meters_గమ్యం_Cm(p(0));
	_y_cm[idx] = meters_గమ్యం_Cm(p(1));
}

PlannerPolygons::కూడు_ఫలితం PlannerPolygons::కూడు_Polygon(స్థిరరాశి మాతృక::Vector2f *vertices_లోపల, పూర్ణాంకం అంకె_vertices,
		తార్కిక is_inclusion_zone, దశాంశం margin)
{
	ఒకవేళ (అంకె_polygons >= kMaxPolygons || అంకె_nodes_2 + అంకె_vertices > _max_nodes) {
		తిరిగి కూడు_ఫలితం::BudgetExceeded;
	}

	ఒకవేళ (margin < -FLT_EPSILON || అంకె_vertices < 3) { తిరిగి కూడు_ఫలితం::Degenerate; }

	కొరకు (పూర్ణాంకం i = 0; i < అంకె_vertices; ++i) {
		// Reject self-intersecting (non-simple) polygons.
		స్థిరరాశి మాతృక::Vector2f &a = vertices_లోపల[i];
		స్థిరరాశి మాతృక::Vector2f &b = vertices_లోపల[(i + 1) % అంకె_vertices];

		కొరకు (పూర్ణాంకం j = i + 2; j < అంకె_vertices; ++j) {
			ఒకవేళ (i == 0 && j == అంకె_vertices - 1) {
				కొనసాగు; // (last edge, first edge) are adjacent
			}

			స్థిరరాశి మాతృక::Vector2f &c = vertices_లోపల[j];
			స్థిరరాశి మాతృక::Vector2f &d = vertices_లోపల[(j + 1) % అంకె_vertices];

			ఒకవేళ (segmentsIntersect(
				    meters_గమ్యం_Cm(a(0)), meters_గమ్యం_Cm(a(1)), meters_గమ్యం_Cm(b(0)), meters_గమ్యం_Cm(b(1)),
				    meters_గమ్యం_Cm(c(0)), meters_గమ్యం_Cm(c(1)), meters_గమ్యం_Cm(d(0)), meters_గమ్యం_Cm(d(1))) != Seg_Seg_ఫలితం::Disjoint) {
				తిరిగి కూడు_ఫలితం::Degenerate;
			}
		}

		// Reject vertices outside the usable fixed-point range
		ఒకవేళ (!లోపల_Fixed_బిందువు_Range(vertices_లోపల[i](0)) || !లోపల_Fixed_బిందువు_Range(vertices_లోపల[i](1))) {
			తిరిగి కూడు_ఫలితం::బహిర్గతం_యొక్క_Range;
		}
	}

	// Canonical orientation: walking vertices in stored order, INSIDE is on
	// the left of each edge. CCW for exclusion (bounded interior is forbidden),
	// CW for inclusion (unbounded exterior is forbidden). Reverse iff input
	// doesn't already match.
	స్థిరరాశి తార్కిక తిరగేయి = (isPolygonCCW(vertices_లోపల, అంకె_vertices) == is_inclusion_zone);

	// Index into vertices_in in stored order (accounts for possible reversal).
	స్వయంచాలక శీర్షం = [&](పూర్ణాంకం i) -> స్థిరరాశి మాతృక::Vector2f & {
		తిరిగి vertices_లోపల[తిరగేయి ? (అంకె_vertices - 1 - i) : i];
	};

	PolygonInfo &poly = _polygons[అంకె_polygons];
	poly.ప్రారంభం_సూచీ  = అంకె_nodes_2;
	poly.is_inclusion = is_inclusion_zone;

	పూర్ణాంకం బహిర్గతం_idx = 0;

	కొరకు (పూర్ణాంకం i = 0; i < అంకె_vertices; i++) {
		స్థిరరాశి పూర్ణాంకం prev = (i + అంకె_vertices - 1) % అంకె_vertices;
		స్థిరరాశి పూర్ణాంకం తదుపరి = (i + 1) % అంకె_vertices;

		స్థిరరాశి మాతృక::Vector2f అంచు_లోపల  = శీర్షం(i) - శీర్షం(prev);
		స్థిరరాశి మాతృక::Vector2f అంచు_బహిర్గతం = శీర్షం(తదుపరి) - శీర్షం(i);

		స్థిరరాశి దశాంశం అంచు_లోపల_norm = అంచు_లోపల.norm();
		స్థిరరాశి దశాంశం అంచు_బహిర్గతం_norm = అంచు_బహిర్గతం.norm();

		ఒకవేళ (అంచు_లోపల_norm < FLT_EPSILON || అంచు_బహిర్గతం_norm < FLT_EPSILON) {
			తిరిగి కూడు_ఫలితం::Degenerate;
		}

		స్థిరరాశి మాతృక::Vector2f అంచు_లోపల_normalized = అంచు_లోపల / అంచు_లోపల_norm;
		స్థిరరాశి మాతృక::Vector2f అంచు_బహిర్గతం_normalized = అంచు_బహిర్గతం / అంచు_బహిర్గతం_norm;

		// Inward normals of the two adjacent edges.
		// Canonical stored orientation: left side = forbidden region, right side = allowed region
		స్థిరరాశి మాతృక::Vector2f n_లోపల  = మాతృక::Vector2f{-అంచు_లోపల_normalized(1), అంచు_లోపల_normalized(0)};
		స్థిరరాశి మాతృక::Vector2f n_బహిర్గతం = మాతృక::Vector2f{-అంచు_బహిర్గతం_normalized(1), అంచు_బహిర్గతం_normalized(0)};

		మాతృక::Vector2f bisector = n_లోపల + n_బహిర్గతం;
		స్థిరరాశి దశాంశం bisector_len  = bisector.నిడివి();

		ఒకవేళ (bisector_len < FLT_EPSILON) {
			తిరిగి కూడు_ఫలితం::Degenerate; // antiparallel edges - polygon doubles back
		}

		స్థిరరాశి మాతృక::Vector2f normalized_bisector = bisector / bisector_len;

		// The following bit ensures that:
		//  - The original edges are moved away from the forbidden interior by `margin`
		//  - Sharp convex corners (<60deg) are "cut off" with a new edge that is exactly
		//    `margin` away from the original vertex. For this the vertex is replaced by
		//    two new ones.

		// Only split convex corners - others are ignored, see _node_not_on_optimal_path
		స్థిరరాశి తార్కిక మూల_convex = అంచు_లోపల_normalized(0) * అంచు_బహిర్గతం_normalized(1)
					   - అంచు_లోపల_normalized(1) * అంచు_బహిర్గతం_normalized(0) > 0.f;

		// Scaling needed wrt the normalised bisector to achieve parallel edges
		స్థిరరాశి దశాంశం స్థానభ్రంశం_scale = 2.f / bisector_len;

		// bisector_len = 2*sin(interior angle/2); split when interior angle < 60 deg <=> bisector_len < 1.
		స్థిరరాశి తార్కిక angle_sharp = bisector_len < 1;

		// If margin zero, do not split - would output the same vertex twice.
		స్థిరరాశి తార్కిక margin_nonzero = margin > FLT_EPSILON;

		// If we do not have enough space, do not split the vertex.
		// This never happens if the planner-internal node buffer is 2x the original buffer.
		స్థిరరాశి తార్కిక space_for_విభజించు_vertices = అంకె_nodes_2 + బహిర్గతం_idx + 1 < _max_nodes;

		ఒకవేళ (మూల_convex && angle_sharp && margin_nonzero && space_for_విభజించు_vertices) {

			// Sharp corner: cut it off along the line
			//  - perpendicular to normalized_bisector
			//  - going through base := vertex(i) - normalized_bisector * margin

			స్థిరరాశి మాతృక::Vector2f base = శీర్షం(i) - normalized_bisector * margin;
			స్థిరరాశి మాతృక::Vector2f perp = {-normalized_bisector(1), normalized_bisector(0)};

			// Construct s such that:
			//  base + perp*s is on the offset incoming edge
			//  base - perp*s is on the offset outgoing edge
			స్థిరరాశి దశాంశం e_బహిర్గతం_dot_b = అంచు_బహిర్గతం_normalized.dot(normalized_bisector);
			స్థిరరాశి దశాంశం s = margin * (1.f - n_బహిర్గతం.dot(normalized_bisector)) / e_బహిర్గతం_dot_b;

			సమితి_కణుపు(poly.ప్రారంభం_సూచీ + బహిర్గతం_idx++, base + perp * s);
			సమితి_కణుపు(poly.ప్రారంభం_సూచీ + బహిర్గతం_idx++, base - perp * s);

		} లేకపోతే {
			// Regular corner or no space for split: single bisector vertex displaced by margin / sin(interior angle/2).
			సమితి_కణుపు(poly.ప్రారంభం_సూచీ + బహిర్గతం_idx++, శీర్షం(i) - normalized_bisector * (margin * స్థానభ్రంశం_scale));
		}
	}

	స్థిరరాశి పూర్ణాంకం అంకె_బహిర్గతం = బహిర్గతం_idx;
	poly.అంకె_vertices = అంకె_బహిర్గతం;

	computeBoundingBox(poly.ప్రారంభం_సూచీ, అంకె_బహిర్గతం, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// In canonical orientation, the node is not possibly on the shortest
	// path if the vertex curves right or straight (forbidden region >= 180
	// deg)
	కొరకు (పూర్ణాంకం i = 0; i < అంకె_బహిర్గతం; i++) {
		స్థిరరాశి పూర్ణాంకం prev = poly.ప్రారంభం_సూచీ + (i + అంకె_బహిర్గతం - 1) % అంకె_బహిర్గతం;
		స్థిరరాశి పూర్ణాంకం curr = poly.ప్రారంభం_సూచీ + i;
		స్థిరరాశి పూర్ణాంకం తదుపరి = poly.ప్రారంభం_సూచీ + (i + 1) % అంకె_బహిర్గతం;
		స్థిరరాశి తార్కిక curves_కుడి_or_straight = orient2d(_x_cm[prev], _y_cm[prev],
						      _x_cm[curr], _y_cm[curr],
						      _x_cm[తదుపరి], _y_cm[తదుపరి]) <= 0;
		కణుపు_not_on_optimal_మార్గం[curr] = curves_కుడి_or_straight;
	}

	అంకె_nodes_2 += అంకె_బహిర్గతం;
	++అంకె_polygons;
	తిరిగి కూడు_ఫలితం::Success;
}

PlannerPolygons::కూడు_ఫలితం PlannerPolygons::కూడు_Approx_Circle(స్థిరరాశి మాతృక::Vector2f &center, స్థిరరాశి దశాంశం circle_radius,
		దశాంశం margin, స్థిరరాశి తార్కిక is_inclusion_zone)
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

	ఒకవేళ (అంకె_polygons >= kMaxPolygons || అంకె_nodes_2 + kCircleApproxVertices > _max_nodes) {
		తిరిగి కూడు_ఫలితం::BudgetExceeded;
	}

	ఒకవేళ (circle_radius <= 0.1f) { తిరిగి కూడు_ఫలితం::Degenerate; }

	// ensures that the k-gon is, w.r.t the real circle:
	//  - over-approximation of circle grown by margin, if exclusion
	//  - under-approximation of circle shrunk by margin, if inclusion
	స్థిరరాశి దశాంశం k_gon_radius = is_inclusion_zone
				   ? circle_radius - margin
				   : circle_radius / cosf(M_PI_F / kCircleApproxVertices) + margin;

	ఒకవేళ (k_gon_radius <= FLT_EPSILON) { // Negative radius - circle empty
		తిరిగి కూడు_ఫలితం::Degenerate;
	}

	ఒకవేళ (!లోపల_Fixed_బిందువు_Range(fabsf(center(0)) + k_gon_radius)
	    || !లోపల_Fixed_బిందువు_Range(fabsf(center(1)) + k_gon_radius)) {
		తిరిగి కూడు_ఫలితం::బహిర్గతం_యొక్క_Range;
	}

	// Canonical orientation: CCW for exclusion, CW for inclusion
	స్థిరరాశి దశాంశం delta_angle = (is_inclusion_zone ? -1.f : 1.f) * 2.f * M_PI_F / kCircleApproxVertices;

	PolygonInfo &poly = _polygons[అంకె_polygons];
	poly.ప్రారంభం_సూచీ = అంకె_nodes_2;
	poly.అంకె_vertices = kCircleApproxVertices;
	poly.is_inclusion = is_inclusion_zone;

	కొరకు (పూర్ణాంకం i = 0; i < kCircleApproxVertices; i++) {
		స్థిరరాశి దశాంశం angle = (i + 0.5f) * delta_angle;
		స్థిరరాశి మాతృక::Vector2f p = center + మాతృక::Vector2f{
			k_gon_radius * cosf(angle), k_gon_radius * sinf(angle)
		};
		సమితి_కణుపు(poly.ప్రారంభం_సూచీ + i, p);
	}

	computeBoundingBox(poly.ప్రారంభం_సూచీ, kCircleApproxVertices, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// For regular k-gons, all corners are either convex or reflex based on
	// is_inclusion, so the reduced visibility graph criterion is simplified
	కొరకు (పూర్ణాంకం i = 0; i < kCircleApproxVertices; i++) {
		కణుపు_not_on_optimal_మార్గం[poly.ప్రారంభం_సూచీ + i] = is_inclusion_zone;
	}

	అంకె_nodes_2 += kCircleApproxVertices;
	++అంకె_polygons;
	తిరిగి కూడు_ఫలితం::Success;
}

శూన్య PlannerPolygons::computeBoundingBox(స్థిరరాశి పూర్ణాంకం ప్రారంభం_సూచీ, స్థిరరాశి పూర్ణాంకం అంకె_vertices,
		int32_t &min_x, int32_t &max_x, int32_t &min_y, int32_t &max_y)
{

	// Store the bounding box, so we can do a very cheap check against the
	// bounding box, and only do the full line-polygon intersection check if
	// the line intersects the bounding box.

	min_x = max_x = _x_cm[ప్రారంభం_సూచీ];
	min_y = max_y = _y_cm[ప్రారంభం_సూచీ];

	కొరకు (పూర్ణాంకం i = 1; i < అంకె_vertices; i++) {
		స్థిరరాశి int32_t x = _x_cm[ప్రారంభం_సూచీ + i];
		స్థిరరాశి int32_t y = _y_cm[ప్రారంభం_సూచీ + i];

		ఒకవేళ (x < min_x) { min_x = x; }

		ఒకవేళ (x > max_x) { max_x = x; }

		ఒకవేళ (y < min_y) { min_y = y; }

		ఒకవేళ (y > max_y) { max_y = y; }
	}
}

తార్కిక PlannerPolygons::బిందువు_Inside_Interior_Cone(స్థిరరాశి PolygonInfo &poly,
		int32_t px, int32_t py, పూర్ణాంకం v) స్థిరరాశి
{
	స్థిరరాశి పూర్ణాంకం prev = poly.ప్రారంభం_సూచీ + ((v + poly.అంకె_vertices - 1) % poly.అంకె_vertices);
	స్థిరరాశి పూర్ణాంకం curr = poly.ప్రారంభం_సూచీ + v;
	స్థిరరాశి పూర్ణాంకం తదుపరి = poly.ప్రారంభం_సూచీ + ((v + 1) % poly.అంకె_vertices);

	స్థిరరాశి పూర్ణాంకం p_ఎడమ_యొక్క_incoming  = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], px, py);
	స్థిరరాశి పూర్ణాంకం p_ఎడమ_యొక్క_outgoing  = orient2d(_x_cm[curr], _y_cm[curr], _x_cm[తదుపరి], _y_cm[తదుపరి], px, py);
	స్థిరరాశి పూర్ణాంకం is_convex           = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], _x_cm[తదుపరి], _y_cm[తదుపరి]);

	తిరిగి is_convex > 0  // If convex==0 (exact 180 deg "corner") the two cases are equivalent
	       ? (p_ఎడమ_యొక్క_incoming > 0 && p_ఎడమ_యొక్క_outgoing > 0)   // Convex: interior = intersection of two half-planes
	       : (p_ఎడమ_యొక్క_incoming > 0 || p_ఎడమ_యొక్క_outgoing > 0);  // Reflex: interior = union of two half-planes
}

తార్కిక PlannerPolygons::intersects_Inside_యొక్క(స్థిరరాశి PolygonInfo &poly,
		int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) స్థిరరాశి
{
	// Single pass over polygon edges, doing two jobs:
	//  (1) classify the test segment vs each polygon edge. early-return on:
	//       - strict crossings (line segment - polygon edge)
	//       - interior crossing at vertex
	//  (2) accumulate midpoint winding contribution -- if (1) is inconclusive,
	//      this decides if the line is inside or outside

	పూర్ణాంకం wn = 0;
	తార్కిక mid_on_boundary = అబద్ధం;

	// Rather than calculating the midpoint (mid = (s + e)/2) explicitly,
	// which will lead to rounding error for uneven numbers, we instead
	// scale all other inputs by two, reducing the range by a factor of two
	// (2^31 cm = 21400 km -> 2^30 cm = 10700 km)
	స్థిరరాశి int32_t twice_mid_x = s_x + e_x;
	స్థిరరాశి int32_t twice_mid_y = s_y + e_y;

	కొరకు (పూర్ణాంకం i = 0; i < poly.అంకె_vertices; i++) {

		స్థిరరాశి పూర్ణాంకం prev_idx = poly.ప్రారంభం_సూచీ + (i + poly.అంకె_vertices - 1) % poly.అంకె_vertices;
		స్థిరరాశి పూర్ణాంకం curr_idx = poly.ప్రారంభం_సూచీ + i;

		స్థిరరాశి int32_t ax = _x_cm[prev_idx], ay = _y_cm[prev_idx];
		స్థిరరాశి int32_t bx = _x_cm[curr_idx], by = _y_cm[curr_idx];

		// (1) test segment vs polygon edge (a, b)
		ఎంపిక (segmentsIntersect(ax, ay, bx, by, s_x, s_y, e_x, e_y)) {
		స్థితి Seg_Seg_ఫలితం::Cross:
			తిరిగి నిజం;

		// skip AInsideCD - a from this segment is b from the adjacent segment
		స్థితి Seg_Seg_ఫలితం::BInsideCD:

			// Polygon vertex b is exactly on the interior of the test segment.
			// Return intersecting if it pokes into the polygon interior through vertex b.
			ఒకవేళ (బిందువు_Inside_Interior_Cone(poly, s_x, s_y, i) || బిందువు_Inside_Interior_Cone(poly, e_x, e_y, i)) {
				తిరిగి నిజం;
			}

			ఆపు;

		అప్రమేయ:
			ఆపు;
		}

		// (2) midpoint winding contribution
		// Ref: Dan Sunday, Inclusion of a Point in a Polygon
		// https://web.archive.org/web/20130126163405/http://geomalgorithms.com/a03-_inclusion.html

		స్థిరరాశి పూర్ణాంకం side = orient2d(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);

		ఒకవేళ (side == 0) {

			స్థిరరాశి తార్కిక mid_on_open_ab = collinearBetween(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);
			స్థిరరాశి తార్కిక mid_is_b = 2 * bx == twice_mid_x && 2 * by == twice_mid_y;

			// Skip point a - a from this segment is b from the adjacent segment
			ఒకవేళ (mid_on_open_ab || mid_is_b) {
				mid_on_boundary = నిజం;
			}

		} లేకపోతే ఒకవేళ (2 * ay <= twice_mid_y) {
			ఒకవేళ (2 * by > twice_mid_y && side > 0) { wn++; }

		} లేకపోతే ఒకవేళ (2 * by <= twice_mid_y && side < 0) { wn--; }
	}


	// Midpoint on boundary, segment runs along it (strict crossing detected earlier).
	// Non-intersecting in either zone.
	ఒకవేళ (mid_on_boundary) {
		తిరిగి అబద్ధం;
	}

	// Exclusion: violation when midpoint is inside (bounded). Inclusion: when outside (unbounded).
	// This is the only place we depart from the inside = left = forbidden canonical orientation,
	// necessary because of the point membership check.
	స్థిరరాశి తార్కిక midpoint_లోపల_bounded_inside = (wn != 0);
	తిరిగి midpoint_లోపల_bounded_inside != poly.is_inclusion;
}

తార్కిక PlannerPolygons::intersectsAnyInside(int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) స్థిరరాశి
{
	స్థిరరాశి int32_t seg_min_x = s_x < e_x ? s_x : e_x;
	స్థిరరాశి int32_t seg_max_x = s_x > e_x ? s_x : e_x;
	స్థిరరాశి int32_t seg_min_y = s_y < e_y ? s_y : e_y;
	స్థిరరాశి int32_t seg_max_y = s_y > e_y ? s_y : e_y;

	కొరకు (పూర్ణాంకం p = 0; p < అంకె_polygons; p++) {
		స్థిరరాశి PolygonInfo &poly = _polygons[p];

		ఒకవేళ (seg_max_x < poly.min_x || seg_min_x > poly.max_x ||
		    seg_max_y < poly.min_y || seg_min_y > poly.max_y) {
			// Segment is entirely outside this polygon.
			// For exclusion zones: outside = safe, skip.
			// For inclusion zones: outside = forbidden region, immediate violation.
			ఒకవేళ (poly.is_inclusion) { తిరిగి నిజం; }

			కొనసాగు;
		}

		ఒకవేళ (intersects_Inside_యొక్క(poly, s_x, s_y, e_x, e_y)) {
			తిరిగి నిజం;
		}
	}

	తిరిగి అబద్ధం;
}

తార్కిక PlannerPolygons::అంచు_Bitangent(పూర్ణాంకం a, int32_t bx, int32_t by) స్థిరరాశి
{
	// Find the polygon owning vertex index a. Indices not belonging to any
	// polygon (in particular the destination at index 0) are trivially bitangent.

	ఒకవేళ (a == dest_సూచీ()) { తిరిగి నిజం; }

	కొరకు (పూర్ణాంకం p = 0; p < అంకె_polygons; p++) {
		స్థిరరాశి PolygonInfo &poly = _polygons[p];

		ఒకవేళ (a >= poly.ప్రారంభం_సూచీ && a < poly.ప్రారంభం_సూచీ + poly.అంకె_vertices) {
			// Define point c = a + (a-b) = 2a - b, so that ac = ab mirrored about a
			// Then: Edge is bitangent at a <=> neither ab nor ac pokes into the interior
			int32_t cx = 2 * _x_cm[a] - bx;
			int32_t cy = 2 * _y_cm[a] - by;
			తిరిగి !బిందువు_Inside_Interior_Cone(poly, bx, by, a - poly.ప్రారంభం_సూచీ)
			       && !బిందువు_Inside_Interior_Cone(poly, cx, cy, a - poly.ప్రారంభం_సూచీ);
		}
	}

	తిరిగి నిజం;
}

తార్కిక PlannerPolygons::అంచు_Bitangent(పూర్ణాంకం a, పూర్ణాంకం b) స్థిరరాశి
{
	తిరిగి అంచు_Bitangent(a, _x_cm[b], _y_cm[b]);
}

తార్కిక PlannerPolygons::అంచు_Bitangent(పూర్ణాంకం a, స్థిరరాశి మాతృక::Vector2f &b) స్థిరరాశి
{
	తిరిగి అంచు_Bitangent(a, meters_గమ్యం_Cm(b(0)), meters_గమ్యం_Cm(b(1)));
}

తార్కిక PlannerPolygons::సమితి_Destination(స్థిరరాశి మాతృక::Vector2f &p)
{
	సమితి_కణుపు(dest_సూచీ(), p);

	స్థిరరాశి int32_t x_cm = meters_గమ్యం_Cm(p(0));
	స్థిరరాశి int32_t y_cm = meters_గమ్యం_Cm(p(1));

	ఒకవేళ (intersectsAnyInside(x_cm, y_cm, x_cm, y_cm)) {
		// If the destination breaches a geofence, still use it but
		// signal to the user that it is invalid.
		తిరిగి అబద్ధం;
	}

	తిరిగి నిజం;
}

మాతృక::Vector2f PlannerPolygons::getDestination() స్థిరరాశి
{
	ఒకవేళ (_max_nodes < 1) {
		తిరిగి మాతృక::Vector2f{NAN, NAN};
	}

	తిరిగి కణుపు_2(dest_సూచీ());
}

తార్కిక PlannerPolygons::అంచు_Visible(పూర్ణాంకం a, పూర్ణాంకం b) స్థిరరాశి
{
	ఒకవేళ (a == b) { తిరిగి అబద్ధం; }

	ఒకవేళ (కణుపు_not_on_optimal_మార్గం[a] || కణుపు_not_on_optimal_మార్గం[b]) { తిరిగి అబద్ధం; }

	// Only bitangent edges can be on a shortest path; this check also cheaply
	// discards edges that violate the polygon at the vertex they come from.
	ఒకవేళ (!అంచు_Bitangent(a, b) || !అంచు_Bitangent(b, a)) { తిరిగి అబద్ధం; }

	తిరిగి !intersectsAnyInside(_x_cm[a], _y_cm[a], _x_cm[b], _y_cm[b]);
}

తార్కిక PlannerPolygons::అంచు_Visible(స్థిరరాశి మాతృక::Vector2f &a, పూర్ణాంకం b) స్థిరరాశి
{
	ఒకవేళ (కణుపు_not_on_optimal_మార్గం[b]) { తిరిగి అబద్ధం; }

	ఒకవేళ (!అంచు_Bitangent(b, a)) { తిరిగి అబద్ధం; }

	తిరిగి !intersectsAnyInside(meters_గమ్యం_Cm(a(0)), meters_గమ్యం_Cm(a(1)), _x_cm[b], _y_cm[b]);
}

తార్కిక PlannerPolygons::అంచు_Visible(స్థిరరాశి మాతృక::Vector2f &a, స్థిరరాశి మాతృక::Vector2f &b) స్థిరరాశి
{
	తిరిగి !intersectsAnyInside(meters_గమ్యం_Cm(a(0)), meters_గమ్యం_Cm(a(1)), meters_గమ్యం_Cm(b(0)), meters_గమ్యం_Cm(b(1)));
}

దశాంశం PlannerPolygons::అంచు_ఖర్చు(పూర్ణాంకం a, పూర్ణాంకం b) స్థిరరాశి
{
	తిరిగి అంచు_Visible(a, b) ? (కణుపు_2(a) - కణుపు_2(b)).norm() : INFINITY;
}


తార్కిక segmentsIntersectInclusive(స్థిరరాశి మాతృక::Vector2d &a, స్థిరరాశి మాతృక::Vector2d &b,
				స్థిరరాశి మాతృక::Vector2d &c, స్థిరరాశి మాతృక::Vector2d &d)
{
	// Reject separated bounds, including gaps between collinear segments.
	కొరకు (పూర్ణాంకం axis = 0; axis < 2; ++axis) {
		ఒకవేళ (math::max(a(axis), b(axis)) < math::min(c(axis), d(axis))
		    || math::max(c(axis), d(axis)) < math::min(a(axis), b(axis))) {
			తిరిగి అబద్ధం;
		}
	}

	స్థిరరాశి మాతృక::Vector2d ab = b - a;
	స్థిరరాశి మాతృక::Vector2d cd = d - c;
	స్థిరరాశి ద్విగుణ side_c = ab.cross(c - a);
	స్థిరరాశి ద్విగుణ side_d = ab.cross(d - a);
	స్థిరరాశి ద్విగుణ side_a = cd.cross(a - c);
	స్థిరరాశి ద్విగుణ side_b = cd.cross(b - c);

	// Each segment must cross or touch the line through the other segment.
	తిరిగి ((side_c <= 0.0 && side_d >= 0.0) || (side_c >= 0.0 && side_d <= 0.0))
	       && ((side_a <= 0.0 && side_b >= 0.0) || (side_a >= 0.0 && side_b <= 0.0));
}

ద్విగుణ బిందువు_గమ్యం_Segment_దూరం_Squared(స్థిరరాశి మాతృక::Vector2d &బిందువు,
				     స్థిరరాశి మాతృక::Vector2d &a, స్థిరరాశి మాతృక::Vector2d &b)
{
	స్థిరరాశి మాతృక::Vector2d direction = b - a;
	స్థిరరాశి మాతృక::Vector2d గమ్యం_బిందువు = బిందువు - a;
	స్థిరరాశి ద్విగుణ నిడివి_squared = direction.norm_squared();
	// along / length_squared locates the projection: 0 at a, 1 at b.
	స్థిరరాశి ద్విగుణ along = గమ్యం_బిందువు.dot(direction);

	ఒకవేళ (along <= 0.0) {
		తిరిగి గమ్యం_బిందువు.norm_squared();

	} లేకపోతే ఒకవేళ (along >= నిడివి_squared) {
		తిరిగి (బిందువు - b).norm_squared();
	}

	// Squared distance to the line: |to_point x direction|^2 / |direction|^2.
	// https://mathworld.wolfram.com/2-DimensionalPoint-LineDistance.html
	స్థిరరాశి ద్విగుణ cross = గమ్యం_బిందువు.cross(direction);
	తిరిగి cross * cross / నిడివి_squared;
}

తార్కిక isPolygonCCW(స్థిరరాశి మాతృక::Vector2f *vertices, పూర్ణాంకం అంకె_vertices)
{
	// https://en.wikipedia.org/wiki/Shoelace_formula
	// Triangle formula
	దశాంశం signed_area_2x = 0.f;

	కొరకు (పూర్ణాంకం i = 0; i < అంకె_vertices; i++) {
		స్థిరరాశి పూర్ణాంకం j = (i + 1) % అంకె_vertices;
		signed_area_2x += vertices[i](0) * vertices[j](1)
				  - vertices[j](0) * vertices[i](1);
	}

	తిరిగి signed_area_2x > 0.f;
}

} // namespace geofence_utils
