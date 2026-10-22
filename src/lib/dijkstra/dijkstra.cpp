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

#अन्तर्भाव "dijkstra.h"

नामाकाश dijkstra
{

तार्किक solveBackward(ध्रुव पूर्णाङ्क अंक_nodes, ध्रुव पूर्णाङ्क goal, ध्रुव भिन्न *cost, ध्रुव तार्किक symmetric,
		   भिन्न *best_cost, पूर्णाङ्क *next_गाठ, तार्किक *visited)
{
	यदि (अंक_nodes <= 0 || goal < 0 || goal >= अंक_nodes
	    || cost == nullptr || best_cost == nullptr || next_गाठ == nullptr || visited == nullptr) {
		प्रत्यागम असत्य;
	}

	प्रति (पूर्णाङ्क i = 0; i < अंक_nodes; ++i) {
		best_cost[i] = kUnreachable;
		next_गाठ[i] = -1;
		visited[i] = असत्य;
	}

	best_cost[goal] = 0.f;
	visited[goal] = सत्य;

	// O(N^2) Dijkstra on the reverse graph. Each iteration relaxes incoming edges
	// v -> u using cost(v, u) and, in the same sweep over v, tracks the smallest
	// best_cost among still-unvisited nodes — that argmin is the u for the next
	// iteration. The very first u is `goal` (the only node with best_cost == 0).
	पूर्णाङ्क u = goal;
	भिन्न u_cost = 0.f;

	प्रति (पूर्णाङ्क iter = 0; iter < अंक_nodes - 1; ++iter) {
		पूर्णाङ्क next_u = -1;
		भिन्न next_cost = kUnreachable;

		प्रति (पूर्णाङ्क v = 0; v < अंक_nodes; ++v) {
			यदि (visited[v]) {
				अनुवर्त;
			}

			// Edge v -> u. For asymmetric layout, look up the (v, u) entry directly.
			// For symmetric packed upper-triangular: index by the unordered pair {v, u}.
			भिन्न edge;

			यदि (symmetric) {
				edge = cost[symmetricPairIndex(v, u, अंक_nodes)];

			} अन्यथा {
				edge = cost[v * अंक_nodes + u];
			}

			// Treat +INFINITY and NaN as missing edges. `edge < kUnreachable` is false for
			// both because NaN is unordered and INFINITY < INFINITY is false.
			यदि (edge < kUnreachable) {
				ध्रुव भिन्न candidate = u_cost + edge;

				यदि (candidate < best_cost[v]) {
					best_cost[v] = candidate;
					next_गाठ[v] = u;
				}
			}

			// Same sweep: track argmin of best_cost over unvisited nodes for the next iter.
			यदि (best_cost[v] < next_cost) {
				next_cost = best_cost[v];
				next_u = v;
			}
		}

		यदि (next_u < 0) {
			// No unvisited node is reachable; remaining nodes stay at kUnreachable.
			भङ्ग;
		}

		visited[next_u] = सत्य;
		u = next_u;
		u_cost = next_cost;
	}

	प्रत्यागम सत्य;
}

} // namespace dijkstra
