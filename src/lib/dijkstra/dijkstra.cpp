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

منطقي solveBackward(ثابت صحيح عدد_nodes, ثابت صحيح goal, ثابت عائم *لګښت, ثابت منطقي متناظر,
		   عائم *غوره_لګښت, صحيح *بل_غوټه, منطقي *لیدل_شوی)
{
	إذا (عدد_nodes <= 0 || goal < 0 || goal >= عدد_nodes
	    || لګښت == تش_اشاره || غوره_لګښت == تش_اشاره || بل_غوټه == تش_اشاره || لیدل_شوی == تش_اشاره) {
		أرجع خطأ;
	}

	لكل (صحيح i = 0; i < عدد_nodes; ++i) {
		غوره_لګښت[i] = kUnreachable;
		بل_غوټه[i] = -1;
		لیدل_شوی[i] = خطأ;
	}

	غوره_لګښت[goal] = 0.f;
	لیدل_شوی[goal] = ریښتیا;

	// O(N^2) Dijkstra on the reverse graph. Each iteration relaxes incoming edges
	// v -> u using cost(v, u) and, in the same sweep over v, tracks the smallest
	// best_cost among still-unvisited nodes — that argmin is the u for the next
	// iteration. The very first u is `goal` (the only node with best_cost == 0).
	صحيح u = goal;
	عائم u_لګښت = 0.f;

	لكل (صحيح iter = 0; iter < عدد_nodes - 1; ++iter) {
		صحيح بل_u = -1;
		عائم بل_لګښت = kUnreachable;

		لكل (صحيح v = 0; v < عدد_nodes; ++v) {
			إذا (لیدل_شوی[v]) {
				استمر;
			}

			// Edge v -> u. For asymmetric layout, look up the (v, u) entry directly.
			// For symmetric packed upper-triangular: index by the unordered pair {v, u}.
			عائم څنډه_2;

			إذا (متناظر) {
				څنډه_2 = لګښت[متناظر_Pair_شاخص_2(v, u, عدد_nodes)];

			} وإلا {
				څنډه_2 = لګښت[v * عدد_nodes + u];
			}

			// Treat +INFINITY and NaN as missing edges. `edge < kUnreachable` is false for
			// both because NaN is unordered and INFINITY < INFINITY is false.
			إذا (څنډه_2 < kUnreachable) {
				ثابت عائم candidate = u_لګښت + څنډه_2;

				إذا (candidate < غوره_لګښت[v]) {
					غوره_لګښت[v] = candidate;
					بل_غوټه[v] = u;
				}
			}

			// Same sweep: track argmin of best_cost over unvisited nodes for the next iter.
			إذا (غوره_لګښت[v] < بل_لګښت) {
				بل_لګښت = غوره_لګښت[v];
				بل_u = v;
			}
		}

		إذا (بل_u < 0) {
			// No unvisited node is reachable; remaining nodes stay at kUnreachable.
			اكسر;
		}

		لیدل_شوی[بل_u] = ریښتیا;
		u = بل_u;
		u_لګښت = بل_لګښت;
	}

	أرجع ریښتیا;
}

} // namespace dijkstra
