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

#ಅನ್ತರ್ಭಾವ "dijkstra.h"

ನಾಮಾಕಾಶ dijkstra
{

ತಾರ್ಕಿಕ solveBackward(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_nodes, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ goal, ಧ್ರುವ ಭಿನ್ನ *cost, ಧ್ರುವ ತಾರ್ಕಿಕ symmetric,
		   ಭಿನ್ನ *best_cost, ಪೂರ್ಣಾಙ್ಕ *next_ಗಂಟು, ತಾರ್ಕಿಕ *visited)
{
	ಯದಿ (ಅಂಕೆ_nodes <= 0 || goal < 0 || goal >= ಅಂಕೆ_nodes
	    || cost == nullptr || best_cost == nullptr || next_ಗಂಟು == nullptr || visited == nullptr) {
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_nodes; ++i) {
		best_cost[i] = kUnreachable;
		next_ಗಂಟು[i] = -1;
		visited[i] = ಅಸತ್ಯ;
	}

	best_cost[goal] = 0.f;
	visited[goal] = ಸತ್ಯ;

	// O(N^2) Dijkstra on the reverse graph. Each iteration relaxes incoming edges
	// v -> u using cost(v, u) and, in the same sweep over v, tracks the smallest
	// best_cost among still-unvisited nodes — that argmin is the u for the next
	// iteration. The very first u is `goal` (the only node with best_cost == 0).
	ಪೂರ್ಣಾಙ್ಕ u = goal;
	ಭಿನ್ನ u_cost = 0.f;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ iter = 0; iter < ಅಂಕೆ_nodes - 1; ++iter) {
		ಪೂರ್ಣಾಙ್ಕ next_u = -1;
		ಭಿನ್ನ next_cost = kUnreachable;

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ v = 0; v < ಅಂಕೆ_nodes; ++v) {
			ಯದಿ (visited[v]) {
				ಅನುವರ್ತ;
			}

			// Edge v -> u. For asymmetric layout, look up the (v, u) entry directly.
			// For symmetric packed upper-triangular: index by the unordered pair {v, u}.
			ಭಿನ್ನ edge;

			ಯದಿ (symmetric) {
				edge = cost[symmetricPairIndex(v, u, ಅಂಕೆ_nodes)];

			} ಅನ್ಯಥಾ {
				edge = cost[v * ಅಂಕೆ_nodes + u];
			}

			// Treat +INFINITY and NaN as missing edges. `edge < kUnreachable` is false for
			// both because NaN is unordered and INFINITY < INFINITY is false.
			ಯದಿ (edge < kUnreachable) {
				ಧ್ರುವ ಭಿನ್ನ candidate = u_cost + edge;

				ಯದಿ (candidate < best_cost[v]) {
					best_cost[v] = candidate;
					next_ಗಂಟು[v] = u;
				}
			}

			// Same sweep: track argmin of best_cost over unvisited nodes for the next iter.
			ಯದಿ (best_cost[v] < next_cost) {
				next_cost = best_cost[v];
				next_u = v;
			}
		}

		ಯದಿ (next_u < 0) {
			// No unvisited node is reachable; remaining nodes stay at kUnreachable.
			ಭಙ್ಗ;
		}

		visited[next_u] = ಸತ್ಯ;
		u = next_u;
		u_cost = next_cost;
	}

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

} // namespace dijkstra
