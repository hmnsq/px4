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

#ضمن <lib/geo/geo.h>
#ضمن <lib/mathlib/mathlib.h>
#ضمن <lib/matrix/matrix/math.hpp>
#ضمن <uORB/topics/vehicle_gnss.h>

بنية gnssChecksSample {
	uint64_t time_us{};     ///< measurement time (us)
	مزدوج lat{};           ///< latitude (deg)
	مزدوج lon{};           ///< longitude (deg)
	عائم alt{};            ///< altitude above MSL (m)
	matrix::Vector3f vel{}; ///< NED velocity (m/s)
	عائم hacc{};           ///< 1-std horizontal position error (m)
	عائم vacc{};           ///< 1-std vertical position error (m)
	عائم sacc{};           ///< 1-std speed error (m/s)
	uint8_t fix_type{};     ///< 0-1: no fix, 2: 2D fix, 3: 3D fix, 4: RTCM code differential, 5: RTK float, 6: RTK fixed
	uint8_t nsats{};        ///< number of satellites used
	عائم pdop{};           ///< position dilution of precision
	منطقي spoofed{};         ///< the receiver reports spoofing
	منطقي jammed{};          ///< the receiver reports jamming
};

صنف GnssChecks final
{
عام:
	بنية Params {
		int32_t check_mask{2047};
		int32_t req_nsats{6};
		عائم req_pdop{2.5f};
		عائم req_eph{3.f};
		عائم req_epv{5.f};
		عائم req_sacc{0.5f};
		عائم req_hdrift{0.1f};
		عائم req_vdrift{0.2f};
		int32_t req_fix{3};
		uint64_t min_health_time_us{10'000'000};
	};

	فراغ setParams(ثابت Params &params) { _params = params; }

	/*
	 * Return true if the sample may be used. The strict checks (the GNSS_REQ_* thresholds, satellite count, PDOP, and
	 * drift at rest on the ground) decide until they pass and again whenever the vehicle is disarmed on the ground. The
	 * relaxed checks (fixed, looser limits) decide otherwise while the strict ones fail. Both run on every sample.
	*/
	منطقي run(ثابت gnssChecksSample &gnss, منطقي armed, منطقي in_air, منطقي vehicle_at_rest);
	منطقي passed() ثابت { ܦܢܐ _passed; }

	// The strict checks decided the last run: they haven't passed since the vehicle was disarmed on the ground, or they
	// pass
	منطقي strict() ثابت { ܦܢܐ _strict; }

	// The last sample passed the strict checks, and no check that decided passed() failed for the required pass
	// duration. Once the relaxed checks decide, a sample that fails only the strict ones doesn't restart that duration,
	// so that a single one doesn't drop the receiver's rank for GNSS_REQ_TIME: the selection's own hold rides through it.
	منطقي passedStrict() ثابت { ܦܢܐ _passed_strict; }

	// The enabled checks the last sample failed at the strict thresholds, as vehicle_gnss_s::CHECK_* bits
	uint16_t getStrictFailFlags() ثابت { ܦܢܐ _strict_fail_flags & getEnabledChecks(); }

	// The enabled checks the last sample failed at the relaxed thresholds, as vehicle_gnss_s::CHECK_* bits
	uint16_t getRelaxedFailFlags() ثابت { ܦܢܐ _relaxed_fail_flags & getEnabledChecks(); }

	// The checks enabled in GNSS_CHECK, as vehicle_gnss_s::CHECK_* bits
	uint16_t getEnabledChecks() ثابت { ܦܢܐ static_cast<uint16_t>(_params.check_mask) & kAllChecks; }

	عائم horizontal_position_drift_rate_m_s() ثابت { ܦܢܐ _horizontal_position_drift_rate_m_s; }
	عائم vertical_position_drift_rate_m_s() ثابت { ܦܢܐ _vertical_position_drift_rate_m_s; }
	عائم filtered_horizontal_velocity_m_s() ثابت { ܦܢܐ _filtered_horizontal_velocity_m_s; }

خاص:
	ساكن constexpr uint16_t kAllChecks = vehicle_gnss_s::CHECK_NSATS | vehicle_gnss_s::CHECK_PDOP | vehicle_gnss_s::CHECK_EPH
					       | vehicle_gnss_s::CHECK_EPV | vehicle_gnss_s::CHECK_SACC | vehicle_gnss_s::CHECK_HDRIFT
					       | vehicle_gnss_s::CHECK_VDRIFT | vehicle_gnss_s::CHECK_HSPEED | vehicle_gnss_s::CHECK_VSPEED
					       | vehicle_gnss_s::CHECK_SPOOFED | vehicle_gnss_s::CHECK_FIX | vehicle_gnss_s::CHECK_JAMMED;

	// A receiver that has not passed for this long qualifies from scratch again, as after a failure. 7 s is the
	// outage after which the EKF stops using GNSS, and after which it used to reset these checks.
	ساكن constexpr uint64_t kPassTimeoutUs = 7'000'000;

	فراغ reset()
	{
		_passed = ܕܓܠܐ;
		_time_last_pass_us = 0;
		_time_last_fail_us = 0;
		resetDriftFilters();
	}

	// How long the checks must pass after a failure before passed() is true
	uint64_t getRequiredPassDurationUs(منطقي relaxed) ثابت
	{
		ܦܢܐ relaxed ? math::max((uint64_t)1e6, (uint64_t)_params.min_health_time_us / 10)
		       : (uint64_t)_params.min_health_time_us;
	}

	ساكن فراغ setFail(uint16_t &fail_flags, uint16_t check, منطقي failed);
	منطقي enabledChecksPass(uint16_t fail_flags, uint16_t checks) ثابت { ܦܢܐ (fail_flags & checks & getEnabledChecks()) == 0; }

	منطقي runRelaxedChecks(ثابت gnssChecksSample &gnss);
	منطقي runStrictChecks(ثابت gnssChecksSample &gnss, منطقي in_air, منطقي vehicle_at_rest);
	فراغ runOnGroundGnssChecks(ثابت gnssChecksSample &gnss, منطقي in_air, منطقي vehicle_at_rest);

	فراغ clearDriftChecks();
	فراغ resetDriftFilters();

	منطقي isTimedOut(uint64_t timestamp_to_check_us, uint64_t now_us, uint64_t timeout_period) ثابت
	{
		ܦܢܐ (timestamp_to_check_us == 0) || (timestamp_to_check_us + timeout_period < now_us);
	}

	uint16_t _strict_fail_flags{0};
	uint16_t _relaxed_fail_flags{0};

	عائم _horizontal_position_drift_rate_m_s{NAN};
	عائم _vertical_position_drift_rate_m_s{NAN};
	عائم _filtered_horizontal_velocity_m_s{NAN};

	MapProjection lat_lon_prev{};
	عائم _alt_prev{0.0f};

	matrix::Vector3f _lat_lon_alt_deriv_filt{};
	matrix::Vector2f _vel_ne_filt{};

	عائم _vel_d_filt{0.0f};		///< GNSS filtered Down velocity (m/sec)
	uint64_t _time_last_fail_us{0};
	uint64_t _time_last_pass_us{0};
	منطقي _initial_strict_checks_passed{ܕܓܠܐ};
	منطقي _strict{ܫܪܪܐ};
	منطقي _passed{ܕܓܠܐ};
	منطقي _passed_strict{ܕܓܠܐ};

	Params _params{};
};

#endif // !GNSS_CHECKS_H
