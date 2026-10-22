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

#അന്തര്ഭാവ "tunes.h"

#അന്തര്ഭാവ <px4_platform_common/log.h>

#അന്തര്ഭാവ <lib/circuit_breaker/circuit_breaker.h>

#അന്തര്ഭാവ <math.h>
#അന്തര്ഭാവ <ctype.h>
#അന്തര്ഭാവ <errno.h>

#define BEAT_TIME_CONVERSION_MS (60 * 1000 * 4)
#define BEAT_TIME_CONVERSION_US (BEAT_TIME_CONVERSION_MS * 1000)
#define BEAT_TIME_CONVERSION    BEAT_TIME_CONVERSION_US

// semitone offsets from C for the characters 'A'-'G'
സ്ഥിരം uint8_t Tunes::_note_tab[] = {9, 11, 0, 2, 4, 5, 7};

Tunes::Tunes(unsigned default_note_length, NoteMode default_note_mode,
	     unsigned default_octave, unsigned default_tempo):
	_default_note_length(default_note_length),
	_default_note_mode(default_note_mode),
	_default_octave(default_octave),
	_default_tempo(default_tempo)
{
	എങ്കിൽ (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_താക്കോൽ)) {
		_tunes_disabled = സത്യം;
	}

	reset(അസത്യം);
}

ശൂന്യം Tunes::reset(തർക്കം repeat_flag)
{
	// reset pointer
	എങ്കിൽ (!repeat_flag)	{
		_tune = nullptr;
		_next_tune = nullptr;

	} അല്ലെങ്കിൽ {
		_tune = _tune_start_ptr;
		_next_tune = _tune;
	}

	// reset music parameter
	_note_length = _default_note_length;
	_note_mode   = _default_note_mode;
	_octave      = _default_octave;
	_tempo       = _default_tempo;
}

Tunes::ControlResult Tunes::set_control(സ്ഥിരം tune_control_s &tune_control)
{
	// Sanity check
	എങ്കിൽ (tune_control.tune_id >= _default_tunes_size) {
		തിരികെ ControlResult::InvalidTune;
	}

	// Accept new tune or a stop?
	എങ്കിൽ (_tune == nullptr ||  	   // No tune is currently being played
	    tune_control.tune_override ||  // Override interrupts everything
	    _default_tunes_interruptable[_current_tune_id]) {

		// Check if this exact tune is already being played back
		എങ്കിൽ (tune_control.tune_id != static_cast<പൂർണ്ണസംഖ്യ>(TuneID::CUSTOM) &&
		    _tune == _default_tunes[tune_control.tune_id]) {
			തിരികെ ControlResult::AlreadyPlaying; // Nothing to do
		}

		// Reset repeat flag. Can jump to true again while tune is being parsed later
		_repeat = അസത്യം;

		// Reset octave, tempo etc.
		reset(_repeat);

		// Volume will remain valid for the entire tune, unless interrupted.
		എങ്കിൽ (tune_control.volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = tune_control.volume;

		} അല്ലെങ്കിൽ {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}


		// Special treatment for custom tunes
		എങ്കിൽ (tune_control.tune_id == static_cast<പൂർണ്ണസംഖ്യ>(TuneID::CUSTOM)) {
			using_custom_സന്ദേശം = സത്യം;
			_frequency        = (unsigned)tune_control.frequency;
			_duration         = (unsigned)tune_control.duration;
			_silence          = (unsigned)tune_control.silence;

		} അല്ലെങ്കിൽ {
			using_custom_സന്ദേശം = അസത്യം;
			_tune             = _default_tunes[tune_control.tune_id];
			_tune_start_ptr   = _tune;
			_next_tune        = _tune;
		}

		_current_tune_id = tune_control.tune_id;
		തിരികെ ControlResult::Success;
	}

	തിരികെ ControlResult::WouldInterrupt;
}

ശൂന്യം Tunes::set_string(സ്ഥിരം അക്ഷരം *സ്ഥിരം string, uint8_t volume)
{
	// Only play new tune if nothing is being played currently.
	എങ്കിൽ (_tune == nullptr) {
		// Set tune string the first time.
		_tune           = string;
		_tune_start_ptr = string;
		_next_tune      = _tune;

		എങ്കിൽ (volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = volume;

		} അല്ലെങ്കിൽ {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}
	}
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence, uint8_t &volume)
{
	എങ്കിൽ (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		volume = 0;
		തിരികെ Tunes::Status::Stop;
	}

	Tunes::Status ret = get_next_note(frequency, duration, silence);

	// Check if note should not be heard -> adjust volume to 0 to be safe.
	എങ്കിൽ (frequency == 0 || duration == 0) {
		volume = 0;

	} അല്ലെങ്കിൽ {
		volume = _volume;
	}

	തിരികെ ret;
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence)
{
	എങ്കിൽ (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		തിരികെ Tunes::Status::Stop;
	}

	// Return the values for frequency and duration if the custom msg was received.
	എങ്കിൽ (using_custom_സന്ദേശം) {
		using_custom_സന്ദേശം = അസത്യം;
		duration  = _duration;
		frequency = _frequency;
		silence   = _silence;
		തിരികെ Tunes::Status::Continue;
	}

	// Make sure we still have a tune.
	എങ്കിൽ ((_next_tune == nullptr) || (_tune == nullptr)) {
		തിരികെ tune_error();
	}

	// Parse characters out of the string until we have resolved a note.
	unsigned note = 0;
	unsigned note_length = _note_length;

	ആകുമ്പോൾ (note == 0) {
		// we always need at least one character from the string.
		പൂർണ്ണസംഖ്യ c = next_char();

		എങ്കിൽ (c == 0) {
			silence = 0;
			തിരികെ tune_end();
		}

		_next_tune++;

		തിരഞ്ഞെടുപ്പ് (c) {

		അവസ്ഥ 'V':	// Select volume.
			// Consume volume if specified in tune but ignore
			// MML compatibility
			next_സംഖ്യ();
			നിർത്തുക;

		അവസ്ഥ 'L':	// Select note length.
			_note_length = next_സംഖ്യ();

			എങ്കിൽ (_note_length < 1) {
				തിരികെ tune_error();
			}

			നിർത്തുക;

		അവസ്ഥ 'O':	// Select octave.
			_octave = next_സംഖ്യ();

			എങ്കിൽ (_octave > 6) {
				_octave = 6;
			}

			നിർത്തുക;

		അവസ്ഥ '<':	// Decrease octave.
			എങ്കിൽ (_octave > 0) {
				_octave--;
			}

			നിർത്തുക;

		അവസ്ഥ '>':	// Increase octave.
			എങ്കിൽ (_octave < 6) {
				_octave++;
			}

			നിർത്തുക;

		അവസ്ഥ 'M':	// Select inter-note gap.
			c = next_char();

			എങ്കിൽ (c == 0) {
				തിരികെ tune_error();
			}

			_next_tune++;

			തിരഞ്ഞെടുപ്പ് (c) {
			അവസ്ഥ 'N':
				_note_mode = NoteMode::NORMAL;
				നിർത്തുക;

			അവസ്ഥ 'L':
				_note_mode = NoteMode::LEGATO;
				നിർത്തുക;

			അവസ്ഥ 'S':
				_note_mode = NoteMode::STACCATO;
				നിർത്തുക;

			അവസ്ഥ 'F':
				_repeat = അസത്യം;
				നിർത്തുക;

			അവസ്ഥ 'B':
				_repeat = സത്യം;
				നിർത്തുക;

			സ്ഥിരസ്ഥിതി:
				തിരികെ tune_error();
			}

			നിർത്തുക;

		അവസ്ഥ 'P':	// Pause for a note length.
		അവസ്ഥ 'R':
			frequency = 0;
			duration = 0;
			note_length = next_സംഖ്യ();
			silence = rest_duration(note_length, next_dots());
			തിരികെ Tunes::Status::Continue;

		അവസ്ഥ 'T': {	// Change tempo.
				unsigned nt = next_സംഖ്യ();

				എങ്കിൽ ((nt >= 32) && (nt <= 255)) {
					_tempo = nt;

				} അല്ലെങ്കിൽ {
					തിരികെ tune_error();
				}

				നിർത്തുക;
			}

		അവസ്ഥ 'N':	// Play an arbitrary note.
			note = next_സംഖ്യ();

			എങ്കിൽ (note > 84) {
				തിരികെ tune_error();
			}

			എങ്കിൽ (note == 0) {
				// This is a rest - pause for the current note length.
				silence = rest_duration(_note_length, next_dots());
				തിരികെ Tunes::Status::Continue;
			}

			നിർത്തുക;

		അവസ്ഥ 'A'...'G':	// Play a note in the current octave.
			note = _note_tab[c - 'A'] + (_octave * 12) + 1;
			c = next_char();

			തിരഞ്ഞെടുപ്പ് (c) {
			അവസ്ഥ '#':	// Up a semitone.
			അവസ്ഥ '+':
				എങ്കിൽ (note < 84) {
					note++;
				}

				_next_tune++;
				നിർത്തുക;

			അവസ്ഥ '-':	// Down a semitone.
				എങ്കിൽ (note > 1) {
					note--;
				}

				_next_tune++;
				നിർത്തുക;

			സ്ഥിരസ്ഥിതി:
				// 0 / No next char here is OK.
				നിർത്തുക;
			}

			// Shorthand length notation.
			note_length = next_സംഖ്യ();

			എങ്കിൽ (note_length == 0) {
				note_length = _note_length;
			}

			നിർത്തുക;

		സ്ഥിരസ്ഥിതി:
			തിരികെ tune_error();
		}
	}

	// Compute the duration of the note and the following silence (if any).
	duration = note_duration(silence, note_length, next_dots());

	// Compute the note frequency.
	frequency = note_to_frequency(note);
	തിരികെ Tunes::Status::Continue;
}

Tunes::Status Tunes::tune_end()
{
	എങ്കിൽ (_tunes_disabled) {
		തിരികെ Tunes::Status::Stop;
	}

	// Restore intial parameters.
	reset(_repeat);

	// Stop or restart the tune.
	എങ്കിൽ (_repeat) {
		തിരികെ Tunes::Status::Continue;

	} അല്ലെങ്കിൽ {
		തിരികെ Tunes::Status::Stop;
	}
}

Tunes::Status Tunes::tune_error()
{
	എങ്കിൽ (_tunes_disabled) {
		തിരികെ Tunes::Status::Stop;
	}

	// The tune appears to be bad (unexpected EOF, bad character, etc.).
	എങ്കിൽ (_next_tune != nullptr) {
		PX4_WARN("Tune error at: %s", _next_tune);
	}

	_repeat = അസത്യം;	// Don't loop on error.
	reset(_repeat);
	തിരികെ Tunes::Status::Error;
}

uint32_t Tunes::note_to_frequency(unsigned note) സ്ഥിരം
{
	// Compute the frequency (Hz).
	തിരികെ (unsigned)(880.0f * powf(2.0f, ((പൂർണ്ണസംഖ്യ)note - 46) / 12.0f));
}

unsigned Tunes::note_duration(unsigned &silence, unsigned note_length, unsigned dots) സ്ഥിരം
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	എങ്കിൽ (note_length == 0) {
		note_length = 1;
	}

	unsigned note_period = whole_note_period / note_length;

	തിരഞ്ഞെടുപ്പ് (_note_mode) {
	അവസ്ഥ NoteMode::NORMAL:
		silence = note_period / 8;
		നിർത്തുക;

	അവസ്ഥ NoteMode::STACCATO:
		silence = note_period / 4;
		നിർത്തുക;

	അവസ്ഥ NoteMode::LEGATO:
	സ്ഥിരസ്ഥിതി:
		silence = 0;
		നിർത്തുക;
	}

	note_period -= silence;

	unsigned dot_extension = note_period / 2;

	ആകുമ്പോൾ (dots--) {
		note_period += dot_extension;
		dot_extension /= 2;
	}

	തിരികെ note_period;
}

unsigned Tunes::rest_duration(unsigned rest_length, unsigned dots) സ്ഥിരം
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	എങ്കിൽ (rest_length == 0) {
		rest_length = 1;
	}

	unsigned rest_period = whole_note_period / rest_length;

	unsigned dot_extension = rest_period / 2;

	ആകുമ്പോൾ (dots--) {
		rest_period += dot_extension;
		dot_extension /= 2;
	}

	തിരികെ rest_period;
}

പൂർണ്ണസംഖ്യ Tunes::next_char()
{
	ആകുമ്പോൾ (isspace(*_next_tune)) {
		_next_tune++;
	}

	തിരികെ toupper(*_next_tune);
}

unsigned Tunes::next_സംഖ്യ()
{
	unsigned സംഖ്യ = 0;
	പൂർണ്ണസംഖ്യ next_character;

	വേണ്ടി (;;) {
		next_character = next_char();

		എങ്കിൽ (!isdigit(next_character)) {
			തിരികെ സംഖ്യ;
		}

		_next_tune++;
		സംഖ്യ = (സംഖ്യ * 10) + (next_character - '0');
	}
}

unsigned Tunes::next_dots()
{
	unsigned dots = 0;

	ആകുമ്പോൾ (next_char() == '.') {
		_next_tune++;
		dots++;
	}

	തിരികെ dots;
}
