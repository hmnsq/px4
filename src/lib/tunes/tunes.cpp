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

#include "tunes.h"

#include <px4_platform_common/log.h>

#include <lib/circuit_breaker/circuit_breaker.h>

#include <math.h>
#include <ctype.h>
#include <errno.h>

#define BEAT_TIME_CONVERSION_MS (60 * 1000 * 4)
#define BEAT_TIME_CONVERSION_US (BEAT_TIME_CONVERSION_MS * 1000)
#define BEAT_TIME_CONVERSION    BEAT_TIME_CONVERSION_US

// semitone offsets from C for the characters 'A'-'G'
const uint8_t Tunes::_note_tab[] = {9, 11, 0, 2, 4, 5, 7};

Tunes::Tunes(unsigned default_note_length, NoteMode default_note_mode,
	     unsigned default_octave, unsigned default_tempo):
	_default_note_length(default_note_length),
	_default_note_mode(default_note_mode),
	_default_octave(default_octave),
	_default_tempo(default_tempo)
{
	nếu (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_KEY)) {
		_tunes_disabled = true;
	}

	reset(false);
}

void Tunes::reset(bool repeat_flag)
{
	// reset pointer
	nếu (!repeat_flag)	{
		_tune = nullptr;
		_next_tune = nullptr;

	} không_thì {
		_tune = _tune_start_ptr;
		_next_tune = _tune;
	}

	// reset music parameter
	_note_length = _default_note_length;
	_note_mode   = _default_note_mode;
	_octave      = _default_octave;
	_tempo       = _default_tempo;
}

Tunes::ControlResult Tunes::set_control(const tune_control_s &tune_control)
{
	// Sanity check
	nếu (tune_control.tune_id >= _default_tunes_size) {
		trả_về ControlResult::InvalidTune;
	}

	// Accept new tune or a stop?
	nếu (_tune == nullptr ||  	   // No tune is currently being played
	    tune_control.tune_override ||  // Override interrupts everything
	    _default_tunes_interruptable[_current_tune_id]) {

		// Check if this exact tune is already being played back
		nếu (tune_control.tune_id != static_cast<số_nguyên>(TuneID::CUSTOM) &&
		    _tune == _default_tunes[tune_control.tune_id]) {
			trả_về ControlResult::AlreadyPlaying; // Nothing to do
		}

		// Reset repeat flag. Can jump to true again while tune is being parsed later
		_repeat = false;

		// Reset octave, tempo etc.
		reset(_repeat);

		// Volume will remain valid for the entire tune, unless interrupted.
		nếu (tune_control.volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = tune_control.volume;

		} không_thì {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}


		// Special treatment for custom tunes
		nếu (tune_control.tune_id == static_cast<số_nguyên>(TuneID::CUSTOM)) {
			_using_custom_msg = true;
			_frequency        = (unsigned)tune_control.frequency;
			_duration         = (unsigned)tune_control.duration;
			_silence          = (unsigned)tune_control.silence;

		} không_thì {
			_using_custom_msg = false;
			_tune             = _default_tunes[tune_control.tune_id];
			_tune_start_ptr   = _tune;
			_next_tune        = _tune;
		}

		_current_tune_id = tune_control.tune_id;
		trả_về ControlResult::Success;
	}

	trả_về ControlResult::WouldInterrupt;
}

void Tunes::set_string(const char *const string, uint8_t volume)
{
	// Only play new tune if nothing is being played currently.
	nếu (_tune == nullptr) {
		// Set tune string the first time.
		_tune           = string;
		_tune_start_ptr = string;
		_next_tune      = _tune;

		nếu (volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = volume;

		} không_thì {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}
	}
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence, uint8_t &volume)
{
	nếu (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		volume = 0;
		trả_về Tunes::Status::Stop;
	}

	Tunes::Status ret = get_next_note(frequency, duration, silence);

	// Check if note should not be heard -> adjust volume to 0 to be safe.
	nếu (frequency == 0 || duration == 0) {
		volume = 0;

	} không_thì {
		volume = _volume;
	}

	trả_về ret;
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence)
{
	nếu (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		trả_về Tunes::Status::Stop;
	}

	// Return the values for frequency and duration if the custom msg was received.
	nếu (_using_custom_msg) {
		_using_custom_msg = false;
		duration  = _duration;
		frequency = _frequency;
		silence   = _silence;
		trả_về Tunes::Status::Continue;
	}

	// Make sure we still have a tune.
	nếu ((_next_tune == nullptr) || (_tune == nullptr)) {
		trả_về tune_error();
	}

	// Parse characters out of the string until we have resolved a note.
	unsigned note = 0;
	unsigned note_length = _note_length;

	trong_khi (note == 0) {
		// we always need at least one character from the string.
		số_nguyên c = next_char();

		nếu (c == 0) {
			silence = 0;
			trả_về tune_end();
		}

		_next_tune++;

		switch (c) {

		case 'V':	// Select volume.
			// Consume volume if specified in tune but ignore
			// MML compatibility
			next_number();
			break;

		case 'L':	// Select note length.
			_note_length = next_number();

			nếu (_note_length < 1) {
				trả_về tune_error();
			}

			break;

		case 'O':	// Select octave.
			_octave = next_number();

			nếu (_octave > 6) {
				_octave = 6;
			}

			break;

		case '<':	// Decrease octave.
			nếu (_octave > 0) {
				_octave--;
			}

			break;

		case '>':	// Increase octave.
			nếu (_octave < 6) {
				_octave++;
			}

			break;

		case 'M':	// Select inter-note gap.
			c = next_char();

			nếu (c == 0) {
				trả_về tune_error();
			}

			_next_tune++;

			switch (c) {
			case 'N':
				_note_mode = NoteMode::NORMAL;
				break;

			case 'L':
				_note_mode = NoteMode::LEGATO;
				break;

			case 'S':
				_note_mode = NoteMode::STACCATO;
				break;

			case 'F':
				_repeat = false;
				break;

			case 'B':
				_repeat = true;
				break;

			default:
				trả_về tune_error();
			}

			break;

		case 'P':	// Pause for a note length.
		case 'R':
			frequency = 0;
			duration = 0;
			note_length = next_number();
			silence = rest_duration(note_length, next_dots());
			trả_về Tunes::Status::Continue;

		case 'T': {	// Change tempo.
				unsigned nt = next_number();

				nếu ((nt >= 32) && (nt <= 255)) {
					_tempo = nt;

				} không_thì {
					trả_về tune_error();
				}

				break;
			}

		case 'N':	// Play an arbitrary note.
			note = next_number();

			nếu (note > 84) {
				trả_về tune_error();
			}

			nếu (note == 0) {
				// This is a rest - pause for the current note length.
				silence = rest_duration(_note_length, next_dots());
				trả_về Tunes::Status::Continue;
			}

			break;

		case 'A'...'G':	// Play a note in the current octave.
			note = _note_tab[c - 'A'] + (_octave * 12) + 1;
			c = next_char();

			switch (c) {
			case '#':	// Up a semitone.
			case '+':
				nếu (note < 84) {
					note++;
				}

				_next_tune++;
				break;

			case '-':	// Down a semitone.
				nếu (note > 1) {
					note--;
				}

				_next_tune++;
				break;

			default:
				// 0 / No next char here is OK.
				break;
			}

			// Shorthand length notation.
			note_length = next_number();

			nếu (note_length == 0) {
				note_length = _note_length;
			}

			break;

		default:
			trả_về tune_error();
		}
	}

	// Compute the duration of the note and the following silence (if any).
	duration = note_duration(silence, note_length, next_dots());

	// Compute the note frequency.
	frequency = note_to_frequency(note);
	trả_về Tunes::Status::Continue;
}

Tunes::Status Tunes::tune_end()
{
	nếu (_tunes_disabled) {
		trả_về Tunes::Status::Stop;
	}

	// Restore intial parameters.
	reset(_repeat);

	// Stop or restart the tune.
	nếu (_repeat) {
		trả_về Tunes::Status::Continue;

	} không_thì {
		trả_về Tunes::Status::Stop;
	}
}

Tunes::Status Tunes::tune_error()
{
	nếu (_tunes_disabled) {
		trả_về Tunes::Status::Stop;
	}

	// The tune appears to be bad (unexpected EOF, bad character, etc.).
	nếu (_next_tune != nullptr) {
		PX4_WARN("Tune error at: %s", _next_tune);
	}

	_repeat = false;	// Don't loop on error.
	reset(_repeat);
	trả_về Tunes::Status::Error;
}

uint32_t Tunes::note_to_frequency(unsigned note) const
{
	// Compute the frequency (Hz).
	trả_về (unsigned)(880.0f * powf(2.0f, ((số_nguyên)note - 46) / 12.0f));
}

unsigned Tunes::note_duration(unsigned &silence, unsigned note_length, unsigned dots) const
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	nếu (note_length == 0) {
		note_length = 1;
	}

	unsigned note_period = whole_note_period / note_length;

	switch (_note_mode) {
	case NoteMode::NORMAL:
		silence = note_period / 8;
		break;

	case NoteMode::STACCATO:
		silence = note_period / 4;
		break;

	case NoteMode::LEGATO:
	default:
		silence = 0;
		break;
	}

	note_period -= silence;

	unsigned dot_extension = note_period / 2;

	trong_khi (dots--) {
		note_period += dot_extension;
		dot_extension /= 2;
	}

	trả_về note_period;
}

unsigned Tunes::rest_duration(unsigned rest_length, unsigned dots) const
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	nếu (rest_length == 0) {
		rest_length = 1;
	}

	unsigned rest_period = whole_note_period / rest_length;

	unsigned dot_extension = rest_period / 2;

	trong_khi (dots--) {
		rest_period += dot_extension;
		dot_extension /= 2;
	}

	trả_về rest_period;
}

số_nguyên Tunes::next_char()
{
	trong_khi (isspace(*_next_tune)) {
		_next_tune++;
	}

	trả_về toupper(*_next_tune);
}

unsigned Tunes::next_number()
{
	unsigned number = 0;
	số_nguyên next_character;

	cho (;;) {
		next_character = next_char();

		nếu (!isdigit(next_character)) {
			trả_về number;
		}

		_next_tune++;
		number = (number * 10) + (next_character - '0');
	}
}

unsigned Tunes::next_dots()
{
	unsigned dots = 0;

	trong_khi (next_char() == '.') {
		_next_tune++;
		dots++;
	}

	trả_về dots;
}
