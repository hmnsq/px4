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

#ಅನ್ತರ್ಭಾವ "clock_offset_estimator.hpp"

// The estimator recovers the FC HRT of a remote event from two observables per
// message: the remote send timestamp and the local HRT at receipt. The model
// used throughout these tests is
//
//     local_hrt = (remote_ts + kClockOffset) + transport
//                  \_______ send instant in HRT ______/   \__ CAN latency __/
//
// where kClockOffset is the (constant) epoch difference between FC HRT and the
// remote clock, and transport >= 0 is the one-way bus latency (jittery). The
// estimator tracks the *minimum* observed (local - remote), so toLocal() returns
// the send instant in HRT, biased late by only the minimum transport seen.

ನಾಮಾಕಾಶ
{
constexpr int64_t  k_Clock_ಸ್ಥಾನಾಂತರ = 4'000'000;  // HRT - remote clock [us]
constexpr uint64_t k_ಆರಂಭ_Remote = 1'000'000;  // first remote send time [us]
constexpr uint64_t kPeriod      = 100'000;    // 10 Hz fix cadence [us]

// HRT at which a message sent at remote_us with the given transport arrives.
constexpr uint64_t ಆಗಮನ_hrt(uint64_t remote_us, uint64_t transport_us)
{
	ಹಿಂತಿರುಗಿ static_cast<uint64_t>(static_cast<int64_t>(remote_us) + k_Clock_ಸ್ಥಾನಾಂತರ) + transport_us;
}

// HRT of the send instant itself (what toLocal() should converge toward).
constexpr uint64_t send_hrt(uint64_t remote_us)
{
	ಹಿಂತಿರುಗಿ static_cast<uint64_t>(static_cast<int64_t>(remote_us) + k_Clock_ಸ್ಥಾನಾಂತರ);
}
} // namespace

TEST(Clock_ಸ್ಥಾನಾಂತರ_Estimator, ReturnsZeroWhenRemoteTimestampUnknown)
{
	Clock_ಸ್ಥಾನಾಂತರ_Estimator estimator;

	// A node that leaves its send timestamp unset (0) cannot be mapped.
	EXPECT_EQ(estimator.ಗಮ್ಯ_Local(0, 5'000'000), 0u);

	// The rejected sample must not corrupt state: a real sample still bootstraps.
	EXPECT_EQ(estimator.ಗಮ್ಯ_Local(k_ಆರಂಭ_Remote, ಆಗಮನ_hrt(k_ಆರಂಭ_Remote, 3'000)),
		  ಆಗಮನ_hrt(k_ಆರಂಭ_Remote, 3'000));
}

TEST(Clock_ಸ್ಥಾನಾಂತರ_Estimator, ಮೊದಲ_Sample_Maps_ಗಮ್ಯ_Receipt_ಸಮಯ)
{
	Clock_ಸ್ಥಾನಾಂತರ_Estimator estimator;

	// With a single observation there is no transport floor to subtract yet, so
	// the best the estimator can do is anchor the remote epoch to receipt time.
	ಸ್ಥಿರಾಂಕ uint64_t local = ಆಗಮನ_hrt(k_ಆರಂಭ_Remote, 12'000);
	EXPECT_EQ(estimator.ಗಮ್ಯ_Local(k_ಆರಂಭ_Remote, local), local);
}

TEST(Clock_ಸ್ಥಾನಾಂತರ_Estimator, Recovers_Send_Instant_ಮೇಲೆ_ಗಮ್ಯ_Constant_Transport)
{
	Clock_ಸ್ಥಾನಾಂತರ_Estimator estimator;

	// With a constant transport the residual bias is exactly that transport from
	// the very first sample, and the mapping is stable across the whole stream.
	constexpr uint64_t kTransport = 5'000;

	ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ k = 0; k < 5; ++k) {
		ಸ್ಥಿರಾಂಕ uint64_t remote = k_ಆರಂಭ_Remote + k * kPeriod;
		ಸ್ಥಿರಾಂಕ uint64_t local  = ಆಗಮನ_hrt(remote, kTransport);

		EXPECT_EQ(estimator.ಗಮ್ಯ_Local(remote, local), send_hrt(remote) + kTransport);
	}
}

TEST(Clock_ಸ್ಥಾನಾಂತರ_Estimator, Converges_ಗಮ್ಯ_Min_Transport_Within_Two_ಚಕ್ರಗಳು)
{
	Clock_ಸ್ಥಾನಾಂತರ_Estimator estimator;

	// Per-message transport: the first sample is a bad (late) one, so the
	// bootstrap over-estimates the floor. The true floor (3 ms) appears by the
	// third sample and must then survive the 40 ms jitter spike at index 3.
	constexpr uint64_t kTransport[]   = {50'000, 8'000, 3'000, 40'000, 3'500, 3'000};
	constexpr uint64_t kMinTransport  = 3'000;
	// Between floor observations the leaky-min drifts up at kMaxClockDriftPpm
	// (100 ppm); over a handful of 10 Hz cycles that is a few tens of us.
	constexpr uint64_t kLeakSlack     = 100;

	ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ k = 0; k < 6; ++k) {
		ಸ್ಥಿರಾಂಕ uint64_t remote = k_ಆರಂಭ_Remote + k * kPeriod;
		ಸ್ಥಿರಾಂಕ uint64_t local  = ಆಗಮನ_hrt(remote, kTransport[k]);

		ಸ್ಥಿರಾಂಕ hrt_abstime mapped = estimator.ಗಮ್ಯ_Local(remote, local);

		// Causal at all times: at/after the nav-send, at/before receipt.
		EXPECT_GE(mapped, send_hrt(remote));
		EXPECT_LE(mapped, local);

		ಆದರೆ (k == 0) {
			// Bootstrap: still 50 ms off the true send instant.
			EXPECT_EQ(mapped - send_hrt(remote), kTransport[0]);

		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (k >= 2) {
			// Converged (and held): the residual bias has collapsed from 50 ms
			// to the minimum observed transport, and the 40 ms spike at k == 3
			// does not pull the estimate back up.
			EXPECT_GE(mapped, send_hrt(remote) + kMinTransport);
			EXPECT_LE(mapped, send_hrt(remote) + kMinTransport + kLeakSlack);
		}
	}
}

TEST(Clock_ಸ್ಥಾನಾಂತರ_Estimator, ResetsOnRemoteClockDiscontinuity)
{
	Clock_ಸ್ಥಾನಾಂತರ_Estimator estimator;

	constexpr uint64_t kTransport = 3'000;

	// Converge on a steady stream.
	uint64_t remote = k_ಆರಂಭ_Remote;

	ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ k = 0; k < 4; ++k) {
		remote = k_ಆರಂಭ_Remote + k * kPeriod;
		EXPECT_EQ(estimator.ಗಮ್ಯ_Local(remote, ಆಗಮನ_hrt(remote, kTransport)),
			  send_hrt(remote) + kTransport);
	}

	// The node clock steps back 1 s (> kResetThresholdUs) while HRT keeps
	// running: the next send timestamp is 1 s behind the established chain. The
	// resulting lag jump must re-bootstrap rather than be tracked as a new floor.
	ಸ್ಥಿರಾಂಕ uint64_t hrt_now     = ಆಗಮನ_hrt(remote + kPeriod, kTransport); // continuous HRT
	ಸ್ಥಿರಾಂಕ uint64_t remote_jump = (remote + kPeriod) - 1'000'000;            // node UTC steps back 1 s

	EXPECT_EQ(estimator.ಗಮ್ಯ_Local(remote_jump, hrt_now), hrt_now); // re-bootstrapped to receipt

	// After the step the FC-HRT-to-node offset is 1 s larger; the estimator
	// reconverges on the new baseline and stays causal.
	ಸ್ಥಿರಾಂಕ uint64_t remote_ಮುಂದಿನ = remote_jump + kPeriod;
	ಸ್ಥಿರಾಂಕ uint64_t hrt_ಮುಂದಿನ    = hrt_now + kPeriod;
	ಸ್ಥಿರಾಂಕ hrt_abstime mapped   = estimator.ಗಮ್ಯ_Local(remote_ಮುಂದಿನ, hrt_ಮುಂದಿನ);

	EXPECT_LE(mapped, hrt_ಮುಂದಿನ);
	EXPECT_EQ(mapped, hrt_ಮುಂದಿನ); // constant transport again -> maps to receipt
}
