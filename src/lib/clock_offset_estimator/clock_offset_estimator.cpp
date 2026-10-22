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

#ضمن "clock_offset_estimator.hpp"

hrt_abstime Clock_دننه_واټن_Estimator::منزل_Local(uint64_t remote_us, hrt_abstime local_us)
{
	إذا (remote_us == 0) {
		أرجع 0;
	}

	ثابت int64_t lag = static_cast<int64_t>(local_us) - static_cast<int64_t>(remote_us);

	إذا (!_initialized || lag > link_دننه_واټن_us + k_Reset_پوله_Us) {
		// Bootstrap, or remote-clock discontinuity.
		link_دننه_واټن_us = lag;
		_initialized = ریښتیا;

	} وإلا {
		// Drift the floor up at the worst-case relative clock rate so we don't
		// pin below the true offset when HRT runs faster than the remote.
		ثابت hrt_abstime dt = (local_us > وروستی_update_us) ? (local_us - وروستی_update_us) : 0;
		link_دننه_واټن_us += static_cast<int64_t>(dt) * kMaxClockDriftPpm / 1'000'000;

		إذا (lag < link_دننه_واټن_us) {
			link_دننه_واټن_us = lag;
		}
	}

	وروستی_update_us = local_us;

	ثابت int64_t پایله = static_cast<int64_t>(remote_us) + link_دننه_واټن_us;

	// Mapped HRT cannot lie after receipt.
	إذا (پایله <= 0 || پایله > static_cast<int64_t>(local_us)) {
		أرجع 0;
	}

	أرجع static_cast<hrt_abstime>(پایله);
}
