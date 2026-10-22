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

#అన్తర్భావ <gtest/gtest.h>
#అన్తర్భావ <cstdlib>
#అన్తర్భావ <math.h>
#అన్తర్భావ "dijkstra.h"

నామాకాశ
{

// Helper: assemble cost matrix entries into a row-major buffer. Use INFINITY for
// missing edges; explicit list of finite (i, j, cost) triples is concise in tests.
నిర్మాణం అంచు { పూర్ణాంకం from, to; దశాంశం ఖర్చు; };

మూస <పూర్ణాంకం N>
శూన్య make_మాతృక(దశాంశం (&m)[N * N], std::initializer_list<అంచు> అంచులు)
{
	కొరకు (పూర్ణాంకం i = 0; i < N * N; ++i) {
		m[i] = INFINITY;
	}

	కొరకు (స్థిరరాశి స్వయంచాలక &e : అంచులు) {
		m[e.from * N + e.to] = e.ఖర్చు;
	}
}

} // namespace

TEST(DijkstraTest, Single_కణుపు_At_Goal)
{
	constexpr పూర్ణాంకం N = 1;
	దశాంశం ఖర్చు[N * N] = { INFINITY };
	దశాంశం ఉత్తమ[N];
	పూర్ణాంకం తదుపరి[N];
	తార్కిక vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 0, ఖర్చు, అబద్ధం, ఉత్తమ, తదుపరి, vis));
	EXPECT_FLOAT_EQ(ఉత్తమ[0], 0.f);
	EXPECT_EQ(తదుపరి[0], -1);
}

TEST(DijkstraTest, Asymmetric_పంక్తి_లేఖాచిత్రం_Forward)
{
	// 0 -> 1 -> 2, going forward is cheap, going backward is expensive.
	constexpr పూర్ణాంకం N = 3;
	దశాంశం ఖర్చు[N * N];
	make_మాతృక<N>(ఖర్చు, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{1, 0, 100.f},
		{2, 1, 100.f},
	});

	దశాంశం ఉత్తమ[N];
	పూర్ణాంకం తదుపరి[N];
	తార్కిక vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 2, ఖర్చు, అబద్ధం, ఉత్తమ, తదుపరి, vis));

	EXPECT_FLOAT_EQ(ఉత్తమ[0], 2.f); // 0 -> 1 -> 2 costs 1 + 1
	EXPECT_FLOAT_EQ(ఉత్తమ[1], 1.f); // 1 -> 2
	EXPECT_FLOAT_EQ(ఉత్తమ[2], 0.f);
	EXPECT_EQ(తదుపరి[0], 1);
	EXPECT_EQ(తదుపరి[1], 2);
	EXPECT_EQ(తదుపరి[2], -1);
}

TEST(DijkstraTest, Asymmetric_పంక్తి_లేఖాచిత్రం_తిరగేయి)
{
	// Same matrix as above; with goal = 0, paths must use the expensive back-edges,
	// confirming the solver respects edge direction (uses cost[i][j] = i -> j, not j -> i).
	constexpr పూర్ణాంకం N = 3;
	దశాంశం ఖర్చు[N * N];
	make_మాతృక<N>(ఖర్చు, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{1, 0, 100.f},
		{2, 1, 100.f},
	});

	దశాంశం ఉత్తమ[N];
	పూర్ణాంకం తదుపరి[N];
	తార్కిక vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 0, ఖర్చు, అబద్ధం, ఉత్తమ, తదుపరి, vis));

	EXPECT_FLOAT_EQ(ఉత్తమ[0], 0.f);
	EXPECT_FLOAT_EQ(ఉత్తమ[1], 100.f);
	EXPECT_FLOAT_EQ(ఉత్తమ[2], 200.f);
	EXPECT_EQ(తదుపరి[1], 0);
	EXPECT_EQ(తదుపరి[2], 1);
	EXPECT_EQ(తదుపరి[0], -1);
}

TEST(DijkstraTest, Triangle_With_Blocked_అంచు)
{
	// Direct edge 0 -> 2 is cheap; remove it and the solver must route via 1.
	constexpr పూర్ణాంకం N = 3;
	దశాంశం ఖర్చు[N * N];
	make_మాతృక<N>(ఖర్చు, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{0, 2, 0.5f},
	});

	దశాంశం ఉత్తమ[N];
	పూర్ణాంకం తదుపరి[N];
	తార్కిక vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 2, ఖర్చు, అబద్ధం, ఉత్తమ, తదుపరి, vis));
	EXPECT_FLOAT_EQ(ఉత్తమ[0], 0.5f);
	EXPECT_EQ(తదుపరి[0], 2);

	// Block the direct edge and re-solve; path must detour through 1.
	ఖర్చు[0 * N + 2] = INFINITY;
	ASSERT_TRUE(dijkstra::solveBackward(N, 2, ఖర్చు, అబద్ధం, ఉత్తమ, తదుపరి, vis));
	EXPECT_FLOAT_EQ(ఉత్తమ[0], 2.f);
	EXPECT_EQ(తదుపరి[0], 1);
}

TEST(DijkstraTest, GoalUnreachableNoInfiniteLoop)
{
	// Two disconnected components: {0, 1} and {2}. Goal = 2, so 0 and 1 must end
	// up unreachable and the algorithm must not loop.
	constexpr పూర్ణాంకం N = 3;
	దశాంశం ఖర్చు[N * N];
	make_మాతృక<N>(ఖర్చు, {
		{0, 1, 1.f},
		{1, 0, 1.f},
	});

	దశాంశం ఉత్తమ[N];
	పూర్ణాంకం తదుపరి[N];
	తార్కిక vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 2, ఖర్చు, అబద్ధం, ఉత్తమ, తదుపరి, vis));
	EXPECT_FLOAT_EQ(ఉత్తమ[2], 0.f);
	EXPECT_FALSE(ఉత్తమ[0] < dijkstra::kUnreachable);
	EXPECT_FALSE(ఉత్తమ[1] < dijkstra::kUnreachable);
	EXPECT_EQ(తదుపరి[0], -1);
	EXPECT_EQ(తదుపరి[1], -1);
}

TEST(DijkstraTest, Na_NTreated_As_Missing_అంచు)
{
	constexpr పూర్ణాంకం N = 3;
	దశాంశం ఖర్చు[N * N];
	make_మాతృక<N>(ఖర్చు, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{0, 2, NAN}, // NaN must be treated like INFINITY, not as a 0 / negative edge
	});

	దశాంశం ఉత్తమ[N];
	పూర్ణాంకం తదుపరి[N];
	తార్కిక vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 2, ఖర్చు, అబద్ధం, ఉత్తమ, తదుపరి, vis));
	EXPECT_FLOAT_EQ(ఉత్తమ[0], 2.f);
	EXPECT_EQ(తదుపరి[0], 1);
}

TEST(DijkstraTest, RejectsInvalidInputs)
{
	constexpr పూర్ణాంకం N = 2;
	దశాంశం ఖర్చు[N * N] = { INFINITY, 1.f, 1.f, INFINITY };
	దశాంశం ఉత్తమ[N];
	పూర్ణాంకం తదుపరి[N];
	తార్కిక vis[N];

	EXPECT_FALSE(dijkstra::solveBackward(0, 0, ఖర్చు, అబద్ధం, ఉత్తమ, తదుపరి, vis));
	EXPECT_FALSE(dijkstra::solveBackward(N, -1, ఖర్చు, అబద్ధం, ఉత్తమ, తదుపరి, vis));
	EXPECT_FALSE(dijkstra::solveBackward(N, N, ఖర్చు, అబద్ధం, ఉత్తమ, తదుపరి, vis));
	EXPECT_FALSE(dijkstra::solveBackward(N, 0, శూన్యసూచిక, అబద్ధం, ఉత్తమ, తదుపరి, vis));
	EXPECT_FALSE(dijkstra::solveBackward(N, 0, ఖర్చు, అబద్ధం, శూన్యసూచిక, తదుపరి, vis));
}

TEST(DijkstraTest, సౌష్ఠవ_Packed_Matches_Asymmetric)
{
	// Same undirected weighted graph expressed two ways: full N*N matrix with both
	// halves filled, and packed upper triangle. Results must match for both layouts.
	constexpr పూర్ణాంకం N = 5;

	// Edges (a, b, w) with a < b.
	నిర్మాణం Undir_అంచు { పూర్ణాంకం a, b; దశాంశం w; };
	స్థిరరాశి Undir_అంచు అంచులు[] = {
		{0, 1, 2.f},
		{0, 2, 4.f},
		{1, 2, 1.f},
		{1, 3, 7.f},
		{2, 3, 3.f},
		{3, 4, 2.f},
	};

	దశాంశం నిండిన[N * N];

	కొరకు (పూర్ణాంకం i = 0; i < N * N; ++i) { నిండిన[i] = INFINITY; }

	constexpr పూర్ణాంకం kPacked = N * (N - 1) / 2;
	దశాంశం packed[kPacked];

	కొరకు (పూర్ణాంకం i = 0; i < kPacked; ++i) { packed[i] = INFINITY; }

	కొరకు (స్థిరరాశి స్వయంచాలక &e : అంచులు) {
		నిండిన[e.a * N + e.b] = e.w;
		నిండిన[e.b * N + e.a] = e.w;
		packed[e.a * (2 * N - e.a - 1) / 2 + (e.b - e.a - 1)] = e.w;
	}

	దశాంశం ఉత్తమ_నిండిన[N], ఉత్తమ_pack[N];
	పూర్ణాంకం తదుపరి_నిండిన[N], తదుపరి_pack[N];
	తార్కిక vis[N];

	కొరకు (పూర్ణాంకం goal = 0; goal < N; ++goal) {
		ASSERT_TRUE(dijkstra::solveBackward(N, goal, నిండిన, అబద్ధం, ఉత్తమ_నిండిన, తదుపరి_నిండిన, vis));
		ASSERT_TRUE(dijkstra::solveBackward(N, goal, packed, నిజం, ఉత్తమ_pack, తదుపరి_pack, vis));

		కొరకు (పూర్ణాంకం i = 0; i < N; ++i) {
			EXPECT_FLOAT_EQ(ఉత్తమ_నిండిన[i], ఉత్తమ_pack[i]) << "goal=" << goal << " i=" << i;
			EXPECT_EQ(తదుపరి_నిండిన[i], తదుపరి_pack[i]) << "goal=" << goal << " i=" << i;
		}
	}
}

TEST(DijkstraTest, ForwardWalkReachesGoal)
{
	// 5-node graph: 0 -> 1 -> 2 -> 3 -> 4, plus a long detour 0 -> 4 directly.
	// Solving once with goal = 4, then for any start we should reach 4 by following
	// next_node without re-solving.
	constexpr పూర్ణాంకం N = 5;
	దశాంశం ఖర్చు[N * N];
	make_మాతృక<N>(ఖర్చు, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{2, 3, 1.f},
		{3, 4, 1.f},
		{0, 4, 100.f}, // direct but expensive
	});

	దశాంశం ఉత్తమ[N];
	పూర్ణాంకం తదుపరి[N];
	తార్కిక vis[N];
	ASSERT_TRUE(dijkstra::solveBackward(N, 4, ఖర్చు, అబద్ధం, ఉత్తమ, తదుపరి, vis));

	కొరకు (పూర్ణాంకం ప్రారంభం = 0; ప్రారంభం < N - 1; ++ప్రారంభం) {
		పూర్ణాంకం u = ప్రారంభం;
		పూర్ణాంకం hops = 0;

		అయితే (u != 4 && hops < N) {
			ASSERT_GE(తదుపరి[u], 0) << "stuck at " << u << " from start " << ప్రారంభం;
			u = తదుపరి[u];
			++hops;
		}

		EXPECT_EQ(u, 4) << "did not reach goal from " << ప్రారంభం;
	}
}

TEST(DijkstraTest, సౌష్ఠవ_Pair_సూచీ)
{
	// create a NxN matrix and fill with random data. Then pack the upper triangle into a 1D array.
	// Then loop through the upper triangle and read the values from the array using  symmetricPairIndex
	// and verify that the values match.
	constexpr size_t N = 21;
	constexpr size_t k_జతలు = N * (N - 1) / 2;

	దశాంశం మాతృక[N][N];
	దశాంశం packed[k_జతలు];
	std::srand(42);
	size_t idx = 0;

	కొరకు (size_t i = 0; i < N; ++i) {
		కొరకు (size_t j = i + 1; j < N; ++j) {
			మాతృక[i][j] = static_cast<దశాంశం>(std::rand()) / static_cast<దశాంశం>(RAND_MAX);
			మాతృక[j][i] = మాతృక[i][j]; // symmetric matrix
			packed[idx++] = మాతృక[i][j];
		}
	}

	size_t counter = 0;

	కొరకు (size_t i = 0; i < N; ++i) {
		కొరకు (size_t j = i + 1; j < N; ++j) {
			counter++;
			EXPECT_FLOAT_EQ(మాతృక[i][j], packed[dijkstra::సౌష్ఠవ_Pair_సూచీ_2(i, j, N)]);
			EXPECT_FLOAT_EQ(మాతృక[i][j], packed[dijkstra::సౌష్ఠవ_Pair_సూచీ_2(j, i, N)]);
		}
	}

	// verify that we have the expected number of pairs
	EXPECT_EQ(counter, k_జతలు);
}
