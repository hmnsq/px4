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

منطقي solveBackward(ثابت صحيح ژمارە_nodes, ثابت صحيح goal, ثابت عائم *cost, ثابت منطقي symmetric,
		   عائم *best_cost, صحيح *next_گرێ, منطقي *visited)
{
	إذا (ژمارە_nodes <= 0 || goal < 0 || goal >= ژمارە_nodes
	    || cost == nullptr || best_cost == nullptr || next_گرێ == nullptr || visited == nullptr) {
		أرجع خطأ;
	}

	لكل (صحيح i = 0; i < ژمارە_nodes; ++i) {
		best_cost[i] = kUnreachable;
		next_گرێ[i] = -1;
		visited[i] = خطأ;
	}

	best_cost[goal] = 0.f;
	visited[goal] = true;

	// O(N^2) Dijkstra on the reverse graph. Each iteration relaxes incoming edges
	// v -> u using cost(v, u) and, in the same sweep over v, tracks the smallest
	// best_cost among still-unvisited nodes — that argmin is the u for the next
	// iteration. The very first u is `goal` (the only node with best_cost == 0).
	صحيح u = goal;
	عائم u_cost = 0.f;

	لكل (صحيح iter = 0; iter < ژمارە_nodes - 1; ++iter) {
		صحيح next_u = -1;
		عائم next_cost = kUnreachable;

		لكل (صحيح v = 0; v < ژمارە_nodes; ++v) {
			إذا (visited[v]) {
				استمر;
			}

			// Edge v -> u. For asymmetric layout, look up the (v, u) entry directly.
			// For symmetric packed upper-triangular: index by the unordered pair {v, u}.
			عائم edge;

			إذا (symmetric) {
				edge = cost[symmetricPairIndex(v, u, ژمارە_nodes)];

			} وإلا {
				edge = cost[v * ژمارە_nodes + u];
			}

			// Treat +INFINITY and NaN as missing edges. `edge < kUnreachable` is false for
			// both because NaN is unordered and INFINITY < INFINITY is false.
			إذا (edge < kUnreachable) {
				ثابت عائم candidate = u_cost + edge;

				إذا (candidate < best_cost[v]) {
					best_cost[v] = candidate;
					next_گرێ[v] = u;
				}
			}

			// Same sweep: track argmin of best_cost over unvisited nodes for the next iter.
			إذا (best_cost[v] < next_cost) {
				next_cost = best_cost[v];
				next_u = v;
			}
		}

		إذا (next_u < 0) {
			// No unvisited node is reachable; remaining nodes stay at kUnreachable.
			اكسر;
		}

		visited[next_u] = true;
		u = next_u;
		u_cost = next_cost;
	}

	أرجع true;
}

} // namespace dijkstra
