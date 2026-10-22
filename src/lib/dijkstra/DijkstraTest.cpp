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

#ضمن <gtest/gtest.h>
#ضمن <cstdlib>
#ضمن <math.h>
#ضمن "dijkstra.h"

نطاق
{

// Helper: assemble cost matrix entries into a row-major buffer. Use INFINITY for
// missing edges; explicit list of finite (i, j, cost) triples is concise in tests.
بنية کنارا { صحيح from, to; عائم لاگت; };

سانچا <صحيح N>
فراغ make_میٹرکس(عائم (&m)[N * N], std::initializer_list<کنارا> کنارے)
{
	لكل (صحيح i = 0; i < N * N; ++i) {
		m[i] = INFINITY;
	}

	لكل (ثابت خودکار &e : کنارے) {
		m[e.from * N + e.to] = e.لاگت;
	}
}

} // namespace

TEST(DijkstraTest, Single_گرہ_At_Goal)
{
	constexpr صحيح N = 1;
	عائم لاگت[N * N] = { INFINITY };
	عائم بہترین[N];
	صحيح اگلا[N];
	منطقي vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 0, لاگت, خطأ, بہترین, اگلا, vis));
	EXPECT_FLOAT_EQ(بہترین[0], 0.f);
	EXPECT_EQ(اگلا[0], -1);
}

TEST(DijkstraTest, Asymmetric_سطر_گراف_Forward)
{
	// 0 -> 1 -> 2, going forward is cheap, going backward is expensive.
	constexpr صحيح N = 3;
	عائم لاگت[N * N];
	make_میٹرکس<N>(لاگت, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{1, 0, 100.f},
		{2, 1, 100.f},
	});

	عائم بہترین[N];
	صحيح اگلا[N];
	منطقي vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 2, لاگت, خطأ, بہترین, اگلا, vis));

	EXPECT_FLOAT_EQ(بہترین[0], 2.f); // 0 -> 1 -> 2 costs 1 + 1
	EXPECT_FLOAT_EQ(بہترین[1], 1.f); // 1 -> 2
	EXPECT_FLOAT_EQ(بہترین[2], 0.f);
	EXPECT_EQ(اگلا[0], 1);
	EXPECT_EQ(اگلا[1], 2);
	EXPECT_EQ(اگلا[2], -1);
}

TEST(DijkstraTest, Asymmetric_سطر_گراف_الٹ)
{
	// Same matrix as above; with goal = 0, paths must use the expensive back-edges,
	// confirming the solver respects edge direction (uses cost[i][j] = i -> j, not j -> i).
	constexpr صحيح N = 3;
	عائم لاگت[N * N];
	make_میٹرکس<N>(لاگت, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{1, 0, 100.f},
		{2, 1, 100.f},
	});

	عائم بہترین[N];
	صحيح اگلا[N];
	منطقي vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 0, لاگت, خطأ, بہترین, اگلا, vis));

	EXPECT_FLOAT_EQ(بہترین[0], 0.f);
	EXPECT_FLOAT_EQ(بہترین[1], 100.f);
	EXPECT_FLOAT_EQ(بہترین[2], 200.f);
	EXPECT_EQ(اگلا[1], 0);
	EXPECT_EQ(اگلا[2], 1);
	EXPECT_EQ(اگلا[0], -1);
}

TEST(DijkstraTest, Triangle_With_Blocked_کنارا)
{
	// Direct edge 0 -> 2 is cheap; remove it and the solver must route via 1.
	constexpr صحيح N = 3;
	عائم لاگت[N * N];
	make_میٹرکس<N>(لاگت, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{0, 2, 0.5f},
	});

	عائم بہترین[N];
	صحيح اگلا[N];
	منطقي vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 2, لاگت, خطأ, بہترین, اگلا, vis));
	EXPECT_FLOAT_EQ(بہترین[0], 0.5f);
	EXPECT_EQ(اگلا[0], 2);

	// Block the direct edge and re-solve; path must detour through 1.
	لاگت[0 * N + 2] = INFINITY;
	ASSERT_TRUE(dijkstra::solveBackward(N, 2, لاگت, خطأ, بہترین, اگلا, vis));
	EXPECT_FLOAT_EQ(بہترین[0], 2.f);
	EXPECT_EQ(اگلا[0], 1);
}

TEST(DijkstraTest, GoalUnreachableNoInfiniteLoop)
{
	// Two disconnected components: {0, 1} and {2}. Goal = 2, so 0 and 1 must end
	// up unreachable and the algorithm must not loop.
	constexpr صحيح N = 3;
	عائم لاگت[N * N];
	make_میٹرکس<N>(لاگت, {
		{0, 1, 1.f},
		{1, 0, 1.f},
	});

	عائم بہترین[N];
	صحيح اگلا[N];
	منطقي vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 2, لاگت, خطأ, بہترین, اگلا, vis));
	EXPECT_FLOAT_EQ(بہترین[2], 0.f);
	EXPECT_FALSE(بہترین[0] < dijkstra::kUnreachable);
	EXPECT_FALSE(بہترین[1] < dijkstra::kUnreachable);
	EXPECT_EQ(اگلا[0], -1);
	EXPECT_EQ(اگلا[1], -1);
}

TEST(DijkstraTest, Na_NTreated_As_Missing_کنارا)
{
	constexpr صحيح N = 3;
	عائم لاگت[N * N];
	make_میٹرکس<N>(لاگت, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{0, 2, NAN}, // NaN must be treated like INFINITY, not as a 0 / negative edge
	});

	عائم بہترین[N];
	صحيح اگلا[N];
	منطقي vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 2, لاگت, خطأ, بہترین, اگلا, vis));
	EXPECT_FLOAT_EQ(بہترین[0], 2.f);
	EXPECT_EQ(اگلا[0], 1);
}

TEST(DijkstraTest, RejectsInvalidInputs)
{
	constexpr صحيح N = 2;
	عائم لاگت[N * N] = { INFINITY, 1.f, 1.f, INFINITY };
	عائم بہترین[N];
	صحيح اگلا[N];
	منطقي vis[N];

	EXPECT_FALSE(dijkstra::solveBackward(0, 0, لاگت, خطأ, بہترین, اگلا, vis));
	EXPECT_FALSE(dijkstra::solveBackward(N, -1, لاگت, خطأ, بہترین, اگلا, vis));
	EXPECT_FALSE(dijkstra::solveBackward(N, N, لاگت, خطأ, بہترین, اگلا, vis));
	EXPECT_FALSE(dijkstra::solveBackward(N, 0, خالی_سوچک, خطأ, بہترین, اگلا, vis));
	EXPECT_FALSE(dijkstra::solveBackward(N, 0, لاگت, خطأ, خالی_سوچک, اگلا, vis));
}

TEST(DijkstraTest, متشاکل_Packed_Matches_Asymmetric)
{
	// Same undirected weighted graph expressed two ways: full N*N matrix with both
	// halves filled, and packed upper triangle. Results must match for both layouts.
	constexpr صحيح N = 5;

	// Edges (a, b, w) with a < b.
	بنية Undir_کنارا { صحيح a, b; عائم w; };
	ثابت Undir_کنارا کنارے[] = {
		{0, 1, 2.f},
		{0, 2, 4.f},
		{1, 2, 1.f},
		{1, 3, 7.f},
		{2, 3, 3.f},
		{3, 4, 2.f},
	};

	عائم بھریا[N * N];

	لكل (صحيح i = 0; i < N * N; ++i) { بھریا[i] = INFINITY; }

	constexpr صحيح kPacked = N * (N - 1) / 2;
	عائم packed[kPacked];

	لكل (صحيح i = 0; i < kPacked; ++i) { packed[i] = INFINITY; }

	لكل (ثابت خودکار &e : کنارے) {
		بھریا[e.a * N + e.b] = e.w;
		بھریا[e.b * N + e.a] = e.w;
		packed[e.a * (2 * N - e.a - 1) / 2 + (e.b - e.a - 1)] = e.w;
	}

	عائم بہترین_بھریا[N], بہترین_pack[N];
	صحيح اگلا_بھریا[N], اگلا_pack[N];
	منطقي vis[N];

	لكل (صحيح goal = 0; goal < N; ++goal) {
		ASSERT_TRUE(dijkstra::solveBackward(N, goal, بھریا, خطأ, بہترین_بھریا, اگلا_بھریا, vis));
		ASSERT_TRUE(dijkstra::solveBackward(N, goal, packed, سچ, بہترین_pack, اگلا_pack, vis));

		لكل (صحيح i = 0; i < N; ++i) {
			EXPECT_FLOAT_EQ(بہترین_بھریا[i], بہترین_pack[i]) << "goal=" << goal << " i=" << i;
			EXPECT_EQ(اگلا_بھریا[i], اگلا_pack[i]) << "goal=" << goal << " i=" << i;
		}
	}
}

TEST(DijkstraTest, ForwardWalkReachesGoal)
{
	// 5-node graph: 0 -> 1 -> 2 -> 3 -> 4, plus a long detour 0 -> 4 directly.
	// Solving once with goal = 4, then for any start we should reach 4 by following
	// next_node without re-solving.
	constexpr صحيح N = 5;
	عائم لاگت[N * N];
	make_میٹرکس<N>(لاگت, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{2, 3, 1.f},
		{3, 4, 1.f},
		{0, 4, 100.f}, // direct but expensive
	});

	عائم بہترین[N];
	صحيح اگلا[N];
	منطقي vis[N];
	ASSERT_TRUE(dijkstra::solveBackward(N, 4, لاگت, خطأ, بہترین, اگلا, vis));

	لكل (صحيح شروع = 0; شروع < N - 1; ++شروع) {
		صحيح u = شروع;
		صحيح hops = 0;

		طالما (u != 4 && hops < N) {
			ASSERT_GE(اگلا[u], 0) << "stuck at " << u << " from start " << شروع;
			u = اگلا[u];
			++hops;
		}

		EXPECT_EQ(u, 4) << "did not reach goal from " << شروع;
	}
}

TEST(DijkstraTest, متشاکل_Pair_اشاریہ)
{
	// create a NxN matrix and fill with random data. Then pack the upper triangle into a 1D array.
	// Then loop through the upper triangle and read the values from the array using  symmetricPairIndex
	// and verify that the values match.
	constexpr size_t N = 21;
	constexpr size_t k_جوڑے = N * (N - 1) / 2;

	عائم میٹرکس[N][N];
	عائم packed[k_جوڑے];
	std::srand(42);
	size_t idx = 0;

	لكل (size_t i = 0; i < N; ++i) {
		لكل (size_t j = i + 1; j < N; ++j) {
			میٹرکس[i][j] = static_cast<عائم>(std::rand()) / static_cast<عائم>(RAND_MAX);
			میٹرکس[j][i] = میٹرکس[i][j]; // symmetric matrix
			packed[idx++] = میٹرکس[i][j];
		}
	}

	size_t counter = 0;

	لكل (size_t i = 0; i < N; ++i) {
		لكل (size_t j = i + 1; j < N; ++j) {
			counter++;
			EXPECT_FLOAT_EQ(میٹرکس[i][j], packed[dijkstra::متشاکل_Pair_اشاریہ_2(i, j, N)]);
			EXPECT_FLOAT_EQ(میٹرکس[i][j], packed[dijkstra::متشاکل_Pair_اشاریہ_2(j, i, N)]);
		}
	}

	// verify that we have the expected number of pairs
	EXPECT_EQ(counter, k_جوڑے);
}
