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

#ضمن "gnss_checks.hpp"

نطاق
{
constexpr uint16_t kDriftChecks = vehicle_gnss_s::CHECK_HDRIFT | vehicle_gnss_s::CHECK_VDRIFT
				  | vehicle_gnss_s::CHECK_HSPEED | vehicle_gnss_s::CHECK_VSPEED;

// The checks that decide while armed, once the strict ones passed since the vehicle was disarmed on the ground
constexpr uint16_t kRelaxedChecks = vehicle_gnss_s::CHECK_FIX | vehicle_gnss_s::CHECK_EPH | vehicle_gnss_s::CHECK_EPV
				    | vehicle_gnss_s::CHECK_SACC | vehicle_gnss_s::CHECK_SPOOFED | vehicle_gnss_s::CHECK_JAMMED;
}

منطقي GnssChecks::پیاپی_2(ثابت gnssChecksSample &gnss, منطقي armed, منطقي درون_air, منطقي vehicle_at_rest)
{
	إذا (زمان_آخرین_pass_us != 0 && is_Timed_بیرون(زمان_آخرین_pass_us, gnss.زمان_us, kPassTimeoutUs)) {
		reset();
	}

	// assume failed first time through
	إذا (زمان_آخرین_fail_us == 0) {
		زمان_آخرین_fail_us = gnss.زمان_us;
	}

	إذا (!armed && !درون_air) {
		_initial_strict_checks_passed = خطأ;
	}

	// Both sets run on every sample: both are reported, and the selection ranks receivers on the strict one in flight
	ثابت منطقي strict_checks_pass = پیاپی_Strict_Checks(gnss, درون_air, vehicle_at_rest);
	ثابت منطقي relaxed_checks_pass = پیاپی_Relaxed_Checks(gnss);

	_passed_strict = strict_checks_pass
			 && is_Timed_بیرون(زمان_آخرین_fail_us, gnss.زمان_us, getRequiredPassDurationUs(خطأ));
	_passed = خطأ;
	_strict = درست;

	إذا (_passed_strict) {
		_initial_strict_checks_passed = درست;
		_passed = درست;

	} وإلا إذا (_initial_strict_checks_passed) {
		_strict = خطأ;

		إذا (relaxed_checks_pass) {
			_passed = is_Timed_بیرون(زمان_آخرین_fail_us, gnss.زمان_us, getRequiredPassDurationUs(درست));

		} وإلا {
			زمان_آخرین_fail_us = gnss.زمان_us;
		}

	} وإلا إذا (!strict_checks_pass) {
		زمان_آخرین_fail_us = gnss.زمان_us;
	}

	lat_lon_prev.initReference(gnss.lat, gnss.lon, gnss.زمان_us);
	_alt_prev = gnss.alt;

	إذا (_passed) {
		زمان_آخرین_pass_us = gnss.زمان_us;
	}

	أرجع _passed;
}

فراغ GnssChecks::مجموعه_Fail(uint16_t &fail_flags, uint16_t check, منطقي failed)
{
	إذا (failed) {
		fail_flags |= check;

	} وإلا {
		fail_flags &= ~check;
	}
}

منطقي GnssChecks::پیاپی_Relaxed_Checks(ثابت gnssChecksSample &gnss)
{
	مجموعه_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_FIX, gnss.fix_type < 3);

	// Check the reported horizontal and vertical position accuracy
	مجموعه_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_EPH, gnss.hacc > 50.f);
	مجموعه_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_EPV, gnss.vacc > 50.f);

	// Check the reported speed accuracy
	مجموعه_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_SACC, gnss.sacc > 10.f);

	مجموعه_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_SPOOFED, gnss.spoofed);
	مجموعه_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_JAMMED, gnss.jammed);

	أرجع enabledChecksPass(_relaxed_fail_flags, kRelaxedChecks);
}

منطقي GnssChecks::پیاپی_Strict_Checks(ثابت gnssChecksSample &gnss, منطقي درون_air, منطقي vehicle_at_rest)
{
	// Check the fix type
	مجموعه_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_FIX, gnss.fix_type < _params.req_fix);

	// Check the number of satellites
	مجموعه_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_NSATS, gnss.nsats < _params.req_nsats);

	// Check the position dilution of precision
	مجموعه_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_PDOP, gnss.pdop > _params.req_pdop);

	// Check the reported horizontal and vertical position accuracy
	مجموعه_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_EPH, gnss.hacc > _params.req_eph);
	مجموعه_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_EPV, gnss.vacc > _params.req_epv);

	// Check the reported speed accuracy
	مجموعه_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_SACC, gnss.sacc > _params.req_sacc);

	مجموعه_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_SPOOFED, gnss.spoofed);
	مجموعه_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_JAMMED, gnss.jammed);

	پیاپی_On_Ground_Gnss_Checks(gnss, درون_air, vehicle_at_rest);

	أرجع enabledChecksPass(_strict_fail_flags, UINT16_MAX);
}

فراغ GnssChecks::پیاپی_On_Ground_Gnss_Checks(ثابت gnssChecksSample &gnss, منطقي درون_air, منطقي vehicle_at_rest)
{
	إذا (درون_air) {
		// These checks are always declared as passed when flying
		// If on ground and moving, the last result before movement commenced is kept
		clearDriftChecks();
		أرجع;
	}

	إذا (vehicle_at_rest) {
		// Calculate time lapsed since last update, limit to prevent numerical errors and calculate a lowpass filter coefficient
		constexpr عائم filt_زمان_const = 10.0f;
		ثابت عائم dt = math::constrain(عائم(int64_t(gnss.زمان_us) - int64_t(
				lat_lon_prev.getProjectionReferenceTimestamp()))
						 * 1e-6f, 0.001f, filt_زمان_const);
		ثابت عائم filter_coef = dt / filt_زمان_const;

		// Calculate position movement since last measurement
		عائم delta_pos_n = 0.0f;
		عائم delta_pos_e = 0.0f;

		// calculate position movement since last fix
		إذا (lat_lon_prev.getProjectionReferenceTimestamp() > 0) {
			lat_lon_prev.project(gnss.lat, gnss.lon, delta_pos_n, delta_pos_e);

		} وإلا {
			// no previous position has been set
			lat_lon_prev.initReference(gnss.lat, gnss.lon, gnss.زمان_us);
			_alt_prev = gnss.alt;
		}

		// Calculate the horizontal and vertical drift velocity components and limit to 10x the threshold
		ثابت ماتریس::Vector3f vel_حد(_params.req_hdrift, _params.req_hdrift, _params.req_vdrift);
		ماتریس::Vector3f delta_pos(delta_pos_n, delta_pos_e, (_alt_prev - gnss.alt));

		// Apply a low pass filter
		_lat_lon_alt_deriv_filt = delta_pos / dt * filter_coef + _lat_lon_alt_deriv_filt * (1.0f - filter_coef);

		// Apply anti-windup to the state instead of the input to avoid generating a bias on asymmetric signals
		_lat_lon_alt_deriv_filt = ماتریس::constrain(_lat_lon_alt_deriv_filt, -10.0f * vel_حد, 10.0f * vel_حد);

		// hdrift: calculate the horizontal drift speed and fail if too high
		horizontal_position_drift_نرخ_m_s = ماتریس::Vector2f(_lat_lon_alt_deriv_filt.xy()).norm();
		مجموعه_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_HDRIFT, horizontal_position_drift_نرخ_m_s > _params.req_hdrift);

		// vdrift: fail if the vertical drift speed is too high
		vertical_position_drift_نرخ_m_s = fabsf(_lat_lon_alt_deriv_filt(2));
		مجموعه_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_VDRIFT, vertical_position_drift_نرخ_m_s > _params.req_vdrift);

		// hspeed: check the magnitude of the filtered horizontal GNSS velocity
		ثابت ماتریس::Vector2f vel_ne = ماتریس::constrain(ماتریس::Vector2f(gnss.vel.xy()),
						-10.0f * _params.req_hdrift,
						10.0f * _params.req_hdrift);
		_vel_ne_filt = vel_ne * filter_coef + _vel_ne_filt * (1.0f - filter_coef);
		_filtered_horizontal_velocity_m_s = _vel_ne_filt.norm();
		مجموعه_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_HSPEED, _filtered_horizontal_velocity_m_s > _params.req_hdrift);

		// vspeed: check the magnitude of the filtered vertical GNSS velocity
		ثابت عائم gnss_vz_حد = 10.f * _params.req_vdrift;
		ثابت عائم gnss_vz = math::constrain(gnss.vel(2), -gnss_vz_حد, gnss_vz_حد);
		_vel_d_filt = gnss_vz * filter_coef + _vel_d_filt * (1.f - filter_coef);

		مجموعه_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_VSPEED, fabsf(_vel_d_filt) > _params.req_vdrift);

	} وإلا {
		// This is the case where the vehicle is on ground and IMU movement is blocking the drift calculation
		resetDriftFilters();
	}
}

فراغ GnssChecks::clearDriftChecks()
{
	_strict_fail_flags &= ~kDriftChecks;

	resetDriftFilters();
}

فراغ GnssChecks::resetDriftFilters()
{
	_vel_ne_filt.مجموعه_Zero();
	_vel_d_filt = 0.f;

	_lat_lon_alt_deriv_filt.مجموعه_Zero();

	horizontal_position_drift_نرخ_m_s = NAN;
	vertical_position_drift_نرخ_m_s = NAN;
	_filtered_horizontal_velocity_m_s = NAN;
}
