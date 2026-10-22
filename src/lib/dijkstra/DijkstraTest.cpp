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

#अन्तर्भाव <gtest/gtest.h>
#अन्तर्भाव <cstdlib>
#अन्तर्भाव <math.h>
#अन्तर्भाव "dijkstra.h"

नामाकाश
{

// Helper: assemble cost matrix entries into a row-major buffer. Use INFINITY for
// missing edges; explicit list of finite (i, j, cost) triples is concise in tests.
संरचना Edge { पूर्णाङ्क from, to; भिन्न cost; };

प्रतिरूपम् <पूर्णाङ्क N>
शून्य makeMatrix(भिन्न (&m)[N * N], std::initializer_list<Edge> धाराः)
{
	प्रति (पूर्णाङ्क i = 0; i < N * N; ++i) {
		m[i] = INFINITY;
	}

	प्रति (ध्रुव स्वचालित &e : धाराः) {
		m[e.from * N + e.to] = e.cost;
	}
}

} // namespace

TEST(DijkstraTest, Single_ग्रन्थिः_At_Goal)
{
	constexpr पूर्णाङ्क N = 1;
	भिन्न cost[N * N] = { INFINITY };
	भिन्न best[N];
	पूर्णाङ्क अग्र[N];
	तार्किक vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 0, cost, असत्य, best, अग्र, vis));
	EXPECT_FLOAT_EQ(best[0], 0.f);
	EXPECT_EQ(अग्र[0], -1);
}

TEST(DijkstraTest, Asymmetric_पङ्क्तिः_Graph_Forward)
{
	// 0 -> 1 -> 2, going forward is cheap, going backward is expensive.
	constexpr पूर्णाङ्क N = 3;
	भिन्न cost[N * N];
	makeMatrix<N>(cost, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{1, 0, 100.f},
		{2, 1, 100.f},
	});

	भिन्न best[N];
	पूर्णाङ्क अग्र[N];
	तार्किक vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 2, cost, असत्य, best, अग्र, vis));

	EXPECT_FLOAT_EQ(best[0], 2.f); // 0 -> 1 -> 2 costs 1 + 1
	EXPECT_FLOAT_EQ(best[1], 1.f); // 1 -> 2
	EXPECT_FLOAT_EQ(best[2], 0.f);
	EXPECT_EQ(अग्र[0], 1);
	EXPECT_EQ(अग्र[1], 2);
	EXPECT_EQ(अग्र[2], -1);
}

TEST(DijkstraTest, Asymmetric_पङ्क्तिः_Graph_Reverse)
{
	// Same matrix as above; with goal = 0, paths must use the expensive back-edges,
	// confirming the solver respects edge direction (uses cost[i][j] = i -> j, not j -> i).
	constexpr पूर्णाङ्क N = 3;
	भिन्न cost[N * N];
	makeMatrix<N>(cost, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{1, 0, 100.f},
		{2, 1, 100.f},
	});

	भिन्न best[N];
	पूर्णाङ्क अग्र[N];
	तार्किक vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 0, cost, असत्य, best, अग्र, vis));

	EXPECT_FLOAT_EQ(best[0], 0.f);
	EXPECT_FLOAT_EQ(best[1], 100.f);
	EXPECT_FLOAT_EQ(best[2], 200.f);
	EXPECT_EQ(अग्र[1], 0);
	EXPECT_EQ(अग्र[2], 1);
	EXPECT_EQ(अग्र[0], -1);
}

TEST(DijkstraTest, TriangleWithBlockedEdge)
{
	// Direct edge 0 -> 2 is cheap; remove it and the solver must route via 1.
	constexpr पूर्णाङ्क N = 3;
	भिन्न cost[N * N];
	makeMatrix<N>(cost, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{0, 2, 0.5f},
	});

	भिन्न best[N];
	पूर्णाङ्क अग्र[N];
	तार्किक vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 2, cost, असत्य, best, अग्र, vis));
	EXPECT_FLOAT_EQ(best[0], 0.5f);
	EXPECT_EQ(अग्र[0], 2);

	// Block the direct edge and re-solve; path must detour through 1.
	cost[0 * N + 2] = INFINITY;
	ASSERT_TRUE(dijkstra::solveBackward(N, 2, cost, असत्य, best, अग्र, vis));
	EXPECT_FLOAT_EQ(best[0], 2.f);
	EXPECT_EQ(अग्र[0], 1);
}

TEST(DijkstraTest, GoalUnreachableNoInfiniteLoop)
{
	// Two disconnected components: {0, 1} and {2}. Goal = 2, so 0 and 1 must end
	// up unreachable and the algorithm must not loop.
	constexpr पूर्णाङ्क N = 3;
	भिन्न cost[N * N];
	makeMatrix<N>(cost, {
		{0, 1, 1.f},
		{1, 0, 1.f},
	});

	भिन्न best[N];
	पूर्णाङ्क अग्र[N];
	तार्किक vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 2, cost, असत्य, best, अग्र, vis));
	EXPECT_FLOAT_EQ(best[2], 0.f);
	EXPECT_FALSE(best[0] < dijkstra::kUnreachable);
	EXPECT_FALSE(best[1] < dijkstra::kUnreachable);
	EXPECT_EQ(अग्र[0], -1);
	EXPECT_EQ(अग्र[1], -1);
}

TEST(DijkstraTest, NaNTreatedAsMissingEdge)
{
	constexpr पूर्णाङ्क N = 3;
	भिन्न cost[N * N];
	makeMatrix<N>(cost, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{0, 2, NAN}, // NaN must be treated like INFINITY, not as a 0 / negative edge
	});

	भिन्न best[N];
	पूर्णाङ्क अग्र[N];
	तार्किक vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 2, cost, असत्य, best, अग्र, vis));
	EXPECT_FLOAT_EQ(best[0], 2.f);
	EXPECT_EQ(अग्र[0], 1);
}

TEST(DijkstraTest, RejectsInvalidInputs)
{
	constexpr पूर्णाङ्क N = 2;
	भिन्न cost[N * N] = { INFINITY, 1.f, 1.f, INFINITY };
	भिन्न best[N];
	पूर्णाङ्क अग्र[N];
	तार्किक vis[N];

	EXPECT_FALSE(dijkstra::solveBackward(0, 0, cost, असत्य, best, अग्र, vis));
	EXPECT_FALSE(dijkstra::solveBackward(N, -1, cost, असत्य, best, अग्र, vis));
	EXPECT_FALSE(dijkstra::solveBackward(N, N, cost, असत्य, best, अग्र, vis));
	EXPECT_FALSE(dijkstra::solveBackward(N, 0, शून्यसूचकः, असत्य, best, अग्र, vis));
	EXPECT_FALSE(dijkstra::solveBackward(N, 0, cost, असत्य, शून्यसूचकः, अग्र, vis));
}

TEST(DijkstraTest, सममितम्_Packed_Matches_Asymmetric)
{
	// Same undirected weighted graph expressed two ways: full N*N matrix with both
	// halves filled, and packed upper triangle. Results must match for both layouts.
	constexpr पूर्णाङ्क N = 5;

	// Edges (a, b, w) with a < b.
	संरचना UndirEdge { पूर्णाङ्क a, b; भिन्न w; };
	ध्रुव UndirEdge धाराः[] = {
		{0, 1, 2.f},
		{0, 2, 4.f},
		{1, 2, 1.f},
		{1, 3, 7.f},
		{2, 3, 3.f},
		{3, 4, 2.f},
	};

	भिन्न पूर्णम्[N * N];

	प्रति (पूर्णाङ्क i = 0; i < N * N; ++i) { पूर्णम्[i] = INFINITY; }

	constexpr पूर्णाङ्क kPacked = N * (N - 1) / 2;
	भिन्न packed[kPacked];

	प्रति (पूर्णाङ्क i = 0; i < kPacked; ++i) { packed[i] = INFINITY; }

	प्रति (ध्रुव स्वचालित &e : धाराः) {
		पूर्णम्[e.a * N + e.b] = e.w;
		पूर्णम्[e.b * N + e.a] = e.w;
		packed[e.a * (2 * N - e.a - 1) / 2 + (e.b - e.a - 1)] = e.w;
	}

	भिन्न best_पूर्णम्[N], best_pack[N];
	पूर्णाङ्क अग्र_पूर्णम्[N], अग्र_pack[N];
	तार्किक vis[N];

	प्रति (पूर्णाङ्क goal = 0; goal < N; ++goal) {
		ASSERT_TRUE(dijkstra::solveBackward(N, goal, पूर्णम्, असत्य, best_पूर्णम्, अग्र_पूर्णम्, vis));
		ASSERT_TRUE(dijkstra::solveBackward(N, goal, packed, सत्य, best_pack, अग्र_pack, vis));

		प्रति (पूर्णाङ्क i = 0; i < N; ++i) {
			EXPECT_FLOAT_EQ(best_पूर्णम्[i], best_pack[i]) << "goal=" << goal << " i=" << i;
			EXPECT_EQ(अग्र_पूर्णम्[i], अग्र_pack[i]) << "goal=" << goal << " i=" << i;
		}
	}
}

TEST(DijkstraTest, ForwardWalkReachesGoal)
{
	// 5-node graph: 0 -> 1 -> 2 -> 3 -> 4, plus a long detour 0 -> 4 directly.
	// Solving once with goal = 4, then for any start we should reach 4 by following
	// next_node without re-solving.
	constexpr पूर्णाङ्क N = 5;
	भिन्न cost[N * N];
	makeMatrix<N>(cost, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{2, 3, 1.f},
		{3, 4, 1.f},
		{0, 4, 100.f}, // direct but expensive
	});

	भिन्न best[N];
	पूर्णाङ्क अग्र[N];
	तार्किक vis[N];
	ASSERT_TRUE(dijkstra::solveBackward(N, 4, cost, असत्य, best, अग्र, vis));

	प्रति (पूर्णाङ्क start = 0; start < N - 1; ++start) {
		पूर्णाङ्क u = start;
		पूर्णाङ्क hops = 0;

		यावत् (u != 4 && hops < N) {
			ASSERT_GE(अग्र[u], 0) << "stuck at " << u << " from start " << start;
			u = अग्र[u];
			++hops;
		}

		EXPECT_EQ(u, 4) << "did not reach goal from " << start;
	}
}

TEST(DijkstraTest, सममितम्_Pair_सूचकांक)
{
	// create a NxN matrix and fill with random data. Then pack the upper triangle into a 1D array.
	// Then loop through the upper triangle and read the values from the array using  symmetricPairIndex
	// and verify that the values match.
	constexpr size_t N = 21;
	constexpr size_t k_युग्मानि = N * (N - 1) / 2;

	भिन्न matrix[N][N];
	भिन्न packed[k_युग्मानि];
	std::बीजं_रोपय(42);
	size_t idx = 0;

	प्रति (size_t i = 0; i < N; ++i) {
		प्रति (size_t j = i + 1; j < N; ++j) {
			matrix[i][j] = static_cast<भिन्न>(std::यादृच्छिकम्()) / static_cast<भिन्न>(RAND_MAX);
			matrix[j][i] = matrix[i][j]; // symmetric matrix
			packed[idx++] = matrix[i][j];
		}
	}

	size_t गणक = 0;

	प्रति (size_t i = 0; i < N; ++i) {
		प्रति (size_t j = i + 1; j < N; ++j) {
			गणक++;
			EXPECT_FLOAT_EQ(matrix[i][j], packed[dijkstra::सममितम्_Pair_सूचकांक_2(i, j, N)]);
			EXPECT_FLOAT_EQ(matrix[i][j], packed[dijkstra::सममितम्_Pair_सूचकांक_2(j, i, N)]);
		}
	}

	// verify that we have the expected number of pairs
	EXPECT_EQ(गणक, k_युग्मानि);
}
