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

#ہدایت once

#شامل <cstdint>

#شامل <uORB/Subscription.hpp>
#شامل <uORB/topics/esc_status.h>
#شامل <uORB/topics/failure_injection.h>

ساخت battery_حالت_s;
ساخت sensor_gnss_s;

نامخلا failure_injection
{

شمار جماعت منوال_2 : uint8_t {
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
ساخت MotorFailureMasks {
	uint16_t stop_نقاب{0};    ///< motor wrong: outputs stopped without informing the allocator
	uint16_t failure_نقاب{0}; ///< motor off: reported as failed motors, removed from the allocation
};

#اگر defined(CONFIG_MODULES_FAILURE_INJECTION_MANAGER)

جماعت Config
{
عوامی:
	/**
	 * Poll the failure_injection subscription and, when a new failure_injection
	 * message arrives, refresh the cached failure config that mode() / any_active()
	 * read. Call once per loop iteration.
	 * @return true if an updated failure config was applied, false if unchanged.
	 */
	منطقی update();

	/** Cache a failure_injection message directly (used by tests). */
	خالی سیٹ(ثابت failure_injection_s &cfg);

	/**
	 * Active failure mode for a (unit, instance), or Mode::Ok if none.
	 * @param unit     one of failure_injection_s::FAILURE_UNIT_*
	 * @param instance 1-based instance, or 0 to match any instance of the unit
	 */
	منوال_2 منوال(uint8_t unit, uint8_t instance) ثابت;

	/** True if any failure is currently active (free no-failure early-out). */
	منطقی any_فعال() ثابت { واپس گنتی_3 > 0; }

نجی:
	uORB::Subscription _sub{ORB_ID(failure_injection)};

	uint8_t  گنتی_3{0};
	uint8_t  _unit[failure_injection_s::MAX_FAILURES] {};
	uint16_t instance_نقاب[failure_injection_s::MAX_FAILURES] {};
	منوال_2     _failure_type[failure_injection_s::MAX_FAILURES] {};
};

/**
 * Per-(consumer, instance) state for stateful failures. Holds the last good
 * sample so Stuck can replay it.
 */
سانچہ<قسم_نام پیغام_T>
ساخت Stuck {
	منطقی valid{جھوٹ};
	پیغام_T value{};
};

سانچہ<قسم_نام...>
استعمال void_t = خالی;

سانچہ<قسم_نام T, قسم_نام = خالی>
ساخت has_timestamp_sample {
	ساکن constexpr منطقی value = جھوٹ;
};
سانچہ<قسم_نام T>
ساخت has_timestamp_sample<T, void_t<decltype(T::timestamp_sample)>> {
	ساکن constexpr منطقی value = سچ;
};

/**
 * Generic whole-message processor for the Ok / Off / Stuck mechanics, for consumers
 * that publish a single message and want the message-agnostic behaviour.
 *
 * @return false if the message must be suppressed (Off); true otherwise. For
 *         Stuck the message is overwritten with the last good sample.
 */
سانچہ<قسم_نام پیغام_T>
منطقی process(منوال_2 منوال, پیغام_T &پیغام_2, Stuck<پیغام_T> &stuck)
{
	انتخاب (منوال) {
	حالت منوال_2::Off:
		واپس جھوٹ;

	حالت منوال_2::Stuck:
		اگر (stuck.valid) {
			ثابت uint64_t timestamp = پیغام_2.timestamp;

			اگر constexpr(has_timestamp_sample<پیغام_T>::value) {
				ثابت uint64_t timestamp_sample = پیغام_2.timestamp_sample;
				پیغام_2 = stuck.value;
				پیغام_2.timestamp_sample = timestamp_sample;

			} ورنہ {
				پیغام_2 = stuck.value;
			}

			پیغام_2.timestamp = timestamp;
		}

		واپس سچ;

	طےشدہ:
		stuck.value = پیغام_2;
		stuck.valid = سچ;
		واپس سچ;
	}
}

/**
 * Convenience overload to fix the fact that the uORB instance is 0-based but the failure_injection instance is 1-based.
 */
سانچہ<قسم_نام پیغام_T>
منطقی process(ثابت Config &config, uint8_t unit, uint8_t uorb_instance, پیغام_T &پیغام_2, Stuck<پیغام_T> &stuck)
{
	واپس process(config.منوال(unit, uorb_instance + 1), پیغام_2, stuck);
}

/**
 * Message-less variant for producers with no payload to replay (e.g. a heartbeat)
 *
 * @return false if the signal must be treated as failed/suppressed (Off), true otherwise.
 */
اندرونی منطقی process(منوال_2 منوال)
{
	واپس منوال != منوال_2::Off;
}

/** Convenience overload, same 0-based to 1-based instance mapping as the generic process(). */
اندرونی منطقی process(ثابت Config &config, uint8_t unit, uint8_t uorb_instance)
{
	واپس process(config.منوال(unit, uorb_instance + 1));
}

/**
 * Battery counterpart to process(): on FAILURE_UNIT_SYSTEM_BATTERY for the given 1-based
 * instance, Off suppresses the publication (the pack reads disconnected) and Wrong reports the
 * warning level selected by SYS_FAIL_BAT_LVL with the remaining charge just below the matching
 * threshold, so that stage of the low-battery failsafe triggers.
 *
 * @return false if the battery_status publication must be suppressed (Off), true otherwise.
 */
منطقی process_battery(ثابت Config &config, uint8_t instance, battery_حالت_s &battery_حالت);

/**
 * GNSS counterpart to process(): on FAILURE_UNIT_SENSOR_GPS for the receiver publishing on the
 * given 0-based uORB instance, Off and Stuck behave as in the generic process() and Wrong reports
 * the fix type selected by SYS_FAIL_GPS_WRG and the jamming state selected by SYS_FAIL_GPS_JAM
 * while leaving the position untouched.
 *
 * @param uorb_instance 0-based uORB instance of the publisher (not the 1-based failure instance).
 * @return false if the sensor_gnss publication must be suppressed (Off), true otherwise.
 */
منطقی process_gnss(ثابت Config &config, uint8_t uorb_instance, sensor_gnss_s &sensor_gnss,
		  Stuck<sensor_gnss_s> &stuck);

/**
 * ESC counterpart to process(): apply the active FAILURE_UNIT_SYSTEM_ESC failures to a copy of
 * status (matched per ESC by actuator_function). Off zeroes the ESC's telemetry (keeping only
 * actuator_function) and reports it offline and unarmed; Wrong keeps it online but reports
 * consistently wrong values. Takes status by const reference and returns the mutated copy so callers
 * can't accidentally apply it in place to a persistent status, which would compound Wrong across
 * calls. Call after Config::update().
 */
esc_حالت_s process_esc(ثابت Config &config, ثابت esc_حالت_s &حالت_2);

/**
 * Motor counterpart to process(): derive the masks of motors failed by
 * FAILURE_UNIT_SYSTEM_MOTOR from the injected failure type. Off is the detected failure
 * (failure_mask: reported as failed motors and removed from the allocation), Wrong the
 * undetected one (stop_mask: outputs stopped without informing the allocator).
 * Call after Config::update().
 */
MotorFailureMasks process_motor(ثابت Config &config);

#ورنہ // !CONFIG_MODULES_FAILURE_INJECTION_MANAGER

جماعت Config
{
عوامی:
	منطقی update() { واپس جھوٹ; }
	خالی سیٹ(ثابت failure_injection_s &) {}
	منوال_2 منوال(uint8_t, uint8_t) ثابت { واپس منوال_2::Ok; }
	منطقی any_فعال() ثابت { واپس جھوٹ; }
};

سانچہ<قسم_نام پیغام_T>
ساخت Stuck {
};

سانچہ<قسم_نام پیغام_T>
منطقی process(منوال_2, پیغام_T &, Stuck<پیغام_T> &) { واپس سچ; }

سانچہ<قسم_نام پیغام_T>
منطقی process(ثابت Config &, uint8_t, uint8_t, پیغام_T &, Stuck<پیغام_T> &) { واپس سچ; }

اندرونی منطقی process(منوال_2) { واپس سچ; }

اندرونی منطقی process(ثابت Config &, uint8_t, uint8_t) { واپس سچ; }

اندرونی منطقی process_battery(ثابت Config &, uint8_t, battery_حالت_s &) { واپس سچ; }

اندرونی منطقی process_gnss(ثابت Config &, uint8_t, sensor_gnss_s &, Stuck<sensor_gnss_s> &) { واپس سچ; }

اندرونی esc_حالت_s process_esc(ثابت Config &, ثابت esc_حالت_s &حالت_2) { واپس حالت_2; }

اندرونی MotorFailureMasks process_motor(ثابت Config &) { واپس {}; }

#اگر_ختم // CONFIG_MODULES_FAILURE_INJECTION_MANAGER

} // namespace failure_injection
