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

#تعریف_کړه BEAT_وخت_CONVERSION_MS (60 * 1000 * 4)
#تعریف_کړه BEAT_وخت_CONVERSION_US (BEAT_وخت_CONVERSION_MS * 1000)
#تعریف_کړه BEAT_وخت_CONVERSION    BEAT_وخت_CONVERSION_US

// semitone offsets from C for the characters 'A'-'G'
ثابت uint8_t Tunes::_note_tab[] = {9, 11, 0, 2, 4, 5, 7};

Tunes::Tunes(بینښه default_note_اوږدتیا, Note_ډېرپېښ default_note_ډېرپېښ,
	     بینښه default_octave, بینښه default_tempo):
	default_note_اوږدتیا_2(default_note_اوږدتیا),
	default_note_ډېرپېښ_2(default_note_ډېرپېښ),
	_default_octave(default_octave),
	_default_tempo(default_tempo)
{
	إذا (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_کلی)) {
		_tunes_disabled = ریښتیا;
	}

	reset(خطأ);
}

فراغ Tunes::reset(منطقي repeat_flag)
{
	// reset pointer
	إذا (!repeat_flag)	{
		_tune = تش_اشاره;
		بل_tune = تش_اشاره;

	} وإلا {
		_tune = tune_پیل_ptr;
		بل_tune = _tune;
	}

	// reset music parameter
	note_اوږدتیا = default_note_اوږدتیا_2;
	note_ډېرپېښ   = default_note_ډېرپېښ_2;
	_octave      = _default_octave;
	_tempo       = _default_tempo;
}

Tunes::Control_پایله Tunes::سېټ_control(ثابت tune_control_s &tune_control)
{
	// Sanity check
	إذا (tune_control.tune_id >= default_tunes_اندازه) {
		أرجع Control_پایله::InvalidTune;
	}

	// Accept new tune or a stop?
	إذا (_tune == تش_اشاره ||  	   // No tune is currently being played
	    tune_control.tune_override ||  // Override interrupts everything
	    _default_tunes_interruptable[_current_tune_id]) {

		// Check if this exact tune is already being played back
		إذا (tune_control.tune_id != static_cast<صحيح>(TuneID::CUSTOM) &&
		    _tune == _default_tunes[tune_control.tune_id]) {
			أرجع Control_پایله::AlreadyPlaying; // Nothing to do
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
			using_custom_پیغام = ریښتیا;
			_frequency        = (بینښه)tune_control.frequency;
			_duration         = (بینښه)tune_control.duration;
			_silence          = (بینښه)tune_control.silence;

		} وإلا {
			using_custom_پیغام = خطأ;
			_tune             = _default_tunes[tune_control.tune_id];
			tune_پیل_ptr   = _tune;
			بل_tune        = _tune;
		}

		_current_tune_id = tune_control.tune_id;
		أرجع Control_پایله::Success;
	}

	أرجع Control_پایله::Would_مداخله;
}

فراغ Tunes::سېټ_string(ثابت حرف *ثابت string, uint8_t volume)
{
	// Only play new tune if nothing is being played currently.
	إذا (_tune == تش_اشاره) {
		// Set tune string the first time.
		_tune           = string;
		tune_پیل_ptr = string;
		بل_tune      = _tune;

		إذا (volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = volume;

		} وإلا {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}
	}
}

Tunes::حالت_2 Tunes::get_بل_note(بینښه &frequency, بینښه &duration, بینښه &silence, uint8_t &volume)
{
	إذا (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		volume = 0;
		أرجع Tunes::حالت_2::Stop;
	}

	Tunes::حالت_2 ret = get_بل_note(frequency, duration, silence);

	// Check if note should not be heard -> adjust volume to 0 to be safe.
	إذا (frequency == 0 || duration == 0) {
		volume = 0;

	} وإلا {
		volume = _volume;
	}

	أرجع ret;
}

Tunes::حالت_2 Tunes::get_بل_note(بینښه &frequency, بینښه &duration, بینښه &silence)
{
	إذا (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		أرجع Tunes::حالت_2::Stop;
	}

	// Return the values for frequency and duration if the custom msg was received.
	إذا (using_custom_پیغام) {
		using_custom_پیغام = خطأ;
		duration  = _duration;
		frequency = _frequency;
		silence   = _silence;
		أرجع Tunes::حالت_2::Continue;
	}

	// Make sure we still have a tune.
	إذا ((بل_tune == تش_اشاره) || (_tune == تش_اشاره)) {
		أرجع tune_error();
	}

	// Parse characters out of the string until we have resolved a note.
	بینښه note = 0;
	بینښه note_اوږدتیا_2 = note_اوږدتیا;

	طالما (note == 0) {
		// we always need at least one character from the string.
		صحيح c = بل_char();

		إذا (c == 0) {
			silence = 0;
			أرجع tune_پای_ټکی();
		}

		بل_tune++;

		بدل (c) {

		حالة 'V':	// Select volume.
			// Consume volume if specified in tune but ignore
			// MML compatibility
			بل_شمېره();
			اكسر;

		حالة 'L':	// Select note length.
			note_اوږدتیا = بل_شمېره();

			إذا (note_اوږدتیا < 1) {
				أرجع tune_error();
			}

			اكسر;

		حالة 'O':	// Select octave.
			_octave = بل_شمېره();

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
			c = بل_char();

			إذا (c == 0) {
				أرجع tune_error();
			}

			بل_tune++;

			بدل (c) {
			حالة 'N':
				note_ډېرپېښ = Note_ډېرپېښ::نورمال;
				اكسر;

			حالة 'L':
				note_ډېرپېښ = Note_ډېرپېښ::LEGATO;
				اكسر;

			حالة 'S':
				note_ډېرپېښ = Note_ډېرپېښ::STACCATO;
				اكسر;

			حالة 'F':
				_repeat = خطأ;
				اكسر;

			حالة 'B':
				_repeat = ریښتیا;
				اكسر;

			افتراضي:
				أرجع tune_error();
			}

			اكسر;

		حالة 'P':	// Pause for a note length.
		حالة 'R':
			frequency = 0;
			duration = 0;
			note_اوږدتیا_2 = بل_شمېره();
			silence = rest_duration(note_اوږدتیا_2, بل_dots());
			أرجع Tunes::حالت_2::Continue;

		حالة 'T': {	// Change tempo.
				بینښه nt = بل_شمېره();

				إذا ((nt >= 32) && (nt <= 255)) {
					_tempo = nt;

				} وإلا {
					أرجع tune_error();
				}

				اكسر;
			}

		حالة 'N':	// Play an arbitrary note.
			note = بل_شمېره();

			إذا (note > 84) {
				أرجع tune_error();
			}

			إذا (note == 0) {
				// This is a rest - pause for the current note length.
				silence = rest_duration(note_اوږدتیا, بل_dots());
				أرجع Tunes::حالت_2::Continue;
			}

			اكسر;

		حالة 'A'...'G':	// Play a note in the current octave.
			note = _note_tab[c - 'A'] + (_octave * 12) + 1;
			c = بل_char();

			بدل (c) {
			حالة '#':	// Up a semitone.
			حالة '+':
				إذا (note < 84) {
					note++;
				}

				بل_tune++;
				اكسر;

			حالة '-':	// Down a semitone.
				إذا (note > 1) {
					note--;
				}

				بل_tune++;
				اكسر;

			افتراضي:
				// 0 / No next char here is OK.
				اكسر;
			}

			// Shorthand length notation.
			note_اوږدتیا_2 = بل_شمېره();

			إذا (note_اوږدتیا_2 == 0) {
				note_اوږدتیا_2 = note_اوږدتیا;
			}

			اكسر;

		افتراضي:
			أرجع tune_error();
		}
	}

	// Compute the duration of the note and the following silence (if any).
	duration = note_duration(silence, note_اوږدتیا_2, بل_dots());

	// Compute the note frequency.
	frequency = note_منزل_frequency(note);
	أرجع Tunes::حالت_2::Continue;
}

Tunes::حالت_2 Tunes::tune_پای_ټکی()
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
	إذا (بل_tune != تش_اشاره) {
		PX4_WARN("Tune error at: %s", بل_tune);
	}

	_repeat = خطأ;	// Don't loop on error.
	reset(_repeat);
	أرجع Tunes::حالت_2::Error;
}

uint32_t Tunes::note_منزل_frequency(بینښه note) ثابت
{
	// Compute the frequency (Hz).
	أرجع (بینښه)(880.0f * powf(2.0f, ((صحيح)note - 46) / 12.0f));
}

بینښه Tunes::note_duration(بینښه &silence, بینښه note_اوږدتیا_2, بینښه dots) ثابت
{
	بینښه ټولیز_note_period = BEAT_وخت_CONVERSION / _tempo;

	إذا (note_اوږدتیا_2 == 0) {
		note_اوږدتیا_2 = 1;
	}

	بینښه note_period = ټولیز_note_period / note_اوږدتیا_2;

	بدل (note_ډېرپېښ) {
	حالة Note_ډېرپېښ::نورمال:
		silence = note_period / 8;
		اكسر;

	حالة Note_ډېرپېښ::STACCATO:
		silence = note_period / 4;
		اكسر;

	حالة Note_ډېرپېښ::LEGATO:
	افتراضي:
		silence = 0;
		اكسر;
	}

	note_period -= silence;

	بینښه dot_extension = note_period / 2;

	طالما (dots--) {
		note_period += dot_extension;
		dot_extension /= 2;
	}

	أرجع note_period;
}

بینښه Tunes::rest_duration(بینښه rest_اوږدتیا, بینښه dots) ثابت
{
	بینښه ټولیز_note_period = BEAT_وخت_CONVERSION / _tempo;

	إذا (rest_اوږدتیا == 0) {
		rest_اوږدتیا = 1;
	}

	بینښه rest_period = ټولیز_note_period / rest_اوږدتیا;

	بینښه dot_extension = rest_period / 2;

	طالما (dots--) {
		rest_period += dot_extension;
		dot_extension /= 2;
	}

	أرجع rest_period;
}

صحيح Tunes::بل_char()
{
	طالما (که_تشه(*بل_tune)) {
		بل_tune++;
	}

	أرجع لوی_توری(*بل_tune);
}

بینښه Tunes::بل_شمېره()
{
	بینښه شمېره = 0;
	صحيح بل_character;

	لكل (;;) {
		بل_character = بل_char();

		إذا (!که_عدد(بل_character)) {
			أرجع شمېره;
		}

		بل_tune++;
		شمېره = (شمېره * 10) + (بل_character - '0');
	}
}

بینښه Tunes::بل_dots()
{
	بینښه dots = 0;

	طالما (بل_char() == '.') {
		بل_tune++;
		dots++;
	}

	أرجع dots;
}
