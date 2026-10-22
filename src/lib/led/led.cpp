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


#شامل "led.h"

صحیح LedController::update(Led_Control_ڈیٹا &control_ڈیٹا)
{
	منطقی had_changes = جھوٹ; // did one of the outputs change?

	// check for parameter updates
	اگر (_parameter_update_sub.updated()) {
		// clear update
		parameter_update_s pupdate;
		_parameter_update_sub.copy(&pupdate);

		updateParams();

		ثابت uint8_t max_brightness_پچھلا = _max_brightness;

		// set maximum brightness (0-255) from percentage
		_max_brightness = roundf(math::constrain(_param_sys_rgb_maxbrt.get(), 0.f, 1.f) * UINT8_MAX);

		// update existing
		برائے (صحیح i = 0; i < BOARD_MAX_LEDS; ++i) {
			control_ڈیٹا.leds[i].brightness = math::min(_max_brightness, control_ڈیٹا.leds[i].brightness);
		}

		اگر (_max_brightness != max_brightness_پچھلا) {
			had_changes = سچ;
		}
	}

	جب (_led_control_sub.updated() || _force_update) {
		ثابت غیرعلامتی آخری_generation = _led_control_sub.get_آخری_generation();

		// handle new state
		led_control_s led_control;

		اگر (_led_control_sub.copy(&led_control)) {

			اگر ((_led_control_sub.get_آخری_generation() != آخری_generation + 1)
			    && (_led_control_sub.get_آخری_generation() != آخری_generation)) {
				perf_گنتی(led_control_sub_گمشدہ_perf);
				PX4_DEBUG("led_control lost, generation %d -> %d", آخری_generation, _led_control_sub.get_آخری_generation());
			}

			// don't apply the new state just yet to avoid interrupting an ongoing blinking state
			برائے (صحیح i = 0; i < BOARD_MAX_LEDS; ++i) {
				اگر (led_control.led_نقاب & (1 << i)) {
					// if next state has already a higher priority state than
					// led_control, set lower prio state directly, so that this
					// information is not lost
					اگر (حالتیں[i].اگلا_state.is_valid() && led_control.ترجیح < حالتیں[i].اگلا_state.ترجیح) {
						حالتیں[i].سیٹ(led_control);

					} ورنہ {
						// if a lower prio event is already in next state and a
						// higher prio event is coming in
						اگر (حالتیں[i].اگلا_state.is_valid() && led_control.ترجیح > حالتیں[i].اگلا_state.ترجیح) {
							حالتیں[i].apply_اگلا_state();
						}

						حالتیں[i].اگلا_state.سیٹ(led_control);
					}
				}
			}
		}

		_force_update = جھوٹ;
	}

	// handle state updates
	hrt_abstime now = hrt_absolute_وقت();

	اگر (آخری_update_call == 0) {
		آخری_update_call = now;
		واپس 0;
	}

	uint16_t blink_delta_t = (uint16_t)((now - آخری_update_call) / 100); // Note: this is in 0.1ms
	constexpr uint16_t breathe_duration = BREATHE_INTERVAL * BREATHE_اقدام / 100;

	صحیح عدد_blinking_leds = 0;
	صحیح عدد_blinking_do_not_change_state = 0;
	صحیح current_priorities[BOARD_MAX_LEDS];

	برائے (صحیح i = 0; i < BOARD_MAX_LEDS; ++i) {
		صحیح ترجیح = led_control_s::MAX_ترجیح;

		برائے (; ترجیح >= 0; --ترجیح) {

			فی_ترجیح_ڈیٹا &cur_ڈیٹا = حالتیں[i].ترجیح[ترجیح];

			اگر (cur_ڈیٹا.منوال == led_control_s::منوال_DISABLED) {
				جاری; // handle next priority
			}

			// handle state updates
			uint16_t current_blink_duration = 0;

			انتخاب (cur_ڈیٹا.منوال) {
			حالت led_control_s::منوال_FLASH:
			حالت led_control_s::منوال_BLINK_FAST:
				current_blink_duration = BLINK_FAST_DURATION / 100;
				توڑو;

			حالت led_control_s::منوال_BLINK_معمول:
				current_blink_duration = BLINK_معمول_DURATION / 100;
				توڑو;

			حالت led_control_s::منوال_BLINK_SLOW:
				current_blink_duration = BLINK_SLOW_DURATION / 100;
				توڑو;

			حالت led_control_s::منوال_BREATHE:
				حالتیں[i].current_blinking_وقت += blink_delta_t;

				جب (حالتیں[i].current_blinking_وقت > breathe_duration) {
					حالتیں[i].current_blinking_وقت -= breathe_duration;
				}

				had_changes = سچ;
				توڑو;
			}

			اگر (current_blink_duration > 0) {
				++عدد_blinking_leds;

				حالتیں[i].current_blinking_وقت += blink_delta_t;

				اگر (حالتیں[i].current_blinking_وقت > current_blink_duration) {
					حالتیں[i].current_blinking_وقت -= current_blink_duration;

					اگر (cur_ڈیٹا.blink_times_بایاں == 246) {
						// handle toggling for infinite case: decrease between 255 and 246
						// In order to handle the flash mode infinite case it needs a
						// total of 10 steps.
						cur_ڈیٹا.blink_times_بایاں = 255;
						++عدد_blinking_do_not_change_state;

					} ورنہ اگر (--cur_ڈیٹا.blink_times_بایاں == 0) {
						cur_ڈیٹا.منوال = led_control_s::منوال_DISABLED;
						حالتیں[i].current_blinking_وقت = 0;

					} ورنہ اگر (cur_ڈیٹا.blink_times_بایاں % 2 == 1) {
						++عدد_blinking_do_not_change_state;
					}

					had_changes = سچ;

				} ورنہ {
					++عدد_blinking_do_not_change_state;
				}
			}

			توڑو; // handle next led
		}

		current_priorities[i] = ترجیح;

	}

	// handle next state:
	// only allow a state change if no led blinks or at least one of the blinking leds signals that it's ok to switch.
	// This makes sure all leds are kept in sync, but does not allow interrupting at arbitrary points.
	اگر (عدد_blinking_leds == 0 || عدد_blinking_leds > عدد_blinking_do_not_change_state) {
		برائے (صحیح i = 0; i < BOARD_MAX_LEDS; ++i) {
			اگر (حالتیں[i].اگلا_state.is_valid()) {
				صحیح اگلا_ترجیح = (صحیح)حالتیں[i].اگلا_state.ترجیح;

				اگر (اگلا_ترجیح >= current_priorities[i]) {
					حالتیں[i].current_blinking_وقت = 0;
					had_changes = سچ;
				}

				حالتیں[i].apply_اگلا_state();
				حالتیں[i].اگلا_state.reset();
			}

		}
	}

	آخری_update_call = now;

	اگر (!had_changes) {
		واپس 0;
	}

	// create output
	get_control_ڈیٹا(control_ڈیٹا);

	واپس 1;
}

خالی LedController::get_control_ڈیٹا(Led_Control_ڈیٹا &control_ڈیٹا)
{
	_breathe_enabled = جھوٹ;

	برائے (صحیح i = 0; i < BOARD_MAX_LEDS; ++i) {
		control_ڈیٹا.leds[i].color = led_control_s::COLOR_OFF; // set output to a defined state
		control_ڈیٹا.leds[i].brightness = _max_brightness;

		برائے (صحیح ترجیح = led_control_s::MAX_ترجیح; ترجیح >= 0; --ترجیح) {
			منطقی flash_output_فعال = سچ;
			ثابت فی_ترجیح_ڈیٹا &cur_ڈیٹا = حالتیں[i].ترجیح[ترجیح];

			اگر (cur_ڈیٹا.منوال == led_control_s::منوال_DISABLED) {
				جاری; // handle next priority
			}

			انتخاب (cur_ڈیٹا.منوال) {
			حالت led_control_s::منوال_ON:
				control_ڈیٹا.leds[i].color = cur_ڈیٹا.color;
				توڑو;

			حالت led_control_s::منوال_BREATHE: {
					// fade on and off
					صحیح شمار_2 = حالتیں[i].current_blinking_وقت / (BREATHE_INTERVAL / 100);
					صحیح n = شمار_2 >= (BREATHE_اقدام / 2) ? BREATHE_اقدام - شمار_2 : شمار_2;
					control_ڈیٹا.leds[i].brightness = (n * n) * _max_brightness / (BREATHE_اقدام * BREATHE_اقدام / 4); // (n/(steps/2))^2
					control_ڈیٹا.leds[i].color = cur_ڈیٹا.color;
					_breathe_enabled = سچ;
					توڑو;
				}

			حالت led_control_s::منوال_FLASH:
				اگر (cur_ڈیٹا.blink_times_بایاں % 10 < 6) { // 2 blinks, then turn off for the rest of the cycle
					flash_output_فعال = جھوٹ;
				}

			/* FALLTHROUGH */
			حالت led_control_s::منوال_BLINK_FAST:
			حالت led_control_s::منوال_BLINK_معمول:
			حالت led_control_s::منوال_BLINK_SLOW:
				اگر (cur_ڈیٹا.blink_times_بایاں % 2 == 0 && flash_output_فعال) {
					control_ڈیٹا.leds[i].color = cur_ڈیٹا.color;
				}

				توڑو;
				// MODE_OFF does not need to be handled, it's already set above
			}

			توڑو; // handle next led
		}
	}
}
