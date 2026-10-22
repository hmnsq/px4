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
 * @file tunes.cpp
 */

#అన్తర్భావ "tunes.h"

#అన్తర్భావ <px4_platform_common/log.h>

#అన్తర్భావ <lib/circuit_breaker/circuit_breaker.h>

#అన్తర్భావ <math.h>
#అన్తర్భావ <ctype.h>
#అన్తర్భావ <errno.h>

#నిర్వచించు BEAT_సమయం_CONVERSION_MS (60 * 1000 * 4)
#నిర్వచించు BEAT_సమయం_CONVERSION_US (BEAT_సమయం_CONVERSION_MS * 1000)
#నిర్వచించు BEAT_సమయం_CONVERSION    BEAT_సమయం_CONVERSION_US

// semitone offsets from C for the characters 'A'-'G'
స్థిరరాశి uint8_t Tunes::_note_tab[] = {9, 11, 0, 2, 4, 5, 7};

Tunes::Tunes(గుర్తులేని default_note_నిడివి, Note_బహుళకం default_note_బహుళకం,
	     గుర్తులేని default_octave, గుర్తులేని default_tempo):
	default_note_నిడివి_2(default_note_నిడివి),
	default_note_బహుళకం_2(default_note_బహుళకం),
	_default_octave(default_octave),
	_default_tempo(default_tempo)
{
	ఒకవేళ (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_కీలకం)) {
		_tunes_disabled = నిజం;
	}

	reset(అబద్ధం);
}

శూన్య Tunes::reset(తార్కిక repeat_flag)
{
	// reset pointer
	ఒకవేళ (!repeat_flag)	{
		_tune = శూన్యసూచిక;
		తదుపరి_tune = శూన్యసూచిక;

	} లేకపోతే {
		_tune = tune_ప్రారంభం_ptr;
		తదుపరి_tune = _tune;
	}

	// reset music parameter
	note_నిడివి = default_note_నిడివి_2;
	note_బహుళకం   = default_note_బహుళకం_2;
	_octave      = _default_octave;
	_tempo       = _default_tempo;
}

Tunes::Control_ఫలితం Tunes::సమితి_control(స్థిరరాశి tune_control_s &tune_control)
{
	// Sanity check
	ఒకవేళ (tune_control.tune_id >= default_tunes_కొలత) {
		తిరిగి Control_ఫలితం::InvalidTune;
	}

	// Accept new tune or a stop?
	ఒకవేళ (_tune == శూన్యసూచిక ||  	   // No tune is currently being played
	    tune_control.tune_override ||  // Override interrupts everything
	    _default_tunes_interruptable[_current_tune_id]) {

		// Check if this exact tune is already being played back
		ఒకవేళ (tune_control.tune_id != static_cast<పూర్ణాంకం>(TuneID::CUSTOM) &&
		    _tune == _default_tunes[tune_control.tune_id]) {
			తిరిగి Control_ఫలితం::AlreadyPlaying; // Nothing to do
		}

		// Reset repeat flag. Can jump to true again while tune is being parsed later
		_repeat = అబద్ధం;

		// Reset octave, tempo etc.
		reset(_repeat);

		// Volume will remain valid for the entire tune, unless interrupted.
		ఒకవేళ (tune_control.volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = tune_control.volume;

		} లేకపోతే {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}


		// Special treatment for custom tunes
		ఒకవేళ (tune_control.tune_id == static_cast<పూర్ణాంకం>(TuneID::CUSTOM)) {
			using_custom_సందేశం = నిజం;
			_frequency        = (గుర్తులేని)tune_control.frequency;
			_duration         = (గుర్తులేని)tune_control.duration;
			_silence          = (గుర్తులేని)tune_control.silence;

		} లేకపోతే {
			using_custom_సందేశం = అబద్ధం;
			_tune             = _default_tunes[tune_control.tune_id];
			tune_ప్రారంభం_ptr   = _tune;
			తదుపరి_tune        = _tune;
		}

		_current_tune_id = tune_control.tune_id;
		తిరిగి Control_ఫలితం::Success;
	}

	తిరిగి Control_ఫలితం::Would_అంతరాయం;
}

శూన్య Tunes::సమితి_string(స్థిరరాశి అక్షరం *స్థిరరాశి string, uint8_t volume)
{
	// Only play new tune if nothing is being played currently.
	ఒకవేళ (_tune == శూన్యసూచిక) {
		// Set tune string the first time.
		_tune           = string;
		tune_ప్రారంభం_ptr = string;
		తదుపరి_tune      = _tune;

		ఒకవేళ (volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = volume;

		} లేకపోతే {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}
	}
}

Tunes::స్థితి_3 Tunes::get_తదుపరి_note(గుర్తులేని &frequency, గుర్తులేని &duration, గుర్తులేని &silence, uint8_t &volume)
{
	ఒకవేళ (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		volume = 0;
		తిరిగి Tunes::స్థితి_3::Stop;
	}

	Tunes::స్థితి_3 ret = get_తదుపరి_note(frequency, duration, silence);

	// Check if note should not be heard -> adjust volume to 0 to be safe.
	ఒకవేళ (frequency == 0 || duration == 0) {
		volume = 0;

	} లేకపోతే {
		volume = _volume;
	}

	తిరిగి ret;
}

Tunes::స్థితి_3 Tunes::get_తదుపరి_note(గుర్తులేని &frequency, గుర్తులేని &duration, గుర్తులేని &silence)
{
	ఒకవేళ (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		తిరిగి Tunes::స్థితి_3::Stop;
	}

	// Return the values for frequency and duration if the custom msg was received.
	ఒకవేళ (using_custom_సందేశం) {
		using_custom_సందేశం = అబద్ధం;
		duration  = _duration;
		frequency = _frequency;
		silence   = _silence;
		తిరిగి Tunes::స్థితి_3::Continue;
	}

	// Make sure we still have a tune.
	ఒకవేళ ((తదుపరి_tune == శూన్యసూచిక) || (_tune == శూన్యసూచిక)) {
		తిరిగి tune_error();
	}

	// Parse characters out of the string until we have resolved a note.
	గుర్తులేని note = 0;
	గుర్తులేని note_నిడివి_2 = note_నిడివి;

	అయితే (note == 0) {
		// we always need at least one character from the string.
		పూర్ణాంకం c = తదుపరి_char();

		ఒకవేళ (c == 0) {
			silence = 0;
			తిరిగి tune_చివర();
		}

		తదుపరి_tune++;

		ఎంపిక (c) {

		స్థితి 'V':	// Select volume.
			// Consume volume if specified in tune but ignore
			// MML compatibility
			తదుపరి_సంఖ్య();
			ఆపు;

		స్థితి 'L':	// Select note length.
			note_నిడివి = తదుపరి_సంఖ్య();

			ఒకవేళ (note_నిడివి < 1) {
				తిరిగి tune_error();
			}

			ఆపు;

		స్థితి 'O':	// Select octave.
			_octave = తదుపరి_సంఖ్య();

			ఒకవేళ (_octave > 6) {
				_octave = 6;
			}

			ఆపు;

		స్థితి '<':	// Decrease octave.
			ఒకవేళ (_octave > 0) {
				_octave--;
			}

			ఆపు;

		స్థితి '>':	// Increase octave.
			ఒకవేళ (_octave < 6) {
				_octave++;
			}

			ఆపు;

		స్థితి 'M':	// Select inter-note gap.
			c = తదుపరి_char();

			ఒకవేళ (c == 0) {
				తిరిగి tune_error();
			}

			తదుపరి_tune++;

			ఎంపిక (c) {
			స్థితి 'N':
				note_బహుళకం = Note_బహుళకం::సాధారణ;
				ఆపు;

			స్థితి 'L':
				note_బహుళకం = Note_బహుళకం::LEGATO;
				ఆపు;

			స్థితి 'S':
				note_బహుళకం = Note_బహుళకం::STACCATO;
				ఆపు;

			స్థితి 'F':
				_repeat = అబద్ధం;
				ఆపు;

			స్థితి 'B':
				_repeat = నిజం;
				ఆపు;

			అప్రమేయ:
				తిరిగి tune_error();
			}

			ఆపు;

		స్థితి 'P':	// Pause for a note length.
		స్థితి 'R':
			frequency = 0;
			duration = 0;
			note_నిడివి_2 = తదుపరి_సంఖ్య();
			silence = rest_duration(note_నిడివి_2, తదుపరి_dots());
			తిరిగి Tunes::స్థితి_3::Continue;

		స్థితి 'T': {	// Change tempo.
				గుర్తులేని nt = తదుపరి_సంఖ్య();

				ఒకవేళ ((nt >= 32) && (nt <= 255)) {
					_tempo = nt;

				} లేకపోతే {
					తిరిగి tune_error();
				}

				ఆపు;
			}

		స్థితి 'N':	// Play an arbitrary note.
			note = తదుపరి_సంఖ్య();

			ఒకవేళ (note > 84) {
				తిరిగి tune_error();
			}

			ఒకవేళ (note == 0) {
				// This is a rest - pause for the current note length.
				silence = rest_duration(note_నిడివి, తదుపరి_dots());
				తిరిగి Tunes::స్థితి_3::Continue;
			}

			ఆపు;

		స్థితి 'A'...'G':	// Play a note in the current octave.
			note = _note_tab[c - 'A'] + (_octave * 12) + 1;
			c = తదుపరి_char();

			ఎంపిక (c) {
			స్థితి '#':	// Up a semitone.
			స్థితి '+':
				ఒకవేళ (note < 84) {
					note++;
				}

				తదుపరి_tune++;
				ఆపు;

			స్థితి '-':	// Down a semitone.
				ఒకవేళ (note > 1) {
					note--;
				}

				తదుపరి_tune++;
				ఆపు;

			అప్రమేయ:
				// 0 / No next char here is OK.
				ఆపు;
			}

			// Shorthand length notation.
			note_నిడివి_2 = తదుపరి_సంఖ్య();

			ఒకవేళ (note_నిడివి_2 == 0) {
				note_నిడివి_2 = note_నిడివి;
			}

			ఆపు;

		అప్రమేయ:
			తిరిగి tune_error();
		}
	}

	// Compute the duration of the note and the following silence (if any).
	duration = note_duration(silence, note_నిడివి_2, తదుపరి_dots());

	// Compute the note frequency.
	frequency = note_గమ్యం_frequency(note);
	తిరిగి Tunes::స్థితి_3::Continue;
}

Tunes::స్థితి_3 Tunes::tune_చివర()
{
	ఒకవేళ (_tunes_disabled) {
		తిరిగి Tunes::స్థితి_3::Stop;
	}

	// Restore intial parameters.
	reset(_repeat);

	// Stop or restart the tune.
	ఒకవేళ (_repeat) {
		తిరిగి Tunes::స్థితి_3::Continue;

	} లేకపోతే {
		తిరిగి Tunes::స్థితి_3::Stop;
	}
}

Tunes::స్థితి_3 Tunes::tune_error()
{
	ఒకవేళ (_tunes_disabled) {
		తిరిగి Tunes::స్థితి_3::Stop;
	}

	// The tune appears to be bad (unexpected EOF, bad character, etc.).
	ఒకవేళ (తదుపరి_tune != శూన్యసూచిక) {
		PX4_WARN("Tune error at: %s", తదుపరి_tune);
	}

	_repeat = అబద్ధం;	// Don't loop on error.
	reset(_repeat);
	తిరిగి Tunes::స్థితి_3::Error;
}

uint32_t Tunes::note_గమ్యం_frequency(గుర్తులేని note) స్థిరరాశి
{
	// Compute the frequency (Hz).
	తిరిగి (గుర్తులేని)(880.0f * powf(2.0f, ((పూర్ణాంకం)note - 46) / 12.0f));
}

గుర్తులేని Tunes::note_duration(గుర్తులేని &silence, గుర్తులేని note_నిడివి_2, గుర్తులేని dots) స్థిరరాశి
{
	గుర్తులేని సంపూర్ణ_note_period = BEAT_సమయం_CONVERSION / _tempo;

	ఒకవేళ (note_నిడివి_2 == 0) {
		note_నిడివి_2 = 1;
	}

	గుర్తులేని note_period = సంపూర్ణ_note_period / note_నిడివి_2;

	ఎంపిక (note_బహుళకం) {
	స్థితి Note_బహుళకం::సాధారణ:
		silence = note_period / 8;
		ఆపు;

	స్థితి Note_బహుళకం::STACCATO:
		silence = note_period / 4;
		ఆపు;

	స్థితి Note_బహుళకం::LEGATO:
	అప్రమేయ:
		silence = 0;
		ఆపు;
	}

	note_period -= silence;

	గుర్తులేని dot_extension = note_period / 2;

	అయితే (dots--) {
		note_period += dot_extension;
		dot_extension /= 2;
	}

	తిరిగి note_period;
}

గుర్తులేని Tunes::rest_duration(గుర్తులేని rest_నిడివి, గుర్తులేని dots) స్థిరరాశి
{
	గుర్తులేని సంపూర్ణ_note_period = BEAT_సమయం_CONVERSION / _tempo;

	ఒకవేళ (rest_నిడివి == 0) {
		rest_నిడివి = 1;
	}

	గుర్తులేని rest_period = సంపూర్ణ_note_period / rest_నిడివి;

	గుర్తులేని dot_extension = rest_period / 2;

	అయితే (dots--) {
		rest_period += dot_extension;
		dot_extension /= 2;
	}

	తిరిగి rest_period;
}

పూర్ణాంకం Tunes::తదుపరి_char()
{
	అయితే (ఖాళీనా(*తదుపరి_tune)) {
		తదుపరి_tune++;
	}

	తిరిగి పెద్ద_అక్షరం(*తదుపరి_tune);
}

గుర్తులేని Tunes::తదుపరి_సంఖ్య()
{
	గుర్తులేని సంఖ్య = 0;
	పూర్ణాంకం తదుపరి_character;

	కొరకు (;;) {
		తదుపరి_character = తదుపరి_char();

		ఒకవేళ (!అంకెనా(తదుపరి_character)) {
			తిరిగి సంఖ్య;
		}

		తదుపరి_tune++;
		సంఖ్య = (సంఖ్య * 10) + (తదుపరి_character - '0');
	}
}

గుర్తులేని Tunes::తదుపరి_dots()
{
	గుర్తులేని dots = 0;

	అయితే (తదుపరి_char() == '.') {
		తదుపరి_tune++;
		dots++;
	}

	తిరిగి dots;
}
