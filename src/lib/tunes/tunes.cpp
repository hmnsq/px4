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

#شامل "tunes.h"

#شامل <px4_platform_common/log.h>

#شامل <lib/circuit_breaker/circuit_breaker.h>

#شامل <math.h>
#شامل <ctype.h>
#شامل <errno.h>

#تعریف_کرو BEAT_وقت_CONVERSION_MS (60 * 1000 * 4)
#تعریف_کرو BEAT_وقت_CONVERSION_US (BEAT_وقت_CONVERSION_MS * 1000)
#تعریف_کرو BEAT_وقت_CONVERSION    BEAT_وقت_CONVERSION_US

// semitone offsets from C for the characters 'A'-'G'
ثابت uint8_t Tunes::_note_tab[] = {9, 11, 0, 2, 4, 5, 7};

Tunes::Tunes(غیرعلامتی default_note_لمبائی, Note_منوال default_note_منوال,
	     غیرعلامتی default_octave, غیرعلامتی default_tempo):
	default_note_لمبائی_2(default_note_لمبائی),
	default_note_منوال_2(default_note_منوال),
	_default_octave(default_octave),
	_default_tempo(default_tempo)
{
	اگر (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_کلید)) {
		_tunes_disabled = سچ;
	}

	reset(جھوٹ);
}

خالی Tunes::reset(منطقی repeat_flag)
{
	// reset pointer
	اگر (!repeat_flag)	{
		_tune = خالی_اشاریہ;
		اگلا_tune = خالی_اشاریہ;

	} ورنہ {
		_tune = tune_آغاز_ptr;
		اگلا_tune = _tune;
	}

	// reset music parameter
	note_لمبائی = default_note_لمبائی_2;
	note_منوال   = default_note_منوال_2;
	_octave      = _default_octave;
	_tempo       = _default_tempo;
}

Tunes::Control_نتیجہ Tunes::سیٹ_control(ثابت tune_control_s &tune_control)
{
	// Sanity check
	اگر (tune_control.tune_id >= default_tunes_سائز) {
		واپس Control_نتیجہ::InvalidTune;
	}

	// Accept new tune or a stop?
	اگر (_tune == خالی_اشاریہ ||  	   // No tune is currently being played
	    tune_control.tune_override ||  // Override interrupts everything
	    _default_tunes_interruptable[_current_tune_id]) {

		// Check if this exact tune is already being played back
		اگر (tune_control.tune_id != static_cast<صحیح>(TuneID::CUSTOM) &&
		    _tune == _default_tunes[tune_control.tune_id]) {
			واپس Control_نتیجہ::AlreadyPlaying; // Nothing to do
		}

		// Reset repeat flag. Can jump to true again while tune is being parsed later
		_repeat = جھوٹ;

		// Reset octave, tempo etc.
		reset(_repeat);

		// Volume will remain valid for the entire tune, unless interrupted.
		اگر (tune_control.volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = tune_control.volume;

		} ورنہ {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}


		// Special treatment for custom tunes
		اگر (tune_control.tune_id == static_cast<صحیح>(TuneID::CUSTOM)) {
			using_custom_پیغام = سچ;
			_frequency        = (غیرعلامتی)tune_control.frequency;
			_duration         = (غیرعلامتی)tune_control.duration;
			_silence          = (غیرعلامتی)tune_control.silence;

		} ورنہ {
			using_custom_پیغام = جھوٹ;
			_tune             = _default_tunes[tune_control.tune_id];
			tune_آغاز_ptr   = _tune;
			اگلا_tune        = _tune;
		}

		_current_tune_id = tune_control.tune_id;
		واپس Control_نتیجہ::Success;
	}

	واپس Control_نتیجہ::Would_مداخلت;
}

خالی Tunes::سیٹ_string(ثابت حرف *ثابت string, uint8_t volume)
{
	// Only play new tune if nothing is being played currently.
	اگر (_tune == خالی_اشاریہ) {
		// Set tune string the first time.
		_tune           = string;
		tune_آغاز_ptr = string;
		اگلا_tune      = _tune;

		اگر (volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = volume;

		} ورنہ {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}
	}
}

Tunes::حالت_3 Tunes::get_اگلا_note(غیرعلامتی &frequency, غیرعلامتی &duration, غیرعلامتی &silence, uint8_t &volume)
{
	اگر (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		volume = 0;
		واپس Tunes::حالت_3::Stop;
	}

	Tunes::حالت_3 ret = get_اگلا_note(frequency, duration, silence);

	// Check if note should not be heard -> adjust volume to 0 to be safe.
	اگر (frequency == 0 || duration == 0) {
		volume = 0;

	} ورنہ {
		volume = _volume;
	}

	واپس ret;
}

Tunes::حالت_3 Tunes::get_اگلا_note(غیرعلامتی &frequency, غیرعلامتی &duration, غیرعلامتی &silence)
{
	اگر (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		واپس Tunes::حالت_3::Stop;
	}

	// Return the values for frequency and duration if the custom msg was received.
	اگر (using_custom_پیغام) {
		using_custom_پیغام = جھوٹ;
		duration  = _duration;
		frequency = _frequency;
		silence   = _silence;
		واپس Tunes::حالت_3::Continue;
	}

	// Make sure we still have a tune.
	اگر ((اگلا_tune == خالی_اشاریہ) || (_tune == خالی_اشاریہ)) {
		واپس tune_error();
	}

	// Parse characters out of the string until we have resolved a note.
	غیرعلامتی note = 0;
	غیرعلامتی note_لمبائی_2 = note_لمبائی;

	جب (note == 0) {
		// we always need at least one character from the string.
		صحیح c = اگلا_char();

		اگر (c == 0) {
			silence = 0;
			واپس tune_انتہا();
		}

		اگلا_tune++;

		انتخاب (c) {

		حالت 'V':	// Select volume.
			// Consume volume if specified in tune but ignore
			// MML compatibility
			اگلا_نمبر();
			توڑو;

		حالت 'L':	// Select note length.
			note_لمبائی = اگلا_نمبر();

			اگر (note_لمبائی < 1) {
				واپس tune_error();
			}

			توڑو;

		حالت 'O':	// Select octave.
			_octave = اگلا_نمبر();

			اگر (_octave > 6) {
				_octave = 6;
			}

			توڑو;

		حالت '<':	// Decrease octave.
			اگر (_octave > 0) {
				_octave--;
			}

			توڑو;

		حالت '>':	// Increase octave.
			اگر (_octave < 6) {
				_octave++;
			}

			توڑو;

		حالت 'M':	// Select inter-note gap.
			c = اگلا_char();

			اگر (c == 0) {
				واپس tune_error();
			}

			اگلا_tune++;

			انتخاب (c) {
			حالت 'N':
				note_منوال = Note_منوال::معمول;
				توڑو;

			حالت 'L':
				note_منوال = Note_منوال::LEGATO;
				توڑو;

			حالت 'S':
				note_منوال = Note_منوال::STACCATO;
				توڑو;

			حالت 'F':
				_repeat = جھوٹ;
				توڑو;

			حالت 'B':
				_repeat = سچ;
				توڑو;

			طےشدہ:
				واپس tune_error();
			}

			توڑو;

		حالت 'P':	// Pause for a note length.
		حالت 'R':
			frequency = 0;
			duration = 0;
			note_لمبائی_2 = اگلا_نمبر();
			silence = rest_duration(note_لمبائی_2, اگلا_dots());
			واپس Tunes::حالت_3::Continue;

		حالت 'T': {	// Change tempo.
				غیرعلامتی nt = اگلا_نمبر();

				اگر ((nt >= 32) && (nt <= 255)) {
					_tempo = nt;

				} ورنہ {
					واپس tune_error();
				}

				توڑو;
			}

		حالت 'N':	// Play an arbitrary note.
			note = اگلا_نمبر();

			اگر (note > 84) {
				واپس tune_error();
			}

			اگر (note == 0) {
				// This is a rest - pause for the current note length.
				silence = rest_duration(note_لمبائی, اگلا_dots());
				واپس Tunes::حالت_3::Continue;
			}

			توڑو;

		حالت 'A'...'G':	// Play a note in the current octave.
			note = _note_tab[c - 'A'] + (_octave * 12) + 1;
			c = اگلا_char();

			انتخاب (c) {
			حالت '#':	// Up a semitone.
			حالت '+':
				اگر (note < 84) {
					note++;
				}

				اگلا_tune++;
				توڑو;

			حالت '-':	// Down a semitone.
				اگر (note > 1) {
					note--;
				}

				اگلا_tune++;
				توڑو;

			طےشدہ:
				// 0 / No next char here is OK.
				توڑو;
			}

			// Shorthand length notation.
			note_لمبائی_2 = اگلا_نمبر();

			اگر (note_لمبائی_2 == 0) {
				note_لمبائی_2 = note_لمبائی;
			}

			توڑو;

		طےشدہ:
			واپس tune_error();
		}
	}

	// Compute the duration of the note and the following silence (if any).
	duration = note_duration(silence, note_لمبائی_2, اگلا_dots());

	// Compute the note frequency.
	frequency = note_مقصد_frequency(note);
	واپس Tunes::حالت_3::Continue;
}

Tunes::حالت_3 Tunes::tune_انتہا()
{
	اگر (_tunes_disabled) {
		واپس Tunes::حالت_3::Stop;
	}

	// Restore intial parameters.
	reset(_repeat);

	// Stop or restart the tune.
	اگر (_repeat) {
		واپس Tunes::حالت_3::Continue;

	} ورنہ {
		واپس Tunes::حالت_3::Stop;
	}
}

Tunes::حالت_3 Tunes::tune_error()
{
	اگر (_tunes_disabled) {
		واپس Tunes::حالت_3::Stop;
	}

	// The tune appears to be bad (unexpected EOF, bad character, etc.).
	اگر (اگلا_tune != خالی_اشاریہ) {
		PX4_WARN("Tune error at: %s", اگلا_tune);
	}

	_repeat = جھوٹ;	// Don't loop on error.
	reset(_repeat);
	واپس Tunes::حالت_3::Error;
}

uint32_t Tunes::note_مقصد_frequency(غیرعلامتی note) ثابت
{
	// Compute the frequency (Hz).
	واپس (غیرعلامتی)(880.0f * powf(2.0f, ((صحیح)note - 46) / 12.0f));
}

غیرعلامتی Tunes::note_duration(غیرعلامتی &silence, غیرعلامتی note_لمبائی_2, غیرعلامتی dots) ثابت
{
	غیرعلامتی پورا_note_period = BEAT_وقت_CONVERSION / _tempo;

	اگر (note_لمبائی_2 == 0) {
		note_لمبائی_2 = 1;
	}

	غیرعلامتی note_period = پورا_note_period / note_لمبائی_2;

	انتخاب (note_منوال) {
	حالت Note_منوال::معمول:
		silence = note_period / 8;
		توڑو;

	حالت Note_منوال::STACCATO:
		silence = note_period / 4;
		توڑو;

	حالت Note_منوال::LEGATO:
	طےشدہ:
		silence = 0;
		توڑو;
	}

	note_period -= silence;

	غیرعلامتی dot_extension = note_period / 2;

	جب (dots--) {
		note_period += dot_extension;
		dot_extension /= 2;
	}

	واپس note_period;
}

غیرعلامتی Tunes::rest_duration(غیرعلامتی rest_لمبائی, غیرعلامتی dots) ثابت
{
	غیرعلامتی پورا_note_period = BEAT_وقت_CONVERSION / _tempo;

	اگر (rest_لمبائی == 0) {
		rest_لمبائی = 1;
	}

	غیرعلامتی rest_period = پورا_note_period / rest_لمبائی;

	غیرعلامتی dot_extension = rest_period / 2;

	جب (dots--) {
		rest_period += dot_extension;
		dot_extension /= 2;
	}

	واپس rest_period;
}

صحیح Tunes::اگلا_char()
{
	جب (خلا_ہے(*اگلا_tune)) {
		اگلا_tune++;
	}

	واپس بڑا_حرف(*اگلا_tune);
}

غیرعلامتی Tunes::اگلا_نمبر()
{
	غیرعلامتی نمبر = 0;
	صحیح اگلا_character;

	برائے (;;) {
		اگلا_character = اگلا_char();

		اگر (!ہندسہ_ہے(اگلا_character)) {
			واپس نمبر;
		}

		اگلا_tune++;
		نمبر = (نمبر * 10) + (اگلا_character - '0');
	}
}

غیرعلامتی Tunes::اگلا_dots()
{
	غیرعلامتی dots = 0;

	جب (اگلا_char() == '.') {
		اگلا_tune++;
		dots++;
	}

	واپس dots;
}
