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

صحيح LedController::update(Led_Control_انکڑے &control_انکڑے)
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
			control_انکڑے.leds[i].brightness = math::min(_max_brightness, control_انکڑے.leds[i].brightness);
		}

		إذا (_max_brightness != max_brightness_prev) {
			had_changes = سچ;
		}
	}

	طالما (_led_control_sub.updated() || _force_update) {
		ثابت بنانشانی آخری_generation = _led_control_sub.get_آخری_generation();

		// handle new state
		led_control_s led_control;

		إذا (_led_control_sub.copy(&led_control)) {

			إذا ((_led_control_sub.get_آخری_generation() != آخری_generation + 1)
			    && (_led_control_sub.get_آخری_generation() != آخری_generation)) {
				perf_گنتی(led_control_sub_گواچا_perf);
				PX4_DEBUG("led_control lost, generation %d -> %d", آخری_generation, _led_control_sub.get_آخری_generation());
			}

			// don't apply the new state just yet to avoid interrupting an ongoing blinking state
			لكل (صحيح i = 0; i < BOARD_MAX_LEDS; ++i) {
				إذا (led_control.led_نقاب & (1 << i)) {
					// if next state has already a higher priority state than
					// led_control, set lower prio state directly, so that this
					// information is not lost
					إذا (حالتاں[i].اگلا_state.is_valid() && led_control.ترجیح < حالتاں[i].اگلا_state.ترجیح) {
						حالتاں[i].سیٹ(led_control);

					} وإلا {
						// if a lower prio event is already in next state and a
						// higher prio event is coming in
						إذا (حالتاں[i].اگلا_state.is_valid() && led_control.ترجیح > حالتاں[i].اگلا_state.ترجیح) {
							حالتاں[i].apply_اگلا_state();
						}

						حالتاں[i].اگلا_state.سیٹ(led_control);
					}
				}
			}
		}

		_force_update = خطأ;
	}

	// handle state updates
	hrt_abstime now = hrt_absolute_ویلا();

	إذا (آخری_update_call == 0) {
		آخری_update_call = now;
		أرجع 0;
	}

	uint16_t blink_delta_t = (uint16_t)((now - آخری_update_call) / 100); // Note: this is in 0.1ms
	constexpr uint16_t breathe_duration = BREATHE_INTERVAL * BREATHE_قدماں / 100;

	صحيح عدد_blinking_leds = 0;
	صحيح عدد_blinking_do_not_change_state = 0;
	صحيح current_priorities[BOARD_MAX_LEDS];

	لكل (صحيح i = 0; i < BOARD_MAX_LEDS; ++i) {
		صحيح ترجیح = led_control_s::MAX_ترجیح;

		لكل (; ترجیح >= 0; --ترجیح) {

			فی_ترجیح_انکڑے &cur_انکڑے = حالتاں[i].ترجیح[ترجیح];

			إذا (cur_انکڑے.منوال == led_control_s::منوال_DISABLED) {
				استمر; // handle next priority
			}

			// handle state updates
			uint16_t current_blink_duration = 0;

			بدل (cur_انکڑے.منوال) {
			حالة led_control_s::منوال_FLASH:
			حالة led_control_s::منوال_BLINK_FAST:
				current_blink_duration = BLINK_FAST_DURATION / 100;
				اكسر;

			حالة led_control_s::منوال_BLINK_عام:
				current_blink_duration = BLINK_عام_DURATION / 100;
				اكسر;

			حالة led_control_s::منوال_BLINK_SLOW:
				current_blink_duration = BLINK_SLOW_DURATION / 100;
				اكسر;

			حالة led_control_s::منوال_BREATHE:
				حالتاں[i].current_blinking_ویلا += blink_delta_t;

				طالما (حالتاں[i].current_blinking_ویلا > breathe_duration) {
					حالتاں[i].current_blinking_ویلا -= breathe_duration;
				}

				had_changes = سچ;
				اكسر;
			}

			إذا (current_blink_duration > 0) {
				++عدد_blinking_leds;

				حالتاں[i].current_blinking_ویلا += blink_delta_t;

				إذا (حالتاں[i].current_blinking_ویلا > current_blink_duration) {
					حالتاں[i].current_blinking_ویلا -= current_blink_duration;

					إذا (cur_انکڑے.blink_times_بایاں == 246) {
						// handle toggling for infinite case: decrease between 255 and 246
						// In order to handle the flash mode infinite case it needs a
						// total of 10 steps.
						cur_انکڑے.blink_times_بایاں = 255;
						++عدد_blinking_do_not_change_state;

					} وإلا إذا (--cur_انکڑے.blink_times_بایاں == 0) {
						cur_انکڑے.منوال = led_control_s::منوال_DISABLED;
						حالتاں[i].current_blinking_ویلا = 0;

					} وإلا إذا (cur_انکڑے.blink_times_بایاں % 2 == 1) {
						++عدد_blinking_do_not_change_state;
					}

					had_changes = سچ;

				} وإلا {
					++عدد_blinking_do_not_change_state;
				}
			}

			اكسر; // handle next led
		}

		current_priorities[i] = ترجیح;

	}

	// handle next state:
	// only allow a state change if no led blinks or at least one of the blinking leds signals that it's ok to switch.
	// This makes sure all leds are kept in sync, but does not allow interrupting at arbitrary points.
	إذا (عدد_blinking_leds == 0 || عدد_blinking_leds > عدد_blinking_do_not_change_state) {
		لكل (صحيح i = 0; i < BOARD_MAX_LEDS; ++i) {
			إذا (حالتاں[i].اگلا_state.is_valid()) {
				صحيح اگلا_ترجیح = (صحيح)حالتاں[i].اگلا_state.ترجیح;

				إذا (اگلا_ترجیح >= current_priorities[i]) {
					حالتاں[i].current_blinking_ویلا = 0;
					had_changes = سچ;
				}

				حالتاں[i].apply_اگلا_state();
				حالتاں[i].اگلا_state.reset();
			}

		}
	}

	آخری_update_call = now;

	إذا (!had_changes) {
		أرجع 0;
	}

	// create output
	get_control_انکڑے(control_انکڑے);

	أرجع 1;
}

فراغ LedController::get_control_انکڑے(Led_Control_انکڑے &control_انکڑے)
{
	_breathe_enabled = خطأ;

	لكل (صحيح i = 0; i < BOARD_MAX_LEDS; ++i) {
		control_انکڑے.leds[i].color = led_control_s::COLOR_OFF; // set output to a defined state
		control_انکڑے.leds[i].brightness = _max_brightness;

		لكل (صحيح ترجیح = led_control_s::MAX_ترجیح; ترجیح >= 0; --ترجیح) {
			منطقي flash_output_سرگرم = سچ;
			ثابت فی_ترجیح_انکڑے &cur_انکڑے = حالتاں[i].ترجیح[ترجیح];

			إذا (cur_انکڑے.منوال == led_control_s::منوال_DISABLED) {
				استمر; // handle next priority
			}

			بدل (cur_انکڑے.منوال) {
			حالة led_control_s::منوال_ON:
				control_انکڑے.leds[i].color = cur_انکڑے.color;
				اكسر;

			حالة led_control_s::منوال_BREATHE: {
					// fade on and off
					صحيح counter = حالتاں[i].current_blinking_ویلا / (BREATHE_INTERVAL / 100);
					صحيح n = counter >= (BREATHE_قدماں / 2) ? BREATHE_قدماں - counter : counter;
					control_انکڑے.leds[i].brightness = (n * n) * _max_brightness / (BREATHE_قدماں * BREATHE_قدماں / 4); // (n/(steps/2))^2
					control_انکڑے.leds[i].color = cur_انکڑے.color;
					_breathe_enabled = سچ;
					اكسر;
				}

			حالة led_control_s::منوال_FLASH:
				إذا (cur_انکڑے.blink_times_بایاں % 10 < 6) { // 2 blinks, then turn off for the rest of the cycle
					flash_output_سرگرم = خطأ;
				}

			/* FALLTHROUGH */
			حالة led_control_s::منوال_BLINK_FAST:
			حالة led_control_s::منوال_BLINK_عام:
			حالة led_control_s::منوال_BLINK_SLOW:
				إذا (cur_انکڑے.blink_times_بایاں % 2 == 0 && flash_output_سرگرم) {
					control_انکڑے.leds[i].color = cur_انکڑے.color;
				}

				اكسر;
				// MODE_OFF does not need to be handled, it's already set above
			}

			اكسر; // handle next led
		}
	}
}
