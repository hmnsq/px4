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

#ضمن "dijkstra.h"

نطاق dijkstra
{

منطقي solveBackward(ثابت صحيح رقم_nodes, ثابت صحيح goal, ثابت عائم *هزینه, ثابت منطقي متقارن,
		   عائم *بهترین_هزینه, صحيح *بعدی_گره, منطقي *دیدهشده)
{
	إذا (رقم_nodes <= 0 || goal < 0 || goal >= رقم_nodes
	    || هزینه == اشاره_تهی || بهترین_هزینه == اشاره_تهی || بعدی_گره == اشاره_تهی || دیدهشده == اشاره_تهی) {
		أرجع خطأ;
	}

	لكل (صحيح i = 0; i < رقم_nodes; ++i) {
		بهترین_هزینه[i] = kUnreachable;
		بعدی_گره[i] = -1;
		دیدهشده[i] = خطأ;
	}

	بهترین_هزینه[goal] = 0.f;
	دیدهشده[goal] = درست;

	// O(N^2) Dijkstra on the reverse graph. Each iteration relaxes incoming edges
	// v -> u using cost(v, u) and, in the same sweep over v, tracks the smallest
	// best_cost among still-unvisited nodes — that argmin is the u for the next
	// iteration. The very first u is `goal` (the only node with best_cost == 0).
	صحيح u = goal;
	عائم u_هزینه = 0.f;

	لكل (صحيح iter = 0; iter < رقم_nodes - 1; ++iter) {
		صحيح بعدی_u = -1;
		عائم بعدی_هزینه = kUnreachable;

		لكل (صحيح v = 0; v < رقم_nodes; ++v) {
			إذا (دیدهشده[v]) {
				استمر;
			}

			// Edge v -> u. For asymmetric layout, look up the (v, u) entry directly.
			// For symmetric packed upper-triangular: index by the unordered pair {v, u}.
			عائم یال_2;

			إذا (متقارن) {
				یال_2 = هزینه[متقارن_Pair_شاخص_2(v, u, رقم_nodes)];

			} وإلا {
				یال_2 = هزینه[v * رقم_nodes + u];
			}

			// Treat +INFINITY and NaN as missing edges. `edge < kUnreachable` is false for
			// both because NaN is unordered and INFINITY < INFINITY is false.
			إذا (یال_2 < kUnreachable) {
				ثابت عائم candidate = u_هزینه + یال_2;

				إذا (candidate < بهترین_هزینه[v]) {
					بهترین_هزینه[v] = candidate;
					بعدی_گره[v] = u;
				}
			}

			// Same sweep: track argmin of best_cost over unvisited nodes for the next iter.
			إذا (بهترین_هزینه[v] < بعدی_هزینه) {
				بعدی_هزینه = بهترین_هزینه[v];
				بعدی_u = v;
			}
		}

		إذا (بعدی_u < 0) {
			// No unvisited node is reachable; remaining nodes stay at kUnreachable.
			اكسر;
		}

		دیدهشده[بعدی_u] = درست;
		u = بعدی_u;
		u_هزینه = بعدی_هزینه;
	}

	أرجع درست;
}

} // namespace dijkstra
