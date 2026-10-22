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

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>
#ಅನ್ತರ್ಭಾವ <cstdlib>
#ಅನ್ತರ್ಭಾವ <math.h>
#ಅನ್ತರ್ಭಾವ "dijkstra.h"

ನಾಮಾಕಾಶ
{

// Helper: assemble cost matrix entries into a row-major buffer. Use INFINITY for
// missing edges; explicit list of finite (i, j, cost) triples is concise in tests.
ರಚನೆ ಅಂಚು { ಪೂರ್ಣಾಂಕ from, to; ದಶಮಾಂಶ ವೆಚ್ಚ; };

ಮಾದರಿ <ಪೂರ್ಣಾಂಕ N>
ಶೂನ್ಯ make_ಮಾತೃಕೆ(ದಶಮಾಂಶ (&m)[N * N], std::initializer_list<ಅಂಚು> ಅಂಚುಗಳು)
{
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < N * N; ++i) {
		m[i] = INFINITY;
	}

	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &e : ಅಂಚುಗಳು) {
		m[e.from * N + e.to] = e.ವೆಚ್ಚ;
	}
}

} // namespace

TEST(DijkstraTest, Single_ಗಂಟು_At_Goal)
{
	constexpr ಪೂರ್ಣಾಂಕ N = 1;
	ದಶಮಾಂಶ ವೆಚ್ಚ[N * N] = { INFINITY };
	ದಶಮಾಂಶ ಉತ್ತಮ[N];
	ಪೂರ್ಣಾಂಕ ಮುಂದಿನ[N];
	ತಾರ್ಕಿಕ vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 0, ವೆಚ್ಚ, ಸುಳ್ಳು, ಉತ್ತಮ, ಮುಂದಿನ, vis));
	EXPECT_FLOAT_EQ(ಉತ್ತಮ[0], 0.f);
	EXPECT_EQ(ಮುಂದಿನ[0], -1);
}

TEST(DijkstraTest, Asymmetric_ಸಾಲು_ನಕ್ಷೆ_Forward)
{
	// 0 -> 1 -> 2, going forward is cheap, going backward is expensive.
	constexpr ಪೂರ್ಣಾಂಕ N = 3;
	ದಶಮಾಂಶ ವೆಚ್ಚ[N * N];
	make_ಮಾತೃಕೆ<N>(ವೆಚ್ಚ, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{1, 0, 100.f},
		{2, 1, 100.f},
	});

	ದಶಮಾಂಶ ಉತ್ತಮ[N];
	ಪೂರ್ಣಾಂಕ ಮುಂದಿನ[N];
	ತಾರ್ಕಿಕ vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 2, ವೆಚ್ಚ, ಸುಳ್ಳು, ಉತ್ತಮ, ಮುಂದಿನ, vis));

	EXPECT_FLOAT_EQ(ಉತ್ತಮ[0], 2.f); // 0 -> 1 -> 2 costs 1 + 1
	EXPECT_FLOAT_EQ(ಉತ್ತಮ[1], 1.f); // 1 -> 2
	EXPECT_FLOAT_EQ(ಉತ್ತಮ[2], 0.f);
	EXPECT_EQ(ಮುಂದಿನ[0], 1);
	EXPECT_EQ(ಮುಂದಿನ[1], 2);
	EXPECT_EQ(ಮುಂದಿನ[2], -1);
}

TEST(DijkstraTest, Asymmetric_ಸಾಲು_ನಕ್ಷೆ_ಹಿಮ್ಮುಖ)
{
	// Same matrix as above; with goal = 0, paths must use the expensive back-edges,
	// confirming the solver respects edge direction (uses cost[i][j] = i -> j, not j -> i).
	constexpr ಪೂರ್ಣಾಂಕ N = 3;
	ದಶಮಾಂಶ ವೆಚ್ಚ[N * N];
	make_ಮಾತೃಕೆ<N>(ವೆಚ್ಚ, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{1, 0, 100.f},
		{2, 1, 100.f},
	});

	ದಶಮಾಂಶ ಉತ್ತಮ[N];
	ಪೂರ್ಣಾಂಕ ಮುಂದಿನ[N];
	ತಾರ್ಕಿಕ vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 0, ವೆಚ್ಚ, ಸುಳ್ಳು, ಉತ್ತಮ, ಮುಂದಿನ, vis));

	EXPECT_FLOAT_EQ(ಉತ್ತಮ[0], 0.f);
	EXPECT_FLOAT_EQ(ಉತ್ತಮ[1], 100.f);
	EXPECT_FLOAT_EQ(ಉತ್ತಮ[2], 200.f);
	EXPECT_EQ(ಮುಂದಿನ[1], 0);
	EXPECT_EQ(ಮುಂದಿನ[2], 1);
	EXPECT_EQ(ಮುಂದಿನ[0], -1);
}

TEST(DijkstraTest, Triangle_With_Blocked_ಅಂಚು)
{
	// Direct edge 0 -> 2 is cheap; remove it and the solver must route via 1.
	constexpr ಪೂರ್ಣಾಂಕ N = 3;
	ದಶಮಾಂಶ ವೆಚ್ಚ[N * N];
	make_ಮಾತೃಕೆ<N>(ವೆಚ್ಚ, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{0, 2, 0.5f},
	});

	ದಶಮಾಂಶ ಉತ್ತಮ[N];
	ಪೂರ್ಣಾಂಕ ಮುಂದಿನ[N];
	ತಾರ್ಕಿಕ vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 2, ವೆಚ್ಚ, ಸುಳ್ಳು, ಉತ್ತಮ, ಮುಂದಿನ, vis));
	EXPECT_FLOAT_EQ(ಉತ್ತಮ[0], 0.5f);
	EXPECT_EQ(ಮುಂದಿನ[0], 2);

	// Block the direct edge and re-solve; path must detour through 1.
	ವೆಚ್ಚ[0 * N + 2] = INFINITY;
	ASSERT_TRUE(dijkstra::solveBackward(N, 2, ವೆಚ್ಚ, ಸುಳ್ಳು, ಉತ್ತಮ, ಮುಂದಿನ, vis));
	EXPECT_FLOAT_EQ(ಉತ್ತಮ[0], 2.f);
	EXPECT_EQ(ಮುಂದಿನ[0], 1);
}

TEST(DijkstraTest, GoalUnreachableNoInfiniteLoop)
{
	// Two disconnected components: {0, 1} and {2}. Goal = 2, so 0 and 1 must end
	// up unreachable and the algorithm must not loop.
	constexpr ಪೂರ್ಣಾಂಕ N = 3;
	ದಶಮಾಂಶ ವೆಚ್ಚ[N * N];
	make_ಮಾತೃಕೆ<N>(ವೆಚ್ಚ, {
		{0, 1, 1.f},
		{1, 0, 1.f},
	});

	ದಶಮಾಂಶ ಉತ್ತಮ[N];
	ಪೂರ್ಣಾಂಕ ಮುಂದಿನ[N];
	ತಾರ್ಕಿಕ vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 2, ವೆಚ್ಚ, ಸುಳ್ಳು, ಉತ್ತಮ, ಮುಂದಿನ, vis));
	EXPECT_FLOAT_EQ(ಉತ್ತಮ[2], 0.f);
	EXPECT_FALSE(ಉತ್ತಮ[0] < dijkstra::kUnreachable);
	EXPECT_FALSE(ಉತ್ತಮ[1] < dijkstra::kUnreachable);
	EXPECT_EQ(ಮುಂದಿನ[0], -1);
	EXPECT_EQ(ಮುಂದಿನ[1], -1);
}

TEST(DijkstraTest, Na_NTreated_As_Missing_ಅಂಚು)
{
	constexpr ಪೂರ್ಣಾಂಕ N = 3;
	ದಶಮಾಂಶ ವೆಚ್ಚ[N * N];
	make_ಮಾತೃಕೆ<N>(ವೆಚ್ಚ, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{0, 2, NAN}, // NaN must be treated like INFINITY, not as a 0 / negative edge
	});

	ದಶಮಾಂಶ ಉತ್ತಮ[N];
	ಪೂರ್ಣಾಂಕ ಮುಂದಿನ[N];
	ತಾರ್ಕಿಕ vis[N];

	ASSERT_TRUE(dijkstra::solveBackward(N, 2, ವೆಚ್ಚ, ಸುಳ್ಳು, ಉತ್ತಮ, ಮುಂದಿನ, vis));
	EXPECT_FLOAT_EQ(ಉತ್ತಮ[0], 2.f);
	EXPECT_EQ(ಮುಂದಿನ[0], 1);
}

TEST(DijkstraTest, RejectsInvalidInputs)
{
	constexpr ಪೂರ್ಣಾಂಕ N = 2;
	ದಶಮಾಂಶ ವೆಚ್ಚ[N * N] = { INFINITY, 1.f, 1.f, INFINITY };
	ದಶಮಾಂಶ ಉತ್ತಮ[N];
	ಪೂರ್ಣಾಂಕ ಮುಂದಿನ[N];
	ತಾರ್ಕಿಕ vis[N];

	EXPECT_FALSE(dijkstra::solveBackward(0, 0, ವೆಚ್ಚ, ಸುಳ್ಳು, ಉತ್ತಮ, ಮುಂದಿನ, vis));
	EXPECT_FALSE(dijkstra::solveBackward(N, -1, ವೆಚ್ಚ, ಸುಳ್ಳು, ಉತ್ತಮ, ಮುಂದಿನ, vis));
	EXPECT_FALSE(dijkstra::solveBackward(N, N, ವೆಚ್ಚ, ಸುಳ್ಳು, ಉತ್ತಮ, ಮುಂದಿನ, vis));
	EXPECT_FALSE(dijkstra::solveBackward(N, 0, ಶೂನ್ಯಸೂಚಕ, ಸುಳ್ಳು, ಉತ್ತಮ, ಮುಂದಿನ, vis));
	EXPECT_FALSE(dijkstra::solveBackward(N, 0, ವೆಚ್ಚ, ಸುಳ್ಳು, ಶೂನ್ಯಸೂಚಕ, ಮುಂದಿನ, vis));
}

TEST(DijkstraTest, ಸಮಮಿತ_Packed_Matches_Asymmetric)
{
	// Same undirected weighted graph expressed two ways: full N*N matrix with both
	// halves filled, and packed upper triangle. Results must match for both layouts.
	constexpr ಪೂರ್ಣಾಂಕ N = 5;

	// Edges (a, b, w) with a < b.
	ರಚನೆ Undir_ಅಂಚು { ಪೂರ್ಣಾಂಕ a, b; ದಶಮಾಂಶ w; };
	ಸ್ಥಿರಾಂಕ Undir_ಅಂಚು ಅಂಚುಗಳು[] = {
		{0, 1, 2.f},
		{0, 2, 4.f},
		{1, 2, 1.f},
		{1, 3, 7.f},
		{2, 3, 3.f},
		{3, 4, 2.f},
	};

	ದಶಮಾಂಶ ತುಂಬಿದ[N * N];

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < N * N; ++i) { ತುಂಬಿದ[i] = INFINITY; }

	constexpr ಪೂರ್ಣಾಂಕ kPacked = N * (N - 1) / 2;
	ದಶಮಾಂಶ packed[kPacked];

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < kPacked; ++i) { packed[i] = INFINITY; }

	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &e : ಅಂಚುಗಳು) {
		ತುಂಬಿದ[e.a * N + e.b] = e.w;
		ತುಂಬಿದ[e.b * N + e.a] = e.w;
		packed[e.a * (2 * N - e.a - 1) / 2 + (e.b - e.a - 1)] = e.w;
	}

	ದಶಮಾಂಶ ಉತ್ತಮ_ತುಂಬಿದ[N], ಉತ್ತಮ_pack[N];
	ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_ತುಂಬಿದ[N], ಮುಂದಿನ_pack[N];
	ತಾರ್ಕಿಕ vis[N];

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ goal = 0; goal < N; ++goal) {
		ASSERT_TRUE(dijkstra::solveBackward(N, goal, ತುಂಬಿದ, ಸುಳ್ಳು, ಉತ್ತಮ_ತುಂಬಿದ, ಮುಂದಿನ_ತುಂಬಿದ, vis));
		ASSERT_TRUE(dijkstra::solveBackward(N, goal, packed, ನಿಜ, ಉತ್ತಮ_pack, ಮುಂದಿನ_pack, vis));

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < N; ++i) {
			EXPECT_FLOAT_EQ(ಉತ್ತಮ_ತುಂಬಿದ[i], ಉತ್ತಮ_pack[i]) << "goal=" << goal << " i=" << i;
			EXPECT_EQ(ಮುಂದಿನ_ತುಂಬಿದ[i], ಮುಂದಿನ_pack[i]) << "goal=" << goal << " i=" << i;
		}
	}
}

TEST(DijkstraTest, ForwardWalkReachesGoal)
{
	// 5-node graph: 0 -> 1 -> 2 -> 3 -> 4, plus a long detour 0 -> 4 directly.
	// Solving once with goal = 4, then for any start we should reach 4 by following
	// next_node without re-solving.
	constexpr ಪೂರ್ಣಾಂಕ N = 5;
	ದಶಮಾಂಶ ವೆಚ್ಚ[N * N];
	make_ಮಾತೃಕೆ<N>(ವೆಚ್ಚ, {
		{0, 1, 1.f},
		{1, 2, 1.f},
		{2, 3, 1.f},
		{3, 4, 1.f},
		{0, 4, 100.f}, // direct but expensive
	});

	ದಶಮಾಂಶ ಉತ್ತಮ[N];
	ಪೂರ್ಣಾಂಕ ಮುಂದಿನ[N];
	ತಾರ್ಕಿಕ vis[N];
	ASSERT_TRUE(dijkstra::solveBackward(N, 4, ವೆಚ್ಚ, ಸುಳ್ಳು, ಉತ್ತಮ, ಮುಂದಿನ, vis));

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ಆರಂಭ = 0; ಆರಂಭ < N - 1; ++ಆರಂಭ) {
		ಪೂರ್ಣಾಂಕ u = ಆರಂಭ;
		ಪೂರ್ಣಾಂಕ hops = 0;

		ಆಗ (u != 4 && hops < N) {
			ASSERT_GE(ಮುಂದಿನ[u], 0) << "stuck at " << u << " from start " << ಆರಂಭ;
			u = ಮುಂದಿನ[u];
			++hops;
		}

		EXPECT_EQ(u, 4) << "did not reach goal from " << ಆರಂಭ;
	}
}

TEST(DijkstraTest, ಸಮಮಿತ_Pair_ಸೂಚ್ಯಂಕ)
{
	// create a NxN matrix and fill with random data. Then pack the upper triangle into a 1D array.
	// Then loop through the upper triangle and read the values from the array using  symmetricPairIndex
	// and verify that the values match.
	constexpr size_t N = 21;
	constexpr size_t k_ಜೋಡಿಗಳು = N * (N - 1) / 2;

	ದಶಮಾಂಶ ಮಾತೃಕೆ[N][N];
	ದಶಮಾಂಶ packed[k_ಜೋಡಿಗಳು];
	std::srand(42);
	size_t idx = 0;

	ಗಾಗಿ (size_t i = 0; i < N; ++i) {
		ಗಾಗಿ (size_t j = i + 1; j < N; ++j) {
			ಮಾತೃಕೆ[i][j] = static_cast<ದಶಮಾಂಶ>(std::rand()) / static_cast<ದಶಮಾಂಶ>(RAND_MAX);
			ಮಾತೃಕೆ[j][i] = ಮಾತೃಕೆ[i][j]; // symmetric matrix
			packed[idx++] = ಮಾತೃಕೆ[i][j];
		}
	}

	size_t counter = 0;

	ಗಾಗಿ (size_t i = 0; i < N; ++i) {
		ಗಾಗಿ (size_t j = i + 1; j < N; ++j) {
			counter++;
			EXPECT_FLOAT_EQ(ಮಾತೃಕೆ[i][j], packed[dijkstra::ಸಮಮಿತ_Pair_ಸೂಚ್ಯಂಕ_2(i, j, N)]);
			EXPECT_FLOAT_EQ(ಮಾತೃಕೆ[i][j], packed[dijkstra::ಸಮಮಿತ_Pair_ಸೂಚ್ಯಂಕ_2(j, i, N)]);
		}
	}

	// verify that we have the expected number of pairs
	EXPECT_EQ(counter, k_ಜೋಡಿಗಳು);
}
