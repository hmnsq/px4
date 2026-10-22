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
	ако (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_KEY)) {
		_tunes_disabled = true;
	}

	reset(false);
}

void Tunes::reset(bool repeat_flag)
{
	// reset pointer
	ако (!repeat_flag)	{
		_tune = nullptr;
		_next_tune = nullptr;

	} инако {
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
	ако (tune_control.tune_id >= _default_tunes_size) {
		врати ControlResult::InvalidTune;
	}

	// Accept new tune or a stop?
	ако (_tune == nullptr ||  	   // No tune is currently being played
	    tune_control.tune_override ||  // Override interrupts everything
	    _default_tunes_interruptable[_current_tune_id]) {

		// Check if this exact tune is already being played back
		ако (tune_control.tune_id != static_cast<цео>(TuneID::CUSTOM) &&
		    _tune == _default_tunes[tune_control.tune_id]) {
			врати ControlResult::AlreadyPlaying; // Nothing to do
		}

		// Reset repeat flag. Can jump to true again while tune is being parsed later
		_repeat = false;

		// Reset octave, tempo etc.
		reset(_repeat);

		// Volume will remain valid for the entire tune, unless interrupted.
		ако (tune_control.volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = tune_control.volume;

		} инако {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}


		// Special treatment for custom tunes
		ако (tune_control.tune_id == static_cast<цео>(TuneID::CUSTOM)) {
			_using_custom_msg = true;
			_frequency        = (unsigned)tune_control.frequency;
			_duration         = (unsigned)tune_control.duration;
			_silence          = (unsigned)tune_control.silence;

		} инако {
			_using_custom_msg = false;
			_tune             = _default_tunes[tune_control.tune_id];
			_tune_start_ptr   = _tune;
			_next_tune        = _tune;
		}

		_current_tune_id = tune_control.tune_id;
		врати ControlResult::Success;
	}

	врати ControlResult::WouldInterrupt;
}

void Tunes::set_string(const char *const string, uint8_t volume)
{
	// Only play new tune if nothing is being played currently.
	ако (_tune == nullptr) {
		// Set tune string the first time.
		_tune           = string;
		_tune_start_ptr = string;
		_next_tune      = _tune;

		ако (volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = volume;

		} инако {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}
	}
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence, uint8_t &volume)
{
	ако (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		volume = 0;
		врати Tunes::Status::Stop;
	}

	Tunes::Status ret = get_next_note(frequency, duration, silence);

	// Check if note should not be heard -> adjust volume to 0 to be safe.
	ако (frequency == 0 || duration == 0) {
		volume = 0;

	} инако {
		volume = _volume;
	}

	врати ret;
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence)
{
	ако (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		врати Tunes::Status::Stop;
	}

	// Return the values for frequency and duration if the custom msg was received.
	ако (_using_custom_msg) {
		_using_custom_msg = false;
		duration  = _duration;
		frequency = _frequency;
		silence   = _silence;
		врати Tunes::Status::Continue;
	}

	// Make sure we still have a tune.
	ако ((_next_tune == nullptr) || (_tune == nullptr)) {
		врати tune_error();
	}

	// Parse characters out of the string until we have resolved a note.
	unsigned note = 0;
	unsigned note_length = _note_length;

	док (note == 0) {
		// we always need at least one character from the string.
		цео c = next_char();

		ако (c == 0) {
			silence = 0;
			врати tune_end();
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

			ако (_note_length < 1) {
				врати tune_error();
			}

			break;

		case 'O':	// Select octave.
			_octave = next_number();

			ако (_octave > 6) {
				_octave = 6;
			}

			break;

		case '<':	// Decrease octave.
			ако (_octave > 0) {
				_octave--;
			}

			break;

		case '>':	// Increase octave.
			ако (_octave < 6) {
				_octave++;
			}

			break;

		case 'M':	// Select inter-note gap.
			c = next_char();

			ако (c == 0) {
				врати tune_error();
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
				врати tune_error();
			}

			break;

		case 'P':	// Pause for a note length.
		case 'R':
			frequency = 0;
			duration = 0;
			note_length = next_number();
			silence = rest_duration(note_length, next_dots());
			врати Tunes::Status::Continue;

		case 'T': {	// Change tempo.
				unsigned nt = next_number();

				ако ((nt >= 32) && (nt <= 255)) {
					_tempo = nt;

				} инако {
					врати tune_error();
				}

				break;
			}

		case 'N':	// Play an arbitrary note.
			note = next_number();

			ако (note > 84) {
				врати tune_error();
			}

			ако (note == 0) {
				// This is a rest - pause for the current note length.
				silence = rest_duration(_note_length, next_dots());
				врати Tunes::Status::Continue;
			}

			break;

		case 'A'...'G':	// Play a note in the current octave.
			note = _note_tab[c - 'A'] + (_octave * 12) + 1;
			c = next_char();

			switch (c) {
			case '#':	// Up a semitone.
			case '+':
				ако (note < 84) {
					note++;
				}

				_next_tune++;
				break;

			case '-':	// Down a semitone.
				ако (note > 1) {
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

			ако (note_length == 0) {
				note_length = _note_length;
			}

			break;

		default:
			врати tune_error();
		}
	}

	// Compute the duration of the note and the following silence (if any).
	duration = note_duration(silence, note_length, next_dots());

	// Compute the note frequency.
	frequency = note_to_frequency(note);
	врати Tunes::Status::Continue;
}

Tunes::Status Tunes::tune_end()
{
	ако (_tunes_disabled) {
		врати Tunes::Status::Stop;
	}

	// Restore intial parameters.
	reset(_repeat);

	// Stop or restart the tune.
	ако (_repeat) {
		врати Tunes::Status::Continue;

	} инако {
		врати Tunes::Status::Stop;
	}
}

Tunes::Status Tunes::tune_error()
{
	ако (_tunes_disabled) {
		врати Tunes::Status::Stop;
	}

	// The tune appears to be bad (unexpected EOF, bad character, etc.).
	ако (_next_tune != nullptr) {
		PX4_WARN("Tune error at: %s", _next_tune);
	}

	_repeat = false;	// Don't loop on error.
	reset(_repeat);
	врати Tunes::Status::Error;
}

uint32_t Tunes::note_to_frequency(unsigned note) const
{
	// Compute the frequency (Hz).
	врати (unsigned)(880.0f * powf(2.0f, ((цео)note - 46) / 12.0f));
}

unsigned Tunes::note_duration(unsigned &silence, unsigned note_length, unsigned dots) const
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	ако (note_length == 0) {
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

	док (dots--) {
		note_period += dot_extension;
		dot_extension /= 2;
	}

	врати note_period;
}

unsigned Tunes::rest_duration(unsigned rest_length, unsigned dots) const
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	ако (rest_length == 0) {
		rest_length = 1;
	}

	unsigned rest_period = whole_note_period / rest_length;

	unsigned dot_extension = rest_period / 2;

	док (dots--) {
		rest_period += dot_extension;
		dot_extension /= 2;
	}

	врати rest_period;
}

цео Tunes::next_char()
{
	док (isspace(*_next_tune)) {
		_next_tune++;
	}

	врати toupper(*_next_tune);
}

unsigned Tunes::next_number()
{
	unsigned number = 0;
	цео next_character;

	за (;;) {
		next_character = next_char();

		ако (!isdigit(next_character)) {
			врати number;
		}

		_next_tune++;
		number = (number * 10) + (next_character - '0');
	}
}

unsigned Tunes::next_dots()
{
	unsigned dots = 0;

	док (next_char() == '.') {
		_next_tune++;
		dots++;
	}

	врати dots;
}
