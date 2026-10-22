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

#ਅਨ੍ਤਰ੍ਭਾਵ "geofence_utils.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/geo/geo.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/mathlib/mathlib.h>

ਨਾਮਾਕਾਸ਼ geofence_utils
{

ਨਾਮਾਕਾਸ਼
{

int32_t meters_ਮੰਜ਼ਿਲ_Cm(ਦਸ਼ਮਲਵ m) { ਵਾਪਸ roundf(m * CM_ਪ੍ਰਤੀ_M); }

} // namespace


ਤਾਰਕਿਕ PlannerPolygons::allocate(ਪੂਰਨ max_nodes)
{
	freeBuffers();

	ਜੇ (max_nodes > 0) {
		_x_cm = ਨਵਾਂ int32_t[max_nodes];
		_y_cm = ਨਵਾਂ int32_t[max_nodes];
		ਗੰਢ_not_on_optimal_ਰਸਤਾ = ਨਵਾਂ ਤਾਰਕਿਕ[max_nodes];

		ਜੇ (_x_cm != ਖਾਲੀ_ਸੂਚਕ && _y_cm != ਖਾਲੀ_ਸੂਚਕ && ਗੰਢ_not_on_optimal_ਰਸਤਾ != ਖਾਲੀ_ਸੂਚਕ) {
			_max_nodes = max_nodes;

		} ਨਹੀਂਤਾਂ {
			freeBuffers();
		}
	}

	reset();

	ਵਾਪਸ _max_nodes == max_nodes;
}

ਖਾਲੀ PlannerPolygons::freeBuffers()
{
	ਮਿਟਾਓ[] _x_cm;
	ਮਿਟਾਓ[] _y_cm;
	ਮਿਟਾਓ[] ਗੰਢ_not_on_optimal_ਰਸਤਾ;
	_x_cm = ਖਾਲੀ_ਸੂਚਕ;
	_y_cm = ਖਾਲੀ_ਸੂਚਕ;
	ਗੰਢ_not_on_optimal_ਰਸਤਾ = ਖਾਲੀ_ਸੂਚਕ;
	_max_nodes = 0;
}

ਖਾਲੀ PlannerPolygons::ਸੈੱਟ_ਗੰਢ(ਪੂਰਨ idx, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &p)
{
	ਜੇ (_max_nodes < 1) {
		ਵਾਪਸ;
	}

	idx = math::constrain(idx, 0, _max_nodes - 1);

	_x_cm[idx] = meters_ਮੰਜ਼ਿਲ_Cm(p(0));
	_y_cm[idx] = meters_ਮੰਜ਼ਿਲ_Cm(p(1));
}

PlannerPolygons::ਜੋੜੋ_ਨਤੀਜਾ PlannerPolygons::ਜੋੜੋ_Polygon(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f *vertices_ਵਿੱਚ, ਪੂਰਨ ਅੰਕ_vertices,
		ਤਾਰਕਿਕ is_inclusion_zone, ਦਸ਼ਮਲਵ margin)
{
	ਜੇ (ਅੰਕ_polygons >= kMaxPolygons || ਅੰਕ_nodes_2 + ਅੰਕ_vertices > _max_nodes) {
		ਵਾਪਸ ਜੋੜੋ_ਨਤੀਜਾ::BudgetExceeded;
	}

	ਜੇ (margin < -FLT_EPSILON || ਅੰਕ_vertices < 3) { ਵਾਪਸ ਜੋੜੋ_ਨਤੀਜਾ::Degenerate; }

	ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_vertices; ++i) {
		// Reject self-intersecting (non-simple) polygons.
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &a = vertices_ਵਿੱਚ[i];
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &b = vertices_ਵਿੱਚ[(i + 1) % ਅੰਕ_vertices];

		ਲਈ (ਪੂਰਨ j = i + 2; j < ਅੰਕ_vertices; ++j) {
			ਜੇ (i == 0 && j == ਅੰਕ_vertices - 1) {
				ਜਾਰੀ; // (last edge, first edge) are adjacent
			}

			ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &c = vertices_ਵਿੱਚ[j];
			ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &d = vertices_ਵਿੱਚ[(j + 1) % ਅੰਕ_vertices];

			ਜੇ (segmentsIntersect(
				    meters_ਮੰਜ਼ਿਲ_Cm(a(0)), meters_ਮੰਜ਼ਿਲ_Cm(a(1)), meters_ਮੰਜ਼ਿਲ_Cm(b(0)), meters_ਮੰਜ਼ਿਲ_Cm(b(1)),
				    meters_ਮੰਜ਼ਿਲ_Cm(c(0)), meters_ਮੰਜ਼ਿਲ_Cm(c(1)), meters_ਮੰਜ਼ਿਲ_Cm(d(0)), meters_ਮੰਜ਼ਿਲ_Cm(d(1))) != Seg_Seg_ਨਤੀਜਾ::Disjoint) {
				ਵਾਪਸ ਜੋੜੋ_ਨਤੀਜਾ::Degenerate;
			}
		}

		// Reject vertices outside the usable fixed-point range
		ਜੇ (!ਵਿੱਚ_Fixed_ਬਿੰਦੂ_Range(vertices_ਵਿੱਚ[i](0)) || !ਵਿੱਚ_Fixed_ਬਿੰਦੂ_Range(vertices_ਵਿੱਚ[i](1))) {
			ਵਾਪਸ ਜੋੜੋ_ਨਤੀਜਾ::ਬਾਹਰਲਾ_ਦਾ_Range;
		}
	}

	// Canonical orientation: walking vertices in stored order, INSIDE is on
	// the left of each edge. CCW for exclusion (bounded interior is forbidden),
	// CW for inclusion (unbounded exterior is forbidden). Reverse iff input
	// doesn't already match.
	ਅਚਲ ਤਾਰਕਿਕ ਉਲਟ = (isPolygonCCW(vertices_ਵਿੱਚ, ਅੰਕ_vertices) == is_inclusion_zone);

	// Index into vertices_in in stored order (accounts for possible reversal).
	ਸਵੈਚਾਲਿਤ ਸ਼ੀਰਸ਼ = [&](ਪੂਰਨ i) -> ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f & {
		ਵਾਪਸ vertices_ਵਿੱਚ[ਉਲਟ ? (ਅੰਕ_vertices - 1 - i) : i];
	};

	PolygonInfo &poly = _polygons[ਅੰਕ_polygons];
	poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ  = ਅੰਕ_nodes_2;
	poly.is_inclusion = is_inclusion_zone;

	ਪੂਰਨ ਬਾਹਰਲਾ_idx = 0;

	ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_vertices; i++) {
		ਅਚਲ ਪੂਰਨ prev = (i + ਅੰਕ_vertices - 1) % ਅੰਕ_vertices;
		ਅਚਲ ਪੂਰਨ ਅਗਲਾ = (i + 1) % ਅੰਕ_vertices;

		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f ਕਿਨਾਰਾ_ਵਿੱਚ  = ਸ਼ੀਰਸ਼(i) - ਸ਼ੀਰਸ਼(prev);
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f ਕਿਨਾਰਾ_ਬਾਹਰਲਾ = ਸ਼ੀਰਸ਼(ਅਗਲਾ) - ਸ਼ੀਰਸ਼(i);

		ਅਚਲ ਦਸ਼ਮਲਵ ਕਿਨਾਰਾ_ਵਿੱਚ_norm = ਕਿਨਾਰਾ_ਵਿੱਚ.norm();
		ਅਚਲ ਦਸ਼ਮਲਵ ਕਿਨਾਰਾ_ਬਾਹਰਲਾ_norm = ਕਿਨਾਰਾ_ਬਾਹਰਲਾ.norm();

		ਜੇ (ਕਿਨਾਰਾ_ਵਿੱਚ_norm < FLT_EPSILON || ਕਿਨਾਰਾ_ਬਾਹਰਲਾ_norm < FLT_EPSILON) {
			ਵਾਪਸ ਜੋੜੋ_ਨਤੀਜਾ::Degenerate;
		}

		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f ਕਿਨਾਰਾ_ਵਿੱਚ_normalized = ਕਿਨਾਰਾ_ਵਿੱਚ / ਕਿਨਾਰਾ_ਵਿੱਚ_norm;
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f ਕਿਨਾਰਾ_ਬਾਹਰਲਾ_normalized = ਕਿਨਾਰਾ_ਬਾਹਰਲਾ / ਕਿਨਾਰਾ_ਬਾਹਰਲਾ_norm;

		// Inward normals of the two adjacent edges.
		// Canonical stored orientation: left side = forbidden region, right side = allowed region
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f n_ਵਿੱਚ  = ਮੈਟ੍ਰਿਕਸ::Vector2f{-ਕਿਨਾਰਾ_ਵਿੱਚ_normalized(1), ਕਿਨਾਰਾ_ਵਿੱਚ_normalized(0)};
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f n_ਬਾਹਰਲਾ = ਮੈਟ੍ਰਿਕਸ::Vector2f{-ਕਿਨਾਰਾ_ਬਾਹਰਲਾ_normalized(1), ਕਿਨਾਰਾ_ਬਾਹਰਲਾ_normalized(0)};

		ਮੈਟ੍ਰਿਕਸ::Vector2f bisector = n_ਵਿੱਚ + n_ਬਾਹਰਲਾ;
		ਅਚਲ ਦਸ਼ਮਲਵ bisector_len  = bisector.ਲੰਮਾਈ();

		ਜੇ (bisector_len < FLT_EPSILON) {
			ਵਾਪਸ ਜੋੜੋ_ਨਤੀਜਾ::Degenerate; // antiparallel edges - polygon doubles back
		}

		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f normalized_bisector = bisector / bisector_len;

		// The following bit ensures that:
		//  - The original edges are moved away from the forbidden interior by `margin`
		//  - Sharp convex corners (<60deg) are "cut off" with a new edge that is exactly
		//    `margin` away from the original vertex. For this the vertex is replaced by
		//    two new ones.

		// Only split convex corners - others are ignored, see _node_not_on_optimal_path
		ਅਚਲ ਤਾਰਕਿਕ ਕੋਨਾ_convex = ਕਿਨਾਰਾ_ਵਿੱਚ_normalized(0) * ਕਿਨਾਰਾ_ਬਾਹਰਲਾ_normalized(1)
					   - ਕਿਨਾਰਾ_ਵਿੱਚ_normalized(1) * ਕਿਨਾਰਾ_ਬਾਹਰਲਾ_normalized(0) > 0.f;

		// Scaling needed wrt the normalised bisector to achieve parallel edges
		ਅਚਲ ਦਸ਼ਮਲਵ ਵਿੱਥ_scale = 2.f / bisector_len;

		// bisector_len = 2*sin(interior angle/2); split when interior angle < 60 deg <=> bisector_len < 1.
		ਅਚਲ ਤਾਰਕਿਕ angle_sharp = bisector_len < 1;

		// If margin zero, do not split - would output the same vertex twice.
		ਅਚਲ ਤਾਰਕਿਕ margin_nonzero = margin > FLT_EPSILON;

		// If we do not have enough space, do not split the vertex.
		// This never happens if the planner-internal node buffer is 2x the original buffer.
		ਅਚਲ ਤਾਰਕਿਕ space_for_ਵੰਡੋ_vertices = ਅੰਕ_nodes_2 + ਬਾਹਰਲਾ_idx + 1 < _max_nodes;

		ਜੇ (ਕੋਨਾ_convex && angle_sharp && margin_nonzero && space_for_ਵੰਡੋ_vertices) {

			// Sharp corner: cut it off along the line
			//  - perpendicular to normalized_bisector
			//  - going through base := vertex(i) - normalized_bisector * margin

			ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f base = ਸ਼ੀਰਸ਼(i) - normalized_bisector * margin;
			ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f perp = {-normalized_bisector(1), normalized_bisector(0)};

			// Construct s such that:
			//  base + perp*s is on the offset incoming edge
			//  base - perp*s is on the offset outgoing edge
			ਅਚਲ ਦਸ਼ਮਲਵ e_ਬਾਹਰਲਾ_dot_b = ਕਿਨਾਰਾ_ਬਾਹਰਲਾ_normalized.dot(normalized_bisector);
			ਅਚਲ ਦਸ਼ਮਲਵ s = margin * (1.f - n_ਬਾਹਰਲਾ.dot(normalized_bisector)) / e_ਬਾਹਰਲਾ_dot_b;

			ਸੈੱਟ_ਗੰਢ(poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ + ਬਾਹਰਲਾ_idx++, base + perp * s);
			ਸੈੱਟ_ਗੰਢ(poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ + ਬਾਹਰਲਾ_idx++, base - perp * s);

		} ਨਹੀਂਤਾਂ {
			// Regular corner or no space for split: single bisector vertex displaced by margin / sin(interior angle/2).
			ਸੈੱਟ_ਗੰਢ(poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ + ਬਾਹਰਲਾ_idx++, ਸ਼ੀਰਸ਼(i) - normalized_bisector * (margin * ਵਿੱਥ_scale));
		}
	}

	ਅਚਲ ਪੂਰਨ ਅੰਕ_ਬਾਹਰਲਾ = ਬਾਹਰਲਾ_idx;
	poly.ਅੰਕ_vertices = ਅੰਕ_ਬਾਹਰਲਾ;

	computeBoundingBox(poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ, ਅੰਕ_ਬਾਹਰਲਾ, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// In canonical orientation, the node is not possibly on the shortest
	// path if the vertex curves right or straight (forbidden region >= 180
	// deg)
	ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_ਬਾਹਰਲਾ; i++) {
		ਅਚਲ ਪੂਰਨ prev = poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ + (i + ਅੰਕ_ਬਾਹਰਲਾ - 1) % ਅੰਕ_ਬਾਹਰਲਾ;
		ਅਚਲ ਪੂਰਨ curr = poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ + i;
		ਅਚਲ ਪੂਰਨ ਅਗਲਾ = poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ + (i + 1) % ਅੰਕ_ਬਾਹਰਲਾ;
		ਅਚਲ ਤਾਰਕਿਕ curves_ਸੱਜਾ_or_straight = orient2d(_x_cm[prev], _y_cm[prev],
						      _x_cm[curr], _y_cm[curr],
						      _x_cm[ਅਗਲਾ], _y_cm[ਅਗਲਾ]) <= 0;
		ਗੰਢ_not_on_optimal_ਰਸਤਾ[curr] = curves_ਸੱਜਾ_or_straight;
	}

	ਅੰਕ_nodes_2 += ਅੰਕ_ਬਾਹਰਲਾ;
	++ਅੰਕ_polygons;
	ਵਾਪਸ ਜੋੜੋ_ਨਤੀਜਾ::Success;
}

PlannerPolygons::ਜੋੜੋ_ਨਤੀਜਾ PlannerPolygons::ਜੋੜੋ_Approx_Circle(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &center, ਅਚਲ ਦਸ਼ਮਲਵ circle_radius,
		ਦਸ਼ਮਲਵ margin, ਅਚਲ ਤਾਰਕਿਕ is_inclusion_zone)
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

	ਜੇ (ਅੰਕ_polygons >= kMaxPolygons || ਅੰਕ_nodes_2 + kCircleApproxVertices > _max_nodes) {
		ਵਾਪਸ ਜੋੜੋ_ਨਤੀਜਾ::BudgetExceeded;
	}

	ਜੇ (circle_radius <= 0.1f) { ਵਾਪਸ ਜੋੜੋ_ਨਤੀਜਾ::Degenerate; }

	// ensures that the k-gon is, w.r.t the real circle:
	//  - over-approximation of circle grown by margin, if exclusion
	//  - under-approximation of circle shrunk by margin, if inclusion
	ਅਚਲ ਦਸ਼ਮਲਵ k_gon_radius = is_inclusion_zone
				   ? circle_radius - margin
				   : circle_radius / cosf(M_PI_F / kCircleApproxVertices) + margin;

	ਜੇ (k_gon_radius <= FLT_EPSILON) { // Negative radius - circle empty
		ਵਾਪਸ ਜੋੜੋ_ਨਤੀਜਾ::Degenerate;
	}

	ਜੇ (!ਵਿੱਚ_Fixed_ਬਿੰਦੂ_Range(fabsf(center(0)) + k_gon_radius)
	    || !ਵਿੱਚ_Fixed_ਬਿੰਦੂ_Range(fabsf(center(1)) + k_gon_radius)) {
		ਵਾਪਸ ਜੋੜੋ_ਨਤੀਜਾ::ਬਾਹਰਲਾ_ਦਾ_Range;
	}

	// Canonical orientation: CCW for exclusion, CW for inclusion
	ਅਚਲ ਦਸ਼ਮਲਵ delta_angle = (is_inclusion_zone ? -1.f : 1.f) * 2.f * M_PI_F / kCircleApproxVertices;

	PolygonInfo &poly = _polygons[ਅੰਕ_polygons];
	poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = ਅੰਕ_nodes_2;
	poly.ਅੰਕ_vertices = kCircleApproxVertices;
	poly.is_inclusion = is_inclusion_zone;

	ਲਈ (ਪੂਰਨ i = 0; i < kCircleApproxVertices; i++) {
		ਅਚਲ ਦਸ਼ਮਲਵ angle = (i + 0.5f) * delta_angle;
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f p = center + ਮੈਟ੍ਰਿਕਸ::Vector2f{
			k_gon_radius * cosf(angle), k_gon_radius * sinf(angle)
		};
		ਸੈੱਟ_ਗੰਢ(poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ + i, p);
	}

	computeBoundingBox(poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ, kCircleApproxVertices, poly.min_x, poly.max_x, poly.min_y, poly.max_y);

	// For regular k-gons, all corners are either convex or reflex based on
	// is_inclusion, so the reduced visibility graph criterion is simplified
	ਲਈ (ਪੂਰਨ i = 0; i < kCircleApproxVertices; i++) {
		ਗੰਢ_not_on_optimal_ਰਸਤਾ[poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ + i] = is_inclusion_zone;
	}

	ਅੰਕ_nodes_2 += kCircleApproxVertices;
	++ਅੰਕ_polygons;
	ਵਾਪਸ ਜੋੜੋ_ਨਤੀਜਾ::Success;
}

ਖਾਲੀ PlannerPolygons::computeBoundingBox(ਅਚਲ ਪੂਰਨ ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ, ਅਚਲ ਪੂਰਨ ਅੰਕ_vertices,
		int32_t &min_x, int32_t &max_x, int32_t &min_y, int32_t &max_y)
{

	// Store the bounding box, so we can do a very cheap check against the
	// bounding box, and only do the full line-polygon intersection check if
	// the line intersects the bounding box.

	min_x = max_x = _x_cm[ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ];
	min_y = max_y = _y_cm[ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ];

	ਲਈ (ਪੂਰਨ i = 1; i < ਅੰਕ_vertices; i++) {
		ਅਚਲ int32_t x = _x_cm[ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ + i];
		ਅਚਲ int32_t y = _y_cm[ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ + i];

		ਜੇ (x < min_x) { min_x = x; }

		ਜੇ (x > max_x) { max_x = x; }

		ਜੇ (y < min_y) { min_y = y; }

		ਜੇ (y > max_y) { max_y = y; }
	}
}

ਤਾਰਕਿਕ PlannerPolygons::ਬਿੰਦੂ_Inside_Interior_Cone(ਅਚਲ PolygonInfo &poly,
		int32_t px, int32_t py, ਪੂਰਨ v) ਅਚਲ
{
	ਅਚਲ ਪੂਰਨ prev = poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ + ((v + poly.ਅੰਕ_vertices - 1) % poly.ਅੰਕ_vertices);
	ਅਚਲ ਪੂਰਨ curr = poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ + v;
	ਅਚਲ ਪੂਰਨ ਅਗਲਾ = poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ + ((v + 1) % poly.ਅੰਕ_vertices);

	ਅਚਲ ਪੂਰਨ p_ਖੱਬਾ_ਦਾ_incoming  = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], px, py);
	ਅਚਲ ਪੂਰਨ p_ਖੱਬਾ_ਦਾ_outgoing  = orient2d(_x_cm[curr], _y_cm[curr], _x_cm[ਅਗਲਾ], _y_cm[ਅਗਲਾ], px, py);
	ਅਚਲ ਪੂਰਨ is_convex           = orient2d(_x_cm[prev], _y_cm[prev], _x_cm[curr], _y_cm[curr], _x_cm[ਅਗਲਾ], _y_cm[ਅਗਲਾ]);

	ਵਾਪਸ is_convex > 0  // If convex==0 (exact 180 deg "corner") the two cases are equivalent
	       ? (p_ਖੱਬਾ_ਦਾ_incoming > 0 && p_ਖੱਬਾ_ਦਾ_outgoing > 0)   // Convex: interior = intersection of two half-planes
	       : (p_ਖੱਬਾ_ਦਾ_incoming > 0 || p_ਖੱਬਾ_ਦਾ_outgoing > 0);  // Reflex: interior = union of two half-planes
}

ਤਾਰਕਿਕ PlannerPolygons::intersects_Inside_ਦਾ(ਅਚਲ PolygonInfo &poly,
		int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ਅਚਲ
{
	// Single pass over polygon edges, doing two jobs:
	//  (1) classify the test segment vs each polygon edge. early-return on:
	//       - strict crossings (line segment - polygon edge)
	//       - interior crossing at vertex
	//  (2) accumulate midpoint winding contribution -- if (1) is inconclusive,
	//      this decides if the line is inside or outside

	ਪੂਰਨ wn = 0;
	ਤਾਰਕਿਕ mid_on_boundary = ਝੂਠ;

	// Rather than calculating the midpoint (mid = (s + e)/2) explicitly,
	// which will lead to rounding error for uneven numbers, we instead
	// scale all other inputs by two, reducing the range by a factor of two
	// (2^31 cm = 21400 km -> 2^30 cm = 10700 km)
	ਅਚਲ int32_t twice_mid_x = s_x + e_x;
	ਅਚਲ int32_t twice_mid_y = s_y + e_y;

	ਲਈ (ਪੂਰਨ i = 0; i < poly.ਅੰਕ_vertices; i++) {

		ਅਚਲ ਪੂਰਨ prev_idx = poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ + (i + poly.ਅੰਕ_vertices - 1) % poly.ਅੰਕ_vertices;
		ਅਚਲ ਪੂਰਨ curr_idx = poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ + i;

		ਅਚਲ int32_t ax = _x_cm[prev_idx], ay = _y_cm[prev_idx];
		ਅਚਲ int32_t bx = _x_cm[curr_idx], by = _y_cm[curr_idx];

		// (1) test segment vs polygon edge (a, b)
		ਚੋਣ (segmentsIntersect(ax, ay, bx, by, s_x, s_y, e_x, e_y)) {
		ਹਾਲਤ Seg_Seg_ਨਤੀਜਾ::Cross:
			ਵਾਪਸ ਸੱਚ;

		// skip AInsideCD - a from this segment is b from the adjacent segment
		ਹਾਲਤ Seg_Seg_ਨਤੀਜਾ::BInsideCD:

			// Polygon vertex b is exactly on the interior of the test segment.
			// Return intersecting if it pokes into the polygon interior through vertex b.
			ਜੇ (ਬਿੰਦੂ_Inside_Interior_Cone(poly, s_x, s_y, i) || ਬਿੰਦੂ_Inside_Interior_Cone(poly, e_x, e_y, i)) {
				ਵਾਪਸ ਸੱਚ;
			}

			ਰੋਕੋ;

		ਮੂਲ:
			ਰੋਕੋ;
		}

		// (2) midpoint winding contribution
		// Ref: Dan Sunday, Inclusion of a Point in a Polygon
		// https://web.archive.org/web/20130126163405/http://geomalgorithms.com/a03-_inclusion.html

		ਅਚਲ ਪੂਰਨ side = orient2d(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);

		ਜੇ (side == 0) {

			ਅਚਲ ਤਾਰਕਿਕ mid_on_open_ab = collinearBetween(2 * ax, 2 * ay, 2 * bx, 2 * by, twice_mid_x, twice_mid_y);
			ਅਚਲ ਤਾਰਕਿਕ mid_is_b = 2 * bx == twice_mid_x && 2 * by == twice_mid_y;

			// Skip point a - a from this segment is b from the adjacent segment
			ਜੇ (mid_on_open_ab || mid_is_b) {
				mid_on_boundary = ਸੱਚ;
			}

		} ਨਹੀਂਤਾਂ ਜੇ (2 * ay <= twice_mid_y) {
			ਜੇ (2 * by > twice_mid_y && side > 0) { wn++; }

		} ਨਹੀਂਤਾਂ ਜੇ (2 * by <= twice_mid_y && side < 0) { wn--; }
	}


	// Midpoint on boundary, segment runs along it (strict crossing detected earlier).
	// Non-intersecting in either zone.
	ਜੇ (mid_on_boundary) {
		ਵਾਪਸ ਝੂਠ;
	}

	// Exclusion: violation when midpoint is inside (bounded). Inclusion: when outside (unbounded).
	// This is the only place we depart from the inside = left = forbidden canonical orientation,
	// necessary because of the point membership check.
	ਅਚਲ ਤਾਰਕਿਕ midpoint_ਵਿੱਚ_bounded_inside = (wn != 0);
	ਵਾਪਸ midpoint_ਵਿੱਚ_bounded_inside != poly.is_inclusion;
}

ਤਾਰਕਿਕ PlannerPolygons::intersectsAnyInside(int32_t s_x, int32_t s_y, int32_t e_x, int32_t e_y) ਅਚਲ
{
	ਅਚਲ int32_t seg_min_x = s_x < e_x ? s_x : e_x;
	ਅਚਲ int32_t seg_max_x = s_x > e_x ? s_x : e_x;
	ਅਚਲ int32_t seg_min_y = s_y < e_y ? s_y : e_y;
	ਅਚਲ int32_t seg_max_y = s_y > e_y ? s_y : e_y;

	ਲਈ (ਪੂਰਨ p = 0; p < ਅੰਕ_polygons; p++) {
		ਅਚਲ PolygonInfo &poly = _polygons[p];

		ਜੇ (seg_max_x < poly.min_x || seg_min_x > poly.max_x ||
		    seg_max_y < poly.min_y || seg_min_y > poly.max_y) {
			// Segment is entirely outside this polygon.
			// For exclusion zones: outside = safe, skip.
			// For inclusion zones: outside = forbidden region, immediate violation.
			ਜੇ (poly.is_inclusion) { ਵਾਪਸ ਸੱਚ; }

			ਜਾਰੀ;
		}

		ਜੇ (intersects_Inside_ਦਾ(poly, s_x, s_y, e_x, e_y)) {
			ਵਾਪਸ ਸੱਚ;
		}
	}

	ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ PlannerPolygons::ਕਿਨਾਰਾ_Bitangent(ਪੂਰਨ a, int32_t bx, int32_t by) ਅਚਲ
{
	// Find the polygon owning vertex index a. Indices not belonging to any
	// polygon (in particular the destination at index 0) are trivially bitangent.

	ਜੇ (a == dest_ਸੂਚਕਾਂਕ()) { ਵਾਪਸ ਸੱਚ; }

	ਲਈ (ਪੂਰਨ p = 0; p < ਅੰਕ_polygons; p++) {
		ਅਚਲ PolygonInfo &poly = _polygons[p];

		ਜੇ (a >= poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ && a < poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ + poly.ਅੰਕ_vertices) {
			// Define point c = a + (a-b) = 2a - b, so that ac = ab mirrored about a
			// Then: Edge is bitangent at a <=> neither ab nor ac pokes into the interior
			int32_t cx = 2 * _x_cm[a] - bx;
			int32_t cy = 2 * _y_cm[a] - by;
			ਵਾਪਸ !ਬਿੰਦੂ_Inside_Interior_Cone(poly, bx, by, a - poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ)
			       && !ਬਿੰਦੂ_Inside_Interior_Cone(poly, cx, cy, a - poly.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ);
		}
	}

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ PlannerPolygons::ਕਿਨਾਰਾ_Bitangent(ਪੂਰਨ a, ਪੂਰਨ b) ਅਚਲ
{
	ਵਾਪਸ ਕਿਨਾਰਾ_Bitangent(a, _x_cm[b], _y_cm[b]);
}

ਤਾਰਕਿਕ PlannerPolygons::ਕਿਨਾਰਾ_Bitangent(ਪੂਰਨ a, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &b) ਅਚਲ
{
	ਵਾਪਸ ਕਿਨਾਰਾ_Bitangent(a, meters_ਮੰਜ਼ਿਲ_Cm(b(0)), meters_ਮੰਜ਼ਿਲ_Cm(b(1)));
}

ਤਾਰਕਿਕ PlannerPolygons::ਸੈੱਟ_Destination(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &p)
{
	ਸੈੱਟ_ਗੰਢ(dest_ਸੂਚਕਾਂਕ(), p);

	ਅਚਲ int32_t x_cm = meters_ਮੰਜ਼ਿਲ_Cm(p(0));
	ਅਚਲ int32_t y_cm = meters_ਮੰਜ਼ਿਲ_Cm(p(1));

	ਜੇ (intersectsAnyInside(x_cm, y_cm, x_cm, y_cm)) {
		// If the destination breaches a geofence, still use it but
		// signal to the user that it is invalid.
		ਵਾਪਸ ਝੂਠ;
	}

	ਵਾਪਸ ਸੱਚ;
}

ਮੈਟ੍ਰਿਕਸ::Vector2f PlannerPolygons::getDestination() ਅਚਲ
{
	ਜੇ (_max_nodes < 1) {
		ਵਾਪਸ ਮੈਟ੍ਰਿਕਸ::Vector2f{NAN, NAN};
	}

	ਵਾਪਸ ਗੰਢ_2(dest_ਸੂਚਕਾਂਕ());
}

ਤਾਰਕਿਕ PlannerPolygons::ਕਿਨਾਰਾ_Visible(ਪੂਰਨ a, ਪੂਰਨ b) ਅਚਲ
{
	ਜੇ (a == b) { ਵਾਪਸ ਝੂਠ; }

	ਜੇ (ਗੰਢ_not_on_optimal_ਰਸਤਾ[a] || ਗੰਢ_not_on_optimal_ਰਸਤਾ[b]) { ਵਾਪਸ ਝੂਠ; }

	// Only bitangent edges can be on a shortest path; this check also cheaply
	// discards edges that violate the polygon at the vertex they come from.
	ਜੇ (!ਕਿਨਾਰਾ_Bitangent(a, b) || !ਕਿਨਾਰਾ_Bitangent(b, a)) { ਵਾਪਸ ਝੂਠ; }

	ਵਾਪਸ !intersectsAnyInside(_x_cm[a], _y_cm[a], _x_cm[b], _y_cm[b]);
}

ਤਾਰਕਿਕ PlannerPolygons::ਕਿਨਾਰਾ_Visible(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &a, ਪੂਰਨ b) ਅਚਲ
{
	ਜੇ (ਗੰਢ_not_on_optimal_ਰਸਤਾ[b]) { ਵਾਪਸ ਝੂਠ; }

	ਜੇ (!ਕਿਨਾਰਾ_Bitangent(b, a)) { ਵਾਪਸ ਝੂਠ; }

	ਵਾਪਸ !intersectsAnyInside(meters_ਮੰਜ਼ਿਲ_Cm(a(0)), meters_ਮੰਜ਼ਿਲ_Cm(a(1)), _x_cm[b], _y_cm[b]);
}

ਤਾਰਕਿਕ PlannerPolygons::ਕਿਨਾਰਾ_Visible(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &a, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &b) ਅਚਲ
{
	ਵਾਪਸ !intersectsAnyInside(meters_ਮੰਜ਼ਿਲ_Cm(a(0)), meters_ਮੰਜ਼ਿਲ_Cm(a(1)), meters_ਮੰਜ਼ਿਲ_Cm(b(0)), meters_ਮੰਜ਼ਿਲ_Cm(b(1)));
}

ਦਸ਼ਮਲਵ PlannerPolygons::ਕਿਨਾਰਾ_ਲਾਗਤ(ਪੂਰਨ a, ਪੂਰਨ b) ਅਚਲ
{
	ਵਾਪਸ ਕਿਨਾਰਾ_Visible(a, b) ? (ਗੰਢ_2(a) - ਗੰਢ_2(b)).norm() : INFINITY;
}


ਤਾਰਕਿਕ segmentsIntersectInclusive(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2d &a, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2d &b,
				ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2d &c, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2d &d)
{
	// Reject separated bounds, including gaps between collinear segments.
	ਲਈ (ਪੂਰਨ axis = 0; axis < 2; ++axis) {
		ਜੇ (math::max(a(axis), b(axis)) < math::min(c(axis), d(axis))
		    || math::max(c(axis), d(axis)) < math::min(a(axis), b(axis))) {
			ਵਾਪਸ ਝੂਠ;
		}
	}

	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2d ab = b - a;
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2d cd = d - c;
	ਅਚਲ ਦੁੱਗਣਾ side_c = ab.cross(c - a);
	ਅਚਲ ਦੁੱਗਣਾ side_d = ab.cross(d - a);
	ਅਚਲ ਦੁੱਗਣਾ side_a = cd.cross(a - c);
	ਅਚਲ ਦੁੱਗਣਾ side_b = cd.cross(b - c);

	// Each segment must cross or touch the line through the other segment.
	ਵਾਪਸ ((side_c <= 0.0 && side_d >= 0.0) || (side_c >= 0.0 && side_d <= 0.0))
	       && ((side_a <= 0.0 && side_b >= 0.0) || (side_a >= 0.0 && side_b <= 0.0));
}

ਦੁੱਗਣਾ ਬਿੰਦੂ_ਮੰਜ਼ਿਲ_Segment_ਦੂਰੀ_Squared(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2d &ਬਿੰਦੂ,
				     ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2d &a, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2d &b)
{
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2d direction = b - a;
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2d ਮੰਜ਼ਿਲ_ਬਿੰਦੂ = ਬਿੰਦੂ - a;
	ਅਚਲ ਦੁੱਗਣਾ ਲੰਮਾਈ_squared = direction.norm_squared();
	// along / length_squared locates the projection: 0 at a, 1 at b.
	ਅਚਲ ਦੁੱਗਣਾ along = ਮੰਜ਼ਿਲ_ਬਿੰਦੂ.dot(direction);

	ਜੇ (along <= 0.0) {
		ਵਾਪਸ ਮੰਜ਼ਿਲ_ਬਿੰਦੂ.norm_squared();

	} ਨਹੀਂਤਾਂ ਜੇ (along >= ਲੰਮਾਈ_squared) {
		ਵਾਪਸ (ਬਿੰਦੂ - b).norm_squared();
	}

	// Squared distance to the line: |to_point x direction|^2 / |direction|^2.
	// https://mathworld.wolfram.com/2-DimensionalPoint-LineDistance.html
	ਅਚਲ ਦੁੱਗਣਾ cross = ਮੰਜ਼ਿਲ_ਬਿੰਦੂ.cross(direction);
	ਵਾਪਸ cross * cross / ਲੰਮਾਈ_squared;
}

ਤਾਰਕਿਕ isPolygonCCW(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f *vertices, ਪੂਰਨ ਅੰਕ_vertices)
{
	// https://en.wikipedia.org/wiki/Shoelace_formula
	// Triangle formula
	ਦਸ਼ਮਲਵ signed_area_2x = 0.f;

	ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_vertices; i++) {
		ਅਚਲ ਪੂਰਨ j = (i + 1) % ਅੰਕ_vertices;
		signed_area_2x += vertices[i](0) * vertices[j](1)
				  - vertices[j](0) * vertices[i](1);
	}

	ਵਾਪਸ signed_area_2x > 0.f;
}

} // namespace geofence_utils
