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

#شامل "gnss_checks.hpp"

نامخلا
{
constexpr uint16_t kDriftChecks = vehicle_gnss_s::CHECK_HDRIFT | vehicle_gnss_s::CHECK_VDRIFT
				  | vehicle_gnss_s::CHECK_HSPEED | vehicle_gnss_s::CHECK_VSPEED;

// The checks that decide while armed, once the strict ones passed since the vehicle was disarmed on the ground
constexpr uint16_t kRelaxedChecks = vehicle_gnss_s::CHECK_FIX | vehicle_gnss_s::CHECK_EPH | vehicle_gnss_s::CHECK_EPV
				    | vehicle_gnss_s::CHECK_SACC | vehicle_gnss_s::CHECK_SPOOFED | vehicle_gnss_s::CHECK_JAMMED;
}

منطقی GnssChecks::تسلسل_2(ثابت gnssChecksSample &gnss, منطقی armed, منطقی اندر_air, منطقی vehicle_at_rest)
{
	اگر (وقت_آخری_pass_us != 0 && is_Timed_برآمد(وقت_آخری_pass_us, gnss.وقت_us, kPassTimeoutUs)) {
		reset();
	}

	// assume failed first time through
	اگر (وقت_آخری_fail_us == 0) {
		وقت_آخری_fail_us = gnss.وقت_us;
	}

	اگر (!armed && !اندر_air) {
		_initial_strict_checks_passed = جھوٹ;
	}

	// Both sets run on every sample: both are reported, and the selection ranks receivers on the strict one in flight
	ثابت منطقی strict_checks_pass = تسلسل_Strict_Checks(gnss, اندر_air, vehicle_at_rest);
	ثابت منطقی relaxed_checks_pass = تسلسل_Relaxed_Checks(gnss);

	_passed_strict = strict_checks_pass
			 && is_Timed_برآمد(وقت_آخری_fail_us, gnss.وقت_us, getRequiredPassDurationUs(جھوٹ));
	_passed = جھوٹ;
	_strict = سچ;

	اگر (_passed_strict) {
		_initial_strict_checks_passed = سچ;
		_passed = سچ;

	} ورنہ اگر (_initial_strict_checks_passed) {
		_strict = جھوٹ;

		اگر (relaxed_checks_pass) {
			_passed = is_Timed_برآمد(وقت_آخری_fail_us, gnss.وقت_us, getRequiredPassDurationUs(سچ));

		} ورنہ {
			وقت_آخری_fail_us = gnss.وقت_us;
		}

	} ورنہ اگر (!strict_checks_pass) {
		وقت_آخری_fail_us = gnss.وقت_us;
	}

	lat_lon_پچھلا.initReference(gnss.lat, gnss.lon, gnss.وقت_us);
	alt_پچھلا = gnss.alt;

	اگر (_passed) {
		وقت_آخری_pass_us = gnss.وقت_us;
	}

	واپس _passed;
}

خالی GnssChecks::سیٹ_Fail(uint16_t &fail_flags, uint16_t check, منطقی failed)
{
	اگر (failed) {
		fail_flags |= check;

	} ورنہ {
		fail_flags &= ~check;
	}
}

منطقی GnssChecks::تسلسل_Relaxed_Checks(ثابت gnssChecksSample &gnss)
{
	سیٹ_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_FIX, gnss.fix_type < 3);

	// Check the reported horizontal and vertical position accuracy
	سیٹ_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_EPH, gnss.hacc > 50.f);
	سیٹ_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_EPV, gnss.vacc > 50.f);

	// Check the reported speed accuracy
	سیٹ_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_SACC, gnss.sacc > 10.f);

	سیٹ_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_SPOOFED, gnss.spoofed);
	سیٹ_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_JAMMED, gnss.jammed);

	واپس enabledChecksPass(_relaxed_fail_flags, kRelaxedChecks);
}

منطقی GnssChecks::تسلسل_Strict_Checks(ثابت gnssChecksSample &gnss, منطقی اندر_air, منطقی vehicle_at_rest)
{
	// Check the fix type
	سیٹ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_FIX, gnss.fix_type < _params.req_fix);

	// Check the number of satellites
	سیٹ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_NSATS, gnss.nsats < _params.req_nsats);

	// Check the position dilution of precision
	سیٹ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_PDOP, gnss.pdop > _params.req_pdop);

	// Check the reported horizontal and vertical position accuracy
	سیٹ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_EPH, gnss.hacc > _params.req_eph);
	سیٹ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_EPV, gnss.vacc > _params.req_epv);

	// Check the reported speed accuracy
	سیٹ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_SACC, gnss.sacc > _params.req_sacc);

	سیٹ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_SPOOFED, gnss.spoofed);
	سیٹ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_JAMMED, gnss.jammed);

	تسلسل_On_Ground_Gnss_Checks(gnss, اندر_air, vehicle_at_rest);

	واپس enabledChecksPass(_strict_fail_flags, UINT16_MAX);
}

خالی GnssChecks::تسلسل_On_Ground_Gnss_Checks(ثابت gnssChecksSample &gnss, منطقی اندر_air, منطقی vehicle_at_rest)
{
	اگر (اندر_air) {
		// These checks are always declared as passed when flying
		// If on ground and moving, the last result before movement commenced is kept
		clearDriftChecks();
		واپس;
	}

	اگر (vehicle_at_rest) {
		// Calculate time lapsed since last update, limit to prevent numerical errors and calculate a lowpass filter coefficient
		constexpr اعشاری filt_وقت_const = 10.0f;
		ثابت اعشاری dt = math::constrain(اعشاری(int64_t(gnss.وقت_us) - int64_t(
				lat_lon_پچھلا.getProjectionReferenceTimestamp()))
						 * 1e-6f, 0.001f, filt_وقت_const);
		ثابت اعشاری filter_coef = dt / filt_وقت_const;

		// Calculate position movement since last measurement
		اعشاری delta_pos_n = 0.0f;
		اعشاری delta_pos_e = 0.0f;

		// calculate position movement since last fix
		اگر (lat_lon_پچھلا.getProjectionReferenceTimestamp() > 0) {
			lat_lon_پچھلا.project(gnss.lat, gnss.lon, delta_pos_n, delta_pos_e);

		} ورنہ {
			// no previous position has been set
			lat_lon_پچھلا.initReference(gnss.lat, gnss.lon, gnss.وقت_us);
			alt_پچھلا = gnss.alt;
		}

		// Calculate the horizontal and vertical drift velocity components and limit to 10x the threshold
		ثابت میٹرکس::Vector3f vel_حد_بندی(_params.req_hdrift, _params.req_hdrift, _params.req_vdrift);
		میٹرکس::Vector3f delta_pos(delta_pos_n, delta_pos_e, (alt_پچھلا - gnss.alt));

		// Apply a low pass filter
		_lat_lon_alt_deriv_filt = delta_pos / dt * filter_coef + _lat_lon_alt_deriv_filt * (1.0f - filter_coef);

		// Apply anti-windup to the state instead of the input to avoid generating a bias on asymmetric signals
		_lat_lon_alt_deriv_filt = میٹرکس::constrain(_lat_lon_alt_deriv_filt, -10.0f * vel_حد_بندی, 10.0f * vel_حد_بندی);

		// hdrift: calculate the horizontal drift speed and fail if too high
		horizontal_position_drift_شرح_m_s = میٹرکس::Vector2f(_lat_lon_alt_deriv_filt.xy()).norm();
		سیٹ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_HDRIFT, horizontal_position_drift_شرح_m_s > _params.req_hdrift);

		// vdrift: fail if the vertical drift speed is too high
		vertical_position_drift_شرح_m_s = fabsf(_lat_lon_alt_deriv_filt(2));
		سیٹ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_VDRIFT, vertical_position_drift_شرح_m_s > _params.req_vdrift);

		// hspeed: check the magnitude of the filtered horizontal GNSS velocity
		ثابت میٹرکس::Vector2f vel_ne = میٹرکس::constrain(میٹرکس::Vector2f(gnss.vel.xy()),
						-10.0f * _params.req_hdrift,
						10.0f * _params.req_hdrift);
		_vel_ne_filt = vel_ne * filter_coef + _vel_ne_filt * (1.0f - filter_coef);
		_filtered_horizontal_velocity_m_s = _vel_ne_filt.norm();
		سیٹ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_HSPEED, _filtered_horizontal_velocity_m_s > _params.req_hdrift);

		// vspeed: check the magnitude of the filtered vertical GNSS velocity
		ثابت اعشاری gnss_vz_حد_بندی = 10.f * _params.req_vdrift;
		ثابت اعشاری gnss_vz = math::constrain(gnss.vel(2), -gnss_vz_حد_بندی, gnss_vz_حد_بندی);
		_vel_d_filt = gnss_vz * filter_coef + _vel_d_filt * (1.f - filter_coef);

		سیٹ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_VSPEED, fabsf(_vel_d_filt) > _params.req_vdrift);

	} ورنہ {
		// This is the case where the vehicle is on ground and IMU movement is blocking the drift calculation
		resetDriftFilters();
	}
}

خالی GnssChecks::clearDriftChecks()
{
	_strict_fail_flags &= ~kDriftChecks;

	resetDriftFilters();
}

خالی GnssChecks::resetDriftFilters()
{
	_vel_ne_filt.سیٹ_Zero();
	_vel_d_filt = 0.f;

	_lat_lon_alt_deriv_filt.سیٹ_Zero();

	horizontal_position_drift_شرح_m_s = NAN;
	vertical_position_drift_شرح_m_s = NAN;
	_filtered_horizontal_velocity_m_s = NAN;
}
