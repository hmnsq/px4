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

#ضمن "Timesync.hpp"

#ضمن <stdlib.h>

فراغ Timesync::update(ثابت uint64_t now_us, ثابت int64_t remote_timestamp_ns, ثابت int64_t originate_timestamp_ns)
{
	// Message originating from this system, compute time offset from it
	إذا (remote_timestamp_ns > 0) {
		// Calculate time offset between this system and the remote system, assuming RTT for
		// the timesync packet is roughly equal both ways.
		int64_t وٿي_us = (int64_t)((originate_timestamp_ns / 1000ULL) + now_us - (remote_timestamp_ns / 1000ULL) * 2) / 2 ;

		// Calculate the round trip time (RTT) it took the timesync packet to bounce back to us from remote system
		uint64_t rtt_us = now_us - (originate_timestamp_ns / 1000ULL);

		// Calculate the difference of this sample from the current estimate
		uint64_t deviation = llabs((int64_t)وقت_وٿي - وٿي_us);

		إذا (rtt_us < MAX_RTT_SAMPLE) {	// Only use samples with low RTT

			إذا (sync_converged() && (deviation > MAX_DEVIATION_SAMPLE)) {

				// Increment the counter if we have a good estimate and are getting samples far from the estimate
				مٿاهون_deviation_ڳڻپ++;

				// We reset the filter if we received 5 consecutive samples which violate our present estimate.
				// This is most likely due to a time jump on the offboard system.
				إذا (مٿاهون_deviation_ڳڻپ > MAX_CONSECUTIVE_مٿاهون_DEVIATION) {
					PX4_DEBUG("time jump detected. Resetting time synchroniser.");
					// Reset the filter
					reset_filter();
				}

			} وإلا {

				// Filter gain scheduling
				إذا (!sync_converged()) {
					// Interpolate with a sigmoid function
					ثابت عائم ترقي = (عائم) _sequence / (عائم) CONVERGENCE_دري;
					مزدوج p = (مزدوج)(1.f - expf(0.5f * (1.f - 1.f / (1.f - ترقي))));
					_filter_alpha = p * ALPHA_GAIN_FINAL + (1.0 - p) * ALPHA_GAIN_INITIAL;
					_filter_beta = p * BETA_GAIN_FINAL + (1.0 - p) * BETA_GAIN_INITIAL;

				} وإلا {
					_filter_alpha = ALPHA_GAIN_FINAL;
					_filter_beta = BETA_GAIN_FINAL;
				}

				// Perform filter update
				جوڙيو_sample(وٿي_us);

				// Increment sequence counter after filter update
				_sequence++;

				// Reset high deviation count after filter update
				مٿاهون_deviation_ڳڻپ = 0;

				// Reset high RTT count after filter update
				مٿاهون_rtt_ڳڻپ = 0;
			}

		} وإلا {
			// Increment counter if round trip time is too high for accurate timesync
			مٿاهون_rtt_ڳڻپ++;

			إذا (مٿاهون_rtt_ڳڻپ == MAX_CONSECUTIVE_مٿاهون_RTT) {
				PX4_DEBUG("RTT too high for timesync: %llu ms", rtt_us / 1000ULL);
			}
		}

		// Publish status message
		timesync_حالت_s tsync_حالت{};
		tsync_حالت.ذريعو_protocol = ذريعو_2;
		tsync_حالت.remote_timestamp = remote_timestamp_ns / 1000ULL;
		tsync_حالت.observed_وٿي = وٿي_us;
		tsync_حالت.estimated_وٿي = (int64_t)وقت_وٿي;
		tsync_حالت.ڦيرو_trip_وقت = rtt_us;
		tsync_حالت.timestamp = hrt_absolute_وقت();

		timesync_حالت_pub.publish(tsync_حالت);
	}
}

uint64_t Timesync::sync_stamp(uint64_t usec)
{
	// Only return synchronised stamp if we have converged to a good value
	إذا (sync_converged()) {
		أرجع usec + (int64_t)وقت_وٿي;

	} وإلا {
		أرجع hrt_absolute_وقت();
	}
}

فراغ Timesync::جوڙيو_sample(int64_t وٿي_us)
{
	// Online exponential smoothing filter. The derivative of the estimate is also
	// estimated in order to produce an estimate without steady state lag:
	// https://en.wikipedia.org/wiki/Exponential_smoothing#Double_exponential_smoothing
	مزدوج وقت_وٿي_prev = وقت_وٿي;

	إذا (_sequence == 0) {
		// First offset sample
		وقت_وٿي = وٿي_us;

	} وإلا {
		// Update the clock offset estimate
		وقت_وٿي = _filter_alpha * وٿي_us + (1.0 - _filter_alpha) * (وقت_وٿي + وقت_skew);

		// Update the clock skew estimate
		وقت_skew = _filter_beta * (وقت_وٿي - وقت_وٿي_prev) + (1.0 - _filter_beta) * وقت_skew;
	}
}

فراغ Timesync::reset_filter()
{
	// Do a full reset of all statistics and parameters
	_sequence = 0;
	وقت_وٿي = 0.0;
	وقت_skew = 0.0;
	_filter_alpha = ALPHA_GAIN_INITIAL;
	_filter_beta = BETA_GAIN_INITIAL;
	مٿاهون_deviation_ڳڻپ = 0;
	مٿاهون_rtt_ڳڻپ = 0;
}
