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

#அந்தர்பாவ "tunes.h"

#அந்தர்பாவ <px4_platform_common/log.h>

#அந்தர்பாவ <lib/circuit_breaker/circuit_breaker.h>

#அந்தர்பாவ <math.h>
#அந்தர்பாவ <ctype.h>
#அந்தர்பாவ <errno.h>

#define BEAT_TIME_CONVERSION_MS (60 * 1000 * 4)
#define BEAT_TIME_CONVERSION_US (BEAT_TIME_CONVERSION_MS * 1000)
#define BEAT_TIME_CONVERSION    BEAT_TIME_CONVERSION_US

// semitone offsets from C for the characters 'A'-'G'
மாறிலி uint8_t Tunes::_note_tab[] = {9, 11, 0, 2, 4, 5, 7};

Tunes::Tunes(unsigned default_note_length, NoteMode default_note_mode,
	     unsigned default_octave, unsigned default_tempo):
	_default_note_length(default_note_length),
	_default_note_mode(default_note_mode),
	_default_octave(default_octave),
	_default_tempo(default_tempo)
{
	எனில் (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_சாவி)) {
		_tunes_disabled = உண்மை;
	}

	reset(பொய்);
}

வெறுமை Tunes::reset(தருக்க repeat_flag)
{
	// reset pointer
	எனில் (!repeat_flag)	{
		_tune = nullptr;
		_next_tune = nullptr;

	} இல்லையெனில் {
		_tune = _tune_start_ptr;
		_next_tune = _tune;
	}

	// reset music parameter
	_note_length = _default_note_length;
	_note_mode   = _default_note_mode;
	_octave      = _default_octave;
	_tempo       = _default_tempo;
}

Tunes::ControlResult Tunes::set_control(மாறிலி tune_control_s &tune_control)
{
	// Sanity check
	எனில் (tune_control.tune_id >= _default_tunes_size) {
		திரும்பு ControlResult::InvalidTune;
	}

	// Accept new tune or a stop?
	எனில் (_tune == nullptr ||  	   // No tune is currently being played
	    tune_control.tune_override ||  // Override interrupts everything
	    _default_tunes_interruptable[_current_tune_id]) {

		// Check if this exact tune is already being played back
		எனில் (tune_control.tune_id != static_cast<முழு>(TuneID::CUSTOM) &&
		    _tune == _default_tunes[tune_control.tune_id]) {
			திரும்பு ControlResult::AlreadyPlaying; // Nothing to do
		}

		// Reset repeat flag. Can jump to true again while tune is being parsed later
		_repeat = பொய்;

		// Reset octave, tempo etc.
		reset(_repeat);

		// Volume will remain valid for the entire tune, unless interrupted.
		எனில் (tune_control.volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = tune_control.volume;

		} இல்லையெனில் {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}


		// Special treatment for custom tunes
		எனில் (tune_control.tune_id == static_cast<முழு>(TuneID::CUSTOM)) {
			using_custom_செய்தி = உண்மை;
			_frequency        = (unsigned)tune_control.frequency;
			_duration         = (unsigned)tune_control.duration;
			_silence          = (unsigned)tune_control.silence;

		} இல்லையெனில் {
			using_custom_செய்தி = பொய்;
			_tune             = _default_tunes[tune_control.tune_id];
			_tune_start_ptr   = _tune;
			_next_tune        = _tune;
		}

		_current_tune_id = tune_control.tune_id;
		திரும்பு ControlResult::Success;
	}

	திரும்பு ControlResult::WouldInterrupt;
}

வெறுமை Tunes::set_string(மாறிலி எழுத்து *மாறிலி string, uint8_t volume)
{
	// Only play new tune if nothing is being played currently.
	எனில் (_tune == nullptr) {
		// Set tune string the first time.
		_tune           = string;
		_tune_start_ptr = string;
		_next_tune      = _tune;

		எனில் (volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = volume;

		} இல்லையெனில் {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}
	}
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence, uint8_t &volume)
{
	எனில் (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		volume = 0;
		திரும்பு Tunes::Status::Stop;
	}

	Tunes::Status ret = get_next_note(frequency, duration, silence);

	// Check if note should not be heard -> adjust volume to 0 to be safe.
	எனில் (frequency == 0 || duration == 0) {
		volume = 0;

	} இல்லையெனில் {
		volume = _volume;
	}

	திரும்பு ret;
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence)
{
	எனில் (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		திரும்பு Tunes::Status::Stop;
	}

	// Return the values for frequency and duration if the custom msg was received.
	எனில் (using_custom_செய்தி) {
		using_custom_செய்தி = பொய்;
		duration  = _duration;
		frequency = _frequency;
		silence   = _silence;
		திரும்பு Tunes::Status::Continue;
	}

	// Make sure we still have a tune.
	எனில் ((_next_tune == nullptr) || (_tune == nullptr)) {
		திரும்பு tune_error();
	}

	// Parse characters out of the string until we have resolved a note.
	unsigned note = 0;
	unsigned note_length = _note_length;

	போது (note == 0) {
		// we always need at least one character from the string.
		முழு c = next_char();

		எனில் (c == 0) {
			silence = 0;
			திரும்பு tune_end();
		}

		_next_tune++;

		தேர்வு (c) {

		நிலை 'V':	// Select volume.
			// Consume volume if specified in tune but ignore
			// MML compatibility
			next_எண்();
			நிறுத்து;

		நிலை 'L':	// Select note length.
			_note_length = next_எண்();

			எனில் (_note_length < 1) {
				திரும்பு tune_error();
			}

			நிறுத்து;

		நிலை 'O':	// Select octave.
			_octave = next_எண்();

			எனில் (_octave > 6) {
				_octave = 6;
			}

			நிறுத்து;

		நிலை '<':	// Decrease octave.
			எனில் (_octave > 0) {
				_octave--;
			}

			நிறுத்து;

		நிலை '>':	// Increase octave.
			எனில் (_octave < 6) {
				_octave++;
			}

			நிறுத்து;

		நிலை 'M':	// Select inter-note gap.
			c = next_char();

			எனில் (c == 0) {
				திரும்பு tune_error();
			}

			_next_tune++;

			தேர்வு (c) {
			நிலை 'N':
				_note_mode = NoteMode::NORMAL;
				நிறுத்து;

			நிலை 'L':
				_note_mode = NoteMode::LEGATO;
				நிறுத்து;

			நிலை 'S':
				_note_mode = NoteMode::STACCATO;
				நிறுத்து;

			நிலை 'F':
				_repeat = பொய்;
				நிறுத்து;

			நிலை 'B':
				_repeat = உண்மை;
				நிறுத்து;

			இயல்பு:
				திரும்பு tune_error();
			}

			நிறுத்து;

		நிலை 'P':	// Pause for a note length.
		நிலை 'R':
			frequency = 0;
			duration = 0;
			note_length = next_எண்();
			silence = rest_duration(note_length, next_dots());
			திரும்பு Tunes::Status::Continue;

		நிலை 'T': {	// Change tempo.
				unsigned nt = next_எண்();

				எனில் ((nt >= 32) && (nt <= 255)) {
					_tempo = nt;

				} இல்லையெனில் {
					திரும்பு tune_error();
				}

				நிறுத்து;
			}

		நிலை 'N':	// Play an arbitrary note.
			note = next_எண்();

			எனில் (note > 84) {
				திரும்பு tune_error();
			}

			எனில் (note == 0) {
				// This is a rest - pause for the current note length.
				silence = rest_duration(_note_length, next_dots());
				திரும்பு Tunes::Status::Continue;
			}

			நிறுத்து;

		நிலை 'A'...'G':	// Play a note in the current octave.
			note = _note_tab[c - 'A'] + (_octave * 12) + 1;
			c = next_char();

			தேர்வு (c) {
			நிலை '#':	// Up a semitone.
			நிலை '+':
				எனில் (note < 84) {
					note++;
				}

				_next_tune++;
				நிறுத்து;

			நிலை '-':	// Down a semitone.
				எனில் (note > 1) {
					note--;
				}

				_next_tune++;
				நிறுத்து;

			இயல்பு:
				// 0 / No next char here is OK.
				நிறுத்து;
			}

			// Shorthand length notation.
			note_length = next_எண்();

			எனில் (note_length == 0) {
				note_length = _note_length;
			}

			நிறுத்து;

		இயல்பு:
			திரும்பு tune_error();
		}
	}

	// Compute the duration of the note and the following silence (if any).
	duration = note_duration(silence, note_length, next_dots());

	// Compute the note frequency.
	frequency = note_to_frequency(note);
	திரும்பு Tunes::Status::Continue;
}

Tunes::Status Tunes::tune_end()
{
	எனில் (_tunes_disabled) {
		திரும்பு Tunes::Status::Stop;
	}

	// Restore intial parameters.
	reset(_repeat);

	// Stop or restart the tune.
	எனில் (_repeat) {
		திரும்பு Tunes::Status::Continue;

	} இல்லையெனில் {
		திரும்பு Tunes::Status::Stop;
	}
}

Tunes::Status Tunes::tune_error()
{
	எனில் (_tunes_disabled) {
		திரும்பு Tunes::Status::Stop;
	}

	// The tune appears to be bad (unexpected EOF, bad character, etc.).
	எனில் (_next_tune != nullptr) {
		PX4_WARN("Tune error at: %s", _next_tune);
	}

	_repeat = பொய்;	// Don't loop on error.
	reset(_repeat);
	திரும்பு Tunes::Status::Error;
}

uint32_t Tunes::note_to_frequency(unsigned note) மாறிலி
{
	// Compute the frequency (Hz).
	திரும்பு (unsigned)(880.0f * powf(2.0f, ((முழு)note - 46) / 12.0f));
}

unsigned Tunes::note_duration(unsigned &silence, unsigned note_length, unsigned dots) மாறிலி
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	எனில் (note_length == 0) {
		note_length = 1;
	}

	unsigned note_period = whole_note_period / note_length;

	தேர்வு (_note_mode) {
	நிலை NoteMode::NORMAL:
		silence = note_period / 8;
		நிறுத்து;

	நிலை NoteMode::STACCATO:
		silence = note_period / 4;
		நிறுத்து;

	நிலை NoteMode::LEGATO:
	இயல்பு:
		silence = 0;
		நிறுத்து;
	}

	note_period -= silence;

	unsigned dot_extension = note_period / 2;

	போது (dots--) {
		note_period += dot_extension;
		dot_extension /= 2;
	}

	திரும்பு note_period;
}

unsigned Tunes::rest_duration(unsigned rest_length, unsigned dots) மாறிலி
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	எனில் (rest_length == 0) {
		rest_length = 1;
	}

	unsigned rest_period = whole_note_period / rest_length;

	unsigned dot_extension = rest_period / 2;

	போது (dots--) {
		rest_period += dot_extension;
		dot_extension /= 2;
	}

	திரும்பு rest_period;
}

முழு Tunes::next_char()
{
	போது (isspace(*_next_tune)) {
		_next_tune++;
	}

	திரும்பு toupper(*_next_tune);
}

unsigned Tunes::next_எண்()
{
	unsigned எண் = 0;
	முழு next_character;

	க்கு (;;) {
		next_character = next_char();

		எனில் (!isdigit(next_character)) {
			திரும்பு எண்;
		}

		_next_tune++;
		எண் = (எண் * 10) + (next_character - '0');
	}
}

unsigned Tunes::next_dots()
{
	unsigned dots = 0;

	போது (next_char() == '.') {
		_next_tune++;
		dots++;
	}

	திரும்பு dots;
}
