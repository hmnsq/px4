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

#ಅನ್ತರ್ಭಾವ "tunes.h"

#ಅನ್ತರ್ಭಾವ <px4_platform_common/log.h>

#ಅನ್ತರ್ಭಾವ <lib/circuit_breaker/circuit_breaker.h>

#ಅನ್ತರ್ಭಾವ <math.h>
#ಅನ್ತರ್ಭಾವ <ctype.h>
#ಅನ್ತರ್ಭಾವ <errno.h>

#define BEAT_TIME_CONVERSION_MS (60 * 1000 * 4)
#define BEAT_TIME_CONVERSION_US (BEAT_TIME_CONVERSION_MS * 1000)
#define BEAT_TIME_CONVERSION    BEAT_TIME_CONVERSION_US

// semitone offsets from C for the characters 'A'-'G'
ಧ್ರುವ uint8_t Tunes::_note_tab[] = {9, 11, 0, 2, 4, 5, 7};

Tunes::Tunes(unsigned default_note_length, NoteMode default_note_mode,
	     unsigned default_octave, unsigned default_tempo):
	_default_note_length(default_note_length),
	_default_note_mode(default_note_mode),
	_default_octave(default_octave),
	_default_tempo(default_tempo)
{
	ಯದಿ (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_ಕೀಲಿ)) {
		_tunes_disabled = ಸತ್ಯ;
	}

	reset(ಅಸತ್ಯ);
}

ಶೂನ್ಯ Tunes::reset(ತಾರ್ಕಿಕ repeat_flag)
{
	// reset pointer
	ಯದಿ (!repeat_flag)	{
		_tune = nullptr;
		_next_tune = nullptr;

	} ಅನ್ಯಥಾ {
		_tune = _tune_start_ptr;
		_next_tune = _tune;
	}

	// reset music parameter
	_note_length = _default_note_length;
	_note_mode   = _default_note_mode;
	_octave      = _default_octave;
	_tempo       = _default_tempo;
}

Tunes::ControlResult Tunes::set_control(ಧ್ರುವ tune_control_s &tune_control)
{
	// Sanity check
	ಯದಿ (tune_control.tune_id >= _default_tunes_size) {
		ಪ್ರತ್ಯಾಗಮ ControlResult::InvalidTune;
	}

	// Accept new tune or a stop?
	ಯದಿ (_tune == nullptr ||  	   // No tune is currently being played
	    tune_control.tune_override ||  // Override interrupts everything
	    _default_tunes_interruptable[_current_tune_id]) {

		// Check if this exact tune is already being played back
		ಯದಿ (tune_control.tune_id != static_cast<ಪೂರ್ಣಾಙ್ಕ>(TuneID::CUSTOM) &&
		    _tune == _default_tunes[tune_control.tune_id]) {
			ಪ್ರತ್ಯಾಗಮ ControlResult::AlreadyPlaying; // Nothing to do
		}

		// Reset repeat flag. Can jump to true again while tune is being parsed later
		_repeat = ಅಸತ್ಯ;

		// Reset octave, tempo etc.
		reset(_repeat);

		// Volume will remain valid for the entire tune, unless interrupted.
		ಯದಿ (tune_control.volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = tune_control.volume;

		} ಅನ್ಯಥಾ {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}


		// Special treatment for custom tunes
		ಯದಿ (tune_control.tune_id == static_cast<ಪೂರ್ಣಾಙ್ಕ>(TuneID::CUSTOM)) {
			using_custom_ಸಂದೇಶ = ಸತ್ಯ;
			_frequency        = (unsigned)tune_control.frequency;
			_duration         = (unsigned)tune_control.duration;
			_silence          = (unsigned)tune_control.silence;

		} ಅನ್ಯಥಾ {
			using_custom_ಸಂದೇಶ = ಅಸತ್ಯ;
			_tune             = _default_tunes[tune_control.tune_id];
			_tune_start_ptr   = _tune;
			_next_tune        = _tune;
		}

		_current_tune_id = tune_control.tune_id;
		ಪ್ರತ್ಯಾಗಮ ControlResult::Success;
	}

	ಪ್ರತ್ಯಾಗಮ ControlResult::WouldInterrupt;
}

ಶೂನ್ಯ Tunes::set_string(ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ string, uint8_t volume)
{
	// Only play new tune if nothing is being played currently.
	ಯದಿ (_tune == nullptr) {
		// Set tune string the first time.
		_tune           = string;
		_tune_start_ptr = string;
		_next_tune      = _tune;

		ಯದಿ (volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = volume;

		} ಅನ್ಯಥಾ {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}
	}
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence, uint8_t &volume)
{
	ಯದಿ (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		volume = 0;
		ಪ್ರತ್ಯಾಗಮ Tunes::Status::Stop;
	}

	Tunes::Status ret = get_next_note(frequency, duration, silence);

	// Check if note should not be heard -> adjust volume to 0 to be safe.
	ಯದಿ (frequency == 0 || duration == 0) {
		volume = 0;

	} ಅನ್ಯಥಾ {
		volume = _volume;
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence)
{
	ಯದಿ (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		ಪ್ರತ್ಯಾಗಮ Tunes::Status::Stop;
	}

	// Return the values for frequency and duration if the custom msg was received.
	ಯದಿ (using_custom_ಸಂದೇಶ) {
		using_custom_ಸಂದೇಶ = ಅಸತ್ಯ;
		duration  = _duration;
		frequency = _frequency;
		silence   = _silence;
		ಪ್ರತ್ಯಾಗಮ Tunes::Status::Continue;
	}

	// Make sure we still have a tune.
	ಯದಿ ((_next_tune == nullptr) || (_tune == nullptr)) {
		ಪ್ರತ್ಯಾಗಮ tune_error();
	}

	// Parse characters out of the string until we have resolved a note.
	unsigned note = 0;
	unsigned note_length = _note_length;

	ಯಾವತ್ (note == 0) {
		// we always need at least one character from the string.
		ಪೂರ್ಣಾಙ್ಕ c = next_char();

		ಯದಿ (c == 0) {
			silence = 0;
			ಪ್ರತ್ಯಾಗಮ tune_end();
		}

		_next_tune++;

		ವಿಕಲ್ಪ (c) {

		ಪಕ್ಷ 'V':	// Select volume.
			// Consume volume if specified in tune but ignore
			// MML compatibility
			next_ಸಂಖ್ಯೆ();
			ಭಙ್ಗ;

		ಪಕ್ಷ 'L':	// Select note length.
			_note_length = next_ಸಂಖ್ಯೆ();

			ಯದಿ (_note_length < 1) {
				ಪ್ರತ್ಯಾಗಮ tune_error();
			}

			ಭಙ್ಗ;

		ಪಕ್ಷ 'O':	// Select octave.
			_octave = next_ಸಂಖ್ಯೆ();

			ಯದಿ (_octave > 6) {
				_octave = 6;
			}

			ಭಙ್ಗ;

		ಪಕ್ಷ '<':	// Decrease octave.
			ಯದಿ (_octave > 0) {
				_octave--;
			}

			ಭಙ್ಗ;

		ಪಕ್ಷ '>':	// Increase octave.
			ಯದಿ (_octave < 6) {
				_octave++;
			}

			ಭಙ್ಗ;

		ಪಕ್ಷ 'M':	// Select inter-note gap.
			c = next_char();

			ಯದಿ (c == 0) {
				ಪ್ರತ್ಯಾಗಮ tune_error();
			}

			_next_tune++;

			ವಿಕಲ್ಪ (c) {
			ಪಕ್ಷ 'N':
				_note_mode = NoteMode::NORMAL;
				ಭಙ್ಗ;

			ಪಕ್ಷ 'L':
				_note_mode = NoteMode::LEGATO;
				ಭಙ್ಗ;

			ಪಕ್ಷ 'S':
				_note_mode = NoteMode::STACCATO;
				ಭಙ್ಗ;

			ಪಕ್ಷ 'F':
				_repeat = ಅಸತ್ಯ;
				ಭಙ್ಗ;

			ಪಕ್ಷ 'B':
				_repeat = ಸತ್ಯ;
				ಭಙ್ಗ;

			ಸ್ವಭಾವ:
				ಪ್ರತ್ಯಾಗಮ tune_error();
			}

			ಭಙ್ಗ;

		ಪಕ್ಷ 'P':	// Pause for a note length.
		ಪಕ್ಷ 'R':
			frequency = 0;
			duration = 0;
			note_length = next_ಸಂಖ್ಯೆ();
			silence = rest_duration(note_length, next_dots());
			ಪ್ರತ್ಯಾಗಮ Tunes::Status::Continue;

		ಪಕ್ಷ 'T': {	// Change tempo.
				unsigned nt = next_ಸಂಖ್ಯೆ();

				ಯದಿ ((nt >= 32) && (nt <= 255)) {
					_tempo = nt;

				} ಅನ್ಯಥಾ {
					ಪ್ರತ್ಯಾಗಮ tune_error();
				}

				ಭಙ್ಗ;
			}

		ಪಕ್ಷ 'N':	// Play an arbitrary note.
			note = next_ಸಂಖ್ಯೆ();

			ಯದಿ (note > 84) {
				ಪ್ರತ್ಯಾಗಮ tune_error();
			}

			ಯದಿ (note == 0) {
				// This is a rest - pause for the current note length.
				silence = rest_duration(_note_length, next_dots());
				ಪ್ರತ್ಯಾಗಮ Tunes::Status::Continue;
			}

			ಭಙ್ಗ;

		ಪಕ್ಷ 'A'...'G':	// Play a note in the current octave.
			note = _note_tab[c - 'A'] + (_octave * 12) + 1;
			c = next_char();

			ವಿಕಲ್ಪ (c) {
			ಪಕ್ಷ '#':	// Up a semitone.
			ಪಕ್ಷ '+':
				ಯದಿ (note < 84) {
					note++;
				}

				_next_tune++;
				ಭಙ್ಗ;

			ಪಕ್ಷ '-':	// Down a semitone.
				ಯದಿ (note > 1) {
					note--;
				}

				_next_tune++;
				ಭಙ್ಗ;

			ಸ್ವಭಾವ:
				// 0 / No next char here is OK.
				ಭಙ್ಗ;
			}

			// Shorthand length notation.
			note_length = next_ಸಂಖ್ಯೆ();

			ಯದಿ (note_length == 0) {
				note_length = _note_length;
			}

			ಭಙ್ಗ;

		ಸ್ವಭಾವ:
			ಪ್ರತ್ಯಾಗಮ tune_error();
		}
	}

	// Compute the duration of the note and the following silence (if any).
	duration = note_duration(silence, note_length, next_dots());

	// Compute the note frequency.
	frequency = note_to_frequency(note);
	ಪ್ರತ್ಯಾಗಮ Tunes::Status::Continue;
}

Tunes::Status Tunes::tune_end()
{
	ಯದಿ (_tunes_disabled) {
		ಪ್ರತ್ಯಾಗಮ Tunes::Status::Stop;
	}

	// Restore intial parameters.
	reset(_repeat);

	// Stop or restart the tune.
	ಯದಿ (_repeat) {
		ಪ್ರತ್ಯಾಗಮ Tunes::Status::Continue;

	} ಅನ್ಯಥಾ {
		ಪ್ರತ್ಯಾಗಮ Tunes::Status::Stop;
	}
}

Tunes::Status Tunes::tune_error()
{
	ಯದಿ (_tunes_disabled) {
		ಪ್ರತ್ಯಾಗಮ Tunes::Status::Stop;
	}

	// The tune appears to be bad (unexpected EOF, bad character, etc.).
	ಯದಿ (_next_tune != nullptr) {
		PX4_WARN("Tune error at: %s", _next_tune);
	}

	_repeat = ಅಸತ್ಯ;	// Don't loop on error.
	reset(_repeat);
	ಪ್ರತ್ಯಾಗಮ Tunes::Status::Error;
}

uint32_t Tunes::note_to_frequency(unsigned note) ಧ್ರುವ
{
	// Compute the frequency (Hz).
	ಪ್ರತ್ಯಾಗಮ (unsigned)(880.0f * powf(2.0f, ((ಪೂರ್ಣಾಙ್ಕ)note - 46) / 12.0f));
}

unsigned Tunes::note_duration(unsigned &silence, unsigned note_length, unsigned dots) ಧ್ರುವ
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	ಯದಿ (note_length == 0) {
		note_length = 1;
	}

	unsigned note_period = whole_note_period / note_length;

	ವಿಕಲ್ಪ (_note_mode) {
	ಪಕ್ಷ NoteMode::NORMAL:
		silence = note_period / 8;
		ಭಙ್ಗ;

	ಪಕ್ಷ NoteMode::STACCATO:
		silence = note_period / 4;
		ಭಙ್ಗ;

	ಪಕ್ಷ NoteMode::LEGATO:
	ಸ್ವಭಾವ:
		silence = 0;
		ಭಙ್ಗ;
	}

	note_period -= silence;

	unsigned dot_extension = note_period / 2;

	ಯಾವತ್ (dots--) {
		note_period += dot_extension;
		dot_extension /= 2;
	}

	ಪ್ರತ್ಯಾಗಮ note_period;
}

unsigned Tunes::rest_duration(unsigned rest_length, unsigned dots) ಧ್ರುವ
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	ಯದಿ (rest_length == 0) {
		rest_length = 1;
	}

	unsigned rest_period = whole_note_period / rest_length;

	unsigned dot_extension = rest_period / 2;

	ಯಾವತ್ (dots--) {
		rest_period += dot_extension;
		dot_extension /= 2;
	}

	ಪ್ರತ್ಯಾಗಮ rest_period;
}

ಪೂರ್ಣಾಙ್ಕ Tunes::next_char()
{
	ಯಾವತ್ (isspace(*_next_tune)) {
		_next_tune++;
	}

	ಪ್ರತ್ಯಾಗಮ toupper(*_next_tune);
}

unsigned Tunes::next_ಸಂಖ್ಯೆ()
{
	unsigned ಸಂಖ್ಯೆ = 0;
	ಪೂರ್ಣಾಙ್ಕ next_character;

	ಪ್ರತಿ (;;) {
		next_character = next_char();

		ಯದಿ (!isdigit(next_character)) {
			ಪ್ರತ್ಯಾಗಮ ಸಂಖ್ಯೆ;
		}

		_next_tune++;
		ಸಂಖ್ಯೆ = (ಸಂಖ್ಯೆ * 10) + (next_character - '0');
	}
}

unsigned Tunes::next_dots()
{
	unsigned dots = 0;

	ಯಾವತ್ (next_char() == '.') {
		_next_tune++;
		dots++;
	}

	ಪ್ರತ್ಯಾಗಮ dots;
}
