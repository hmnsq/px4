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

#अन्तर्भाव "tunes.h"

#अन्तर्भाव <px4_platform_common/log.h>

#अन्तर्भाव <lib/circuit_breaker/circuit_breaker.h>

#अन्तर्भाव <math.h>
#अन्तर्भाव <ctype.h>
#अन्तर्भाव <errno.h>

#define BEAT_TIME_CONVERSION_MS (60 * 1000 * 4)
#define BEAT_TIME_CONVERSION_US (BEAT_TIME_CONVERSION_MS * 1000)
#define BEAT_TIME_CONVERSION    BEAT_TIME_CONVERSION_US

// semitone offsets from C for the characters 'A'-'G'
ध्रुव uint8_t Tunes::_note_tab[] = {9, 11, 0, 2, 4, 5, 7};

Tunes::Tunes(unsigned default_note_length, NoteMode default_note_mode,
	     unsigned default_octave, unsigned default_tempo):
	_default_note_length(default_note_length),
	_default_note_mode(default_note_mode),
	_default_octave(default_octave),
	_default_tempo(default_tempo)
{
	जइ (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_कुञ्चिका)) {
		_tunes_disabled = सच्च;
	}

	reset(मिच्छ);
}

शून्य Tunes::reset(तार्किक repeat_flag)
{
	// reset pointer
	जइ (!repeat_flag)	{
		_tune = nullptr;
		_next_tune = nullptr;

	} अण्णहा {
		_tune = _tune_start_ptr;
		_next_tune = _tune;
	}

	// reset music parameter
	_note_length = _default_note_length;
	_note_mode   = _default_note_mode;
	_octave      = _default_octave;
	_tempo       = _default_tempo;
}

Tunes::ControlResult Tunes::set_control(ध्रुव tune_control_s &tune_control)
{
	// Sanity check
	जइ (tune_control.tune_id >= _default_tunes_size) {
		प्रत्यागम ControlResult::InvalidTune;
	}

	// Accept new tune or a stop?
	जइ (_tune == nullptr ||  	   // No tune is currently being played
	    tune_control.tune_override ||  // Override interrupts everything
	    _default_tunes_interruptable[_current_tune_id]) {

		// Check if this exact tune is already being played back
		जइ (tune_control.tune_id != static_cast<पूर्णाङ्क>(TuneID::CUSTOM) &&
		    _tune == _default_tunes[tune_control.tune_id]) {
			प्रत्यागम ControlResult::AlreadyPlaying; // Nothing to do
		}

		// Reset repeat flag. Can jump to true again while tune is being parsed later
		_repeat = मिच्छ;

		// Reset octave, tempo etc.
		reset(_repeat);

		// Volume will remain valid for the entire tune, unless interrupted.
		जइ (tune_control.volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = tune_control.volume;

		} अण्णहा {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}


		// Special treatment for custom tunes
		जइ (tune_control.tune_id == static_cast<पूर्णाङ्क>(TuneID::CUSTOM)) {
			using_custom_सन्देशः = सच्च;
			_frequency        = (unsigned)tune_control.frequency;
			_duration         = (unsigned)tune_control.duration;
			_silence          = (unsigned)tune_control.silence;

		} अण्णहा {
			using_custom_सन्देशः = मिच्छ;
			_tune             = _default_tunes[tune_control.tune_id];
			_tune_start_ptr   = _tune;
			_next_tune        = _tune;
		}

		_current_tune_id = tune_control.tune_id;
		प्रत्यागम ControlResult::Success;
	}

	प्रत्यागम ControlResult::WouldInterrupt;
}

शून्य Tunes::set_string(ध्रुव वर्ण *ध्रुव string, uint8_t volume)
{
	// Only play new tune if nothing is being played currently.
	जइ (_tune == nullptr) {
		// Set tune string the first time.
		_tune           = string;
		_tune_start_ptr = string;
		_next_tune      = _tune;

		जइ (volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = volume;

		} अण्णहा {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}
	}
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence, uint8_t &volume)
{
	जइ (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		volume = 0;
		प्रत्यागम Tunes::Status::Stop;
	}

	Tunes::Status ret = get_next_note(frequency, duration, silence);

	// Check if note should not be heard -> adjust volume to 0 to be safe.
	जइ (frequency == 0 || duration == 0) {
		volume = 0;

	} अण्णहा {
		volume = _volume;
	}

	प्रत्यागम ret;
}

Tunes::Status Tunes::get_next_note(unsigned &frequency, unsigned &duration, unsigned &silence)
{
	जइ (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		प्रत्यागम Tunes::Status::Stop;
	}

	// Return the values for frequency and duration if the custom msg was received.
	जइ (using_custom_सन्देशः) {
		using_custom_सन्देशः = मिच्छ;
		duration  = _duration;
		frequency = _frequency;
		silence   = _silence;
		प्रत्यागम Tunes::Status::Continue;
	}

	// Make sure we still have a tune.
	जइ ((_next_tune == nullptr) || (_tune == nullptr)) {
		प्रत्यागम tune_error();
	}

	// Parse characters out of the string until we have resolved a note.
	unsigned note = 0;
	unsigned note_length = _note_length;

	जाव (note == 0) {
		// we always need at least one character from the string.
		पूर्णाङ्क c = next_char();

		जइ (c == 0) {
			silence = 0;
			प्रत्यागम tune_end();
		}

		_next_tune++;

		विकल्प (c) {

		पक्ष 'V':	// Select volume.
			// Consume volume if specified in tune but ignore
			// MML compatibility
			next_सङ्ख्या();
			भङ्ग;

		पक्ष 'L':	// Select note length.
			_note_length = next_सङ्ख्या();

			जइ (_note_length < 1) {
				प्रत्यागम tune_error();
			}

			भङ्ग;

		पक्ष 'O':	// Select octave.
			_octave = next_सङ्ख्या();

			जइ (_octave > 6) {
				_octave = 6;
			}

			भङ्ग;

		पक्ष '<':	// Decrease octave.
			जइ (_octave > 0) {
				_octave--;
			}

			भङ्ग;

		पक्ष '>':	// Increase octave.
			जइ (_octave < 6) {
				_octave++;
			}

			भङ्ग;

		पक्ष 'M':	// Select inter-note gap.
			c = next_char();

			जइ (c == 0) {
				प्रत्यागम tune_error();
			}

			_next_tune++;

			विकल्प (c) {
			पक्ष 'N':
				_note_mode = NoteMode::NORMAL;
				भङ्ग;

			पक्ष 'L':
				_note_mode = NoteMode::LEGATO;
				भङ्ग;

			पक्ष 'S':
				_note_mode = NoteMode::STACCATO;
				भङ्ग;

			पक्ष 'F':
				_repeat = मिच्छ;
				भङ्ग;

			पक्ष 'B':
				_repeat = सच्च;
				भङ्ग;

			स्वभाव:
				प्रत्यागम tune_error();
			}

			भङ्ग;

		पक्ष 'P':	// Pause for a note length.
		पक्ष 'R':
			frequency = 0;
			duration = 0;
			note_length = next_सङ्ख्या();
			silence = rest_duration(note_length, next_dots());
			प्रत्यागम Tunes::Status::Continue;

		पक्ष 'T': {	// Change tempo.
				unsigned nt = next_सङ्ख्या();

				जइ ((nt >= 32) && (nt <= 255)) {
					_tempo = nt;

				} अण्णहा {
					प्रत्यागम tune_error();
				}

				भङ्ग;
			}

		पक्ष 'N':	// Play an arbitrary note.
			note = next_सङ्ख्या();

			जइ (note > 84) {
				प्रत्यागम tune_error();
			}

			जइ (note == 0) {
				// This is a rest - pause for the current note length.
				silence = rest_duration(_note_length, next_dots());
				प्रत्यागम Tunes::Status::Continue;
			}

			भङ्ग;

		पक्ष 'A'...'G':	// Play a note in the current octave.
			note = _note_tab[c - 'A'] + (_octave * 12) + 1;
			c = next_char();

			विकल्प (c) {
			पक्ष '#':	// Up a semitone.
			पक्ष '+':
				जइ (note < 84) {
					note++;
				}

				_next_tune++;
				भङ्ग;

			पक्ष '-':	// Down a semitone.
				जइ (note > 1) {
					note--;
				}

				_next_tune++;
				भङ्ग;

			स्वभाव:
				// 0 / No next char here is OK.
				भङ्ग;
			}

			// Shorthand length notation.
			note_length = next_सङ्ख्या();

			जइ (note_length == 0) {
				note_length = _note_length;
			}

			भङ्ग;

		स्वभाव:
			प्रत्यागम tune_error();
		}
	}

	// Compute the duration of the note and the following silence (if any).
	duration = note_duration(silence, note_length, next_dots());

	// Compute the note frequency.
	frequency = note_to_frequency(note);
	प्रत्यागम Tunes::Status::Continue;
}

Tunes::Status Tunes::tune_end()
{
	जइ (_tunes_disabled) {
		प्रत्यागम Tunes::Status::Stop;
	}

	// Restore intial parameters.
	reset(_repeat);

	// Stop or restart the tune.
	जइ (_repeat) {
		प्रत्यागम Tunes::Status::Continue;

	} अण्णहा {
		प्रत्यागम Tunes::Status::Stop;
	}
}

Tunes::Status Tunes::tune_error()
{
	जइ (_tunes_disabled) {
		प्रत्यागम Tunes::Status::Stop;
	}

	// The tune appears to be bad (unexpected EOF, bad character, etc.).
	जइ (_next_tune != nullptr) {
		PX4_WARN("Tune error at: %s", _next_tune);
	}

	_repeat = मिच्छ;	// Don't loop on error.
	reset(_repeat);
	प्रत्यागम Tunes::Status::Error;
}

uint32_t Tunes::note_to_frequency(unsigned note) ध्रुव
{
	// Compute the frequency (Hz).
	प्रत्यागम (unsigned)(880.0f * powf(2.0f, ((पूर्णाङ्क)note - 46) / 12.0f));
}

unsigned Tunes::note_duration(unsigned &silence, unsigned note_length, unsigned dots) ध्रुव
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	जइ (note_length == 0) {
		note_length = 1;
	}

	unsigned note_period = whole_note_period / note_length;

	विकल्प (_note_mode) {
	पक्ष NoteMode::NORMAL:
		silence = note_period / 8;
		भङ्ग;

	पक्ष NoteMode::STACCATO:
		silence = note_period / 4;
		भङ्ग;

	पक्ष NoteMode::LEGATO:
	स्वभाव:
		silence = 0;
		भङ्ग;
	}

	note_period -= silence;

	unsigned dot_extension = note_period / 2;

	जाव (dots--) {
		note_period += dot_extension;
		dot_extension /= 2;
	}

	प्रत्यागम note_period;
}

unsigned Tunes::rest_duration(unsigned rest_length, unsigned dots) ध्रुव
{
	unsigned whole_note_period = BEAT_TIME_CONVERSION / _tempo;

	जइ (rest_length == 0) {
		rest_length = 1;
	}

	unsigned rest_period = whole_note_period / rest_length;

	unsigned dot_extension = rest_period / 2;

	जाव (dots--) {
		rest_period += dot_extension;
		dot_extension /= 2;
	}

	प्रत्यागम rest_period;
}

पूर्णाङ्क Tunes::next_char()
{
	जाव (isspace(*_next_tune)) {
		_next_tune++;
	}

	प्रत्यागम toupper(*_next_tune);
}

unsigned Tunes::next_सङ्ख्या()
{
	unsigned सङ्ख्या = 0;
	पूर्णाङ्क next_character;

	प्रति (;;) {
		next_character = next_char();

		जइ (!isdigit(next_character)) {
			प्रत्यागम सङ्ख्या;
		}

		_next_tune++;
		सङ्ख्या = (सङ्ख्या * 10) + (next_character - '0');
	}
}

unsigned Tunes::next_dots()
{
	unsigned dots = 0;

	जाव (next_char() == '.') {
		_next_tune++;
		dots++;
	}

	प्रत्यागम dots;
}
