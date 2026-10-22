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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_hrt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_led.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/module_params.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/parameters/param.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/perf/perf_counter.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/Subscription.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/SubscriptionInterval.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/led_control.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/parameter_update.h>

using namespace ਸਮਾਂ_literals;

ਬਣਤਰ Led_Control_ਅੰਕੜੇ_Single {
	uint8_t color{led_control_s::COLOR_OFF}; ///< one of led_control_s::COLOR_*
	uint8_t brightness{0}; ///< brightness in [0, 255]
};
ਬਣਤਰ Led_Control_ਅੰਕੜੇ {
	Led_Control_ਅੰਕੜੇ_Single leds[BOARD_MAX_LEDS] {};
};


/**
 ** class LedController
 * Handles the led_control topic: blinking, priorities and state updates.
 */
class LedController : ਜਨਤਕ ModuleParams
{
ਜਨਤਕ:
	LedController() : ModuleParams(nullptr) {}
	~LedController() override
	{
		perf_free(led_control_sub_ਗੁਆਚਾ_perf);
	}

	/**
	 * get maximum time between two consecutive calls to update() in us.
	 */
	ਪੂਰਨ ਵੱਧ_ਤੋਂ_ਵੱਧ_update_interval() ਅਚਲ
	{
		ਵਾਪਸ _breathe_enabled ? BREATHE_INTERVAL : BLINK_FAST_DURATION;
	}

	/**
	 * Update and retrieve the Led state. It will do the orb_copy() and needs to be called at least every
	 * maximum_update_interval(). In addition a caller might poll on the led_control_sub
	 * @param control_data output structure (will always be set)
	 * @return 1 if control_data set (state changed), 0 if control_data not changed (state did not change), <0 error otherwise
	 */
	ਪੂਰਨ update(Led_Control_ਅੰਕੜੇ &control_ਅੰਕੜੇ);

	ਸਥਿਰ constexpr ਪੂਰਨ BREATHE_INTERVAL = 25 * 1000; /**< single step when in breathe mode */
	ਸਥਿਰ constexpr ਪੂਰਨ BREATHE_ਕਦਮਾਂ = 64; /**< number of steps in breathe mode for a full on-off cycle */

	ਸਥਿਰ constexpr ਪੂਰਨ BLINK_FAST_DURATION = 100 * 1000; /**< duration of half a blinking cycle
									(on-to-off and off-to-on) in us */
	ਸਥਿਰ constexpr ਪੂਰਨ BLINK_ਸਾਧਾਰਨ_DURATION = 500 * 1000; /**< duration of half a blinking cycle
									(on-to-off and off-to-on) in us */
	ਸਥਿਰ constexpr ਪੂਰਨ BLINK_SLOW_DURATION = 2000 * 1000; /**< duration of half a blinking cycle
									(on-to-off and off-to-on) in us */

ਨਿਜੀ:

	/** set control_data based on current Led states */
	ਸਤਰਅੰਦਰ ਖਾਲੀ get_control_ਅੰਕੜੇ(Led_Control_ਅੰਕੜੇ &control_ਅੰਕੜੇ);

	ਬਣਤਰ ਪ੍ਰਤੀ_ਤਰਜੀਹ_ਅੰਕੜੇ {
		uint8_t color = 0; ///< one of led_control_s::COLOR_*
		uint8_t ਬਹੁਲਕ = led_control_s::ਬਹੁਲਕ_DISABLED; ///< one of led_control_s::MODE_*
		uint8_t blink_times_ਖੱਬਾ = 0; /**< how many times left to blink (MSB bit is used for infinite case).
									This limits the number of complete blink cycles to 64 (if not infinite) */
	};

	ਬਣਤਰ ਅਗਲਾ_State {
		uint8_t color;
		uint8_t ਬਹੁਲਕ;
		uint8_t ਅੰਕ_blinks;
		uint8_t ਤਰਜੀਹ = led_control_s::MAX_ਤਰਜੀਹ + 1;

		ਖਾਲੀ ਸੈੱਟ(ਅਚਲ led_control_s &led_control)
		{
			color = led_control.color;
			ਬਹੁਲਕ = led_control.ਬਹੁਲਕ;
			ਅੰਕ_blinks = led_control.ਅੰਕ_blinks;
			ਤਰਜੀਹ = led_control.ਤਰਜੀਹ;

			ਜੇ (ਤਰਜੀਹ > led_control_s::MAX_ਤਰਜੀਹ) {
				ਤਰਜੀਹ = led_control_s::MAX_ਤਰਜੀਹ;
			}
		}
		ਖਾਲੀ reset() { ਤਰਜੀਹ = led_control_s::MAX_ਤਰਜੀਹ + 1; }
		bool is_valid() ਅਚਲ { ਵਾਪਸ ਤਰਜੀਹ != led_control_s::MAX_ਤਰਜੀਹ + 1; }
	};

	ਬਣਤਰ ਪ੍ਰਤੀ_Led_ਅੰਕੜੇ {
		ਪ੍ਰਤੀ_ਤਰਜੀਹ_ਅੰਕੜੇ ਤਰਜੀਹ[led_control_s::MAX_ਤਰਜੀਹ + 1];
		uint16_t current_blinking_ਸਮਾਂ = 0; ///< how long the Led was in current state (in 0.1 ms, wraps if > 6.5s)
		ਅਗਲਾ_State ਅਗਲਾ_state;

		ਖਾਲੀ ਸੈੱਟ(ਅਚਲ led_control_s &led_control)
		{
			ਪੂਰਨ ਅਗਲਾ_ਤਰਜੀਹ = (ਪੂਰਨ)led_control.ਤਰਜੀਹ;
			ਤਰਜੀਹ[ਅਗਲਾ_ਤਰਜੀਹ].color = led_control.color;
			ਤਰਜੀਹ[ਅਗਲਾ_ਤਰਜੀਹ].ਬਹੁਲਕ = led_control.ਬਹੁਲਕ;

			// initialise the flash counter
			ਜੇ (led_control.ਬਹੁਲਕ == led_control_s::ਬਹੁਲਕ_FLASH) {
				ਤਰਜੀਹ[ਅਗਲਾ_ਤਰਜੀਹ].blink_times_ਖੱਬਾ = led_control.ਅੰਕ_blinks * 10;

			} ਨਹੀਂਤਾਂ {
				ਤਰਜੀਹ[ਅਗਲਾ_ਤਰਜੀਹ].blink_times_ਖੱਬਾ = led_control.ਅੰਕ_blinks * 2;
			}

			ਜੇ (ਤਰਜੀਹ[ਅਗਲਾ_ਤਰਜੀਹ].blink_times_ਖੱਬਾ == 0) {
				// handle infinite case
				ਤਰਜੀਹ[ਅਗਲਾ_ਤਰਜੀਹ].blink_times_ਖੱਬਾ = 246;
			}
		}

		ਖਾਲੀ apply_ਅਗਲਾ_state()
		{
			ਪੂਰਨ ਅਗਲਾ_ਤਰਜੀਹ = (ਪੂਰਨ)ਅਗਲਾ_state.ਤਰਜੀਹ;
			ਤਰਜੀਹ[ਅਗਲਾ_ਤਰਜੀਹ].color = ਅਗਲਾ_state.color;
			ਤਰਜੀਹ[ਅਗਲਾ_ਤਰਜੀਹ].ਬਹੁਲਕ = ਅਗਲਾ_state.ਬਹੁਲਕ;

			ਜੇ (ਅਗਲਾ_state.ਬਹੁਲਕ == led_control_s::ਬਹੁਲਕ_FLASH) {
				ਤਰਜੀਹ[ਅਗਲਾ_ਤਰਜੀਹ].blink_times_ਖੱਬਾ = ਅਗਲਾ_state.ਅੰਕ_blinks * 10;

			} ਨਹੀਂਤਾਂ {
				ਤਰਜੀਹ[ਅਗਲਾ_ਤਰਜੀਹ].blink_times_ਖੱਬਾ = ਅਗਲਾ_state.ਅੰਕ_blinks * 2;
			}

			ਜੇ (ਤਰਜੀਹ[ਅਗਲਾ_ਤਰਜੀਹ].blink_times_ਖੱਬਾ == 0) {
				// handle infinite case
				ਤਰਜੀਹ[ਅਗਲਾ_ਤਰਜੀਹ].blink_times_ਖੱਬਾ = 254;
			}
		}
	};

	ਪ੍ਰਤੀ_Led_ਅੰਕੜੇ ਹਾਲਤਾਂ[BOARD_MAX_LEDS]; ///< keep current LED states

	uORB::Subscription _led_control_sub{ORB_ID(led_control)}; ///< uorb subscription
	uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

	hrt_abstime ਆਖਰੀ_update_call{0};

	perf_counter_t led_control_sub_ਗੁਆਚਾ_perf{perf_alloc(PC_ਗਿਣਤੀ, MODULE_NAME": led_control message missed")};

	uint8_t _max_brightness{UINT8_MAX};

	bool _force_update{true}; ///< force an orb_copy in the beginning
	bool _breathe_enabled{false}; ///< true if at least one of the led's is currently in breathe mode

	DEFINE_PARAMETERS(
		(ParamFloat<px4::params::SYS_RGB_MAXBRT>) _param_sys_rgb_maxbrt
	)
};
