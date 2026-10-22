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

#تعریف_کن BEAT_زمان_CONVERSION_MS (60 * 1000 * 4)
#تعریف_کن BEAT_زمان_CONVERSION_US (BEAT_زمان_CONVERSION_MS * 1000)
#تعریف_کن BEAT_زمان_CONVERSION    BEAT_زمان_CONVERSION_US

// semitone offsets from C for the characters 'A'-'G'
ثابت uint8_t Tunes::_note_tab[] = {9, 11, 0, 2, 4, 5, 7};

Tunes::Tunes(بیعلامت default_note_درازی, Note_نما default_note_نما,
	     بیعلامت default_octave, بیعلامت default_tempo):
	default_note_درازی_2(default_note_درازی),
	default_note_نما_2(default_note_نما),
	_default_octave(default_octave),
	_default_tempo(default_tempo)
{
	إذا (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_کلید)) {
		_tunes_disabled = درست;
	}

	reset(خطأ);
}

فراغ Tunes::reset(منطقي repeat_flag)
{
	// reset pointer
	إذا (!repeat_flag)	{
		_tune = اشاره_تهی;
		بعدی_tune = اشاره_تهی;

	} وإلا {
		_tune = tune_شروع_ptr;
		بعدی_tune = _tune;
	}

	// reset music parameter
	note_درازی = default_note_درازی_2;
	note_نما   = default_note_نما_2;
	_octave      = _default_octave;
	_tempo       = _default_tempo;
}

Tunes::Control_نتیجه Tunes::مجموعه_control(ثابت tune_control_s &tune_control)
{
	// Sanity check
	إذا (tune_control.tune_id >= default_tunes_اندازه) {
		أرجع Control_نتیجه::InvalidTune;
	}

	// Accept new tune or a stop?
	إذا (_tune == اشاره_تهی ||  	   // No tune is currently being played
	    tune_control.tune_override ||  // Override interrupts everything
	    _default_tunes_interruptable[_current_tune_id]) {

		// Check if this exact tune is already being played back
		إذا (tune_control.tune_id != static_cast<صحيح>(TuneID::CUSTOM) &&
		    _tune == _default_tunes[tune_control.tune_id]) {
			أرجع Control_نتیجه::AlreadyPlaying; // Nothing to do
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
			using_custom_پیام = درست;
			_frequency        = (بیعلامت)tune_control.frequency;
			_duration         = (بیعلامت)tune_control.duration;
			_silence          = (بیعلامت)tune_control.silence;

		} وإلا {
			using_custom_پیام = خطأ;
			_tune             = _default_tunes[tune_control.tune_id];
			tune_شروع_ptr   = _tune;
			بعدی_tune        = _tune;
		}

		_current_tune_id = tune_control.tune_id;
		أرجع Control_نتیجه::Success;
	}

	أرجع Control_نتیجه::Would_وقفه;
}

فراغ Tunes::مجموعه_string(ثابت حرف *ثابت string, uint8_t volume)
{
	// Only play new tune if nothing is being played currently.
	إذا (_tune == اشاره_تهی) {
		// Set tune string the first time.
		_tune           = string;
		tune_شروع_ptr = string;
		بعدی_tune      = _tune;

		إذا (volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = volume;

		} وإلا {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}
	}
}

Tunes::وضعیت_2 Tunes::get_بعدی_note(بیعلامت &frequency, بیعلامت &duration, بیعلامت &silence, uint8_t &volume)
{
	إذا (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		volume = 0;
		أرجع Tunes::وضعیت_2::Stop;
	}

	Tunes::وضعیت_2 ret = get_بعدی_note(frequency, duration, silence);

	// Check if note should not be heard -> adjust volume to 0 to be safe.
	إذا (frequency == 0 || duration == 0) {
		volume = 0;

	} وإلا {
		volume = _volume;
	}

	أرجع ret;
}

Tunes::وضعیت_2 Tunes::get_بعدی_note(بیعلامت &frequency, بیعلامت &duration, بیعلامت &silence)
{
	إذا (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		أرجع Tunes::وضعیت_2::Stop;
	}

	// Return the values for frequency and duration if the custom msg was received.
	إذا (using_custom_پیام) {
		using_custom_پیام = خطأ;
		duration  = _duration;
		frequency = _frequency;
		silence   = _silence;
		أرجع Tunes::وضعیت_2::Continue;
	}

	// Make sure we still have a tune.
	إذا ((بعدی_tune == اشاره_تهی) || (_tune == اشاره_تهی)) {
		أرجع tune_error();
	}

	// Parse characters out of the string until we have resolved a note.
	بیعلامت note = 0;
	بیعلامت note_درازی_2 = note_درازی;

	طالما (note == 0) {
		// we always need at least one character from the string.
		صحيح c = بعدی_char();

		إذا (c == 0) {
			silence = 0;
			أرجع tune_انتها();
		}

		بعدی_tune++;

		بدل (c) {

		حالة 'V':	// Select volume.
			// Consume volume if specified in tune but ignore
			// MML compatibility
			بعدی_عدد();
			اكسر;

		حالة 'L':	// Select note length.
			note_درازی = بعدی_عدد();

			إذا (note_درازی < 1) {
				أرجع tune_error();
			}

			اكسر;

		حالة 'O':	// Select octave.
			_octave = بعدی_عدد();

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
			c = بعدی_char();

			إذا (c == 0) {
				أرجع tune_error();
			}

			بعدی_tune++;

			بدل (c) {
			حالة 'N':
				note_نما = Note_نما::نرمال;
				اكسر;

			حالة 'L':
				note_نما = Note_نما::LEGATO;
				اكسر;

			حالة 'S':
				note_نما = Note_نما::STACCATO;
				اكسر;

			حالة 'F':
				_repeat = خطأ;
				اكسر;

			حالة 'B':
				_repeat = درست;
				اكسر;

			افتراضي:
				أرجع tune_error();
			}

			اكسر;

		حالة 'P':	// Pause for a note length.
		حالة 'R':
			frequency = 0;
			duration = 0;
			note_درازی_2 = بعدی_عدد();
			silence = rest_duration(note_درازی_2, بعدی_dots());
			أرجع Tunes::وضعیت_2::Continue;

		حالة 'T': {	// Change tempo.
				بیعلامت nt = بعدی_عدد();

				إذا ((nt >= 32) && (nt <= 255)) {
					_tempo = nt;

				} وإلا {
					أرجع tune_error();
				}

				اكسر;
			}

		حالة 'N':	// Play an arbitrary note.
			note = بعدی_عدد();

			إذا (note > 84) {
				أرجع tune_error();
			}

			إذا (note == 0) {
				// This is a rest - pause for the current note length.
				silence = rest_duration(note_درازی, بعدی_dots());
				أرجع Tunes::وضعیت_2::Continue;
			}

			اكسر;

		حالة 'A'...'G':	// Play a note in the current octave.
			note = _note_tab[c - 'A'] + (_octave * 12) + 1;
			c = بعدی_char();

			بدل (c) {
			حالة '#':	// Up a semitone.
			حالة '+':
				إذا (note < 84) {
					note++;
				}

				بعدی_tune++;
				اكسر;

			حالة '-':	// Down a semitone.
				إذا (note > 1) {
					note--;
				}

				بعدی_tune++;
				اكسر;

			افتراضي:
				// 0 / No next char here is OK.
				اكسر;
			}

			// Shorthand length notation.
			note_درازی_2 = بعدی_عدد();

			إذا (note_درازی_2 == 0) {
				note_درازی_2 = note_درازی;
			}

			اكسر;

		افتراضي:
			أرجع tune_error();
		}
	}

	// Compute the duration of the note and the following silence (if any).
	duration = note_duration(silence, note_درازی_2, بعدی_dots());

	// Compute the note frequency.
	frequency = note_مقصد_frequency(note);
	أرجع Tunes::وضعیت_2::Continue;
}

Tunes::وضعیت_2 Tunes::tune_انتها()
{
	إذا (_tunes_disabled) {
		أرجع Tunes::وضعیت_2::Stop;
	}

	// Restore intial parameters.
	reset(_repeat);

	// Stop or restart the tune.
	إذا (_repeat) {
		أرجع Tunes::وضعیت_2::Continue;

	} وإلا {
		أرجع Tunes::وضعیت_2::Stop;
	}
}

Tunes::وضعیت_2 Tunes::tune_error()
{
	إذا (_tunes_disabled) {
		أرجع Tunes::وضعیت_2::Stop;
	}

	// The tune appears to be bad (unexpected EOF, bad character, etc.).
	إذا (بعدی_tune != اشاره_تهی) {
		PX4_WARN("Tune error at: %s", بعدی_tune);
	}

	_repeat = خطأ;	// Don't loop on error.
	reset(_repeat);
	أرجع Tunes::وضعیت_2::Error;
}

uint32_t Tunes::note_مقصد_frequency(بیعلامت note) ثابت
{
	// Compute the frequency (Hz).
	أرجع (بیعلامت)(880.0f * powf(2.0f, ((صحيح)note - 46) / 12.0f));
}

بیعلامت Tunes::note_duration(بیعلامت &silence, بیعلامت note_درازی_2, بیعلامت dots) ثابت
{
	بیعلامت کامل_note_period = BEAT_زمان_CONVERSION / _tempo;

	إذا (note_درازی_2 == 0) {
		note_درازی_2 = 1;
	}

	بیعلامت note_period = کامل_note_period / note_درازی_2;

	بدل (note_نما) {
	حالة Note_نما::نرمال:
		silence = note_period / 8;
		اكسر;

	حالة Note_نما::STACCATO:
		silence = note_period / 4;
		اكسر;

	حالة Note_نما::LEGATO:
	افتراضي:
		silence = 0;
		اكسر;
	}

	note_period -= silence;

	بیعلامت dot_extension = note_period / 2;

	طالما (dots--) {
		note_period += dot_extension;
		dot_extension /= 2;
	}

	أرجع note_period;
}

بیعلامت Tunes::rest_duration(بیعلامت rest_درازی, بیعلامت dots) ثابت
{
	بیعلامت کامل_note_period = BEAT_زمان_CONVERSION / _tempo;

	إذا (rest_درازی == 0) {
		rest_درازی = 1;
	}

	بیعلامت rest_period = کامل_note_period / rest_درازی;

	بیعلامت dot_extension = rest_period / 2;

	طالما (dots--) {
		rest_period += dot_extension;
		dot_extension /= 2;
	}

	أرجع rest_period;
}

صحيح Tunes::بعدی_char()
{
	طالما (آیا_فاصله(*بعدی_tune)) {
		بعدی_tune++;
	}

	أرجع به_بزرگ(*بعدی_tune);
}

بیعلامت Tunes::بعدی_عدد()
{
	بیعلامت عدد = 0;
	صحيح بعدی_character;

	لكل (;;) {
		بعدی_character = بعدی_char();

		إذا (!آیا_رقم(بعدی_character)) {
			أرجع عدد;
		}

		بعدی_tune++;
		عدد = (عدد * 10) + (بعدی_character - '0');
	}
}

بیعلامت Tunes::بعدی_dots()
{
	بیعلامت dots = 0;

	طالما (بعدی_char() == '.') {
		بعدی_tune++;
		dots++;
	}

	أرجع dots;
}
