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

#ਅਨ੍ਤਰ੍ਭਾਵ "dijkstra.h"

ਨਾਮਾਕਾਸ਼ dijkstra
{

ਤਾਰਕਿਕ solveBackward(ਅਚਲ ਪੂਰਨ ਅੰਕ_nodes, ਅਚਲ ਪੂਰਨ goal, ਅਚਲ ਦਸ਼ਮਲਵ *ਲਾਗਤ, ਅਚਲ ਤਾਰਕਿਕ ਸਮਮਿਤ,
		   ਦਸ਼ਮਲਵ *ਉੱਤਮ_ਲਾਗਤ, ਪੂਰਨ *ਅਗਲਾ_ਗੰਢ, ਤਾਰਕਿਕ *ਦੇਖਿਆ)
{
	ਜੇ (ਅੰਕ_nodes <= 0 || goal < 0 || goal >= ਅੰਕ_nodes
	    || ਲਾਗਤ == ਖਾਲੀ_ਸੂਚਕ || ਉੱਤਮ_ਲਾਗਤ == ਖਾਲੀ_ਸੂਚਕ || ਅਗਲਾ_ਗੰਢ == ਖਾਲੀ_ਸੂਚਕ || ਦੇਖਿਆ == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ ਝੂਠ;
	}

	ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_nodes; ++i) {
		ਉੱਤਮ_ਲਾਗਤ[i] = kUnreachable;
		ਅਗਲਾ_ਗੰਢ[i] = -1;
		ਦੇਖਿਆ[i] = ਝੂਠ;
	}

	ਉੱਤਮ_ਲਾਗਤ[goal] = 0.f;
	ਦੇਖਿਆ[goal] = ਸੱਚ;

	// O(N^2) Dijkstra on the reverse graph. Each iteration relaxes incoming edges
	// v -> u using cost(v, u) and, in the same sweep over v, tracks the smallest
	// best_cost among still-unvisited nodes — that argmin is the u for the next
	// iteration. The very first u is `goal` (the only node with best_cost == 0).
	ਪੂਰਨ u = goal;
	ਦਸ਼ਮਲਵ u_ਲਾਗਤ = 0.f;

	ਲਈ (ਪੂਰਨ iter = 0; iter < ਅੰਕ_nodes - 1; ++iter) {
		ਪੂਰਨ ਅਗਲਾ_u = -1;
		ਦਸ਼ਮਲਵ ਅਗਲਾ_ਲਾਗਤ = kUnreachable;

		ਲਈ (ਪੂਰਨ v = 0; v < ਅੰਕ_nodes; ++v) {
			ਜੇ (ਦੇਖਿਆ[v]) {
				ਜਾਰੀ;
			}

			// Edge v -> u. For asymmetric layout, look up the (v, u) entry directly.
			// For symmetric packed upper-triangular: index by the unordered pair {v, u}.
			ਦਸ਼ਮਲਵ ਕਿਨਾਰਾ_2;

			ਜੇ (ਸਮਮਿਤ) {
				ਕਿਨਾਰਾ_2 = ਲਾਗਤ[ਸਮਮਿਤ_Pair_ਸੂਚਕਾਂਕ_2(v, u, ਅੰਕ_nodes)];

			} ਨਹੀਂਤਾਂ {
				ਕਿਨਾਰਾ_2 = ਲਾਗਤ[v * ਅੰਕ_nodes + u];
			}

			// Treat +INFINITY and NaN as missing edges. `edge < kUnreachable` is false for
			// both because NaN is unordered and INFINITY < INFINITY is false.
			ਜੇ (ਕਿਨਾਰਾ_2 < kUnreachable) {
				ਅਚਲ ਦਸ਼ਮਲਵ candidate = u_ਲਾਗਤ + ਕਿਨਾਰਾ_2;

				ਜੇ (candidate < ਉੱਤਮ_ਲਾਗਤ[v]) {
					ਉੱਤਮ_ਲਾਗਤ[v] = candidate;
					ਅਗਲਾ_ਗੰਢ[v] = u;
				}
			}

			// Same sweep: track argmin of best_cost over unvisited nodes for the next iter.
			ਜੇ (ਉੱਤਮ_ਲਾਗਤ[v] < ਅਗਲਾ_ਲਾਗਤ) {
				ਅਗਲਾ_ਲਾਗਤ = ਉੱਤਮ_ਲਾਗਤ[v];
				ਅਗਲਾ_u = v;
			}
		}

		ਜੇ (ਅਗਲਾ_u < 0) {
			// No unvisited node is reachable; remaining nodes stay at kUnreachable.
			ਰੋਕੋ;
		}

		ਦੇਖਿਆ[ਅਗਲਾ_u] = ਸੱਚ;
		u = ਅਗਲਾ_u;
		u_ਲਾਗਤ = ਅਗਲਾ_ਲਾਗਤ;
	}

	ਵਾਪਸ ਸੱਚ;
}

} // namespace dijkstra
