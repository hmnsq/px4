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

#అన్తర్భావ "clock_offset_estimator.hpp"

hrt_abstime Clock_స్థానభ్రంశం_Estimator::గమ్యం_Local(uint64_t remote_us, hrt_abstime local_us)
{
	ఒకవేళ (remote_us == 0) {
		తిరిగి 0;
	}

	స్థిరరాశి int64_t lag = static_cast<int64_t>(local_us) - static_cast<int64_t>(remote_us);

	ఒకవేళ (!_initialized || lag > link_స్థానభ్రంశం_us + k_Reset_పరిమితి_Us) {
		// Bootstrap, or remote-clock discontinuity.
		link_స్థానభ్రంశం_us = lag;
		_initialized = నిజం;

	} లేకపోతే {
		// Drift the floor up at the worst-case relative clock rate so we don't
		// pin below the true offset when HRT runs faster than the remote.
		స్థిరరాశి hrt_abstime dt = (local_us > చివరి_update_us) ? (local_us - చివరి_update_us) : 0;
		link_స్థానభ్రంశం_us += static_cast<int64_t>(dt) * kMaxClockDriftPpm / 1'000'000;

		ఒకవేళ (lag < link_స్థానభ్రంశం_us) {
			link_స్థానభ్రంశం_us = lag;
		}
	}

	చివరి_update_us = local_us;

	స్థిరరాశి int64_t ఫలితం = static_cast<int64_t>(remote_us) + link_స్థానభ్రంశం_us;

	// Mapped HRT cannot lie after receipt.
	ఒకవేళ (ఫలితం <= 0 || ఫలితం > static_cast<int64_t>(local_us)) {
		తిరిగి 0;
	}

	తిరిగి static_cast<hrt_abstime>(ఫలితం);
}
