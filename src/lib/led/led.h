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

#لارښوونه once

#ضمن <drivers/drv_hrt.h>
#ضمن <drivers/drv_led.h>
#ضمن <px4_platform_common/module_params.h>
#ضمن <lib/parameters/param.h>
#ضمن <lib/perf/perf_counter.h>
#ضمن <uORB/Subscription.hpp>
#ضمن <uORB/SubscriptionInterval.hpp>
#ضمن <uORB/topics/led_control.h>
#ضمن <uORB/topics/parameter_update.h>

using namespace وخت_literals;

بنية Led_Control_معلومات_Single {
	uint8_t color{led_control_s::COLOR_OFF}; ///< one of led_control_s::COLOR_*
	uint8_t brightness{0}; ///< brightness in [0, 255]
};
بنية Led_Control_معلومات {
	Led_Control_معلومات_Single leds[BOARD_MAX_LEDS] {};
};


/**
 ** class LedController
 * Handles the led_control topic: blinking, priorities and state updates.
 */
class LedController : عام ModuleParams
{
عام:
	LedController() : ModuleParams(nullptr) {}
	~LedController() override
	{
		perf_free(led_control_sub_ورک_perf);
	}

	/**
	 * get maximum time between two consecutive calls to update() in us.
	 */
	صحيح اعظمي_update_interval() ثابت
	{
		أرجع _breathe_enabled ? BREATHE_INTERVAL : BLINK_FAST_DURATION;
	}

	/**
	 * Update and retrieve the Led state. It will do the orb_copy() and needs to be called at least every
	 * maximum_update_interval(). In addition a caller might poll on the led_control_sub
	 * @param control_data output structure (will always be set)
	 * @return 1 if control_data set (state changed), 0 if control_data not changed (state did not change), <0 error otherwise
	 */
	صحيح update(Led_Control_معلومات &control_معلومات);

	ساكن constexpr صحيح BREATHE_INTERVAL = 25 * 1000; /**< single step when in breathe mode */
	ساكن constexpr صحيح BREATHE_ګامونه = 64; /**< number of steps in breathe mode for a full on-off cycle */

	ساكن constexpr صحيح BLINK_FAST_DURATION = 100 * 1000; /**< duration of half a blinking cycle
									(on-to-off and off-to-on) in us */
	ساكن constexpr صحيح BLINK_نورمال_DURATION = 500 * 1000; /**< duration of half a blinking cycle
									(on-to-off and off-to-on) in us */
	ساكن constexpr صحيح BLINK_SLOW_DURATION = 2000 * 1000; /**< duration of half a blinking cycle
									(on-to-off and off-to-on) in us */

خاص:

	/** set control_data based on current Led states */
	مضمن فراغ get_control_معلومات(Led_Control_معلومات &control_معلومات);

	بنية فی_لومړیتوب_معلومات {
		uint8_t color = 0; ///< one of led_control_s::COLOR_*
		uint8_t ډېرپېښ = led_control_s::ډېرپېښ_DISABLED; ///< one of led_control_s::MODE_*
		uint8_t blink_times_کیڼ = 0; /**< how many times left to blink (MSB bit is used for infinite case).
									This limits the number of complete blink cycles to 64 (if not infinite) */
	};

	بنية بل_State {
		uint8_t color;
		uint8_t ډېرپېښ;
		uint8_t عدد_blinks;
		uint8_t لومړیتوب = led_control_s::MAX_لومړیتوب + 1;

		فراغ سېټ(ثابت led_control_s &led_control)
		{
			color = led_control.color;
			ډېرپېښ = led_control.ډېرپېښ;
			عدد_blinks = led_control.عدد_blinks;
			لومړیتوب = led_control.لومړیتوب;

			إذا (لومړیتوب > led_control_s::MAX_لومړیتوب) {
				لومړیتوب = led_control_s::MAX_لومړیتوب;
			}
		}
		فراغ reset() { لومړیتوب = led_control_s::MAX_لومړیتوب + 1; }
		bool is_valid() ثابت { أرجع لومړیتوب != led_control_s::MAX_لومړیتوب + 1; }
	};

	بنية فی_Led_معلومات {
		فی_لومړیتوب_معلومات لومړیتوب[led_control_s::MAX_لومړیتوب + 1];
		uint16_t current_blinking_وخت = 0; ///< how long the Led was in current state (in 0.1 ms, wraps if > 6.5s)
		بل_State بل_state;

		فراغ سېټ(ثابت led_control_s &led_control)
		{
			صحيح بل_لومړیتوب = (صحيح)led_control.لومړیتوب;
			لومړیتوب[بل_لومړیتوب].color = led_control.color;
			لومړیتوب[بل_لومړیتوب].ډېرپېښ = led_control.ډېرپېښ;

			// initialise the flash counter
			إذا (led_control.ډېرپېښ == led_control_s::ډېرپېښ_FLASH) {
				لومړیتوب[بل_لومړیتوب].blink_times_کیڼ = led_control.عدد_blinks * 10;

			} وإلا {
				لومړیتوب[بل_لومړیتوب].blink_times_کیڼ = led_control.عدد_blinks * 2;
			}

			إذا (لومړیتوب[بل_لومړیتوب].blink_times_کیڼ == 0) {
				// handle infinite case
				لومړیتوب[بل_لومړیتوب].blink_times_کیڼ = 246;
			}
		}

		فراغ apply_بل_state()
		{
			صحيح بل_لومړیتوب = (صحيح)بل_state.لومړیتوب;
			لومړیتوب[بل_لومړیتوب].color = بل_state.color;
			لومړیتوب[بل_لومړیتوب].ډېرپېښ = بل_state.ډېرپېښ;

			إذا (بل_state.ډېرپېښ == led_control_s::ډېرپېښ_FLASH) {
				لومړیتوب[بل_لومړیتوب].blink_times_کیڼ = بل_state.عدد_blinks * 10;

			} وإلا {
				لومړیتوب[بل_لومړیتوب].blink_times_کیڼ = بل_state.عدد_blinks * 2;
			}

			إذا (لومړیتوب[بل_لومړیتوب].blink_times_کیڼ == 0) {
				// handle infinite case
				لومړیتوب[بل_لومړیتوب].blink_times_کیڼ = 254;
			}
		}
	};

	فی_Led_معلومات حالتونه[BOARD_MAX_LEDS]; ///< keep current LED states

	uORB::Subscription _led_control_sub{ORB_ID(led_control)}; ///< uorb subscription
	uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

	hrt_abstime وروستی_update_call{0};

	perf_counter_t led_control_sub_ورک_perf{perf_alloc(PC_شمېر, MODULE_NAME": led_control message missed")};

	uint8_t _max_brightness{UINT8_MAX};

	bool _force_update{true}; ///< force an orb_copy in the beginning
	bool _breathe_enabled{false}; ///< true if at least one of the led's is currently in breathe mode

	DEFINE_PARAMETERS(
		(ParamFloat<px4::params::SYS_RGB_MAXBRT>) _param_sys_rgb_maxbrt
	)
};
