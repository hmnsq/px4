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
 * @file Timesync.hpp
 * time synchroniser definition.
 *
 * @author Mohammed Kabir <mhkabir98@gmail.com>
 */

#निर्देशः once

#अन्तर्भाव <drivers/drv_hrt.h>

#अन्तर्भाव <uORB/PublicationMulti.hpp>
#अन्तर्भाव <uORB/topics/timesync_status.h>

#अन्तर्भाव <math.h>
#अन्तर्भाव <float.h>

उपयुज्य नामाकाश time_literals;

स्थिर constexpr time_t PX4_EPOCH_SECS = 1234567890ULL;

// Filter gains
//
// Alpha : Used to smooth the overall clock offset estimate. Smaller values will lead
// to a smoother estimate, but track time drift more slowly, introducing a bias
// in the estimate. Larger values will cause low-amplitude oscillations.
//
// Beta : Used to smooth the clock skew estimate. Smaller values will lead to a
// tighter estimation of the skew (derivative), but will negatively affect how fast the
// filter reacts to clock skewing (e.g cause by temperature changes to the oscillator).
// Larger values will cause large-amplitude oscillations.
स्थिर constexpr द्विगुण ALPHA_GAIN_INITIAL = 0.05;
स्थिर constexpr द्विगुण BETA_GAIN_INITIAL = 0.05;
स्थिर constexpr द्विगुण ALPHA_GAIN_FINAL = 0.003;
स्थिर constexpr द्विगुण BETA_GAIN_FINAL = 0.003;

// Filter gain scheduling
//
// The filter interpolates between the INITIAL and FINAL gains while the number of
// exhanged timesync packets is less than CONVERGENCE_WINDOW. A lower value will
// allow the timesync to converge faster, but with potentially less accurate initial
// offset and skew estimates.
स्थिर constexpr uint32_t CONVERGENCE_गवाक्षः = 500;

// Outlier rejection and filter reset
//
// Samples with round-trip time higher than MAX_RTT_SAMPLE are not used to update the filter.
// More than MAX_CONSECUTIVE_HIGH_RTT number of such events in a row will throw a debug message
// but not reset the filter.
// Samples whose calculated clock offset is more than MAX_DEVIATION_SAMPLE off from the current
// estimate are not used to update the filter. More than MAX_CONSECUTIVE_HIGH_DEVIATION number
// of such events in a row will reset the filter. This usually happens only due to a time jump
// on the remote system.
// TODO : automatically determine these using ping statistics?
स्थिर constexpr uint64_t MAX_RTT_SAMPLE = 10_ms;
स्थिर constexpr uint64_t MAX_DEVIATION_SAMPLE = 100_ms;
स्थिर constexpr uint32_t MAX_CONSECUTIVE_उच्चः_RTT = 10;
स्थिर constexpr uint32_t MAX_CONSECUTIVE_उच्चः_DEVIATION = 10;

वर्ग Timesync
{
सार्वजनिक:
	Timesync(uint8_t source = timesync_स्थितिः_s::SOURCE_PROTOCOL_UNKNOWN) : _source(source) {};
	~Timesync() = स्वभाव;

	शून्य update(ध्रुव uint64_t now_us, ध्रुव int64_t remote_timestamp_ns, int64_t originate_timestamp_ns);

	/**
	 * Convert remote timestamp to local hrt time (usec)
	 * Use synchronised time if available, monotonic boot time otherwise
	 */
	uint64_t sync_stamp(uint64_t usec);

	int64_t विस्थापनम्() ध्रुव { प्रत्यागम (int64_t)time_विस्थापनम्; }

	/**
	 * Return true if the timesync algorithm converged to a good estimate,
	 * return false otherwise
	 */
	तार्किक sync_converged() ध्रुव { प्रत्यागम _sequence >= CONVERGENCE_गवाक्षः; }

	/**
	 * Reset the exponential filter and its states
	 */
	शून्य reset_filter();


गुप्त:

	/**
	 * Online exponential filter to smooth time offset
	 */
	शून्य सङ्कलय_sample(int64_t विस्थापनम्_us);
	uORB::PublicationMulti<timesync_स्थितिः_s>  timesync_स्थितिः_pub{ORB_ID(timesync_स्थितिः)};

	uint32_t _sequence{0};

	// Timesync statistics
	द्विगुण time_विस्थापनम्{0};
	द्विगुण _time_skew{0};

	// Filter parameters
	द्विगुण _filter_alpha{ALPHA_GAIN_INITIAL};
	द्विगुण _filter_beta{BETA_GAIN_INITIAL};

	// Outlier rejection and filter reset
	uint32_t उच्चः_deviation_गणनम्{0};
	uint32_t उच्चः_rtt_गणनम्{0};

	uint8_t _source{};
};
