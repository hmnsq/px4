/****************************************************************************
 *
 *   Copyright (c) 2026 PX4 Development Team. All rights reserved.
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

#ضمن "dijkstra.h"

نطاق dijkstra
{

منطقي solveBackward(ثابت صحيح انگ_nodes, ثابت صحيح goal, ثابت عائم *لاڳت, ثابت منطقي متناظر,
		   عائم *بهترين_لاڳت, صحيح *ايندڙ_ڳنڍ, منطقي *گهميل)
{
	إذا (انگ_nodes <= 0 || goal < 0 || goal >= انگ_nodes
	    || لاڳت == خالي_اشارو || بهترين_لاڳت == خالي_اشارو || ايندڙ_ڳنڍ == خالي_اشارو || گهميل == خالي_اشارو) {
		أرجع خطأ;
	}

	لكل (صحيح i = 0; i < انگ_nodes; ++i) {
		بهترين_لاڳت[i] = kUnreachable;
		ايندڙ_ڳنڍ[i] = -1;
		گهميل[i] = خطأ;
	}

	بهترين_لاڳت[goal] = 0.f;
	گهميل[goal] = سچ;

	// O(N^2) Dijkstra on the reverse graph. Each iteration relaxes incoming edges
	// v -> u using cost(v, u) and, in the same sweep over v, tracks the smallest
	// best_cost among still-unvisited nodes — that argmin is the u for the next
	// iteration. The very first u is `goal` (the only node with best_cost == 0).
	صحيح u = goal;
	عائم u_لاڳت = 0.f;

	لكل (صحيح iter = 0; iter < انگ_nodes - 1; ++iter) {
		صحيح ايندڙ_u = -1;
		عائم ايندڙ_لاڳت = kUnreachable;

		لكل (صحيح v = 0; v < انگ_nodes; ++v) {
			إذا (گهميل[v]) {
				استمر;
			}

			// Edge v -> u. For asymmetric layout, look up the (v, u) entry directly.
			// For symmetric packed upper-triangular: index by the unordered pair {v, u}.
			عائم ڪنارو_2;

			إذا (متناظر) {
				ڪنارو_2 = لاڳت[متناظر_Pair_اشاريو_2(v, u, انگ_nodes)];

			} وإلا {
				ڪنارو_2 = لاڳت[v * انگ_nodes + u];
			}

			// Treat +INFINITY and NaN as missing edges. `edge < kUnreachable` is false for
			// both because NaN is unordered and INFINITY < INFINITY is false.
			إذا (ڪنارو_2 < kUnreachable) {
				ثابت عائم candidate = u_لاڳت + ڪنارو_2;

				إذا (candidate < بهترين_لاڳت[v]) {
					بهترين_لاڳت[v] = candidate;
					ايندڙ_ڳنڍ[v] = u;
				}
			}

			// Same sweep: track argmin of best_cost over unvisited nodes for the next iter.
			إذا (بهترين_لاڳت[v] < ايندڙ_لاڳت) {
				ايندڙ_لاڳت = بهترين_لاڳت[v];
				ايندڙ_u = v;
			}
		}

		إذا (ايندڙ_u < 0) {
			// No unvisited node is reachable; remaining nodes stay at kUnreachable.
			اكسر;
		}

		گهميل[ايندڙ_u] = سچ;
		u = ايندڙ_u;
		u_لاڳت = ايندڙ_لاڳت;
	}

	أرجع سچ;
}

} // namespace dijkstra
