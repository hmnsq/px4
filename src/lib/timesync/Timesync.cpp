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

#ਅਨ੍ਤਰ੍ਭਾਵ "Timesync.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>

ਖਾਲੀ Timesync::update(ਅਚਲ uint64_t now_us, ਅਚਲ int64_t remote_timestamp_ns, ਅਚਲ int64_t originate_timestamp_ns)
{
	// Message originating from this system, compute time offset from it
	ਜੇ (remote_timestamp_ns > 0) {
		// Calculate time offset between this system and the remote system, assuming RTT for
		// the timesync packet is roughly equal both ways.
		int64_t ਵਿੱਥ_us = (int64_t)((originate_timestamp_ns / 1000ULL) + now_us - (remote_timestamp_ns / 1000ULL) * 2) / 2 ;

		// Calculate the round trip time (RTT) it took the timesync packet to bounce back to us from remote system
		uint64_t rtt_us = now_us - (originate_timestamp_ns / 1000ULL);

		// Calculate the difference of this sample from the current estimate
		uint64_t deviation = llabs((int64_t)ਸਮਾਂ_ਵਿੱਥ - ਵਿੱਥ_us);

		ਜੇ (rtt_us < MAX_RTT_SAMPLE) {	// Only use samples with low RTT

			ਜੇ (sync_converged() && (deviation > MAX_DEVIATION_SAMPLE)) {

				// Increment the counter if we have a good estimate and are getting samples far from the estimate
				ਉੱਚਾ_deviation_ਗਿਣਤੀ++;

				// We reset the filter if we received 5 consecutive samples which violate our present estimate.
				// This is most likely due to a time jump on the offboard system.
				ਜੇ (ਉੱਚਾ_deviation_ਗਿਣਤੀ > MAX_CONSECUTIVE_ਉੱਚਾ_DEVIATION) {
					PX4_DEBUG("time jump detected. Resetting time synchroniser.");
					// Reset the filter
					reset_filter();
				}

			} ਨਹੀਂਤਾਂ {

				// Filter gain scheduling
				ਜੇ (!sync_converged()) {
					// Interpolate with a sigmoid function
					ਅਚਲ ਦਸ਼ਮਲਵ ਤਰੱਕੀ = (ਦਸ਼ਮਲਵ) _sequence / (ਦਸ਼ਮਲਵ) CONVERGENCE_ਖਿੜਕੀ;
					ਦੁੱਗਣਾ p = (ਦੁੱਗਣਾ)(1.f - expf(0.5f * (1.f - 1.f / (1.f - ਤਰੱਕੀ))));
					_filter_alpha = p * ALPHA_GAIN_FINAL + (1.0 - p) * ALPHA_GAIN_INITIAL;
					_filter_beta = p * BETA_GAIN_FINAL + (1.0 - p) * BETA_GAIN_INITIAL;

				} ਨਹੀਂਤਾਂ {
					_filter_alpha = ALPHA_GAIN_FINAL;
					_filter_beta = BETA_GAIN_FINAL;
				}

				// Perform filter update
				ਜੋੜੋ_sample(ਵਿੱਥ_us);

				// Increment sequence counter after filter update
				_sequence++;

				// Reset high deviation count after filter update
				ਉੱਚਾ_deviation_ਗਿਣਤੀ = 0;

				// Reset high RTT count after filter update
				ਉੱਚਾ_rtt_ਗਿਣਤੀ = 0;
			}

		} ਨਹੀਂਤਾਂ {
			// Increment counter if round trip time is too high for accurate timesync
			ਉੱਚਾ_rtt_ਗਿਣਤੀ++;

			ਜੇ (ਉੱਚਾ_rtt_ਗਿਣਤੀ == MAX_CONSECUTIVE_ਉੱਚਾ_RTT) {
				PX4_DEBUG("RTT too high for timesync: %llu ms", rtt_us / 1000ULL);
			}
		}

		// Publish status message
		timesync_ਸਥਿਤੀ_s tsync_ਸਥਿਤੀ{};
		tsync_ਸਥਿਤੀ.ਸਰੋਤ_protocol = ਸਰੋਤ_2;
		tsync_ਸਥਿਤੀ.remote_timestamp = remote_timestamp_ns / 1000ULL;
		tsync_ਸਥਿਤੀ.observed_ਵਿੱਥ = ਵਿੱਥ_us;
		tsync_ਸਥਿਤੀ.estimated_ਵਿੱਥ = (int64_t)ਸਮਾਂ_ਵਿੱਥ;
		tsync_ਸਥਿਤੀ.ਗੇੜਾ_trip_ਸਮਾਂ = rtt_us;
		tsync_ਸਥਿਤੀ.timestamp = hrt_absolute_ਸਮਾਂ();

		timesync_ਸਥਿਤੀ_pub.publish(tsync_ਸਥਿਤੀ);
	}
}

uint64_t Timesync::sync_stamp(uint64_t usec)
{
	// Only return synchronised stamp if we have converged to a good value
	ਜੇ (sync_converged()) {
		ਵਾਪਸ usec + (int64_t)ਸਮਾਂ_ਵਿੱਥ;

	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ hrt_absolute_ਸਮਾਂ();
	}
}

ਖਾਲੀ Timesync::ਜੋੜੋ_sample(int64_t ਵਿੱਥ_us)
{
	// Online exponential smoothing filter. The derivative of the estimate is also
	// estimated in order to produce an estimate without steady state lag:
	// https://en.wikipedia.org/wiki/Exponential_smoothing#Double_exponential_smoothing
	ਦੁੱਗਣਾ ਸਮਾਂ_ਵਿੱਥ_prev = ਸਮਾਂ_ਵਿੱਥ;

	ਜੇ (_sequence == 0) {
		// First offset sample
		ਸਮਾਂ_ਵਿੱਥ = ਵਿੱਥ_us;

	} ਨਹੀਂਤਾਂ {
		// Update the clock offset estimate
		ਸਮਾਂ_ਵਿੱਥ = _filter_alpha * ਵਿੱਥ_us + (1.0 - _filter_alpha) * (ਸਮਾਂ_ਵਿੱਥ + ਸਮਾਂ_skew);

		// Update the clock skew estimate
		ਸਮਾਂ_skew = _filter_beta * (ਸਮਾਂ_ਵਿੱਥ - ਸਮਾਂ_ਵਿੱਥ_prev) + (1.0 - _filter_beta) * ਸਮਾਂ_skew;
	}
}

ਖਾਲੀ Timesync::reset_filter()
{
	// Do a full reset of all statistics and parameters
	_sequence = 0;
	ਸਮਾਂ_ਵਿੱਥ = 0.0;
	ਸਮਾਂ_skew = 0.0;
	_filter_alpha = ALPHA_GAIN_INITIAL;
	_filter_beta = BETA_GAIN_INITIAL;
	ਉੱਚਾ_deviation_ਗਿਣਤੀ = 0;
	ਉੱਚਾ_rtt_ਗਿਣਤੀ = 0;
}
