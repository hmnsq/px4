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

#परिभाषय BEAT_TIME_CONVERSION_MS (60 * 1000 * 4)
#परिभाषय BEAT_TIME_CONVERSION_US (BEAT_TIME_CONVERSION_MS * 1000)
#परिभाषय BEAT_TIME_CONVERSION    BEAT_TIME_CONVERSION_US

// semitone offsets from C for the characters 'A'-'G'
ध्रुव uint8_t Tunes::_note_tab[] = {9, 11, 0, 2, 4, 5, 7};

Tunes::Tunes(अचिह्नित default_note_दीर्घता, Note_बहुलकम् default_note_बहुलकम्,
	     अचिह्नित default_octave, अचिह्नित default_tempo):
	default_note_दीर्घता_2(default_note_दीर्घता),
	default_note_बहुलकम्_2(default_note_बहुलकम्),
	_default_octave(default_octave),
	_default_tempo(default_tempo)
{
	यदि (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_कुञ्चिका)) {
		_tunes_disabled = सत्य;
	}

	reset(असत्य);
}

शून्य Tunes::reset(तार्किक repeat_flag)
{
	// reset pointer
	यदि (!repeat_flag)	{
		_tune = शून्यसूचकः;
		अग्र_tune = शून्यसूचकः;

	} अन्यथा {
		_tune = _tune_start_ptr;
		अग्र_tune = _tune;
	}

	// reset music parameter
	note_दीर्घता = default_note_दीर्घता_2;
	note_बहुलकम्   = default_note_बहुलकम्_2;
	_octave      = _default_octave;
	_tempo       = _default_tempo;
}

Tunes::Control_परिणाम Tunes::समुच्चयः_control(ध्रुव tune_control_s &tune_control)
{
	// Sanity check
	यदि (tune_control.tune_id >= default_tunes_आकार) {
		प्रत्यागम Control_परिणाम::InvalidTune;
	}

	// Accept new tune or a stop?
	यदि (_tune == शून्यसूचकः ||  	   // No tune is currently being played
	    tune_control.tune_override ||  // Override interrupts everything
	    _default_tunes_interruptable[_current_tune_id]) {

		// Check if this exact tune is already being played back
		यदि (tune_control.tune_id != static_cast<पूर्णाङ्क>(TuneID::CUSTOM) &&
		    _tune == _default_tunes[tune_control.tune_id]) {
			प्रत्यागम Control_परिणाम::AlreadyPlaying; // Nothing to do
		}

		// Reset repeat flag. Can jump to true again while tune is being parsed later
		_repeat = असत्य;

		// Reset octave, tempo etc.
		reset(_repeat);

		// Volume will remain valid for the entire tune, unless interrupted.
		यदि (tune_control.volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = tune_control.volume;

		} अन्यथा {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}


		// Special treatment for custom tunes
		यदि (tune_control.tune_id == static_cast<पूर्णाङ्क>(TuneID::CUSTOM)) {
			using_custom_सन्देशः = सत्य;
			_frequency        = (अचिह्नित)tune_control.frequency;
			_duration         = (अचिह्नित)tune_control.duration;
			_silence          = (अचिह्नित)tune_control.silence;

		} अन्यथा {
			using_custom_सन्देशः = असत्य;
			_tune             = _default_tunes[tune_control.tune_id];
			_tune_start_ptr   = _tune;
			अग्र_tune        = _tune;
		}

		_current_tune_id = tune_control.tune_id;
		प्रत्यागम Control_परिणाम::Success;
	}

	प्रत्यागम Control_परिणाम::Would_व्यवधानम्;
}

शून्य Tunes::समुच्चयः_string(ध्रुव वर्ण *ध्रुव string, uint8_t volume)
{
	// Only play new tune if nothing is being played currently.
	यदि (_tune == शून्यसूचकः) {
		// Set tune string the first time.
		_tune           = string;
		_tune_start_ptr = string;
		अग्र_tune      = _tune;

		यदि (volume <= tune_control_s::VOLUME_LEVEL_MAX) {
			_volume = volume;

		} अन्यथा {
			_volume = tune_control_s::VOLUME_LEVEL_MAX;
		}
	}
}

Tunes::स्थितिः_2 Tunes::get_अग्र_note(अचिह्नित &frequency, अचिह्नित &duration, अचिह्नित &silence, uint8_t &volume)
{
	यदि (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		volume = 0;
		प्रत्यागम Tunes::स्थितिः_2::Stop;
	}

	Tunes::स्थितिः_2 ret = get_अग्र_note(frequency, duration, silence);

	// Check if note should not be heard -> adjust volume to 0 to be safe.
	यदि (frequency == 0 || duration == 0) {
		volume = 0;

	} अन्यथा {
		volume = _volume;
	}

	प्रत्यागम ret;
}

Tunes::स्थितिः_2 Tunes::get_अग्र_note(अचिह्नित &frequency, अचिह्नित &duration, अचिह्नित &silence)
{
	यदि (_tunes_disabled) {
		frequency = 0;
		duration = 0;
		silence = 0;
		प्रत्यागम Tunes::स्थितिः_2::Stop;
	}

	// Return the values for frequency and duration if the custom msg was received.
	यदि (using_custom_सन्देशः) {
		using_custom_सन्देशः = असत्य;
		duration  = _duration;
		frequency = _frequency;
		silence   = _silence;
		प्रत्यागम Tunes::स्थितिः_2::Continue;
	}

	// Make sure we still have a tune.
	यदि ((अग्र_tune == शून्यसूचकः) || (_tune == शून्यसूचकः)) {
		प्रत्यागम tune_error();
	}

	// Parse characters out of the string until we have resolved a note.
	अचिह्नित note = 0;
	अचिह्नित note_दीर्घता_2 = note_दीर्घता;

	यावत् (note == 0) {
		// we always need at least one character from the string.
		पूर्णाङ्क c = अग्र_char();

		यदि (c == 0) {
			silence = 0;
			प्रत्यागम tune_अन्तः();
		}

		अग्र_tune++;

		विकल्प (c) {

		पक्ष 'V':	// Select volume.
			// Consume volume if specified in tune but ignore
			// MML compatibility
			अग्र_सङ्ख्या();
			भङ्ग;

		पक्ष 'L':	// Select note length.
			note_दीर्घता = अग्र_सङ्ख्या();

			यदि (note_दीर्घता < 1) {
				प्रत्यागम tune_error();
			}

			भङ्ग;

		पक्ष 'O':	// Select octave.
			_octave = अग्र_सङ्ख्या();

			यदि (_octave > 6) {
				_octave = 6;
			}

			भङ्ग;

		पक्ष '<':	// Decrease octave.
			यदि (_octave > 0) {
				_octave--;
			}

			भङ्ग;

		पक्ष '>':	// Increase octave.
			यदि (_octave < 6) {
				_octave++;
			}

			भङ्ग;

		पक्ष 'M':	// Select inter-note gap.
			c = अग्र_char();

			यदि (c == 0) {
				प्रत्यागम tune_error();
			}

			अग्र_tune++;

			विकल्प (c) {
			पक्ष 'N':
				note_बहुलकम् = Note_बहुलकम्::प्रसामान्यम्;
				भङ्ग;

			पक्ष 'L':
				note_बहुलकम् = Note_बहुलकम्::LEGATO;
				भङ्ग;

			पक्ष 'S':
				note_बहुलकम् = Note_बहुलकम्::STACCATO;
				भङ्ग;

			पक्ष 'F':
				_repeat = असत्य;
				भङ्ग;

			पक्ष 'B':
				_repeat = सत्य;
				भङ्ग;

			स्वभाव:
				प्रत्यागम tune_error();
			}

			भङ्ग;

		पक्ष 'P':	// Pause for a note length.
		पक्ष 'R':
			frequency = 0;
			duration = 0;
			note_दीर्घता_2 = अग्र_सङ्ख्या();
			silence = rest_duration(note_दीर्घता_2, अग्र_dots());
			प्रत्यागम Tunes::स्थितिः_2::Continue;

		पक्ष 'T': {	// Change tempo.
				अचिह्नित nt = अग्र_सङ्ख्या();

				यदि ((nt >= 32) && (nt <= 255)) {
					_tempo = nt;

				} अन्यथा {
					प्रत्यागम tune_error();
				}

				भङ्ग;
			}

		पक्ष 'N':	// Play an arbitrary note.
			note = अग्र_सङ्ख्या();

			यदि (note > 84) {
				प्रत्यागम tune_error();
			}

			यदि (note == 0) {
				// This is a rest - pause for the current note length.
				silence = rest_duration(note_दीर्घता, अग्र_dots());
				प्रत्यागम Tunes::स्थितिः_2::Continue;
			}

			भङ्ग;

		पक्ष 'A'...'G':	// Play a note in the current octave.
			note = _note_tab[c - 'A'] + (_octave * 12) + 1;
			c = अग्र_char();

			विकल्प (c) {
			पक्ष '#':	// Up a semitone.
			पक्ष '+':
				यदि (note < 84) {
					note++;
				}

				अग्र_tune++;
				भङ्ग;

			पक्ष '-':	// Down a semitone.
				यदि (note > 1) {
					note--;
				}

				अग्र_tune++;
				भङ्ग;

			स्वभाव:
				// 0 / No next char here is OK.
				भङ्ग;
			}

			// Shorthand length notation.
			note_दीर्घता_2 = अग्र_सङ्ख्या();

			यदि (note_दीर्घता_2 == 0) {
				note_दीर्घता_2 = note_दीर्घता;
			}

			भङ्ग;

		स्वभाव:
			प्रत्यागम tune_error();
		}
	}

	// Compute the duration of the note and the following silence (if any).
	duration = note_duration(silence, note_दीर्घता_2, अग्र_dots());

	// Compute the note frequency.
	frequency = note_गन्तव्यम्_frequency(note);
	प्रत्यागम Tunes::स्थितिः_2::Continue;
}

Tunes::स्थितिः_2 Tunes::tune_अन्तः()
{
	यदि (_tunes_disabled) {
		प्रत्यागम Tunes::स्थितिः_2::Stop;
	}

	// Restore intial parameters.
	reset(_repeat);

	// Stop or restart the tune.
	यदि (_repeat) {
		प्रत्यागम Tunes::स्थितिः_2::Continue;

	} अन्यथा {
		प्रत्यागम Tunes::स्थितिः_2::Stop;
	}
}

Tunes::स्थितिः_2 Tunes::tune_error()
{
	यदि (_tunes_disabled) {
		प्रत्यागम Tunes::स्थितिः_2::Stop;
	}

	// The tune appears to be bad (unexpected EOF, bad character, etc.).
	यदि (अग्र_tune != शून्यसूचकः) {
		PX4_WARN("Tune error at: %s", अग्र_tune);
	}

	_repeat = असत्य;	// Don't loop on error.
	reset(_repeat);
	प्रत्यागम Tunes::स्थितिः_2::Error;
}

uint32_t Tunes::note_गन्तव्यम्_frequency(अचिह्नित note) ध्रुव
{
	// Compute the frequency (Hz).
	प्रत्यागम (अचिह्नित)(880.0f * powf(2.0f, ((पूर्णाङ्क)note - 46) / 12.0f));
}

अचिह्नित Tunes::note_duration(अचिह्नित &silence, अचिह्नित note_दीर्घता_2, अचिह्नित dots) ध्रुव
{
	अचिह्नित सम्पूर्णम्_note_period = BEAT_TIME_CONVERSION / _tempo;

	यदि (note_दीर्घता_2 == 0) {
		note_दीर्घता_2 = 1;
	}

	अचिह्नित note_period = सम्पूर्णम्_note_period / note_दीर्घता_2;

	विकल्प (note_बहुलकम्) {
	पक्ष Note_बहुलकम्::प्रसामान्यम्:
		silence = note_period / 8;
		भङ्ग;

	पक्ष Note_बहुलकम्::STACCATO:
		silence = note_period / 4;
		भङ्ग;

	पक्ष Note_बहुलकम्::LEGATO:
	स्वभाव:
		silence = 0;
		भङ्ग;
	}

	note_period -= silence;

	अचिह्नित dot_extension = note_period / 2;

	यावत् (dots--) {
		note_period += dot_extension;
		dot_extension /= 2;
	}

	प्रत्यागम note_period;
}

अचिह्नित Tunes::rest_duration(अचिह्नित rest_दीर्घता, अचिह्नित dots) ध्रुव
{
	अचिह्नित सम्पूर्णम्_note_period = BEAT_TIME_CONVERSION / _tempo;

	यदि (rest_दीर्घता == 0) {
		rest_दीर्घता = 1;
	}

	अचिह्नित rest_period = सम्पूर्णम्_note_period / rest_दीर्घता;

	अचिह्नित dot_extension = rest_period / 2;

	यावत् (dots--) {
		rest_period += dot_extension;
		dot_extension /= 2;
	}

	प्रत्यागम rest_period;
}

पूर्णाङ्क Tunes::अग्र_char()
{
	यावत् (रिक्तम्_वा(*अग्र_tune)) {
		अग्र_tune++;
	}

	प्रत्यागम महाक्षरम्(*अग्र_tune);
}

अचिह्नित Tunes::अग्र_सङ्ख्या()
{
	अचिह्नित सङ्ख्या = 0;
	पूर्णाङ्क अग्र_character;

	प्रति (;;) {
		अग्र_character = अग्र_char();

		यदि (!अङ्कः_वा(अग्र_character)) {
			प्रत्यागम सङ्ख्या;
		}

		अग्र_tune++;
		सङ्ख्या = (सङ्ख्या * 10) + (अग्र_character - '0');
	}
}

अचिह्नित Tunes::अग्र_dots()
{
	अचिह्नित dots = 0;

	यावत् (अग्र_char() == '.') {
		अग्र_tune++;
		dots++;
	}

	प्रत्यागम dots;
}
