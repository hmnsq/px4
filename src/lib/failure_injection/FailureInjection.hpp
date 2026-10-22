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

#निर्देशः once

#अन्तर्भाव <cstdint>

#अन्तर्भाव <uORB/Subscription.hpp>
#अन्तर्भाव <uORB/topics/esc_status.h>
#अन्तर्भाव <uORB/topics/failure_injection.h>

संरचना battery_स्थितिः_s;
संरचना sensor_gnss_s;

नामाकाश failure_injection
{

गणना वर्ग बहुलकम्_2 : uint8_t {
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
संरचना MotorFailureMasks {
	uint16_t stop_आवरणम्{0};    ///< motor wrong: outputs stopped without informing the allocator
	uint16_t failure_आवरणम्{0}; ///< motor off: reported as failed motors, removed from the allocation
};

#यदि defined(CONFIG_MODULES_FAILURE_INJECTION_MANAGER)

वर्ग Config
{
सार्वजनिक:
	/**
	 * Poll the failure_injection subscription and, when a new failure_injection
	 * message arrives, refresh the cached failure config that mode() / any_active()
	 * read. Call once per loop iteration.
	 * @return true if an updated failure config was applied, false if unchanged.
	 */
	तार्किक update();

	/** Cache a failure_injection message directly (used by tests). */
	शून्य समुच्चयः(ध्रुव failure_injection_s &cfg);

	/**
	 * Active failure mode for a (unit, instance), or Mode::Ok if none.
	 * @param unit     one of failure_injection_s::FAILURE_UNIT_*
	 * @param instance 1-based instance, or 0 to match any instance of the unit
	 */
	बहुलकम्_2 बहुलकम्(uint8_t unit, uint8_t instance) ध्रुव;

	/** True if any failure is currently active (free no-failure early-out). */
	तार्किक any_सक्रिय() ध्रुव { प्रत्यागम गणनम्_3 > 0; }

गुप्त:
	uORB::Subscription _sub{ORB_ID(failure_injection)};

	uint8_t  गणनम्_3{0};
	uint8_t  _unit[failure_injection_s::MAX_FAILURES] {};
	uint16_t instance_आवरणम्[failure_injection_s::MAX_FAILURES] {};
	बहुलकम्_2     _failure_type[failure_injection_s::MAX_FAILURES] {};
};

/**
 * Per-(consumer, instance) state for stateful failures. Holds the last good
 * sample so Stuck can replay it.
 */
प्रतिरूपम्<प्रकारनाम सन्देशः_T>
संरचना Stuck {
	तार्किक valid{असत्य};
	सन्देशः_T value{};
};

प्रतिरूपम्<प्रकारनाम...>
उपयुज्य void_t = शून्य;

प्रतिरूपम्<प्रकारनाम T, प्रकारनाम = शून्य>
संरचना has_timestamp_sample {
	स्थिर constexpr तार्किक value = असत्य;
};
प्रतिरूपम्<प्रकारनाम T>
संरचना has_timestamp_sample<T, void_t<decltype(T::timestamp_sample)>> {
	स्थिर constexpr तार्किक value = सत्य;
};

/**
 * Generic whole-message processor for the Ok / Off / Stuck mechanics, for consumers
 * that publish a single message and want the message-agnostic behaviour.
 *
 * @return false if the message must be suppressed (Off); true otherwise. For
 *         Stuck the message is overwritten with the last good sample.
 */
प्रतिरूपम्<प्रकारनाम सन्देशः_T>
तार्किक process(बहुलकम्_2 बहुलकम्, सन्देशः_T &सन्देशः_2, Stuck<सन्देशः_T> &stuck)
{
	विकल्प (बहुलकम्) {
	पक्ष बहुलकम्_2::Off:
		प्रत्यागम असत्य;

	पक्ष बहुलकम्_2::Stuck:
		यदि (stuck.valid) {
			ध्रुव uint64_t timestamp = सन्देशः_2.timestamp;

			यदि constexpr(has_timestamp_sample<सन्देशः_T>::value) {
				ध्रुव uint64_t timestamp_sample = सन्देशः_2.timestamp_sample;
				सन्देशः_2 = stuck.value;
				सन्देशः_2.timestamp_sample = timestamp_sample;

			} अन्यथा {
				सन्देशः_2 = stuck.value;
			}

			सन्देशः_2.timestamp = timestamp;
		}

		प्रत्यागम सत्य;

	स्वभाव:
		stuck.value = सन्देशः_2;
		stuck.valid = सत्य;
		प्रत्यागम सत्य;
	}
}

/**
 * Convenience overload to fix the fact that the uORB instance is 0-based but the failure_injection instance is 1-based.
 */
प्रतिरूपम्<प्रकारनाम सन्देशः_T>
तार्किक process(ध्रुव Config &config, uint8_t unit, uint8_t uorb_instance, सन्देशः_T &सन्देशः_2, Stuck<सन्देशः_T> &stuck)
{
	प्रत्यागम process(config.बहुलकम्(unit, uorb_instance + 1), सन्देशः_2, stuck);
}

/**
 * Message-less variant for producers with no payload to replay (e.g. a heartbeat)
 *
 * @return false if the signal must be treated as failed/suppressed (Off), true otherwise.
 */
अन्तःस्थ तार्किक process(बहुलकम्_2 बहुलकम्)
{
	प्रत्यागम बहुलकम् != बहुलकम्_2::Off;
}

/** Convenience overload, same 0-based to 1-based instance mapping as the generic process(). */
अन्तःस्थ तार्किक process(ध्रुव Config &config, uint8_t unit, uint8_t uorb_instance)
{
	प्रत्यागम process(config.बहुलकम्(unit, uorb_instance + 1));
}

/**
 * Battery counterpart to process(): on FAILURE_UNIT_SYSTEM_BATTERY for the given 1-based
 * instance, Off suppresses the publication (the pack reads disconnected) and Wrong reports the
 * warning level selected by SYS_FAIL_BAT_LVL with the remaining charge just below the matching
 * threshold, so that stage of the low-battery failsafe triggers.
 *
 * @return false if the battery_status publication must be suppressed (Off), true otherwise.
 */
तार्किक process_battery(ध्रुव Config &config, uint8_t instance, battery_स्थितिः_s &battery_स्थितिः);

/**
 * GNSS counterpart to process(): on FAILURE_UNIT_SENSOR_GPS for the receiver publishing on the
 * given 0-based uORB instance, Off and Stuck behave as in the generic process() and Wrong reports
 * the fix type selected by SYS_FAIL_GPS_WRG and the jamming state selected by SYS_FAIL_GPS_JAM
 * while leaving the position untouched.
 *
 * @param uorb_instance 0-based uORB instance of the publisher (not the 1-based failure instance).
 * @return false if the sensor_gnss publication must be suppressed (Off), true otherwise.
 */
तार्किक process_gnss(ध्रुव Config &config, uint8_t uorb_instance, sensor_gnss_s &sensor_gnss,
		  Stuck<sensor_gnss_s> &stuck);

/**
 * ESC counterpart to process(): apply the active FAILURE_UNIT_SYSTEM_ESC failures to a copy of
 * status (matched per ESC by actuator_function). Off zeroes the ESC's telemetry (keeping only
 * actuator_function) and reports it offline and unarmed; Wrong keeps it online but reports
 * consistently wrong values. Takes status by const reference and returns the mutated copy so callers
 * can't accidentally apply it in place to a persistent status, which would compound Wrong across
 * calls. Call after Config::update().
 */
esc_स्थितिः_s process_esc(ध्रुव Config &config, ध्रुव esc_स्थितिः_s &स्थितिः);

/**
 * Motor counterpart to process(): derive the masks of motors failed by
 * FAILURE_UNIT_SYSTEM_MOTOR from the injected failure type. Off is the detected failure
 * (failure_mask: reported as failed motors and removed from the allocation), Wrong the
 * undetected one (stop_mask: outputs stopped without informing the allocator).
 * Call after Config::update().
 */
MotorFailureMasks process_motor(ध्रुव Config &config);

#अन्यथा // !CONFIG_MODULES_FAILURE_INJECTION_MANAGER

वर्ग Config
{
सार्वजनिक:
	तार्किक update() { प्रत्यागम असत्य; }
	शून्य समुच्चयः(ध्रुव failure_injection_s &) {}
	बहुलकम्_2 बहुलकम्(uint8_t, uint8_t) ध्रुव { प्रत्यागम बहुलकम्_2::Ok; }
	तार्किक any_सक्रिय() ध्रुव { प्रत्यागम असत्य; }
};

प्रतिरूपम्<प्रकारनाम सन्देशः_T>
संरचना Stuck {
};

प्रतिरूपम्<प्रकारनाम सन्देशः_T>
तार्किक process(बहुलकम्_2, सन्देशः_T &, Stuck<सन्देशः_T> &) { प्रत्यागम सत्य; }

प्रतिरूपम्<प्रकारनाम सन्देशः_T>
तार्किक process(ध्रुव Config &, uint8_t, uint8_t, सन्देशः_T &, Stuck<सन्देशः_T> &) { प्रत्यागम सत्य; }

अन्तःस्थ तार्किक process(बहुलकम्_2) { प्रत्यागम सत्य; }

अन्तःस्थ तार्किक process(ध्रुव Config &, uint8_t, uint8_t) { प्रत्यागम सत्य; }

अन्तःस्थ तार्किक process_battery(ध्रुव Config &, uint8_t, battery_स्थितिः_s &) { प्रत्यागम सत्य; }

अन्तःस्थ तार्किक process_gnss(ध्रुव Config &, uint8_t, sensor_gnss_s &, Stuck<sensor_gnss_s> &) { प्रत्यागम सत्य; }

अन्तःस्थ esc_स्थितिः_s process_esc(ध्रुव Config &, ध्रुव esc_स्थितिः_s &स्थितिः) { प्रत्यागम स्थितिः; }

अन्तःस्थ MotorFailureMasks process_motor(ध्रुव Config &) { प्रत्यागम {}; }

#यदि_अन्तः // CONFIG_MODULES_FAILURE_INJECTION_MANAGER

} // namespace failure_injection
