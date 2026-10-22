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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ GNSS_CHECKS_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GNSS_CHECKS_H

#ਅਨ੍ਤਰ੍ਭਾਵ <lib/geo/geo.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/mathlib/mathlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/matrix/matrix/math.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/vehicle_gnss.h>

ਬਣਤਰ gnssChecksSample {
	uint64_t ਸਮਾਂ_us{};     ///< measurement time (us)
	ਦੁੱਗਣਾ lat{};           ///< latitude (deg)
	ਦੁੱਗਣਾ lon{};           ///< longitude (deg)
	ਦਸ਼ਮਲਵ alt{};            ///< altitude above MSL (m)
	ਮੈਟ੍ਰਿਕਸ::Vector3f vel{}; ///< NED velocity (m/s)
	ਦਸ਼ਮਲਵ hacc{};           ///< 1-std horizontal position error (m)
	ਦਸ਼ਮਲਵ vacc{};           ///< 1-std vertical position error (m)
	ਦਸ਼ਮਲਵ sacc{};           ///< 1-std speed error (m/s)
	uint8_t fix_type{};     ///< 0-1: no fix, 2: 2D fix, 3: 3D fix, 4: RTCM code differential, 5: RTK float, 6: RTK fixed
	uint8_t nsats{};        ///< number of satellites used
	ਦਸ਼ਮਲਵ pdop{};           ///< position dilution of precision
	ਤਾਰਕਿਕ spoofed{};         ///< the receiver reports spoofing
	ਤਾਰਕਿਕ jammed{};          ///< the receiver reports jamming
};

ਵਰਗ GnssChecks final
{
ਜਨਤਕ:
	ਬਣਤਰ Params {
		int32_t check_ਨਕਾਬ{2047};
		int32_t req_nsats{6};
		ਦਸ਼ਮਲਵ req_pdop{2.5f};
		ਦਸ਼ਮਲਵ req_eph{3.f};
		ਦਸ਼ਮਲਵ req_epv{5.f};
		ਦਸ਼ਮਲਵ req_sacc{0.5f};
		ਦਸ਼ਮਲਵ req_hdrift{0.1f};
		ਦਸ਼ਮਲਵ req_vdrift{0.2f};
		int32_t req_fix{3};
		uint64_t min_health_ਸਮਾਂ_us{10'000'000};
	};

	ਖਾਲੀ ਸੈੱਟ_Params(ਅਚਲ Params &params) { _params = params; }

	/*
	 * Return true if the sample may be used. The strict checks (the GNSS_REQ_* thresholds, satellite count, PDOP, and
	 * drift at rest on the ground) decide until they pass and again whenever the vehicle is disarmed on the ground. The
	 * relaxed checks (fixed, looser limits) decide otherwise while the strict ones fail. Both run on every sample.
	*/
	ਤਾਰਕਿਕ ਲੜੀਵਾਰ_2(ਅਚਲ gnssChecksSample &gnss, ਤਾਰਕਿਕ armed, ਤਾਰਕਿਕ ਵਿੱਚ_air, ਤਾਰਕਿਕ vehicle_at_rest);
	ਤਾਰਕਿਕ passed() ਅਚਲ { ਵਾਪਸ _passed; }

	// The strict checks decided the last run: they haven't passed since the vehicle was disarmed on the ground, or they
	// pass
	ਤਾਰਕਿਕ strict() ਅਚਲ { ਵਾਪਸ _strict; }

	// The last sample passed the strict checks, and no check that decided passed() failed for the required pass
	// duration. Once the relaxed checks decide, a sample that fails only the strict ones doesn't restart that duration,
	// so that a single one doesn't drop the receiver's rank for GNSS_REQ_TIME: the selection's own hold rides through it.
	ਤਾਰਕਿਕ passedStrict() ਅਚਲ { ਵਾਪਸ _passed_strict; }

	// The enabled checks the last sample failed at the strict thresholds, as vehicle_gnss_s::CHECK_* bits
	uint16_t getStrictFailFlags() ਅਚਲ { ਵਾਪਸ _strict_fail_flags & getEnabledChecks(); }

	// The enabled checks the last sample failed at the relaxed thresholds, as vehicle_gnss_s::CHECK_* bits
	uint16_t getRelaxedFailFlags() ਅਚਲ { ਵਾਪਸ _relaxed_fail_flags & getEnabledChecks(); }

	// The checks enabled in GNSS_CHECK, as vehicle_gnss_s::CHECK_* bits
	uint16_t getEnabledChecks() ਅਚਲ { ਵਾਪਸ static_cast<uint16_t>(_params.check_ਨਕਾਬ) & kAllChecks; }

	ਦਸ਼ਮਲਵ horizontal_position_drift_ਦਰ_m_s_2() ਅਚਲ { ਵਾਪਸ horizontal_position_drift_ਦਰ_m_s; }
	ਦਸ਼ਮਲਵ vertical_position_drift_ਦਰ_m_s_2() ਅਚਲ { ਵਾਪਸ vertical_position_drift_ਦਰ_m_s; }
	ਦਸ਼ਮਲਵ filtered_horizontal_velocity_m_s() ਅਚਲ { ਵਾਪਸ _filtered_horizontal_velocity_m_s; }

ਨਿਜੀ:
	ਸਥਿਰ constexpr uint16_t kAllChecks = vehicle_gnss_s::CHECK_NSATS | vehicle_gnss_s::CHECK_PDOP | vehicle_gnss_s::CHECK_EPH
					       | vehicle_gnss_s::CHECK_EPV | vehicle_gnss_s::CHECK_SACC | vehicle_gnss_s::CHECK_HDRIFT
					       | vehicle_gnss_s::CHECK_VDRIFT | vehicle_gnss_s::CHECK_HSPEED | vehicle_gnss_s::CHECK_VSPEED
					       | vehicle_gnss_s::CHECK_SPOOFED | vehicle_gnss_s::CHECK_FIX | vehicle_gnss_s::CHECK_JAMMED;

	// A receiver that has not passed for this long qualifies from scratch again, as after a failure. 7 s is the
	// outage after which the EKF stops using GNSS, and after which it used to reset these checks.
	ਸਥਿਰ constexpr uint64_t kPassTimeoutUs = 7'000'000;

	ਖਾਲੀ reset()
	{
		_passed = ਝੂਠ;
		ਸਮਾਂ_ਆਖਰੀ_pass_us = 0;
		ਸਮਾਂ_ਆਖਰੀ_fail_us = 0;
		resetDriftFilters();
	}

	// How long the checks must pass after a failure before passed() is true
	uint64_t getRequiredPassDurationUs(ਤਾਰਕਿਕ relaxed) ਅਚਲ
	{
		ਵਾਪਸ relaxed ? math::max((uint64_t)1e6, (uint64_t)_params.min_health_ਸਮਾਂ_us / 10)
		       : (uint64_t)_params.min_health_ਸਮਾਂ_us;
	}

	ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_Fail(uint16_t &fail_flags, uint16_t check, ਤਾਰਕਿਕ failed);
	ਤਾਰਕਿਕ enabledChecksPass(uint16_t fail_flags, uint16_t checks) ਅਚਲ { ਵਾਪਸ (fail_flags & checks & getEnabledChecks()) == 0; }

	ਤਾਰਕਿਕ ਲੜੀਵਾਰ_Relaxed_Checks(ਅਚਲ gnssChecksSample &gnss);
	ਤਾਰਕਿਕ ਲੜੀਵਾਰ_Strict_Checks(ਅਚਲ gnssChecksSample &gnss, ਤਾਰਕਿਕ ਵਿੱਚ_air, ਤਾਰਕਿਕ vehicle_at_rest);
	ਖਾਲੀ ਲੜੀਵਾਰ_On_Ground_Gnss_Checks(ਅਚਲ gnssChecksSample &gnss, ਤਾਰਕਿਕ ਵਿੱਚ_air, ਤਾਰਕਿਕ vehicle_at_rest);

	ਖਾਲੀ clearDriftChecks();
	ਖਾਲੀ resetDriftFilters();

	ਤਾਰਕਿਕ is_Timed_ਬਾਹਰਲਾ(uint64_t timestamp_ਮੰਜ਼ਿਲ_check_us, uint64_t now_us, uint64_t timeout_period) ਅਚਲ
	{
		ਵਾਪਸ (timestamp_ਮੰਜ਼ਿਲ_check_us == 0) || (timestamp_ਮੰਜ਼ਿਲ_check_us + timeout_period < now_us);
	}

	uint16_t _strict_fail_flags{0};
	uint16_t _relaxed_fail_flags{0};

	ਦਸ਼ਮਲਵ horizontal_position_drift_ਦਰ_m_s{NAN};
	ਦਸ਼ਮਲਵ vertical_position_drift_ਦਰ_m_s{NAN};
	ਦਸ਼ਮਲਵ _filtered_horizontal_velocity_m_s{NAN};

	MapProjection lat_lon_prev{};
	ਦਸ਼ਮਲਵ _alt_prev{0.0f};

	ਮੈਟ੍ਰਿਕਸ::Vector3f _lat_lon_alt_deriv_filt{};
	ਮੈਟ੍ਰਿਕਸ::Vector2f _vel_ne_filt{};

	ਦਸ਼ਮਲਵ _vel_d_filt{0.0f};		///< GNSS filtered Down velocity (m/sec)
	uint64_t ਸਮਾਂ_ਆਖਰੀ_fail_us{0};
	uint64_t ਸਮਾਂ_ਆਖਰੀ_pass_us{0};
	ਤਾਰਕਿਕ _initial_strict_checks_passed{ਝੂਠ};
	ਤਾਰਕਿਕ _strict{ਸੱਚ};
	ਤਾਰਕਿਕ _passed{ਝੂਠ};
	ਤਾਰਕਿਕ _passed_strict{ਝੂਠ};

	Params _params{};
};

#ਜੇ_ਅੰਤ // !GNSS_CHECKS_H
