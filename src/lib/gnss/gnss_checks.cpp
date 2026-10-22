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

#ಅನ್ತರ್ಭಾವ "gnss_checks.hpp"

ನಾಮಾಕಾಶ
{
constexpr uint16_t kDriftChecks = vehicle_gnss_s::CHECK_HDRIFT | vehicle_gnss_s::CHECK_VDRIFT
				  | vehicle_gnss_s::CHECK_HSPEED | vehicle_gnss_s::CHECK_VSPEED;

// The checks that decide while armed, once the strict ones passed since the vehicle was disarmed on the ground
constexpr uint16_t kRelaxedChecks = vehicle_gnss_s::CHECK_FIX | vehicle_gnss_s::CHECK_EPH | vehicle_gnss_s::CHECK_EPV
				    | vehicle_gnss_s::CHECK_SACC | vehicle_gnss_s::CHECK_SPOOFED | vehicle_gnss_s::CHECK_JAMMED;
}

ತಾರ್ಕಿಕ GnssChecks::ಸತತ_2(ಸ್ಥಿರಾಂಕ gnssChecksSample &gnss, ತಾರ್ಕಿಕ armed, ತಾರ್ಕಿಕ ಒಳ_air, ತಾರ್ಕಿಕ vehicle_at_rest)
{
	ಆದರೆ (ಸಮಯ_ಕೊನೆಯ_pass_us != 0 && is_Timed_ಹೊರಹೋಗು(ಸಮಯ_ಕೊನೆಯ_pass_us, gnss.ಸಮಯ_us, kPassTimeoutUs)) {
		reset();
	}

	// assume failed first time through
	ಆದರೆ (ಸಮಯ_ಕೊನೆಯ_fail_us == 0) {
		ಸಮಯ_ಕೊನೆಯ_fail_us = gnss.ಸಮಯ_us;
	}

	ಆದರೆ (!armed && !ಒಳ_air) {
		_initial_strict_checks_passed = ಸುಳ್ಳು;
	}

	// Both sets run on every sample: both are reported, and the selection ranks receivers on the strict one in flight
	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ strict_checks_pass = ಸತತ_Strict_Checks(gnss, ಒಳ_air, vehicle_at_rest);
	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ relaxed_checks_pass = ಸತತ_Relaxed_Checks(gnss);

	_passed_strict = strict_checks_pass
			 && is_Timed_ಹೊರಹೋಗು(ಸಮಯ_ಕೊನೆಯ_fail_us, gnss.ಸಮಯ_us, getRequiredPassDurationUs(ಸುಳ್ಳು));
	_passed = ಸುಳ್ಳು;
	_strict = ನಿಜ;

	ಆದರೆ (_passed_strict) {
		_initial_strict_checks_passed = ನಿಜ;
		_passed = ನಿಜ;

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (_initial_strict_checks_passed) {
		_strict = ಸುಳ್ಳು;

		ಆದರೆ (relaxed_checks_pass) {
			_passed = is_Timed_ಹೊರಹೋಗು(ಸಮಯ_ಕೊನೆಯ_fail_us, gnss.ಸಮಯ_us, getRequiredPassDurationUs(ನಿಜ));

		} ಇಲ್ಲದಿದ್ದರೆ {
			ಸಮಯ_ಕೊನೆಯ_fail_us = gnss.ಸಮಯ_us;
		}

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!strict_checks_pass) {
		ಸಮಯ_ಕೊನೆಯ_fail_us = gnss.ಸಮಯ_us;
	}

	lat_lon_prev.initReference(gnss.lat, gnss.lon, gnss.ಸಮಯ_us);
	_alt_prev = gnss.alt;

	ಆದರೆ (_passed) {
		ಸಮಯ_ಕೊನೆಯ_pass_us = gnss.ಸಮಯ_us;
	}

	ಹಿಂತಿರುಗಿ _passed;
}

ಶೂನ್ಯ GnssChecks::ಗಣ_Fail(uint16_t &fail_flags, uint16_t check, ತಾರ್ಕಿಕ failed)
{
	ಆದರೆ (failed) {
		fail_flags |= check;

	} ಇಲ್ಲದಿದ್ದರೆ {
		fail_flags &= ~check;
	}
}

ತಾರ್ಕಿಕ GnssChecks::ಸತತ_Relaxed_Checks(ಸ್ಥಿರಾಂಕ gnssChecksSample &gnss)
{
	ಗಣ_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_FIX, gnss.fix_type < 3);

	// Check the reported horizontal and vertical position accuracy
	ಗಣ_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_EPH, gnss.hacc > 50.f);
	ಗಣ_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_EPV, gnss.vacc > 50.f);

	// Check the reported speed accuracy
	ಗಣ_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_SACC, gnss.sacc > 10.f);

	ಗಣ_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_SPOOFED, gnss.spoofed);
	ಗಣ_Fail(_relaxed_fail_flags, vehicle_gnss_s::CHECK_JAMMED, gnss.jammed);

	ಹಿಂತಿರುಗಿ enabledChecksPass(_relaxed_fail_flags, kRelaxedChecks);
}

ತಾರ್ಕಿಕ GnssChecks::ಸತತ_Strict_Checks(ಸ್ಥಿರಾಂಕ gnssChecksSample &gnss, ತಾರ್ಕಿಕ ಒಳ_air, ತಾರ್ಕಿಕ vehicle_at_rest)
{
	// Check the fix type
	ಗಣ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_FIX, gnss.fix_type < _params.req_fix);

	// Check the number of satellites
	ಗಣ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_NSATS, gnss.nsats < _params.req_nsats);

	// Check the position dilution of precision
	ಗಣ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_PDOP, gnss.pdop > _params.req_pdop);

	// Check the reported horizontal and vertical position accuracy
	ಗಣ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_EPH, gnss.hacc > _params.req_eph);
	ಗಣ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_EPV, gnss.vacc > _params.req_epv);

	// Check the reported speed accuracy
	ಗಣ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_SACC, gnss.sacc > _params.req_sacc);

	ಗಣ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_SPOOFED, gnss.spoofed);
	ಗಣ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_JAMMED, gnss.jammed);

	ಸತತ_On_Ground_Gnss_Checks(gnss, ಒಳ_air, vehicle_at_rest);

	ಹಿಂತಿರುಗಿ enabledChecksPass(_strict_fail_flags, UINT16_MAX);
}

ಶೂನ್ಯ GnssChecks::ಸತತ_On_Ground_Gnss_Checks(ಸ್ಥಿರಾಂಕ gnssChecksSample &gnss, ತಾರ್ಕಿಕ ಒಳ_air, ತಾರ್ಕಿಕ vehicle_at_rest)
{
	ಆದರೆ (ಒಳ_air) {
		// These checks are always declared as passed when flying
		// If on ground and moving, the last result before movement commenced is kept
		clearDriftChecks();
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (vehicle_at_rest) {
		// Calculate time lapsed since last update, limit to prevent numerical errors and calculate a lowpass filter coefficient
		constexpr ದಶಮಾಂಶ filt_ಸಮಯ_const = 10.0f;
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dt = math::constrain(ದಶಮಾಂಶ(int64_t(gnss.ಸಮಯ_us) - int64_t(
				lat_lon_prev.getProjectionReferenceTimestamp()))
						 * 1e-6f, 0.001f, filt_ಸಮಯ_const);
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ filter_coef = dt / filt_ಸಮಯ_const;

		// Calculate position movement since last measurement
		ದಶಮಾಂಶ delta_pos_n = 0.0f;
		ದಶಮಾಂಶ delta_pos_e = 0.0f;

		// calculate position movement since last fix
		ಆದರೆ (lat_lon_prev.getProjectionReferenceTimestamp() > 0) {
			lat_lon_prev.project(gnss.lat, gnss.lon, delta_pos_n, delta_pos_e);

		} ಇಲ್ಲದಿದ್ದರೆ {
			// no previous position has been set
			lat_lon_prev.initReference(gnss.lat, gnss.lon, gnss.ಸಮಯ_us);
			_alt_prev = gnss.alt;
		}

		// Calculate the horizontal and vertical drift velocity components and limit to 10x the threshold
		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f vel_ಎಲ್ಲೆ(_params.req_hdrift, _params.req_hdrift, _params.req_vdrift);
		ಮಾತೃಕೆ::Vector3f delta_pos(delta_pos_n, delta_pos_e, (_alt_prev - gnss.alt));

		// Apply a low pass filter
		_lat_lon_alt_deriv_filt = delta_pos / dt * filter_coef + _lat_lon_alt_deriv_filt * (1.0f - filter_coef);

		// Apply anti-windup to the state instead of the input to avoid generating a bias on asymmetric signals
		_lat_lon_alt_deriv_filt = ಮಾತೃಕೆ::constrain(_lat_lon_alt_deriv_filt, -10.0f * vel_ಎಲ್ಲೆ, 10.0f * vel_ಎಲ್ಲೆ);

		// hdrift: calculate the horizontal drift speed and fail if too high
		horizontal_position_drift_ದರ_m_s = ಮಾತೃಕೆ::Vector2f(_lat_lon_alt_deriv_filt.xy()).norm();
		ಗಣ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_HDRIFT, horizontal_position_drift_ದರ_m_s > _params.req_hdrift);

		// vdrift: fail if the vertical drift speed is too high
		vertical_position_drift_ದರ_m_s = fabsf(_lat_lon_alt_deriv_filt(2));
		ಗಣ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_VDRIFT, vertical_position_drift_ದರ_m_s > _params.req_vdrift);

		// hspeed: check the magnitude of the filtered horizontal GNSS velocity
		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f vel_ne = ಮಾತೃಕೆ::constrain(ಮಾತೃಕೆ::Vector2f(gnss.vel.xy()),
						-10.0f * _params.req_hdrift,
						10.0f * _params.req_hdrift);
		_vel_ne_filt = vel_ne * filter_coef + _vel_ne_filt * (1.0f - filter_coef);
		_filtered_horizontal_velocity_m_s = _vel_ne_filt.norm();
		ಗಣ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_HSPEED, _filtered_horizontal_velocity_m_s > _params.req_hdrift);

		// vspeed: check the magnitude of the filtered vertical GNSS velocity
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ gnss_vz_ಎಲ್ಲೆ = 10.f * _params.req_vdrift;
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ gnss_vz = math::constrain(gnss.vel(2), -gnss_vz_ಎಲ್ಲೆ, gnss_vz_ಎಲ್ಲೆ);
		_vel_d_filt = gnss_vz * filter_coef + _vel_d_filt * (1.f - filter_coef);

		ಗಣ_Fail(_strict_fail_flags, vehicle_gnss_s::CHECK_VSPEED, fabsf(_vel_d_filt) > _params.req_vdrift);

	} ಇಲ್ಲದಿದ್ದರೆ {
		// This is the case where the vehicle is on ground and IMU movement is blocking the drift calculation
		resetDriftFilters();
	}
}

ಶೂನ್ಯ GnssChecks::clearDriftChecks()
{
	_strict_fail_flags &= ~kDriftChecks;

	resetDriftFilters();
}

ಶೂನ್ಯ GnssChecks::resetDriftFilters()
{
	_vel_ne_filt.ಗಣ_Zero();
	_vel_d_filt = 0.f;

	_lat_lon_alt_deriv_filt.ಗಣ_Zero();

	horizontal_position_drift_ದರ_m_s = NAN;
	vertical_position_drift_ದರ_m_s = NAN;
	_filtered_horizontal_velocity_m_s = NAN;
}
