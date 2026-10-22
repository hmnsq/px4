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

#અન્તર્ભાવ <cstdint>

#અન્તર્ભાવ <uORB/Subscription.hpp>
#અન્તર્ભાવ <uORB/topics/esc_status.h>
#અન્તર્ભાવ <uORB/topics/failure_injection.h>

સંરચના battery_status_s;
સંરચના sensor_gnss_s;

નામાકાશ failure_injection
{

ગણના વર્ગ Mode : uint8_t {
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
સંરચના MotorFailureMasks {
	uint16_t stop_mask{0};    ///< motor wrong: outputs stopped without informing the allocator
	uint16_t failure_mask{0}; ///< motor off: reported as failed motors, removed from the allocation
};

#જો defined(CONFIG_MODULES_FAILURE_INJECTION_MANAGER)

વર્ગ Config
{
જાહેર:
	/**
	 * Poll the failure_injection subscription and, when a new failure_injection
	 * message arrives, refresh the cached failure config that mode() / any_active()
	 * read. Call once per loop iteration.
	 * @return true if an updated failure config was applied, false if unchanged.
	 */
	તાર્કિક update();

	/** Cache a failure_injection message directly (used by tests). */
	શૂન્ય set(અચલ failure_injection_s &cfg);

	/**
	 * Active failure mode for a (unit, instance), or Mode::Ok if none.
	 * @param unit     one of failure_injection_s::FAILURE_UNIT_*
	 * @param instance 1-based instance, or 0 to match any instance of the unit
	 */
	Mode mode(uint8_t unit, uint8_t instance) અચલ;

	/** True if any failure is currently active (free no-failure early-out). */
	તાર્કિક any_active() અચલ { પાછા ગણતરી_3 > 0; }

ખાનગી:
	uORB::Subscription _sub{ORB_ID(failure_injection)};

	uint8_t  ગણતરી_3{0};
	uint8_t  _unit[failure_injection_s::MAX_FAILURES] {};
	uint16_t _instance_mask[failure_injection_s::MAX_FAILURES] {};
	Mode     _failure_type[failure_injection_s::MAX_FAILURES] {};
};

/**
 * Per-(consumer, instance) state for stateful failures. Holds the last good
 * sample so Stuck can replay it.
 */
template<typename સંદેશ_T>
સંરચના Stuck {
	તાર્કિક valid{ખોટું};
	સંદેશ_T value{};
};

template<typename...>
using void_t = શૂન્ય;

template<typename T, typename = શૂન્ય>
સંરચના has_timestamp_sample {
	સ્થિર constexpr તાર્કિક value = ખોટું;
};
template<typename T>
સંરચના has_timestamp_sample<T, void_t<decltype(T::timestamp_sample)>> {
	સ્થિર constexpr તાર્કિક value = સાચું;
};

/**
 * Generic whole-message processor for the Ok / Off / Stuck mechanics, for consumers
 * that publish a single message and want the message-agnostic behaviour.
 *
 * @return false if the message must be suppressed (Off); true otherwise. For
 *         Stuck the message is overwritten with the last good sample.
 */
template<typename સંદેશ_T>
તાર્કિક process(Mode mode, સંદેશ_T &સંદેશ, Stuck<સંદેશ_T> &stuck)
{
	પસંદગી (mode) {
	સ્થિતિ Mode::Off:
		પાછા ખોટું;

	સ્થિતિ Mode::Stuck:
		જો (stuck.valid) {
			અચલ uint64_t timestamp = સંદેશ.timestamp;

			જો constexpr(has_timestamp_sample<સંદેશ_T>::value) {
				અચલ uint64_t timestamp_sample = સંદેશ.timestamp_sample;
				સંદેશ = stuck.value;
				સંદેશ.timestamp_sample = timestamp_sample;

			} નહીંતર {
				સંદેશ = stuck.value;
			}

			સંદેશ.timestamp = timestamp;
		}

		પાછા સાચું;

	મૂળભૂત:
		stuck.value = સંદેશ;
		stuck.valid = સાચું;
		પાછા સાચું;
	}
}

/**
 * Convenience overload to fix the fact that the uORB instance is 0-based but the failure_injection instance is 1-based.
 */
template<typename સંદેશ_T>
તાર્કિક process(અચલ Config &config, uint8_t unit, uint8_t uorb_instance, સંદેશ_T &સંદેશ, Stuck<સંદેશ_T> &stuck)
{
	પાછા process(config.mode(unit, uorb_instance + 1), સંદેશ, stuck);
}

/**
 * Message-less variant for producers with no payload to replay (e.g. a heartbeat)
 *
 * @return false if the signal must be treated as failed/suppressed (Off), true otherwise.
 */
અંતઃરેખ તાર્કિક process(Mode mode)
{
	પાછા mode != Mode::Off;
}

/** Convenience overload, same 0-based to 1-based instance mapping as the generic process(). */
અંતઃરેખ તાર્કિક process(અચલ Config &config, uint8_t unit, uint8_t uorb_instance)
{
	પાછા process(config.mode(unit, uorb_instance + 1));
}

/**
 * Battery counterpart to process(): on FAILURE_UNIT_SYSTEM_BATTERY for the given 1-based
 * instance, Off suppresses the publication (the pack reads disconnected) and Wrong reports the
 * warning level selected by SYS_FAIL_BAT_LVL with the remaining charge just below the matching
 * threshold, so that stage of the low-battery failsafe triggers.
 *
 * @return false if the battery_status publication must be suppressed (Off), true otherwise.
 */
તાર્કિક process_battery(અચલ Config &config, uint8_t instance, battery_status_s &battery_status);

/**
 * GNSS counterpart to process(): on FAILURE_UNIT_SENSOR_GPS for the receiver publishing on the
 * given 0-based uORB instance, Off and Stuck behave as in the generic process() and Wrong reports
 * the fix type selected by SYS_FAIL_GPS_WRG and the jamming state selected by SYS_FAIL_GPS_JAM
 * while leaving the position untouched.
 *
 * @param uorb_instance 0-based uORB instance of the publisher (not the 1-based failure instance).
 * @return false if the sensor_gnss publication must be suppressed (Off), true otherwise.
 */
તાર્કિક process_gnss(અચલ Config &config, uint8_t uorb_instance, sensor_gnss_s &sensor_gnss,
		  Stuck<sensor_gnss_s> &stuck);

/**
 * ESC counterpart to process(): apply the active FAILURE_UNIT_SYSTEM_ESC failures to a copy of
 * status (matched per ESC by actuator_function). Off zeroes the ESC's telemetry (keeping only
 * actuator_function) and reports it offline and unarmed; Wrong keeps it online but reports
 * consistently wrong values. Takes status by const reference and returns the mutated copy so callers
 * can't accidentally apply it in place to a persistent status, which would compound Wrong across
 * calls. Call after Config::update().
 */
esc_status_s process_esc(અચલ Config &config, અચલ esc_status_s &status);

/**
 * Motor counterpart to process(): derive the masks of motors failed by
 * FAILURE_UNIT_SYSTEM_MOTOR from the injected failure type. Off is the detected failure
 * (failure_mask: reported as failed motors and removed from the allocation), Wrong the
 * undetected one (stop_mask: outputs stopped without informing the allocator).
 * Call after Config::update().
 */
MotorFailureMasks process_motor(અચલ Config &config);

#નહીંતર // !CONFIG_MODULES_FAILURE_INJECTION_MANAGER

વર્ગ Config
{
જાહેર:
	તાર્કિક update() { પાછા ખોટું; }
	શૂન્ય set(અચલ failure_injection_s &) {}
	Mode mode(uint8_t, uint8_t) અચલ { પાછા Mode::Ok; }
	તાર્કિક any_active() અચલ { પાછા ખોટું; }
};

template<typename સંદેશ_T>
સંરચના Stuck {
};

template<typename સંદેશ_T>
તાર્કિક process(Mode, સંદેશ_T &, Stuck<સંદેશ_T> &) { પાછા સાચું; }

template<typename સંદેશ_T>
તાર્કિક process(અચલ Config &, uint8_t, uint8_t, સંદેશ_T &, Stuck<સંદેશ_T> &) { પાછા સાચું; }

અંતઃરેખ તાર્કિક process(Mode) { પાછા સાચું; }

અંતઃરેખ તાર્કિક process(અચલ Config &, uint8_t, uint8_t) { પાછા સાચું; }

અંતઃરેખ તાર્કિક process_battery(અચલ Config &, uint8_t, battery_status_s &) { પાછા સાચું; }

અંતઃરેખ તાર્કિક process_gnss(અચલ Config &, uint8_t, sensor_gnss_s &, Stuck<sensor_gnss_s> &) { પાછા સાચું; }

અંતઃરેખ esc_status_s process_esc(અચલ Config &, અચલ esc_status_s &status) { પાછા status; }

અંતઃરેખ MotorFailureMasks process_motor(અચલ Config &) { પાછા {}; }

#endif // CONFIG_MODULES_FAILURE_INJECTION_MANAGER

} // namespace failure_injection
