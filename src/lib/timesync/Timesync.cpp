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

#ಅನ್ತರ್ಭಾವ "Timesync.hpp"

#ಅನ್ತರ್ಭಾವ <stdlib.h>

ಶೂನ್ಯ Timesync::update(ಸ್ಥಿರಾಂಕ uint64_t now_us, ಸ್ಥಿರಾಂಕ int64_t remote_timestamp_ns, ಸ್ಥಿರಾಂಕ int64_t originate_timestamp_ns)
{
	// Message originating from this system, compute time offset from it
	ಆದರೆ (remote_timestamp_ns > 0) {
		// Calculate time offset between this system and the remote system, assuming RTT for
		// the timesync packet is roughly equal both ways.
		int64_t ಸ್ಥಾನಾಂತರ_us = (int64_t)((originate_timestamp_ns / 1000ULL) + now_us - (remote_timestamp_ns / 1000ULL) * 2) / 2 ;

		// Calculate the round trip time (RTT) it took the timesync packet to bounce back to us from remote system
		uint64_t rtt_us = now_us - (originate_timestamp_ns / 1000ULL);

		// Calculate the difference of this sample from the current estimate
		uint64_t deviation = llabs((int64_t)ಸಮಯ_ಸ್ಥಾನಾಂತರ - ಸ್ಥಾನಾಂತರ_us);

		ಆದರೆ (rtt_us < MAX_RTT_SAMPLE) {	// Only use samples with low RTT

			ಆದರೆ (sync_converged() && (deviation > MAX_DEVIATION_SAMPLE)) {

				// Increment the counter if we have a good estimate and are getting samples far from the estimate
				ಮೇಲಿನ_deviation_ಎಣಿಕೆ++;

				// We reset the filter if we received 5 consecutive samples which violate our present estimate.
				// This is most likely due to a time jump on the offboard system.
				ಆದರೆ (ಮೇಲಿನ_deviation_ಎಣಿಕೆ > MAX_CONSECUTIVE_ಮೇಲಿನ_DEVIATION) {
					PX4_DEBUG("time jump detected. Resetting time synchroniser.");
					// Reset the filter
					reset_filter();
				}

			} ಇಲ್ಲದಿದ್ದರೆ {

				// Filter gain scheduling
				ಆದರೆ (!sync_converged()) {
					// Interpolate with a sigmoid function
					ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಪ್ರಗತಿ = (ದಶಮಾಂಶ) _sequence / (ದಶಮಾಂಶ) CONVERGENCE_ಕಿಟಕಿ;
					ದ್ವಿಗುಣ p = (ದ್ವಿಗುಣ)(1.f - expf(0.5f * (1.f - 1.f / (1.f - ಪ್ರಗತಿ))));
					_filter_alpha = p * ALPHA_GAIN_FINAL + (1.0 - p) * ALPHA_GAIN_INITIAL;
					_filter_beta = p * BETA_GAIN_FINAL + (1.0 - p) * BETA_GAIN_INITIAL;

				} ಇಲ್ಲದಿದ್ದರೆ {
					_filter_alpha = ALPHA_GAIN_FINAL;
					_filter_beta = BETA_GAIN_FINAL;
				}

				// Perform filter update
				ಕೂಡಿಸು_sample(ಸ್ಥಾನಾಂತರ_us);

				// Increment sequence counter after filter update
				_sequence++;

				// Reset high deviation count after filter update
				ಮೇಲಿನ_deviation_ಎಣಿಕೆ = 0;

				// Reset high RTT count after filter update
				ಮೇಲಿನ_rtt_ಎಣಿಕೆ = 0;
			}

		} ಇಲ್ಲದಿದ್ದರೆ {
			// Increment counter if round trip time is too high for accurate timesync
			ಮೇಲಿನ_rtt_ಎಣಿಕೆ++;

			ಆದರೆ (ಮೇಲಿನ_rtt_ಎಣಿಕೆ == MAX_CONSECUTIVE_ಮೇಲಿನ_RTT) {
				PX4_DEBUG("RTT too high for timesync: %llu ms", rtt_us / 1000ULL);
			}
		}

		// Publish status message
		timesync_ಸ್ಥಿತಿ_s tsync_ಸ್ಥಿತಿ{};
		tsync_ಸ್ಥಿತಿ.ಮೂಲ_protocol = ಮೂಲ_2;
		tsync_ಸ್ಥಿತಿ.remote_timestamp = remote_timestamp_ns / 1000ULL;
		tsync_ಸ್ಥಿತಿ.observed_ಸ್ಥಾನಾಂತರ = ಸ್ಥಾನಾಂತರ_us;
		tsync_ಸ್ಥಿತಿ.estimated_ಸ್ಥಾನಾಂತರ = (int64_t)ಸಮಯ_ಸ್ಥಾನಾಂತರ;
		tsync_ಸ್ಥಿತಿ.ಸುತ್ತು_trip_ಸಮಯ = rtt_us;
		tsync_ಸ್ಥಿತಿ.timestamp = hrt_absolute_ಸಮಯ();

		timesync_ಸ್ಥಿತಿ_pub.publish(tsync_ಸ್ಥಿತಿ);
	}
}

uint64_t Timesync::sync_stamp(uint64_t usec)
{
	// Only return synchronised stamp if we have converged to a good value
	ಆದರೆ (sync_converged()) {
		ಹಿಂತಿರುಗಿ usec + (int64_t)ಸಮಯ_ಸ್ಥಾನಾಂತರ;

	} ಇಲ್ಲದಿದ್ದರೆ {
		ಹಿಂತಿರುಗಿ hrt_absolute_ಸಮಯ();
	}
}

ಶೂನ್ಯ Timesync::ಕೂಡಿಸು_sample(int64_t ಸ್ಥಾನಾಂತರ_us)
{
	// Online exponential smoothing filter. The derivative of the estimate is also
	// estimated in order to produce an estimate without steady state lag:
	// https://en.wikipedia.org/wiki/Exponential_smoothing#Double_exponential_smoothing
	ದ್ವಿಗುಣ ಸಮಯ_ಸ್ಥಾನಾಂತರ_prev = ಸಮಯ_ಸ್ಥಾನಾಂತರ;

	ಆದರೆ (_sequence == 0) {
		// First offset sample
		ಸಮಯ_ಸ್ಥಾನಾಂತರ = ಸ್ಥಾನಾಂತರ_us;

	} ಇಲ್ಲದಿದ್ದರೆ {
		// Update the clock offset estimate
		ಸಮಯ_ಸ್ಥಾನಾಂತರ = _filter_alpha * ಸ್ಥಾನಾಂತರ_us + (1.0 - _filter_alpha) * (ಸಮಯ_ಸ್ಥಾನಾಂತರ + ಸಮಯ_skew);

		// Update the clock skew estimate
		ಸಮಯ_skew = _filter_beta * (ಸಮಯ_ಸ್ಥಾನಾಂತರ - ಸಮಯ_ಸ್ಥಾನಾಂತರ_prev) + (1.0 - _filter_beta) * ಸಮಯ_skew;
	}
}

ಶೂನ್ಯ Timesync::reset_filter()
{
	// Do a full reset of all statistics and parameters
	_sequence = 0;
	ಸಮಯ_ಸ್ಥಾನಾಂತರ = 0.0;
	ಸಮಯ_skew = 0.0;
	_filter_alpha = ALPHA_GAIN_INITIAL;
	_filter_beta = BETA_GAIN_INITIAL;
	ಮೇಲಿನ_deviation_ಎಣಿಕೆ = 0;
	ಮೇಲಿನ_rtt_ಎಣಿಕೆ = 0;
}
