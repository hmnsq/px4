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

#અન્તર્ભાવ "tunes.h"

#અન્તર્ભાવ <px4_platform_common/log.h>

#અન્તર્ભાવ <lib/circuit_breaker/circuit_breaker.h>

#અન્તર્ભાવ <math.h>
#અન્તર્ભાવ <ctype.h>
#અન્તર્ભાવ <errno.h>

#define BEAT_TIME_CONVERSION_MS (60 * 1000 * 4)
#define BEAT_TIME_CONVERSION_US (BEAT_TIME_CONVERSION_MS * 1000)
#define BEAT_TIME_CONVERSION    BEAT_TIME_CONVERSION_US

// semitone offsets from C for the characters 'A'-'G'
અચલ uint8_t Tunes::_note_tab[] = {9, 11, 0, 2, 4, 5, 7};

Tunes::Tunes(unsigned default_note_length, NoteMode default_note_mode,
	     unsigned default_octave, unsigned default_tempo):
	_default_note_length(default_note_length),
	_default_note_mode(default_note_mode),
	_default_octave(default_octave),
	_default_tempo(default_tempo)
{
	જો (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_ચાવી)) {
		_tunes_disabled = સાચું;
	}

	reset(ખોટું);
}

શૂન્ય Tunes::reset(તાર્કિક repeat_flag)
{
	// reset pointer
	જો (!repeat_flag)	{
		_tune = nullptr;
		_next_tune = nullptr;

	} નહીંતર {
		_tune = _tune_start_ptr;
		_next_tune = _tune;
	}

	// reset music parameter
	_note_length = _default_note_length;
	_note_mode   = _default_note_mode;
	_octave      = _default_octave;
	_tempo       = _default_tempo;
}

Tunes::ControlResult Tunes::set_control(અચલ tune_control_s &tune_control)
{
	// Sanity check
	જો (tune_control.tune_id >= _default_tunes_size) {
		પાછા ControlResult::InvalidTune;
	}

	// Accept new tune or a stop?
	જો (_tune == nullptr ||  	   // No tune is currently being played
	    tune_control.tune_override ||  // Override interrupts everything
	    _default_tunes_interruptable[_current_tune_id]) {

		// Check if this exact tune is already being played back
		જો (tune_control.tune_id != static_cast<પૂર્ણાંક>(TuneID::CUSTOM) &&
		    _tune == _default_tunes[tune_control.tune_id]) {
			પાછા ControlResult::AlreadyPlaying; // Nothing to do
		}

		// Reset repeat flag. Can jump to true again while tune is being parsed later
		_repeat = ખોટું;

		// Reset octave, tempo etc.
		reset(_repeat);

		// Volume will remain valid for the entire tune, unless interrupted.
		જો (tune_control.volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = tune_control.volume;

		} નહીંતર {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}


		// Special treatment for custom tunes
		જો (tune_control.tune_id == static_cast<પૂર્ણાંક>(TuneID::CUSTOM)) {
			using_custom_સંદેશ = સાચું;
			_frequency        = (unsigned)tune_control.frequency;
			_duration         = (unsigned)tune_control.duration;
			_silence          = (unsigned)tune_control.silence;

		} નહીંતર {
			using_custom_સંદેશ = ખોટું;
			_tune             = _default_tunes[tune_control.tune_id];
			_tune_start_ptr   = _tune;
			_next_tune        = _tune;
		}

		_current_tune_id = tune_control.tune_id;
		પાછા ControlResult::Success;
	}

	પાછા ControlResult::WouldInterrupt;
}

શૂન્ય Tunes::set_string(અચલ અક્ષર *અચલ string, uint8_t volume)
{
	// Only play new tune if nothing is being played currently.
	જો (_tune == nullptr) {
		// Set tune string the first time.
		_tune           = string;
		_tune_start_ptr = string;
		_next_tune      = _tune;

		જો (volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = volume;

		} નહીંતર {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}
	}
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence, uint8_t &volume)
{
	જો (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		volume = 0;
		પાછા Tunes::Status::Stop;
	}

	Tunes::Status ret = get_next_note(frequency, duration, silence);

	// Check if note should not be heard -> adjust volume to 0 to be safe.
	જો (frequency == 0 || duration == 0) {
		volume = 0;

	} નહીંતર {
		volume = _volume;
	}

	પાછા ret;
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence)
{
	જો (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		પાછા Tunes::Status::Stop;
	}

	// Return the values for frequency and duration if the custom msg was received.
	જો (using_custom_સંદેશ) {
		using_custom_સંદેશ = ખોટું;
		duration  = _duration;
		frequency = _frequency;
		silence   = _silence;
		પાછા Tunes::Status::Continue;
	}

	// Make sure we still have a tune.
	જો ((_next_tune == nullptr) || (_tune == nullptr)) {
		પાછા tune_error();
	}

	// Parse characters out of the string until we have resolved a note.
	unsigned note = 0;
	unsigned note_length = _note_length;

	જ્યારે (note == 0) {
		// we always need at least one character from the string.
		પૂર્ણાંક c = next_char();

		જો (c == 0) {
			silence = 0;
			પાછા tune_end();
		}

		_next_tune++;

		પસંદગી (c) {

		સ્થિતિ 'V':	// Select volume.
			// Consume volume if specified in tune but ignore
			// MML compatibility
			next_સંખ્યા();
			તોડો;

		સ્થિતિ 'L':	// Select note length.
			_note_length = next_સંખ્યા();

			જો (_note_length < 1) {
				પાછા tune_error();
			}

			તોડો;

		સ્થિતિ 'O':	// Select octave.
			_octave = next_સંખ્યા();

			જો (_octave > 6) {
				_octave = 6;
			}

			તોડો;

		સ્થિતિ '<':	// Decrease octave.
			જો (_octave > 0) {
				_octave--;
			}

			તોડો;

		સ્થિતિ '>':	// Increase octave.
			જો (_octave < 6) {
				_octave++;
			}

			તોડો;

		સ્થિતિ 'M':	// Select inter-note gap.
			c = next_char();

			જો (c == 0) {
				પાછા tune_error();
			}

			_next_tune++;

			પસંદગી (c) {
			સ્થિતિ 'N':
				_note_mode = NoteMode::NORMAL;
				તોડો;

			સ્થિતિ 'L':
				_note_mode = NoteMode::LEGATO;
				તોડો;

			સ્થિતિ 'S':
				_note_mode = NoteMode::STACCATO;
				તોડો;

			સ્થિતિ 'F':
				_repeat = ખોટું;
				તોડો;

			સ્થિતિ 'B':
				_repeat = સાચું;
				તોડો;

			મૂળભૂત:
				પાછા tune_error();
			}

			તોડો;

		સ્થિતિ 'P':	// Pause for a note length.
		સ્થિતિ 'R':
			frequency = 0;
			duration = 0;
			note_length = next_સંખ્યા();
			silence = rest_duration(note_length, next_dots());
			પાછા Tunes::Status::Continue;

		સ્થિતિ 'T': {	// Change tempo.
				unsigned nt = next_સંખ્યા();

				જો ((nt >= 32) && (nt <= 255)) {
					_tempo = nt;

				} નહીંતર {
					પાછા tune_error();
				}

				તોડો;
			}

		સ્થિતિ 'N':	// Play an arbitrary note.
			note = next_સંખ્યા();

			જો (note > 84) {
				પાછા tune_error();
			}

			જો (note == 0) {
				// This is a rest - pause for the current note length.
				silence = rest_duration(_note_length, next_dots());
				પાછા Tunes::Status::Continue;
			}

			તોડો;

		સ્થિતિ 'A'...'G':	// Play a note in the current octave.
			note = _note_tab[c - 'A'] + (_octave * 12) + 1;
			c = next_char();

			પસંદગી (c) {
			સ્થિતિ '#':	// Up a semitone.
			સ્થિતિ '+':
				જો (note < 84) {
					note++;
				}

				_next_tune++;
				તોડો;

			સ્થિતિ '-':	// Down a semitone.
				જો (note > 1) {
					note--;
				}

				_next_tune++;
				તોડો;

			મૂળભૂત:
				// 0 / No next char here is OK.
				તોડો;
			}

			// Shorthand length notation.
			note_length = next_સંખ્યા();

			જો (note_length == 0) {
				note_length = _note_length;
			}

			તોડો;

		મૂળભૂત:
			પાછા tune_error();
		}
	}

	// Compute the duration of the note and the following silence (if any).
	duration = note_duration(silence, note_length, next_dots());

	// Compute the note frequency.
	frequency = note_to_frequency(note);
	પાછા Tunes::Status::Continue;
}

Tunes::Status Tunes::tune_end()
{
	જો (_tunes_disabled) {
		પાછા Tunes::Status::Stop;
	}

	// Restore intial parameters.
	reset(_repeat);

	// Stop or restart the tune.
	જો (_repeat) {
		પાછા Tunes::Status::Continue;

	} નહીંતર {
		પાછા Tunes::Status::Stop;
	}
}

Tunes::Status Tunes::tune_error()
{
	જો (_tunes_disabled) {
		પાછા Tunes::Status::Stop;
	}

	// The tune appears to be bad (unexpected EOF, bad character, etc.).
	જો (_next_tune != nullptr) {
		PX4_WARN("Tune error at: %s", _next_tune);
	}

	_repeat = ખોટું;	// Don't loop on error.
	reset(_repeat);
	પાછા Tunes::Status::Error;
}

uint32_t Tunes::note_to_frequency(unsigned note) અચલ
{
	// Compute the frequency (Hz).
	પાછા (unsigned)(880.0f * powf(2.0f, ((પૂર્ણાંક)note - 46) / 12.0f));
}

unsigned Tunes::note_duration(unsigned &silence, unsigned note_length, unsigned dots) અચલ
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	જો (note_length == 0) {
		note_length = 1;
	}

	unsigned note_period = whole_note_period / note_length;

	પસંદગી (_note_mode) {
	સ્થિતિ NoteMode::NORMAL:
		silence = note_period / 8;
		તોડો;

	સ્થિતિ NoteMode::STACCATO:
		silence = note_period / 4;
		તોડો;

	સ્થિતિ NoteMode::LEGATO:
	મૂળભૂત:
		silence = 0;
		તોડો;
	}

	note_period -= silence;

	unsigned dot_extension = note_period / 2;

	જ્યારે (dots--) {
		note_period += dot_extension;
		dot_extension /= 2;
	}

	પાછા note_period;
}

unsigned Tunes::rest_duration(unsigned rest_length, unsigned dots) અચલ
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	જો (rest_length == 0) {
		rest_length = 1;
	}

	unsigned rest_period = whole_note_period / rest_length;

	unsigned dot_extension = rest_period / 2;

	જ્યારે (dots--) {
		rest_period += dot_extension;
		dot_extension /= 2;
	}

	પાછા rest_period;
}

પૂર્ણાંક Tunes::next_char()
{
	જ્યારે (isspace(*_next_tune)) {
		_next_tune++;
	}

	પાછા toupper(*_next_tune);
}

unsigned Tunes::next_સંખ્યા()
{
	unsigned સંખ્યા = 0;
	પૂર્ણાંક next_character;

	માટે (;;) {
		next_character = next_char();

		જો (!isdigit(next_character)) {
			પાછા સંખ્યા;
		}

		_next_tune++;
		સંખ્યા = (સંખ્યા * 10) + (next_character - '0');
	}
}

unsigned Tunes::next_dots()
{
	unsigned dots = 0;

	જ્યારે (next_char() == '.') {
		_next_tune++;
		dots++;
	}

	પાછા dots;
}
