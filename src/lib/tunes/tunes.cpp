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
	اگر (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_کلید)) {
		_tunes_disabled = درست;
	}

	reset(نادرست);
}

تهی Tunes::reset(منطقی repeat_flag)
{
	// reset pointer
	اگر (!repeat_flag)	{
		_tune = nullptr;
		_next_tune = nullptr;

	} وگرنه {
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
	اگر (tune_control.tune_id >= _default_tunes_size) {
		بازگشت ControlResult::InvalidTune;
	}

	// Accept new tune or a stop?
	اگر (_tune == nullptr ||  	   // No tune is currently being played
	    tune_control.tune_override ||  // Override interrupts everything
	    _default_tunes_interruptable[_current_tune_id]) {

		// Check if this exact tune is already being played back
		اگر (tune_control.tune_id != static_cast<صحیح>(TuneID::CUSTOM) &&
		    _tune == _default_tunes[tune_control.tune_id]) {
			بازگشت ControlResult::AlreadyPlaying; // Nothing to do
		}

		// Reset repeat flag. Can jump to true again while tune is being parsed later
		_repeat = نادرست;

		// Reset octave, tempo etc.
		reset(_repeat);

		// Volume will remain valid for the entire tune, unless interrupted.
		اگر (tune_control.volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = tune_control.volume;

		} وگرنه {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}


		// Special treatment for custom tunes
		اگر (tune_control.tune_id == static_cast<صحیح>(TuneID::CUSTOM)) {
			using_custom_پیام = درست;
			_frequency        = (unsigned)tune_control.frequency;
			_duration         = (unsigned)tune_control.duration;
			_silence          = (unsigned)tune_control.silence;

		} وگرنه {
			using_custom_پیام = نادرست;
			_tune             = _default_tunes[tune_control.tune_id];
			_tune_start_ptr   = _tune;
			_next_tune        = _tune;
		}

		_current_tune_id = tune_control.tune_id;
		بازگشت ControlResult::Success;
	}

	بازگشت ControlResult::WouldInterrupt;
}

تهی Tunes::set_string(ثابت نویسه *ثابت string, uint8_t volume)
{
	// Only play new tune if nothing is being played currently.
	اگر (_tune == nullptr) {
		// Set tune string the first time.
		_tune           = string;
		_tune_start_ptr = string;
		_next_tune      = _tune;

		اگر (volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = volume;

		} وگرنه {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}
	}
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence, uint8_t &volume)
{
	اگر (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		volume = 0;
		بازگشت Tunes::Status::Stop;
	}

	Tunes::Status ret = get_next_note(frequency, duration, silence);

	// Check if note should not be heard -> adjust volume to 0 to be safe.
	اگر (frequency == 0 || duration == 0) {
		volume = 0;

	} وگرنه {
		volume = _volume;
	}

	بازگشت ret;
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence)
{
	اگر (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		بازگشت Tunes::Status::Stop;
	}

	// Return the values for frequency and duration if the custom msg was received.
	اگر (using_custom_پیام) {
		using_custom_پیام = نادرست;
		duration  = _duration;
		frequency = _frequency;
		silence   = _silence;
		بازگشت Tunes::Status::Continue;
	}

	// Make sure we still have a tune.
	اگر ((_next_tune == nullptr) || (_tune == nullptr)) {
		بازگشت tune_error();
	}

	// Parse characters out of the string until we have resolved a note.
	unsigned note = 0;
	unsigned note_length = _note_length;

	وقتی (note == 0) {
		// we always need at least one character from the string.
		صحیح c = next_char();

		اگر (c == 0) {
			silence = 0;
			بازگشت tune_end();
		}

		_next_tune++;

		انتخاب (c) {

		حالت 'V':	// Select volume.
			// Consume volume if specified in tune but ignore
			// MML compatibility
			next_عدد();
			بشکن;

		حالت 'L':	// Select note length.
			_note_length = next_عدد();

			اگر (_note_length < 1) {
				بازگشت tune_error();
			}

			بشکن;

		حالت 'O':	// Select octave.
			_octave = next_عدد();

			اگر (_octave > 6) {
				_octave = 6;
			}

			بشکن;

		حالت '<':	// Decrease octave.
			اگر (_octave > 0) {
				_octave--;
			}

			بشکن;

		حالت '>':	// Increase octave.
			اگر (_octave < 6) {
				_octave++;
			}

			بشکن;

		حالت 'M':	// Select inter-note gap.
			c = next_char();

			اگر (c == 0) {
				بازگشت tune_error();
			}

			_next_tune++;

			انتخاب (c) {
			حالت 'N':
				_note_mode = NoteMode::NORMAL;
				بشکن;

			حالت 'L':
				_note_mode = NoteMode::LEGATO;
				بشکن;

			حالت 'S':
				_note_mode = NoteMode::STACCATO;
				بشکن;

			حالت 'F':
				_repeat = نادرست;
				بشکن;

			حالت 'B':
				_repeat = درست;
				بشکن;

			پیشفرض:
				بازگشت tune_error();
			}

			بشکن;

		حالت 'P':	// Pause for a note length.
		حالت 'R':
			frequency = 0;
			duration = 0;
			note_length = next_عدد();
			silence = rest_duration(note_length, next_dots());
			بازگشت Tunes::Status::Continue;

		حالت 'T': {	// Change tempo.
				unsigned nt = next_عدد();

				اگر ((nt >= 32) && (nt <= 255)) {
					_tempo = nt;

				} وگرنه {
					بازگشت tune_error();
				}

				بشکن;
			}

		حالت 'N':	// Play an arbitrary note.
			note = next_عدد();

			اگر (note > 84) {
				بازگشت tune_error();
			}

			اگر (note == 0) {
				// This is a rest - pause for the current note length.
				silence = rest_duration(_note_length, next_dots());
				بازگشت Tunes::Status::Continue;
			}

			بشکن;

		حالت 'A'...'G':	// Play a note in the current octave.
			note = _note_tab[c - 'A'] + (_octave * 12) + 1;
			c = next_char();

			انتخاب (c) {
			حالت '#':	// Up a semitone.
			حالت '+':
				اگر (note < 84) {
					note++;
				}

				_next_tune++;
				بشکن;

			حالت '-':	// Down a semitone.
				اگر (note > 1) {
					note--;
				}

				_next_tune++;
				بشکن;

			پیشفرض:
				// 0 / No next char here is OK.
				بشکن;
			}

			// Shorthand length notation.
			note_length = next_عدد();

			اگر (note_length == 0) {
				note_length = _note_length;
			}

			بشکن;

		پیشفرض:
			بازگشت tune_error();
		}
	}

	// Compute the duration of the note and the following silence (if any).
	duration = note_duration(silence, note_length, next_dots());

	// Compute the note frequency.
	frequency = note_to_frequency(note);
	بازگشت Tunes::Status::Continue;
}

Tunes::Status Tunes::tune_end()
{
	اگر (_tunes_disabled) {
		بازگشت Tunes::Status::Stop;
	}

	// Restore intial parameters.
	reset(_repeat);

	// Stop or restart the tune.
	اگر (_repeat) {
		بازگشت Tunes::Status::Continue;

	} وگرنه {
		بازگشت Tunes::Status::Stop;
	}
}

Tunes::Status Tunes::tune_error()
{
	اگر (_tunes_disabled) {
		بازگشت Tunes::Status::Stop;
	}

	// The tune appears to be bad (unexpected EOF, bad character, etc.).
	اگر (_next_tune != nullptr) {
		PX4_WARN("Tune error at: %s", _next_tune);
	}

	_repeat = نادرست;	// Don't loop on error.
	reset(_repeat);
	بازگشت Tunes::Status::Error;
}

uint32_t Tunes::note_to_frequency(unsigned note) ثابت
{
	// Compute the frequency (Hz).
	بازگشت (unsigned)(880.0f * powf(2.0f, ((صحیح)note - 46) / 12.0f));
}

unsigned Tunes::note_duration(unsigned &silence, unsigned note_length, unsigned dots) ثابت
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	اگر (note_length == 0) {
		note_length = 1;
	}

	unsigned note_period = whole_note_period / note_length;

	انتخاب (_note_mode) {
	حالت NoteMode::NORMAL:
		silence = note_period / 8;
		بشکن;

	حالت NoteMode::STACCATO:
		silence = note_period / 4;
		بشکن;

	حالت NoteMode::LEGATO:
	پیشفرض:
		silence = 0;
		بشکن;
	}

	note_period -= silence;

	unsigned dot_extension = note_period / 2;

	وقتی (dots--) {
		note_period += dot_extension;
		dot_extension /= 2;
	}

	بازگشت note_period;
}

unsigned Tunes::rest_duration(unsigned rest_length, unsigned dots) ثابت
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	اگر (rest_length == 0) {
		rest_length = 1;
	}

	unsigned rest_period = whole_note_period / rest_length;

	unsigned dot_extension = rest_period / 2;

	وقتی (dots--) {
		rest_period += dot_extension;
		dot_extension /= 2;
	}

	بازگشت rest_period;
}

صحیح Tunes::next_char()
{
	وقتی (isspace(*_next_tune)) {
		_next_tune++;
	}

	بازگشت toupper(*_next_tune);
}

unsigned Tunes::next_عدد()
{
	unsigned عدد = 0;
	صحیح next_character;

	برای (;;) {
		next_character = next_char();

		اگر (!isdigit(next_character)) {
			بازگشت عدد;
		}

		_next_tune++;
		عدد = (عدد * 10) + (next_character - '0');
	}
}

unsigned Tunes::next_dots()
{
	unsigned dots = 0;

	وقتی (next_char() == '.') {
		_next_tune++;
		dots++;
	}

	بازگشت dots;
}
