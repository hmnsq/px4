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

#अन्तर्भाव "geofence_utils.h"
#अन्तर्भाव <lib/geo/geo.h>
#अन्तर्भाव <lib/mathlib/mathlib.h>

नामाकाश geofence_utils
{

नामाकाश
{

int32_t meters_गन्तव्यम्_Cm(भिन्न m) { प्रत्यागम roundf(m * CM_प्रत्येकम्_M); }

} // namespace


तार्किक PlannerPolygons::allocate(पूर्णाङ्क max_nodes)
{
	freeBuffers();

	यदि (max_nodes > 0) {
		_x_cm = नूतन int32_t[max_nodes];
		_y_cm = नूतन int32_t[max_nodes];
		ग्रन्थिः_not_on_optimal_मार्गः = नूतन तार्किक[max_nodes];

		यदि (_x_cm != शून्यसूचकः && _y_cm != शून्यसूचकः && ग्रन्थिः_not_on_optimal_मार्गः != शून्यसूचकः) {
			_max_nodes = max_nodes;

		} अन्यथा {
			freeBuffers();
		}
	}

	reset();

	प्रत्यागम _max_nodes == max_nodes;
}

शून्य PlannerPolygons::freeBuffers()
{
	विलोपय[] _x_cm;
	विलोपय[] _y_cm;
	विलोपय[] ग्रन्थिः_not_on_optimal_मार्गः;
	_x_cm = शून्यसूचकः;
	_y_cm = शून्यसूचकः;
	ग्रन्थिः_not_on_optimal_मार्गः = शून्यसूचकः;
	_max_nodes = 0;
}

शून्य PlannerPolygons::समुच्चयः_ग्रन्थिः(पूर्णाङ्क idx, ध्रुव matrix::Vector2f &p)
{
	यदि (_max_nodes < 1) {
		प्रत्यागम;
	}

	idx = math::constrain(idx, 0, _max_nodes - 1);

	_x_cm[idx] = meters_गन्तव्यम्_Cm(p(0));
	_y_cm[idx] = meters_गन्तव्यम्_Cm(p(1));
}

PlannerPolygons::सङ्कलय_परिणाम PlannerPolygons::सङ्कलय_Polygon(ध्रुव matrix::Vector2f *vertices_अन्तर्, पूर्णाङ्क अङ्कः_vertices,
		तार्किक is_inclusion_zone, भिन्न margin)
{
	यदि (अङ्कः_polygons >= kMaxPolygons || अङ्कः_nodes_2 + अङ्कः_vertices > _max_nodes) {
		प्रत्यागम सङ्कलय_परिणाम::BudgetExceeded;
	}

	यदि (margin < -FLT_EPSILON || अङ्कः_vertices < 3) { प्रत्यागम सङ्कलय_परिणाम::Degenerate; }

	प्रति (पूर्णाङ्क i = 0; i < अङ्कः_vertices; ++i) {
		// Reject self-intersecting (non-simple) polygons.
		ध्रुव matrix::Vector2f &a = vertices_अन्तर्[i];
		ध्रुव matrix::Vector2f &b = vertices_अन्तर्[(i + 1) % अङ्कः_vertices];

		प्रति (पूर्णाङ्क j = i + 2; j < अङ्कः_vertices; ++j) {
			यदि (i == 0 && j == अङ्कः_vertices - 1) {
				अनुवर्त; // (last edge, first edge) are adjacent
			}

			ध्रुव matrix::Vector2f &c = vertices_अन्तर्[j];
			ध्रुव matrix::Vector2f &d = vertices_अन्तर्[(j + 1) % अङ्कः_vertices];

			यदि (segmentsIntersect(
				    meters_गन्तव्यम्_Cm(a(0)), meters_गन्तव्यम्_Cm(a(1)), meters_गन्तव्यम्_Cm(b(0)), meters_गन्तव्यम्_Cm(b(1)),
				    meters_गन्तव्यम्_Cm(c(0)), meters_गन्तव्यम्_Cm(c(1)), meters_गन्तव्यम्_Cm(d(0)), meters_गन्तव्यम्_Cm(d(1))) != Seg_Seg_परिणाम::Disjoint) {
				प्रत्यागम सङ्कलय_परिणाम::Degenerate;
			}
		}

		// Reject vertices outside the usable fixed-point range
		यदि (!अन्तर्_Fixed_Point_Range(vertices_अन्तर्[i](0)) || !अन्तर्_Fixed_Point_Range(vertices_अन्तर्[i](1))) {
			प्रत्यागम सङ्कलय_परिणाम::बहिर्गमनम्_सम्बन्धि_Range;
		}
	}

	// Canonical orientation: walking vertices in stored order, INSIDE is on
	// the left of each edge. CCW for exclusion (bounded interior is forbidden),
	// CW for inclusion (unbounded exterior is forbidden). Reverse iff input
	// doesn't already match.
	ध्रुव तार्किक reverse = (isPolygonCCW(vertices_अन्तर्, अङ्कः_vertices) == is_inclusion_zone);

	// Index into vertices_in in stored order (accounts for possible reversal).
	स्वचालित vertex = [&](पूर्णाङ्क i) -> ध्रुव matrix::Vector2f & {
		प्रत्यागम vertices_अन्तर्[reverse ? (अङ्कः_vertices - 1 - i) : i];
	};

	PolygonInfo &poly = _polygons[अङ्कः_polygons];
	poly.start_सूचकांक  = अङ्कः_nodes_2;
	poly.is_inclusion = is_inclusion_zone;

	पूर्णाङ्क बहिर्गमनम्_idx = 0;

	प्रति (पूर्णाङ्क i = 0; i < अङ्कः_vertices; i++) {
		ध्रुव पूर्णाङ्क पूर्व = (i + अङ्कः_vertices - 1) % अङ्कः_vertices;
		ध्रुव पूर्णाङ्क अग्र = (i + 1) % अङ्कः_vertices;

		ध्रुव matrix::Vector2f edge_अन्तर्  = vertex(i) - vertex(पूर्व);
		ध्रुव matrix::Vector2f edge_बहिर्गमनम् = vertex(अग्र) - vertex(i);

		ध्रुव भिन्न edge_अन्तर्_norm = edge_अन्तर्.norm();
		ध्रुव भिन्न edge_बहिर्गमनम्_norm = edge_बहिर्गमनम्.norm();

		यदि (edge_अन्तर्_norm < FLT_EPSILON || edge_बहिर्गमनम्_norm < FLT_EPSILON) {
			प्रत्यागम सङ्कलय_परिणाम::Degenerate;
		}

		ध्रुव matrix::Vector2f edge_अन्तर्_normalized = edge_अन्तर् / edge_अन्तर्_norm;
		ध्रुव matrix::Vector2f edge_बहिर्गमनम्_normalized = edge_बहिर्गमनम् / edge_बहिर्गमनम्_norm;

		// Inward normals of the two adjacent edges.
		// Canonical stored orientation: left side = forbidden region, right side = allowed region
		ध्रुव matrix::Vector2f n_अन्तर्  = matrix::Vector2f{-edge_अन्तर्_normalized(1), edge_अन्तर्_normalized(0)};
		ध्रुव matrix::Vector2f n_बहिर्गमनम् = matrix::Vector2f{-edge_बहिर्गमनम्_normalized(1), edge_बहिर्गमनम्_normalized(0)};

		matrix::Vector2f bisector = n_अन्तर् + n_बहिर्गमनम्;
		ध्रुव भिन्न bisector_len  = bisector.दीर्घता_2();

		यदि (bisector_len < FLT_EPSILON) {
			प्रत्यागम सङ्कलय_परिणाम::Degenerate; // antiparallel edges - polygon doubles back
		}

		ध्रुव matrix::Vector2f normalized_bisector = bisector / bisector_len;

		// The following bit ensures that:
		//  - The original edges are moved away from the forbidden interior by `margin`
		//  - Sharp convex corners (<60deg) are "cut off" with a new edge that is exactly
		//    `margin` away from the original vertex. For this the vertex is replaced by
		//    two new ones.

		// Only split convex corners - others are ignored, see _node_not_on_optimal_path
		ध्रुव तार्किक corner_convex = edge_अन्तर्_normalized(0) * edge_बहिर्गमनम्_normalized(1)
					   - edge_अन्तर्_normalized(1) * edge_बहिर्गमनम्_normalized(0) > 0.f;

		// Scaling needed wrt the normalised bisector to achieve parallel edges
		ध्रुव भिन्न विस्थापनम्_scale = 2.f / bisector_len;

		// bisector_len = 2*sin(interior angle/2); split when interior angle < 60 deg <=> bisector_len < 1.
		ध्रुव तार्किक angle_sharp = bisector_len < 1;

		// If margin zero, do not split - would output the same vertex twice.
		ध्रुव तार्किक margin_nonzero = margin > FLT_EPSILON;

		// If we do not have enough space, do not split the vertex.
		// This never happens if the planner-internal node buffer is 2x the original buffer.
		ध्रुव तार्किक space_for_विभज_vertices = अङ्कः_nodes_2 + बहिर्गमनम्_idx + 1 < _max_nodes;

		यदि (corner_convex && angle_sharp && margin_nonzero && space_for_विभज_vertices) {

			// Sharp corner: cut it off along the line
			//  - perpendicular to normalized_bisector
			//  - going through base := vertex(i) - normalized_bisector * margin

			ध्रुव matrix::Vector2f base = vertex(i) - normalized_bisector * margin;
			ध्रुव matrix::Vector2f perp = {-normalized_bisector(1), normalized_bisector(0)};

			// Construct s such that:
			//  base + perp*s is on the offset incoming edge
			//  base - perp*s is on the offset outgoing edge
			ध्रुव भिन्न e_बहिर्गमनम्_dot_b = edge_बहिर्गमनम्_normalized.dot(normalized_bisector);
			ध्रुव भिन्न s = margin * (1.f - n_बहिर्गमनम्.dot(normalized_bisector)) / e_बहिर्गमनम्_dot_b;

			समुच्चयः_ग्रन्थिः(poly.start_सूचकांक + बहिर्गमनम्_idx++, base + perp * s);
			समुच्चयः_ग्रन्थिः(poly.start_सूचकांक + बहिर्गमनम्_idx++, base - perp * s);

		} अन्यथा {
			// Regular corner or no space for split: single bisector vertex displaced by margin / sin(interior angle/2).
			समुच्चयः_ग्रन्थिः(poly.start_सूचकांक + बहिर्गमनम्_idx++, vertex(i) - normalized_bisector * (margin * विस्थापनम्_scale));
		}
	}

	ध्रुव पूर्णाङ्क अङ्कः_बहिर्गमनम् = बहिर्गमनम्_idx;
	poly.अङ्कः_vertices = अङ्कः_बहिर्गमनम्;

	computeBoundingBox(poly.start_सूचकांक, अङ्कः_बहिर्गमनम्, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// In canonical orientation, the node is not possibly on the shortest
	// path if the vertex curves right or straight (forbidden region >= 180
	// deg)
	प्रति (पूर्णाङ्क i = 0; i < अङ्कः_बहिर्गमनम्; i++) {
		ध्रुव पूर्णाङ्क पूर्व = poly.start_सूचकांक + (i + अङ्कः_बहिर्गमनम् - 1) % अङ्कः_बहिर्गमनम्;
		ध्रुव पूर्णाङ्क curr = poly.start_सूचकांक + i;
		ध्रुव पूर्णाङ्क अग्र = poly.start_सूचकांक + (i + 1) % अङ्कः_बहिर्गमनम्;
		ध्रुव तार्किक curves_दक्षिणः_or_straight = orient2d(_x_cm[पूर्व], _y_cm[पूर्व],
						      _x_cm[curr], _y_cm[curr],
						      _x_cm[अग्र], _y_cm[अग्र]) <= 0;
		ग्रन्थिः_not_on_optimal_मार्गः[curr] = curves_दक्षिणः_or_straight;
	}

	अङ्कः_nodes_2 += अङ्कः_बहिर्गमनम्;
	++अङ्कः_polygons;
	प्रत्यागम सङ्कलय_परिणाम::Success;
}

PlannerPolygons::सङ्कलय_परिणाम PlannerPolygons::सङ्कलय_Approx_Circle(ध्रुव matrix::Vector2f &center, ध्रुव भिन्न circle_radius,
		भिन्न margin, ध्रुव तार्किक is_inclusion_zone)
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

	यदि (अङ्कः_polygons >= kMaxPolygons || अङ्कः_nodes_2 + kCircleApproxVertices > _max_nodes) {
		प्रत्यागम सङ्कलय_परिणाम::BudgetExceeded;
	}

	यदि (circle_radius <= 0.1f) { प्रत्यागम सङ्कलय_परिणाम::Degenerate; }

	// ensures that the k-gon is, w.r.t the real circle:
	//  - over-approximation of circle grown by margin, if exclusion
	//  - under-approximation of circle shrunk by margin, if inclusion
	ध्रुव भिन्न k_gon_radius = is_inclusion_zone
				   ? circle_radius - margin
				   : circle_radius / cosf(M_PI_F / kCircleApproxVertices) + margin;

	यदि (k_gon_radius <= FLT_EPSILON) { // Negative radius - circle empty
		प्रत्यागम सङ्कलय_परिणाम::Degenerate;
	}

	यदि (!अन्तर्_Fixed_Point_Range(fabsf(center(0)) + k_gon_radius)
	    || !अन्तर्_Fixed_Point_Range(fabsf(center(1)) + k_gon_radius)) {
		प्रत्यागम सङ्कलय_परिणाम::बहिर्गमनम्_सम्बन्धि_Range;
	}

	// Canonical orientation: CCW for exclusion, CW for inclusion
	ध्रुव भिन्न delta_angle = (is_inclusion_zone ? -1.f : 1.f) * 2.f * M_PI_F / kCircleApproxVertices;

	PolygonInfo &poly = _polygons[अङ्कः_polygons];
	poly.start_सूचकांक = अङ्कः_nodes_2;
	poly.अङ्कः_vertices = kCircleApproxVertices;
	poly.is_inclusion = is_inclusion_zone;

	प्रति (पूर्णाङ्क i = 0; i < kCircleApproxVertices; i++) {
		ध्रुव भिन्न angle = (i + 0.5f) * delta_angle;
		ध्रुव matrix::Vector2f p = center + matrix::Vector2f{
			k_gon_radius * cosf(angle), k_gon_radius * sinf(angle)
		};
		समुच्चयः_ग्रन्थिः(poly.start_सूचकांक + i, p);
	}

	computeBoundingBox(poly.start_सूचकांक, kCircleApproxVertices, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// For regular k-gons, all corners are either convex or reflex based on
	// is_inclusion, so the reduced visibility graph criterion is simplified
	प्रति (पूर्णाङ्क i = 0; i < kCircleApproxVertices; i++) {
		ग्रन्थिः_not_on_optimal_मार्गः[poly.start_सूचकांक + i] = is_inclusion_zone;
	}

	अङ्कः_nodes_2 += kCircleApproxVertices;
	++अङ्कः_polygons;
	प्रत्यागम सङ्कलय_परिणाम::Success;
}

शून्य PlannerPolygons::computeBoundingBox(ध्रुव पूर्णाङ्क start_सूचकांक, ध्रुव पूर्णाङ्क अङ्कः_vertices,
		int32_t &min_x, int32_t &max_x, int32_t &min_y, int32_t &max_y)
{

	// Store the bounding box, so we can do a very cheap check against the
	// bounding box, and only do the full line-polygon intersection check if
	// the line intersects the bounding box.

	min_x = max_x = _x_cm[start_सूचकांक];
	min_y = max_y = _y_cm[start_सूचकांक];

	प्रति (पूर्णाङ्क i = 1; i < अङ्कः_vertices; i++) {
		ध्रुव int32_t x = _x_cm[start_सूचकांक + i];
		ध्रुव int32_t y = _y_cm[start_सूचकांक + i];

		यदि (x < min_x) { min_x = x; }

		यदि (x > max_x) { max_x = x; }

		यदि (y < min_y) { min_y = y; }

		यदि (y > max_y) { max_y = y; }
	}
}

तार्किक PlannerPolygons::pointInsideInteriorCone(ध्रुव PolygonInfo &poly,
		int32_t px, int32_t py, पूर्णाङ्क v) ध्रुव
{
	ध्रुव पूर्णाङ्क पूर्व = poly.start_सूचकांक + ((v + poly.अङ्कः_vertices - 1) % poly.अङ्कः_vertices);
	ध्रुव पूर्णाङ्क curr = poly.start_सूचकांक + v;
	ध्रुव पूर्णाङ्क अग्र = poly.start_सूचकांक + ((v + 1) % poly.अङ्कः_vertices);

	ध्रुव पूर्णाङ्क p_वामः_सम्बन्धि_incoming  = orient2d(_x_cm[पूर्व], _y_cm[पूर्व], _x_cm[curr], _y_cm[curr], px, py);
	ध्रुव पूर्णाङ्क p_वामः_सम्बन्धि_outgoing  = orient2d(_x_cm[curr], _y_cm[curr], _x_cm[अग्र], _y_cm[अग्र], px, py);
	ध्रुव पूर्णाङ्क is_convex           = orient2d(_x_cm[पूर्व], _y_cm[पूर्व], _x_cm[curr], _y_cm[curr], _x_cm[अग्र], _y_cm[अग्र]);

	प्रत्यागम is_convex > 0  // If convex==0 (exact 180 deg "corner") the two cases are equivalent
	       ? (p_वामः_सम्बन्धि_incoming > 0 && p_वामः_सम्बन्धि_outgoing > 0)   // Convex: interior = intersection of two half-planes
	       : (p_वामः_सम्बन्धि_incoming > 0 || p_वामः_सम्बन्धि_outgoing > 0);  // Reflex: interior = union of two half-planes
}

तार्किक PlannerPolygons::intersects_Inside_सम्बन्धि(ध्रुव PolygonInfo &poly,
		int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ध्रुव
{
	// Single pass over polygon edges, doing two jobs:
	//  (1) classify the test segment vs each polygon edge. early-return on:
	//       - strict crossings (line segment - polygon edge)
	//       - interior crossing at vertex
	//  (2) accumulate midpoint winding contribution -- if (1) is inconclusive,
	//      this decides if the line is inside or outside

	पूर्णाङ्क wn = 0;
	तार्किक mid_on_boundary = असत्य;

	// Rather than calculating the midpoint (mid = (s + e)/2) explicitly,
	// which will lead to rounding error for uneven numbers, we instead
	// scale all other inputs by two, reducing the range by a factor of two
	// (2^31 cm = 21400 km -> 2^30 cm = 10700 km)
	ध्रुव int32_t twice_mid_x = s_x + e_x;
	ध्रुव int32_t twice_mid_y = s_y + e_y;

	प्रति (पूर्णाङ्क i = 0; i < poly.अङ्कः_vertices; i++) {

		ध्रुव पूर्णाङ्क पूर्व_idx = poly.start_सूचकांक + (i + poly.अङ्कः_vertices - 1) % poly.अङ्कः_vertices;
		ध्रुव पूर्णाङ्क curr_idx = poly.start_सूचकांक + i;

		ध्रुव int32_t ax = _x_cm[पूर्व_idx], ay = _y_cm[पूर्व_idx];
		ध्रुव int32_t bx = _x_cm[curr_idx], by = _y_cm[curr_idx];

		// (1) test segment vs polygon edge (a, b)
		विकल्प (segmentsIntersect(ax, ay, bx, by, s_x, s_y, e_x, e_y)) {
		पक्ष Seg_Seg_परिणाम::Cross:
			प्रत्यागम सत्य;

		// skip AInsideCD - a from this segment is b from the adjacent segment
		पक्ष Seg_Seg_परिणाम::BInsideCD:

			// Polygon vertex b is exactly on the interior of the test segment.
			// Return intersecting if it pokes into the polygon interior through vertex b.
			यदि (pointInsideInteriorCone(poly, s_x, s_y, i) || pointInsideInteriorCone(poly, e_x, e_y, i)) {
				प्रत्यागम सत्य;
			}

			भङ्ग;

		स्वभाव:
			भङ्ग;
		}

		// (2) midpoint winding contribution
		// Ref: Dan Sunday, Inclusion of a Point in a Polygon
		// https://web.archive.org/web/20130126163405/http://geomalgorithms.com/a03-_inclusion.html

		ध्रुव पूर्णाङ्क side = orient2d(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);

		यदि (side == 0) {

			ध्रुव तार्किक mid_on_open_ab = collinearBetween(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);
			ध्रुव तार्किक mid_is_b = 2 * bx == twice_mid_x && 2 * by == twice_mid_y;

			// Skip point a - a from this segment is b from the adjacent segment
			यदि (mid_on_open_ab || mid_is_b) {
				mid_on_boundary = सत्य;
			}

		} अन्यथा यदि (2 * ay <= twice_mid_y) {
			यदि (2 * by > twice_mid_y && side > 0) { wn++; }

		} अन्यथा यदि (2 * by <= twice_mid_y && side < 0) { wn--; }
	}


	// Midpoint on boundary, segment runs along it (strict crossing detected earlier).
	// Non-intersecting in either zone.
	यदि (mid_on_boundary) {
		प्रत्यागम असत्य;
	}

	// Exclusion: violation when midpoint is inside (bounded). Inclusion: when outside (unbounded).
	// This is the only place we depart from the inside = left = forbidden canonical orientation,
	// necessary because of the point membership check.
	ध्रुव तार्किक midpoint_अन्तर्_bounded_inside = (wn != 0);
	प्रत्यागम midpoint_अन्तर्_bounded_inside != poly.is_inclusion;
}

तार्किक PlannerPolygons::intersectsAnyInside(int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ध्रुव
{
	ध्रुव int32_t seg_min_x = s_x < e_x ? s_x : e_x;
	ध्रुव int32_t seg_max_x = s_x > e_x ? s_x : e_x;
	ध्रुव int32_t seg_min_y = s_y < e_y ? s_y : e_y;
	ध्रुव int32_t seg_max_y = s_y > e_y ? s_y : e_y;

	प्रति (पूर्णाङ्क p = 0; p < अङ्कः_polygons; p++) {
		ध्रुव PolygonInfo &poly = _polygons[p];

		यदि (seg_max_x < poly.min_x || seg_min_x > poly.max_x ||
		    seg_max_y < poly.min_y || seg_min_y > poly.max_y) {
			// Segment is entirely outside this polygon.
			// For exclusion zones: outside = safe, skip.
			// For inclusion zones: outside = forbidden region, immediate violation.
			यदि (poly.is_inclusion) { प्रत्यागम सत्य; }

			अनुवर्त;
		}

		यदि (intersects_Inside_सम्बन्धि(poly, s_x, s_y, e_x, e_y)) {
			प्रत्यागम सत्य;
		}
	}

	प्रत्यागम असत्य;
}

तार्किक PlannerPolygons::edgeBitangent(पूर्णाङ्क a, int32_t bx, int32_t by) ध्रुव
{
	// Find the polygon owning vertex index a. Indices not belonging to any
	// polygon (in particular the destination at index 0) are trivially bitangent.

	यदि (a == dest_सूचकांक()) { प्रत्यागम सत्य; }

	प्रति (पूर्णाङ्क p = 0; p < अङ्कः_polygons; p++) {
		ध्रुव PolygonInfo &poly = _polygons[p];

		यदि (a >= poly.start_सूचकांक && a < poly.start_सूचकांक + poly.अङ्कः_vertices) {
			// Define point c = a + (a-b) = 2a - b, so that ac = ab mirrored about a
			// Then: Edge is bitangent at a <=> neither ab nor ac pokes into the interior
			int32_t cx = 2 * _x_cm[a] - bx;
			int32_t cy = 2 * _y_cm[a] - by;
			प्रत्यागम !pointInsideInteriorCone(poly, bx, by, a - poly.start_सूचकांक)
			       && !pointInsideInteriorCone(poly, cx, cy, a - poly.start_सूचकांक);
		}
	}

	प्रत्यागम सत्य;
}

तार्किक PlannerPolygons::edgeBitangent(पूर्णाङ्क a, पूर्णाङ्क b) ध्रुव
{
	प्रत्यागम edgeBitangent(a, _x_cm[b], _y_cm[b]);
}

तार्किक PlannerPolygons::edgeBitangent(पूर्णाङ्क a, ध्रुव matrix::Vector2f &b) ध्रुव
{
	प्रत्यागम edgeBitangent(a, meters_गन्तव्यम्_Cm(b(0)), meters_गन्तव्यम्_Cm(b(1)));
}

तार्किक PlannerPolygons::समुच्चयः_Destination(ध्रुव matrix::Vector2f &p)
{
	समुच्चयः_ग्रन्थिः(dest_सूचकांक(), p);

	ध्रुव int32_t x_cm = meters_गन्तव्यम्_Cm(p(0));
	ध्रुव int32_t y_cm = meters_गन्तव्यम्_Cm(p(1));

	यदि (intersectsAnyInside(x_cm, y_cm, x_cm, y_cm)) {
		// If the destination breaches a geofence, still use it but
		// signal to the user that it is invalid.
		प्रत्यागम असत्य;
	}

	प्रत्यागम सत्य;
}

matrix::Vector2f PlannerPolygons::getDestination() ध्रुव
{
	यदि (_max_nodes < 1) {
		प्रत्यागम matrix::Vector2f{NAN, NAN};
	}

	प्रत्यागम ग्रन्थिः_2(dest_सूचकांक());
}

तार्किक PlannerPolygons::edgeVisible(पूर्णाङ्क a, पूर्णाङ्क b) ध्रुव
{
	यदि (a == b) { प्रत्यागम असत्य; }

	यदि (ग्रन्थिः_not_on_optimal_मार्गः[a] || ग्रन्थिः_not_on_optimal_मार्गः[b]) { प्रत्यागम असत्य; }

	// Only bitangent edges can be on a shortest path; this check also cheaply
	// discards edges that violate the polygon at the vertex they come from.
	यदि (!edgeBitangent(a, b) || !edgeBitangent(b, a)) { प्रत्यागम असत्य; }

	प्रत्यागम !intersectsAnyInside(_x_cm[a], _y_cm[a], _x_cm[b], _y_cm[b]);
}

तार्किक PlannerPolygons::edgeVisible(ध्रुव matrix::Vector2f &a, पूर्णाङ्क b) ध्रुव
{
	यदि (ग्रन्थिः_not_on_optimal_मार्गः[b]) { प्रत्यागम असत्य; }

	यदि (!edgeBitangent(b, a)) { प्रत्यागम असत्य; }

	प्रत्यागम !intersectsAnyInside(meters_गन्तव्यम्_Cm(a(0)), meters_गन्तव्यम्_Cm(a(1)), _x_cm[b], _y_cm[b]);
}

तार्किक PlannerPolygons::edgeVisible(ध्रुव matrix::Vector2f &a, ध्रुव matrix::Vector2f &b) ध्रुव
{
	प्रत्यागम !intersectsAnyInside(meters_गन्तव्यम्_Cm(a(0)), meters_गन्तव्यम्_Cm(a(1)), meters_गन्तव्यम्_Cm(b(0)), meters_गन्तव्यम्_Cm(b(1)));
}

भिन्न PlannerPolygons::edgeCost(पूर्णाङ्क a, पूर्णाङ्क b) ध्रुव
{
	प्रत्यागम edgeVisible(a, b) ? (ग्रन्थिः_2(a) - ग्रन्थिः_2(b)).norm() : INFINITY;
}


तार्किक segmentsIntersectInclusive(ध्रुव matrix::Vector2d &a, ध्रुव matrix::Vector2d &b,
				ध्रुव matrix::Vector2d &c, ध्रुव matrix::Vector2d &d)
{
	// Reject separated bounds, including gaps between collinear segments.
	प्रति (पूर्णाङ्क axis = 0; axis < 2; ++axis) {
		यदि (math::max(a(axis), b(axis)) < math::min(c(axis), d(axis))
		    || math::max(c(axis), d(axis)) < math::min(a(axis), b(axis))) {
			प्रत्यागम असत्य;
		}
	}

	ध्रुव matrix::Vector2d ab = b - a;
	ध्रुव matrix::Vector2d cd = d - c;
	ध्रुव द्विगुण side_c = ab.cross(c - a);
	ध्रुव द्विगुण side_d = ab.cross(d - a);
	ध्रुव द्विगुण side_a = cd.cross(a - c);
	ध्रुव द्विगुण side_b = cd.cross(b - c);

	// Each segment must cross or touch the line through the other segment.
	प्रत्यागम ((side_c <= 0.0 && side_d >= 0.0) || (side_c >= 0.0 && side_d <= 0.0))
	       && ((side_a <= 0.0 && side_b >= 0.0) || (side_a >= 0.0 && side_b <= 0.0));
}

द्विगुण point_गन्तव्यम्_Segment_Distance_Squared(ध्रुव matrix::Vector2d &point,
				     ध्रुव matrix::Vector2d &a, ध्रुव matrix::Vector2d &b)
{
	ध्रुव matrix::Vector2d direction = b - a;
	ध्रुव matrix::Vector2d गन्तव्यम्_point = point - a;
	ध्रुव द्विगुण दीर्घता_squared = direction.norm_squared();
	// along / length_squared locates the projection: 0 at a, 1 at b.
	ध्रुव द्विगुण along = गन्तव्यम्_point.dot(direction);

	यदि (along <= 0.0) {
		प्रत्यागम गन्तव्यम्_point.norm_squared();

	} अन्यथा यदि (along >= दीर्घता_squared) {
		प्रत्यागम (point - b).norm_squared();
	}

	// Squared distance to the line: |to_point x direction|^2 / |direction|^2.
	// https://mathworld.wolfram.com/2-DimensionalPoint-LineDistance.html
	ध्रुव द्विगुण cross = गन्तव्यम्_point.cross(direction);
	प्रत्यागम cross * cross / दीर्घता_squared;
}

तार्किक isPolygonCCW(ध्रुव matrix::Vector2f *vertices, पूर्णाङ्क अङ्कः_vertices)
{
	// https://en.wikipedia.org/wiki/Shoelace_formula
	// Triangle formula
	भिन्न signed_area_2x = 0.f;

	प्रति (पूर्णाङ्क i = 0; i < अङ्कः_vertices; i++) {
		ध्रुव पूर्णाङ्क j = (i + 1) % अङ्कः_vertices;
		signed_area_2x += vertices[i](0) * vertices[j](1)
				  - vertices[j](0) * vertices[i](1);
	}

	प्रत्यागम signed_area_2x > 0.f;
}

} // namespace geofence_utils
