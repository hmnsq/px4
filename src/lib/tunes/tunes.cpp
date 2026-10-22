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

#ଅନ୍ତର୍ଭାଵ "tunes.h"

#ଅନ୍ତର୍ଭାଵ <px4_platform_common/log.h>

#ଅନ୍ତର୍ଭାଵ <lib/circuit_breaker/circuit_breaker.h>

#ଅନ୍ତର୍ଭାଵ <math.h>
#ଅନ୍ତର୍ଭାଵ <ctype.h>
#ଅନ୍ତର୍ଭାଵ <errno.h>

#define BEAT_TIME_CONVERSION_MS (60 * 1000 * 4)
#define BEAT_TIME_CONVERSION_US (BEAT_TIME_CONVERSION_MS * 1000)
#define BEAT_TIME_CONVERSION    BEAT_TIME_CONVERSION_US

// semitone offsets from C for the characters 'A'-'G'
ଅଚଳ uint8_t Tunes::_note_tab[] = {9, 11, 0, 2, 4, 5, 7};

Tunes::Tunes(unsigned default_note_length, NoteMode default_note_mode,
	     unsigned default_octave, unsigned default_tempo):
	_default_note_length(default_note_length),
	_default_note_mode(default_note_mode),
	_default_octave(default_octave),
	_default_tempo(default_tempo)
{
	ଯଦି (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_ଚାବି)) {
		_tunes_disabled = ସତ୍ୟ;
	}

	reset(ମିଥ୍ୟା);
}

ଶୂନ୍ୟ Tunes::reset(ତାର୍କିକ repeat_flag)
{
	// reset pointer
	ଯଦି (!repeat_flag)	{
		_tune = nullptr;
		_next_tune = nullptr;

	} ଅନ୍ୟଥା {
		_tune = _tune_start_ptr;
		_next_tune = _tune;
	}

	// reset music parameter
	_note_length = _default_note_length;
	_note_mode   = _default_note_mode;
	_octave      = _default_octave;
	_tempo       = _default_tempo;
}

Tunes::ControlResult Tunes::set_control(ଅଚଳ tune_control_s &tune_control)
{
	// Sanity check
	ଯଦି (tune_control.tune_id >= _default_tunes_size) {
		ଫେରନ୍ତୁ ControlResult::InvalidTune;
	}

	// Accept new tune or a stop?
	ଯଦି (_tune == nullptr ||  	   // No tune is currently being played
	    tune_control.tune_override ||  // Override interrupts everything
	    _default_tunes_interruptable[_current_tune_id]) {

		// Check if this exact tune is already being played back
		ଯଦି (tune_control.tune_id != static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(TuneID::CUSTOM) &&
		    _tune == _default_tunes[tune_control.tune_id]) {
			ଫେରନ୍ତୁ ControlResult::AlreadyPlaying; // Nothing to do
		}

		// Reset repeat flag. Can jump to true again while tune is being parsed later
		_repeat = ମିଥ୍ୟା;

		// Reset octave, tempo etc.
		reset(_repeat);

		// Volume will remain valid for the entire tune, unless interrupted.
		ଯଦି (tune_control.volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = tune_control.volume;

		} ଅନ୍ୟଥା {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}


		// Special treatment for custom tunes
		ଯଦି (tune_control.tune_id == static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(TuneID::CUSTOM)) {
			using_custom_ସନ୍ଦେଶ = ସତ୍ୟ;
			_frequency        = (unsigned)tune_control.frequency;
			_duration         = (unsigned)tune_control.duration;
			_silence          = (unsigned)tune_control.silence;

		} ଅନ୍ୟଥା {
			using_custom_ସନ୍ଦେଶ = ମିଥ୍ୟା;
			_tune             = _default_tunes[tune_control.tune_id];
			_tune_start_ptr   = _tune;
			_next_tune        = _tune;
		}

		_current_tune_id = tune_control.tune_id;
		ଫେରନ୍ତୁ ControlResult::Success;
	}

	ଫେରନ୍ତୁ ControlResult::WouldInterrupt;
}

ଶୂନ୍ୟ Tunes::set_string(ଅଚଳ ଅକ୍ଷର *ଅଚଳ string, uint8_t volume)
{
	// Only play new tune if nothing is being played currently.
	ଯଦି (_tune == nullptr) {
		// Set tune string the first time.
		_tune           = string;
		_tune_start_ptr = string;
		_next_tune      = _tune;

		ଯଦି (volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = volume;

		} ଅନ୍ୟଥା {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}
	}
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence, uint8_t &volume)
{
	ଯଦି (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		volume = 0;
		ଫେରନ୍ତୁ Tunes::Status::Stop;
	}

	Tunes::Status ret = get_next_note(frequency, duration, silence);

	// Check if note should not be heard -> adjust volume to 0 to be safe.
	ଯଦି (frequency == 0 || duration == 0) {
		volume = 0;

	} ଅନ୍ୟଥା {
		volume = _volume;
	}

	ଫେରନ୍ତୁ ret;
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence)
{
	ଯଦି (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		ଫେରନ୍ତୁ Tunes::Status::Stop;
	}

	// Return the values for frequency and duration if the custom msg was received.
	ଯଦି (using_custom_ସନ୍ଦେଶ) {
		using_custom_ସନ୍ଦେଶ = ମିଥ୍ୟା;
		duration  = _duration;
		frequency = _frequency;
		silence   = _silence;
		ଫେରନ୍ତୁ Tunes::Status::Continue;
	}

	// Make sure we still have a tune.
	ଯଦି ((_next_tune == nullptr) || (_tune == nullptr)) {
		ଫେରନ୍ତୁ tune_error();
	}

	// Parse characters out of the string until we have resolved a note.
	unsigned note = 0;
	unsigned note_length = _note_length;

	ଯେତେବେଳେ (note == 0) {
		// we always need at least one character from the string.
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = next_char();

		ଯଦି (c == 0) {
			silence = 0;
			ଫେରନ୍ତୁ tune_end();
		}

		_next_tune++;

		ବାଛ (c) {

		ସ୍ଥିତି 'V':	// Select volume.
			// Consume volume if specified in tune but ignore
			// MML compatibility
			next_ସଂଖ୍ୟା();
			ଭାଙ୍ଗ;

		ସ୍ଥିତି 'L':	// Select note length.
			_note_length = next_ସଂଖ୍ୟା();

			ଯଦି (_note_length < 1) {
				ଫେରନ୍ତୁ tune_error();
			}

			ଭାଙ୍ଗ;

		ସ୍ଥିତି 'O':	// Select octave.
			_octave = next_ସଂଖ୍ୟା();

			ଯଦି (_octave > 6) {
				_octave = 6;
			}

			ଭାଙ୍ଗ;

		ସ୍ଥିତି '<':	// Decrease octave.
			ଯଦି (_octave > 0) {
				_octave--;
			}

			ଭାଙ୍ଗ;

		ସ୍ଥିତି '>':	// Increase octave.
			ଯଦି (_octave < 6) {
				_octave++;
			}

			ଭାଙ୍ଗ;

		ସ୍ଥିତି 'M':	// Select inter-note gap.
			c = next_char();

			ଯଦି (c == 0) {
				ଫେରନ୍ତୁ tune_error();
			}

			_next_tune++;

			ବାଛ (c) {
			ସ୍ଥିତି 'N':
				_note_mode = NoteMode::NORMAL;
				ଭାଙ୍ଗ;

			ସ୍ଥିତି 'L':
				_note_mode = NoteMode::LEGATO;
				ଭାଙ୍ଗ;

			ସ୍ଥିତି 'S':
				_note_mode = NoteMode::STACCATO;
				ଭାଙ୍ଗ;

			ସ୍ଥିତି 'F':
				_repeat = ମିଥ୍ୟା;
				ଭାଙ୍ଗ;

			ସ୍ଥିତି 'B':
				_repeat = ସତ୍ୟ;
				ଭାଙ୍ଗ;

			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
				ଫେରନ୍ତୁ tune_error();
			}

			ଭାଙ୍ଗ;

		ସ୍ଥିତି 'P':	// Pause for a note length.
		ସ୍ଥିତି 'R':
			frequency = 0;
			duration = 0;
			note_length = next_ସଂଖ୍ୟା();
			silence = rest_duration(note_length, next_dots());
			ଫେରନ୍ତୁ Tunes::Status::Continue;

		ସ୍ଥିତି 'T': {	// Change tempo.
				unsigned nt = next_ସଂଖ୍ୟା();

				ଯଦି ((nt >= 32) && (nt <= 255)) {
					_tempo = nt;

				} ଅନ୍ୟଥା {
					ଫେରନ୍ତୁ tune_error();
				}

				ଭାଙ୍ଗ;
			}

		ସ୍ଥିତି 'N':	// Play an arbitrary note.
			note = next_ସଂଖ୍ୟା();

			ଯଦି (note > 84) {
				ଫେରନ୍ତୁ tune_error();
			}

			ଯଦି (note == 0) {
				// This is a rest - pause for the current note length.
				silence = rest_duration(_note_length, next_dots());
				ଫେରନ୍ତୁ Tunes::Status::Continue;
			}

			ଭାଙ୍ଗ;

		ସ୍ଥିତି 'A'...'G':	// Play a note in the current octave.
			note = _note_tab[c - 'A'] + (_octave * 12) + 1;
			c = next_char();

			ବାଛ (c) {
			ସ୍ଥିତି '#':	// Up a semitone.
			ସ୍ଥିତି '+':
				ଯଦି (note < 84) {
					note++;
				}

				_next_tune++;
				ଭାଙ୍ଗ;

			ସ୍ଥିତି '-':	// Down a semitone.
				ଯଦି (note > 1) {
					note--;
				}

				_next_tune++;
				ଭାଙ୍ଗ;

			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
				// 0 / No next char here is OK.
				ଭାଙ୍ଗ;
			}

			// Shorthand length notation.
			note_length = next_ସଂଖ୍ୟା();

			ଯଦି (note_length == 0) {
				note_length = _note_length;
			}

			ଭାଙ୍ଗ;

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଫେରନ୍ତୁ tune_error();
		}
	}

	// Compute the duration of the note and the following silence (if any).
	duration = note_duration(silence, note_length, next_dots());

	// Compute the note frequency.
	frequency = note_to_frequency(note);
	ଫେରନ୍ତୁ Tunes::Status::Continue;
}

Tunes::Status Tunes::tune_end()
{
	ଯଦି (_tunes_disabled) {
		ଫେରନ୍ତୁ Tunes::Status::Stop;
	}

	// Restore intial parameters.
	reset(_repeat);

	// Stop or restart the tune.
	ଯଦି (_repeat) {
		ଫେରନ୍ତୁ Tunes::Status::Continue;

	} ଅନ୍ୟଥା {
		ଫେରନ୍ତୁ Tunes::Status::Stop;
	}
}

Tunes::Status Tunes::tune_error()
{
	ଯଦି (_tunes_disabled) {
		ଫେରନ୍ତୁ Tunes::Status::Stop;
	}

	// The tune appears to be bad (unexpected EOF, bad character, etc.).
	ଯଦି (_next_tune != nullptr) {
		PX4_WARN("Tune error at: %s", _next_tune);
	}

	_repeat = ମିଥ୍ୟା;	// Don't loop on error.
	reset(_repeat);
	ଫେରନ୍ତୁ Tunes::Status::Error;
}

uint32_t Tunes::note_to_frequency(unsigned note) ଅଚଳ
{
	// Compute the frequency (Hz).
	ଫେରନ୍ତୁ (unsigned)(880.0f * powf(2.0f, ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)note - 46) / 12.0f));
}

unsigned Tunes::note_duration(unsigned &silence, unsigned note_length, unsigned dots) ଅଚଳ
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	ଯଦି (note_length == 0) {
		note_length = 1;
	}

	unsigned note_period = whole_note_period / note_length;

	ବାଛ (_note_mode) {
	ସ୍ଥିତି NoteMode::NORMAL:
		silence = note_period / 8;
		ଭାଙ୍ଗ;

	ସ୍ଥିତି NoteMode::STACCATO:
		silence = note_period / 4;
		ଭାଙ୍ଗ;

	ସ୍ଥିତି NoteMode::LEGATO:
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		silence = 0;
		ଭାଙ୍ଗ;
	}

	note_period -= silence;

	unsigned dot_extension = note_period / 2;

	ଯେତେବେଳେ (dots--) {
		note_period += dot_extension;
		dot_extension /= 2;
	}

	ଫେରନ୍ତୁ note_period;
}

unsigned Tunes::rest_duration(unsigned rest_length, unsigned dots) ଅଚଳ
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	ଯଦି (rest_length == 0) {
		rest_length = 1;
	}

	unsigned rest_period = whole_note_period / rest_length;

	unsigned dot_extension = rest_period / 2;

	ଯେତେବେଳେ (dots--) {
		rest_period += dot_extension;
		dot_extension /= 2;
	}

	ଫେରନ୍ତୁ rest_period;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Tunes::next_char()
{
	ଯେତେବେଳେ (isspace(*_next_tune)) {
		_next_tune++;
	}

	ଫେରନ୍ତୁ toupper(*_next_tune);
}

unsigned Tunes::next_ସଂଖ୍ୟା()
{
	unsigned ସଂଖ୍ୟା = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା next_character;

	ପାଇଁ (;;) {
		next_character = next_char();

		ଯଦି (!isdigit(next_character)) {
			ଫେରନ୍ତୁ ସଂଖ୍ୟା;
		}

		_next_tune++;
		ସଂଖ୍ୟା = (ସଂଖ୍ୟା * 10) + (next_character - '0');
	}
}

unsigned Tunes::next_dots()
{
	unsigned dots = 0;

	ଯେତେବେଳେ (next_char() == '.') {
		_next_tune++;
		dots++;
	}

	ଫେରନ୍ତୁ dots;
}
