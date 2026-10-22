/****************************************************************************
 *
 *   Copyright (c) 2026 PX4 Development Team. All rights reserved.
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
 * @file FailureInjection.hpp
 *
 * Shared helper for applying failure injection where a message is produced.
 *
 * The failure_injection manager publishes the deduplicated failure_injection
 * topic. Each producer caches it in a Config (update() per loop), looks up the
 * Mode for its (unit, instance), and applies it: process<MsgT>() covers the
 * generic Off (suppress) / Stuck (replay last sample) for single-message
 * producers, the message-less process() the payload-less Off-only units;
 * value-mutating and multi-instance cases are driver-specific.
 */

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <cstdint>

#ಅನ್ತರ್ಭಾವ <uORB/Subscription.hpp>
#ಅನ್ತರ್ಭಾವ <uORB/topics/esc_status.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/failure_injection.h>

ರಚನೆ battery_ಸ್ಥಿತಿ_s;
ರಚನೆ sensor_gnss_s;

ನಾಮಾಕಾಶ failure_injection
{

ಎಣಿಕೆ ವರ್ಗ ಬಹುಲಕ_2 : uint8_t {
	Ok           = failure_injection_s::FAILURE_TYPE_OK,
	Off          = failure_injection_s::FAILURE_TYPE_OFF,
	Stuck        = failure_injection_s::FAILURE_TYPE_STUCK,
	Garbage      = failure_injection_s::FAILURE_TYPE_GARBAGE,
	Wrong        = failure_injection_s::FAILURE_TYPE_WRONG,
	Slow         = failure_injection_s::FAILURE_TYPE_SLOW,
	Delayed      = failure_injection_s::FAILURE_TYPE_DELAYED,
	Intermittent = failure_injection_s::FAILURE_TYPE_INTERMITTENT,
};

/** Masks of motors failed by injection, bit i = motor instance i+1. */
ರಚನೆ MotorFailureMasks {
	uint16_t stop_ಮುಖವಾಡ{0};    ///< motor wrong: outputs stopped without informing the allocator
	uint16_t failure_ಮುಖವಾಡ{0}; ///< motor off: reported as failed motors, removed from the allocation
};

#ಆದರೆ defined(CONFIG_MODULES_FAILURE_INJECTION_MANAGER)

ವರ್ಗ Config
{
ಸಾರ್ವಜನಿಕ:
	/**
	 * Poll the failure_injection subscription and, when a new failure_injection
	 * message arrives, refresh the cached failure config that mode() / any_active()
	 * read. Call once per loop iteration.
	 * @return true if an updated failure config was applied, false if unchanged.
	 */
	ತಾರ್ಕಿಕ update();

	/** Cache a failure_injection message directly (used by tests). */
	ಶೂನ್ಯ ಗಣ(ಸ್ಥಿರಾಂಕ failure_injection_s &cfg);

	/**
	 * Active failure mode for a (unit, instance), or Mode::Ok if none.
	 * @param unit     one of failure_injection_s::FAILURE_UNIT_*
	 * @param instance 1-based instance, or 0 to match any instance of the unit
	 */
	ಬಹುಲಕ_2 ಬಹುಲಕ(uint8_t unit, uint8_t instance) ಸ್ಥಿರಾಂಕ;

	/** True if any failure is currently active (free no-failure early-out). */
	ತಾರ್ಕಿಕ any_ಸಕ್ರಿಯ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_4 > 0; }

ಖಾಸಗಿ:
	uORB::Subscription _sub{ORB_ID(failure_injection)};

	uint8_t  ಎಣಿಕೆ_4{0};
	uint8_t  _unit[failure_injection_s::MAX_FAILURES] {};
	uint16_t instance_ಮುಖವಾಡ[failure_injection_s::MAX_FAILURES] {};
	ಬಹುಲಕ_2     _failure_type[failure_injection_s::MAX_FAILURES] {};
};

/**
 * Per-(consumer, instance) state for stateful failures. Holds the last good
 * sample so Stuck can replay it.
 */
ಮಾದರಿ<ಬಗೆ_ಹೆಸರು ಸಂದೇಶ_T>
ರಚನೆ Stuck {
	ತಾರ್ಕಿಕ valid{ಸುಳ್ಳು};
	ಸಂದೇಶ_T value{};
};

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು...>
ಬಳಸಿ void_t = ಶೂನ್ಯ;

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T, ಬಗೆ_ಹೆಸರು = ಶೂನ್ಯ>
ರಚನೆ has_timestamp_sample {
	ಸ್ಥಿರ constexpr ತಾರ್ಕಿಕ value = ಸುಳ್ಳು;
};
ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T>
ರಚನೆ has_timestamp_sample<T, void_t<decltype(T::timestamp_sample)>> {
	ಸ್ಥಿರ constexpr ತಾರ್ಕಿಕ value = ನಿಜ;
};

/**
 * Generic whole-message processor for the Ok / Off / Stuck mechanics, for consumers
 * that publish a single message and want the message-agnostic behaviour.
 *
 * @return false if the message must be suppressed (Off); true otherwise. For
 *         Stuck the message is overwritten with the last good sample.
 */
ಮಾದರಿ<ಬಗೆ_ಹೆಸರು ಸಂದೇಶ_T>
ತಾರ್ಕಿಕ process(ಬಹುಲಕ_2 ಬಹುಲಕ, ಸಂದೇಶ_T &ಸಂದೇಶ_2, Stuck<ಸಂದೇಶ_T> &stuck)
{
	ಆಯ್ಕೆ (ಬಹುಲಕ) {
	ಸ್ಥಿತಿ ಬಹುಲಕ_2::Off:
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಸ್ಥಿತಿ ಬಹುಲಕ_2::Stuck:
		ಆದರೆ (stuck.valid) {
			ಸ್ಥಿರಾಂಕ uint64_t timestamp = ಸಂದೇಶ_2.timestamp;

			ಆದರೆ constexpr(has_timestamp_sample<ಸಂದೇಶ_T>::value) {
				ಸ್ಥಿರಾಂಕ uint64_t timestamp_sample = ಸಂದೇಶ_2.timestamp_sample;
				ಸಂದೇಶ_2 = stuck.value;
				ಸಂದೇಶ_2.timestamp_sample = timestamp_sample;

			} ಇಲ್ಲದಿದ್ದರೆ {
				ಸಂದೇಶ_2 = stuck.value;
			}

			ಸಂದೇಶ_2.timestamp = timestamp;
		}

		ಹಿಂತಿರುಗಿ ನಿಜ;

	ಪೂರ್ವನಿಯೋಜಿತ:
		stuck.value = ಸಂದೇಶ_2;
		stuck.valid = ನಿಜ;
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}
}

/**
 * Convenience overload to fix the fact that the uORB instance is 0-based but the failure_injection instance is 1-based.
 */
ಮಾದರಿ<ಬಗೆ_ಹೆಸರು ಸಂದೇಶ_T>
ತಾರ್ಕಿಕ process(ಸ್ಥಿರಾಂಕ Config &config, uint8_t unit, uint8_t uorb_instance, ಸಂದೇಶ_T &ಸಂದೇಶ_2, Stuck<ಸಂದೇಶ_T> &stuck)
{
	ಹಿಂತಿರುಗಿ process(config.ಬಹುಲಕ(unit, uorb_instance + 1), ಸಂದೇಶ_2, stuck);
}

/**
 * Message-less variant for producers with no payload to replay (e.g. a heartbeat)
 *
 * @return false if the signal must be treated as failed/suppressed (Off), true otherwise.
 */
ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ process(ಬಹುಲಕ_2 ಬಹುಲಕ)
{
	ಹಿಂತಿರುಗಿ ಬಹುಲಕ != ಬಹುಲಕ_2::Off;
}

/** Convenience overload, same 0-based to 1-based instance mapping as the generic process(). */
ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ process(ಸ್ಥಿರಾಂಕ Config &config, uint8_t unit, uint8_t uorb_instance)
{
	ಹಿಂತಿರುಗಿ process(config.ಬಹುಲಕ(unit, uorb_instance + 1));
}

/**
 * Battery counterpart to process(): on FAILURE_UNIT_SYSTEM_BATTERY for the given 1-based
 * instance, Off suppresses the publication (the pack reads disconnected) and Wrong reports the
 * warning level selected by SYS_FAIL_BAT_LVL with the remaining charge just below the matching
 * threshold, so that stage of the low-battery failsafe triggers.
 *
 * @return false if the battery_status publication must be suppressed (Off), true otherwise.
 */
ತಾರ್ಕಿಕ process_battery(ಸ್ಥಿರಾಂಕ Config &config, uint8_t instance, battery_ಸ್ಥಿತಿ_s &battery_ಸ್ಥಿತಿ);

/**
 * GNSS counterpart to process(): on FAILURE_UNIT_SENSOR_GPS for the receiver publishing on the
 * given 0-based uORB instance, Off and Stuck behave as in the generic process() and Wrong reports
 * the fix type selected by SYS_FAIL_GPS_WRG and the jamming state selected by SYS_FAIL_GPS_JAM
 * while leaving the position untouched.
 *
 * @param uorb_instance 0-based uORB instance of the publisher (not the 1-based failure instance).
 * @return false if the sensor_gnss publication must be suppressed (Off), true otherwise.
 */
ತಾರ್ಕಿಕ process_gnss(ಸ್ಥಿರಾಂಕ Config &config, uint8_t uorb_instance, sensor_gnss_s &sensor_gnss,
		  Stuck<sensor_gnss_s> &stuck);

/**
 * ESC counterpart to process(): apply the active FAILURE_UNIT_SYSTEM_ESC failures to a copy of
 * status (matched per ESC by actuator_function). Off zeroes the ESC's telemetry (keeping only
 * actuator_function) and reports it offline and unarmed; Wrong keeps it online but reports
 * consistently wrong values. Takes status by const reference and returns the mutated copy so callers
 * can't accidentally apply it in place to a persistent status, which would compound Wrong across
 * calls. Call after Config::update().
 */
esc_ಸ್ಥಿತಿ_s process_esc(ಸ್ಥಿರಾಂಕ Config &config, ಸ್ಥಿರಾಂಕ esc_ಸ್ಥಿತಿ_s &ಸ್ಥಿತಿ_2);

/**
 * Motor counterpart to process(): derive the masks of motors failed by
 * FAILURE_UNIT_SYSTEM_MOTOR from the injected failure type. Off is the detected failure
 * (failure_mask: reported as failed motors and removed from the allocation), Wrong the
 * undetected one (stop_mask: outputs stopped without informing the allocator).
 * Call after Config::update().
 */
MotorFailureMasks process_motor(ಸ್ಥಿರಾಂಕ Config &config);

#ಇಲ್ಲದಿದ್ದರೆ // !CONFIG_MODULES_FAILURE_INJECTION_MANAGER

ವರ್ಗ Config
{
ಸಾರ್ವಜನಿಕ:
	ತಾರ್ಕಿಕ update() { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
	ಶೂನ್ಯ ಗಣ(ಸ್ಥಿರಾಂಕ failure_injection_s &) {}
	ಬಹುಲಕ_2 ಬಹುಲಕ(uint8_t, uint8_t) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಬಹುಲಕ_2::Ok; }
	ತಾರ್ಕಿಕ any_ಸಕ್ರಿಯ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
};

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು ಸಂದೇಶ_T>
ರಚನೆ Stuck {
};

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು ಸಂದೇಶ_T>
ತಾರ್ಕಿಕ process(ಬಹುಲಕ_2, ಸಂದೇಶ_T &, Stuck<ಸಂದೇಶ_T> &) { ಹಿಂತಿರುಗಿ ನಿಜ; }

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು ಸಂದೇಶ_T>
ತಾರ್ಕಿಕ process(ಸ್ಥಿರಾಂಕ Config &, uint8_t, uint8_t, ಸಂದೇಶ_T &, Stuck<ಸಂದೇಶ_T> &) { ಹಿಂತಿರುಗಿ ನಿಜ; }

ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ process(ಬಹುಲಕ_2) { ಹಿಂತಿರುಗಿ ನಿಜ; }

ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ process(ಸ್ಥಿರಾಂಕ Config &, uint8_t, uint8_t) { ಹಿಂತಿರುಗಿ ನಿಜ; }

ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ process_battery(ಸ್ಥಿರಾಂಕ Config &, uint8_t, battery_ಸ್ಥಿತಿ_s &) { ಹಿಂತಿರುಗಿ ನಿಜ; }

ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ process_gnss(ಸ್ಥಿರಾಂಕ Config &, uint8_t, sensor_gnss_s &, Stuck<sensor_gnss_s> &) { ಹಿಂತಿರುಗಿ ನಿಜ; }

ಸಾಲೊಳಗೆ esc_ಸ್ಥಿತಿ_s process_esc(ಸ್ಥಿರಾಂಕ Config &, ಸ್ಥಿರಾಂಕ esc_ಸ್ಥಿತಿ_s &ಸ್ಥಿತಿ_2) { ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2; }

ಸಾಲೊಳಗೆ MotorFailureMasks process_motor(ಸ್ಥಿರಾಂಕ Config &) { ಹಿಂತಿರುಗಿ {}; }

#ಆದರೆ_ಅಂತ್ಯ // CONFIG_MODULES_FAILURE_INJECTION_MANAGER

} // namespace failure_injection
