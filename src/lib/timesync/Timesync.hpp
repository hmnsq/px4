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

#సూచన once

#అన్తర్భావ <drivers/drv_hrt.h>

#అన్తర్భావ <uORB/PublicationMulti.hpp>
#అన్తర్భావ <uORB/topics/timesync_status.h>

#అన్తర్భావ <math.h>
#అన్తర్భావ <float.h>

ఉపయోగించి నామాకాశ సమయం_literals;

స్థిర constexpr time_t PX4_EPOCH_SECS = 1234567890ULL;

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
స్థిర constexpr ద్విగుణ ALPHA_GAIN_INITIAL = 0.05;
స్థిర constexpr ద్విగుణ BETA_GAIN_INITIAL = 0.05;
స్థిర constexpr ద్విగుణ ALPHA_GAIN_FINAL = 0.003;
స్థిర constexpr ద్విగుణ BETA_GAIN_FINAL = 0.003;

// Filter gain scheduling
//
// The filter interpolates between the INITIAL and FINAL gains while the number of
// exhanged timesync packets is less than CONVERGENCE_WINDOW. A lower value will
// allow the timesync to converge faster, but with potentially less accurate initial
// offset and skew estimates.
స్థిర constexpr uint32_t CONVERGENCE_కిటికీ = 500;

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
స్థిర constexpr uint64_t MAX_RTT_SAMPLE = 10_ms;
స్థిర constexpr uint64_t MAX_DEVIATION_SAMPLE = 100_ms;
స్థిర constexpr uint32_t MAX_CONSECUTIVE_ఎగువ_RTT = 10;
స్థిర constexpr uint32_t MAX_CONSECUTIVE_ఎగువ_DEVIATION = 10;

తరగతి Timesync
{
బహిరంగ:
	Timesync(uint8_t ఆధారం = timesync_స్థితి_s::ఆధారం_PROTOCOL_UNKNOWN) : ఆధారం_2(ఆధారం) {};
	~Timesync() = అప్రమేయ;

	శూన్య update(స్థిరరాశి uint64_t now_us, స్థిరరాశి int64_t remote_timestamp_ns, int64_t originate_timestamp_ns);

	/**
	 * Convert remote timestamp to local hrt time (usec)
	 * Use synchronised time if available, monotonic boot time otherwise
	 */
	uint64_t sync_stamp(uint64_t usec);

	int64_t స్థానభ్రంశం() స్థిరరాశి { తిరిగి (int64_t)సమయం_స్థానభ్రంశం; }

	/**
	 * Return true if the timesync algorithm converged to a good estimate,
	 * return false otherwise
	 */
	తార్కిక sync_converged() స్థిరరాశి { తిరిగి _sequence >= CONVERGENCE_కిటికీ; }

	/**
	 * Reset the exponential filter and its states
	 */
	శూన్య reset_filter();


అంతరంగ:

	/**
	 * Online exponential filter to smooth time offset
	 */
	శూన్య కూడు_sample(int64_t స్థానభ్రంశం_us);
	uORB::PublicationMulti<timesync_స్థితి_s>  timesync_స్థితి_pub{ORB_ID(timesync_స్థితి)};

	uint32_t _sequence{0};

	// Timesync statistics
	ద్విగుణ సమయం_స్థానభ్రంశం{0};
	ద్విగుణ సమయం_skew{0};

	// Filter parameters
	ద్విగుణ _filter_alpha{ALPHA_GAIN_INITIAL};
	ద్విగుణ _filter_beta{BETA_GAIN_INITIAL};

	// Outlier rejection and filter reset
	uint32_t ఎగువ_deviation_లెక్క{0};
	uint32_t ఎగువ_rtt_లెక్క{0};

	uint8_t ఆధారం_2{};
};
