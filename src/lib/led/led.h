/****************************************************************************
 *
 *   Copyright (c) 2017 PX4 Development Team. All rights reserved.
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
 * @file led.h
 *
 * Led controller helper class, used by Led drivers
 *
 * @author Beat Küng <beat-kueng@gmx.net>
 */

#pragma once

#অন্তর্ভাব <drivers/drv_hrt.h>
#অন্তর্ভাব <drivers/drv_led.h>
#অন্তর্ভাব <px4_platform_common/module_params.h>
#অন্তর্ভাব <lib/parameters/param.h>
#অন্তর্ভাব <lib/perf/perf_counter.h>
#অন্তর্ভাব <uORB/Subscription.hpp>
#অন্তর্ভাব <uORB/SubscriptionInterval.hpp>
#অন্তর্ভাব <uORB/topics/led_control.h>
#অন্তর্ভাব <uORB/topics/parameter_update.h>

using namespace time_literals;

কাঠামো Led_Control_তথ্য_Single {
	uint8_t color{led_control_s::COLOR_OFF}; ///< one of led_control_s::COLOR_*
	uint8_t brightness{0}; ///< brightness in [0, 255]
};
কাঠামো Led_Control_তথ্য {
	Led_Control_তথ্য_Single leds[BOARD_MAX_LEDS] {};
};


/**
 ** class LedController
 * Handles the led_control topic: blinking, priorities and state updates.
 */
class LedController : সর্বজনীন ModuleParams
{
সর্বজনীন:
	LedController() : ModuleParams(nullptr) {}
	~LedController() override
	{
		perf_free(_led_control_sub_lost_perf);
	}

	/**
	 * get maximum time between two consecutive calls to update() in us.
	 */
	পূর্ণসংখ্যা maximum_update_interval() ধ্রুবক
	{
		ফেরত _breathe_enabled ? BREATHE_INTERVAL : BLINK_FAST_DURATION;
	}

	/**
	 * Update and retrieve the Led state. It will do the orb_copy() and needs to be called at least every
	 * maximum_update_interval(). In addition a caller might poll on the led_control_sub
	 * @param control_data output structure (will always be set)
	 * @return 1 if control_data set (state changed), 0 if control_data not changed (state did not change), <0 error otherwise
	 */
	পূর্ণসংখ্যা update(Led_Control_তথ্য &control_তথ্য);

	স্থির constexpr পূর্ণসংখ্যা BREATHE_INTERVAL = 25 * 1000; /**< single step when in breathe mode */
	স্থির constexpr পূর্ণসংখ্যা BREATHE_STEPS = 64; /**< number of steps in breathe mode for a full on-off cycle */

	স্থির constexpr পূর্ণসংখ্যা BLINK_FAST_DURATION = 100 * 1000; /**< duration of half a blinking cycle
									(on-to-off and off-to-on) in us */
	স্থির constexpr পূর্ণসংখ্যা BLINK_NORMAL_DURATION = 500 * 1000; /**< duration of half a blinking cycle
									(on-to-off and off-to-on) in us */
	স্থির constexpr পূর্ণসংখ্যা BLINK_SLOW_DURATION = 2000 * 1000; /**< duration of half a blinking cycle
									(on-to-off and off-to-on) in us */

ব্যক্তিগত:

	/** set control_data based on current Led states */
	অন্তঃরেখ শূন্য get_control_তথ্য(Led_Control_তথ্য &control_তথ্য);

	কাঠামো Per_Priority_তথ্য {
		uint8_t color = 0; ///< one of led_control_s::COLOR_*
		uint8_t mode = led_control_s::MODE_DISABLED; ///< one of led_control_s::MODE_*
		uint8_t blink_times_বাম = 0; /**< how many times left to blink (MSB bit is used for infinite case).
									This limits the number of complete blink cycles to 64 (if not infinite) */
	};

	কাঠামো পরবর্তী_State {
		uint8_t color;
		uint8_t mode;
		uint8_t অঙ্ক_blinks;
		uint8_t priority = led_control_s::MAX_PRIORITY + 1;

		শূন্য set(ধ্রুবক led_control_s &led_control)
		{
			color = led_control.color;
			mode = led_control.mode;
			অঙ্ক_blinks = led_control.অঙ্ক_blinks;
			priority = led_control.priority;

			যদি (priority > led_control_s::MAX_PRIORITY) {
				priority = led_control_s::MAX_PRIORITY;
			}
		}
		শূন্য reset() { priority = led_control_s::MAX_PRIORITY + 1; }
		bool is_valid() ধ্রুবক { ফেরত priority != led_control_s::MAX_PRIORITY + 1; }
	};

	কাঠামো Per_Led_তথ্য {
		Per_Priority_তথ্য priority[led_control_s::MAX_PRIORITY + 1];
		uint16_t current_blinking_time = 0; ///< how long the Led was in current state (in 0.1 ms, wraps if > 6.5s)
		পরবর্তী_State পরবর্তী_state;

		শূন্য set(ধ্রুবক led_control_s &led_control)
		{
			পূর্ণসংখ্যা পরবর্তী_priority = (পূর্ণসংখ্যা)led_control.priority;
			priority[পরবর্তী_priority].color = led_control.color;
			priority[পরবর্তী_priority].mode = led_control.mode;

			// initialise the flash counter
			যদি (led_control.mode == led_control_s::MODE_FLASH) {
				priority[পরবর্তী_priority].blink_times_বাম = led_control.অঙ্ক_blinks * 10;

			} অন্যথা {
				priority[পরবর্তী_priority].blink_times_বাম = led_control.অঙ্ক_blinks * 2;
			}

			যদি (priority[পরবর্তী_priority].blink_times_বাম == 0) {
				// handle infinite case
				priority[পরবর্তী_priority].blink_times_বাম = 246;
			}
		}

		শূন্য apply_পরবর্তী_state()
		{
			পূর্ণসংখ্যা পরবর্তী_priority = (পূর্ণসংখ্যা)পরবর্তী_state.priority;
			priority[পরবর্তী_priority].color = পরবর্তী_state.color;
			priority[পরবর্তী_priority].mode = পরবর্তী_state.mode;

			যদি (পরবর্তী_state.mode == led_control_s::MODE_FLASH) {
				priority[পরবর্তী_priority].blink_times_বাম = পরবর্তী_state.অঙ্ক_blinks * 10;

			} অন্যথা {
				priority[পরবর্তী_priority].blink_times_বাম = পরবর্তী_state.অঙ্ক_blinks * 2;
			}

			যদি (priority[পরবর্তী_priority].blink_times_বাম == 0) {
				// handle infinite case
				priority[পরবর্তী_priority].blink_times_বাম = 254;
			}
		}
	};

	Per_Led_তথ্য _states[BOARD_MAX_LEDS]; ///< keep current LED states

	uORB::Subscription _led_control_sub{ORB_ID(led_control)}; ///< uorb subscription
	uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

	hrt_abstime _last_update_call{0};

	perf_গণক_t _led_control_sub_lost_perf{perf_alloc(PC_গুনতি, MODULE_NAME": led_control message missed")};

	uint8_t _max_brightness{UINT8_MAX};

	bool _force_update{true}; ///< force an orb_copy in the beginning
	bool _breathe_enabled{false}; ///< true if at least one of the led's is currently in breathe mode

	DEFINE_PARAMETERS(
		(ParamFloat<px4::params::SYS_RGB_MAXBRT>) _param_sys_rgb_maxbrt
	)
};
