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

#অন্তর্ভাব "gnss_checks.hpp"

নামাকাশ
{
constexpr uint16_t kDriftChecks = vehicle_gnss_s::CHECK_HDRIFT | vehicle_gnss_s::CHECK_VDRIFT
				  | vehicle_gnss_s::CHECK_HSPEED | vehicle_gnss_s::CHECK_VSPEED;

// The checks that decide while armed, once the strict ones passed since the vehicle was disarmed on the ground
constexpr uint16_t kRelaxedChecks = vehicle_gnss_s::CHECK_FIX | vehicle_gnss_s::CHECK_EPH | vehicle_gnss_s::CHECK_EPV
				    | vehicle_gnss_s::CHECK_SACC | vehicle_gnss_s::CHECK_SPOOFED | vehicle_gnss_s::CHECK_JAMMED;
}

যৌক্তিক GnssChecks::run(ধ্রুবক gnssChecksSample &gnss, যৌক্তিক armed, যৌক্তিক in_air, যৌক্তিক vehicle_at_rest)
{
	যদি (_time_last_pass_us != 0 && isTimedOut(_time_last_pass_us, gnss.time_us, kPassTimeoutUs)) {
		reset();
	}

	// assume failed first time through
	যদি (_time_last_fail_us == 0) {
		_time_last_fail_us = gnss.time_us;
	}

	যদি (!armed && !in_air) {
		_initial_strict_checks_passed = মিথ্যা;
	}

	// Both sets run on every sample: both are reported, and the selection ranks receivers on the strict one in flight
	ধ্রুবক যৌক্তিক strict_checks_pass = runStrictChecks(gnss, in_air, vehicle_at_rest);
	ধ্রুবক যৌক্তিক relaxed_checks_pass = runRelaxedChecks(gnss);

	_passed_strict = strict_checks_pass
			 && isTimedOut(_time_last_fail_us, gnss.time_us, getRequiredPassDurationUs(মিথ্যা));
	_passed = মিথ্যা;
	_strict = সত্য;

	যদি (_passed_strict) {
		_initial_strict_checks_passed = সত্য;
		_passed = সত্য;

	} অন্যথা যদি (_initial_strict_checks_passed) {
		_strict = মিথ্যা;

		যদি (relaxed_checks_pass) {
			_passed = isTimedOut(_time_last_fail_us, gnss.time_us, getRequiredPassDurationUs(সত্য));

		} অন্যথা {
			_time_last_fail_us = gnss.time_us;
		}

	} অন্যথা যদি (!strict_checks_pass) {
		_time_last_fail_us = gnss.time_us;
	}

	lat_lon_পূর্ব.initReference(gnss.lat, gnss.lon, gnss.time_us);
	alt_পূর্ব = gnss.alt;

	যদি (_passed) {
		_time_last_pass_us = gnss.time_us;
	}

	ফেরত _passed;
}

শূন্য GnssChecks::setFail(uint16_t &fail_flags, uint16_t check, যৌক্তিক failed)
{
	যদি (failed) {
		fail_flags |= check;

	} অন্যথা {
		fail_flags &= ~check;
	}
}

যৌক্তিক GnssChecks::runRelaxedChecks(ধ্রুবক gnssChecksSample &gnss)
{
	setFail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_FIX, gnss.fix_type < 3);

	// Check the reported horizontal and vertical position accuracy
	setFail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_EPH, gnss.hacc > 50.f);
	setFail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_EPV, gnss.vacc > 50.f);

	// Check the reported speed accuracy
	setFail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_SACC, gnss.sacc > 10.f);

	setFail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_SPOOFED, gnss.spoofed);
	setFail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_JAMMED, gnss.jammed);

	ফেরত enabledChecksPass(_relaxed_fail_flags, kRelaxedChecks);
}

যৌক্তিক GnssChecks::runStrictChecks(ধ্রুবক gnssChecksSample &gnss, যৌক্তিক in_air, যৌক্তিক vehicle_at_rest)
{
	// Check the fix type
	setFail(_strict_fail_flags, vehicle_gnss_s::CHECK_FIX, gnss.fix_type < _params.req_fix);

	// Check the number of satellites
	setFail(_strict_fail_flags, vehicle_gnss_s::CHECK_NSATS, gnss.nsats < _params.req_nsats);

	// Check the position dilution of precision
	setFail(_strict_fail_flags, vehicle_gnss_s::CHECK_PDOP, gnss.pdop > _params.req_pdop);

	// Check the reported horizontal and vertical position accuracy
	setFail(_strict_fail_flags, vehicle_gnss_s::CHECK_EPH, gnss.hacc > _params.req_eph);
	setFail(_strict_fail_flags, vehicle_gnss_s::CHECK_EPV, gnss.vacc > _params.req_epv);

	// Check the reported speed accuracy
	setFail(_strict_fail_flags, vehicle_gnss_s::CHECK_SACC, gnss.sacc > _params.req_sacc);

	setFail(_strict_fail_flags, vehicle_gnss_s::CHECK_SPOOFED, gnss.spoofed);
	setFail(_strict_fail_flags, vehicle_gnss_s::CHECK_JAMMED, gnss.jammed);

	runOnGroundGnssChecks(gnss, in_air, vehicle_at_rest);

	ফেরত enabledChecksPass(_strict_fail_flags, UINT16_MAX);
}

শূন্য GnssChecks::runOnGroundGnssChecks(ধ্রুবক gnssChecksSample &gnss, যৌক্তিক in_air, যৌক্তিক vehicle_at_rest)
{
	যদি (in_air) {
		// These checks are always declared as passed when flying
		// If on ground and moving, the last result before movement commenced is kept
		clearDriftChecks();
		ফেরত;
	}

	যদি (vehicle_at_rest) {
		// Calculate time lapsed since last update, limit to prevent numerical errors and calculate a lowpass filter coefficient
		constexpr দশমিক filt_time_const = 10.0f;
		ধ্রুবক দশমিক dt = math::constrain(দশমিক(int64_t(gnss.time_us) - int64_t(
				lat_lon_পূর্ব.getProjectionReferenceTimestamp()))
						 * 1e-6f, 0.001f, filt_time_const);
		ধ্রুবক দশমিক filter_coef = dt / filt_time_const;

		// Calculate position movement since last measurement
		দশমিক delta_pos_n = 0.0f;
		দশমিক delta_pos_e = 0.0f;

		// calculate position movement since last fix
		যদি (lat_lon_পূর্ব.getProjectionReferenceTimestamp() > 0) {
			lat_lon_পূর্ব.project(gnss.lat, gnss.lon, delta_pos_n, delta_pos_e);

		} অন্যথা {
			// no previous position has been set
			lat_lon_পূর্ব.initReference(gnss.lat, gnss.lon, gnss.time_us);
			alt_পূর্ব = gnss.alt;
		}

		// Calculate the horizontal and vertical drift velocity components and limit to 10x the threshold
		ধ্রুবক matrix::Vector3f vel_limit(_params.req_hdrift, _params.req_hdrift, _params.req_vdrift);
		matrix::Vector3f delta_pos(delta_pos_n, delta_pos_e, (alt_পূর্ব - gnss.alt));

		// Apply a low pass filter
		_lat_lon_alt_deriv_filt = delta_pos / dt * filter_coef + _lat_lon_alt_deriv_filt * (1.0f - filter_coef);

		// Apply anti-windup to the state instead of the input to avoid generating a bias on asymmetric signals
		_lat_lon_alt_deriv_filt = matrix::constrain(_lat_lon_alt_deriv_filt, -10.0f * vel_limit, 10.0f * vel_limit);

		// hdrift: calculate the horizontal drift speed and fail if too high
		_horizontal_position_drift_rate_m_s = matrix::Vector2f(_lat_lon_alt_deriv_filt.xy()).norm();
		setFail(_strict_fail_flags, vehicle_gnss_s::CHECK_HDRIFT, _horizontal_position_drift_rate_m_s > _params.req_hdrift);

		// vdrift: fail if the vertical drift speed is too high
		_vertical_position_drift_rate_m_s = fabsf(_lat_lon_alt_deriv_filt(2));
		setFail(_strict_fail_flags, vehicle_gnss_s::CHECK_VDRIFT, _vertical_position_drift_rate_m_s > _params.req_vdrift);

		// hspeed: check the magnitude of the filtered horizontal GNSS velocity
		ধ্রুবক matrix::Vector2f vel_ne = matrix::constrain(matrix::Vector2f(gnss.vel.xy()),
						-10.0f * _params.req_hdrift,
						10.0f * _params.req_hdrift);
		_vel_ne_filt = vel_ne * filter_coef + _vel_ne_filt * (1.0f - filter_coef);
		_filtered_horizontal_velocity_m_s = _vel_ne_filt.norm();
		setFail(_strict_fail_flags, vehicle_gnss_s::CHECK_HSPEED, _filtered_horizontal_velocity_m_s > _params.req_hdrift);

		// vspeed: check the magnitude of the filtered vertical GNSS velocity
		ধ্রুবক দশমিক gnss_vz_limit = 10.f * _params.req_vdrift;
		ধ্রুবক দশমিক gnss_vz = math::constrain(gnss.vel(2), -gnss_vz_limit, gnss_vz_limit);
		_vel_d_filt = gnss_vz * filter_coef + _vel_d_filt * (1.f - filter_coef);

		setFail(_strict_fail_flags, vehicle_gnss_s::CHECK_VSPEED, fabsf(_vel_d_filt) > _params.req_vdrift);

	} অন্যথা {
		// This is the case where the vehicle is on ground and IMU movement is blocking the drift calculation
		resetDriftFilters();
	}
}

শূন্য GnssChecks::clearDriftChecks()
{
	_strict_fail_flags &= ~kDriftChecks;

	resetDriftFilters();
}

শূন্য GnssChecks::resetDriftFilters()
{
	_vel_ne_filt.setZero();
	_vel_d_filt = 0.f;

	_lat_lon_alt_deriv_filt.setZero();

	_horizontal_position_drift_rate_m_s = NAN;
	_vertical_position_drift_rate_m_s = NAN;
	_filtered_horizontal_velocity_m_s = NAN;
}
