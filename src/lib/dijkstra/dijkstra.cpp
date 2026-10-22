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

#அந்தர்பாவ "dijkstra.h"

நாமாகாஶ dijkstra
{

தருக்க solveBackward(மாறிலி முழு இலக்கம்_nodes, மாறிலி முழு goal, மாறிலி தசம *cost, மாறிலி தருக்க symmetric,
		   தசம *best_cost, முழு *next_முனை, தருக்க *visited)
{
	எனில் (இலக்கம்_nodes <= 0 || goal < 0 || goal >= இலக்கம்_nodes
	    || cost == nullptr || best_cost == nullptr || next_முனை == nullptr || visited == nullptr) {
		திரும்பு பொய்;
	}

	க்கு (முழு i = 0; i < இலக்கம்_nodes; ++i) {
		best_cost[i] = kUnreachable;
		next_முனை[i] = -1;
		visited[i] = பொய்;
	}

	best_cost[goal] = 0.f;
	visited[goal] = உண்மை;

	// O(N^2) Dijkstra on the reverse graph. Each iteration relaxes incoming edges
	// v -> u using cost(v, u) and, in the same sweep over v, tracks the smallest
	// best_cost among still-unvisited nodes — that argmin is the u for the next
	// iteration. The very first u is `goal` (the only node with best_cost == 0).
	முழு u = goal;
	தசம u_cost = 0.f;

	க்கு (முழு iter = 0; iter < இலக்கம்_nodes - 1; ++iter) {
		முழு next_u = -1;
		தசம next_cost = kUnreachable;

		க்கு (முழு v = 0; v < இலக்கம்_nodes; ++v) {
			எனில் (visited[v]) {
				தொடர்;
			}

			// Edge v -> u. For asymmetric layout, look up the (v, u) entry directly.
			// For symmetric packed upper-triangular: index by the unordered pair {v, u}.
			தசம edge;

			எனில் (symmetric) {
				edge = cost[symmetricPairIndex(v, u, இலக்கம்_nodes)];

			} இல்லையெனில் {
				edge = cost[v * இலக்கம்_nodes + u];
			}

			// Treat +INFINITY and NaN as missing edges. `edge < kUnreachable` is false for
			// both because NaN is unordered and INFINITY < INFINITY is false.
			எனில் (edge < kUnreachable) {
				மாறிலி தசம candidate = u_cost + edge;

				எனில் (candidate < best_cost[v]) {
					best_cost[v] = candidate;
					next_முனை[v] = u;
				}
			}

			// Same sweep: track argmin of best_cost over unvisited nodes for the next iter.
			எனில் (best_cost[v] < next_cost) {
				next_cost = best_cost[v];
				next_u = v;
			}
		}

		எனில் (next_u < 0) {
			// No unvisited node is reachable; remaining nodes stay at kUnreachable.
			நிறுத்து;
		}

		visited[next_u] = உண்மை;
		u = next_u;
		u_cost = next_cost;
	}

	திரும்பு உண்மை;
}

} // namespace dijkstra
