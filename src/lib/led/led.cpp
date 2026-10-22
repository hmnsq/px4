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

पूर्णाङ्क LedController::update(Led_Control_दत्त &control_दत्त)
{
	तार्किक had_changes = असत्य; // did one of the outputs change?

	// check for parameter updates
	यदि (_parameter_update_sub.updated()) {
		// clear update
		parameter_update_s pupdate;
		_parameter_update_sub.copy(&pupdate);

		updateParams();

		ध्रुव uint8_t max_brightness_पूर्व = _max_brightness;

		// set maximum brightness (0-255) from percentage
		_max_brightness = roundf(math::constrain(_param_sys_rgb_maxbrt.get(), 0.f, 1.f) * UINT8_MAX);

		// update existing
		प्रति (पूर्णाङ्क i = 0; i < BOARD_MAX_LEDS; ++i) {
			control_दत्त.leds[i].brightness = math::min(_max_brightness, control_दत्त.leds[i].brightness);
		}

		यदि (_max_brightness != max_brightness_पूर्व) {
			had_changes = सत्य;
		}
	}

	यावत् (_led_control_sub.updated() || _force_update) {
		ध्रुव अचिह्नित चरमः_generation = _led_control_sub.get_चरमः_generation();

		// handle new state
		led_control_s led_control;

		यदि (_led_control_sub.copy(&led_control)) {

			यदि ((_led_control_sub.get_चरमः_generation() != चरमः_generation + 1)
			    && (_led_control_sub.get_चरमः_generation() != चरमः_generation)) {
				perf_गणनम्(led_control_sub_नष्टम्_perf);
				PX4_DEBUG("led_control lost, generation %d -> %d", चरमः_generation, _led_control_sub.get_चरमः_generation());
			}

			// don't apply the new state just yet to avoid interrupting an ongoing blinking state
			प्रति (पूर्णाङ्क i = 0; i < BOARD_MAX_LEDS; ++i) {
				यदि (led_control.led_आवरणम् & (1 << i)) {
					// if next state has already a higher priority state than
					// led_control, set lower prio state directly, so that this
					// information is not lost
					यदि (अवस्थाः[i].अग्र_state.is_valid() && led_control.प्राथमिकता < अवस्थाः[i].अग्र_state.प्राथमिकता) {
						अवस्थाः[i].समुच्चयः(led_control);

					} अन्यथा {
						// if a lower prio event is already in next state and a
						// higher prio event is coming in
						यदि (अवस्थाः[i].अग्र_state.is_valid() && led_control.प्राथमिकता > अवस्थाः[i].अग्र_state.प्राथमिकता) {
							अवस्थाः[i].apply_अग्र_state();
						}

						अवस्थाः[i].अग्र_state.समुच्चयः(led_control);
					}
				}
			}
		}

		_force_update = असत्य;
	}

	// handle state updates
	hrt_abstime now = hrt_absolute_time();

	यदि (चरमः_update_call == 0) {
		चरमः_update_call = now;
		प्रत्यागम 0;
	}

	uint16_t blink_delta_t = (uint16_t)((now - चरमः_update_call) / 100); // Note: this is in 0.1ms
	constexpr uint16_t breathe_duration = BREATHE_INTERVAL * BREATHE_चरणाः / 100;

	पूर्णाङ्क अङ्कः_blinking_leds = 0;
	पूर्णाङ्क अङ्कः_blinking_do_not_change_state = 0;
	पूर्णाङ्क current_priorities[BOARD_MAX_LEDS];

	प्रति (पूर्णाङ्क i = 0; i < BOARD_MAX_LEDS; ++i) {
		पूर्णाङ्क प्राथमिकता = led_control_s::MAX_प्राथमिकता;

		प्रति (; प्राथमिकता >= 0; --प्राथमिकता) {

			प्रत्येकम्_प्राथमिकता_दत्त &cur_दत्त = अवस्थाः[i].प्राथमिकता[प्राथमिकता];

			यदि (cur_दत्त.बहुलकम् == led_control_s::बहुलकम्_DISABLED) {
				अनुवर्त; // handle next priority
			}

			// handle state updates
			uint16_t current_blink_duration = 0;

			विकल्प (cur_दत्त.बहुलकम्) {
			पक्ष led_control_s::बहुलकम्_FLASH:
			पक्ष led_control_s::बहुलकम्_BLINK_FAST:
				current_blink_duration = BLINK_FAST_DURATION / 100;
				भङ्ग;

			पक्ष led_control_s::बहुलकम्_BLINK_प्रसामान्यम्:
				current_blink_duration = BLINK_प्रसामान्यम्_DURATION / 100;
				भङ्ग;

			पक्ष led_control_s::बहुलकम्_BLINK_SLOW:
				current_blink_duration = BLINK_SLOW_DURATION / 100;
				भङ्ग;

			पक्ष led_control_s::बहुलकम्_BREATHE:
				अवस्थाः[i].current_blinking_time += blink_delta_t;

				यावत् (अवस्थाः[i].current_blinking_time > breathe_duration) {
					अवस्थाः[i].current_blinking_time -= breathe_duration;
				}

				had_changes = सत्य;
				भङ्ग;
			}

			यदि (current_blink_duration > 0) {
				++अङ्कः_blinking_leds;

				अवस्थाः[i].current_blinking_time += blink_delta_t;

				यदि (अवस्थाः[i].current_blinking_time > current_blink_duration) {
					अवस्थाः[i].current_blinking_time -= current_blink_duration;

					यदि (cur_दत्त.blink_times_वामः == 246) {
						// handle toggling for infinite case: decrease between 255 and 246
						// In order to handle the flash mode infinite case it needs a
						// total of 10 steps.
						cur_दत्त.blink_times_वामः = 255;
						++अङ्कः_blinking_do_not_change_state;

					} अन्यथा यदि (--cur_दत्त.blink_times_वामः == 0) {
						cur_दत्त.बहुलकम् = led_control_s::बहुलकम्_DISABLED;
						अवस्थाः[i].current_blinking_time = 0;

					} अन्यथा यदि (cur_दत्त.blink_times_वामः % 2 == 1) {
						++अङ्कः_blinking_do_not_change_state;
					}

					had_changes = सत्य;

				} अन्यथा {
					++अङ्कः_blinking_do_not_change_state;
				}
			}

			भङ्ग; // handle next led
		}

		current_priorities[i] = प्राथमिकता;

	}

	// handle next state:
	// only allow a state change if no led blinks or at least one of the blinking leds signals that it's ok to switch.
	// This makes sure all leds are kept in sync, but does not allow interrupting at arbitrary points.
	यदि (अङ्कः_blinking_leds == 0 || अङ्कः_blinking_leds > अङ्कः_blinking_do_not_change_state) {
		प्रति (पूर्णाङ्क i = 0; i < BOARD_MAX_LEDS; ++i) {
			यदि (अवस्थाः[i].अग्र_state.is_valid()) {
				पूर्णाङ्क अग्र_प्राथमिकता = (पूर्णाङ्क)अवस्थाः[i].अग्र_state.प्राथमिकता;

				यदि (अग्र_प्राथमिकता >= current_priorities[i]) {
					अवस्थाः[i].current_blinking_time = 0;
					had_changes = सत्य;
				}

				अवस्थाः[i].apply_अग्र_state();
				अवस्थाः[i].अग्र_state.reset();
			}

		}
	}

	चरमः_update_call = now;

	यदि (!had_changes) {
		प्रत्यागम 0;
	}

	// create output
	get_control_दत्त(control_दत्त);

	प्रत्यागम 1;
}

शून्य LedController::get_control_दत्त(Led_Control_दत्त &control_दत्त)
{
	_breathe_enabled = असत्य;

	प्रति (पूर्णाङ्क i = 0; i < BOARD_MAX_LEDS; ++i) {
		control_दत्त.leds[i].color = led_control_s::COLOR_OFF; // set output to a defined state
		control_दत्त.leds[i].brightness = _max_brightness;

		प्रति (पूर्णाङ्क प्राथमिकता = led_control_s::MAX_प्राथमिकता; प्राथमिकता >= 0; --प्राथमिकता) {
			तार्किक flash_output_सक्रिय = सत्य;
			ध्रुव प्रत्येकम्_प्राथमिकता_दत्त &cur_दत्त = अवस्थाः[i].प्राथमिकता[प्राथमिकता];

			यदि (cur_दत्त.बहुलकम् == led_control_s::बहुलकम्_DISABLED) {
				अनुवर्त; // handle next priority
			}

			विकल्प (cur_दत्त.बहुलकम्) {
			पक्ष led_control_s::बहुलकम्_ON:
				control_दत्त.leds[i].color = cur_दत्त.color;
				भङ्ग;

			पक्ष led_control_s::बहुलकम्_BREATHE: {
					// fade on and off
					पूर्णाङ्क गणक = अवस्थाः[i].current_blinking_time / (BREATHE_INTERVAL / 100);
					पूर्णाङ्क n = गणक >= (BREATHE_चरणाः / 2) ? BREATHE_चरणाः - गणक : गणक;
					control_दत्त.leds[i].brightness = (n * n) * _max_brightness / (BREATHE_चरणाः * BREATHE_चरणाः / 4); // (n/(steps/2))^2
					control_दत्त.leds[i].color = cur_दत्त.color;
					_breathe_enabled = सत्य;
					भङ्ग;
				}

			पक्ष led_control_s::बहुलकम्_FLASH:
				यदि (cur_दत्त.blink_times_वामः % 10 < 6) { // 2 blinks, then turn off for the rest of the cycle
					flash_output_सक्रिय = असत्य;
				}

			/* FALLTHROUGH */
			पक्ष led_control_s::बहुलकम्_BLINK_FAST:
			पक्ष led_control_s::बहुलकम्_BLINK_प्रसामान्यम्:
			पक्ष led_control_s::बहुलकम्_BLINK_SLOW:
				यदि (cur_दत्त.blink_times_वामः % 2 == 0 && flash_output_सक्रिय) {
					control_दत्त.leds[i].color = cur_दत्त.color;
				}

				भङ्ग;
				// MODE_OFF does not need to be handled, it's already set above
			}

			भङ्ग; // handle next led
		}
	}
}
