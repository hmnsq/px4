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
 * @file led.cpp
 */


#ضمن "led.h"

صحيح LedController::update(Led_Control_معلومات &control_معلومات)
{
	منطقي had_changes = خطأ; // did one of the outputs change?

	// check for parameter updates
	إذا (_parameter_update_sub.updated()) {
		// clear update
		parameter_update_s pupdate;
		_parameter_update_sub.copy(&pupdate);

		updateParams();

		ثابت uint8_t max_brightness_prev = _max_brightness;

		// set maximum brightness (0-255) from percentage
		_max_brightness = roundf(math::constrain(_param_sys_rgb_maxbrt.get(), 0.f, 1.f) * UINT8_MAX);

		// update existing
		لكل (صحيح i = 0; i < BOARD_MAX_LEDS; ++i) {
			control_معلومات.leds[i].brightness = math::min(_max_brightness, control_معلومات.leds[i].brightness);
		}

		إذا (_max_brightness != max_brightness_prev) {
			had_changes = ریښتیا;
		}
	}

	طالما (_led_control_sub.updated() || _force_update) {
		ثابت بینښه وروستی_generation = _led_control_sub.get_وروستی_generation();

		// handle new state
		led_control_s led_control;

		إذا (_led_control_sub.copy(&led_control)) {

			إذا ((_led_control_sub.get_وروستی_generation() != وروستی_generation + 1)
			    && (_led_control_sub.get_وروستی_generation() != وروستی_generation)) {
				perf_شمېر(led_control_sub_ورک_perf);
				PX4_DEBUG("led_control lost, generation %d -> %d", وروستی_generation, _led_control_sub.get_وروستی_generation());
			}

			// don't apply the new state just yet to avoid interrupting an ongoing blinking state
			لكل (صحيح i = 0; i < BOARD_MAX_LEDS; ++i) {
				إذا (led_control.led_ماسک & (1 << i)) {
					// if next state has already a higher priority state than
					// led_control, set lower prio state directly, so that this
					// information is not lost
					إذا (حالتونه[i].بل_state.is_valid() && led_control.لومړیتوب < حالتونه[i].بل_state.لومړیتوب) {
						حالتونه[i].سېټ(led_control);

					} وإلا {
						// if a lower prio event is already in next state and a
						// higher prio event is coming in
						إذا (حالتونه[i].بل_state.is_valid() && led_control.لومړیتوب > حالتونه[i].بل_state.لومړیتوب) {
							حالتونه[i].apply_بل_state();
						}

						حالتونه[i].بل_state.سېټ(led_control);
					}
				}
			}
		}

		_force_update = خطأ;
	}

	// handle state updates
	hrt_abstime now = hrt_absolute_وخت();

	إذا (وروستی_update_call == 0) {
		وروستی_update_call = now;
		أرجع 0;
	}

	uint16_t blink_delta_t = (uint16_t)((now - وروستی_update_call) / 100); // Note: this is in 0.1ms
	constexpr uint16_t breathe_duration = BREATHE_INTERVAL * BREATHE_ګامونه / 100;

	صحيح عدد_blinking_leds = 0;
	صحيح عدد_blinking_do_not_change_state = 0;
	صحيح current_priorities[BOARD_MAX_LEDS];

	لكل (صحيح i = 0; i < BOARD_MAX_LEDS; ++i) {
		صحيح لومړیتوب = led_control_s::MAX_لومړیتوب;

		لكل (; لومړیتوب >= 0; --لومړیتوب) {

			فی_لومړیتوب_معلومات &cur_معلومات = حالتونه[i].لومړیتوب[لومړیتوب];

			إذا (cur_معلومات.ډېرپېښ == led_control_s::ډېرپېښ_DISABLED) {
				استمر; // handle next priority
			}

			// handle state updates
			uint16_t current_blink_duration = 0;

			بدل (cur_معلومات.ډېرپېښ) {
			حالة led_control_s::ډېرپېښ_FLASH:
			حالة led_control_s::ډېرپېښ_BLINK_FAST:
				current_blink_duration = BLINK_FAST_DURATION / 100;
				اكسر;

			حالة led_control_s::ډېرپېښ_BLINK_نورمال:
				current_blink_duration = BLINK_نورمال_DURATION / 100;
				اكسر;

			حالة led_control_s::ډېرپېښ_BLINK_SLOW:
				current_blink_duration = BLINK_SLOW_DURATION / 100;
				اكسر;

			حالة led_control_s::ډېرپېښ_BREATHE:
				حالتونه[i].current_blinking_وخت += blink_delta_t;

				طالما (حالتونه[i].current_blinking_وخت > breathe_duration) {
					حالتونه[i].current_blinking_وخت -= breathe_duration;
				}

				had_changes = ریښتیا;
				اكسر;
			}

			إذا (current_blink_duration > 0) {
				++عدد_blinking_leds;

				حالتونه[i].current_blinking_وخت += blink_delta_t;

				إذا (حالتونه[i].current_blinking_وخت > current_blink_duration) {
					حالتونه[i].current_blinking_وخت -= current_blink_duration;

					إذا (cur_معلومات.blink_times_کیڼ == 246) {
						// handle toggling for infinite case: decrease between 255 and 246
						// In order to handle the flash mode infinite case it needs a
						// total of 10 steps.
						cur_معلومات.blink_times_کیڼ = 255;
						++عدد_blinking_do_not_change_state;

					} وإلا إذا (--cur_معلومات.blink_times_کیڼ == 0) {
						cur_معلومات.ډېرپېښ = led_control_s::ډېرپېښ_DISABLED;
						حالتونه[i].current_blinking_وخت = 0;

					} وإلا إذا (cur_معلومات.blink_times_کیڼ % 2 == 1) {
						++عدد_blinking_do_not_change_state;
					}

					had_changes = ریښتیا;

				} وإلا {
					++عدد_blinking_do_not_change_state;
				}
			}

			اكسر; // handle next led
		}

		current_priorities[i] = لومړیتوب;

	}

	// handle next state:
	// only allow a state change if no led blinks or at least one of the blinking leds signals that it's ok to switch.
	// This makes sure all leds are kept in sync, but does not allow interrupting at arbitrary points.
	إذا (عدد_blinking_leds == 0 || عدد_blinking_leds > عدد_blinking_do_not_change_state) {
		لكل (صحيح i = 0; i < BOARD_MAX_LEDS; ++i) {
			إذا (حالتونه[i].بل_state.is_valid()) {
				صحيح بل_لومړیتوب = (صحيح)حالتونه[i].بل_state.لومړیتوب;

				إذا (بل_لومړیتوب >= current_priorities[i]) {
					حالتونه[i].current_blinking_وخت = 0;
					had_changes = ریښتیا;
				}

				حالتونه[i].apply_بل_state();
				حالتونه[i].بل_state.reset();
			}

		}
	}

	وروستی_update_call = now;

	إذا (!had_changes) {
		أرجع 0;
	}

	// create output
	get_control_معلومات(control_معلومات);

	أرجع 1;
}

فراغ LedController::get_control_معلومات(Led_Control_معلومات &control_معلومات)
{
	_breathe_enabled = خطأ;

	لكل (صحيح i = 0; i < BOARD_MAX_LEDS; ++i) {
		control_معلومات.leds[i].color = led_control_s::COLOR_OFF; // set output to a defined state
		control_معلومات.leds[i].brightness = _max_brightness;

		لكل (صحيح لومړیتوب = led_control_s::MAX_لومړیتوب; لومړیتوب >= 0; --لومړیتوب) {
			منطقي flash_output_فعال = ریښتیا;
			ثابت فی_لومړیتوب_معلومات &cur_معلومات = حالتونه[i].لومړیتوب[لومړیتوب];

			إذا (cur_معلومات.ډېرپېښ == led_control_s::ډېرپېښ_DISABLED) {
				استمر; // handle next priority
			}

			بدل (cur_معلومات.ډېرپېښ) {
			حالة led_control_s::ډېرپېښ_ON:
				control_معلومات.leds[i].color = cur_معلومات.color;
				اكسر;

			حالة led_control_s::ډېرپېښ_BREATHE: {
					// fade on and off
					صحيح counter = حالتونه[i].current_blinking_وخت / (BREATHE_INTERVAL / 100);
					صحيح n = counter >= (BREATHE_ګامونه / 2) ? BREATHE_ګامونه - counter : counter;
					control_معلومات.leds[i].brightness = (n * n) * _max_brightness / (BREATHE_ګامونه * BREATHE_ګامونه / 4); // (n/(steps/2))^2
					control_معلومات.leds[i].color = cur_معلومات.color;
					_breathe_enabled = ریښتیا;
					اكسر;
				}

			حالة led_control_s::ډېرپېښ_FLASH:
				إذا (cur_معلومات.blink_times_کیڼ % 10 < 6) { // 2 blinks, then turn off for the rest of the cycle
					flash_output_فعال = خطأ;
				}

			/* FALLTHROUGH */
			حالة led_control_s::ډېرپېښ_BLINK_FAST:
			حالة led_control_s::ډېرپېښ_BLINK_نورمال:
			حالة led_control_s::ډېرپېښ_BLINK_SLOW:
				إذا (cur_معلومات.blink_times_کیڼ % 2 == 0 && flash_output_فعال) {
					control_معلومات.leds[i].color = cur_معلومات.color;
				}

				اكسر;
				// MODE_OFF does not need to be handled, it's already set above
			}

			اكسر; // handle next led
		}
	}
}
