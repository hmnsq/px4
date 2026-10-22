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

#ਅਨ੍ਤਰ੍ਭਾਵ "tunes.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/log.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <lib/circuit_breaker/circuit_breaker.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <ctype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BEAT_ਸਮਾਂ_CONVERSION_MS (60 * 1000 * 4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BEAT_ਸਮਾਂ_CONVERSION_US (BEAT_ਸਮਾਂ_CONVERSION_MS * 1000)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BEAT_ਸਮਾਂ_CONVERSION    BEAT_ਸਮਾਂ_CONVERSION_US

// semitone offsets from C for the characters 'A'-'G'
ਅਚਲ uint8_t Tunes::_note_tab[] = {9, 11, 0, 2, 4, 5, 7};

Tunes::Tunes(ਬਿਨਾਨਿਸ਼ਾਨੀ default_note_ਲੰਮਾਈ, Note_ਬਹੁਲਕ default_note_ਬਹੁਲਕ,
	     ਬਿਨਾਨਿਸ਼ਾਨੀ default_octave, ਬਿਨਾਨਿਸ਼ਾਨੀ default_tempo):
	default_note_ਲੰਮਾਈ_2(default_note_ਲੰਮਾਈ),
	default_note_ਬਹੁਲਕ_2(default_note_ਬਹੁਲਕ),
	_default_octave(default_octave),
	_default_tempo(default_tempo)
{
	ਜੇ (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_ਕੁੰਜੀ)) {
		_tunes_disabled = ਸੱਚ;
	}

	reset(ਝੂਠ);
}

ਖਾਲੀ Tunes::reset(ਤਾਰਕਿਕ repeat_flag)
{
	// reset pointer
	ਜੇ (!repeat_flag)	{
		_tune = ਖਾਲੀ_ਸੂਚਕ;
		ਅਗਲਾ_tune = ਖਾਲੀ_ਸੂਚਕ;

	} ਨਹੀਂਤਾਂ {
		_tune = tune_ਸ਼ੁਰੂ_ptr;
		ਅਗਲਾ_tune = _tune;
	}

	// reset music parameter
	note_ਲੰਮਾਈ = default_note_ਲੰਮਾਈ_2;
	note_ਬਹੁਲਕ   = default_note_ਬਹੁਲਕ_2;
	_octave      = _default_octave;
	_tempo       = _default_tempo;
}

Tunes::Control_ਨਤੀਜਾ Tunes::ਸੈੱਟ_control(ਅਚਲ tune_control_s &tune_control)
{
	// Sanity check
	ਜੇ (tune_control.tune_id >= default_tunes_ਮਾਪ) {
		ਵਾਪਸ Control_ਨਤੀਜਾ::InvalidTune;
	}

	// Accept new tune or a stop?
	ਜੇ (_tune == ਖਾਲੀ_ਸੂਚਕ ||  	   // No tune is currently being played
	    tune_control.tune_override ||  // Override interrupts everything
	    _default_tunes_interruptable[_current_tune_id]) {

		// Check if this exact tune is already being played back
		ਜੇ (tune_control.tune_id != static_cast<ਪੂਰਨ>(TuneID::CUSTOM) &&
		    _tune == _default_tunes[tune_control.tune_id]) {
			ਵਾਪਸ Control_ਨਤੀਜਾ::AlreadyPlaying; // Nothing to do
		}

		// Reset repeat flag. Can jump to true again while tune is being parsed later
		_repeat = ਝੂਠ;

		// Reset octave, tempo etc.
		reset(_repeat);

		// Volume will remain valid for the entire tune, unless interrupted.
		ਜੇ (tune_control.volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = tune_control.volume;

		} ਨਹੀਂਤਾਂ {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}


		// Special treatment for custom tunes
		ਜੇ (tune_control.tune_id == static_cast<ਪੂਰਨ>(TuneID::CUSTOM)) {
			using_custom_ਸੁਨੇਹਾ = ਸੱਚ;
			_frequency        = (ਬਿਨਾਨਿਸ਼ਾਨੀ)tune_control.frequency;
			_duration         = (ਬਿਨਾਨਿਸ਼ਾਨੀ)tune_control.duration;
			_silence          = (ਬਿਨਾਨਿਸ਼ਾਨੀ)tune_control.silence;

		} ਨਹੀਂਤਾਂ {
			using_custom_ਸੁਨੇਹਾ = ਝੂਠ;
			_tune             = _default_tunes[tune_control.tune_id];
			tune_ਸ਼ੁਰੂ_ptr   = _tune;
			ਅਗਲਾ_tune        = _tune;
		}

		_current_tune_id = tune_control.tune_id;
		ਵਾਪਸ Control_ਨਤੀਜਾ::Success;
	}

	ਵਾਪਸ Control_ਨਤੀਜਾ::Would_ਰੁਕਾਵਟ;
}

ਖਾਲੀ Tunes::ਸੈੱਟ_string(ਅਚਲ ਅੱਖਰ *ਅਚਲ string, uint8_t volume)
{
	// Only play new tune if nothing is being played currently.
	ਜੇ (_tune == ਖਾਲੀ_ਸੂਚਕ) {
		// Set tune string the first time.
		_tune           = string;
		tune_ਸ਼ੁਰੂ_ptr = string;
		ਅਗਲਾ_tune      = _tune;

		ਜੇ (volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = volume;

		} ਨਹੀਂਤਾਂ {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}
	}
}

Tunes::ਸਥਿਤੀ_2 Tunes::get_ਅਗਲਾ_note(ਬਿਨਾਨਿਸ਼ਾਨੀ &frequency, ਬਿਨਾਨਿਸ਼ਾਨੀ &duration, ਬਿਨਾਨਿਸ਼ਾਨੀ &silence, uint8_t &volume)
{
	ਜੇ (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		volume = 0;
		ਵਾਪਸ Tunes::ਸਥਿਤੀ_2::Stop;
	}

	Tunes::ਸਥਿਤੀ_2 ret = get_ਅਗਲਾ_note(frequency, duration, silence);

	// Check if note should not be heard -> adjust volume to 0 to be safe.
	ਜੇ (frequency == 0 || duration == 0) {
		volume = 0;

	} ਨਹੀਂਤਾਂ {
		volume = _volume;
	}

	ਵਾਪਸ ret;
}

Tunes::ਸਥਿਤੀ_2 Tunes::get_ਅਗਲਾ_note(ਬਿਨਾਨਿਸ਼ਾਨੀ &frequency, ਬਿਨਾਨਿਸ਼ਾਨੀ &duration, ਬਿਨਾਨਿਸ਼ਾਨੀ &silence)
{
	ਜੇ (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		ਵਾਪਸ Tunes::ਸਥਿਤੀ_2::Stop;
	}

	// Return the values for frequency and duration if the custom msg was received.
	ਜੇ (using_custom_ਸੁਨੇਹਾ) {
		using_custom_ਸੁਨੇਹਾ = ਝੂਠ;
		duration  = _duration;
		frequency = _frequency;
		silence   = _silence;
		ਵਾਪਸ Tunes::ਸਥਿਤੀ_2::Continue;
	}

	// Make sure we still have a tune.
	ਜੇ ((ਅਗਲਾ_tune == ਖਾਲੀ_ਸੂਚਕ) || (_tune == ਖਾਲੀ_ਸੂਚਕ)) {
		ਵਾਪਸ tune_error();
	}

	// Parse characters out of the string until we have resolved a note.
	ਬਿਨਾਨਿਸ਼ਾਨੀ note = 0;
	ਬਿਨਾਨਿਸ਼ਾਨੀ note_ਲੰਮਾਈ_2 = note_ਲੰਮਾਈ;

	ਜਦੋਂ (note == 0) {
		// we always need at least one character from the string.
		ਪੂਰਨ c = ਅਗਲਾ_char();

		ਜੇ (c == 0) {
			silence = 0;
			ਵਾਪਸ tune_ਸਿਰਾ();
		}

		ਅਗਲਾ_tune++;

		ਚੋਣ (c) {

		ਹਾਲਤ 'V':	// Select volume.
			// Consume volume if specified in tune but ignore
			// MML compatibility
			ਅਗਲਾ_ਸੰਖਿਆ();
			ਰੋਕੋ;

		ਹਾਲਤ 'L':	// Select note length.
			note_ਲੰਮਾਈ = ਅਗਲਾ_ਸੰਖਿਆ();

			ਜੇ (note_ਲੰਮਾਈ < 1) {
				ਵਾਪਸ tune_error();
			}

			ਰੋਕੋ;

		ਹਾਲਤ 'O':	// Select octave.
			_octave = ਅਗਲਾ_ਸੰਖਿਆ();

			ਜੇ (_octave > 6) {
				_octave = 6;
			}

			ਰੋਕੋ;

		ਹਾਲਤ '<':	// Decrease octave.
			ਜੇ (_octave > 0) {
				_octave--;
			}

			ਰੋਕੋ;

		ਹਾਲਤ '>':	// Increase octave.
			ਜੇ (_octave < 6) {
				_octave++;
			}

			ਰੋਕੋ;

		ਹਾਲਤ 'M':	// Select inter-note gap.
			c = ਅਗਲਾ_char();

			ਜੇ (c == 0) {
				ਵਾਪਸ tune_error();
			}

			ਅਗਲਾ_tune++;

			ਚੋਣ (c) {
			ਹਾਲਤ 'N':
				note_ਬਹੁਲਕ = Note_ਬਹੁਲਕ::ਸਾਧਾਰਨ;
				ਰੋਕੋ;

			ਹਾਲਤ 'L':
				note_ਬਹੁਲਕ = Note_ਬਹੁਲਕ::LEGATO;
				ਰੋਕੋ;

			ਹਾਲਤ 'S':
				note_ਬਹੁਲਕ = Note_ਬਹੁਲਕ::STACCATO;
				ਰੋਕੋ;

			ਹਾਲਤ 'F':
				_repeat = ਝੂਠ;
				ਰੋਕੋ;

			ਹਾਲਤ 'B':
				_repeat = ਸੱਚ;
				ਰੋਕੋ;

			ਮੂਲ:
				ਵਾਪਸ tune_error();
			}

			ਰੋਕੋ;

		ਹਾਲਤ 'P':	// Pause for a note length.
		ਹਾਲਤ 'R':
			frequency = 0;
			duration = 0;
			note_ਲੰਮਾਈ_2 = ਅਗਲਾ_ਸੰਖਿਆ();
			silence = rest_duration(note_ਲੰਮਾਈ_2, ਅਗਲਾ_dots());
			ਵਾਪਸ Tunes::ਸਥਿਤੀ_2::Continue;

		ਹਾਲਤ 'T': {	// Change tempo.
				ਬਿਨਾਨਿਸ਼ਾਨੀ nt = ਅਗਲਾ_ਸੰਖਿਆ();

				ਜੇ ((nt >= 32) && (nt <= 255)) {
					_tempo = nt;

				} ਨਹੀਂਤਾਂ {
					ਵਾਪਸ tune_error();
				}

				ਰੋਕੋ;
			}

		ਹਾਲਤ 'N':	// Play an arbitrary note.
			note = ਅਗਲਾ_ਸੰਖਿਆ();

			ਜੇ (note > 84) {
				ਵਾਪਸ tune_error();
			}

			ਜੇ (note == 0) {
				// This is a rest - pause for the current note length.
				silence = rest_duration(note_ਲੰਮਾਈ, ਅਗਲਾ_dots());
				ਵਾਪਸ Tunes::ਸਥਿਤੀ_2::Continue;
			}

			ਰੋਕੋ;

		ਹਾਲਤ 'A'...'G':	// Play a note in the current octave.
			note = _note_tab[c - 'A'] + (_octave * 12) + 1;
			c = ਅਗਲਾ_char();

			ਚੋਣ (c) {
			ਹਾਲਤ '#':	// Up a semitone.
			ਹਾਲਤ '+':
				ਜੇ (note < 84) {
					note++;
				}

				ਅਗਲਾ_tune++;
				ਰੋਕੋ;

			ਹਾਲਤ '-':	// Down a semitone.
				ਜੇ (note > 1) {
					note--;
				}

				ਅਗਲਾ_tune++;
				ਰੋਕੋ;

			ਮੂਲ:
				// 0 / No next char here is OK.
				ਰੋਕੋ;
			}

			// Shorthand length notation.
			note_ਲੰਮਾਈ_2 = ਅਗਲਾ_ਸੰਖਿਆ();

			ਜੇ (note_ਲੰਮਾਈ_2 == 0) {
				note_ਲੰਮਾਈ_2 = note_ਲੰਮਾਈ;
			}

			ਰੋਕੋ;

		ਮੂਲ:
			ਵਾਪਸ tune_error();
		}
	}

	// Compute the duration of the note and the following silence (if any).
	duration = note_duration(silence, note_ਲੰਮਾਈ_2, ਅਗਲਾ_dots());

	// Compute the note frequency.
	frequency = note_ਮੰਜ਼ਿਲ_frequency(note);
	ਵਾਪਸ Tunes::ਸਥਿਤੀ_2::Continue;
}

Tunes::ਸਥਿਤੀ_2 Tunes::tune_ਸਿਰਾ()
{
	ਜੇ (_tunes_disabled) {
		ਵਾਪਸ Tunes::ਸਥਿਤੀ_2::Stop;
	}

	// Restore intial parameters.
	reset(_repeat);

	// Stop or restart the tune.
	ਜੇ (_repeat) {
		ਵਾਪਸ Tunes::ਸਥਿਤੀ_2::Continue;

	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ Tunes::ਸਥਿਤੀ_2::Stop;
	}
}

Tunes::ਸਥਿਤੀ_2 Tunes::tune_error()
{
	ਜੇ (_tunes_disabled) {
		ਵਾਪਸ Tunes::ਸਥਿਤੀ_2::Stop;
	}

	// The tune appears to be bad (unexpected EOF, bad character, etc.).
	ਜੇ (ਅਗਲਾ_tune != ਖਾਲੀ_ਸੂਚਕ) {
		PX4_WARN("Tune error at: %s", ਅਗਲਾ_tune);
	}

	_repeat = ਝੂਠ;	// Don't loop on error.
	reset(_repeat);
	ਵਾਪਸ Tunes::ਸਥਿਤੀ_2::Error;
}

uint32_t Tunes::note_ਮੰਜ਼ਿਲ_frequency(ਬਿਨਾਨਿਸ਼ਾਨੀ note) ਅਚਲ
{
	// Compute the frequency (Hz).
	ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ)(880.0f * powf(2.0f, ((ਪੂਰਨ)note - 46) / 12.0f));
}

ਬਿਨਾਨਿਸ਼ਾਨੀ Tunes::note_duration(ਬਿਨਾਨਿਸ਼ਾਨੀ &silence, ਬਿਨਾਨਿਸ਼ਾਨੀ note_ਲੰਮਾਈ_2, ਬਿਨਾਨਿਸ਼ਾਨੀ dots) ਅਚਲ
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਾਰਾ_note_period = BEAT_ਸਮਾਂ_CONVERSION / _tempo;

	ਜੇ (note_ਲੰਮਾਈ_2 == 0) {
		note_ਲੰਮਾਈ_2 = 1;
	}

	ਬਿਨਾਨਿਸ਼ਾਨੀ note_period = ਸਾਰਾ_note_period / note_ਲੰਮਾਈ_2;

	ਚੋਣ (note_ਬਹੁਲਕ) {
	ਹਾਲਤ Note_ਬਹੁਲਕ::ਸਾਧਾਰਨ:
		silence = note_period / 8;
		ਰੋਕੋ;

	ਹਾਲਤ Note_ਬਹੁਲਕ::STACCATO:
		silence = note_period / 4;
		ਰੋਕੋ;

	ਹਾਲਤ Note_ਬਹੁਲਕ::LEGATO:
	ਮੂਲ:
		silence = 0;
		ਰੋਕੋ;
	}

	note_period -= silence;

	ਬਿਨਾਨਿਸ਼ਾਨੀ dot_extension = note_period / 2;

	ਜਦੋਂ (dots--) {
		note_period += dot_extension;
		dot_extension /= 2;
	}

	ਵਾਪਸ note_period;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ Tunes::rest_duration(ਬਿਨਾਨਿਸ਼ਾਨੀ rest_ਲੰਮਾਈ, ਬਿਨਾਨਿਸ਼ਾਨੀ dots) ਅਚਲ
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਾਰਾ_note_period = BEAT_ਸਮਾਂ_CONVERSION / _tempo;

	ਜੇ (rest_ਲੰਮਾਈ == 0) {
		rest_ਲੰਮਾਈ = 1;
	}

	ਬਿਨਾਨਿਸ਼ਾਨੀ rest_period = ਸਾਰਾ_note_period / rest_ਲੰਮਾਈ;

	ਬਿਨਾਨਿਸ਼ਾਨੀ dot_extension = rest_period / 2;

	ਜਦੋਂ (dots--) {
		rest_period += dot_extension;
		dot_extension /= 2;
	}

	ਵਾਪਸ rest_period;
}

ਪੂਰਨ Tunes::ਅਗਲਾ_char()
{
	ਜਦੋਂ (ਖਾਲੀ_ਥਾਂ_ਹੈ(*ਅਗਲਾ_tune)) {
		ਅਗਲਾ_tune++;
	}

	ਵਾਪਸ ਵੱਡਾ_ਅੱਖਰ(*ਅਗਲਾ_tune);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ Tunes::ਅਗਲਾ_ਸੰਖਿਆ()
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੰਖਿਆ = 0;
	ਪੂਰਨ ਅਗਲਾ_character;

	ਲਈ (;;) {
		ਅਗਲਾ_character = ਅਗਲਾ_char();

		ਜੇ (!ਅੰਕ_ਹੈ(ਅਗਲਾ_character)) {
			ਵਾਪਸ ਸੰਖਿਆ;
		}

		ਅਗਲਾ_tune++;
		ਸੰਖਿਆ = (ਸੰਖਿਆ * 10) + (ਅਗਲਾ_character - '0');
	}
}

ਬਿਨਾਨਿਸ਼ਾਨੀ Tunes::ਅਗਲਾ_dots()
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ dots = 0;

	ਜਦੋਂ (ਅਗਲਾ_char() == '.') {
		ਅਗਲਾ_tune++;
		dots++;
	}

	ਵਾਪਸ dots;
}
