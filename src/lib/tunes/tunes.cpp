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

#ضمن "tunes.h"

#ضمن <px4_platform_common/log.h>

#ضمن <lib/circuit_breaker/circuit_breaker.h>

#ضمن <math.h>
#ضمن <ctype.h>
#ضمن <errno.h>

#وصف_ڪريو BEAT_وقت_CONVERSION_MS (60 * 1000 * 4)
#وصف_ڪريو BEAT_وقت_CONVERSION_US (BEAT_وقت_CONVERSION_MS * 1000)
#وصف_ڪريو BEAT_وقت_CONVERSION    BEAT_وقت_CONVERSION_US

// semitone offsets from C for the characters 'A'-'G'
ثابت uint8_t Tunes::_note_tab[] = {9, 11, 0, 2, 4, 5, 7};

Tunes::Tunes(بنانشان default_note_ڊيگهائي, Note_گهڻائي default_note_گهڻائي,
	     بنانشان default_octave, بنانشان default_tempo):
	default_note_ڊيگهائي_2(default_note_ڊيگهائي),
	default_note_گهڻائي_2(default_note_گهڻائي),
	_default_octave(default_octave),
	_default_tempo(default_tempo)
{
	إذا (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_ڪنجي)) {
		_tunes_disabled = سچ;
	}

	reset(خطأ);
}

فراغ Tunes::reset(منطقي repeat_flag)
{
	// reset pointer
	إذا (!repeat_flag)	{
		_tune = خالي_اشارو;
		ايندڙ_tune = خالي_اشارو;

	} وإلا {
		_tune = tune_شروعات_ptr;
		ايندڙ_tune = _tune;
	}

	// reset music parameter
	note_ڊيگهائي = default_note_ڊيگهائي_2;
	note_گهڻائي   = default_note_گهڻائي_2;
	_octave      = _default_octave;
	_tempo       = _default_tempo;
}

Tunes::Control_نتيجو Tunes::سيٽ_control(ثابت tune_control_s &tune_control)
{
	// Sanity check
	إذا (tune_control.tune_id >= default_tunes_سائيز) {
		أرجع Control_نتيجو::InvalidTune;
	}

	// Accept new tune or a stop?
	إذا (_tune == خالي_اشارو ||  	   // No tune is currently being played
	    tune_control.tune_override ||  // Override interrupts everything
	    _default_tunes_interruptable[_current_tune_id]) {

		// Check if this exact tune is already being played back
		إذا (tune_control.tune_id != static_cast<صحيح>(TuneID::CUSTOM) &&
		    _tune == _default_tunes[tune_control.tune_id]) {
			أرجع Control_نتيجو::AlreadyPlaying; // Nothing to do
		}

		// Reset repeat flag. Can jump to true again while tune is being parsed later
		_repeat = خطأ;

		// Reset octave, tempo etc.
		reset(_repeat);

		// Volume will remain valid for the entire tune, unless interrupted.
		إذا (tune_control.volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = tune_control.volume;

		} وإلا {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}


		// Special treatment for custom tunes
		إذا (tune_control.tune_id == static_cast<صحيح>(TuneID::CUSTOM)) {
			using_custom_نياپو = سچ;
			_frequency        = (بنانشان)tune_control.frequency;
			_duration         = (بنانشان)tune_control.duration;
			_silence          = (بنانشان)tune_control.silence;

		} وإلا {
			using_custom_نياپو = خطأ;
			_tune             = _default_tunes[tune_control.tune_id];
			tune_شروعات_ptr   = _tune;
			ايندڙ_tune        = _tune;
		}

		_current_tune_id = tune_control.tune_id;
		أرجع Control_نتيجو::Success;
	}

	أرجع Control_نتيجو::Would_مداخلت;
}

فراغ Tunes::سيٽ_string(ثابت حرف *ثابت string, uint8_t volume)
{
	// Only play new tune if nothing is being played currently.
	إذا (_tune == خالي_اشارو) {
		// Set tune string the first time.
		_tune           = string;
		tune_شروعات_ptr = string;
		ايندڙ_tune      = _tune;

		إذا (volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = volume;

		} وإلا {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}
	}
}

Tunes::حالت_2 Tunes::get_ايندڙ_note(بنانشان &frequency, بنانشان &duration, بنانشان &silence, uint8_t &volume)
{
	إذا (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		volume = 0;
		أرجع Tunes::حالت_2::Stop;
	}

	Tunes::حالت_2 ret = get_ايندڙ_note(frequency, duration, silence);

	// Check if note should not be heard -> adjust volume to 0 to be safe.
	إذا (frequency == 0 || duration == 0) {
		volume = 0;

	} وإلا {
		volume = _volume;
	}

	أرجع ret;
}

Tunes::حالت_2 Tunes::get_ايندڙ_note(بنانشان &frequency, بنانشان &duration, بنانشان &silence)
{
	إذا (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		أرجع Tunes::حالت_2::Stop;
	}

	// Return the values for frequency and duration if the custom msg was received.
	إذا (using_custom_نياپو) {
		using_custom_نياپو = خطأ;
		duration  = _duration;
		frequency = _frequency;
		silence   = _silence;
		أرجع Tunes::حالت_2::Continue;
	}

	// Make sure we still have a tune.
	إذا ((ايندڙ_tune == خالي_اشارو) || (_tune == خالي_اشارو)) {
		أرجع tune_error();
	}

	// Parse characters out of the string until we have resolved a note.
	بنانشان note = 0;
	بنانشان note_ڊيگهائي_2 = note_ڊيگهائي;

	طالما (note == 0) {
		// we always need at least one character from the string.
		صحيح c = ايندڙ_char();

		إذا (c == 0) {
			silence = 0;
			أرجع tune_پڇاڙي();
		}

		ايندڙ_tune++;

		بدل (c) {

		حالة 'V':	// Select volume.
			// Consume volume if specified in tune but ignore
			// MML compatibility
			ايندڙ_عدد();
			اكسر;

		حالة 'L':	// Select note length.
			note_ڊيگهائي = ايندڙ_عدد();

			إذا (note_ڊيگهائي < 1) {
				أرجع tune_error();
			}

			اكسر;

		حالة 'O':	// Select octave.
			_octave = ايندڙ_عدد();

			إذا (_octave > 6) {
				_octave = 6;
			}

			اكسر;

		حالة '<':	// Decrease octave.
			إذا (_octave > 0) {
				_octave--;
			}

			اكسر;

		حالة '>':	// Increase octave.
			إذا (_octave < 6) {
				_octave++;
			}

			اكسر;

		حالة 'M':	// Select inter-note gap.
			c = ايندڙ_char();

			إذا (c == 0) {
				أرجع tune_error();
			}

			ايندڙ_tune++;

			بدل (c) {
			حالة 'N':
				note_گهڻائي = Note_گهڻائي::عام_2;
				اكسر;

			حالة 'L':
				note_گهڻائي = Note_گهڻائي::LEGATO;
				اكسر;

			حالة 'S':
				note_گهڻائي = Note_گهڻائي::STACCATO;
				اكسر;

			حالة 'F':
				_repeat = خطأ;
				اكسر;

			حالة 'B':
				_repeat = سچ;
				اكسر;

			افتراضي:
				أرجع tune_error();
			}

			اكسر;

		حالة 'P':	// Pause for a note length.
		حالة 'R':
			frequency = 0;
			duration = 0;
			note_ڊيگهائي_2 = ايندڙ_عدد();
			silence = rest_duration(note_ڊيگهائي_2, ايندڙ_dots());
			أرجع Tunes::حالت_2::Continue;

		حالة 'T': {	// Change tempo.
				بنانشان nt = ايندڙ_عدد();

				إذا ((nt >= 32) && (nt <= 255)) {
					_tempo = nt;

				} وإلا {
					أرجع tune_error();
				}

				اكسر;
			}

		حالة 'N':	// Play an arbitrary note.
			note = ايندڙ_عدد();

			إذا (note > 84) {
				أرجع tune_error();
			}

			إذا (note == 0) {
				// This is a rest - pause for the current note length.
				silence = rest_duration(note_ڊيگهائي, ايندڙ_dots());
				أرجع Tunes::حالت_2::Continue;
			}

			اكسر;

		حالة 'A'...'G':	// Play a note in the current octave.
			note = _note_tab[c - 'A'] + (_octave * 12) + 1;
			c = ايندڙ_char();

			بدل (c) {
			حالة '#':	// Up a semitone.
			حالة '+':
				إذا (note < 84) {
					note++;
				}

				ايندڙ_tune++;
				اكسر;

			حالة '-':	// Down a semitone.
				إذا (note > 1) {
					note--;
				}

				ايندڙ_tune++;
				اكسر;

			افتراضي:
				// 0 / No next char here is OK.
				اكسر;
			}

			// Shorthand length notation.
			note_ڊيگهائي_2 = ايندڙ_عدد();

			إذا (note_ڊيگهائي_2 == 0) {
				note_ڊيگهائي_2 = note_ڊيگهائي;
			}

			اكسر;

		افتراضي:
			أرجع tune_error();
		}
	}

	// Compute the duration of the note and the following silence (if any).
	duration = note_duration(silence, note_ڊيگهائي_2, ايندڙ_dots());

	// Compute the note frequency.
	frequency = note_منزل_frequency(note);
	أرجع Tunes::حالت_2::Continue;
}

Tunes::حالت_2 Tunes::tune_پڇاڙي()
{
	إذا (_tunes_disabled) {
		أرجع Tunes::حالت_2::Stop;
	}

	// Restore intial parameters.
	reset(_repeat);

	// Stop or restart the tune.
	إذا (_repeat) {
		أرجع Tunes::حالت_2::Continue;

	} وإلا {
		أرجع Tunes::حالت_2::Stop;
	}
}

Tunes::حالت_2 Tunes::tune_error()
{
	إذا (_tunes_disabled) {
		أرجع Tunes::حالت_2::Stop;
	}

	// The tune appears to be bad (unexpected EOF, bad character, etc.).
	إذا (ايندڙ_tune != خالي_اشارو) {
		PX4_WARN("Tune error at: %s", ايندڙ_tune);
	}

	_repeat = خطأ;	// Don't loop on error.
	reset(_repeat);
	أرجع Tunes::حالت_2::Error;
}

uint32_t Tunes::note_منزل_frequency(بنانشان note) ثابت
{
	// Compute the frequency (Hz).
	أرجع (بنانشان)(880.0f * powf(2.0f, ((صحيح)note - 46) / 12.0f));
}

بنانشان Tunes::note_duration(بنانشان &silence, بنانشان note_ڊيگهائي_2, بنانشان dots) ثابت
{
	بنانشان سڄو_note_period = BEAT_وقت_CONVERSION / _tempo;

	إذا (note_ڊيگهائي_2 == 0) {
		note_ڊيگهائي_2 = 1;
	}

	بنانشان note_period = سڄو_note_period / note_ڊيگهائي_2;

	بدل (note_گهڻائي) {
	حالة Note_گهڻائي::عام_2:
		silence = note_period / 8;
		اكسر;

	حالة Note_گهڻائي::STACCATO:
		silence = note_period / 4;
		اكسر;

	حالة Note_گهڻائي::LEGATO:
	افتراضي:
		silence = 0;
		اكسر;
	}

	note_period -= silence;

	بنانشان dot_extension = note_period / 2;

	طالما (dots--) {
		note_period += dot_extension;
		dot_extension /= 2;
	}

	أرجع note_period;
}

بنانشان Tunes::rest_duration(بنانشان rest_ڊيگهائي, بنانشان dots) ثابت
{
	بنانشان سڄو_note_period = BEAT_وقت_CONVERSION / _tempo;

	إذا (rest_ڊيگهائي == 0) {
		rest_ڊيگهائي = 1;
	}

	بنانشان rest_period = سڄو_note_period / rest_ڊيگهائي;

	بنانشان dot_extension = rest_period / 2;

	طالما (dots--) {
		rest_period += dot_extension;
		dot_extension /= 2;
	}

	أرجع rest_period;
}

صحيح Tunes::ايندڙ_char()
{
	طالما (خال_آهي(*ايندڙ_tune)) {
		ايندڙ_tune++;
	}

	أرجع وڏو_اکر(*ايندڙ_tune);
}

بنانشان Tunes::ايندڙ_عدد()
{
	بنانشان عدد = 0;
	صحيح ايندڙ_character;

	لكل (;;) {
		ايندڙ_character = ايندڙ_char();

		إذا (!انگ_آهي(ايندڙ_character)) {
			أرجع عدد;
		}

		ايندڙ_tune++;
		عدد = (عدد * 10) + (ايندڙ_character - '0');
	}
}

بنانشان Tunes::ايندڙ_dots()
{
	بنانشان dots = 0;

	طالما (ايندڙ_char() == '.') {
		ايندڙ_tune++;
		dots++;
	}

	أرجع dots;
}
