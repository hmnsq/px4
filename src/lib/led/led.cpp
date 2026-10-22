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


#ਅਨ੍ਤਰ੍ਭਾਵ "led.h"

ਪੂਰਨ LedController::update(Led_Control_ਅੰਕੜੇ &control_ਅੰਕੜੇ)
{
	ਤਾਰਕਿਕ had_changes = ਝੂਠ; // did one of the outputs change?

	// check for parameter updates
	ਜੇ (_parameter_update_sub.updated()) {
		// clear update
		parameter_update_s pupdate;
		_parameter_update_sub.copy(&pupdate);

		updateParams();

		ਅਚਲ uint8_t max_brightness_prev = _max_brightness;

		// set maximum brightness (0-255) from percentage
		_max_brightness = roundf(math::constrain(_param_sys_rgb_maxbrt.get(), 0.f, 1.f) * UINT8_MAX);

		// update existing
		ਲਈ (ਪੂਰਨ i = 0; i < BOARD_MAX_LEDS; ++i) {
			control_ਅੰਕੜੇ.leds[i].brightness = math::min(_max_brightness, control_ਅੰਕੜੇ.leds[i].brightness);
		}

		ਜੇ (_max_brightness != max_brightness_prev) {
			had_changes = ਸੱਚ;
		}
	}

	ਜਦੋਂ (_led_control_sub.updated() || _force_update) {
		ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਆਖਰੀ_generation = _led_control_sub.get_ਆਖਰੀ_generation();

		// handle new state
		led_control_s led_control;

		ਜੇ (_led_control_sub.copy(&led_control)) {

			ਜੇ ((_led_control_sub.get_ਆਖਰੀ_generation() != ਆਖਰੀ_generation + 1)
			    && (_led_control_sub.get_ਆਖਰੀ_generation() != ਆਖਰੀ_generation)) {
				perf_ਗਿਣਤੀ(led_control_sub_ਗੁਆਚਾ_perf);
				PX4_DEBUG("led_control lost, generation %d -> %d", ਆਖਰੀ_generation, _led_control_sub.get_ਆਖਰੀ_generation());
			}

			// don't apply the new state just yet to avoid interrupting an ongoing blinking state
			ਲਈ (ਪੂਰਨ i = 0; i < BOARD_MAX_LEDS; ++i) {
				ਜੇ (led_control.led_ਨਕਾਬ & (1 << i)) {
					// if next state has already a higher priority state than
					// led_control, set lower prio state directly, so that this
					// information is not lost
					ਜੇ (ਹਾਲਤਾਂ[i].ਅਗਲਾ_state.is_valid() && led_control.ਤਰਜੀਹ < ਹਾਲਤਾਂ[i].ਅਗਲਾ_state.ਤਰਜੀਹ) {
						ਹਾਲਤਾਂ[i].ਸੈੱਟ(led_control);

					} ਨਹੀਂਤਾਂ {
						// if a lower prio event is already in next state and a
						// higher prio event is coming in
						ਜੇ (ਹਾਲਤਾਂ[i].ਅਗਲਾ_state.is_valid() && led_control.ਤਰਜੀਹ > ਹਾਲਤਾਂ[i].ਅਗਲਾ_state.ਤਰਜੀਹ) {
							ਹਾਲਤਾਂ[i].apply_ਅਗਲਾ_state();
						}

						ਹਾਲਤਾਂ[i].ਅਗਲਾ_state.ਸੈੱਟ(led_control);
					}
				}
			}
		}

		_force_update = ਝੂਠ;
	}

	// handle state updates
	hrt_abstime now = hrt_absolute_ਸਮਾਂ();

	ਜੇ (ਆਖਰੀ_update_call == 0) {
		ਆਖਰੀ_update_call = now;
		ਵਾਪਸ 0;
	}

	uint16_t blink_delta_t = (uint16_t)((now - ਆਖਰੀ_update_call) / 100); // Note: this is in 0.1ms
	constexpr uint16_t breathe_duration = BREATHE_INTERVAL * BREATHE_ਕਦਮਾਂ / 100;

	ਪੂਰਨ ਅੰਕ_blinking_leds = 0;
	ਪੂਰਨ ਅੰਕ_blinking_do_not_change_state = 0;
	ਪੂਰਨ current_priorities[BOARD_MAX_LEDS];

	ਲਈ (ਪੂਰਨ i = 0; i < BOARD_MAX_LEDS; ++i) {
		ਪੂਰਨ ਤਰਜੀਹ = led_control_s::MAX_ਤਰਜੀਹ;

		ਲਈ (; ਤਰਜੀਹ >= 0; --ਤਰਜੀਹ) {

			ਪ੍ਰਤੀ_ਤਰਜੀਹ_ਅੰਕੜੇ &cur_ਅੰਕੜੇ = ਹਾਲਤਾਂ[i].ਤਰਜੀਹ[ਤਰਜੀਹ];

			ਜੇ (cur_ਅੰਕੜੇ.ਬਹੁਲਕ == led_control_s::ਬਹੁਲਕ_DISABLED) {
				ਜਾਰੀ; // handle next priority
			}

			// handle state updates
			uint16_t current_blink_duration = 0;

			ਚੋਣ (cur_ਅੰਕੜੇ.ਬਹੁਲਕ) {
			ਹਾਲਤ led_control_s::ਬਹੁਲਕ_FLASH:
			ਹਾਲਤ led_control_s::ਬਹੁਲਕ_BLINK_FAST:
				current_blink_duration = BLINK_FAST_DURATION / 100;
				ਰੋਕੋ;

			ਹਾਲਤ led_control_s::ਬਹੁਲਕ_BLINK_ਸਾਧਾਰਨ:
				current_blink_duration = BLINK_ਸਾਧਾਰਨ_DURATION / 100;
				ਰੋਕੋ;

			ਹਾਲਤ led_control_s::ਬਹੁਲਕ_BLINK_SLOW:
				current_blink_duration = BLINK_SLOW_DURATION / 100;
				ਰੋਕੋ;

			ਹਾਲਤ led_control_s::ਬਹੁਲਕ_BREATHE:
				ਹਾਲਤਾਂ[i].current_blinking_ਸਮਾਂ += blink_delta_t;

				ਜਦੋਂ (ਹਾਲਤਾਂ[i].current_blinking_ਸਮਾਂ > breathe_duration) {
					ਹਾਲਤਾਂ[i].current_blinking_ਸਮਾਂ -= breathe_duration;
				}

				had_changes = ਸੱਚ;
				ਰੋਕੋ;
			}

			ਜੇ (current_blink_duration > 0) {
				++ਅੰਕ_blinking_leds;

				ਹਾਲਤਾਂ[i].current_blinking_ਸਮਾਂ += blink_delta_t;

				ਜੇ (ਹਾਲਤਾਂ[i].current_blinking_ਸਮਾਂ > current_blink_duration) {
					ਹਾਲਤਾਂ[i].current_blinking_ਸਮਾਂ -= current_blink_duration;

					ਜੇ (cur_ਅੰਕੜੇ.blink_times_ਖੱਬਾ == 246) {
						// handle toggling for infinite case: decrease between 255 and 246
						// In order to handle the flash mode infinite case it needs a
						// total of 10 steps.
						cur_ਅੰਕੜੇ.blink_times_ਖੱਬਾ = 255;
						++ਅੰਕ_blinking_do_not_change_state;

					} ਨਹੀਂਤਾਂ ਜੇ (--cur_ਅੰਕੜੇ.blink_times_ਖੱਬਾ == 0) {
						cur_ਅੰਕੜੇ.ਬਹੁਲਕ = led_control_s::ਬਹੁਲਕ_DISABLED;
						ਹਾਲਤਾਂ[i].current_blinking_ਸਮਾਂ = 0;

					} ਨਹੀਂਤਾਂ ਜੇ (cur_ਅੰਕੜੇ.blink_times_ਖੱਬਾ % 2 == 1) {
						++ਅੰਕ_blinking_do_not_change_state;
					}

					had_changes = ਸੱਚ;

				} ਨਹੀਂਤਾਂ {
					++ਅੰਕ_blinking_do_not_change_state;
				}
			}

			ਰੋਕੋ; // handle next led
		}

		current_priorities[i] = ਤਰਜੀਹ;

	}

	// handle next state:
	// only allow a state change if no led blinks or at least one of the blinking leds signals that it's ok to switch.
	// This makes sure all leds are kept in sync, but does not allow interrupting at arbitrary points.
	ਜੇ (ਅੰਕ_blinking_leds == 0 || ਅੰਕ_blinking_leds > ਅੰਕ_blinking_do_not_change_state) {
		ਲਈ (ਪੂਰਨ i = 0; i < BOARD_MAX_LEDS; ++i) {
			ਜੇ (ਹਾਲਤਾਂ[i].ਅਗਲਾ_state.is_valid()) {
				ਪੂਰਨ ਅਗਲਾ_ਤਰਜੀਹ = (ਪੂਰਨ)ਹਾਲਤਾਂ[i].ਅਗਲਾ_state.ਤਰਜੀਹ;

				ਜੇ (ਅਗਲਾ_ਤਰਜੀਹ >= current_priorities[i]) {
					ਹਾਲਤਾਂ[i].current_blinking_ਸਮਾਂ = 0;
					had_changes = ਸੱਚ;
				}

				ਹਾਲਤਾਂ[i].apply_ਅਗਲਾ_state();
				ਹਾਲਤਾਂ[i].ਅਗਲਾ_state.reset();
			}

		}
	}

	ਆਖਰੀ_update_call = now;

	ਜੇ (!had_changes) {
		ਵਾਪਸ 0;
	}

	// create output
	get_control_ਅੰਕੜੇ(control_ਅੰਕੜੇ);

	ਵਾਪਸ 1;
}

ਖਾਲੀ LedController::get_control_ਅੰਕੜੇ(Led_Control_ਅੰਕੜੇ &control_ਅੰਕੜੇ)
{
	_breathe_enabled = ਝੂਠ;

	ਲਈ (ਪੂਰਨ i = 0; i < BOARD_MAX_LEDS; ++i) {
		control_ਅੰਕੜੇ.leds[i].color = led_control_s::COLOR_OFF; // set output to a defined state
		control_ਅੰਕੜੇ.leds[i].brightness = _max_brightness;

		ਲਈ (ਪੂਰਨ ਤਰਜੀਹ = led_control_s::MAX_ਤਰਜੀਹ; ਤਰਜੀਹ >= 0; --ਤਰਜੀਹ) {
			ਤਾਰਕਿਕ flash_output_ਸਰਗਰਮ = ਸੱਚ;
			ਅਚਲ ਪ੍ਰਤੀ_ਤਰਜੀਹ_ਅੰਕੜੇ &cur_ਅੰਕੜੇ = ਹਾਲਤਾਂ[i].ਤਰਜੀਹ[ਤਰਜੀਹ];

			ਜੇ (cur_ਅੰਕੜੇ.ਬਹੁਲਕ == led_control_s::ਬਹੁਲਕ_DISABLED) {
				ਜਾਰੀ; // handle next priority
			}

			ਚੋਣ (cur_ਅੰਕੜੇ.ਬਹੁਲਕ) {
			ਹਾਲਤ led_control_s::ਬਹੁਲਕ_ON:
				control_ਅੰਕੜੇ.leds[i].color = cur_ਅੰਕੜੇ.color;
				ਰੋਕੋ;

			ਹਾਲਤ led_control_s::ਬਹੁਲਕ_BREATHE: {
					// fade on and off
					ਪੂਰਨ counter = ਹਾਲਤਾਂ[i].current_blinking_ਸਮਾਂ / (BREATHE_INTERVAL / 100);
					ਪੂਰਨ n = counter >= (BREATHE_ਕਦਮਾਂ / 2) ? BREATHE_ਕਦਮਾਂ - counter : counter;
					control_ਅੰਕੜੇ.leds[i].brightness = (n * n) * _max_brightness / (BREATHE_ਕਦਮਾਂ * BREATHE_ਕਦਮਾਂ / 4); // (n/(steps/2))^2
					control_ਅੰਕੜੇ.leds[i].color = cur_ਅੰਕੜੇ.color;
					_breathe_enabled = ਸੱਚ;
					ਰੋਕੋ;
				}

			ਹਾਲਤ led_control_s::ਬਹੁਲਕ_FLASH:
				ਜੇ (cur_ਅੰਕੜੇ.blink_times_ਖੱਬਾ % 10 < 6) { // 2 blinks, then turn off for the rest of the cycle
					flash_output_ਸਰਗਰਮ = ਝੂਠ;
				}

			/* FALLTHROUGH */
			ਹਾਲਤ led_control_s::ਬਹੁਲਕ_BLINK_FAST:
			ਹਾਲਤ led_control_s::ਬਹੁਲਕ_BLINK_ਸਾਧਾਰਨ:
			ਹਾਲਤ led_control_s::ਬਹੁਲਕ_BLINK_SLOW:
				ਜੇ (cur_ਅੰਕੜੇ.blink_times_ਖੱਬਾ % 2 == 0 && flash_output_ਸਰਗਰਮ) {
					control_ਅੰਕੜੇ.leds[i].color = cur_ਅੰਕੜੇ.color;
				}

				ਰੋਕੋ;
				// MODE_OFF does not need to be handled, it's already set above
			}

			ਰੋਕੋ; // handle next led
		}
	}
}
