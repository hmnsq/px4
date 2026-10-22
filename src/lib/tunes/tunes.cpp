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

#define BEAT_TIME_CONVERSION_MS (60 * 1000 * 4)
#define BEAT_TIME_CONVERSION_US (BEAT_TIME_CONVERSION_MS * 1000)
#define BEAT_TIME_CONVERSION    BEAT_TIME_CONVERSION_US

// semitone offsets from C for the characters 'A'-'G'
ثابت uint8_t Tunes::_note_tab[] = {9, 11, 0, 2, 4, 5, 7};

Tunes::Tunes(unsigned default_note_length, NoteMode default_note_mode,
	     unsigned default_octave, unsigned default_tempo):
	_default_note_length(default_note_length),
	_default_note_mode(default_note_mode),
	_default_octave(default_octave),
	_default_tempo(default_tempo)
{
	𐡄𐡍 (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_KEY)) {
		_tunes_disabled = 𐡒𐡔𐡈;
	}

	reset(خطأ);
}

فراغ Tunes::reset(منطقي repeat_flag)
{
	// reset pointer
	𐡄𐡍 (!repeat_flag)	{
		_tune = nullptr;
		_next_tune = nullptr;

	} وإلا {
		_tune = _tune_start_ptr;
		_next_tune = _tune;
	}

	// reset music parameter
	_note_length = _default_note_length;
	_note_mode   = _default_note_mode;
	_octave      = _default_octave;
	_tempo       = _default_tempo;
}

Tunes::ControlResult Tunes::set_control(ثابت tune_control_s &tune_control)
{
	// Sanity check
	𐡄𐡍 (tune_control.tune_id >= _default_tunes_size) {
		أرجع ControlResult::InvalidTune;
	}

	// Accept new tune or a stop?
	𐡄𐡍 (_tune == nullptr ||  	   // No tune is currently being played
	    tune_control.tune_override ||  // Override interrupts everything
	    _default_tunes_interruptable[_current_tune_id]) {

		// Check if this exact tune is already being played back
		𐡄𐡍 (tune_control.tune_id != static_cast<صحيح>(TuneID::CUSTOM) &&
		    _tune == _default_tunes[tune_control.tune_id]) {
			أرجع ControlResult::AlreadyPlaying; // Nothing to do
		}

		// Reset repeat flag. Can jump to true again while tune is being parsed later
		_repeat = خطأ;

		// Reset octave, tempo etc.
		reset(_repeat);

		// Volume will remain valid for the entire tune, unless interrupted.
		𐡄𐡍 (tune_control.volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = tune_control.volume;

		} وإلا {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}


		// Special treatment for custom tunes
		𐡄𐡍 (tune_control.tune_id == static_cast<صحيح>(TuneID::CUSTOM)) {
			_using_custom_msg = 𐡒𐡔𐡈;
			_frequency        = (unsigned)tune_control.frequency;
			_duration         = (unsigned)tune_control.duration;
			_silence          = (unsigned)tune_control.silence;

		} وإلا {
			_using_custom_msg = خطأ;
			_tune             = _default_tunes[tune_control.tune_id];
			_tune_start_ptr   = _tune;
			_next_tune        = _tune;
		}

		_current_tune_id = tune_control.tune_id;
		أرجع ControlResult::Success;
	}

	أرجع ControlResult::WouldInterrupt;
}

فراغ Tunes::set_string(ثابت حرف *ثابت string, uint8_t volume)
{
	// Only play new tune if nothing is being played currently.
	𐡄𐡍 (_tune == nullptr) {
		// Set tune string the first time.
		_tune           = string;
		_tune_start_ptr = string;
		_next_tune      = _tune;

		𐡄𐡍 (volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = volume;

		} وإلا {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}
	}
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence, uint8_t &volume)
{
	𐡄𐡍 (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		volume = 0;
		أرجع Tunes::Status::Stop;
	}

	Tunes::Status ret = get_next_note(frequency, duration, silence);

	// Check if note should not be heard -> adjust volume to 0 to be safe.
	𐡄𐡍 (frequency == 0 || duration == 0) {
		volume = 0;

	} وإلا {
		volume = _volume;
	}

	أرجع ret;
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence)
{
	𐡄𐡍 (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		أرجع Tunes::Status::Stop;
	}

	// Return the values for frequency and duration if the custom msg was received.
	𐡄𐡍 (_using_custom_msg) {
		_using_custom_msg = خطأ;
		duration  = _duration;
		frequency = _frequency;
		silence   = _silence;
		أرجع Tunes::Status::Continue;
	}

	// Make sure we still have a tune.
	𐡄𐡍 ((_next_tune == nullptr) || (_tune == nullptr)) {
		أرجع tune_error();
	}

	// Parse characters out of the string until we have resolved a note.
	unsigned note = 0;
	unsigned note_length = _note_length;

	طالما (note == 0) {
		// we always need at least one character from the string.
		صحيح c = next_char();

		𐡄𐡍 (c == 0) {
			silence = 0;
			أرجع tune_end();
		}

		_next_tune++;

		بدل (c) {

		حالة 'V':	// Select volume.
			// Consume volume if specified in tune but ignore
			// MML compatibility
			next_number();
			اكسر;

		حالة 'L':	// Select note length.
			_note_length = next_number();

			𐡄𐡍 (_note_length < 1) {
				أرجع tune_error();
			}

			اكسر;

		حالة 'O':	// Select octave.
			_octave = next_number();

			𐡄𐡍 (_octave > 6) {
				_octave = 6;
			}

			اكسر;

		حالة '<':	// Decrease octave.
			𐡄𐡍 (_octave > 0) {
				_octave--;
			}

			اكسر;

		حالة '>':	// Increase octave.
			𐡄𐡍 (_octave < 6) {
				_octave++;
			}

			اكسر;

		حالة 'M':	// Select inter-note gap.
			c = next_char();

			𐡄𐡍 (c == 0) {
				أرجع tune_error();
			}

			_next_tune++;

			بدل (c) {
			حالة 'N':
				_note_mode = NoteMode::NORMAL;
				اكسر;

			حالة 'L':
				_note_mode = NoteMode::LEGATO;
				اكسر;

			حالة 'S':
				_note_mode = NoteMode::STACCATO;
				اكسر;

			حالة 'F':
				_repeat = خطأ;
				اكسر;

			حالة 'B':
				_repeat = 𐡒𐡔𐡈;
				اكسر;

			افتراضي:
				أرجع tune_error();
			}

			اكسر;

		حالة 'P':	// Pause for a note length.
		حالة 'R':
			frequency = 0;
			duration = 0;
			note_length = next_number();
			silence = rest_duration(note_length, next_dots());
			أرجع Tunes::Status::Continue;

		حالة 'T': {	// Change tempo.
				unsigned nt = next_number();

				𐡄𐡍 ((nt >= 32) && (nt <= 255)) {
					_tempo = nt;

				} وإلا {
					أرجع tune_error();
				}

				اكسر;
			}

		حالة 'N':	// Play an arbitrary note.
			note = next_number();

			𐡄𐡍 (note > 84) {
				أرجع tune_error();
			}

			𐡄𐡍 (note == 0) {
				// This is a rest - pause for the current note length.
				silence = rest_duration(_note_length, next_dots());
				أرجع Tunes::Status::Continue;
			}

			اكسر;

		حالة 'A'...'G':	// Play a note in the current octave.
			note = _note_tab[c - 'A'] + (_octave * 12) + 1;
			c = next_char();

			بدل (c) {
			حالة '#':	// Up a semitone.
			حالة '+':
				𐡄𐡍 (note < 84) {
					note++;
				}

				_next_tune++;
				اكسر;

			حالة '-':	// Down a semitone.
				𐡄𐡍 (note > 1) {
					note--;
				}

				_next_tune++;
				اكسر;

			افتراضي:
				// 0 / No next char here is OK.
				اكسر;
			}

			// Shorthand length notation.
			note_length = next_number();

			𐡄𐡍 (note_length == 0) {
				note_length = _note_length;
			}

			اكسر;

		افتراضي:
			أرجع tune_error();
		}
	}

	// Compute the duration of the note and the following silence (if any).
	duration = note_duration(silence, note_length, next_dots());

	// Compute the note frequency.
	frequency = note_to_frequency(note);
	أرجع Tunes::Status::Continue;
}

Tunes::Status Tunes::tune_end()
{
	𐡄𐡍 (_tunes_disabled) {
		أرجع Tunes::Status::Stop;
	}

	// Restore intial parameters.
	reset(_repeat);

	// Stop or restart the tune.
	𐡄𐡍 (_repeat) {
		أرجع Tunes::Status::Continue;

	} وإلا {
		أرجع Tunes::Status::Stop;
	}
}

Tunes::Status Tunes::tune_error()
{
	𐡄𐡍 (_tunes_disabled) {
		أرجع Tunes::Status::Stop;
	}

	// The tune appears to be bad (unexpected EOF, bad character, etc.).
	𐡄𐡍 (_next_tune != nullptr) {
		PX4_WARN("Tune error at: %s", _next_tune);
	}

	_repeat = خطأ;	// Don't loop on error.
	reset(_repeat);
	أرجع Tunes::Status::Error;
}

uint32_t Tunes::note_to_frequency(unsigned note) ثابت
{
	// Compute the frequency (Hz).
	أرجع (unsigned)(880.0f * powf(2.0f, ((صحيح)note - 46) / 12.0f));
}

unsigned Tunes::note_duration(unsigned &silence, unsigned note_length, unsigned dots) ثابت
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	𐡄𐡍 (note_length == 0) {
		note_length = 1;
	}

	unsigned note_period = whole_note_period / note_length;

	بدل (_note_mode) {
	حالة NoteMode::NORMAL:
		silence = note_period / 8;
		اكسر;

	حالة NoteMode::STACCATO:
		silence = note_period / 4;
		اكسر;

	حالة NoteMode::LEGATO:
	افتراضي:
		silence = 0;
		اكسر;
	}

	note_period -= silence;

	unsigned dot_extension = note_period / 2;

	طالما (dots--) {
		note_period += dot_extension;
		dot_extension /= 2;
	}

	أرجع note_period;
}

unsigned Tunes::rest_duration(unsigned rest_length, unsigned dots) ثابت
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	𐡄𐡍 (rest_length == 0) {
		rest_length = 1;
	}

	unsigned rest_period = whole_note_period / rest_length;

	unsigned dot_extension = rest_period / 2;

	طالما (dots--) {
		rest_period += dot_extension;
		dot_extension /= 2;
	}

	أرجع rest_period;
}

صحيح Tunes::next_char()
{
	طالما (isspace(*_next_tune)) {
		_next_tune++;
	}

	أرجع toupper(*_next_tune);
}

unsigned Tunes::next_number()
{
	unsigned number = 0;
	صحيح next_character;

	لكل (;;) {
		next_character = next_char();

		𐡄𐡍 (!isdigit(next_character)) {
			أرجع number;
		}

		_next_tune++;
		number = (number * 10) + (next_character - '0');
	}
}

unsigned Tunes::next_dots()
{
	unsigned dots = 0;

	طالما (next_char() == '.') {
		_next_tune++;
		dots++;
	}

	أرجع dots;
}
