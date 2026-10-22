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

#ಅನ್ತರ್ಭಾವ "clock_offset_estimator.hpp"

hrt_abstime Clock_ಸ್ಥಾನಾಂತರ_Estimator::ಗಮ್ಯ_Local(uint64_t remote_us, hrt_abstime local_us)
{
	ಆದರೆ (remote_us == 0) {
		ಹಿಂತಿರುಗಿ 0;
	}

	ಸ್ಥಿರಾಂಕ int64_t lag = static_cast<int64_t>(local_us) - static_cast<int64_t>(remote_us);

	ಆದರೆ (!_initialized || lag > link_ಸ್ಥಾನಾಂತರ_us + k_Reset_ಮಿತಿ_Us) {
		// Bootstrap, or remote-clock discontinuity.
		link_ಸ್ಥಾನಾಂತರ_us = lag;
		_initialized = ನಿಜ;

	} ಇಲ್ಲದಿದ್ದರೆ {
		// Drift the floor up at the worst-case relative clock rate so we don't
		// pin below the true offset when HRT runs faster than the remote.
		ಸ್ಥಿರಾಂಕ hrt_abstime dt = (local_us > ಕೊನೆಯ_update_us) ? (local_us - ಕೊನೆಯ_update_us) : 0;
		link_ಸ್ಥಾನಾಂತರ_us += static_cast<int64_t>(dt) * kMaxClockDriftPpm / 1'000'000;

		ಆದರೆ (lag < link_ಸ್ಥಾನಾಂತರ_us) {
			link_ಸ್ಥಾನಾಂತರ_us = lag;
		}
	}

	ಕೊನೆಯ_update_us = local_us;

	ಸ್ಥಿರಾಂಕ int64_t ಫಲಿತಾಂಶ = static_cast<int64_t>(remote_us) + link_ಸ್ಥಾನಾಂತರ_us;

	// Mapped HRT cannot lie after receipt.
	ಆದರೆ (ಫಲಿತಾಂಶ <= 0 || ಫಲಿತಾಂಶ > static_cast<int64_t>(local_us)) {
		ಹಿಂತಿರುಗಿ 0;
	}

	ಹಿಂತಿರುಗಿ static_cast<hrt_abstime>(ಫಲಿತಾಂಶ);
}
