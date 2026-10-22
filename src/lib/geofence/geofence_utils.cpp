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

int32_t meters_ಗಮ್ಯ_Cm(ದಶಮಾಂಶ m) { ಹಿಂತಿರುಗಿ roundf(m * CM_ಪ್ರತಿ_M); }

} // namespace


ತಾರ್ಕಿಕ PlannerPolygons::allocate(ಪೂರ್ಣಾಂಕ max_nodes)
{
	freeBuffers();

	ಆದರೆ (max_nodes > 0) {
		_x_cm = ಹೊಸ int32_t[max_nodes];
		_y_cm = ಹೊಸ int32_t[max_nodes];
		ಗಂಟು_not_on_optimal_ಮಾರ್ಗ = ಹೊಸ ತಾರ್ಕಿಕ[max_nodes];

		ಆದರೆ (_x_cm != ಶೂನ್ಯಸೂಚಕ && _y_cm != ಶೂನ್ಯಸೂಚಕ && ಗಂಟು_not_on_optimal_ಮಾರ್ಗ != ಶೂನ್ಯಸೂಚಕ) {
			_max_nodes = max_nodes;

		} ಇಲ್ಲದಿದ್ದರೆ {
			freeBuffers();
		}
	}

	reset();

	ಹಿಂತಿರುಗಿ _max_nodes == max_nodes;
}

ಶೂನ್ಯ PlannerPolygons::freeBuffers()
{
	ಅಳಿಸು[] _x_cm;
	ಅಳಿಸು[] _y_cm;
	ಅಳಿಸು[] ಗಂಟು_not_on_optimal_ಮಾರ್ಗ;
	_x_cm = ಶೂನ್ಯಸೂಚಕ;
	_y_cm = ಶೂನ್ಯಸೂಚಕ;
	ಗಂಟು_not_on_optimal_ಮಾರ್ಗ = ಶೂನ್ಯಸೂಚಕ;
	_max_nodes = 0;
}

ಶೂನ್ಯ PlannerPolygons::ಗಣ_ಗಂಟು(ಪೂರ್ಣಾಂಕ idx, ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f &p)
{
	ಆದರೆ (_max_nodes < 1) {
		ಹಿಂತಿರುಗಿ;
	}

	idx = math::constrain(idx, 0, _max_nodes - 1);

	_x_cm[idx] = meters_ಗಮ್ಯ_Cm(p(0));
	_y_cm[idx] = meters_ಗಮ್ಯ_Cm(p(1));
}

PlannerPolygons::ಕೂಡಿಸು_ಫಲಿತಾಂಶ PlannerPolygons::ಕೂಡಿಸು_Polygon(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f *vertices_ಒಳ, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_vertices,
		ತಾರ್ಕಿಕ is_inclusion_zone, ದಶಮಾಂಶ margin)
{
	ಆದರೆ (ಅಂಕೆ_polygons >= kMaxPolygons || ಅಂಕೆ_nodes_2 + ಅಂಕೆ_vertices > _max_nodes) {
		ಹಿಂತಿರುಗಿ ಕೂಡಿಸು_ಫಲಿತಾಂಶ::BudgetExceeded;
	}

	ಆದರೆ (margin < -FLT_EPSILON || ಅಂಕೆ_vertices < 3) { ಹಿಂತಿರುಗಿ ಕೂಡಿಸು_ಫಲಿತಾಂಶ::Degenerate; }

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_vertices; ++i) {
		// Reject self-intersecting (non-simple) polygons.
		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f &a = vertices_ಒಳ[i];
		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f &b = vertices_ಒಳ[(i + 1) % ಅಂಕೆ_vertices];

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = i + 2; j < ಅಂಕೆ_vertices; ++j) {
			ಆದರೆ (i == 0 && j == ಅಂಕೆ_vertices - 1) {
				ಮುಂದುವರಿ; // (last edge, first edge) are adjacent
			}

			ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f &c = vertices_ಒಳ[j];
			ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f &d = vertices_ಒಳ[(j + 1) % ಅಂಕೆ_vertices];

			ಆದರೆ (segmentsIntersect(
				    meters_ಗಮ್ಯ_Cm(a(0)), meters_ಗಮ್ಯ_Cm(a(1)), meters_ಗಮ್ಯ_Cm(b(0)), meters_ಗಮ್ಯ_Cm(b(1)),
				    meters_ಗಮ್ಯ_Cm(c(0)), meters_ಗಮ್ಯ_Cm(c(1)), meters_ಗಮ್ಯ_Cm(d(0)), meters_ಗಮ್ಯ_Cm(d(1))) != Seg_Seg_ಫಲಿತಾಂಶ::Disjoint) {
				ಹಿಂತಿರುಗಿ ಕೂಡಿಸು_ಫಲಿತಾಂಶ::Degenerate;
			}
		}

		// Reject vertices outside the usable fixed-point range
		ಆದರೆ (!ಒಳ_Fixed_ಬಿಂದು_Range(vertices_ಒಳ[i](0)) || !ಒಳ_Fixed_ಬಿಂದು_Range(vertices_ಒಳ[i](1))) {
			ಹಿಂತಿರುಗಿ ಕೂಡಿಸು_ಫಲಿತಾಂಶ::ಹೊರಹೋಗು_ದ_Range;
		}
	}

	// Canonical orientation: walking vertices in stored order, INSIDE is on
	// the left of each edge. CCW for exclusion (bounded interior is forbidden),
	// CW for inclusion (unbounded exterior is forbidden). Reverse iff input
	// doesn't already match.
	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ ಹಿಮ್ಮುಖ = (isPolygonCCW(vertices_ಒಳ, ಅಂಕೆ_vertices) == is_inclusion_zone);

	// Index into vertices_in in stored order (accounts for possible reversal).
	ಸ್ವಯಂಚಾಲಿತ ಶೃಂಗ = [&](ಪೂರ್ಣಾಂಕ i) -> ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f & {
		ಹಿಂತಿರುಗಿ vertices_ಒಳ[ಹಿಮ್ಮುಖ ? (ಅಂಕೆ_vertices - 1 - i) : i];
	};

	PolygonInfo &poly = _polygons[ಅಂಕೆ_polygons];
	poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ  = ಅಂಕೆ_nodes_2;
	poly.is_inclusion = is_inclusion_zone;

	ಪೂರ್ಣಾಂಕ ಹೊರಹೋಗು_idx = 0;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_vertices; i++) {
		ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ prev = (i + ಅಂಕೆ_vertices - 1) % ಅಂಕೆ_vertices;
		ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಮುಂದಿನ = (i + 1) % ಅಂಕೆ_vertices;

		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f ಅಂಚು_ಒಳ  = ಶೃಂಗ(i) - ಶೃಂಗ(prev);
		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f ಅಂಚು_ಹೊರಹೋಗು = ಶೃಂಗ(ಮುಂದಿನ) - ಶೃಂಗ(i);

		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಅಂಚು_ಒಳ_norm = ಅಂಚು_ಒಳ.norm();
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಅಂಚು_ಹೊರಹೋಗು_norm = ಅಂಚು_ಹೊರಹೋಗು.norm();

		ಆದರೆ (ಅಂಚು_ಒಳ_norm < FLT_EPSILON || ಅಂಚು_ಹೊರಹೋಗು_norm < FLT_EPSILON) {
			ಹಿಂತಿರುಗಿ ಕೂಡಿಸು_ಫಲಿತಾಂಶ::Degenerate;
		}

		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f ಅಂಚು_ಒಳ_normalized = ಅಂಚು_ಒಳ / ಅಂಚು_ಒಳ_norm;
		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f ಅಂಚು_ಹೊರಹೋಗು_normalized = ಅಂಚು_ಹೊರಹೋಗು / ಅಂಚು_ಹೊರಹೋಗು_norm;

		// Inward normals of the two adjacent edges.
		// Canonical stored orientation: left side = forbidden region, right side = allowed region
		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f n_ಒಳ  = ಮಾತೃಕೆ::Vector2f{-ಅಂಚು_ಒಳ_normalized(1), ಅಂಚು_ಒಳ_normalized(0)};
		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f n_ಹೊರಹೋಗು = ಮಾತೃಕೆ::Vector2f{-ಅಂಚು_ಹೊರಹೋಗು_normalized(1), ಅಂಚು_ಹೊರಹೋಗು_normalized(0)};

		ಮಾತೃಕೆ::Vector2f bisector = n_ಒಳ + n_ಹೊರಹೋಗು;
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ bisector_len  = bisector.ಉದ್ದಳತೆ();

		ಆದರೆ (bisector_len < FLT_EPSILON) {
			ಹಿಂತಿರುಗಿ ಕೂಡಿಸು_ಫಲಿತಾಂಶ::Degenerate; // antiparallel edges - polygon doubles back
		}

		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f normalized_bisector = bisector / bisector_len;

		// The following bit ensures that:
		//  - The original edges are moved away from the forbidden interior by `margin`
		//  - Sharp convex corners (<60deg) are "cut off" with a new edge that is exactly
		//    `margin` away from the original vertex. For this the vertex is replaced by
		//    two new ones.

		// Only split convex corners - others are ignored, see _node_not_on_optimal_path
		ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ ಮೂಲೆ_convex = ಅಂಚು_ಒಳ_normalized(0) * ಅಂಚು_ಹೊರಹೋಗು_normalized(1)
					   - ಅಂಚು_ಒಳ_normalized(1) * ಅಂಚು_ಹೊರಹೋಗು_normalized(0) > 0.f;

		// Scaling needed wrt the normalised bisector to achieve parallel edges
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಸ್ಥಾನಾಂತರ_scale = 2.f / bisector_len;

		// bisector_len = 2*sin(interior angle/2); split when interior angle < 60 deg <=> bisector_len < 1.
		ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ angle_sharp = bisector_len < 1;

		// If margin zero, do not split - would output the same vertex twice.
		ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ margin_nonzero = margin > FLT_EPSILON;

		// If we do not have enough space, do not split the vertex.
		// This never happens if the planner-internal node buffer is 2x the original buffer.
		ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ space_for_ವಿಭಜಿಸು_vertices = ಅಂಕೆ_nodes_2 + ಹೊರಹೋಗು_idx + 1 < _max_nodes;

		ಆದರೆ (ಮೂಲೆ_convex && angle_sharp && margin_nonzero && space_for_ವಿಭಜಿಸು_vertices) {

			// Sharp corner: cut it off along the line
			//  - perpendicular to normalized_bisector
			//  - going through base := vertex(i) - normalized_bisector * margin

			ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f base = ಶೃಂಗ(i) - normalized_bisector * margin;
			ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f perp = {-normalized_bisector(1), normalized_bisector(0)};

			// Construct s such that:
			//  base + perp*s is on the offset incoming edge
			//  base - perp*s is on the offset outgoing edge
			ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ e_ಹೊರಹೋಗು_dot_b = ಅಂಚು_ಹೊರಹೋಗು_normalized.dot(normalized_bisector);
			ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ s = margin * (1.f - n_ಹೊರಹೋಗು.dot(normalized_bisector)) / e_ಹೊರಹೋಗು_dot_b;

			ಗಣ_ಗಂಟು(poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ + ಹೊರಹೋಗು_idx++, base + perp * s);
			ಗಣ_ಗಂಟು(poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ + ಹೊರಹೋಗು_idx++, base - perp * s);

		} ಇಲ್ಲದಿದ್ದರೆ {
			// Regular corner or no space for split: single bisector vertex displaced by margin / sin(interior angle/2).
			ಗಣ_ಗಂಟು(poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ + ಹೊರಹೋಗು_idx++, ಶೃಂಗ(i) - normalized_bisector * (margin * ಸ್ಥಾನಾಂತರ_scale));
		}
	}

	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಹೊರಹೋಗು = ಹೊರಹೋಗು_idx;
	poly.ಅಂಕೆ_vertices = ಅಂಕೆ_ಹೊರಹೋಗು;

	computeBoundingBox(poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ, ಅಂಕೆ_ಹೊರಹೋಗು, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// In canonical orientation, the node is not possibly on the shortest
	// path if the vertex curves right or straight (forbidden region >= 180
	// deg)
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_ಹೊರಹೋಗು; i++) {
		ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ prev = poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ + (i + ಅಂಕೆ_ಹೊರಹೋಗು - 1) % ಅಂಕೆ_ಹೊರಹೋಗು;
		ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ curr = poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ + i;
		ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಮುಂದಿನ = poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ + (i + 1) % ಅಂಕೆ_ಹೊರಹೋಗು;
		ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ curves_ಬಲ_or_straight = orient2d(_x_cm[prev], _y_cm[prev],
						      _x_cm[curr], _y_cm[curr],
						      _x_cm[ಮುಂದಿನ], _y_cm[ಮುಂದಿನ]) <= 0;
		ಗಂಟು_not_on_optimal_ಮಾರ್ಗ[curr] = curves_ಬಲ_or_straight;
	}

	ಅಂಕೆ_nodes_2 += ಅಂಕೆ_ಹೊರಹೋಗು;
	++ಅಂಕೆ_polygons;
	ಹಿಂತಿರುಗಿ ಕೂಡಿಸು_ಫಲಿತಾಂಶ::Success;
}

PlannerPolygons::ಕೂಡಿಸು_ಫಲಿತಾಂಶ PlannerPolygons::ಕೂಡಿಸು_Approx_Circle(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f &center, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ circle_radius,
		ದಶಮಾಂಶ margin, ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ is_inclusion_zone)
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

	ಆದರೆ (ಅಂಕೆ_polygons >= kMaxPolygons || ಅಂಕೆ_nodes_2 + kCircleApproxVertices > _max_nodes) {
		ಹಿಂತಿರುಗಿ ಕೂಡಿಸು_ಫಲಿತಾಂಶ::BudgetExceeded;
	}

	ಆದರೆ (circle_radius <= 0.1f) { ಹಿಂತಿರುಗಿ ಕೂಡಿಸು_ಫಲಿತಾಂಶ::Degenerate; }

	// ensures that the k-gon is, w.r.t the real circle:
	//  - over-approximation of circle grown by margin, if exclusion
	//  - under-approximation of circle shrunk by margin, if inclusion
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ k_gon_radius = is_inclusion_zone
				   ? circle_radius - margin
				   : circle_radius / cosf(M_PI_F / kCircleApproxVertices) + margin;

	ಆದರೆ (k_gon_radius <= FLT_EPSILON) { // Negative radius - circle empty
		ಹಿಂತಿರುಗಿ ಕೂಡಿಸು_ಫಲಿತಾಂಶ::Degenerate;
	}

	ಆದರೆ (!ಒಳ_Fixed_ಬಿಂದು_Range(fabsf(center(0)) + k_gon_radius)
	    || !ಒಳ_Fixed_ಬಿಂದು_Range(fabsf(center(1)) + k_gon_radius)) {
		ಹಿಂತಿರುಗಿ ಕೂಡಿಸು_ಫಲಿತಾಂಶ::ಹೊರಹೋಗು_ದ_Range;
	}

	// Canonical orientation: CCW for exclusion, CW for inclusion
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ delta_angle = (is_inclusion_zone ? -1.f : 1.f) * 2.f * M_PI_F / kCircleApproxVertices;

	PolygonInfo &poly = _polygons[ಅಂಕೆ_polygons];
	poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ = ಅಂಕೆ_nodes_2;
	poly.ಅಂಕೆ_vertices = kCircleApproxVertices;
	poly.is_inclusion = is_inclusion_zone;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < kCircleApproxVertices; i++) {
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ angle = (i + 0.5f) * delta_angle;
		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f p = center + ಮಾತೃಕೆ::Vector2f{
			k_gon_radius * cosf(angle), k_gon_radius * sinf(angle)
		};
		ಗಣ_ಗಂಟು(poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ + i, p);
	}

	computeBoundingBox(poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ, kCircleApproxVertices, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// For regular k-gons, all corners are either convex or reflex based on
	// is_inclusion, so the reduced visibility graph criterion is simplified
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < kCircleApproxVertices; i++) {
		ಗಂಟು_not_on_optimal_ಮಾರ್ಗ[poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ + i] = is_inclusion_zone;
	}

	ಅಂಕೆ_nodes_2 += kCircleApproxVertices;
	++ಅಂಕೆ_polygons;
	ಹಿಂತಿರುಗಿ ಕೂಡಿಸು_ಫಲಿತಾಂಶ::Success;
}

ಶೂನ್ಯ PlannerPolygons::computeBoundingBox(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಆರಂಭ_ಸೂಚ್ಯಂಕ, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಅಂಕೆ_vertices,
		int32_t &min_x, int32_t &max_x, int32_t &min_y, int32_t &max_y)
{

	// Store the bounding box, so we can do a very cheap check against the
	// bounding box, and only do the full line-polygon intersection check if
	// the line intersects the bounding box.

	min_x = max_x = _x_cm[ಆರಂಭ_ಸೂಚ್ಯಂಕ];
	min_y = max_y = _y_cm[ಆರಂಭ_ಸೂಚ್ಯಂಕ];

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < ಅಂಕೆ_vertices; i++) {
		ಸ್ಥಿರಾಂಕ int32_t x = _x_cm[ಆರಂಭ_ಸೂಚ್ಯಂಕ + i];
		ಸ್ಥಿರಾಂಕ int32_t y = _y_cm[ಆರಂಭ_ಸೂಚ್ಯಂಕ + i];

		ಆದರೆ (x < min_x) { min_x = x; }

		ಆದರೆ (x > max_x) { max_x = x; }

		ಆದರೆ (y < min_y) { min_y = y; }

		ಆದರೆ (y > max_y) { max_y = y; }
	}
}

ತಾರ್ಕಿಕ PlannerPolygons::ಬಿಂದು_Inside_Interior_Cone(ಸ್ಥಿರಾಂಕ PolygonInfo &poly,
		int32_t px, int32_t py, ಪೂರ್ಣಾಂಕ v) ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ prev = poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ + ((v + poly.ಅಂಕೆ_vertices - 1) % poly.ಅಂಕೆ_vertices);
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ curr = poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ + v;
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಮುಂದಿನ = poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ + ((v + 1) % poly.ಅಂಕೆ_vertices);

	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ p_ಎಡ_ದ_incoming  = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], px, py);
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ p_ಎಡ_ದ_outgoing  = orient2d(_x_cm[curr], _y_cm[curr], _x_cm[ಮುಂದಿನ], _y_cm[ಮುಂದಿನ], px, py);
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ is_convex           = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], _x_cm[ಮುಂದಿನ], _y_cm[ಮುಂದಿನ]);

	ಹಿಂತಿರುಗಿ is_convex > 0  // If convex==0 (exact 180 deg "corner") the two cases are equivalent
	       ? (p_ಎಡ_ದ_incoming > 0 && p_ಎಡ_ದ_outgoing > 0)   // Convex: interior = intersection of two half-planes
	       : (p_ಎಡ_ದ_incoming > 0 || p_ಎಡ_ದ_outgoing > 0);  // Reflex: interior = union of two half-planes
}

ತಾರ್ಕಿಕ PlannerPolygons::intersects_Inside_ದ(ಸ್ಥಿರಾಂಕ PolygonInfo &poly,
		int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ಸ್ಥಿರಾಂಕ
{
	// Single pass over polygon edges, doing two jobs:
	//  (1) classify the test segment vs each polygon edge. early-return on:
	//       - strict crossings (line segment - polygon edge)
	//       - interior crossing at vertex
	//  (2) accumulate midpoint winding contribution -- if (1) is inconclusive,
	//      this decides if the line is inside or outside

	ಪೂರ್ಣಾಂಕ wn = 0;
	ತಾರ್ಕಿಕ mid_on_boundary = ಸುಳ್ಳು;

	// Rather than calculating the midpoint (mid = (s + e)/2) explicitly,
	// which will lead to rounding error for uneven numbers, we instead
	// scale all other inputs by two, reducing the range by a factor of two
	// (2^31 cm = 21400 km -> 2^30 cm = 10700 km)
	ಸ್ಥಿರಾಂಕ int32_t twice_mid_x = s_x + e_x;
	ಸ್ಥಿರಾಂಕ int32_t twice_mid_y = s_y + e_y;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < poly.ಅಂಕೆ_vertices; i++) {

		ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ prev_idx = poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ + (i + poly.ಅಂಕೆ_vertices - 1) % poly.ಅಂಕೆ_vertices;
		ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ curr_idx = poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ + i;

		ಸ್ಥಿರಾಂಕ int32_t ax = _x_cm[prev_idx], ay = _y_cm[prev_idx];
		ಸ್ಥಿರಾಂಕ int32_t bx = _x_cm[curr_idx], by = _y_cm[curr_idx];

		// (1) test segment vs polygon edge (a, b)
		ಆಯ್ಕೆ (segmentsIntersect(ax, ay, bx, by, s_x, s_y, e_x, e_y)) {
		ಸ್ಥಿತಿ Seg_Seg_ಫಲಿತಾಂಶ::Cross:
			ಹಿಂತಿರುಗಿ ನಿಜ;

		// skip AInsideCD - a from this segment is b from the adjacent segment
		ಸ್ಥಿತಿ Seg_Seg_ಫಲಿತಾಂಶ::BInsideCD:

			// Polygon vertex b is exactly on the interior of the test segment.
			// Return intersecting if it pokes into the polygon interior through vertex b.
			ಆದರೆ (ಬಿಂದು_Inside_Interior_Cone(poly, s_x, s_y, i) || ಬಿಂದು_Inside_Interior_Cone(poly, e_x, e_y, i)) {
				ಹಿಂತಿರುಗಿ ನಿಜ;
			}

			ನಿಲ್ಲಿಸು;

		ಪೂರ್ವನಿಯೋಜಿತ:
			ನಿಲ್ಲಿಸು;
		}

		// (2) midpoint winding contribution
		// Ref: Dan Sunday, Inclusion of a Point in a Polygon
		// https://web.archive.org/web/20130126163405/http://geomalgorithms.com/a03-_inclusion.html

		ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ side = orient2d(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);

		ಆದರೆ (side == 0) {

			ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ mid_on_open_ab = collinearBetween(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);
			ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ mid_is_b = 2 * bx == twice_mid_x && 2 * by == twice_mid_y;

			// Skip point a - a from this segment is b from the adjacent segment
			ಆದರೆ (mid_on_open_ab || mid_is_b) {
				mid_on_boundary = ನಿಜ;
			}

		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (2 * ay <= twice_mid_y) {
			ಆದರೆ (2 * by > twice_mid_y && side > 0) { wn++; }

		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (2 * by <= twice_mid_y && side < 0) { wn--; }
	}


	// Midpoint on boundary, segment runs along it (strict crossing detected earlier).
	// Non-intersecting in either zone.
	ಆದರೆ (mid_on_boundary) {
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	// Exclusion: violation when midpoint is inside (bounded). Inclusion: when outside (unbounded).
	// This is the only place we depart from the inside = left = forbidden canonical orientation,
	// necessary because of the point membership check.
	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ midpoint_ಒಳ_bounded_inside = (wn != 0);
	ಹಿಂತಿರುಗಿ midpoint_ಒಳ_bounded_inside != poly.is_inclusion;
}

ತಾರ್ಕಿಕ PlannerPolygons::intersectsAnyInside(int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರಾಂಕ int32_t seg_min_x = s_x < e_x ? s_x : e_x;
	ಸ್ಥಿರಾಂಕ int32_t seg_max_x = s_x > e_x ? s_x : e_x;
	ಸ್ಥಿರಾಂಕ int32_t seg_min_y = s_y < e_y ? s_y : e_y;
	ಸ್ಥಿರಾಂಕ int32_t seg_max_y = s_y > e_y ? s_y : e_y;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ p = 0; p < ಅಂಕೆ_polygons; p++) {
		ಸ್ಥಿರಾಂಕ PolygonInfo &poly = _polygons[p];

		ಆದರೆ (seg_max_x < poly.min_x || seg_min_x > poly.max_x ||
		    seg_max_y < poly.min_y || seg_min_y > poly.max_y) {
			// Segment is entirely outside this polygon.
			// For exclusion zones: outside = safe, skip.
			// For inclusion zones: outside = forbidden region, immediate violation.
			ಆದರೆ (poly.is_inclusion) { ಹಿಂತಿರುಗಿ ನಿಜ; }

			ಮುಂದುವರಿ;
		}

		ಆದರೆ (intersects_Inside_ದ(poly, s_x, s_y, e_x, e_y)) {
			ಹಿಂತಿರುಗಿ ನಿಜ;
		}
	}

	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ PlannerPolygons::ಅಂಚು_Bitangent(ಪೂರ್ಣಾಂಕ a, int32_t bx, int32_t by) ಸ್ಥಿರಾಂಕ
{
	// Find the polygon owning vertex index a. Indices not belonging to any
	// polygon (in particular the destination at index 0) are trivially bitangent.

	ಆದರೆ (a == dest_ಸೂಚ್ಯಂಕ()) { ಹಿಂತಿರುಗಿ ನಿಜ; }

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ p = 0; p < ಅಂಕೆ_polygons; p++) {
		ಸ್ಥಿರಾಂಕ PolygonInfo &poly = _polygons[p];

		ಆದರೆ (a >= poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ && a < poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ + poly.ಅಂಕೆ_vertices) {
			// Define point c = a + (a-b) = 2a - b, so that ac = ab mirrored about a
			// Then: Edge is bitangent at a <=> neither ab nor ac pokes into the interior
			int32_t cx = 2 * _x_cm[a] - bx;
			int32_t cy = 2 * _y_cm[a] - by;
			ಹಿಂತಿರುಗಿ !ಬಿಂದು_Inside_Interior_Cone(poly, bx, by, a - poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ)
			       && !ಬಿಂದು_Inside_Interior_Cone(poly, cx, cy, a - poly.ಆರಂಭ_ಸೂಚ್ಯಂಕ);
		}
	}

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ PlannerPolygons::ಅಂಚು_Bitangent(ಪೂರ್ಣಾಂಕ a, ಪೂರ್ಣಾಂಕ b) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ ಅಂಚು_Bitangent(a, _x_cm[b], _y_cm[b]);
}

ತಾರ್ಕಿಕ PlannerPolygons::ಅಂಚು_Bitangent(ಪೂರ್ಣಾಂಕ a, ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f &b) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ ಅಂಚು_Bitangent(a, meters_ಗಮ್ಯ_Cm(b(0)), meters_ಗಮ್ಯ_Cm(b(1)));
}

ತಾರ್ಕಿಕ PlannerPolygons::ಗಣ_Destination(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f &p)
{
	ಗಣ_ಗಂಟು(dest_ಸೂಚ್ಯಂಕ(), p);

	ಸ್ಥಿರಾಂಕ int32_t x_cm = meters_ಗಮ್ಯ_Cm(p(0));
	ಸ್ಥಿರಾಂಕ int32_t y_cm = meters_ಗಮ್ಯ_Cm(p(1));

	ಆದರೆ (intersectsAnyInside(x_cm, y_cm, x_cm, y_cm)) {
		// If the destination breaches a geofence, still use it but
		// signal to the user that it is invalid.
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಮಾತೃಕೆ::Vector2f PlannerPolygons::getDestination() ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (_max_nodes < 1) {
		ಹಿಂತಿರುಗಿ ಮಾತೃಕೆ::Vector2f{NAN, NAN};
	}

	ಹಿಂತಿರುಗಿ ಗಂಟು_2(dest_ಸೂಚ್ಯಂಕ());
}

ತಾರ್ಕಿಕ PlannerPolygons::ಅಂಚು_Visible(ಪೂರ್ಣಾಂಕ a, ಪೂರ್ಣಾಂಕ b) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (a == b) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }

	ಆದರೆ (ಗಂಟು_not_on_optimal_ಮಾರ್ಗ[a] || ಗಂಟು_not_on_optimal_ಮಾರ್ಗ[b]) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }

	// Only bitangent edges can be on a shortest path; this check also cheaply
	// discards edges that violate the polygon at the vertex they come from.
	ಆದರೆ (!ಅಂಚು_Bitangent(a, b) || !ಅಂಚು_Bitangent(b, a)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }

	ಹಿಂತಿರುಗಿ !intersectsAnyInside(_x_cm[a], _y_cm[a], _x_cm[b], _y_cm[b]);
}

ತಾರ್ಕಿಕ PlannerPolygons::ಅಂಚು_Visible(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f &a, ಪೂರ್ಣಾಂಕ b) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (ಗಂಟು_not_on_optimal_ಮಾರ್ಗ[b]) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }

	ಆದರೆ (!ಅಂಚು_Bitangent(b, a)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }

	ಹಿಂತಿರುಗಿ !intersectsAnyInside(meters_ಗಮ್ಯ_Cm(a(0)), meters_ಗಮ್ಯ_Cm(a(1)), _x_cm[b], _y_cm[b]);
}

ತಾರ್ಕಿಕ PlannerPolygons::ಅಂಚು_Visible(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f &a, ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f &b) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ !intersectsAnyInside(meters_ಗಮ್ಯ_Cm(a(0)), meters_ಗಮ್ಯ_Cm(a(1)), meters_ಗಮ್ಯ_Cm(b(0)), meters_ಗಮ್ಯ_Cm(b(1)));
}

ದಶಮಾಂಶ PlannerPolygons::ಅಂಚು_ವೆಚ್ಚ(ಪೂರ್ಣಾಂಕ a, ಪೂರ್ಣಾಂಕ b) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ ಅಂಚು_Visible(a, b) ? (ಗಂಟು_2(a) - ಗಂಟು_2(b)).norm() : INFINITY;
}


ತಾರ್ಕಿಕ segmentsIntersectInclusive(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2d &a, ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2d &b,
				ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2d &c, ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2d &d)
{
	// Reject separated bounds, including gaps between collinear segments.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ axis = 0; axis < 2; ++axis) {
		ಆದರೆ (math::max(a(axis), b(axis)) < math::min(c(axis), d(axis))
		    || math::max(c(axis), d(axis)) < math::min(a(axis), b(axis))) {
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		}
	}

	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2d ab = b - a;
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2d cd = d - c;
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ side_c = ab.cross(c - a);
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ side_d = ab.cross(d - a);
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ side_a = cd.cross(a - c);
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ side_b = cd.cross(b - c);

	// Each segment must cross or touch the line through the other segment.
	ಹಿಂತಿರುಗಿ ((side_c <= 0.0 && side_d >= 0.0) || (side_c >= 0.0 && side_d <= 0.0))
	       && ((side_a <= 0.0 && side_b >= 0.0) || (side_a >= 0.0 && side_b <= 0.0));
}

ದ್ವಿಗುಣ ಬಿಂದು_ಗಮ್ಯ_Segment_ದೂರ_Squared(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2d &ಬಿಂದು,
				     ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2d &a, ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2d &b)
{
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2d direction = b - a;
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2d ಗಮ್ಯ_ಬಿಂದು = ಬಿಂದು - a;
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ ಉದ್ದಳತೆ_squared = direction.norm_squared();
	// along / length_squared locates the projection: 0 at a, 1 at b.
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ along = ಗಮ್ಯ_ಬಿಂದು.dot(direction);

	ಆದರೆ (along <= 0.0) {
		ಹಿಂತಿರುಗಿ ಗಮ್ಯ_ಬಿಂದು.norm_squared();

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (along >= ಉದ್ದಳತೆ_squared) {
		ಹಿಂತಿರುಗಿ (ಬಿಂದು - b).norm_squared();
	}

	// Squared distance to the line: |to_point x direction|^2 / |direction|^2.
	// https://mathworld.wolfram.com/2-DimensionalPoint-LineDistance.html
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ cross = ಗಮ್ಯ_ಬಿಂದು.cross(direction);
	ಹಿಂತಿರುಗಿ cross * cross / ಉದ್ದಳತೆ_squared;
}

ತಾರ್ಕಿಕ isPolygonCCW(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f *vertices, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_vertices)
{
	// https://en.wikipedia.org/wiki/Shoelace_formula
	// Triangle formula
	ದಶಮಾಂಶ signed_area_2x = 0.f;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_vertices; i++) {
		ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ j = (i + 1) % ಅಂಕೆ_vertices;
		signed_area_2x += vertices[i](0) * vertices[j](1)
				  - vertices[j](0) * vertices[i](1);
	}

	ಹಿಂತಿರುಗಿ signed_area_2x > 0.f;
}

} // namespace geofence_utils
