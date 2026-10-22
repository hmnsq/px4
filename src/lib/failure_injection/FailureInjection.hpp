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

#pragma once

#অন্তর্ভাব <cstdint>

#অন্তর্ভাব <uORB/Subscription.hpp>
#অন্তর্ভাব <uORB/topics/esc_status.h>
#অন্তর্ভাব <uORB/topics/failure_injection.h>

সংরচনা battery_status_s;
সংরচনা sensor_gnss_s;

নামাকাশ failure_injection
{

গণনা বর্গ Mode : uint8_t {
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
সংরচনা MotorFailureMasks {
	uint16_t stop_mask{0};    ///< motor wrong: outputs stopped without informing the allocator
	uint16_t failure_mask{0}; ///< motor off: reported as failed motors, removed from the allocation
};

#যদি defined(CONFIG_MODULES_FAILURE_INJECTION_MANAGER)

বর্গ Config
{
সার্বজনিক:
	/**
	 * Poll the failure_injection subscription and, when a new failure_injection
	 * message arrives, refresh the cached failure config that mode() / any_active()
	 * read. Call once per loop iteration.
	 * @return true if an updated failure config was applied, false if unchanged.
	 */
	তার্কিক update();

	/** Cache a failure_injection message directly (used by tests). */
	শূন্য set(ধ্রুব failure_injection_s &cfg);

	/**
	 * Active failure mode for a (unit, instance), or Mode::Ok if none.
	 * @param unit     one of failure_injection_s::FAILURE_UNIT_*
	 * @param instance 1-based instance, or 0 to match any instance of the unit
	 */
	Mode mode(uint8_t unit, uint8_t instance) ধ্রুব;

	/** True if any failure is currently active (free no-failure early-out). */
	তার্কিক any_active() ধ্রুব { প্রত্যাগম গুনতি_3 > 0; }

গুপ্ত:
	uORB::Subscription _sub{ORB_ID(failure_injection)};

	uint8_t  গুনতি_3{0};
	uint8_t  _unit[failure_injection_s::MAX_FAILURES] {};
	uint16_t _instance_mask[failure_injection_s::MAX_FAILURES] {};
	Mode     _failure_type[failure_injection_s::MAX_FAILURES] {};
};

/**
 * Per-(consumer, instance) state for stateful failures. Holds the last good
 * sample so Stuck can replay it.
 */
template<typename বার্তা_T>
সংরচনা Stuck {
	তার্কিক valid{অসত্য};
	বার্তা_T value{};
};

template<typename...>
using void_t = শূন্য;

template<typename T, typename = শূন্য>
সংরচনা has_timestamp_sample {
	স্থির constexpr তার্কিক value = অসত্য;
};
template<typename T>
সংরচনা has_timestamp_sample<T, void_t<decltype(T::timestamp_sample)>> {
	স্থির constexpr তার্কিক value = সত্য;
};

/**
 * Generic whole-message processor for the Ok / Off / Stuck mechanics, for consumers
 * that publish a single message and want the message-agnostic behaviour.
 *
 * @return false if the message must be suppressed (Off); true otherwise. For
 *         Stuck the message is overwritten with the last good sample.
 */
template<typename বার্তা_T>
তার্কিক process(Mode mode, বার্তা_T &বার্তা, Stuck<বার্তা_T> &stuck)
{
	বিকল্প (mode) {
	পক্ষ Mode::Off:
		প্রত্যাগম অসত্য;

	পক্ষ Mode::Stuck:
		যদি (stuck.valid) {
			ধ্রুব uint64_t timestamp = বার্তা.timestamp;

			যদি constexpr(has_timestamp_sample<বার্তা_T>::value) {
				ধ্রুব uint64_t timestamp_sample = বার্তা.timestamp_sample;
				বার্তা = stuck.value;
				বার্তা.timestamp_sample = timestamp_sample;

			} অন্যথা {
				বার্তা = stuck.value;
			}

			বার্তা.timestamp = timestamp;
		}

		প্রত্যাগম সত্য;

	স্বভাব:
		stuck.value = বার্তা;
		stuck.valid = সত্য;
		প্রত্যাগম সত্য;
	}
}

/**
 * Convenience overload to fix the fact that the uORB instance is 0-based but the failure_injection instance is 1-based.
 */
template<typename বার্তা_T>
তার্কিক process(ধ্রুব Config &config, uint8_t unit, uint8_t uorb_instance, বার্তা_T &বার্তা, Stuck<বার্তা_T> &stuck)
{
	প্রত্যাগম process(config.mode(unit, uorb_instance + 1), বার্তা, stuck);
}

/**
 * Message-less variant for producers with no payload to replay (e.g. a heartbeat)
 *
 * @return false if the signal must be treated as failed/suppressed (Off), true otherwise.
 */
অন্তঃস্থ তার্কিক process(Mode mode)
{
	প্রত্যাগম mode != Mode::Off;
}

/** Convenience overload, same 0-based to 1-based instance mapping as the generic process(). */
অন্তঃস্থ তার্কিক process(ধ্রুব Config &config, uint8_t unit, uint8_t uorb_instance)
{
	প্রত্যাগম process(config.mode(unit, uorb_instance + 1));
}

/**
 * Battery counterpart to process(): on FAILURE_UNIT_SYSTEM_BATTERY for the given 1-based
 * instance, Off suppresses the publication (the pack reads disconnected) and Wrong reports the
 * warning level selected by SYS_FAIL_BAT_LVL with the remaining charge just below the matching
 * threshold, so that stage of the low-battery failsafe triggers.
 *
 * @return false if the battery_status publication must be suppressed (Off), true otherwise.
 */
তার্কিক process_battery(ধ্রুব Config &config, uint8_t instance, battery_status_s &battery_status);

/**
 * GNSS counterpart to process(): on FAILURE_UNIT_SENSOR_GPS for the receiver publishing on the
 * given 0-based uORB instance, Off and Stuck behave as in the generic process() and Wrong reports
 * the fix type selected by SYS_FAIL_GPS_WRG and the jamming state selected by SYS_FAIL_GPS_JAM
 * while leaving the position untouched.
 *
 * @param uorb_instance 0-based uORB instance of the publisher (not the 1-based failure instance).
 * @return false if the sensor_gnss publication must be suppressed (Off), true otherwise.
 */
তার্কিক process_gnss(ধ্রুব Config &config, uint8_t uorb_instance, sensor_gnss_s &sensor_gnss,
		  Stuck<sensor_gnss_s> &stuck);

/**
 * ESC counterpart to process(): apply the active FAILURE_UNIT_SYSTEM_ESC failures to a copy of
 * status (matched per ESC by actuator_function). Off zeroes the ESC's telemetry (keeping only
 * actuator_function) and reports it offline and unarmed; Wrong keeps it online but reports
 * consistently wrong values. Takes status by const reference and returns the mutated copy so callers
 * can't accidentally apply it in place to a persistent status, which would compound Wrong across
 * calls. Call after Config::update().
 */
esc_status_s process_esc(ধ্রুব Config &config, ধ্রুব esc_status_s &status);

/**
 * Motor counterpart to process(): derive the masks of motors failed by
 * FAILURE_UNIT_SYSTEM_MOTOR from the injected failure type. Off is the detected failure
 * (failure_mask: reported as failed motors and removed from the allocation), Wrong the
 * undetected one (stop_mask: outputs stopped without informing the allocator).
 * Call after Config::update().
 */
MotorFailureMasks process_motor(ধ্রুব Config &config);

#অন্যথা // !CONFIG_MODULES_FAILURE_INJECTION_MANAGER

বর্গ Config
{
সার্বজনিক:
	তার্কিক update() { প্রত্যাগম অসত্য; }
	শূন্য set(ধ্রুব failure_injection_s &) {}
	Mode mode(uint8_t, uint8_t) ধ্রুব { প্রত্যাগম Mode::Ok; }
	তার্কিক any_active() ধ্রুব { প্রত্যাগম অসত্য; }
};

template<typename বার্তা_T>
সংরচনা Stuck {
};

template<typename বার্তা_T>
তার্কিক process(Mode, বার্তা_T &, Stuck<বার্তা_T> &) { প্রত্যাগম সত্য; }

template<typename বার্তা_T>
তার্কিক process(ধ্রুব Config &, uint8_t, uint8_t, বার্তা_T &, Stuck<বার্তা_T> &) { প্রত্যাগম সত্য; }

অন্তঃস্থ তার্কিক process(Mode) { প্রত্যাগম সত্য; }

অন্তঃস্থ তার্কিক process(ধ্রুব Config &, uint8_t, uint8_t) { প্রত্যাগম সত্য; }

অন্তঃস্থ তার্কিক process_battery(ধ্রুব Config &, uint8_t, battery_status_s &) { প্রত্যাগম সত্য; }

অন্তঃস্থ তার্কিক process_gnss(ধ্রুব Config &, uint8_t, sensor_gnss_s &, Stuck<sensor_gnss_s> &) { প্রত্যাগম সত্য; }

অন্তঃস্থ esc_status_s process_esc(ধ্রুব Config &, ধ্রুব esc_status_s &status) { প্রত্যাগম status; }

অন্তঃস্থ MotorFailureMasks process_motor(ধ্রুব Config &) { প্রত্যাগম {}; }

#endif // CONFIG_MODULES_FAILURE_INJECTION_MANAGER

} // namespace failure_injection
