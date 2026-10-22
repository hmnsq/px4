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

#অন্তর্ভাব <lib/geo/geo.h>
#অন্তর্ভাব <lib/mathlib/mathlib.h>
#অন্তর্ভাব <lib/matrix/matrix/math.hpp>
#অন্তর্ভাব <uORB/topics/vehicle_gnss.h>

কাঠামো gnssChecksSample {
	uint64_t time_us{};     ///< measurement time (us)
	দ্বিগুণ lat{};           ///< latitude (deg)
	দ্বিগুণ lon{};           ///< longitude (deg)
	দশমিক alt{};            ///< altitude above MSL (m)
	matrix::Vector3f vel{}; ///< NED velocity (m/s)
	দশমিক hacc{};           ///< 1-std horizontal position error (m)
	দশমিক vacc{};           ///< 1-std vertical position error (m)
	দশমিক sacc{};           ///< 1-std speed error (m/s)
	uint8_t fix_type{};     ///< 0-1: no fix, 2: 2D fix, 3: 3D fix, 4: RTCM code differential, 5: RTK float, 6: RTK fixed
	uint8_t nsats{};        ///< number of satellites used
	দশমিক pdop{};           ///< position dilution of precision
	যৌক্তিক spoofed{};         ///< the receiver reports spoofing
	যৌক্তিক jammed{};          ///< the receiver reports jamming
};

শ্রেণী GnssChecks final
{
সর্বজনীন:
	কাঠামো Params {
		int32_t check_mask{2047};
		int32_t req_nsats{6};
		দশমিক req_pdop{2.5f};
		দশমিক req_eph{3.f};
		দশমিক req_epv{5.f};
		দশমিক req_sacc{0.5f};
		দশমিক req_hdrift{0.1f};
		দশমিক req_vdrift{0.2f};
		int32_t req_fix{3};
		uint64_t min_health_time_us{10'000'000};
	};

	শূন্য setParams(ধ্রুবক Params &params) { _params = params; }

	/*
	 * Return true if the sample may be used. The strict checks (the GNSS_REQ_* thresholds, satellite count, PDOP, and
	 * drift at rest on the ground) decide until they pass and again whenever the vehicle is disarmed on the ground. The
	 * relaxed checks (fixed, looser limits) decide otherwise while the strict ones fail. Both run on every sample.
	*/
	যৌক্তিক run(ধ্রুবক gnssChecksSample &gnss, যৌক্তিক armed, যৌক্তিক in_air, যৌক্তিক vehicle_at_rest);
	যৌক্তিক passed() ধ্রুবক { ফেরত _passed; }

	// The strict checks decided the last run: they haven't passed since the vehicle was disarmed on the ground, or they
	// pass
	যৌক্তিক strict() ধ্রুবক { ফেরত _strict; }

	// The last sample passed the strict checks, and no check that decided passed() failed for the required pass
	// duration. Once the relaxed checks decide, a sample that fails only the strict ones doesn't restart that duration,
	// so that a single one doesn't drop the receiver's rank for GNSS_REQ_TIME: the selection's own hold rides through it.
	যৌক্তিক passedStrict() ধ্রুবক { ফেরত _passed_strict; }

	// The enabled checks the last sample failed at the strict thresholds, as vehicle_gnss_s::CHECK_* bits
	uint16_t getStrictFailFlags() ধ্রুবক { ফেরত _strict_fail_flags & getEnabledChecks(); }

	// The enabled checks the last sample failed at the relaxed thresholds, as vehicle_gnss_s::CHECK_* bits
	uint16_t getRelaxedFailFlags() ধ্রুবক { ফেরত _relaxed_fail_flags & getEnabledChecks(); }

	// The checks enabled in GNSS_CHECK, as vehicle_gnss_s::CHECK_* bits
	uint16_t getEnabledChecks() ধ্রুবক { ফেরত static_cast<uint16_t>(_params.check_mask) & kAllChecks; }

	দশমিক horizontal_position_drift_rate_m_s() ধ্রুবক { ফেরত _horizontal_position_drift_rate_m_s; }
	দশমিক vertical_position_drift_rate_m_s() ধ্রুবক { ফেরত _vertical_position_drift_rate_m_s; }
	দশমিক filtered_horizontal_velocity_m_s() ধ্রুবক { ফেরত _filtered_horizontal_velocity_m_s; }

ব্যক্তিগত:
	স্থির constexpr uint16_t kAllChecks = vehicle_gnss_s::CHECK_NSATS | vehicle_gnss_s::CHECK_PDOP | vehicle_gnss_s::CHECK_EPH
					       | vehicle_gnss_s::CHECK_EPV | vehicle_gnss_s::CHECK_SACC | vehicle_gnss_s::CHECK_HDRIFT
					       | vehicle_gnss_s::CHECK_VDRIFT | vehicle_gnss_s::CHECK_HSPEED | vehicle_gnss_s::CHECK_VSPEED
					       | vehicle_gnss_s::CHECK_SPOOFED | vehicle_gnss_s::CHECK_FIX | vehicle_gnss_s::CHECK_JAMMED;

	// A receiver that has not passed for this long qualifies from scratch again, as after a failure. 7 s is the
	// outage after which the EKF stops using GNSS, and after which it used to reset these checks.
	স্থির constexpr uint64_t kPassTimeoutUs = 7'000'000;

	শূন্য reset()
	{
		_passed = মিথ্যা;
		_time_last_pass_us = 0;
		_time_last_fail_us = 0;
		resetDriftFilters();
	}

	// How long the checks must pass after a failure before passed() is true
	uint64_t getRequiredPassDurationUs(যৌক্তিক relaxed) ধ্রুবক
	{
		ফেরত relaxed ? math::max((uint64_t)1e6, (uint64_t)_params.min_health_time_us / 10)
		       : (uint64_t)_params.min_health_time_us;
	}

	স্থির শূন্য setFail(uint16_t &fail_flags, uint16_t check, যৌক্তিক failed);
	যৌক্তিক enabledChecksPass(uint16_t fail_flags, uint16_t checks) ধ্রুবক { ফেরত (fail_flags & checks & getEnabledChecks()) == 0; }

	যৌক্তিক runRelaxedChecks(ধ্রুবক gnssChecksSample &gnss);
	যৌক্তিক runStrictChecks(ধ্রুবক gnssChecksSample &gnss, যৌক্তিক in_air, যৌক্তিক vehicle_at_rest);
	শূন্য runOnGroundGnssChecks(ধ্রুবক gnssChecksSample &gnss, যৌক্তিক in_air, যৌক্তিক vehicle_at_rest);

	শূন্য clearDriftChecks();
	শূন্য resetDriftFilters();

	যৌক্তিক isTimedOut(uint64_t timestamp_to_check_us, uint64_t now_us, uint64_t timeout_period) ধ্রুবক
	{
		ফেরত (timestamp_to_check_us == 0) || (timestamp_to_check_us + timeout_period < now_us);
	}

	uint16_t _strict_fail_flags{0};
	uint16_t _relaxed_fail_flags{0};

	দশমিক _horizontal_position_drift_rate_m_s{NAN};
	দশমিক _vertical_position_drift_rate_m_s{NAN};
	দশমিক _filtered_horizontal_velocity_m_s{NAN};

	MapProjection lat_lon_পূর্ব{};
	দশমিক alt_পূর্ব{0.0f};

	matrix::Vector3f _lat_lon_alt_deriv_filt{};
	matrix::Vector2f _vel_ne_filt{};

	দশমিক _vel_d_filt{0.0f};		///< GNSS filtered Down velocity (m/sec)
	uint64_t _time_last_fail_us{0};
	uint64_t _time_last_pass_us{0};
	যৌক্তিক _initial_strict_checks_passed{মিথ্যা};
	যৌক্তিক _strict{সত্য};
	যৌক্তিক _passed{মিথ্যা};
	যৌক্তিক _passed_strict{মিথ্যা};

	Params _params{};
};

#endif // !GNSS_CHECKS_H
