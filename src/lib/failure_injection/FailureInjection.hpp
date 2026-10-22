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

#ଅନ୍ତର୍ଭାଵ <cstdint>

#ଅନ୍ତର୍ଭାଵ <uORB/Subscription.hpp>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/esc_status.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/failure_injection.h>

ସଂରଚନା battery_status_s;
ସଂରଚନା sensor_gnss_s;

ନାମାକାଶ failure_injection
{

ଗଣନା ଶ୍ରେଣୀ Mode : uint8_t {
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
ସଂରଚନା MotorFailureMasks {
	uint16_t stop_mask{0};    ///< motor wrong: outputs stopped without informing the allocator
	uint16_t failure_mask{0}; ///< motor off: reported as failed motors, removed from the allocation
};

#ଯଦି defined(CONFIG_MODULES_FAILURE_INJECTION_MANAGER)

ଶ୍ରେଣୀ Config
{
ସର୍ବସାଧାରଣ:
	/**
	 * Poll the failure_injection subscription and, when a new failure_injection
	 * message arrives, refresh the cached failure config that mode() / any_active()
	 * read. Call once per loop iteration.
	 * @return true if an updated failure config was applied, false if unchanged.
	 */
	ତାର୍କିକ update();

	/** Cache a failure_injection message directly (used by tests). */
	ଶୂନ୍ୟ set(ଅଚଳ failure_injection_s &cfg);

	/**
	 * Active failure mode for a (unit, instance), or Mode::Ok if none.
	 * @param unit     one of failure_injection_s::FAILURE_UNIT_*
	 * @param instance 1-based instance, or 0 to match any instance of the unit
	 */
	Mode mode(uint8_t unit, uint8_t instance) ଅଚଳ;

	/** True if any failure is currently active (free no-failure early-out). */
	ତାର୍କିକ any_active() ଅଚଳ { ଫେରନ୍ତୁ ଗଣନା_4 > 0; }

ବ୍ୟକ୍ତିଗତ:
	uORB::Subscription _sub{ORB_ID(failure_injection)};

	uint8_t  ଗଣନା_4{0};
	uint8_t  _unit[failure_injection_s::MAX_FAILURES] {};
	uint16_t _instance_mask[failure_injection_s::MAX_FAILURES] {};
	Mode     _failure_type[failure_injection_s::MAX_FAILURES] {};
};

/**
 * Per-(consumer, instance) state for stateful failures. Holds the last good
 * sample so Stuck can replay it.
 */
template<typename ସନ୍ଦେଶ_T>
ସଂରଚନା Stuck {
	ତାର୍କିକ valid{ମିଥ୍ୟା};
	ସନ୍ଦେଶ_T value{};
};

template<typename...>
using void_t = ଶୂନ୍ୟ;

template<typename T, typename = ଶୂନ୍ୟ>
ସଂରଚନା has_timestamp_sample {
	ସ୍ଥିର constexpr ତାର୍କିକ value = ମିଥ୍ୟା;
};
template<typename T>
ସଂରଚନା has_timestamp_sample<T, void_t<decltype(T::timestamp_sample)>> {
	ସ୍ଥିର constexpr ତାର୍କିକ value = ସତ୍ୟ;
};

/**
 * Generic whole-message processor for the Ok / Off / Stuck mechanics, for consumers
 * that publish a single message and want the message-agnostic behaviour.
 *
 * @return false if the message must be suppressed (Off); true otherwise. For
 *         Stuck the message is overwritten with the last good sample.
 */
template<typename ସନ୍ଦେଶ_T>
ତାର୍କିକ process(Mode mode, ସନ୍ଦେଶ_T &ସନ୍ଦେଶ, Stuck<ସନ୍ଦେଶ_T> &stuck)
{
	ବାଛ (mode) {
	ସ୍ଥିତି Mode::Off:
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ସ୍ଥିତି Mode::Stuck:
		ଯଦି (stuck.valid) {
			ଅଚଳ uint64_t timestamp = ସନ୍ଦେଶ.timestamp;

			ଯଦି constexpr(has_timestamp_sample<ସନ୍ଦେଶ_T>::value) {
				ଅଚଳ uint64_t timestamp_sample = ସନ୍ଦେଶ.timestamp_sample;
				ସନ୍ଦେଶ = stuck.value;
				ସନ୍ଦେଶ.timestamp_sample = timestamp_sample;

			} ଅନ୍ୟଥା {
				ସନ୍ଦେଶ = stuck.value;
			}

			ସନ୍ଦେଶ.timestamp = timestamp;
		}

		ଫେରନ୍ତୁ ସତ୍ୟ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		stuck.value = ସନ୍ଦେଶ;
		stuck.valid = ସତ୍ୟ;
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}
}

/**
 * Convenience overload to fix the fact that the uORB instance is 0-based but the failure_injection instance is 1-based.
 */
template<typename ସନ୍ଦେଶ_T>
ତାର୍କିକ process(ଅଚଳ Config &config, uint8_t unit, uint8_t uorb_instance, ସନ୍ଦେଶ_T &ସନ୍ଦେଶ, Stuck<ସନ୍ଦେଶ_T> &stuck)
{
	ଫେରନ୍ତୁ process(config.mode(unit, uorb_instance + 1), ସନ୍ଦେଶ, stuck);
}

/**
 * Message-less variant for producers with no payload to replay (e.g. a heartbeat)
 *
 * @return false if the signal must be treated as failed/suppressed (Off), true otherwise.
 */
ଧାଡ଼ିଭିତରେ ତାର୍କିକ process(Mode mode)
{
	ଫେରନ୍ତୁ mode != Mode::Off;
}

/** Convenience overload, same 0-based to 1-based instance mapping as the generic process(). */
ଧାଡ଼ିଭିତରେ ତାର୍କିକ process(ଅଚଳ Config &config, uint8_t unit, uint8_t uorb_instance)
{
	ଫେରନ୍ତୁ process(config.mode(unit, uorb_instance + 1));
}

/**
 * Battery counterpart to process(): on FAILURE_UNIT_SYSTEM_BATTERY for the given 1-based
 * instance, Off suppresses the publication (the pack reads disconnected) and Wrong reports the
 * warning level selected by SYS_FAIL_BAT_LVL with the remaining charge just below the matching
 * threshold, so that stage of the low-battery failsafe triggers.
 *
 * @return false if the battery_status publication must be suppressed (Off), true otherwise.
 */
ତାର୍କିକ process_battery(ଅଚଳ Config &config, uint8_t instance, battery_status_s &battery_status);

/**
 * GNSS counterpart to process(): on FAILURE_UNIT_SENSOR_GPS for the receiver publishing on the
 * given 0-based uORB instance, Off and Stuck behave as in the generic process() and Wrong reports
 * the fix type selected by SYS_FAIL_GPS_WRG and the jamming state selected by SYS_FAIL_GPS_JAM
 * while leaving the position untouched.
 *
 * @param uorb_instance 0-based uORB instance of the publisher (not the 1-based failure instance).
 * @return false if the sensor_gnss publication must be suppressed (Off), true otherwise.
 */
ତାର୍କିକ process_gnss(ଅଚଳ Config &config, uint8_t uorb_instance, sensor_gnss_s &sensor_gnss,
		  Stuck<sensor_gnss_s> &stuck);

/**
 * ESC counterpart to process(): apply the active FAILURE_UNIT_SYSTEM_ESC failures to a copy of
 * status (matched per ESC by actuator_function). Off zeroes the ESC's telemetry (keeping only
 * actuator_function) and reports it offline and unarmed; Wrong keeps it online but reports
 * consistently wrong values. Takes status by const reference and returns the mutated copy so callers
 * can't accidentally apply it in place to a persistent status, which would compound Wrong across
 * calls. Call after Config::update().
 */
esc_status_s process_esc(ଅଚଳ Config &config, ଅଚଳ esc_status_s &status);

/**
 * Motor counterpart to process(): derive the masks of motors failed by
 * FAILURE_UNIT_SYSTEM_MOTOR from the injected failure type. Off is the detected failure
 * (failure_mask: reported as failed motors and removed from the allocation), Wrong the
 * undetected one (stop_mask: outputs stopped without informing the allocator).
 * Call after Config::update().
 */
MotorFailureMasks process_motor(ଅଚଳ Config &config);

#ଅନ୍ୟଥା // !CONFIG_MODULES_FAILURE_INJECTION_MANAGER

ଶ୍ରେଣୀ Config
{
ସର୍ବସାଧାରଣ:
	ତାର୍କିକ update() { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
	ଶୂନ୍ୟ set(ଅଚଳ failure_injection_s &) {}
	Mode mode(uint8_t, uint8_t) ଅଚଳ { ଫେରନ୍ତୁ Mode::Ok; }
	ତାର୍କିକ any_active() ଅଚଳ { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
};

template<typename ସନ୍ଦେଶ_T>
ସଂରଚନା Stuck {
};

template<typename ସନ୍ଦେଶ_T>
ତାର୍କିକ process(Mode, ସନ୍ଦେଶ_T &, Stuck<ସନ୍ଦେଶ_T> &) { ଫେରନ୍ତୁ ସତ୍ୟ; }

template<typename ସନ୍ଦେଶ_T>
ତାର୍କିକ process(ଅଚଳ Config &, uint8_t, uint8_t, ସନ୍ଦେଶ_T &, Stuck<ସନ୍ଦେଶ_T> &) { ଫେରନ୍ତୁ ସତ୍ୟ; }

ଧାଡ଼ିଭିତରେ ତାର୍କିକ process(Mode) { ଫେରନ୍ତୁ ସତ୍ୟ; }

ଧାଡ଼ିଭିତରେ ତାର୍କିକ process(ଅଚଳ Config &, uint8_t, uint8_t) { ଫେରନ୍ତୁ ସତ୍ୟ; }

ଧାଡ଼ିଭିତରେ ତାର୍କିକ process_battery(ଅଚଳ Config &, uint8_t, battery_status_s &) { ଫେରନ୍ତୁ ସତ୍ୟ; }

ଧାଡ଼ିଭିତରେ ତାର୍କିକ process_gnss(ଅଚଳ Config &, uint8_t, sensor_gnss_s &, Stuck<sensor_gnss_s> &) { ଫେରନ୍ତୁ ସତ୍ୟ; }

ଧାଡ଼ିଭିତରେ esc_status_s process_esc(ଅଚଳ Config &, ଅଚଳ esc_status_s &status) { ଫେରନ୍ତୁ status; }

ଧାଡ଼ିଭିତରେ MotorFailureMasks process_motor(ଅଚଳ Config &) { ଫେରନ୍ତୁ {}; }

#endif // CONFIG_MODULES_FAILURE_INJECTION_MANAGER

} // namespace failure_injection
