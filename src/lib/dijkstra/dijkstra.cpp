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

ತಾರ್ಕಿಕ solveBackward(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಅಂಕೆ_nodes, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ goal, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ *ವೆಚ್ಚ, ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ ಸಮಮಿತ,
		   ದಶಮಾಂಶ *ಉತ್ತಮ_ವೆಚ್ಚ, ಪೂರ್ಣಾಂಕ *ಮುಂದಿನ_ಗಂಟು, ತಾರ್ಕಿಕ *ಭೇಟಿಯಾದ)
{
	ಆದರೆ (ಅಂಕೆ_nodes <= 0 || goal < 0 || goal >= ಅಂಕೆ_nodes
	    || ವೆಚ್ಚ == ಶೂನ್ಯಸೂಚಕ || ಉತ್ತಮ_ವೆಚ್ಚ == ಶೂನ್ಯಸೂಚಕ || ಮುಂದಿನ_ಗಂಟು == ಶೂನ್ಯಸೂಚಕ || ಭೇಟಿಯಾದ == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_nodes; ++i) {
		ಉತ್ತಮ_ವೆಚ್ಚ[i] = kUnreachable;
		ಮುಂದಿನ_ಗಂಟು[i] = -1;
		ಭೇಟಿಯಾದ[i] = ಸುಳ್ಳು;
	}

	ಉತ್ತಮ_ವೆಚ್ಚ[goal] = 0.f;
	ಭೇಟಿಯಾದ[goal] = ನಿಜ;

	// O(N^2) Dijkstra on the reverse graph. Each iteration relaxes incoming edges
	// v -> u using cost(v, u) and, in the same sweep over v, tracks the smallest
	// best_cost among still-unvisited nodes — that argmin is the u for the next
	// iteration. The very first u is `goal` (the only node with best_cost == 0).
	ಪೂರ್ಣಾಂಕ u = goal;
	ದಶಮಾಂಶ u_ವೆಚ್ಚ = 0.f;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ iter = 0; iter < ಅಂಕೆ_nodes - 1; ++iter) {
		ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_u = -1;
		ದಶಮಾಂಶ ಮುಂದಿನ_ವೆಚ್ಚ = kUnreachable;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ v = 0; v < ಅಂಕೆ_nodes; ++v) {
			ಆದರೆ (ಭೇಟಿಯಾದ[v]) {
				ಮುಂದುವರಿ;
			}

			// Edge v -> u. For asymmetric layout, look up the (v, u) entry directly.
			// For symmetric packed upper-triangular: index by the unordered pair {v, u}.
			ದಶಮಾಂಶ ಅಂಚು_2;

			ಆದರೆ (ಸಮಮಿತ) {
				ಅಂಚು_2 = ವೆಚ್ಚ[ಸಮಮಿತ_Pair_ಸೂಚ್ಯಂಕ_2(v, u, ಅಂಕೆ_nodes)];

			} ಇಲ್ಲದಿದ್ದರೆ {
				ಅಂಚು_2 = ವೆಚ್ಚ[v * ಅಂಕೆ_nodes + u];
			}

			// Treat +INFINITY and NaN as missing edges. `edge < kUnreachable` is false for
			// both because NaN is unordered and INFINITY < INFINITY is false.
			ಆದರೆ (ಅಂಚು_2 < kUnreachable) {
				ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ candidate = u_ವೆಚ್ಚ + ಅಂಚು_2;

				ಆದರೆ (candidate < ಉತ್ತಮ_ವೆಚ್ಚ[v]) {
					ಉತ್ತಮ_ವೆಚ್ಚ[v] = candidate;
					ಮುಂದಿನ_ಗಂಟು[v] = u;
				}
			}

			// Same sweep: track argmin of best_cost over unvisited nodes for the next iter.
			ಆದರೆ (ಉತ್ತಮ_ವೆಚ್ಚ[v] < ಮುಂದಿನ_ವೆಚ್ಚ) {
				ಮುಂದಿನ_ವೆಚ್ಚ = ಉತ್ತಮ_ವೆಚ್ಚ[v];
				ಮುಂದಿನ_u = v;
			}
		}

		ಆದರೆ (ಮುಂದಿನ_u < 0) {
			// No unvisited node is reachable; remaining nodes stay at kUnreachable.
			ನಿಲ್ಲಿಸು;
		}

		ಭೇಟಿಯಾದ[ಮುಂದಿನ_u] = ನಿಜ;
		u = ಮುಂದಿನ_u;
		u_ವೆಚ್ಚ = ಮುಂದಿನ_ವೆಚ್ಚ;
	}

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

} // namespace dijkstra
