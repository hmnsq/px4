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

#సూచన once

#అన్తర్భావ <cstdint>

#అన్తర్భావ <uORB/Subscription.hpp>
#అన్తర్భావ <uORB/topics/esc_status.h>
#అన్తర్భావ <uORB/topics/failure_injection.h>

నిర్మాణం battery_స్థితి_s;
నిర్మాణం sensor_gnss_s;

నామాకాశ failure_injection
{

గణన తరగతి బహుళకం_2 : uint8_t {
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
నిర్మాణం MotorFailureMasks {
	uint16_t stop_ముసుగు{0};    ///< motor wrong: outputs stopped without informing the allocator
	uint16_t failure_ముసుగు{0}; ///< motor off: reported as failed motors, removed from the allocation
};

#ఒకవేళ defined(CONFIG_MODULES_FAILURE_INJECTION_MANAGER)

తరగతి Config
{
బహిరంగ:
	/**
	 * Poll the failure_injection subscription and, when a new failure_injection
	 * message arrives, refresh the cached failure config that mode() / any_active()
	 * read. Call once per loop iteration.
	 * @return true if an updated failure config was applied, false if unchanged.
	 */
	తార్కిక update();

	/** Cache a failure_injection message directly (used by tests). */
	శూన్య సమితి(స్థిరరాశి failure_injection_s &cfg);

	/**
	 * Active failure mode for a (unit, instance), or Mode::Ok if none.
	 * @param unit     one of failure_injection_s::FAILURE_UNIT_*
	 * @param instance 1-based instance, or 0 to match any instance of the unit
	 */
	బహుళకం_2 బహుళకం(uint8_t unit, uint8_t instance) స్థిరరాశి;

	/** True if any failure is currently active (free no-failure early-out). */
	తార్కిక any_చురుకైన() స్థిరరాశి { తిరిగి లెక్క_3 > 0; }

అంతరంగ:
	uORB::Subscription _sub{ORB_ID(failure_injection)};

	uint8_t  లెక్క_3{0};
	uint8_t  _unit[failure_injection_s::MAX_FAILURES] {};
	uint16_t instance_ముసుగు[failure_injection_s::MAX_FAILURES] {};
	బహుళకం_2     _failure_type[failure_injection_s::MAX_FAILURES] {};
};

/**
 * Per-(consumer, instance) state for stateful failures. Holds the last good
 * sample so Stuck can replay it.
 */
మూస<రకం_పేరు సందేశం_T>
నిర్మాణం Stuck {
	తార్కిక valid{అబద్ధం};
	సందేశం_T value{};
};

మూస<రకం_పేరు...>
ఉపయోగించి void_t = శూన్య;

మూస<రకం_పేరు T, రకం_పేరు = శూన్య>
నిర్మాణం has_timestamp_sample {
	స్థిర constexpr తార్కిక value = అబద్ధం;
};
మూస<రకం_పేరు T>
నిర్మాణం has_timestamp_sample<T, void_t<decltype(T::timestamp_sample)>> {
	స్థిర constexpr తార్కిక value = నిజం;
};

/**
 * Generic whole-message processor for the Ok / Off / Stuck mechanics, for consumers
 * that publish a single message and want the message-agnostic behaviour.
 *
 * @return false if the message must be suppressed (Off); true otherwise. For
 *         Stuck the message is overwritten with the last good sample.
 */
మూస<రకం_పేరు సందేశం_T>
తార్కిక process(బహుళకం_2 బహుళకం, సందేశం_T &సందేశం_2, Stuck<సందేశం_T> &stuck)
{
	ఎంపిక (బహుళకం) {
	స్థితి బహుళకం_2::Off:
		తిరిగి అబద్ధం;

	స్థితి బహుళకం_2::Stuck:
		ఒకవేళ (stuck.valid) {
			స్థిరరాశి uint64_t timestamp = సందేశం_2.timestamp;

			ఒకవేళ constexpr(has_timestamp_sample<సందేశం_T>::value) {
				స్థిరరాశి uint64_t timestamp_sample = సందేశం_2.timestamp_sample;
				సందేశం_2 = stuck.value;
				సందేశం_2.timestamp_sample = timestamp_sample;

			} లేకపోతే {
				సందేశం_2 = stuck.value;
			}

			సందేశం_2.timestamp = timestamp;
		}

		తిరిగి నిజం;

	అప్రమేయ:
		stuck.value = సందేశం_2;
		stuck.valid = నిజం;
		తిరిగి నిజం;
	}
}

/**
 * Convenience overload to fix the fact that the uORB instance is 0-based but the failure_injection instance is 1-based.
 */
మూస<రకం_పేరు సందేశం_T>
తార్కిక process(స్థిరరాశి Config &config, uint8_t unit, uint8_t uorb_instance, సందేశం_T &సందేశం_2, Stuck<సందేశం_T> &stuck)
{
	తిరిగి process(config.బహుళకం(unit, uorb_instance + 1), సందేశం_2, stuck);
}

/**
 * Message-less variant for producers with no payload to replay (e.g. a heartbeat)
 *
 * @return false if the signal must be treated as failed/suppressed (Off), true otherwise.
 */
అంతఃవరుస తార్కిక process(బహుళకం_2 బహుళకం)
{
	తిరిగి బహుళకం != బహుళకం_2::Off;
}

/** Convenience overload, same 0-based to 1-based instance mapping as the generic process(). */
అంతఃవరుస తార్కిక process(స్థిరరాశి Config &config, uint8_t unit, uint8_t uorb_instance)
{
	తిరిగి process(config.బహుళకం(unit, uorb_instance + 1));
}

/**
 * Battery counterpart to process(): on FAILURE_UNIT_SYSTEM_BATTERY for the given 1-based
 * instance, Off suppresses the publication (the pack reads disconnected) and Wrong reports the
 * warning level selected by SYS_FAIL_BAT_LVL with the remaining charge just below the matching
 * threshold, so that stage of the low-battery failsafe triggers.
 *
 * @return false if the battery_status publication must be suppressed (Off), true otherwise.
 */
తార్కిక process_battery(స్థిరరాశి Config &config, uint8_t instance, battery_స్థితి_s &battery_స్థితి);

/**
 * GNSS counterpart to process(): on FAILURE_UNIT_SENSOR_GPS for the receiver publishing on the
 * given 0-based uORB instance, Off and Stuck behave as in the generic process() and Wrong reports
 * the fix type selected by SYS_FAIL_GPS_WRG and the jamming state selected by SYS_FAIL_GPS_JAM
 * while leaving the position untouched.
 *
 * @param uorb_instance 0-based uORB instance of the publisher (not the 1-based failure instance).
 * @return false if the sensor_gnss publication must be suppressed (Off), true otherwise.
 */
తార్కిక process_gnss(స్థిరరాశి Config &config, uint8_t uorb_instance, sensor_gnss_s &sensor_gnss,
		  Stuck<sensor_gnss_s> &stuck);

/**
 * ESC counterpart to process(): apply the active FAILURE_UNIT_SYSTEM_ESC failures to a copy of
 * status (matched per ESC by actuator_function). Off zeroes the ESC's telemetry (keeping only
 * actuator_function) and reports it offline and unarmed; Wrong keeps it online but reports
 * consistently wrong values. Takes status by const reference and returns the mutated copy so callers
 * can't accidentally apply it in place to a persistent status, which would compound Wrong across
 * calls. Call after Config::update().
 */
esc_స్థితి_s process_esc(స్థిరరాశి Config &config, స్థిరరాశి esc_స్థితి_s &స్థితి_2);

/**
 * Motor counterpart to process(): derive the masks of motors failed by
 * FAILURE_UNIT_SYSTEM_MOTOR from the injected failure type. Off is the detected failure
 * (failure_mask: reported as failed motors and removed from the allocation), Wrong the
 * undetected one (stop_mask: outputs stopped without informing the allocator).
 * Call after Config::update().
 */
MotorFailureMasks process_motor(స్థిరరాశి Config &config);

#లేకపోతే // !CONFIG_MODULES_FAILURE_INJECTION_MANAGER

తరగతి Config
{
బహిరంగ:
	తార్కిక update() { తిరిగి అబద్ధం; }
	శూన్య సమితి(స్థిరరాశి failure_injection_s &) {}
	బహుళకం_2 బహుళకం(uint8_t, uint8_t) స్థిరరాశి { తిరిగి బహుళకం_2::Ok; }
	తార్కిక any_చురుకైన() స్థిరరాశి { తిరిగి అబద్ధం; }
};

మూస<రకం_పేరు సందేశం_T>
నిర్మాణం Stuck {
};

మూస<రకం_పేరు సందేశం_T>
తార్కిక process(బహుళకం_2, సందేశం_T &, Stuck<సందేశం_T> &) { తిరిగి నిజం; }

మూస<రకం_పేరు సందేశం_T>
తార్కిక process(స్థిరరాశి Config &, uint8_t, uint8_t, సందేశం_T &, Stuck<సందేశం_T> &) { తిరిగి నిజం; }

అంతఃవరుస తార్కిక process(బహుళకం_2) { తిరిగి నిజం; }

అంతఃవరుస తార్కిక process(స్థిరరాశి Config &, uint8_t, uint8_t) { తిరిగి నిజం; }

అంతఃవరుస తార్కిక process_battery(స్థిరరాశి Config &, uint8_t, battery_స్థితి_s &) { తిరిగి నిజం; }

అంతఃవరుస తార్కిక process_gnss(స్థిరరాశి Config &, uint8_t, sensor_gnss_s &, Stuck<sensor_gnss_s> &) { తిరిగి నిజం; }

అంతఃవరుస esc_స్థితి_s process_esc(స్థిరరాశి Config &, స్థిరరాశి esc_స్థితి_s &స్థితి_2) { తిరిగి స్థితి_2; }

అంతఃవరుస MotorFailureMasks process_motor(స్థిరరాశి Config &) { తిరిగి {}; }

#అయితే_ముగింపు // CONFIG_MODULES_FAILURE_INJECTION_MANAGER

} // namespace failure_injection
