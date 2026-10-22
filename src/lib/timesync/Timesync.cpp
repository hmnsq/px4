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

#अन्तर्भाव "Timesync.hpp"

#अन्तर्भाव <stdlib.h>

शून्य Timesync::update(ध्रुव uint64_t now_us, ध्रुव int64_t remote_timestamp_ns, ध्रुव int64_t originate_timestamp_ns)
{
	// Message originating from this system, compute time offset from it
	यदि (remote_timestamp_ns > 0) {
		// Calculate time offset between this system and the remote system, assuming RTT for
		// the timesync packet is roughly equal both ways.
		int64_t विस्थापनम्_us = (int64_t)((originate_timestamp_ns / 1000ULL) + now_us - (remote_timestamp_ns / 1000ULL) * 2) / 2 ;

		// Calculate the round trip time (RTT) it took the timesync packet to bounce back to us from remote system
		uint64_t rtt_us = now_us - (originate_timestamp_ns / 1000ULL);

		// Calculate the difference of this sample from the current estimate
		uint64_t deviation = llabs((int64_t)time_विस्थापनम् - विस्थापनम्_us);

		यदि (rtt_us < MAX_RTT_SAMPLE) {	// Only use samples with low RTT

			यदि (sync_converged() && (deviation > MAX_DEVIATION_SAMPLE)) {

				// Increment the counter if we have a good estimate and are getting samples far from the estimate
				उच्चः_deviation_गणनम्++;

				// We reset the filter if we received 5 consecutive samples which violate our present estimate.
				// This is most likely due to a time jump on the offboard system.
				यदि (उच्चः_deviation_गणनम् > MAX_CONSECUTIVE_उच्चः_DEVIATION) {
					PX4_DEBUG("time jump detected. Resetting time synchroniser.");
					// Reset the filter
					reset_filter();
				}

			} अन्यथा {

				// Filter gain scheduling
				यदि (!sync_converged()) {
					// Interpolate with a sigmoid function
					ध्रुव भिन्न प्रगतिः = (भिन्न) _sequence / (भिन्न) CONVERGENCE_गवाक्षः;
					द्विगुण p = (द्विगुण)(1.f - expf(0.5f * (1.f - 1.f / (1.f - प्रगतिः))));
					_filter_alpha = p * ALPHA_GAIN_FINAL + (1.0 - p) * ALPHA_GAIN_INITIAL;
					_filter_beta = p * BETA_GAIN_FINAL + (1.0 - p) * BETA_GAIN_INITIAL;

				} अन्यथा {
					_filter_alpha = ALPHA_GAIN_FINAL;
					_filter_beta = BETA_GAIN_FINAL;
				}

				// Perform filter update
				सङ्कलय_sample(विस्थापनम्_us);

				// Increment sequence counter after filter update
				_sequence++;

				// Reset high deviation count after filter update
				उच्चः_deviation_गणनम् = 0;

				// Reset high RTT count after filter update
				उच्चः_rtt_गणनम् = 0;
			}

		} अन्यथा {
			// Increment counter if round trip time is too high for accurate timesync
			उच्चः_rtt_गणनम्++;

			यदि (उच्चः_rtt_गणनम् == MAX_CONSECUTIVE_उच्चः_RTT) {
				PX4_DEBUG("RTT too high for timesync: %llu ms", rtt_us / 1000ULL);
			}
		}

		// Publish status message
		timesync_स्थितिः_s tsync_स्थितिः{};
		tsync_स्थितिः.source_protocol = _source;
		tsync_स्थितिः.remote_timestamp = remote_timestamp_ns / 1000ULL;
		tsync_स्थितिः.observed_विस्थापनम् = विस्थापनम्_us;
		tsync_स्थितिः.estimated_विस्थापनम् = (int64_t)time_विस्थापनम्;
		tsync_स्थितिः.आवर्तनम्_trip_time = rtt_us;
		tsync_स्थितिः.timestamp = hrt_absolute_time();

		timesync_स्थितिः_pub.publish(tsync_स्थितिः);
	}
}

uint64_t Timesync::sync_stamp(uint64_t usec)
{
	// Only return synchronised stamp if we have converged to a good value
	यदि (sync_converged()) {
		प्रत्यागम usec + (int64_t)time_विस्थापनम्;

	} अन्यथा {
		प्रत्यागम hrt_absolute_time();
	}
}

शून्य Timesync::सङ्कलय_sample(int64_t विस्थापनम्_us)
{
	// Online exponential smoothing filter. The derivative of the estimate is also
	// estimated in order to produce an estimate without steady state lag:
	// https://en.wikipedia.org/wiki/Exponential_smoothing#Double_exponential_smoothing
	द्विगुण time_विस्थापनम्_पूर्व = time_विस्थापनम्;

	यदि (_sequence == 0) {
		// First offset sample
		time_विस्थापनम् = विस्थापनम्_us;

	} अन्यथा {
		// Update the clock offset estimate
		time_विस्थापनम् = _filter_alpha * विस्थापनम्_us + (1.0 - _filter_alpha) * (time_विस्थापनम् + _time_skew);

		// Update the clock skew estimate
		_time_skew = _filter_beta * (time_विस्थापनम् - time_विस्थापनम्_पूर्व) + (1.0 - _filter_beta) * _time_skew;
	}
}

शून्य Timesync::reset_filter()
{
	// Do a full reset of all statistics and parameters
	_sequence = 0;
	time_विस्थापनम् = 0.0;
	_time_skew = 0.0;
	_filter_alpha = ALPHA_GAIN_INITIAL;
	_filter_beta = BETA_GAIN_INITIAL;
	उच्चः_deviation_गणनम् = 0;
	उच्चः_rtt_गणनम् = 0;
}
