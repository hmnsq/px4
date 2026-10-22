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

#ವ್ಯಾಖ್ಯಾನಿಸು BEAT_ಸಮಯ_CONVERSION_MS (60 * 1000 * 4)
#ವ್ಯಾಖ್ಯಾನಿಸು BEAT_ಸಮಯ_CONVERSION_US (BEAT_ಸಮಯ_CONVERSION_MS * 1000)
#ವ್ಯಾಖ್ಯಾನಿಸು BEAT_ಸಮಯ_CONVERSION    BEAT_ಸಮಯ_CONVERSION_US

// semitone offsets from C for the characters 'A'-'G'
ಸ್ಥಿರಾಂಕ uint8_t Tunes::_note_tab[] = {9, 11, 0, 2, 4, 5, 7};

Tunes::Tunes(ಚಿಹ್ನೆರಹಿತ default_note_ಉದ್ದಳತೆ, Note_ಬಹುಲಕ default_note_ಬಹುಲಕ,
	     ಚಿಹ್ನೆರಹಿತ default_octave, ಚಿಹ್ನೆರಹಿತ default_tempo):
	default_note_ಉದ್ದಳತೆ_2(default_note_ಉದ್ದಳತೆ),
	default_note_ಬಹುಲಕ_2(default_note_ಬಹುಲಕ),
	_default_octave(default_octave),
	_default_tempo(default_tempo)
{
	ಆದರೆ (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_ಕೀಲಿ)) {
		_tunes_disabled = ನಿಜ;
	}

	reset(ಸುಳ್ಳು);
}

ಶೂನ್ಯ Tunes::reset(ತಾರ್ಕಿಕ repeat_flag)
{
	// reset pointer
	ಆದರೆ (!repeat_flag)	{
		_tune = ಶೂನ್ಯಸೂಚಕ;
		ಮುಂದಿನ_tune = ಶೂನ್ಯಸೂಚಕ;

	} ಇಲ್ಲದಿದ್ದರೆ {
		_tune = tune_ಆರಂಭ_ptr;
		ಮುಂದಿನ_tune = _tune;
	}

	// reset music parameter
	note_ಉದ್ದಳತೆ = default_note_ಉದ್ದಳತೆ_2;
	note_ಬಹುಲಕ   = default_note_ಬಹುಲಕ_2;
	_octave      = _default_octave;
	_tempo       = _default_tempo;
}

Tunes::Control_ಫಲಿತಾಂಶ Tunes::ಗಣ_control(ಸ್ಥಿರಾಂಕ tune_control_s &tune_control)
{
	// Sanity check
	ಆದರೆ (tune_control.tune_id >= default_tunes_ಗಾತ್ರ) {
		ಹಿಂತಿರುಗಿ Control_ಫಲಿತಾಂಶ::InvalidTune;
	}

	// Accept new tune or a stop?
	ಆದರೆ (_tune == ಶೂನ್ಯಸೂಚಕ ||  	   // No tune is currently being played
	    tune_control.tune_override ||  // Override interrupts everything
	    _default_tunes_interruptable[_current_tune_id]) {

		// Check if this exact tune is already being played back
		ಆದರೆ (tune_control.tune_id != static_cast<ಪೂರ್ಣಾಂಕ>(TuneID::CUSTOM) &&
		    _tune == _default_tunes[tune_control.tune_id]) {
			ಹಿಂತಿರುಗಿ Control_ಫಲಿತಾಂಶ::AlreadyPlaying; // Nothing to do
		}

		// Reset repeat flag. Can jump to true again while tune is being parsed later
		_repeat = ಸುಳ್ಳು;

		// Reset octave, tempo etc.
		reset(_repeat);

		// Volume will remain valid for the entire tune, unless interrupted.
		ಆದರೆ (tune_control.volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = tune_control.volume;

		} ಇಲ್ಲದಿದ್ದರೆ {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}


		// Special treatment for custom tunes
		ಆದರೆ (tune_control.tune_id == static_cast<ಪೂರ್ಣಾಂಕ>(TuneID::CUSTOM)) {
			using_custom_ಸಂದೇಶ = ನಿಜ;
			_frequency        = (ಚಿಹ್ನೆರಹಿತ)tune_control.frequency;
			_duration         = (ಚಿಹ್ನೆರಹಿತ)tune_control.duration;
			_silence          = (ಚಿಹ್ನೆರಹಿತ)tune_control.silence;

		} ಇಲ್ಲದಿದ್ದರೆ {
			using_custom_ಸಂದೇಶ = ಸುಳ್ಳು;
			_tune             = _default_tunes[tune_control.tune_id];
			tune_ಆರಂಭ_ptr   = _tune;
			ಮುಂದಿನ_tune        = _tune;
		}

		_current_tune_id = tune_control.tune_id;
		ಹಿಂತಿರುಗಿ Control_ಫಲಿತಾಂಶ::Success;
	}

	ಹಿಂತಿರುಗಿ Control_ಫಲಿತಾಂಶ::Would_ಅಡಚಣೆ;
}

ಶೂನ್ಯ Tunes::ಗಣ_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ string, uint8_t volume)
{
	// Only play new tune if nothing is being played currently.
	ಆದರೆ (_tune == ಶೂನ್ಯಸೂಚಕ) {
		// Set tune string the first time.
		_tune           = string;
		tune_ಆರಂಭ_ptr = string;
		ಮುಂದಿನ_tune      = _tune;

		ಆದರೆ (volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = volume;

		} ಇಲ್ಲದಿದ್ದರೆ {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}
	}
}

Tunes::ಸ್ಥಿತಿ_3 Tunes::get_ಮುಂದಿನ_note(ಚಿಹ್ನೆರಹಿತ &frequency, ಚಿಹ್ನೆರಹಿತ &duration, ಚಿಹ್ನೆರಹಿತ &silence, uint8_t &volume)
{
	ಆದರೆ (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		volume = 0;
		ಹಿಂತಿರುಗಿ Tunes::ಸ್ಥಿತಿ_3::Stop;
	}

	Tunes::ಸ್ಥಿತಿ_3 ret = get_ಮುಂದಿನ_note(frequency, duration, silence);

	// Check if note should not be heard -> adjust volume to 0 to be safe.
	ಆದರೆ (frequency == 0 || duration == 0) {
		volume = 0;

	} ಇಲ್ಲದಿದ್ದರೆ {
		volume = _volume;
	}

	ಹಿಂತಿರುಗಿ ret;
}

Tunes::ಸ್ಥಿತಿ_3 Tunes::get_ಮುಂದಿನ_note(ಚಿಹ್ನೆರಹಿತ &frequency, ಚಿಹ್ನೆರಹಿತ &duration, ಚಿಹ್ನೆರಹಿತ &silence)
{
	ಆದರೆ (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		ಹಿಂತಿರುಗಿ Tunes::ಸ್ಥಿತಿ_3::Stop;
	}

	// Return the values for frequency and duration if the custom msg was received.
	ಆದರೆ (using_custom_ಸಂದೇಶ) {
		using_custom_ಸಂದೇಶ = ಸುಳ್ಳು;
		duration  = _duration;
		frequency = _frequency;
		silence   = _silence;
		ಹಿಂತಿರುಗಿ Tunes::ಸ್ಥಿತಿ_3::Continue;
	}

	// Make sure we still have a tune.
	ಆದರೆ ((ಮುಂದಿನ_tune == ಶೂನ್ಯಸೂಚಕ) || (_tune == ಶೂನ್ಯಸೂಚಕ)) {
		ಹಿಂತಿರುಗಿ tune_error();
	}

	// Parse characters out of the string until we have resolved a note.
	ಚಿಹ್ನೆರಹಿತ note = 0;
	ಚಿಹ್ನೆರಹಿತ note_ಉದ್ದಳತೆ_2 = note_ಉದ್ದಳತೆ;

	ಆಗ (note == 0) {
		// we always need at least one character from the string.
		ಪೂರ್ಣಾಂಕ c = ಮುಂದಿನ_char();

		ಆದರೆ (c == 0) {
			silence = 0;
			ಹಿಂತಿರುಗಿ tune_ಕೊನೆ();
		}

		ಮುಂದಿನ_tune++;

		ಆಯ್ಕೆ (c) {

		ಸ್ಥಿತಿ 'V':	// Select volume.
			// Consume volume if specified in tune but ignore
			// MML compatibility
			ಮುಂದಿನ_ಸಂಖ್ಯೆ();
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'L':	// Select note length.
			note_ಉದ್ದಳತೆ = ಮುಂದಿನ_ಸಂಖ್ಯೆ();

			ಆದರೆ (note_ಉದ್ದಳತೆ < 1) {
				ಹಿಂತಿರುಗಿ tune_error();
			}

			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'O':	// Select octave.
			_octave = ಮುಂದಿನ_ಸಂಖ್ಯೆ();

			ಆದರೆ (_octave > 6) {
				_octave = 6;
			}

			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ '<':	// Decrease octave.
			ಆದರೆ (_octave > 0) {
				_octave--;
			}

			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ '>':	// Increase octave.
			ಆದರೆ (_octave < 6) {
				_octave++;
			}

			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'M':	// Select inter-note gap.
			c = ಮುಂದಿನ_char();

			ಆದರೆ (c == 0) {
				ಹಿಂತಿರುಗಿ tune_error();
			}

			ಮುಂದಿನ_tune++;

			ಆಯ್ಕೆ (c) {
			ಸ್ಥಿತಿ 'N':
				note_ಬಹುಲಕ = Note_ಬಹುಲಕ::ಸಾಮಾನ್ಯ;
				ನಿಲ್ಲಿಸು;

			ಸ್ಥಿತಿ 'L':
				note_ಬಹುಲಕ = Note_ಬಹುಲಕ::LEGATO;
				ನಿಲ್ಲಿಸು;

			ಸ್ಥಿತಿ 'S':
				note_ಬಹುಲಕ = Note_ಬಹುಲಕ::STACCATO;
				ನಿಲ್ಲಿಸು;

			ಸ್ಥಿತಿ 'F':
				_repeat = ಸುಳ್ಳು;
				ನಿಲ್ಲಿಸು;

			ಸ್ಥಿತಿ 'B':
				_repeat = ನಿಜ;
				ನಿಲ್ಲಿಸು;

			ಪೂರ್ವನಿಯೋಜಿತ:
				ಹಿಂತಿರುಗಿ tune_error();
			}

			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'P':	// Pause for a note length.
		ಸ್ಥಿತಿ 'R':
			frequency = 0;
			duration = 0;
			note_ಉದ್ದಳತೆ_2 = ಮುಂದಿನ_ಸಂಖ್ಯೆ();
			silence = rest_duration(note_ಉದ್ದಳತೆ_2, ಮುಂದಿನ_dots());
			ಹಿಂತಿರುಗಿ Tunes::ಸ್ಥಿತಿ_3::Continue;

		ಸ್ಥಿತಿ 'T': {	// Change tempo.
				ಚಿಹ್ನೆರಹಿತ nt = ಮುಂದಿನ_ಸಂಖ್ಯೆ();

				ಆದರೆ ((nt >= 32) && (nt <= 255)) {
					_tempo = nt;

				} ಇಲ್ಲದಿದ್ದರೆ {
					ಹಿಂತಿರುಗಿ tune_error();
				}

				ನಿಲ್ಲಿಸು;
			}

		ಸ್ಥಿತಿ 'N':	// Play an arbitrary note.
			note = ಮುಂದಿನ_ಸಂಖ್ಯೆ();

			ಆದರೆ (note > 84) {
				ಹಿಂತಿರುಗಿ tune_error();
			}

			ಆದರೆ (note == 0) {
				// This is a rest - pause for the current note length.
				silence = rest_duration(note_ಉದ್ದಳತೆ, ಮುಂದಿನ_dots());
				ಹಿಂತಿರುಗಿ Tunes::ಸ್ಥಿತಿ_3::Continue;
			}

			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'A'...'G':	// Play a note in the current octave.
			note = _note_tab[c - 'A'] + (_octave * 12) + 1;
			c = ಮುಂದಿನ_char();

			ಆಯ್ಕೆ (c) {
			ಸ್ಥಿತಿ '#':	// Up a semitone.
			ಸ್ಥಿತಿ '+':
				ಆದರೆ (note < 84) {
					note++;
				}

				ಮುಂದಿನ_tune++;
				ನಿಲ್ಲಿಸು;

			ಸ್ಥಿತಿ '-':	// Down a semitone.
				ಆದರೆ (note > 1) {
					note--;
				}

				ಮುಂದಿನ_tune++;
				ನಿಲ್ಲಿಸು;

			ಪೂರ್ವನಿಯೋಜಿತ:
				// 0 / No next char here is OK.
				ನಿಲ್ಲಿಸು;
			}

			// Shorthand length notation.
			note_ಉದ್ದಳತೆ_2 = ಮುಂದಿನ_ಸಂಖ್ಯೆ();

			ಆದರೆ (note_ಉದ್ದಳತೆ_2 == 0) {
				note_ಉದ್ದಳತೆ_2 = note_ಉದ್ದಳತೆ;
			}

			ನಿಲ್ಲಿಸು;

		ಪೂರ್ವನಿಯೋಜಿತ:
			ಹಿಂತಿರುಗಿ tune_error();
		}
	}

	// Compute the duration of the note and the following silence (if any).
	duration = note_duration(silence, note_ಉದ್ದಳತೆ_2, ಮುಂದಿನ_dots());

	// Compute the note frequency.
	frequency = note_ಗಮ್ಯ_frequency(note);
	ಹಿಂತಿರುಗಿ Tunes::ಸ್ಥಿತಿ_3::Continue;
}

Tunes::ಸ್ಥಿತಿ_3 Tunes::tune_ಕೊನೆ()
{
	ಆದರೆ (_tunes_disabled) {
		ಹಿಂತಿರುಗಿ Tunes::ಸ್ಥಿತಿ_3::Stop;
	}

	// Restore intial parameters.
	reset(_repeat);

	// Stop or restart the tune.
	ಆದರೆ (_repeat) {
		ಹಿಂತಿರುಗಿ Tunes::ಸ್ಥಿತಿ_3::Continue;

	} ಇಲ್ಲದಿದ್ದರೆ {
		ಹಿಂತಿರುಗಿ Tunes::ಸ್ಥಿತಿ_3::Stop;
	}
}

Tunes::ಸ್ಥಿತಿ_3 Tunes::tune_error()
{
	ಆದರೆ (_tunes_disabled) {
		ಹಿಂತಿರುಗಿ Tunes::ಸ್ಥಿತಿ_3::Stop;
	}

	// The tune appears to be bad (unexpected EOF, bad character, etc.).
	ಆದರೆ (ಮುಂದಿನ_tune != ಶೂನ್ಯಸೂಚಕ) {
		PX4_WARN("Tune error at: %s", ಮುಂದಿನ_tune);
	}

	_repeat = ಸುಳ್ಳು;	// Don't loop on error.
	reset(_repeat);
	ಹಿಂತಿರುಗಿ Tunes::ಸ್ಥಿತಿ_3::Error;
}

uint32_t Tunes::note_ಗಮ್ಯ_frequency(ಚಿಹ್ನೆರಹಿತ note) ಸ್ಥಿರಾಂಕ
{
	// Compute the frequency (Hz).
	ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ)(880.0f * powf(2.0f, ((ಪೂರ್ಣಾಂಕ)note - 46) / 12.0f));
}

ಚಿಹ್ನೆರಹಿತ Tunes::note_duration(ಚಿಹ್ನೆರಹಿತ &silence, ಚಿಹ್ನೆರಹಿತ note_ಉದ್ದಳತೆ_2, ಚಿಹ್ನೆರಹಿತ dots) ಸ್ಥಿರಾಂಕ
{
	ಚಿಹ್ನೆರಹಿತ ಸಂಪೂರ್ಣ_note_period = BEAT_ಸಮಯ_CONVERSION / _tempo;

	ಆದರೆ (note_ಉದ್ದಳತೆ_2 == 0) {
		note_ಉದ್ದಳತೆ_2 = 1;
	}

	ಚಿಹ್ನೆರಹಿತ note_period = ಸಂಪೂರ್ಣ_note_period / note_ಉದ್ದಳತೆ_2;

	ಆಯ್ಕೆ (note_ಬಹುಲಕ) {
	ಸ್ಥಿತಿ Note_ಬಹುಲಕ::ಸಾಮಾನ್ಯ:
		silence = note_period / 8;
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Note_ಬಹುಲಕ::STACCATO:
		silence = note_period / 4;
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Note_ಬಹುಲಕ::LEGATO:
	ಪೂರ್ವನಿಯೋಜಿತ:
		silence = 0;
		ನಿಲ್ಲಿಸು;
	}

	note_period -= silence;

	ಚಿಹ್ನೆರಹಿತ dot_extension = note_period / 2;

	ಆಗ (dots--) {
		note_period += dot_extension;
		dot_extension /= 2;
	}

	ಹಿಂತಿರುಗಿ note_period;
}

ಚಿಹ್ನೆರಹಿತ Tunes::rest_duration(ಚಿಹ್ನೆರಹಿತ rest_ಉದ್ದಳತೆ, ಚಿಹ್ನೆರಹಿತ dots) ಸ್ಥಿರಾಂಕ
{
	ಚಿಹ್ನೆರಹಿತ ಸಂಪೂರ್ಣ_note_period = BEAT_ಸಮಯ_CONVERSION / _tempo;

	ಆದರೆ (rest_ಉದ್ದಳತೆ == 0) {
		rest_ಉದ್ದಳತೆ = 1;
	}

	ಚಿಹ್ನೆರಹಿತ rest_period = ಸಂಪೂರ್ಣ_note_period / rest_ಉದ್ದಳತೆ;

	ಚಿಹ್ನೆರಹಿತ dot_extension = rest_period / 2;

	ಆಗ (dots--) {
		rest_period += dot_extension;
		dot_extension /= 2;
	}

	ಹಿಂತಿರುಗಿ rest_period;
}

ಪೂರ್ಣಾಂಕ Tunes::ಮುಂದಿನ_char()
{
	ಆಗ (ಖಾಲಿಯೇ(*ಮುಂದಿನ_tune)) {
		ಮುಂದಿನ_tune++;
	}

	ಹಿಂತಿರುಗಿ ದೊಡ್ಡ_ಅಕ್ಷರ(*ಮುಂದಿನ_tune);
}

ಚಿಹ್ನೆರಹಿತ Tunes::ಮುಂದಿನ_ಸಂಖ್ಯೆ()
{
	ಚಿಹ್ನೆರಹಿತ ಸಂಖ್ಯೆ = 0;
	ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_character;

	ಗಾಗಿ (;;) {
		ಮುಂದಿನ_character = ಮುಂದಿನ_char();

		ಆದರೆ (!ಅಂಕಿಯೇ(ಮುಂದಿನ_character)) {
			ಹಿಂತಿರುಗಿ ಸಂಖ್ಯೆ;
		}

		ಮುಂದಿನ_tune++;
		ಸಂಖ್ಯೆ = (ಸಂಖ್ಯೆ * 10) + (ಮುಂದಿನ_character - '0');
	}
}

ಚಿಹ್ನೆರಹಿತ Tunes::ಮುಂದಿನ_dots()
{
	ಚಿಹ್ನೆರಹಿತ dots = 0;

	ಆಗ (ಮುಂದಿನ_char() == '.') {
		ಮುಂದಿನ_tune++;
		dots++;
	}

	ಹಿಂತಿರುಗಿ dots;
}
