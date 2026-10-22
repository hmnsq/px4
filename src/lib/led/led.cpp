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


#અન્તર્ભાવ "led.h"

પૂર્ણાંક LedController::update(LedControlData &control_data)
{
	તાર્કિક had_changes = ખોટું; // did one of the outputs change?

	// check for parameter updates
	જો (_parameter_update_sub.updated()) {
		// clear update
		parameter_update_s pupdate;
		_parameter_update_sub.copy(&pupdate);

		updateParams();

		અચલ uint8_t max_brightness_prev = _max_brightness;

		// set maximum brightness (0-255) from percentage
		_max_brightness = roundf(math::constrain(_param_sys_rgb_maxbrt.get(), 0.f, 1.f) * UINT8_MAX);

		// update existing
		માટે (પૂર્ણાંક i = 0; i < BOARD_MAX_LEDS; ++i) {
			control_data.leds[i].brightness = math::min(_max_brightness, control_data.leds[i].brightness);
		}

		જો (_max_brightness != max_brightness_prev) {
			had_changes = સાચું;
		}
	}

	જ્યારે (_led_control_sub.updated() || _force_update) {
		અચલ unsigned last_generation = _led_control_sub.get_last_generation();

		// handle new state
		led_control_s led_control;

		જો (_led_control_sub.copy(&led_control)) {

			જો ((_led_control_sub.get_last_generation() != last_generation + 1)
			    && (_led_control_sub.get_last_generation() != last_generation)) {
				perf_ગણતરી(_led_control_sub_lost_perf);
				PX4_DEBUG("led_control lost, generation %d -> %d", last_generation, _led_control_sub.get_last_generation());
			}

			// don't apply the new state just yet to avoid interrupting an ongoing blinking state
			માટે (પૂર્ણાંક i = 0; i < BOARD_MAX_LEDS; ++i) {
				જો (led_control.led_mask & (1 << i)) {
					// if next state has already a higher priority state than
					// led_control, set lower prio state directly, so that this
					// information is not lost
					જો (_states[i].next_state.is_valid() && led_control.priority < _states[i].next_state.priority) {
						_states[i].set(led_control);

					} નહીંતર {
						// if a lower prio event is already in next state and a
						// higher prio event is coming in
						જો (_states[i].next_state.is_valid() && led_control.priority > _states[i].next_state.priority) {
							_states[i].apply_next_state();
						}

						_states[i].next_state.set(led_control);
					}
				}
			}
		}

		_force_update = ખોટું;
	}

	// handle state updates
	hrt_abstime now = hrt_absolute_time();

	જો (_last_update_call == 0) {
		_last_update_call = now;
		પાછા 0;
	}

	uint16_t blink_delta_t = (uint16_t)((now - _last_update_call) / 100); // Note: this is in 0.1ms
	constexpr uint16_t breathe_duration = BREATHE_INTERVAL * BREATHE_STEPS / 100;

	પૂર્ણાંક અંક_blinking_leds = 0;
	પૂર્ણાંક અંક_blinking_do_not_change_state = 0;
	પૂર્ણાંક current_priorities[BOARD_MAX_LEDS];

	માટે (પૂર્ણાંક i = 0; i < BOARD_MAX_LEDS; ++i) {
		પૂર્ણાંક priority = led_control_s::MAX_PRIORITY;

		માટે (; priority >= 0; --priority) {

			PerPriorityData &cur_data = _states[i].priority[priority];

			જો (cur_data.mode == led_control_s::MODE_DISABLED) {
				ચાલુ; // handle next priority
			}

			// handle state updates
			uint16_t current_blink_duration = 0;

			પસંદગી (cur_data.mode) {
			સ્થિતિ led_control_s::MODE_FLASH:
			સ્થિતિ led_control_s::MODE_BLINK_FAST:
				current_blink_duration = BLINK_FAST_DURATION / 100;
				તોડો;

			સ્થિતિ led_control_s::MODE_BLINK_NORMAL:
				current_blink_duration = BLINK_NORMAL_DURATION / 100;
				તોડો;

			સ્થિતિ led_control_s::MODE_BLINK_SLOW:
				current_blink_duration = BLINK_SLOW_DURATION / 100;
				તોડો;

			સ્થિતિ led_control_s::MODE_BREATHE:
				_states[i].current_blinking_time += blink_delta_t;

				જ્યારે (_states[i].current_blinking_time > breathe_duration) {
					_states[i].current_blinking_time -= breathe_duration;
				}

				had_changes = સાચું;
				તોડો;
			}

			જો (current_blink_duration > 0) {
				++અંક_blinking_leds;

				_states[i].current_blinking_time += blink_delta_t;

				જો (_states[i].current_blinking_time > current_blink_duration) {
					_states[i].current_blinking_time -= current_blink_duration;

					જો (cur_data.blink_times_ડાબું == 246) {
						// handle toggling for infinite case: decrease between 255 and 246
						// In order to handle the flash mode infinite case it needs a
						// total of 10 steps.
						cur_data.blink_times_ડાબું = 255;
						++અંક_blinking_do_not_change_state;

					} નહીંતર જો (--cur_data.blink_times_ડાબું == 0) {
						cur_data.mode = led_control_s::MODE_DISABLED;
						_states[i].current_blinking_time = 0;

					} નહીંતર જો (cur_data.blink_times_ડાબું % 2 == 1) {
						++અંક_blinking_do_not_change_state;
					}

					had_changes = સાચું;

				} નહીંતર {
					++અંક_blinking_do_not_change_state;
				}
			}

			તોડો; // handle next led
		}

		current_priorities[i] = priority;

	}

	// handle next state:
	// only allow a state change if no led blinks or at least one of the blinking leds signals that it's ok to switch.
	// This makes sure all leds are kept in sync, but does not allow interrupting at arbitrary points.
	જો (અંક_blinking_leds == 0 || અંક_blinking_leds > અંક_blinking_do_not_change_state) {
		માટે (પૂર્ણાંક i = 0; i < BOARD_MAX_LEDS; ++i) {
			જો (_states[i].next_state.is_valid()) {
				પૂર્ણાંક next_priority = (પૂર્ણાંક)_states[i].next_state.priority;

				જો (next_priority >= current_priorities[i]) {
					_states[i].current_blinking_time = 0;
					had_changes = સાચું;
				}

				_states[i].apply_next_state();
				_states[i].next_state.reset();
			}

		}
	}

	_last_update_call = now;

	જો (!had_changes) {
		પાછા 0;
	}

	// create output
	get_control_data(control_data);

	પાછા 1;
}

શૂન્ય LedController::get_control_data(LedControlData &control_data)
{
	_breathe_enabled = ખોટું;

	માટે (પૂર્ણાંક i = 0; i < BOARD_MAX_LEDS; ++i) {
		control_data.leds[i].color = led_control_s::COLOR_OFF; // set output to a defined state
		control_data.leds[i].brightness = _max_brightness;

		માટે (પૂર્ણાંક priority = led_control_s::MAX_PRIORITY; priority >= 0; --priority) {
			તાર્કિક flash_output_active = સાચું;
			અચલ PerPriorityData &cur_data = _states[i].priority[priority];

			જો (cur_data.mode == led_control_s::MODE_DISABLED) {
				ચાલુ; // handle next priority
			}

			પસંદગી (cur_data.mode) {
			સ્થિતિ led_control_s::MODE_ON:
				control_data.leds[i].color = cur_data.color;
				તોડો;

			સ્થિતિ led_control_s::MODE_BREATHE: {
					// fade on and off
					પૂર્ણાંક counter = _states[i].current_blinking_time / (BREATHE_INTERVAL / 100);
					પૂર્ણાંક n = counter >= (BREATHE_STEPS / 2) ? BREATHE_STEPS - counter : counter;
					control_data.leds[i].brightness = (n * n) * _max_brightness / (BREATHE_STEPS * BREATHE_STEPS / 4); // (n/(steps/2))^2
					control_data.leds[i].color = cur_data.color;
					_breathe_enabled = સાચું;
					તોડો;
				}

			સ્થિતિ led_control_s::MODE_FLASH:
				જો (cur_data.blink_times_ડાબું % 10 < 6) { // 2 blinks, then turn off for the rest of the cycle
					flash_output_active = ખોટું;
				}

			/* FALLTHROUGH */
			સ્થિતિ led_control_s::MODE_BLINK_FAST:
			સ્થિતિ led_control_s::MODE_BLINK_NORMAL:
			સ્થિતિ led_control_s::MODE_BLINK_SLOW:
				જો (cur_data.blink_times_ડાબું % 2 == 0 && flash_output_active) {
					control_data.leds[i].color = cur_data.color;
				}

				તોડો;
				// MODE_OFF does not need to be handled, it's already set above
			}

			તોડો; // handle next led
		}
	}
}
