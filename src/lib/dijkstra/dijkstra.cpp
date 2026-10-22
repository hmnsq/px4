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

#ଅନ୍ତର୍ଭାଵ "dijkstra.h"

ନାମାକାଶ dijkstra
{

ତାର୍କିକ solveBackward(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_nodes, ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା goal, ଅଚଳ ଦଶମିକ *cost, ଅଚଳ ତାର୍କିକ symmetric,
		   ଦଶମିକ *best_cost, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *next_ଗଣ୍ଠି, ତାର୍କିକ *visited)
{
	ଯଦି (ଅଙ୍କ_nodes <= 0 || goal < 0 || goal >= ଅଙ୍କ_nodes
	    || cost == nullptr || best_cost == nullptr || next_ଗଣ୍ଠି == nullptr || visited == nullptr) {
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_nodes; ++i) {
		best_cost[i] = kUnreachable;
		next_ଗଣ୍ଠି[i] = -1;
		visited[i] = ମିଥ୍ୟା;
	}

	best_cost[goal] = 0.f;
	visited[goal] = ସତ୍ୟ;

	// O(N^2) Dijkstra on the reverse graph. Each iteration relaxes incoming edges
	// v -> u using cost(v, u) and, in the same sweep over v, tracks the smallest
	// best_cost among still-unvisited nodes — that argmin is the u for the next
	// iteration. The very first u is `goal` (the only node with best_cost == 0).
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା u = goal;
	ଦଶମିକ u_cost = 0.f;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା iter = 0; iter < ଅଙ୍କ_nodes - 1; ++iter) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା next_u = -1;
		ଦଶମିକ next_cost = kUnreachable;

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା v = 0; v < ଅଙ୍କ_nodes; ++v) {
			ଯଦି (visited[v]) {
				ଜାରି;
			}

			// Edge v -> u. For asymmetric layout, look up the (v, u) entry directly.
			// For symmetric packed upper-triangular: index by the unordered pair {v, u}.
			ଦଶମିକ edge;

			ଯଦି (symmetric) {
				edge = cost[symmetricPairIndex(v, u, ଅଙ୍କ_nodes)];

			} ଅନ୍ୟଥା {
				edge = cost[v * ଅଙ୍କ_nodes + u];
			}

			// Treat +INFINITY and NaN as missing edges. `edge < kUnreachable` is false for
			// both because NaN is unordered and INFINITY < INFINITY is false.
			ଯଦି (edge < kUnreachable) {
				ଅଚଳ ଦଶମିକ candidate = u_cost + edge;

				ଯଦି (candidate < best_cost[v]) {
					best_cost[v] = candidate;
					next_ଗଣ୍ଠି[v] = u;
				}
			}

			// Same sweep: track argmin of best_cost over unvisited nodes for the next iter.
			ଯଦି (best_cost[v] < next_cost) {
				next_cost = best_cost[v];
				next_u = v;
			}
		}

		ଯଦି (next_u < 0) {
			// No unvisited node is reachable; remaining nodes stay at kUnreachable.
			ଭାଙ୍ଗ;
		}

		visited[next_u] = ସତ୍ୟ;
		u = next_u;
		u_cost = next_cost;
	}

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

} // namespace dijkstra
