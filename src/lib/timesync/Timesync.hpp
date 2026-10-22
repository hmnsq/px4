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

#pragma once

#ಅನ್ತರ್ಭಾವ <drivers/drv_hrt.h>

#ಅನ್ತರ್ಭಾವ <uORB/PublicationMulti.hpp>
#ಅನ್ತರ್ಭಾವ <uORB/topics/timesync_status.h>

#ಅನ್ತರ್ಭಾವ <math.h>
#ಅನ್ತರ್ಭಾವ <float.h>

using ನಾಮಾಕಾಶ time_literals;

ಸ್ಥಿರ constexpr time_t PX4_EPOCH_SECS = 1234567890ULL;

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
ಸ್ಥಿರ constexpr ದ್ವಿಗುಣ ALPHA_GAIN_INITIAL = 0.05;
ಸ್ಥಿರ constexpr ದ್ವಿಗುಣ BETA_GAIN_INITIAL = 0.05;
ಸ್ಥಿರ constexpr ದ್ವಿಗುಣ ALPHA_GAIN_FINAL = 0.003;
ಸ್ಥಿರ constexpr ದ್ವಿಗುಣ BETA_GAIN_FINAL = 0.003;

// Filter gain scheduling
//
// The filter interpolates between the INITIAL and FINAL gains while the number of
// exhanged timesync packets is less than CONVERGENCE_WINDOW. A lower value will
// allow the timesync to converge faster, but with potentially less accurate initial
// offset and skew estimates.
ಸ್ಥಿರ constexpr uint32_t CONVERGENCE_WINDOW = 500;

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
ಸ್ಥಿರ constexpr uint64_t MAX_RTT_SAMPLE = 10_ms;
ಸ್ಥಿರ constexpr uint64_t MAX_DEVIATION_SAMPLE = 100_ms;
ಸ್ಥಿರ constexpr uint32_t MAX_CONSECUTIVE_ಮೇಲಿನ_RTT = 10;
ಸ್ಥಿರ constexpr uint32_t MAX_CONSECUTIVE_ಮೇಲಿನ_DEVIATION = 10;

ವರ್ಗ Timesync
{
ಸಾರ್ವಜನಿಕ:
	Timesync(uint8_t source = timesync_status_s::SOURCE_PROTOCOL_UNKNOWN) : _source(source) {};
	~Timesync() = ಸ್ವಭಾವ;

	ಶೂನ್ಯ update(ಧ್ರುವ uint64_t now_us, ಧ್ರುವ int64_t remote_timestamp_ns, int64_t originate_timestamp_ns);

	/**
	 * Convert remote timestamp to local hrt time (usec)
	 * Use synchronised time if available, monotonic boot time otherwise
	 */
	uint64_t sync_stamp(uint64_t usec);

	int64_t offset() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ (int64_t)_time_offset; }

	/**
	 * Return true if the timesync algorithm converged to a good estimate,
	 * return false otherwise
	 */
	ತಾರ್ಕಿಕ sync_converged() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _sequence >= CONVERGENCE_WINDOW; }

	/**
	 * Reset the exponential filter and its states
	 */
	ಶೂನ್ಯ reset_filter();


ಗುಪ್ತ:

	/**
	 * Online exponential filter to smooth time offset
	 */
	ಶೂನ್ಯ add_sample(int64_t offset_us);
	uORB::PublicationMulti<timesync_status_s>  _timesync_status_pub{ORB_ID(timesync_status)};

	uint32_t _sequence{0};

	// Timesync statistics
	ದ್ವಿಗುಣ _time_offset{0};
	ದ್ವಿಗುಣ _time_skew{0};

	// Filter parameters
	ದ್ವಿಗುಣ _filter_alpha{ALPHA_GAIN_INITIAL};
	ದ್ವಿಗುಣ _filter_beta{BETA_GAIN_INITIAL};

	// Outlier rejection and filter reset
	uint32_t ಮೇಲಿನ_deviation_ಎಣಿಕೆ{0};
	uint32_t ಮೇಲಿನ_rtt_ಎಣಿಕೆ{0};

	uint8_t _source{};
};
