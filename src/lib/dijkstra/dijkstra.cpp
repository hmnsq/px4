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

#అన్తర్భావ "dijkstra.h"

నామాకాశ dijkstra
{

తార్కిక solveBackward(స్థిరరాశి పూర్ణాంకం అంకె_nodes, స్థిరరాశి పూర్ణాంకం goal, స్థిరరాశి దశాంశం *ఖర్చు, స్థిరరాశి తార్కిక సౌష్ఠవ,
		   దశాంశం *ఉత్తమ_ఖర్చు, పూర్ణాంకం *తదుపరి_కణుపు, తార్కిక *సందర్శించిన)
{
	ఒకవేళ (అంకె_nodes <= 0 || goal < 0 || goal >= అంకె_nodes
	    || ఖర్చు == శూన్యసూచిక || ఉత్తమ_ఖర్చు == శూన్యసూచిక || తదుపరి_కణుపు == శూన్యసూచిక || సందర్శించిన == శూన్యసూచిక) {
		తిరిగి అబద్ధం;
	}

	కొరకు (పూర్ణాంకం i = 0; i < అంకె_nodes; ++i) {
		ఉత్తమ_ఖర్చు[i] = kUnreachable;
		తదుపరి_కణుపు[i] = -1;
		సందర్శించిన[i] = అబద్ధం;
	}

	ఉత్తమ_ఖర్చు[goal] = 0.f;
	సందర్శించిన[goal] = నిజం;

	// O(N^2) Dijkstra on the reverse graph. Each iteration relaxes incoming edges
	// v -> u using cost(v, u) and, in the same sweep over v, tracks the smallest
	// best_cost among still-unvisited nodes — that argmin is the u for the next
	// iteration. The very first u is `goal` (the only node with best_cost == 0).
	పూర్ణాంకం u = goal;
	దశాంశం u_ఖర్చు = 0.f;

	కొరకు (పూర్ణాంకం iter = 0; iter < అంకె_nodes - 1; ++iter) {
		పూర్ణాంకం తదుపరి_u = -1;
		దశాంశం తదుపరి_ఖర్చు = kUnreachable;

		కొరకు (పూర్ణాంకం v = 0; v < అంకె_nodes; ++v) {
			ఒకవేళ (సందర్శించిన[v]) {
				కొనసాగు;
			}

			// Edge v -> u. For asymmetric layout, look up the (v, u) entry directly.
			// For symmetric packed upper-triangular: index by the unordered pair {v, u}.
			దశాంశం అంచు_2;

			ఒకవేళ (సౌష్ఠవ) {
				అంచు_2 = ఖర్చు[సౌష్ఠవ_Pair_సూచీ_2(v, u, అంకె_nodes)];

			} లేకపోతే {
				అంచు_2 = ఖర్చు[v * అంకె_nodes + u];
			}

			// Treat +INFINITY and NaN as missing edges. `edge < kUnreachable` is false for
			// both because NaN is unordered and INFINITY < INFINITY is false.
			ఒకవేళ (అంచు_2 < kUnreachable) {
				స్థిరరాశి దశాంశం candidate = u_ఖర్చు + అంచు_2;

				ఒకవేళ (candidate < ఉత్తమ_ఖర్చు[v]) {
					ఉత్తమ_ఖర్చు[v] = candidate;
					తదుపరి_కణుపు[v] = u;
				}
			}

			// Same sweep: track argmin of best_cost over unvisited nodes for the next iter.
			ఒకవేళ (ఉత్తమ_ఖర్చు[v] < తదుపరి_ఖర్చు) {
				తదుపరి_ఖర్చు = ఉత్తమ_ఖర్చు[v];
				తదుపరి_u = v;
			}
		}

		ఒకవేళ (తదుపరి_u < 0) {
			// No unvisited node is reachable; remaining nodes stay at kUnreachable.
			ఆపు;
		}

		సందర్శించిన[తదుపరి_u] = నిజం;
		u = తదుపరి_u;
		u_ఖర్చు = తదుపరి_ఖర్చు;
	}

	తిరిగి నిజం;
}

} // namespace dijkstra
