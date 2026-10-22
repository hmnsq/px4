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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ GNSS_CHECKS_H
#ವ್ಯಾಖ್ಯಾನಿಸು GNSS_CHECKS_H

#ಅನ್ತರ್ಭಾವ <lib/geo/geo.h>
#ಅನ್ತರ್ಭಾವ <lib/mathlib/mathlib.h>
#ಅನ್ತರ್ಭಾವ <lib/matrix/matrix/math.hpp>
#ಅನ್ತರ್ಭಾವ <uORB/topics/vehicle_gnss.h>

ರಚನೆ gnssChecksSample {
	uint64_t ಸಮಯ_us{};     ///< measurement time (us)
	ದ್ವಿಗುಣ lat{};           ///< latitude (deg)
	ದ್ವಿಗುಣ lon{};           ///< longitude (deg)
	ದಶಮಾಂಶ alt{};            ///< altitude above MSL (m)
	ಮಾತೃಕೆ::Vector3f vel{}; ///< NED velocity (m/s)
	ದಶಮಾಂಶ hacc{};           ///< 1-std horizontal position error (m)
	ದಶಮಾಂಶ vacc{};           ///< 1-std vertical position error (m)
	ದಶಮಾಂಶ sacc{};           ///< 1-std speed error (m/s)
	uint8_t fix_type{};     ///< 0-1: no fix, 2: 2D fix, 3: 3D fix, 4: RTCM code differential, 5: RTK float, 6: RTK fixed
	uint8_t nsats{};        ///< number of satellites used
	ದಶಮಾಂಶ pdop{};           ///< position dilution of precision
	ತಾರ್ಕಿಕ spoofed{};         ///< the receiver reports spoofing
	ತಾರ್ಕಿಕ jammed{};          ///< the receiver reports jamming
};

ವರ್ಗ GnssChecks final
{
ಸಾರ್ವಜನಿಕ:
	ರಚನೆ Params {
		int32_t check_ಮುಖವಾಡ{2047};
		int32_t req_nsats{6};
		ದಶಮಾಂಶ req_pdop{2.5f};
		ದಶಮಾಂಶ req_eph{3.f};
		ದಶಮಾಂಶ req_epv{5.f};
		ದಶಮಾಂಶ req_sacc{0.5f};
		ದಶಮಾಂಶ req_hdrift{0.1f};
		ದಶಮಾಂಶ req_vdrift{0.2f};
		int32_t req_fix{3};
		uint64_t min_health_ಸಮಯ_us{10'000'000};
	};

	ಶೂನ್ಯ ಗಣ_Params(ಸ್ಥಿರಾಂಕ Params &params) { _params = params; }

	/*
	 * Return true if the sample may be used. The strict checks (the GNSS_REQ_* thresholds, satellite count, PDOP, and
	 * drift at rest on the ground) decide until they pass and again whenever the vehicle is disarmed on the ground. The
	 * relaxed checks (fixed, looser limits) decide otherwise while the strict ones fail. Both run on every sample.
	*/
	ತಾರ್ಕಿಕ ಸತತ_2(ಸ್ಥಿರಾಂಕ gnssChecksSample &gnss, ತಾರ್ಕಿಕ armed, ತಾರ್ಕಿಕ ಒಳ_air, ತಾರ್ಕಿಕ vehicle_at_rest);
	ತಾರ್ಕಿಕ passed() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _passed; }

	// The strict checks decided the last run: they haven't passed since the vehicle was disarmed on the ground, or they
	// pass
	ತಾರ್ಕಿಕ strict() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _strict; }

	// The last sample passed the strict checks, and no check that decided passed() failed for the required pass
	// duration. Once the relaxed checks decide, a sample that fails only the strict ones doesn't restart that duration,
	// so that a single one doesn't drop the receiver's rank for GNSS_REQ_TIME: the selection's own hold rides through it.
	ತಾರ್ಕಿಕ passedStrict() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _passed_strict; }

	// The enabled checks the last sample failed at the strict thresholds, as vehicle_gnss_s::CHECK_* bits
	uint16_t getStrictFailFlags() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _strict_fail_flags & getEnabledChecks(); }

	// The enabled checks the last sample failed at the relaxed thresholds, as vehicle_gnss_s::CHECK_* bits
	uint16_t getRelaxedFailFlags() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _relaxed_fail_flags & getEnabledChecks(); }

	// The checks enabled in GNSS_CHECK, as vehicle_gnss_s::CHECK_* bits
	uint16_t getEnabledChecks() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ static_cast<uint16_t>(_params.check_ಮುಖವಾಡ) & kAllChecks; }

	ದಶಮಾಂಶ horizontal_position_drift_ದರ_m_s_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ horizontal_position_drift_ದರ_m_s; }
	ದಶಮಾಂಶ vertical_position_drift_ದರ_m_s_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ vertical_position_drift_ದರ_m_s; }
	ದಶಮಾಂಶ filtered_horizontal_velocity_m_s() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _filtered_horizontal_velocity_m_s; }

ಖಾಸಗಿ:
	ಸ್ಥಿರ constexpr uint16_t kAllChecks = vehicle_gnss_s::CHECK_NSATS | vehicle_gnss_s::CHECK_PDOP | vehicle_gnss_s::CHECK_EPH
					       | vehicle_gnss_s::CHECK_EPV | vehicle_gnss_s::CHECK_SACC | vehicle_gnss_s::CHECK_HDRIFT
					       | vehicle_gnss_s::CHECK_VDRIFT | vehicle_gnss_s::CHECK_HSPEED | vehicle_gnss_s::CHECK_VSPEED
					       | vehicle_gnss_s::CHECK_SPOOFED | vehicle_gnss_s::CHECK_FIX | vehicle_gnss_s::CHECK_JAMMED;

	// A receiver that has not passed for this long qualifies from scratch again, as after a failure. 7 s is the
	// outage after which the EKF stops using GNSS, and after which it used to reset these checks.
	ಸ್ಥಿರ constexpr uint64_t kPassTimeoutUs = 7'000'000;

	ಶೂನ್ಯ reset()
	{
		_passed = ಸುಳ್ಳು;
		ಸಮಯ_ಕೊನೆಯ_pass_us = 0;
		ಸಮಯ_ಕೊನೆಯ_fail_us = 0;
		resetDriftFilters();
	}

	// How long the checks must pass after a failure before passed() is true
	uint64_t getRequiredPassDurationUs(ತಾರ್ಕಿಕ relaxed) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ relaxed ? math::max((uint64_t)1e6, (uint64_t)_params.min_health_ಸಮಯ_us / 10)
		       : (uint64_t)_params.min_health_ಸಮಯ_us;
	}

	ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_Fail(uint16_t &fail_flags, uint16_t check, ತಾರ್ಕಿಕ failed);
	ತಾರ್ಕಿಕ enabledChecksPass(uint16_t fail_flags, uint16_t checks) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ (fail_flags & checks & getEnabledChecks()) == 0; }

	ತಾರ್ಕಿಕ ಸತತ_Relaxed_Checks(ಸ್ಥಿರಾಂಕ gnssChecksSample &gnss);
	ತಾರ್ಕಿಕ ಸತತ_Strict_Checks(ಸ್ಥಿರಾಂಕ gnssChecksSample &gnss, ತಾರ್ಕಿಕ ಒಳ_air, ತಾರ್ಕಿಕ vehicle_at_rest);
	ಶೂನ್ಯ ಸತತ_On_Ground_Gnss_Checks(ಸ್ಥಿರಾಂಕ gnssChecksSample &gnss, ತಾರ್ಕಿಕ ಒಳ_air, ತಾರ್ಕಿಕ vehicle_at_rest);

	ಶೂನ್ಯ clearDriftChecks();
	ಶೂನ್ಯ resetDriftFilters();

	ತಾರ್ಕಿಕ is_Timed_ಹೊರಹೋಗು(uint64_t timestamp_ಗಮ್ಯ_check_us, uint64_t now_us, uint64_t timeout_period) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ (timestamp_ಗಮ್ಯ_check_us == 0) || (timestamp_ಗಮ್ಯ_check_us + timeout_period < now_us);
	}

	uint16_t _strict_fail_flags{0};
	uint16_t _relaxed_fail_flags{0};

	ದಶಮಾಂಶ horizontal_position_drift_ದರ_m_s{NAN};
	ದಶಮಾಂಶ vertical_position_drift_ದರ_m_s{NAN};
	ದಶಮಾಂಶ _filtered_horizontal_velocity_m_s{NAN};

	MapProjection lat_lon_prev{};
	ದಶಮಾಂಶ _alt_prev{0.0f};

	ಮಾತೃಕೆ::Vector3f _lat_lon_alt_deriv_filt{};
	ಮಾತೃಕೆ::Vector2f _vel_ne_filt{};

	ದಶಮಾಂಶ _vel_d_filt{0.0f};		///< GNSS filtered Down velocity (m/sec)
	uint64_t ಸಮಯ_ಕೊನೆಯ_fail_us{0};
	uint64_t ಸಮಯ_ಕೊನೆಯ_pass_us{0};
	ತಾರ್ಕಿಕ _initial_strict_checks_passed{ಸುಳ್ಳು};
	ತಾರ್ಕಿಕ _strict{ನಿಜ};
	ತಾರ್ಕಿಕ _passed{ಸುಳ್ಳು};
	ತಾರ್ಕಿಕ _passed_strict{ಸುಳ್ಳು};

	Params _params{};
};

#ಆದರೆ_ಅಂತ್ಯ // !GNSS_CHECKS_H
