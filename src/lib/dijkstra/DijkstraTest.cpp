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

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstdlib>
#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "dijkstra.h"

ਨਾਮਾਕਾਸ਼
{

// Helper: assemble cost matrix entries into a row-major buffer. Use INFINITY for
// missing edges; explicit list of finite (i, j, cost) triples is concise in tests.
ਬਣਤਰ ਕਿਨਾਰਾ { ਪੂਰਨ from, to; ਦਸ਼ਮਲਵ ਲਾਗਤ; };

ਸਾਂਚਾ <ਪੂਰਨ N>
ਖਾਲੀ make_ਮੈਟ੍ਰਿਕਸ(ਦਸ਼ਮਲਵ (&m)[N * N], std::initializer_list<ਕਿਨਾਰਾ> ਕਿਨਾਰੇ)
{
	ਲਈ (ਪੂਰਨ i = 0; i < N * N; ++i) {
		m[i] = INFINITY;
	}

	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &e : ਕਿਨਾਰੇ) {
		m[e.from * N + e.to] = e.ਲਾਗਤ;
	}
}

} // namespace

TEST(DijkstraTest, Single_ਗੰਢ_At_Goal)
{
	constexpr ਪੂਰਨ N = 1;
	ਦਸ਼ਮਲਵ ਲਾਗਤ[N * N] = { INFINITY };
	ਦਸ਼ਮਲਵ ਉੱਤਮ[N];
	ਪੂਰਨ ਅਗਲਾ[N];
	ਤਾਰਕਿਕ vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 0, ਲਾਗਤ, ਝੂਠ, ਉੱਤਮ, ਅਗਲਾ, vis));
	EXPECT_FLOAT_EQ(ਉੱਤਮ[0], 0.f);
	EXPECT_EQ(ਅਗਲਾ[0], -1);
}

TEST(DijkstraTest, Asymmetric_ਸਤਰ_ਗ੍ਰਾਫ_Forward)
{
	// 0 -> 1 -> 2, going forward is cheap, going backward is expensive.
	constexpr ਪੂਰਨ N = 3;
	ਦਸ਼ਮਲਵ ਲਾਗਤ[N * N];
	make_ਮੈਟ੍ਰਿਕਸ<N>(ਲਾਗਤ, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{1, 0, 100.f},
		{2, 1, 100.f},
	});

	ਦਸ਼ਮਲਵ ਉੱਤਮ[N];
	ਪੂਰਨ ਅਗਲਾ[N];
	ਤਾਰਕਿਕ vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 2, ਲਾਗਤ, ਝੂਠ, ਉੱਤਮ, ਅਗਲਾ, vis));

	EXPECT_FLOAT_EQ(ਉੱਤਮ[0], 2.f); // 0 -> 1 -> 2 costs 1 + 1
	EXPECT_FLOAT_EQ(ਉੱਤਮ[1], 1.f); // 1 -> 2
	EXPECT_FLOAT_EQ(ਉੱਤਮ[2], 0.f);
	EXPECT_EQ(ਅਗਲਾ[0], 1);
	EXPECT_EQ(ਅਗਲਾ[1], 2);
	EXPECT_EQ(ਅਗਲਾ[2], -1);
}

TEST(DijkstraTest, Asymmetric_ਸਤਰ_ਗ੍ਰਾਫ_ਉਲਟ)
{
	// Same matrix as above; with goal = 0, paths must use the expensive back-edges,
	// confirming the solver respects edge direction (uses cost[i][j] = i -> j, not j -> i).
	constexpr ਪੂਰਨ N = 3;
	ਦਸ਼ਮਲਵ ਲਾਗਤ[N * N];
	make_ਮੈਟ੍ਰਿਕਸ<N>(ਲਾਗਤ, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{1, 0, 100.f},
		{2, 1, 100.f},
	});

	ਦਸ਼ਮਲਵ ਉੱਤਮ[N];
	ਪੂਰਨ ਅਗਲਾ[N];
	ਤਾਰਕਿਕ vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 0, ਲਾਗਤ, ਝੂਠ, ਉੱਤਮ, ਅਗਲਾ, vis));

	EXPECT_FLOAT_EQ(ਉੱਤਮ[0], 0.f);
	EXPECT_FLOAT_EQ(ਉੱਤਮ[1], 100.f);
	EXPECT_FLOAT_EQ(ਉੱਤਮ[2], 200.f);
	EXPECT_EQ(ਅਗਲਾ[1], 0);
	EXPECT_EQ(ਅਗਲਾ[2], 1);
	EXPECT_EQ(ਅਗਲਾ[0], -1);
}

TEST(DijkstraTest, Triangle_With_Blocked_ਕਿਨਾਰਾ)
{
	// Direct edge 0 -> 2 is cheap; remove it and the solver must route via 1.
	constexpr ਪੂਰਨ N = 3;
	ਦਸ਼ਮਲਵ ਲਾਗਤ[N * N];
	make_ਮੈਟ੍ਰਿਕਸ<N>(ਲਾਗਤ, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{0, 2, 0.5f},
	});

	ਦਸ਼ਮਲਵ ਉੱਤਮ[N];
	ਪੂਰਨ ਅਗਲਾ[N];
	ਤਾਰਕਿਕ vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 2, ਲਾਗਤ, ਝੂਠ, ਉੱਤਮ, ਅਗਲਾ, vis));
	EXPECT_FLOAT_EQ(ਉੱਤਮ[0], 0.5f);
	EXPECT_EQ(ਅਗਲਾ[0], 2);

	// Block the direct edge and re-solve; path must detour through 1.
	ਲਾਗਤ[0 * N + 2] = INFINITY;
	ASSERT_TRUE(dijkstra::solveBackward(N, 2, ਲਾਗਤ, ਝੂਠ, ਉੱਤਮ, ਅਗਲਾ, vis));
	EXPECT_FLOAT_EQ(ਉੱਤਮ[0], 2.f);
	EXPECT_EQ(ਅਗਲਾ[0], 1);
}

TEST(DijkstraTest, GoalUnreachableNoInfiniteLoop)
{
	// Two disconnected components: {0, 1} and {2}. Goal = 2, so 0 and 1 must end
	// up unreachable and the algorithm must not loop.
	constexpr ਪੂਰਨ N = 3;
	ਦਸ਼ਮਲਵ ਲਾਗਤ[N * N];
	make_ਮੈਟ੍ਰਿਕਸ<N>(ਲਾਗਤ, {
		{0, 1, 1.f},
		{1, 0, 1.f},
	});

	ਦਸ਼ਮਲਵ ਉੱਤਮ[N];
	ਪੂਰਨ ਅਗਲਾ[N];
	ਤਾਰਕਿਕ vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 2, ਲਾਗਤ, ਝੂਠ, ਉੱਤਮ, ਅਗਲਾ, vis));
	EXPECT_FLOAT_EQ(ਉੱਤਮ[2], 0.f);
	EXPECT_FALSE(ਉੱਤਮ[0] < dijkstra::kUnreachable);
	EXPECT_FALSE(ਉੱਤਮ[1] < dijkstra::kUnreachable);
	EXPECT_EQ(ਅਗਲਾ[0], -1);
	EXPECT_EQ(ਅਗਲਾ[1], -1);
}

TEST(DijkstraTest, Na_NTreated_As_Missing_ਕਿਨਾਰਾ)
{
	constexpr ਪੂਰਨ N = 3;
	ਦਸ਼ਮਲਵ ਲਾਗਤ[N * N];
	make_ਮੈਟ੍ਰਿਕਸ<N>(ਲਾਗਤ, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{0, 2, NAN}, // NaN must be treated like INFINITY, not as a 0 / negative edge
	});

	ਦਸ਼ਮਲਵ ਉੱਤਮ[N];
	ਪੂਰਨ ਅਗਲਾ[N];
	ਤਾਰਕਿਕ vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 2, ਲਾਗਤ, ਝੂਠ, ਉੱਤਮ, ਅਗਲਾ, vis));
	EXPECT_FLOAT_EQ(ਉੱਤਮ[0], 2.f);
	EXPECT_EQ(ਅਗਲਾ[0], 1);
}

TEST(DijkstraTest, RejectsInvalidInputs)
{
	constexpr ਪੂਰਨ N = 2;
	ਦਸ਼ਮਲਵ ਲਾਗਤ[N * N] = { INFINITY, 1.f, 1.f, INFINITY };
	ਦਸ਼ਮਲਵ ਉੱਤਮ[N];
	ਪੂਰਨ ਅਗਲਾ[N];
	ਤਾਰਕਿਕ vis[N];

	EXPECT_FALSE(dijkstra::solveBackward(0, 0, ਲਾਗਤ, ਝੂਠ, ਉੱਤਮ, ਅਗਲਾ, vis));
	EXPECT_FALSE(dijkstra::solveBackward(N, -1, ਲਾਗਤ, ਝੂਠ, ਉੱਤਮ, ਅਗਲਾ, vis));
	EXPECT_FALSE(dijkstra::solveBackward(N, N, ਲਾਗਤ, ਝੂਠ, ਉੱਤਮ, ਅਗਲਾ, vis));
	EXPECT_FALSE(dijkstra::solveBackward(N, 0, ਖਾਲੀ_ਸੂਚਕ, ਝੂਠ, ਉੱਤਮ, ਅਗਲਾ, vis));
	EXPECT_FALSE(dijkstra::solveBackward(N, 0, ਲਾਗਤ, ਝੂਠ, ਖਾਲੀ_ਸੂਚਕ, ਅਗਲਾ, vis));
}

TEST(DijkstraTest, ਸਮਮਿਤ_Packed_Matches_Asymmetric)
{
	// Same undirected weighted graph expressed two ways: full N*N matrix with both
	// halves filled, and packed upper triangle. Results must match for both layouts.
	constexpr ਪੂਰਨ N = 5;

	// Edges (a, b, w) with a < b.
	ਬਣਤਰ Undir_ਕਿਨਾਰਾ { ਪੂਰਨ a, b; ਦਸ਼ਮਲਵ w; };
	ਅਚਲ Undir_ਕਿਨਾਰਾ ਕਿਨਾਰੇ[] = {
		{0, 1, 2.f},
		{0, 2, 4.f},
		{1, 2, 1.f},
		{1, 3, 7.f},
		{2, 3, 3.f},
		{3, 4, 2.f},
	};

	ਦਸ਼ਮਲਵ ਭਰਿਆ[N * N];

	ਲਈ (ਪੂਰਨ i = 0; i < N * N; ++i) { ਭਰਿਆ[i] = INFINITY; }

	constexpr ਪੂਰਨ kPacked = N * (N - 1) / 2;
	ਦਸ਼ਮਲਵ packed[kPacked];

	ਲਈ (ਪੂਰਨ i = 0; i < kPacked; ++i) { packed[i] = INFINITY; }

	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &e : ਕਿਨਾਰੇ) {
		ਭਰਿਆ[e.a * N + e.b] = e.w;
		ਭਰਿਆ[e.b * N + e.a] = e.w;
		packed[e.a * (2 * N - e.a - 1) / 2 + (e.b - e.a - 1)] = e.w;
	}

	ਦਸ਼ਮਲਵ ਉੱਤਮ_ਭਰਿਆ[N], ਉੱਤਮ_pack[N];
	ਪੂਰਨ ਅਗਲਾ_ਭਰਿਆ[N], ਅਗਲਾ_pack[N];
	ਤਾਰਕਿਕ vis[N];

	ਲਈ (ਪੂਰਨ goal = 0; goal < N; ++goal) {
		ASSERT_TRUE(dijkstra::solveBackward(N, goal, ਭਰਿਆ, ਝੂਠ, ਉੱਤਮ_ਭਰਿਆ, ਅਗਲਾ_ਭਰਿਆ, vis));
		ASSERT_TRUE(dijkstra::solveBackward(N, goal, packed, ਸੱਚ, ਉੱਤਮ_pack, ਅਗਲਾ_pack, vis));

		ਲਈ (ਪੂਰਨ i = 0; i < N; ++i) {
			EXPECT_FLOAT_EQ(ਉੱਤਮ_ਭਰਿਆ[i], ਉੱਤਮ_pack[i]) << "goal=" << goal << " i=" << i;
			EXPECT_EQ(ਅਗਲਾ_ਭਰਿਆ[i], ਅਗਲਾ_pack[i]) << "goal=" << goal << " i=" << i;
		}
	}
}

TEST(DijkstraTest, ForwardWalkReachesGoal)
{
	// 5-node graph: 0 -> 1 -> 2 -> 3 -> 4, plus a long detour 0 -> 4 directly.
	// Solving once with goal = 4, then for any start we should reach 4 by following
	// next_node without re-solving.
	constexpr ਪੂਰਨ N = 5;
	ਦਸ਼ਮਲਵ ਲਾਗਤ[N * N];
	make_ਮੈਟ੍ਰਿਕਸ<N>(ਲਾਗਤ, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{2, 3, 1.f},
		{3, 4, 1.f},
		{0, 4, 100.f}, // direct but expensive
	});

	ਦਸ਼ਮਲਵ ਉੱਤਮ[N];
	ਪੂਰਨ ਅਗਲਾ[N];
	ਤਾਰਕਿਕ vis[N];
	ASSERT_TRUE(dijkstra::solveBackward(N, 4, ਲਾਗਤ, ਝੂਠ, ਉੱਤਮ, ਅਗਲਾ, vis));

	ਲਈ (ਪੂਰਨ ਸ਼ੁਰੂ = 0; ਸ਼ੁਰੂ < N - 1; ++ਸ਼ੁਰੂ) {
		ਪੂਰਨ u = ਸ਼ੁਰੂ;
		ਪੂਰਨ hops = 0;

		ਜਦੋਂ (u != 4 && hops < N) {
			ASSERT_GE(ਅਗਲਾ[u], 0) << "stuck at " << u << " from start " << ਸ਼ੁਰੂ;
			u = ਅਗਲਾ[u];
			++hops;
		}

		EXPECT_EQ(u, 4) << "did not reach goal from " << ਸ਼ੁਰੂ;
	}
}

TEST(DijkstraTest, ਸਮਮਿਤ_Pair_ਸੂਚਕਾਂਕ)
{
	// create a NxN matrix and fill with random data. Then pack the upper triangle into a 1D array.
	// Then loop through the upper triangle and read the values from the array using  symmetricPairIndex
	// and verify that the values match.
	constexpr size_t N = 21;
	constexpr size_t k_ਜੋੜੇ = N * (N - 1) / 2;

	ਦਸ਼ਮਲਵ ਮੈਟ੍ਰਿਕਸ[N][N];
	ਦਸ਼ਮਲਵ packed[k_ਜੋੜੇ];
	std::srand(42);
	size_t idx = 0;

	ਲਈ (size_t i = 0; i < N; ++i) {
		ਲਈ (size_t j = i + 1; j < N; ++j) {
			ਮੈਟ੍ਰਿਕਸ[i][j] = static_cast<ਦਸ਼ਮਲਵ>(std::rand()) / static_cast<ਦਸ਼ਮਲਵ>(RAND_MAX);
			ਮੈਟ੍ਰਿਕਸ[j][i] = ਮੈਟ੍ਰਿਕਸ[i][j]; // symmetric matrix
			packed[idx++] = ਮੈਟ੍ਰਿਕਸ[i][j];
		}
	}

	size_t counter = 0;

	ਲਈ (size_t i = 0; i < N; ++i) {
		ਲਈ (size_t j = i + 1; j < N; ++j) {
			counter++;
			EXPECT_FLOAT_EQ(ਮੈਟ੍ਰਿਕਸ[i][j], packed[dijkstra::ਸਮਮਿਤ_Pair_ਸੂਚਕਾਂਕ_2(i, j, N)]);
			EXPECT_FLOAT_EQ(ਮੈਟ੍ਰਿਕਸ[i][j], packed[dijkstra::ਸਮਮਿਤ_Pair_ਸੂਚਕਾਂਕ_2(j, i, N)]);
		}
	}

	// verify that we have the expected number of pairs
	EXPECT_EQ(counter, k_ਜੋੜੇ);
}
