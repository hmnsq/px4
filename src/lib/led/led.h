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

#അന്തര്ഭാവ <drivers/drv_hrt.h>
#അന്തര്ഭാവ <drivers/drv_led.h>
#അന്തര്ഭാവ <px4_platform_common/module_params.h>
#അന്തര്ഭാവ <lib/parameters/param.h>
#അന്തര്ഭാവ <lib/perf/perf_counter.h>
#അന്തര്ഭാവ <uORB/Subscription.hpp>
#അന്തര്ഭാവ <uORB/SubscriptionInterval.hpp>
#അന്തര്ഭാവ <uORB/topics/led_control.h>
#അന്തര്ഭാവ <uORB/topics/parameter_update.h>

using namespace time_literals;

ഘടന LedControlDataSingle {
	uint8_t color{led_control_s::COLOR_OFF}; ///< one of led_control_s::COLOR_*
	uint8_t brightness{0}; ///< brightness in [0, 255]
};
ഘടന LedControlData {
	LedControlDataSingle leds[BOARD_MAX_LEDS] {};
};


/**
 ** class LedController
 * Handles the led_control topic: blinking, priorities and state updates.
 */
class LedController : പൊതു ModuleParams
{
പൊതു:
	LedController() : ModuleParams(nullptr) {}
	~LedController() override
	{
		perf_free(_led_control_sub_lost_perf);
	}

	/**
	 * get maximum time between two consecutive calls to update() in us.
	 */
	പൂർണ്ണസംഖ്യ maximum_update_interval() സ്ഥിരം
	{
		തിരികെ _breathe_enabled ? BREATHE_INTERVAL : BLINK_FAST_DURATION;
	}

	/**
	 * Update and retrieve the Led state. It will do the orb_copy() and needs to be called at least every
	 * maximum_update_interval(). In addition a caller might poll on the led_control_sub
	 * @param control_data output structure (will always be set)
	 * @return 1 if control_data set (state changed), 0 if control_data not changed (state did not change), <0 error otherwise
	 */
	പൂർണ്ണസംഖ്യ update(LedControlData &control_data);

	സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ BREATHE_INTERVAL = 25 * 1000; /**< single step when in breathe mode */
	സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ BREATHE_STEPS = 64; /**< number of steps in breathe mode for a full on-off cycle */

	സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ BLINK_FAST_DURATION = 100 * 1000; /**< duration of half a blinking cycle
									(on-to-off and off-to-on) in us */
	സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ BLINK_NORMAL_DURATION = 500 * 1000; /**< duration of half a blinking cycle
									(on-to-off and off-to-on) in us */
	സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ BLINK_SLOW_DURATION = 2000 * 1000; /**< duration of half a blinking cycle
									(on-to-off and off-to-on) in us */

സ്വകാര്യ:

	/** set control_data based on current Led states */
	വരിയിൽ ശൂന്യം get_control_data(LedControlData &control_data);

	ഘടന PerPriorityData {
		uint8_t color = 0; ///< one of led_control_s::COLOR_*
		uint8_t mode = led_control_s::MODE_DISABLED; ///< one of led_control_s::MODE_*
		uint8_t blink_times_ഇടത് = 0; /**< how many times left to blink (MSB bit is used for infinite case).
									This limits the number of complete blink cycles to 64 (if not infinite) */
	};

	ഘടന NextState {
		uint8_t color;
		uint8_t mode;
		uint8_t അക്കം_blinks;
		uint8_t priority = led_control_s::MAX_PRIORITY + 1;

		ശൂന്യം set(സ്ഥിരം led_control_s &led_control)
		{
			color = led_control.color;
			mode = led_control.mode;
			അക്കം_blinks = led_control.അക്കം_blinks;
			priority = led_control.priority;

			എങ്കിൽ (priority > led_control_s::MAX_PRIORITY) {
				priority = led_control_s::MAX_PRIORITY;
			}
		}
		ശൂന്യം reset() { priority = led_control_s::MAX_PRIORITY + 1; }
		bool is_valid() സ്ഥിരം { തിരികെ priority != led_control_s::MAX_PRIORITY + 1; }
	};

	ഘടന PerLedData {
		PerPriorityData priority[led_control_s::MAX_PRIORITY + 1];
		uint16_t current_blinking_time = 0; ///< how long the Led was in current state (in 0.1 ms, wraps if > 6.5s)
		NextState next_state;

		ശൂന്യം set(സ്ഥിരം led_control_s &led_control)
		{
			പൂർണ്ണസംഖ്യ next_priority = (പൂർണ്ണസംഖ്യ)led_control.priority;
			priority[next_priority].color = led_control.color;
			priority[next_priority].mode = led_control.mode;

			// initialise the flash counter
			എങ്കിൽ (led_control.mode == led_control_s::MODE_FLASH) {
				priority[next_priority].blink_times_ഇടത് = led_control.അക്കം_blinks * 10;

			} അല്ലെങ്കിൽ {
				priority[next_priority].blink_times_ഇടത് = led_control.അക്കം_blinks * 2;
			}

			എങ്കിൽ (priority[next_priority].blink_times_ഇടത് == 0) {
				// handle infinite case
				priority[next_priority].blink_times_ഇടത് = 246;
			}
		}

		ശൂന്യം apply_next_state()
		{
			പൂർണ്ണസംഖ്യ next_priority = (പൂർണ്ണസംഖ്യ)next_state.priority;
			priority[next_priority].color = next_state.color;
			priority[next_priority].mode = next_state.mode;

			എങ്കിൽ (next_state.mode == led_control_s::MODE_FLASH) {
				priority[next_priority].blink_times_ഇടത് = next_state.അക്കം_blinks * 10;

			} അല്ലെങ്കിൽ {
				priority[next_priority].blink_times_ഇടത് = next_state.അക്കം_blinks * 2;
			}

			എങ്കിൽ (priority[next_priority].blink_times_ഇടത് == 0) {
				// handle infinite case
				priority[next_priority].blink_times_ഇടത് = 254;
			}
		}
	};

	PerLedData _states[BOARD_MAX_LEDS]; ///< keep current LED states

	uORB::Subscription _led_control_sub{ORB_ID(led_control)}; ///< uorb subscription
	uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

	hrt_abstime _last_update_call{0};

	perf_counter_t _led_control_sub_lost_perf{perf_alloc(PC_എണ്ണം, MODULE_NAME": led_control message missed")};

	uint8_t _max_brightness{UINT8_MAX};

	bool _force_update{true}; ///< force an orb_copy in the beginning
	bool _breathe_enabled{false}; ///< true if at least one of the led's is currently in breathe mode

	DEFINE_PARAMETERS(
		(ParamFloat<px4::params::SYS_RGB_MAXBRT>) _param_sys_rgb_maxbrt
	)
};
