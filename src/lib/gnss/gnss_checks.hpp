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

#ifndef GNSS_CHECKS_H
#define GNSS_CHECKS_H

#അന്തര്ഭാവ <lib/geo/geo.h>
#അന്തര്ഭാവ <lib/mathlib/mathlib.h>
#അന്തര്ഭാവ <lib/matrix/matrix/math.hpp>
#അന്തര്ഭാവ <uORB/topics/vehicle_gnss.h>

ഘടന gnssChecksSample {
	uint64_t time_us{};     ///< measurement time (us)
	ഇരട്ട lat{};           ///< latitude (deg)
	ഇരട്ട lon{};           ///< longitude (deg)
	ദശാംശം alt{};            ///< altitude above MSL (m)
	matrix::Vector3f vel{}; ///< NED velocity (m/s)
	ദശാംശം hacc{};           ///< 1-std horizontal position error (m)
	ദശാംശം vacc{};           ///< 1-std vertical position error (m)
	ദശാംശം sacc{};           ///< 1-std speed error (m/s)
	uint8_t fix_type{};     ///< 0-1: no fix, 2: 2D fix, 3: 3D fix, 4: RTCM code differential, 5: RTK float, 6: RTK fixed
	uint8_t nsats{};        ///< number of satellites used
	ദശാംശം pdop{};           ///< position dilution of precision
	തർക്കം spoofed{};         ///< the receiver reports spoofing
	തർക്കം jammed{};          ///< the receiver reports jamming
};

വർഗ്ഗം GnssChecks final
{
പൊതു:
	ഘടന Params {
		int32_t check_mask{2047};
		int32_t req_nsats{6};
		ദശാംശം req_pdop{2.5f};
		ദശാംശം req_eph{3.f};
		ദശാംശം req_epv{5.f};
		ദശാംശം req_sacc{0.5f};
		ദശാംശം req_hdrift{0.1f};
		ദശാംശം req_vdrift{0.2f};
		int32_t req_fix{3};
		uint64_t min_health_time_us{10'000'000};
	};

	ശൂന്യം setParams(സ്ഥിരം Params &params) { _params = params; }

	/*
	 * Return true if the sample may be used. The strict checks (the GNSS_REQ_* thresholds, satellite count, PDOP, and
	 * drift at rest on the ground) decide until they pass and again whenever the vehicle is disarmed on the ground. The
	 * relaxed checks (fixed, looser limits) decide otherwise while the strict ones fail. Both run on every sample.
	*/
	തർക്കം run(സ്ഥിരം gnssChecksSample &gnss, തർക്കം armed, തർക്കം in_air, തർക്കം vehicle_at_rest);
	തർക്കം passed() സ്ഥിരം { തിരികെ _passed; }

	// The strict checks decided the last run: they haven't passed since the vehicle was disarmed on the ground, or they
	// pass
	തർക്കം strict() സ്ഥിരം { തിരികെ _strict; }

	// The last sample passed the strict checks, and no check that decided passed() failed for the required pass
	// duration. Once the relaxed checks decide, a sample that fails only the strict ones doesn't restart that duration,
	// so that a single one doesn't drop the receiver's rank for GNSS_REQ_TIME: the selection's own hold rides through it.
	തർക്കം passedStrict() സ്ഥിരം { തിരികെ _passed_strict; }

	// The enabled checks the last sample failed at the strict thresholds, as vehicle_gnss_s::CHECK_* bits
	uint16_t getStrictFailFlags() സ്ഥിരം { തിരികെ _strict_fail_flags & getEnabledChecks(); }

	// The enabled checks the last sample failed at the relaxed thresholds, as vehicle_gnss_s::CHECK_* bits
	uint16_t getRelaxedFailFlags() സ്ഥിരം { തിരികെ _relaxed_fail_flags & getEnabledChecks(); }

	// The checks enabled in GNSS_CHECK, as vehicle_gnss_s::CHECK_* bits
	uint16_t getEnabledChecks() സ്ഥിരം { തിരികെ static_cast<uint16_t>(_params.check_mask) & kAllChecks; }

	ദശാംശം horizontal_position_drift_rate_m_s() സ്ഥിരം { തിരികെ _horizontal_position_drift_rate_m_s; }
	ദശാംശം vertical_position_drift_rate_m_s() സ്ഥിരം { തിരികെ _vertical_position_drift_rate_m_s; }
	ദശാംശം filtered_horizontal_velocity_m_s() സ്ഥിരം { തിരികെ _filtered_horizontal_velocity_m_s; }

സ്വകാര്യ:
	സ്ഥിതം constexpr uint16_t kAllChecks = vehicle_gnss_s::CHECK_NSATS | vehicle_gnss_s::CHECK_PDOP | vehicle_gnss_s::CHECK_EPH
					       | vehicle_gnss_s::CHECK_EPV | vehicle_gnss_s::CHECK_SACC | vehicle_gnss_s::CHECK_HDRIFT
					       | vehicle_gnss_s::CHECK_VDRIFT | vehicle_gnss_s::CHECK_HSPEED | vehicle_gnss_s::CHECK_VSPEED
					       | vehicle_gnss_s::CHECK_SPOOFED | vehicle_gnss_s::CHECK_FIX | vehicle_gnss_s::CHECK_JAMMED;

	// A receiver that has not passed for this long qualifies from scratch again, as after a failure. 7 s is the
	// outage after which the EKF stops using GNSS, and after which it used to reset these checks.
	സ്ഥിതം constexpr uint64_t kPassTimeoutUs = 7'000'000;

	ശൂന്യം reset()
	{
		_passed = അസത്യം;
		_time_last_pass_us = 0;
		_time_last_fail_us = 0;
		resetDriftFilters();
	}

	// How long the checks must pass after a failure before passed() is true
	uint64_t getRequiredPassDurationUs(തർക്കം relaxed) സ്ഥിരം
	{
		തിരികെ relaxed ? math::max((uint64_t)1e6, (uint64_t)_params.min_health_time_us / 10)
		       : (uint64_t)_params.min_health_time_us;
	}

	സ്ഥിതം ശൂന്യം setFail(uint16_t &fail_flags, uint16_t check, തർക്കം failed);
	തർക്കം enabledChecksPass(uint16_t fail_flags, uint16_t checks) സ്ഥിരം { തിരികെ (fail_flags & checks & getEnabledChecks()) == 0; }

	തർക്കം runRelaxedChecks(സ്ഥിരം gnssChecksSample &gnss);
	തർക്കം runStrictChecks(സ്ഥിരം gnssChecksSample &gnss, തർക്കം in_air, തർക്കം vehicle_at_rest);
	ശൂന്യം runOnGroundGnssChecks(സ്ഥിരം gnssChecksSample &gnss, തർക്കം in_air, തർക്കം vehicle_at_rest);

	ശൂന്യം clearDriftChecks();
	ശൂന്യം resetDriftFilters();

	തർക്കം isTimedOut(uint64_t timestamp_to_check_us, uint64_t now_us, uint64_t timeout_period) സ്ഥിരം
	{
		തിരികെ (timestamp_to_check_us == 0) || (timestamp_to_check_us + timeout_period < now_us);
	}

	uint16_t _strict_fail_flags{0};
	uint16_t _relaxed_fail_flags{0};

	ദശാംശം _horizontal_position_drift_rate_m_s{NAN};
	ദശാംശം _vertical_position_drift_rate_m_s{NAN};
	ദശാംശം _filtered_horizontal_velocity_m_s{NAN};

	MapProjection lat_lon_prev{};
	ദശാംശം _alt_prev{0.0f};

	matrix::Vector3f _lat_lon_alt_deriv_filt{};
	matrix::Vector2f _vel_ne_filt{};

	ദശാംശം _vel_d_filt{0.0f};		///< GNSS filtered Down velocity (m/sec)
	uint64_t _time_last_fail_us{0};
	uint64_t _time_last_pass_us{0};
	തർക്കം _initial_strict_checks_passed{അസത്യം};
	തർക്കം _strict{സത്യം};
	തർക്കം _passed{അസത്യം};
	തർക്കം _passed_strict{അസത്യം};

	Params _params{};
};

#endif // !GNSS_CHECKS_H
