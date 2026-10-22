/****************************************************************************
 *
 *   Copyright (c) 2025 PX4 Development Team. All rights reserved.
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
 * @file gnss_checks.cpp
 * GNSS quality checks: strict ones to qualify a receiver, relaxed ones once armed
 */

#అన్తర్భావ "gnss_checks.hpp"

నామాకాశ
{
constexpr uint16_t kDriftChecks = vehicle_gnss_s::CHECK_HDRIFT | vehicle_gnss_s::CHECK_VDRIFT
				  | vehicle_gnss_s::CHECK_HSPEED | vehicle_gnss_s::CHECK_VSPEED;

// The checks that decide while armed, once the strict ones passed since the vehicle was disarmed on the ground
constexpr uint16_t kRelaxedChecks = vehicle_gnss_s::CHECK_FIX | vehicle_gnss_s::CHECK_EPH | vehicle_gnss_s::CHECK_EPV
				    | vehicle_gnss_s::CHECK_SACC | vehicle_gnss_s::CHECK_SPOOFED | vehicle_gnss_s::CHECK_JAMMED;
}

తార్కిక GnssChecks::నిరంతరం_2(స్థిరరాశి gnssChecksSample &gnss, తార్కిక armed, తార్కిక లోపల_air, తార్కిక vehicle_at_rest)
{
	ఒకవేళ (సమయం_చివరి_pass_us != 0 && is_Timed_బహిర్గతం(సమయం_చివరి_pass_us, gnss.సమయం_us, kPassTimeoutUs)) {
		reset();
	}

	// assume failed first time through
	ఒకవేళ (సమయం_చివరి_fail_us == 0) {
		సమయం_చివరి_fail_us = gnss.సమయం_us;
	}

	ఒకవేళ (!armed && !లోపల_air) {
		_initial_strict_checks_passed = అబద్ధం;
	}

	// Both sets run on every sample: both are reported, and the selection ranks receivers on the strict one in flight
	స్థిరరాశి తార్కిక strict_checks_pass = నిరంతరం_Strict_Checks(gnss, లోపల_air, vehicle_at_rest);
	స్థిరరాశి తార్కిక relaxed_checks_pass = నిరంతరం_Relaxed_Checks(gnss);

	_passed_strict = strict_checks_pass
			 && is_Timed_బహిర్గతం(సమయం_చివరి_fail_us, gnss.సమయం_us, getRequiredPassDurationUs(అబద్ధం));
	_passed = అబద్ధం;
	_strict = నిజం;

	ఒకవేళ (_passed_strict) {
		_initial_strict_checks_passed = నిజం;
		_passed = నిజం;

	} లేకపోతే ఒకవేళ (_initial_strict_checks_passed) {
		_strict = అబద్ధం;

		ఒకవేళ (relaxed_checks_pass) {
			_passed = is_Timed_బహిర్గతం(సమయం_చివరి_fail_us, gnss.సమయం_us, getRequiredPassDurationUs(నిజం));

		} లేకపోతే {
			సమయం_చివరి_fail_us = gnss.సమయం_us;
		}

	} లేకపోతే ఒకవేళ (!strict_checks_pass) {
		సమయం_చివరి_fail_us = gnss.సమయం_us;
	}

	lat_lon_prev.initReference(gnss.lat, gnss.lon, gnss.సమయం_us);
	_alt_prev = gnss.alt;

	ఒకవేళ (_passed) {
		సమయం_చివరి_pass_us = gnss.సమయం_us;
	}

	తిరిగి _passed;
}

శూన్య GnssChecks::సమితి_Fail(uint16_t &fail_flags, uint16_t check, తార్కిక failed)
{
	ఒకవేళ (failed) {
		fail_flags |= check;

	} లేకపోతే {
		fail_flags &= ~check;
	}
}

తార్కిక GnssChecks::నిరంతరం_Relaxed_Checks(స్థిరరాశి gnssChecksSample &gnss)
{
	సమితి_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_FIX, gnss.fix_type < 3);

	// Check the reported horizontal and vertical position accuracy
	సమితి_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_EPH, gnss.hacc > 50.f);
	సమితి_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_EPV, gnss.vacc > 50.f);

	// Check the reported speed accuracy
	సమితి_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_SACC, gnss.sacc > 10.f);

	సమితి_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_SPOOFED, gnss.spoofed);
	సమితి_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_JAMMED, gnss.jammed);

	తిరిగి enabledChecksPass(_relaxed_fail_flags, kRelaxedChecks);
}

తార్కిక GnssChecks::నిరంతరం_Strict_Checks(స్థిరరాశి gnssChecksSample &gnss, తార్కిక లోపల_air, తార్కిక vehicle_at_rest)
{
	// Check the fix type
	సమితి_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_FIX, gnss.fix_type < _params.req_fix);

	// Check the number of satellites
	సమితి_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_NSATS, gnss.nsats < _params.req_nsats);

	// Check the position dilution of precision
	సమితి_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_PDOP, gnss.pdop > _params.req_pdop);

	// Check the reported horizontal and vertical position accuracy
	సమితి_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_EPH, gnss.hacc > _params.req_eph);
	సమితి_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_EPV, gnss.vacc > _params.req_epv);

	// Check the reported speed accuracy
	సమితి_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_SACC, gnss.sacc > _params.req_sacc);

	సమితి_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_SPOOFED, gnss.spoofed);
	సమితి_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_JAMMED, gnss.jammed);

	నిరంతరం_On_Ground_Gnss_Checks(gnss, లోపల_air, vehicle_at_rest);

	తిరిగి enabledChecksPass(_strict_fail_flags, UINT16_MAX);
}

శూన్య GnssChecks::నిరంతరం_On_Ground_Gnss_Checks(స్థిరరాశి gnssChecksSample &gnss, తార్కిక లోపల_air, తార్కిక vehicle_at_rest)
{
	ఒకవేళ (లోపల_air) {
		// These checks are always declared as passed when flying
		// If on ground and moving, the last result before movement commenced is kept
		clearDriftChecks();
		తిరిగి;
	}

	ఒకవేళ (vehicle_at_rest) {
		// Calculate time lapsed since last update, limit to prevent numerical errors and calculate a lowpass filter coefficient
		constexpr దశాంశం filt_సమయం_const = 10.0f;
		స్థిరరాశి దశాంశం dt = math::constrain(దశాంశం(int64_t(gnss.సమయం_us) - int64_t(
				lat_lon_prev.getProjectionReferenceTimestamp()))
						 * 1e-6f, 0.001f, filt_సమయం_const);
		స్థిరరాశి దశాంశం filter_coef = dt / filt_సమయం_const;

		// Calculate position movement since last measurement
		దశాంశం delta_pos_n = 0.0f;
		దశాంశం delta_pos_e = 0.0f;

		// calculate position movement since last fix
		ఒకవేళ (lat_lon_prev.getProjectionReferenceTimestamp() > 0) {
			lat_lon_prev.project(gnss.lat, gnss.lon, delta_pos_n, delta_pos_e);

		} లేకపోతే {
			// no previous position has been set
			lat_lon_prev.initReference(gnss.lat, gnss.lon, gnss.సమయం_us);
			_alt_prev = gnss.alt;
		}

		// Calculate the horizontal and vertical drift velocity components and limit to 10x the threshold
		స్థిరరాశి మాతృక::Vector3f vel_హద్దు(_params.req_hdrift, _params.req_hdrift, _params.req_vdrift);
		మాతృక::Vector3f delta_pos(delta_pos_n, delta_pos_e, (_alt_prev - gnss.alt));

		// Apply a low pass filter
		_lat_lon_alt_deriv_filt = delta_pos / dt * filter_coef + _lat_lon_alt_deriv_filt * (1.0f - filter_coef);

		// Apply anti-windup to the state instead of the input to avoid generating a bias on asymmetric signals
		_lat_lon_alt_deriv_filt = మాతృక::constrain(_lat_lon_alt_deriv_filt, -10.0f * vel_హద్దు, 10.0f * vel_హద్దు);

		// hdrift: calculate the horizontal drift speed and fail if too high
		horizontal_position_drift_రేటు_m_s = మాతృక::Vector2f(_lat_lon_alt_deriv_filt.xy()).norm();
		సమితి_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_HDRIFT, horizontal_position_drift_రేటు_m_s > _params.req_hdrift);

		// vdrift: fail if the vertical drift speed is too high
		vertical_position_drift_రేటు_m_s = fabsf(_lat_lon_alt_deriv_filt(2));
		సమితి_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_VDRIFT, vertical_position_drift_రేటు_m_s > _params.req_vdrift);

		// hspeed: check the magnitude of the filtered horizontal GNSS velocity
		స్థిరరాశి మాతృక::Vector2f vel_ne = మాతృక::constrain(మాతృక::Vector2f(gnss.vel.xy()),
						-10.0f * _params.req_hdrift,
						10.0f * _params.req_hdrift);
		_vel_ne_filt = vel_ne * filter_coef + _vel_ne_filt * (1.0f - filter_coef);
		_filtered_horizontal_velocity_m_s = _vel_ne_filt.norm();
		సమితి_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_HSPEED, _filtered_horizontal_velocity_m_s > _params.req_hdrift);

		// vspeed: check the magnitude of the filtered vertical GNSS velocity
		స్థిరరాశి దశాంశం gnss_vz_హద్దు = 10.f * _params.req_vdrift;
		స్థిరరాశి దశాంశం gnss_vz = math::constrain(gnss.vel(2), -gnss_vz_హద్దు, gnss_vz_హద్దు);
		_vel_d_filt = gnss_vz * filter_coef + _vel_d_filt * (1.f - filter_coef);

		సమితి_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_VSPEED, fabsf(_vel_d_filt) > _params.req_vdrift);

	} లేకపోతే {
		// This is the case where the vehicle is on ground and IMU movement is blocking the drift calculation
		resetDriftFilters();
	}
}

శూన్య GnssChecks::clearDriftChecks()
{
	_strict_fail_flags &= ~kDriftChecks;

	resetDriftFilters();
}

శూన్య GnssChecks::resetDriftFilters()
{
	_vel_ne_filt.సమితి_Zero();
	_vel_d_filt = 0.f;

	_lat_lon_alt_deriv_filt.సమితి_Zero();

	horizontal_position_drift_రేటు_m_s = NAN;
	vertical_position_drift_రేటు_m_s = NAN;
	_filtered_horizontal_velocity_m_s = NAN;
}
