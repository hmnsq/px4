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

#شامل <lib/geo/geo.h>
#شامل <lib/mathlib/mathlib.h>
#شامل <lib/matrix/matrix/math.hpp>
#شامل <uORB/topics/vehicle_gnss.h>

ساختار gnssChecksSample {
	uint64_t time_us{};     ///< measurement time (us)
	دوگانه lat{};           ///< latitude (deg)
	دوگانه lon{};           ///< longitude (deg)
	اعشاری alt{};            ///< altitude above MSL (m)
	matrix::Vector3f vel{}; ///< NED velocity (m/s)
	اعشاری hacc{};           ///< 1-std horizontal position error (m)
	اعشاری vacc{};           ///< 1-std vertical position error (m)
	اعشاری sacc{};           ///< 1-std speed error (m/s)
	uint8_t fix_type{};     ///< 0-1: no fix, 2: 2D fix, 3: 3D fix, 4: RTCM code differential, 5: RTK float, 6: RTK fixed
	uint8_t nsats{};        ///< number of satellites used
	اعشاری pdop{};           ///< position dilution of precision
	منطقی spoofed{};         ///< the receiver reports spoofing
	منطقی jammed{};          ///< the receiver reports jamming
};

رده GnssChecks final
{
عمومی:
	ساختار Params {
		int32_t check_mask{2047};
		int32_t req_nsats{6};
		اعشاری req_pdop{2.5f};
		اعشاری req_eph{3.f};
		اعشاری req_epv{5.f};
		اعشاری req_sacc{0.5f};
		اعشاری req_hdrift{0.1f};
		اعشاری req_vdrift{0.2f};
		int32_t req_fix{3};
		uint64_t min_health_time_us{10'000'000};
	};

	تهی setParams(ثابت Params &params) { _params = params; }

	/*
	 * Return true if the sample may be used. The strict checks (the GNSS_REQ_* thresholds, satellite count, PDOP, and
	 * drift at rest on the ground) decide until they pass and again whenever the vehicle is disarmed on the ground. The
	 * relaxed checks (fixed, looser limits) decide otherwise while the strict ones fail. Both run on every sample.
	*/
	منطقی run(ثابت gnssChecksSample &gnss, منطقی armed, منطقی in_air, منطقی vehicle_at_rest);
	منطقی passed() ثابت { بازگشت _passed; }

	// The strict checks decided the last run: they haven't passed since the vehicle was disarmed on the ground, or they
	// pass
	منطقی strict() ثابت { بازگشت _strict; }

	// The last sample passed the strict checks, and no check that decided passed() failed for the required pass
	// duration. Once the relaxed checks decide, a sample that fails only the strict ones doesn't restart that duration,
	// so that a single one doesn't drop the receiver's rank for GNSS_REQ_TIME: the selection's own hold rides through it.
	منطقی passedStrict() ثابت { بازگشت _passed_strict; }

	// The enabled checks the last sample failed at the strict thresholds, as vehicle_gnss_s::CHECK_* bits
	uint16_t getStrictFailFlags() ثابت { بازگشت _strict_fail_flags & getEnabledChecks(); }

	// The enabled checks the last sample failed at the relaxed thresholds, as vehicle_gnss_s::CHECK_* bits
	uint16_t getRelaxedFailFlags() ثابت { بازگشت _relaxed_fail_flags & getEnabledChecks(); }

	// The checks enabled in GNSS_CHECK, as vehicle_gnss_s::CHECK_* bits
	uint16_t getEnabledChecks() ثابت { بازگشت static_cast<uint16_t>(_params.check_mask) & kAllChecks; }

	اعشاری horizontal_position_drift_rate_m_s() ثابت { بازگشت _horizontal_position_drift_rate_m_s; }
	اعشاری vertical_position_drift_rate_m_s() ثابت { بازگشت _vertical_position_drift_rate_m_s; }
	اعشاری filtered_horizontal_velocity_m_s() ثابت { بازگشت _filtered_horizontal_velocity_m_s; }

خصوصی:
	ایستا constexpr uint16_t kAllChecks = vehicle_gnss_s::CHECK_NSATS | vehicle_gnss_s::CHECK_PDOP | vehicle_gnss_s::CHECK_EPH
					       | vehicle_gnss_s::CHECK_EPV | vehicle_gnss_s::CHECK_SACC | vehicle_gnss_s::CHECK_HDRIFT
					       | vehicle_gnss_s::CHECK_VDRIFT | vehicle_gnss_s::CHECK_HSPEED | vehicle_gnss_s::CHECK_VSPEED
					       | vehicle_gnss_s::CHECK_SPOOFED | vehicle_gnss_s::CHECK_FIX | vehicle_gnss_s::CHECK_JAMMED;

	// A receiver that has not passed for this long qualifies from scratch again, as after a failure. 7 s is the
	// outage after which the EKF stops using GNSS, and after which it used to reset these checks.
	ایستا constexpr uint64_t kPassTimeoutUs = 7'000'000;

	تهی reset()
	{
		_passed = نادرست;
		_time_last_pass_us = 0;
		_time_last_fail_us = 0;
		resetDriftFilters();
	}

	// How long the checks must pass after a failure before passed() is true
	uint64_t getRequiredPassDurationUs(منطقی relaxed) ثابت
	{
		بازگشت relaxed ? math::max((uint64_t)1e6, (uint64_t)_params.min_health_time_us / 10)
		       : (uint64_t)_params.min_health_time_us;
	}

	ایستا تهی setFail(uint16_t &fail_flags, uint16_t check, منطقی failed);
	منطقی enabledChecksPass(uint16_t fail_flags, uint16_t checks) ثابت { بازگشت (fail_flags & checks & getEnabledChecks()) == 0; }

	منطقی runRelaxedChecks(ثابت gnssChecksSample &gnss);
	منطقی runStrictChecks(ثابت gnssChecksSample &gnss, منطقی in_air, منطقی vehicle_at_rest);
	تهی runOnGroundGnssChecks(ثابت gnssChecksSample &gnss, منطقی in_air, منطقی vehicle_at_rest);

	تهی clearDriftChecks();
	تهی resetDriftFilters();

	منطقی isTimedOut(uint64_t timestamp_to_check_us, uint64_t now_us, uint64_t timeout_period) ثابت
	{
		بازگشت (timestamp_to_check_us == 0) || (timestamp_to_check_us + timeout_period < now_us);
	}

	uint16_t _strict_fail_flags{0};
	uint16_t _relaxed_fail_flags{0};

	اعشاری _horizontal_position_drift_rate_m_s{NAN};
	اعشاری _vertical_position_drift_rate_m_s{NAN};
	اعشاری _filtered_horizontal_velocity_m_s{NAN};

	MapProjection lat_lon_prev{};
	اعشاری _alt_prev{0.0f};

	matrix::Vector3f _lat_lon_alt_deriv_filt{};
	matrix::Vector2f _vel_ne_filt{};

	اعشاری _vel_d_filt{0.0f};		///< GNSS filtered Down velocity (m/sec)
	uint64_t _time_last_fail_us{0};
	uint64_t _time_last_pass_us{0};
	منطقی _initial_strict_checks_passed{نادرست};
	منطقی _strict{درست};
	منطقی _passed{نادرست};
	منطقی _passed_strict{نادرست};

	Params _params{};
};

#endif // !GNSS_CHECKS_H
