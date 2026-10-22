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

যৌক্তিক solveBackward(ধ্রুবক পূর্ণসংখ্যা অঙ্ক_nodes, ধ্রুবক পূর্ণসংখ্যা goal, ধ্রুবক দশমিক *cost, ধ্রুবক যৌক্তিক symmetric,
		   দশমিক *best_cost, পূর্ণসংখ্যা *পরবর্তী_গ্রন্থি, যৌক্তিক *visited)
{
	যদি (অঙ্ক_nodes <= 0 || goal < 0 || goal >= অঙ্ক_nodes
	    || cost == nullptr || best_cost == nullptr || পরবর্তী_গ্রন্থি == nullptr || visited == nullptr) {
		ফেরত মিথ্যা;
	}

	জন্য (পূর্ণসংখ্যা i = 0; i < অঙ্ক_nodes; ++i) {
		best_cost[i] = kUnreachable;
		পরবর্তী_গ্রন্থি[i] = -1;
		visited[i] = মিথ্যা;
	}

	best_cost[goal] = 0.f;
	visited[goal] = সত্য;

	// O(N^2) Dijkstra on the reverse graph. Each iteration relaxes incoming edges
	// v -> u using cost(v, u) and, in the same sweep over v, tracks the smallest
	// best_cost among still-unvisited nodes — that argmin is the u for the next
	// iteration. The very first u is `goal` (the only node with best_cost == 0).
	পূর্ণসংখ্যা u = goal;
	দশমিক u_cost = 0.f;

	জন্য (পূর্ণসংখ্যা iter = 0; iter < অঙ্ক_nodes - 1; ++iter) {
		পূর্ণসংখ্যা পরবর্তী_u = -1;
		দশমিক পরবর্তী_cost = kUnreachable;

		জন্য (পূর্ণসংখ্যা v = 0; v < অঙ্ক_nodes; ++v) {
			যদি (visited[v]) {
				চালু;
			}

			// Edge v -> u. For asymmetric layout, look up the (v, u) entry directly.
			// For symmetric packed upper-triangular: index by the unordered pair {v, u}.
			দশমিক edge;

			যদি (symmetric) {
				edge = cost[symmetric_Pair_সূচক(v, u, অঙ্ক_nodes)];

			} অন্যথা {
				edge = cost[v * অঙ্ক_nodes + u];
			}

			// Treat +INFINITY and NaN as missing edges. `edge < kUnreachable` is false for
			// both because NaN is unordered and INFINITY < INFINITY is false.
			যদি (edge < kUnreachable) {
				ধ্রুবক দশমিক candidate = u_cost + edge;

				যদি (candidate < best_cost[v]) {
					best_cost[v] = candidate;
					পরবর্তী_গ্রন্থি[v] = u;
				}
			}

			// Same sweep: track argmin of best_cost over unvisited nodes for the next iter.
			যদি (best_cost[v] < পরবর্তী_cost) {
				পরবর্তী_cost = best_cost[v];
				পরবর্তী_u = v;
			}
		}

		যদি (পরবর্তী_u < 0) {
			// No unvisited node is reachable; remaining nodes stay at kUnreachable.
			বিরতি;
		}

		visited[পরবর্তী_u] = সত্য;
		u = পরবর্তী_u;
		u_cost = পরবর্তী_cost;
	}

	ফেরত সত্য;
}

} // namespace dijkstra
