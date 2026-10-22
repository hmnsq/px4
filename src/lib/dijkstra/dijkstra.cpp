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

#অন্তর্ভাব "dijkstra.h"

নামাকাশ dijkstra
{

তার্কিক solveBackward(ধ্রুব পূর্ণাঙ্ক অঙ্ক_nodes, ধ্রুব পূর্ণাঙ্ক goal, ধ্রুব ভিন্ন *cost, ধ্রুব তার্কিক symmetric,
		   ভিন্ন *best_cost, পূর্ণাঙ্ক *next_গ্রন্থি, তার্কিক *visited)
{
	যদি (অঙ্ক_nodes <= 0 || goal < 0 || goal >= অঙ্ক_nodes
	    || cost == nullptr || best_cost == nullptr || next_গ্রন্থি == nullptr || visited == nullptr) {
		প্রত্যাগম অসত্য;
	}

	প্রতি (পূর্ণাঙ্ক i = 0; i < অঙ্ক_nodes; ++i) {
		best_cost[i] = kUnreachable;
		next_গ্রন্থি[i] = -1;
		visited[i] = অসত্য;
	}

	best_cost[goal] = 0.f;
	visited[goal] = সত্য;

	// O(N^2) Dijkstra on the reverse graph. Each iteration relaxes incoming edges
	// v -> u using cost(v, u) and, in the same sweep over v, tracks the smallest
	// best_cost among still-unvisited nodes — that argmin is the u for the next
	// iteration. The very first u is `goal` (the only node with best_cost == 0).
	পূর্ণাঙ্ক u = goal;
	ভিন্ন u_cost = 0.f;

	প্রতি (পূর্ণাঙ্ক iter = 0; iter < অঙ্ক_nodes - 1; ++iter) {
		পূর্ণাঙ্ক next_u = -1;
		ভিন্ন next_cost = kUnreachable;

		প্রতি (পূর্ণাঙ্ক v = 0; v < অঙ্ক_nodes; ++v) {
			যদি (visited[v]) {
				অনুবর্ত;
			}

			// Edge v -> u. For asymmetric layout, look up the (v, u) entry directly.
			// For symmetric packed upper-triangular: index by the unordered pair {v, u}.
			ভিন্ন edge;

			যদি (symmetric) {
				edge = cost[symmetricPairIndex(v, u, অঙ্ক_nodes)];

			} অন্যথা {
				edge = cost[v * অঙ্ক_nodes + u];
			}

			// Treat +INFINITY and NaN as missing edges. `edge < kUnreachable` is false for
			// both because NaN is unordered and INFINITY < INFINITY is false.
			যদি (edge < kUnreachable) {
				ধ্রুব ভিন্ন candidate = u_cost + edge;

				যদি (candidate < best_cost[v]) {
					best_cost[v] = candidate;
					next_গ্রন্থি[v] = u;
				}
			}

			// Same sweep: track argmin of best_cost over unvisited nodes for the next iter.
			যদি (best_cost[v] < next_cost) {
				next_cost = best_cost[v];
				next_u = v;
			}
		}

		যদি (next_u < 0) {
			// No unvisited node is reachable; remaining nodes stay at kUnreachable.
			ভঙ্গ;
		}

		visited[next_u] = সত্য;
		u = next_u;
		u_cost = next_cost;
	}

	প্রত্যাগম সত্য;
}

} // namespace dijkstra
