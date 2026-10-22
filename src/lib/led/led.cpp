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


#ಅನ್ತರ್ಭಾವ "led.h"

ಪೂರ್ಣಾಙ್ಕ LedController::update(LedControlData &control_data)
{
	ತಾರ್ಕಿಕ had_changes = ಅಸತ್ಯ; // did one of the outputs change?

	// check for parameter updates
	ಯದಿ (_parameter_update_sub.updated()) {
		// clear update
		parameter_update_s pupdate;
		_parameter_update_sub.copy(&pupdate);

		updateParams();

		ಧ್ರುವ uint8_t max_brightness_prev = _max_brightness;

		// set maximum brightness (0-255) from percentage
		_max_brightness = roundf(math::constrain(_param_sys_rgb_maxbrt.get(), 0.f, 1.f) * UINT8_MAX);

		// update existing
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < BOARD_MAX_LEDS; ++i) {
			control_data.leds[i].brightness = math::min(_max_brightness, control_data.leds[i].brightness);
		}

		ಯದಿ (_max_brightness != max_brightness_prev) {
			had_changes = ಸತ್ಯ;
		}
	}

	ಯಾವತ್ (_led_control_sub.updated() || _force_update) {
		ಧ್ರುವ unsigned last_generation = _led_control_sub.get_last_generation();

		// handle new state
		led_control_s led_control;

		ಯದಿ (_led_control_sub.copy(&led_control)) {

			ಯದಿ ((_led_control_sub.get_last_generation() != last_generation + 1)
			    && (_led_control_sub.get_last_generation() != last_generation)) {
				perf_ಎಣಿಕೆ(_led_control_sub_lost_perf);
				PX4_DEBUG("led_control lost, generation %d -> %d", last_generation, _led_control_sub.get_last_generation());
			}

			// don't apply the new state just yet to avoid interrupting an ongoing blinking state
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < BOARD_MAX_LEDS; ++i) {
				ಯದಿ (led_control.led_mask & (1 << i)) {
					// if next state has already a higher priority state than
					// led_control, set lower prio state directly, so that this
					// information is not lost
					ಯದಿ (_states[i].next_state.is_valid() && led_control.priority < _states[i].next_state.priority) {
						_states[i].set(led_control);

					} ಅನ್ಯಥಾ {
						// if a lower prio event is already in next state and a
						// higher prio event is coming in
						ಯದಿ (_states[i].next_state.is_valid() && led_control.priority > _states[i].next_state.priority) {
							_states[i].apply_next_state();
						}

						_states[i].next_state.set(led_control);
					}
				}
			}
		}

		_force_update = ಅಸತ್ಯ;
	}

	// handle state updates
	hrt_abstime now = hrt_absolute_time();

	ಯದಿ (_last_update_call == 0) {
		_last_update_call = now;
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	uint16_t blink_delta_t = (uint16_t)((now - _last_update_call) / 100); // Note: this is in 0.1ms
	constexpr uint16_t breathe_duration = BREATHE_INTERVAL * BREATHE_STEPS / 100;

	ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_blinking_leds = 0;
	ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_blinking_do_not_change_state = 0;
	ಪೂರ್ಣಾಙ್ಕ current_priorities[BOARD_MAX_LEDS];

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < BOARD_MAX_LEDS; ++i) {
		ಪೂರ್ಣಾಙ್ಕ priority = led_control_s::MAX_PRIORITY;

		ಪ್ರತಿ (; priority >= 0; --priority) {

			PerPriorityData &cur_data = _states[i].priority[priority];

			ಯದಿ (cur_data.mode == led_control_s::MODE_DISABLED) {
				ಅನುವರ್ತ; // handle next priority
			}

			// handle state updates
			uint16_t current_blink_duration = 0;

			ವಿಕಲ್ಪ (cur_data.mode) {
			ಪಕ್ಷ led_control_s::MODE_FLASH:
			ಪಕ್ಷ led_control_s::MODE_BLINK_FAST:
				current_blink_duration = BLINK_FAST_DURATION / 100;
				ಭಙ್ಗ;

			ಪಕ್ಷ led_control_s::MODE_BLINK_NORMAL:
				current_blink_duration = BLINK_NORMAL_DURATION / 100;
				ಭಙ್ಗ;

			ಪಕ್ಷ led_control_s::MODE_BLINK_SLOW:
				current_blink_duration = BLINK_SLOW_DURATION / 100;
				ಭಙ್ಗ;

			ಪಕ್ಷ led_control_s::MODE_BREATHE:
				_states[i].current_blinking_time += blink_delta_t;

				ಯಾವತ್ (_states[i].current_blinking_time > breathe_duration) {
					_states[i].current_blinking_time -= breathe_duration;
				}

				had_changes = ಸತ್ಯ;
				ಭಙ್ಗ;
			}

			ಯದಿ (current_blink_duration > 0) {
				++ಅಂಕೆ_blinking_leds;

				_states[i].current_blinking_time += blink_delta_t;

				ಯದಿ (_states[i].current_blinking_time > current_blink_duration) {
					_states[i].current_blinking_time -= current_blink_duration;

					ಯದಿ (cur_data.blink_times_ಎಡ == 246) {
						// handle toggling for infinite case: decrease between 255 and 246
						// In order to handle the flash mode infinite case it needs a
						// total of 10 steps.
						cur_data.blink_times_ಎಡ = 255;
						++ಅಂಕೆ_blinking_do_not_change_state;

					} ಅನ್ಯಥಾ ಯದಿ (--cur_data.blink_times_ಎಡ == 0) {
						cur_data.mode = led_control_s::MODE_DISABLED;
						_states[i].current_blinking_time = 0;

					} ಅನ್ಯಥಾ ಯದಿ (cur_data.blink_times_ಎಡ % 2 == 1) {
						++ಅಂಕೆ_blinking_do_not_change_state;
					}

					had_changes = ಸತ್ಯ;

				} ಅನ್ಯಥಾ {
					++ಅಂಕೆ_blinking_do_not_change_state;
				}
			}

			ಭಙ್ಗ; // handle next led
		}

		current_priorities[i] = priority;

	}

	// handle next state:
	// only allow a state change if no led blinks or at least one of the blinking leds signals that it's ok to switch.
	// This makes sure all leds are kept in sync, but does not allow interrupting at arbitrary points.
	ಯದಿ (ಅಂಕೆ_blinking_leds == 0 || ಅಂಕೆ_blinking_leds > ಅಂಕೆ_blinking_do_not_change_state) {
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < BOARD_MAX_LEDS; ++i) {
			ಯದಿ (_states[i].next_state.is_valid()) {
				ಪೂರ್ಣಾಙ್ಕ next_priority = (ಪೂರ್ಣಾಙ್ಕ)_states[i].next_state.priority;

				ಯದಿ (next_priority >= current_priorities[i]) {
					_states[i].current_blinking_time = 0;
					had_changes = ಸತ್ಯ;
				}

				_states[i].apply_next_state();
				_states[i].next_state.reset();
			}

		}
	}

	_last_update_call = now;

	ಯದಿ (!had_changes) {
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	// create output
	get_control_data(control_data);

	ಪ್ರತ್ಯಾಗಮ 1;
}

ಶೂನ್ಯ LedController::get_control_data(LedControlData &control_data)
{
	_breathe_enabled = ಅಸತ್ಯ;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < BOARD_MAX_LEDS; ++i) {
		control_data.leds[i].color = led_control_s::COLOR_OFF; // set output to a defined state
		control_data.leds[i].brightness = _max_brightness;

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ priority = led_control_s::MAX_PRIORITY; priority >= 0; --priority) {
			ತಾರ್ಕಿಕ flash_output_active = ಸತ್ಯ;
			ಧ್ರುವ PerPriorityData &cur_data = _states[i].priority[priority];

			ಯದಿ (cur_data.mode == led_control_s::MODE_DISABLED) {
				ಅನುವರ್ತ; // handle next priority
			}

			ವಿಕಲ್ಪ (cur_data.mode) {
			ಪಕ್ಷ led_control_s::MODE_ON:
				control_data.leds[i].color = cur_data.color;
				ಭಙ್ಗ;

			ಪಕ್ಷ led_control_s::MODE_BREATHE: {
					// fade on and off
					ಪೂರ್ಣಾಙ್ಕ counter = _states[i].current_blinking_time / (BREATHE_INTERVAL / 100);
					ಪೂರ್ಣಾಙ್ಕ n = counter >= (BREATHE_STEPS / 2) ? BREATHE_STEPS - counter : counter;
					control_data.leds[i].brightness = (n * n) * _max_brightness / (BREATHE_STEPS * BREATHE_STEPS / 4); // (n/(steps/2))^2
					control_data.leds[i].color = cur_data.color;
					_breathe_enabled = ಸತ್ಯ;
					ಭಙ್ಗ;
				}

			ಪಕ್ಷ led_control_s::MODE_FLASH:
				ಯದಿ (cur_data.blink_times_ಎಡ % 10 < 6) { // 2 blinks, then turn off for the rest of the cycle
					flash_output_active = ಅಸತ್ಯ;
				}

			/* FALLTHROUGH */
			ಪಕ್ಷ led_control_s::MODE_BLINK_FAST:
			ಪಕ್ಷ led_control_s::MODE_BLINK_NORMAL:
			ಪಕ್ಷ led_control_s::MODE_BLINK_SLOW:
				ಯದಿ (cur_data.blink_times_ಎಡ % 2 == 0 && flash_output_active) {
					control_data.leds[i].color = cur_data.color;
				}

				ಭಙ್ಗ;
				// MODE_OFF does not need to be handled, it's already set above
			}

			ಭಙ್ಗ; // handle next led
		}
	}
}
