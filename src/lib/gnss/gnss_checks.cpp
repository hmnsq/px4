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

#अन्तर्भाव "gnss_checks.hpp"

नामाकाश
{
constexpr uint16_t kDriftChecks = vehicle_gnss_s::CHECK_HDRIFT | vehicle_gnss_s::CHECK_VDRIFT
				  | vehicle_gnss_s::CHECK_HSPEED | vehicle_gnss_s::CHECK_VSPEED;

// The checks that decide while armed, once the strict ones passed since the vehicle was disarmed on the ground
constexpr uint16_t kRelaxedChecks = vehicle_gnss_s::CHECK_FIX | vehicle_gnss_s::CHECK_EPH | vehicle_gnss_s::CHECK_EPV
				    | vehicle_gnss_s::CHECK_SACC | vehicle_gnss_s::CHECK_SPOOFED | vehicle_gnss_s::CHECK_JAMMED;
}

तार्किक GnssChecks::सततम्_2(ध्रुव gnssChecksSample &gnss, तार्किक armed, तार्किक अन्तर्_air, तार्किक vehicle_at_rest)
{
	यदि (time_चरमः_pass_us != 0 && is_Timed_बहिर्गमनम्(time_चरमः_pass_us, gnss.time_us, kPassTimeoutUs)) {
		reset();
	}

	// assume failed first time through
	यदि (time_चरमः_fail_us == 0) {
		time_चरमः_fail_us = gnss.time_us;
	}

	यदि (!armed && !अन्तर्_air) {
		_initial_strict_checks_passed = असत्य;
	}

	// Both sets run on every sample: both are reported, and the selection ranks receivers on the strict one in flight
	ध्रुव तार्किक strict_checks_pass = सततम्_Strict_Checks(gnss, अन्तर्_air, vehicle_at_rest);
	ध्रुव तार्किक relaxed_checks_pass = सततम्_Relaxed_Checks(gnss);

	_passed_strict = strict_checks_pass
			 && is_Timed_बहिर्गमनम्(time_चरमः_fail_us, gnss.time_us, getRequiredPassDurationUs(असत्य));
	_passed = असत्य;
	_strict = सत्य;

	यदि (_passed_strict) {
		_initial_strict_checks_passed = सत्य;
		_passed = सत्य;

	} अन्यथा यदि (_initial_strict_checks_passed) {
		_strict = असत्य;

		यदि (relaxed_checks_pass) {
			_passed = is_Timed_बहिर्गमनम्(time_चरमः_fail_us, gnss.time_us, getRequiredPassDurationUs(सत्य));

		} अन्यथा {
			time_चरमः_fail_us = gnss.time_us;
		}

	} अन्यथा यदि (!strict_checks_pass) {
		time_चरमः_fail_us = gnss.time_us;
	}

	lat_lon_पूर्व.initReference(gnss.lat, gnss.lon, gnss.time_us);
	alt_पूर्व = gnss.alt;

	यदि (_passed) {
		time_चरमः_pass_us = gnss.time_us;
	}

	प्रत्यागम _passed;
}

शून्य GnssChecks::समुच्चयः_Fail(uint16_t &fail_flags, uint16_t check, तार्किक failed)
{
	यदि (failed) {
		fail_flags |= check;

	} अन्यथा {
		fail_flags &= ~check;
	}
}

तार्किक GnssChecks::सततम्_Relaxed_Checks(ध्रुव gnssChecksSample &gnss)
{
	समुच्चयः_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_FIX, gnss.fix_type < 3);

	// Check the reported horizontal and vertical position accuracy
	समुच्चयः_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_EPH, gnss.hacc > 50.f);
	समुच्चयः_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_EPV, gnss.vacc > 50.f);

	// Check the reported speed accuracy
	समुच्चयः_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_SACC, gnss.sacc > 10.f);

	समुच्चयः_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_SPOOFED, gnss.spoofed);
	समुच्चयः_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_JAMMED, gnss.jammed);

	प्रत्यागम enabledChecksPass(_relaxed_fail_flags, kRelaxedChecks);
}

तार्किक GnssChecks::सततम्_Strict_Checks(ध्रुव gnssChecksSample &gnss, तार्किक अन्तर्_air, तार्किक vehicle_at_rest)
{
	// Check the fix type
	समुच्चयः_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_FIX, gnss.fix_type < _params.req_fix);

	// Check the number of satellites
	समुच्चयः_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_NSATS, gnss.nsats < _params.req_nsats);

	// Check the position dilution of precision
	समुच्चयः_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_PDOP, gnss.pdop > _params.req_pdop);

	// Check the reported horizontal and vertical position accuracy
	समुच्चयः_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_EPH, gnss.hacc > _params.req_eph);
	समुच्चयः_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_EPV, gnss.vacc > _params.req_epv);

	// Check the reported speed accuracy
	समुच्चयः_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_SACC, gnss.sacc > _params.req_sacc);

	समुच्चयः_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_SPOOFED, gnss.spoofed);
	समुच्चयः_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_JAMMED, gnss.jammed);

	सततम्_On_Ground_Gnss_Checks(gnss, अन्तर्_air, vehicle_at_rest);

	प्रत्यागम enabledChecksPass(_strict_fail_flags, UINT16_MAX);
}

शून्य GnssChecks::सततम्_On_Ground_Gnss_Checks(ध्रुव gnssChecksSample &gnss, तार्किक अन्तर्_air, तार्किक vehicle_at_rest)
{
	यदि (अन्तर्_air) {
		// These checks are always declared as passed when flying
		// If on ground and moving, the last result before movement commenced is kept
		clearDriftChecks();
		प्रत्यागम;
	}

	यदि (vehicle_at_rest) {
		// Calculate time lapsed since last update, limit to prevent numerical errors and calculate a lowpass filter coefficient
		constexpr भिन्न filt_time_const = 10.0f;
		ध्रुव भिन्न dt = math::constrain(भिन्न(int64_t(gnss.time_us) - int64_t(
				lat_lon_पूर्व.getProjectionReferenceTimestamp()))
						 * 1e-6f, 0.001f, filt_time_const);
		ध्रुव भिन्न filter_coef = dt / filt_time_const;

		// Calculate position movement since last measurement
		भिन्न delta_pos_n = 0.0f;
		भिन्न delta_pos_e = 0.0f;

		// calculate position movement since last fix
		यदि (lat_lon_पूर्व.getProjectionReferenceTimestamp() > 0) {
			lat_lon_पूर्व.project(gnss.lat, gnss.lon, delta_pos_n, delta_pos_e);

		} अन्यथा {
			// no previous position has been set
			lat_lon_पूर्व.initReference(gnss.lat, gnss.lon, gnss.time_us);
			alt_पूर्व = gnss.alt;
		}

		// Calculate the horizontal and vertical drift velocity components and limit to 10x the threshold
		ध्रुव matrix::Vector3f vel_सीमा(_params.req_hdrift, _params.req_hdrift, _params.req_vdrift);
		matrix::Vector3f delta_pos(delta_pos_n, delta_pos_e, (alt_पूर्व - gnss.alt));

		// Apply a low pass filter
		_lat_lon_alt_deriv_filt = delta_pos / dt * filter_coef + _lat_lon_alt_deriv_filt * (1.0f - filter_coef);

		// Apply anti-windup to the state instead of the input to avoid generating a bias on asymmetric signals
		_lat_lon_alt_deriv_filt = matrix::constrain(_lat_lon_alt_deriv_filt, -10.0f * vel_सीमा, 10.0f * vel_सीमा);

		// hdrift: calculate the horizontal drift speed and fail if too high
		horizontal_position_drift_दरः_m_s = matrix::Vector2f(_lat_lon_alt_deriv_filt.xy()).norm();
		समुच्चयः_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_HDRIFT, horizontal_position_drift_दरः_m_s > _params.req_hdrift);

		// vdrift: fail if the vertical drift speed is too high
		vertical_position_drift_दरः_m_s = fabsf(_lat_lon_alt_deriv_filt(2));
		समुच्चयः_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_VDRIFT, vertical_position_drift_दरः_m_s > _params.req_vdrift);

		// hspeed: check the magnitude of the filtered horizontal GNSS velocity
		ध्रुव matrix::Vector2f vel_ne = matrix::constrain(matrix::Vector2f(gnss.vel.xy()),
						-10.0f * _params.req_hdrift,
						10.0f * _params.req_hdrift);
		_vel_ne_filt = vel_ne * filter_coef + _vel_ne_filt * (1.0f - filter_coef);
		_filtered_horizontal_velocity_m_s = _vel_ne_filt.norm();
		समुच्चयः_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_HSPEED, _filtered_horizontal_velocity_m_s > _params.req_hdrift);

		// vspeed: check the magnitude of the filtered vertical GNSS velocity
		ध्रुव भिन्न gnss_vz_सीमा = 10.f * _params.req_vdrift;
		ध्रुव भिन्न gnss_vz = math::constrain(gnss.vel(2), -gnss_vz_सीमा, gnss_vz_सीमा);
		_vel_d_filt = gnss_vz * filter_coef + _vel_d_filt * (1.f - filter_coef);

		समुच्चयः_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_VSPEED, fabsf(_vel_d_filt) > _params.req_vdrift);

	} अन्यथा {
		// This is the case where the vehicle is on ground and IMU movement is blocking the drift calculation
		resetDriftFilters();
	}
}

शून्य GnssChecks::clearDriftChecks()
{
	_strict_fail_flags &= ~kDriftChecks;

	resetDriftFilters();
}

शून्य GnssChecks::resetDriftFilters()
{
	_vel_ne_filt.समुच्चयः_Zero();
	_vel_d_filt = 0.f;

	_lat_lon_alt_deriv_filt.समुच्चयः_Zero();

	horizontal_position_drift_दरः_m_s = NAN;
	vertical_position_drift_दरः_m_s = NAN;
	_filtered_horizontal_velocity_m_s = NAN;
}
