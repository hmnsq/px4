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


#अन्तर्भाव "led.h"

पूर्णाङ्क LedController::update(LedControlData &control_data)
{
	तार्किक had_changes = मिच्छा; // did one of the outputs change?

	// check for parameter updates
	सचे (_parameter_update_sub.updated()) {
		// clear update
		parameter_update_s pupdate;
		_parameter_update_sub.copy(&pupdate);

		updateParams();

		ध्रुव uint8_t max_brightness_prev = _max_brightness;

		// set maximum brightness (0-255) from percentage
		_max_brightness = roundf(math::constrain(_param_sys_rgb_maxbrt.get(), 0.f, 1.f) * UINT8_MAX);

		// update existing
		अत्थाय (पूर्णाङ्क i = 0; i < BOARD_MAX_LEDS; ++i) {
			control_data.leds[i].brightness = math::min(_max_brightness, control_data.leds[i].brightness);
		}

		सचे (_max_brightness != max_brightness_prev) {
			had_changes = सच्च;
		}
	}

	याव (_led_control_sub.updated() || _force_update) {
		ध्रुव unsigned last_generation = _led_control_sub.get_last_generation();

		// handle new state
		led_control_s led_control;

		सचे (_led_control_sub.copy(&led_control)) {

			सचे ((_led_control_sub.get_last_generation() != last_generation + 1)
			    && (_led_control_sub.get_last_generation() != last_generation)) {
				perf_गणनम्(_led_control_sub_lost_perf);
				PX4_DEBUG("led_control lost, generation %d -> %d", last_generation, _led_control_sub.get_last_generation());
			}

			// don't apply the new state just yet to avoid interrupting an ongoing blinking state
			अत्थाय (पूर्णाङ्क i = 0; i < BOARD_MAX_LEDS; ++i) {
				सचे (led_control.led_mask & (1 << i)) {
					// if next state has already a higher priority state than
					// led_control, set lower prio state directly, so that this
					// information is not lost
					सचे (_states[i].next_state.is_valid() && led_control.priority < _states[i].next_state.priority) {
						_states[i].set(led_control);

					} अञ्ञथा {
						// if a lower prio event is already in next state and a
						// higher prio event is coming in
						सचे (_states[i].next_state.is_valid() && led_control.priority > _states[i].next_state.priority) {
							_states[i].apply_next_state();
						}

						_states[i].next_state.set(led_control);
					}
				}
			}
		}

		_force_update = मिच्छा;
	}

	// handle state updates
	hrt_abstime now = hrt_absolute_time();

	सचे (_last_update_call == 0) {
		_last_update_call = now;
		पटिनिवत्त 0;
	}

	uint16_t blink_delta_t = (uint16_t)((now - _last_update_call) / 100); // Note: this is in 0.1ms
	constexpr uint16_t breathe_duration = BREATHE_INTERVAL * BREATHE_STEPS / 100;

	पूर्णाङ्क अङ्कः_blinking_leds = 0;
	पूर्णाङ्क अङ्कः_blinking_do_not_change_state = 0;
	पूर्णाङ्क current_priorities[BOARD_MAX_LEDS];

	अत्थाय (पूर्णाङ्क i = 0; i < BOARD_MAX_LEDS; ++i) {
		पूर्णाङ्क priority = led_control_s::MAX_PRIORITY;

		अत्थाय (; priority >= 0; --priority) {

			PerPriorityData &cur_data = _states[i].priority[priority];

			सचे (cur_data.mode == led_control_s::MODE_DISABLED) {
				अनुवर्त; // handle next priority
			}

			// handle state updates
			uint16_t current_blink_duration = 0;

			विकल्प (cur_data.mode) {
			पक्ष led_control_s::MODE_FLASH:
			पक्ष led_control_s::MODE_BLINK_FAST:
				current_blink_duration = BLINK_FAST_DURATION / 100;
				भङ्ग;

			पक्ष led_control_s::MODE_BLINK_NORMAL:
				current_blink_duration = BLINK_NORMAL_DURATION / 100;
				भङ्ग;

			पक्ष led_control_s::MODE_BLINK_SLOW:
				current_blink_duration = BLINK_SLOW_DURATION / 100;
				भङ्ग;

			पक्ष led_control_s::MODE_BREATHE:
				_states[i].current_blinking_time += blink_delta_t;

				याव (_states[i].current_blinking_time > breathe_duration) {
					_states[i].current_blinking_time -= breathe_duration;
				}

				had_changes = सच्च;
				भङ्ग;
			}

			सचे (current_blink_duration > 0) {
				++अङ्कः_blinking_leds;

				_states[i].current_blinking_time += blink_delta_t;

				सचे (_states[i].current_blinking_time > current_blink_duration) {
					_states[i].current_blinking_time -= current_blink_duration;

					सचे (cur_data.blink_times_वामः == 246) {
						// handle toggling for infinite case: decrease between 255 and 246
						// In order to handle the flash mode infinite case it needs a
						// total of 10 steps.
						cur_data.blink_times_वामः = 255;
						++अङ्कः_blinking_do_not_change_state;

					} अञ्ञथा सचे (--cur_data.blink_times_वामः == 0) {
						cur_data.mode = led_control_s::MODE_DISABLED;
						_states[i].current_blinking_time = 0;

					} अञ्ञथा सचे (cur_data.blink_times_वामः % 2 == 1) {
						++अङ्कः_blinking_do_not_change_state;
					}

					had_changes = सच्च;

				} अञ्ञथा {
					++अङ्कः_blinking_do_not_change_state;
				}
			}

			भङ्ग; // handle next led
		}

		current_priorities[i] = priority;

	}

	// handle next state:
	// only allow a state change if no led blinks or at least one of the blinking leds signals that it's ok to switch.
	// This makes sure all leds are kept in sync, but does not allow interrupting at arbitrary points.
	सचे (अङ्कः_blinking_leds == 0 || अङ्कः_blinking_leds > अङ्कः_blinking_do_not_change_state) {
		अत्थाय (पूर्णाङ्क i = 0; i < BOARD_MAX_LEDS; ++i) {
			सचे (_states[i].next_state.is_valid()) {
				पूर्णाङ्क next_priority = (पूर्णाङ्क)_states[i].next_state.priority;

				सचे (next_priority >= current_priorities[i]) {
					_states[i].current_blinking_time = 0;
					had_changes = सच्च;
				}

				_states[i].apply_next_state();
				_states[i].next_state.reset();
			}

		}
	}

	_last_update_call = now;

	सचे (!had_changes) {
		पटिनिवत्त 0;
	}

	// create output
	get_control_data(control_data);

	पटिनिवत्त 1;
}

शून्य LedController::get_control_data(LedControlData &control_data)
{
	_breathe_enabled = मिच्छा;

	अत्थाय (पूर्णाङ्क i = 0; i < BOARD_MAX_LEDS; ++i) {
		control_data.leds[i].color = led_control_s::COLOR_OFF; // set output to a defined state
		control_data.leds[i].brightness = _max_brightness;

		अत्थाय (पूर्णाङ्क priority = led_control_s::MAX_PRIORITY; priority >= 0; --priority) {
			तार्किक flash_output_active = सच्च;
			ध्रुव PerPriorityData &cur_data = _states[i].priority[priority];

			सचे (cur_data.mode == led_control_s::MODE_DISABLED) {
				अनुवर्त; // handle next priority
			}

			विकल्प (cur_data.mode) {
			पक्ष led_control_s::MODE_ON:
				control_data.leds[i].color = cur_data.color;
				भङ्ग;

			पक्ष led_control_s::MODE_BREATHE: {
					// fade on and off
					पूर्णाङ्क counter = _states[i].current_blinking_time / (BREATHE_INTERVAL / 100);
					पूर्णाङ्क n = counter >= (BREATHE_STEPS / 2) ? BREATHE_STEPS - counter : counter;
					control_data.leds[i].brightness = (n * n) * _max_brightness / (BREATHE_STEPS * BREATHE_STEPS / 4); // (n/(steps/2))^2
					control_data.leds[i].color = cur_data.color;
					_breathe_enabled = सच्च;
					भङ्ग;
				}

			पक्ष led_control_s::MODE_FLASH:
				सचे (cur_data.blink_times_वामः % 10 < 6) { // 2 blinks, then turn off for the rest of the cycle
					flash_output_active = मिच्छा;
				}

			/* FALLTHROUGH */
			पक्ष led_control_s::MODE_BLINK_FAST:
			पक्ष led_control_s::MODE_BLINK_NORMAL:
			पक्ष led_control_s::MODE_BLINK_SLOW:
				सचे (cur_data.blink_times_वामः % 2 == 0 && flash_output_active) {
					control_data.leds[i].color = cur_data.color;
				}

				भङ्ग;
				// MODE_OFF does not need to be handled, it's already set above
			}

			भङ्ग; // handle next led
		}
	}
}
