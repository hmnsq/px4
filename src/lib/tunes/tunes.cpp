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
	ຖ້າ (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_KEY)) {
		_tunes_disabled = true;
	}

	reset(false);
}

void Tunes::reset(bool repeat_flag)
{
	// reset pointer
	ຖ້າ (!repeat_flag)	{
		_tune = nullptr;
		_next_tune = nullptr;

	} ບໍ່ດັ່ງນັ້ນ {
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
	ຖ້າ (tune_control.tune_id >= _default_tunes_size) {
		ສົ່ງຄືນ ControlResult::InvalidTune;
	}

	// Accept new tune or a stop?
	ຖ້າ (_tune == nullptr ||  	   // No tune is currently being played
	    tune_control.tune_override ||  // Override interrupts everything
	    _default_tunes_interruptable[_current_tune_id]) {

		// Check if this exact tune is already being played back
		ຖ້າ (tune_control.tune_id != static_cast<ຈຳນວນເຕັມ>(TuneID::CUSTOM) &&
		    _tune == _default_tunes[tune_control.tune_id]) {
			ສົ່ງຄືນ ControlResult::AlreadyPlaying; // Nothing to do
		}

		// Reset repeat flag. Can jump to true again while tune is being parsed later
		_repeat = false;

		// Reset octave, tempo etc.
		reset(_repeat);

		// Volume will remain valid for the entire tune, unless interrupted.
		ຖ້າ (tune_control.volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = tune_control.volume;

		} ບໍ່ດັ່ງນັ້ນ {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}


		// Special treatment for custom tunes
		ຖ້າ (tune_control.tune_id == static_cast<ຈຳນວນເຕັມ>(TuneID::CUSTOM)) {
			_using_custom_msg = true;
			_frequency        = (unsigned)tune_control.frequency;
			_duration         = (unsigned)tune_control.duration;
			_silence          = (unsigned)tune_control.silence;

		} ບໍ່ດັ່ງນັ້ນ {
			_using_custom_msg = false;
			_tune             = _default_tunes[tune_control.tune_id];
			_tune_start_ptr   = _tune;
			_next_tune        = _tune;
		}

		_current_tune_id = tune_control.tune_id;
		ສົ່ງຄືນ ControlResult::Success;
	}

	ສົ່ງຄືນ ControlResult::WouldInterrupt;
}

void Tunes::set_string(const var_na *const string, uint8_t volume)
{
	// Only play new tune if nothing is being played currently.
	ຖ້າ (_tune == nullptr) {
		// Set tune string the first time.
		_tune           = string;
		_tune_start_ptr = string;
		_next_tune      = _tune;

		ຖ້າ (volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = volume;

		} ບໍ່ດັ່ງນັ້ນ {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}
	}
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence, uint8_t &volume)
{
	ຖ້າ (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		volume = 0;
		ສົ່ງຄືນ Tunes::Status::Stop;
	}

	Tunes::Status ret = get_next_note(frequency, duration, silence);

	// Check if note should not be heard -> adjust volume to 0 to be safe.
	ຖ້າ (frequency == 0 || duration == 0) {
		volume = 0;

	} ບໍ່ດັ່ງນັ້ນ {
		volume = _volume;
	}

	ສົ່ງຄືນ ret;
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence)
{
	ຖ້າ (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		ສົ່ງຄືນ Tunes::Status::Stop;
	}

	// Return the values for frequency and duration if the custom msg was received.
	ຖ້າ (_using_custom_msg) {
		_using_custom_msg = false;
		duration  = _duration;
		frequency = _frequency;
		silence   = _silence;
		ສົ່ງຄືນ Tunes::Status::Continue;
	}

	// Make sure we still have a tune.
	ຖ້າ ((_next_tune == nullptr) || (_tune == nullptr)) {
		ສົ່ງຄືນ tune_error();
	}

	// Parse characters out of the string until we have resolved a note.
	unsigned note = 0;
	unsigned note_length = _note_length;

	ໃນຂະນະທີ່ (note == 0) {
		// we always need at least one character from the string.
		ຈຳນວນເຕັມ c = next_char();

		ຖ້າ (c == 0) {
			silence = 0;
			ສົ່ງຄືນ tune_end();
		}

		_next_tune++;

		switch (c) {

		pak_sa 'V':	// Select volume.
			// Consume volume if specified in tune but ignore
			// MML compatibility
			next_number();
			bha_nga;

		pak_sa 'L':	// Select note length.
			_note_length = next_number();

			ຖ້າ (_note_length < 1) {
				ສົ່ງຄືນ tune_error();
			}

			bha_nga;

		pak_sa 'O':	// Select octave.
			_octave = next_number();

			ຖ້າ (_octave > 6) {
				_octave = 6;
			}

			bha_nga;

		pak_sa '<':	// Decrease octave.
			ຖ້າ (_octave > 0) {
				_octave--;
			}

			bha_nga;

		pak_sa '>':	// Increase octave.
			ຖ້າ (_octave < 6) {
				_octave++;
			}

			bha_nga;

		pak_sa 'M':	// Select inter-note gap.
			c = next_char();

			ຖ້າ (c == 0) {
				ສົ່ງຄືນ tune_error();
			}

			_next_tune++;

			switch (c) {
			pak_sa 'N':
				_note_mode = NoteMode::NORMAL;
				bha_nga;

			pak_sa 'L':
				_note_mode = NoteMode::LEGATO;
				bha_nga;

			pak_sa 'S':
				_note_mode = NoteMode::STACCATO;
				bha_nga;

			pak_sa 'F':
				_repeat = false;
				bha_nga;

			pak_sa 'B':
				_repeat = true;
				bha_nga;

			default:
				ສົ່ງຄືນ tune_error();
			}

			bha_nga;

		pak_sa 'P':	// Pause for a note length.
		pak_sa 'R':
			frequency = 0;
			duration = 0;
			note_length = next_number();
			silence = rest_duration(note_length, next_dots());
			ສົ່ງຄືນ Tunes::Status::Continue;

		pak_sa 'T': {	// Change tempo.
				unsigned nt = next_number();

				ຖ້າ ((nt >= 32) && (nt <= 255)) {
					_tempo = nt;

				} ບໍ່ດັ່ງນັ້ນ {
					ສົ່ງຄືນ tune_error();
				}

				bha_nga;
			}

		pak_sa 'N':	// Play an arbitrary note.
			note = next_number();

			ຖ້າ (note > 84) {
				ສົ່ງຄືນ tune_error();
			}

			ຖ້າ (note == 0) {
				// This is a rest - pause for the current note length.
				silence = rest_duration(_note_length, next_dots());
				ສົ່ງຄືນ Tunes::Status::Continue;
			}

			bha_nga;

		pak_sa 'A'...'G':	// Play a note in the current octave.
			note = _note_tab[c - 'A'] + (_octave * 12) + 1;
			c = next_char();

			switch (c) {
			pak_sa '#':	// Up a semitone.
			pak_sa '+':
				ຖ້າ (note < 84) {
					note++;
				}

				_next_tune++;
				bha_nga;

			pak_sa '-':	// Down a semitone.
				ຖ້າ (note > 1) {
					note--;
				}

				_next_tune++;
				bha_nga;

			default:
				// 0 / No next char here is OK.
				bha_nga;
			}

			// Shorthand length notation.
			note_length = next_number();

			ຖ້າ (note_length == 0) {
				note_length = _note_length;
			}

			bha_nga;

		default:
			ສົ່ງຄືນ tune_error();
		}
	}

	// Compute the duration of the note and the following silence (if any).
	duration = note_duration(silence, note_length, next_dots());

	// Compute the note frequency.
	frequency = note_to_frequency(note);
	ສົ່ງຄືນ Tunes::Status::Continue;
}

Tunes::Status Tunes::tune_end()
{
	ຖ້າ (_tunes_disabled) {
		ສົ່ງຄືນ Tunes::Status::Stop;
	}

	// Restore intial parameters.
	reset(_repeat);

	// Stop or restart the tune.
	ຖ້າ (_repeat) {
		ສົ່ງຄືນ Tunes::Status::Continue;

	} ບໍ່ດັ່ງນັ້ນ {
		ສົ່ງຄືນ Tunes::Status::Stop;
	}
}

Tunes::Status Tunes::tune_error()
{
	ຖ້າ (_tunes_disabled) {
		ສົ່ງຄືນ Tunes::Status::Stop;
	}

	// The tune appears to be bad (unexpected EOF, bad character, etc.).
	ຖ້າ (_next_tune != nullptr) {
		PX4_WARN("Tune error at: %s", _next_tune);
	}

	_repeat = false;	// Don't loop on error.
	reset(_repeat);
	ສົ່ງຄືນ Tunes::Status::Error;
}

uint32_t Tunes::note_to_frequency(unsigned note) const
{
	// Compute the frequency (Hz).
	ສົ່ງຄືນ (unsigned)(880.0f * powf(2.0f, ((ຈຳນວນເຕັມ)note - 46) / 12.0f));
}

unsigned Tunes::note_duration(unsigned &silence, unsigned note_length, unsigned dots) const
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	ຖ້າ (note_length == 0) {
		note_length = 1;
	}

	unsigned note_period = whole_note_period / note_length;

	switch (_note_mode) {
	pak_sa NoteMode::NORMAL:
		silence = note_period / 8;
		bha_nga;

	pak_sa NoteMode::STACCATO:
		silence = note_period / 4;
		bha_nga;

	pak_sa NoteMode::LEGATO:
	default:
		silence = 0;
		bha_nga;
	}

	note_period -= silence;

	unsigned dot_extension = note_period / 2;

	ໃນຂະນະທີ່ (dots--) {
		note_period += dot_extension;
		dot_extension /= 2;
	}

	ສົ່ງຄືນ note_period;
}

unsigned Tunes::rest_duration(unsigned rest_length, unsigned dots) const
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	ຖ້າ (rest_length == 0) {
		rest_length = 1;
	}

	unsigned rest_period = whole_note_period / rest_length;

	unsigned dot_extension = rest_period / 2;

	ໃນຂະນະທີ່ (dots--) {
		rest_period += dot_extension;
		dot_extension /= 2;
	}

	ສົ່ງຄືນ rest_period;
}

ຈຳນວນເຕັມ Tunes::next_char()
{
	ໃນຂະນະທີ່ (isspace(*_next_tune)) {
		_next_tune++;
	}

	ສົ່ງຄືນ toupper(*_next_tune);
}

unsigned Tunes::next_number()
{
	unsigned number = 0;
	ຈຳນວນເຕັມ next_character;

	ສຳລັບ (;;) {
		next_character = next_char();

		ຖ້າ (!isdigit(next_character)) {
			ສົ່ງຄືນ number;
		}

		_next_tune++;
		number = (number * 10) + (next_character - '0');
	}
}

unsigned Tunes::next_dots()
{
	unsigned dots = 0;

	ໃນຂະນະທີ່ (next_char() == '.') {
		_next_tune++;
		dots++;
	}

	ສົ່ງຄືນ dots;
}
