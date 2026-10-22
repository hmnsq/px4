/****************************************************************************
 *
 *   Copyright (c) 2018-2022 PX4 Development Team. All rights reserved.
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

/**
 * @file Timesync.cpp
 * timesync implementation.
 *
 * @author Mohammed Kabir <mhkabir98@gmail.com>
 */

#అన్తర్భావ "Timesync.hpp"

#అన్తర్భావ <stdlib.h>

శూన్య Timesync::update(స్థిరరాశి uint64_t now_us, స్థిరరాశి int64_t remote_timestamp_ns, స్థిరరాశి int64_t originate_timestamp_ns)
{
	// Message originating from this system, compute time offset from it
	ఒకవేళ (remote_timestamp_ns > 0) {
		// Calculate time offset between this system and the remote system, assuming RTT for
		// the timesync packet is roughly equal both ways.
		int64_t స్థానభ్రంశం_us = (int64_t)((originate_timestamp_ns / 1000ULL) + now_us - (remote_timestamp_ns / 1000ULL) * 2) / 2 ;

		// Calculate the round trip time (RTT) it took the timesync packet to bounce back to us from remote system
		uint64_t rtt_us = now_us - (originate_timestamp_ns / 1000ULL);

		// Calculate the difference of this sample from the current estimate
		uint64_t deviation = llabs((int64_t)సమయం_స్థానభ్రంశం - స్థానభ్రంశం_us);

		ఒకవేళ (rtt_us < MAX_RTT_SAMPLE) {	// Only use samples with low RTT

			ఒకవేళ (sync_converged() && (deviation > MAX_DEVIATION_SAMPLE)) {

				// Increment the counter if we have a good estimate and are getting samples far from the estimate
				ఎగువ_deviation_లెక్క++;

				// We reset the filter if we received 5 consecutive samples which violate our present estimate.
				// This is most likely due to a time jump on the offboard system.
				ఒకవేళ (ఎగువ_deviation_లెక్క > MAX_CONSECUTIVE_ఎగువ_DEVIATION) {
					PX4_DEBUG("time jump detected. Resetting time synchroniser.");
					// Reset the filter
					reset_filter();
				}

			} లేకపోతే {

				// Filter gain scheduling
				ఒకవేళ (!sync_converged()) {
					// Interpolate with a sigmoid function
					స్థిరరాశి దశాంశం పురోగతి = (దశాంశం) _sequence / (దశాంశం) CONVERGENCE_కిటికీ;
					ద్విగుణ p = (ద్విగుణ)(1.f - expf(0.5f * (1.f - 1.f / (1.f - పురోగతి))));
					_filter_alpha = p * ALPHA_GAIN_FINAL + (1.0 - p) * ALPHA_GAIN_INITIAL;
					_filter_beta = p * BETA_GAIN_FINAL + (1.0 - p) * BETA_GAIN_INITIAL;

				} లేకపోతే {
					_filter_alpha = ALPHA_GAIN_FINAL;
					_filter_beta = BETA_GAIN_FINAL;
				}

				// Perform filter update
				కూడు_sample(స్థానభ్రంశం_us);

				// Increment sequence counter after filter update
				_sequence++;

				// Reset high deviation count after filter update
				ఎగువ_deviation_లెక్క = 0;

				// Reset high RTT count after filter update
				ఎగువ_rtt_లెక్క = 0;
			}

		} లేకపోతే {
			// Increment counter if round trip time is too high for accurate timesync
			ఎగువ_rtt_లెక్క++;

			ఒకవేళ (ఎగువ_rtt_లెక్క == MAX_CONSECUTIVE_ఎగువ_RTT) {
				PX4_DEBUG("RTT too high for timesync: %llu ms", rtt_us / 1000ULL);
			}
		}

		// Publish status message
		timesync_స్థితి_s tsync_స్థితి{};
		tsync_స్థితి.ఆధారం_protocol = ఆధారం_2;
		tsync_స్థితి.remote_timestamp = remote_timestamp_ns / 1000ULL;
		tsync_స్థితి.observed_స్థానభ్రంశం = స్థానభ్రంశం_us;
		tsync_స్థితి.estimated_స్థానభ్రంశం = (int64_t)సమయం_స్థానభ్రంశం;
		tsync_స్థితి.విడత_trip_సమయం = rtt_us;
		tsync_స్థితి.timestamp = hrt_absolute_సమయం();

		timesync_స్థితి_pub.publish(tsync_స్థితి);
	}
}

uint64_t Timesync::sync_stamp(uint64_t usec)
{
	// Only return synchronised stamp if we have converged to a good value
	ఒకవేళ (sync_converged()) {
		తిరిగి usec + (int64_t)సమయం_స్థానభ్రంశం;

	} లేకపోతే {
		తిరిగి hrt_absolute_సమయం();
	}
}

శూన్య Timesync::కూడు_sample(int64_t స్థానభ్రంశం_us)
{
	// Online exponential smoothing filter. The derivative of the estimate is also
	// estimated in order to produce an estimate without steady state lag:
	// https://en.wikipedia.org/wiki/Exponential_smoothing#Double_exponential_smoothing
	ద్విగుణ సమయం_స్థానభ్రంశం_prev = సమయం_స్థానభ్రంశం;

	ఒకవేళ (_sequence == 0) {
		// First offset sample
		సమయం_స్థానభ్రంశం = స్థానభ్రంశం_us;

	} లేకపోతే {
		// Update the clock offset estimate
		సమయం_స్థానభ్రంశం = _filter_alpha * స్థానభ్రంశం_us + (1.0 - _filter_alpha) * (సమయం_స్థానభ్రంశం + సమయం_skew);

		// Update the clock skew estimate
		సమయం_skew = _filter_beta * (సమయం_స్థానభ్రంశం - సమయం_స్థానభ్రంశం_prev) + (1.0 - _filter_beta) * సమయం_skew;
	}
}

శూన్య Timesync::reset_filter()
{
	// Do a full reset of all statistics and parameters
	_sequence = 0;
	సమయం_స్థానభ్రంశం = 0.0;
	సమయం_skew = 0.0;
	_filter_alpha = ALPHA_GAIN_INITIAL;
	_filter_beta = BETA_GAIN_INITIAL;
	ఎగువ_deviation_లెక్క = 0;
	ఎగువ_rtt_లెక్క = 0;
}
