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

#അന്തര്ഭാവ "dijkstra.h"

നാമാകാശ dijkstra
{

തർക്കം solveBackward(സ്ഥിരം പൂർണ്ണസംഖ്യ അക്കം_nodes, സ്ഥിരം പൂർണ്ണസംഖ്യ goal, സ്ഥിരം ദശാംശം *cost, സ്ഥിരം തർക്കം symmetric,
		   ദശാംശം *best_cost, പൂർണ്ണസംഖ്യ *next_കണ്ണി, തർക്കം *visited)
{
	എങ്കിൽ (അക്കം_nodes <= 0 || goal < 0 || goal >= അക്കം_nodes
	    || cost == nullptr || best_cost == nullptr || next_കണ്ണി == nullptr || visited == nullptr) {
		തിരികെ അസത്യം;
	}

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_nodes; ++i) {
		best_cost[i] = kUnreachable;
		next_കണ്ണി[i] = -1;
		visited[i] = അസത്യം;
	}

	best_cost[goal] = 0.f;
	visited[goal] = സത്യം;

	// O(N^2) Dijkstra on the reverse graph. Each iteration relaxes incoming edges
	// v -> u using cost(v, u) and, in the same sweep over v, tracks the smallest
	// best_cost among still-unvisited nodes — that argmin is the u for the next
	// iteration. The very first u is `goal` (the only node with best_cost == 0).
	പൂർണ്ണസംഖ്യ u = goal;
	ദശാംശം u_cost = 0.f;

	വേണ്ടി (പൂർണ്ണസംഖ്യ iter = 0; iter < അക്കം_nodes - 1; ++iter) {
		പൂർണ്ണസംഖ്യ next_u = -1;
		ദശാംശം next_cost = kUnreachable;

		വേണ്ടി (പൂർണ്ണസംഖ്യ v = 0; v < അക്കം_nodes; ++v) {
			എങ്കിൽ (visited[v]) {
				തുടരുക;
			}

			// Edge v -> u. For asymmetric layout, look up the (v, u) entry directly.
			// For symmetric packed upper-triangular: index by the unordered pair {v, u}.
			ദശാംശം edge;

			എങ്കിൽ (symmetric) {
				edge = cost[symmetricPairIndex(v, u, അക്കം_nodes)];

			} അല്ലെങ്കിൽ {
				edge = cost[v * അക്കം_nodes + u];
			}

			// Treat +INFINITY and NaN as missing edges. `edge < kUnreachable` is false for
			// both because NaN is unordered and INFINITY < INFINITY is false.
			എങ്കിൽ (edge < kUnreachable) {
				സ്ഥിരം ദശാംശം candidate = u_cost + edge;

				എങ്കിൽ (candidate < best_cost[v]) {
					best_cost[v] = candidate;
					next_കണ്ണി[v] = u;
				}
			}

			// Same sweep: track argmin of best_cost over unvisited nodes for the next iter.
			എങ്കിൽ (best_cost[v] < next_cost) {
				next_cost = best_cost[v];
				next_u = v;
			}
		}

		എങ്കിൽ (next_u < 0) {
			// No unvisited node is reachable; remaining nodes stay at kUnreachable.
			നിർത്തുക;
		}

		visited[next_u] = സത്യം;
		u = next_u;
		u_cost = next_cost;
	}

	തിരികെ സത്യം;
}

} // namespace dijkstra
