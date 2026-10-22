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
 * @file tunes.h
 */

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/uORB.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/tune_control.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "tune_definition.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TUNE_DEFAULT_NOTE_ਲੰਮਾਈ 4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TUNE_DEFAULT_OCTAVE 4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TUNE_DEFAULT_TEMPO 120

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TUNE_MAX_UPDATE_INTERVAL_US 100000


/**
 *  Library for parsing tunes from melody-strings or dedicated tune messages.
 *  Needs to be instantiated as it keeps track of which tune is to be played
 *  next. Also handles repeated tunes.
 */
class Tunes
{
ਜਨਤਕ:
	ਗਿਣਤੀ class Note_ਬਹੁਲਕ {ਸਾਧਾਰਨ, LEGATO, STACCATO};

	ਗਿਣਤੀ class ਸਥਿਤੀ_2 {
		Continue = 1,
		Stop = 0,
		Error = -1,
	};

	ਗਿਣਤੀ class Control_ਨਤੀਜਾ {
		Success = 0,
		AlreadyPlaying = 1,
		InvalidTune = -EINVAL,
		Would_ਰੁਕਾਵਟ = -EBUSY,
	};

	/**
	 * Constructor with the default parameters set to:
	 * default_tempo: TUNE_DEFAULT_TEMPO
	 * default_octave: TUNE_DEFAULT_OCTAVE
	 * default_note_length: TUNE_DEFAULT_NOTE_LENGTH
	 * default_mode: NORMAL
	 */
	Tunes(ਬਿਨਾਨਿਸ਼ਾਨੀ default_note_ਲੰਮਾਈ = TUNE_DEFAULT_NOTE_ਲੰਮਾਈ,
	      Note_ਬਹੁਲਕ default_note_ਬਹੁਲਕ   = Note_ਬਹੁਲਕ::ਸਾਧਾਰਨ,
	      ਬਿਨਾਨਿਸ਼ਾਨੀ default_octave      = TUNE_DEFAULT_OCTAVE,
	      ਬਿਨਾਨਿਸ਼ਾਨੀ default_tempo       = TUNE_DEFAULT_TEMPO);

	~Tunes() = ਮੂਲ;

	/**
	 * Set tune to be played using the message. If a tune is already being played
	 * the call to this function will be ignored, unless the override flag is set
	 * or the tune being already played is a repeated tune.
	 * @param  tune_control struct containig the uORB message
	 * @return              return ControlResult::InvalidTune if the default tune does not exist,
	 * 			ControlResult::WouldInterrupt if tune was already playing and not interruptible,
	 * 			ControlResult::AlreadyPlaying if same tune was already playing,
	 * 			ControlResult::Success if new tune was set.
	 */
	Control_ਨਤੀਜਾ ਸੈੱਟ_control(ਅਚਲ tune_control_s &tune_control);

	/**
	 * Set tune to be played using a string.
	 * Parses a tune string, formatted with the syntax of the Microsoft GWBasic/QBasic.
	 * Ownership of the string is NOT transferred. The string has to be kept in
	 * memory for the whole duration of the melody.
	 *
	 * @param  string    tune input string
	 */
	ਖਾਲੀ ਸੈੱਟ_string(ਅਚਲ ਅੱਖਰ *ਅਚਲ string, uint8_t volume);

	/**
	 * Get next note in the current tune, which has been provided by either
	 * set_control or play_string
	 * @param  frequency return frequency value (Hz)
	 * @param  duration  return duration of the tone (us)
	 * @param  silence   return silence duration (us)
	 * @return           -1 for error, 0 for play one tone and 1 for continue a sequence
	 */
	Tunes::ਸਥਿਤੀ_2 get_ਅਗਲਾ_note(ਬਿਨਾਨਿਸ਼ਾਨੀ &frequency, ਬਿਨਾਨਿਸ਼ਾਨੀ &duration, ਬਿਨਾਨਿਸ਼ਾਨੀ &silence);

	/**
	 * Get next note in the current tune, which has been provided by either
	 * set_control or play_string
	 * @param  frequency return frequency value (Hz)
	 * @param  duration  return duration of the note (us)
	 * @param  silence   return silence duration (us)
	 * @param  volume    return the volume level of the note (between 0-100)
	 * @return           -1 for no tune available/error, 0 to not play anything and 1 to play
	 */
	Tunes::ਸਥਿਤੀ_2 get_ਅਗਲਾ_note(ਬਿਨਾਨਿਸ਼ਾਨੀ &frequency, ਬਿਨਾਨਿਸ਼ਾਨੀ &duration,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ &silence, uint8_t &volume);

	/**
	 *  Get the number of default tunes. This is useful for when a tune is
	 *  requested via its tune ID.
	 *  @return             Number of default tunes accessible via tune ID
	 */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ get_default_tunes_ਮਾਪ() ਅਚਲ {ਵਾਪਸ default_tunes_ਮਾਪ;}

	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ get_ਵੱਧ_ਤੋਂ_ਵੱਧ_update_interval() {ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)TUNE_MAX_UPDATE_INTERVAL_US;}

ਨਿਜੀ:

	/**
	 * Convert note to frequency
	 *
	 * @param  note unsigned value of the semitone from C
	 * @return      frequency (Hz)
	 */
	uint32_t note_ਮੰਜ਼ਿਲ_frequency(ਬਿਨਾਨਿਸ਼ਾਨੀ note) ਅਚਲ;

	/**
	 * Calculate the duration in microseconds of play and silence for a
	 * note given the current tempo, length and mode and the number of
	 * dots following in the play string.
	 *
	 * @param  silence     return silence duration (us)
	 * @param  note_length note length
	 * @param  dots        extention of the note length
	 * @return             duration of the note (us)
	 */
	ਬਿਨਾਨਿਸ਼ਾਨੀ note_duration(ਬਿਨਾਨਿਸ਼ਾਨੀ &silence, ਬਿਨਾਨਿਸ਼ਾਨੀ note_ਲੰਮਾਈ_2, ਬਿਨਾਨਿਸ਼ਾਨੀ dots) ਅਚਲ;

	/**
	 * Calculate the duration in microseconds of a rest corresponding to
	 * a given note length.
	 *
	 * @param  rest_length rest lenght in fraction of a note
	 * @param  dots        number of extension dots
	 * @return             rest duration (us)
	 */
	ਬਿਨਾਨਿਸ਼ਾਨੀ rest_duration(ਬਿਨਾਨਿਸ਼ਾਨੀ rest_ਲੰਮਾਈ, ਬਿਨਾਨਿਸ਼ਾਨੀ dots) ਅਚਲ;

	/**
	 * Find the next character in the string, discard any whitespace.
	 *
	 * @return uppercase version of the char.
	 */
	ਪੂਰਨ ਅਗਲਾ_char();

	/**
	 * Extract a number from the string, consuming all the digit characters.
	 *
	 * @return extracted number.
	 */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਗਲਾ_ਸੰਖਿਆ();

	/**
	 * Consume dot characters from the string
	 *
	 * @return number of consumed dots
	 */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਗਲਾ_dots();

	/**
	 * Reset the tune parameters. This is necessary when for example a tune moved
	 * one or more octaves up or down. reset() should always be called before
	 * (re)-starting a tune.
	 */
	ਖਾਲੀ reset(bool repeat_flag);

	Tunes::ਸਥਿਤੀ_2 tune_ਸਿਰਾ();

	Tunes::ਸਥਿਤੀ_2 tune_error();

	ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ  _default_tunes[];
	ਸਥਿਰ ਅਚਲ bool         _default_tunes_interruptable[];
	ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ default_tunes_ਮਾਪ;
	ਸਥਿਰ ਅਚਲ uint8_t      _note_tab[];

	ਅਚਲ ਅੱਖਰ *ਅਗਲਾ_tune      = nullptr; ///< next note in the string
	ਅਚਲ ਅੱਖਰ *_tune           = nullptr; ///< current tune string
	ਅਚਲ ਅੱਖਰ *tune_ਸ਼ੁਰੂ_ptr = nullptr; ///< pointer to repeat tune

	ਪੂਰਨ _current_tune_id = static_cast<ਪੂਰਨ>(TuneID::NONE);

	bool _repeat = false; ///< if true, tune restarts at end

	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ default_note_ਲੰਮਾਈ_2 = TUNE_DEFAULT_NOTE_ਲੰਮਾਈ;
	Note_ਬਹੁਲਕ     default_note_ਬਹੁਲਕ_2   = Note_ਬਹੁਲਕ::ਸਾਧਾਰਨ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ _default_octave      = TUNE_DEFAULT_OCTAVE;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ _default_tempo       = TUNE_DEFAULT_TEMPO;

	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ note_ਲੰਮਾਈ         = TUNE_DEFAULT_NOTE_ਲੰਮਾਈ;
	Note_ਬਹੁਲਕ     note_ਬਹੁਲਕ           = Note_ਬਹੁਲਕ::ਸਾਧਾਰਨ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ _octave              = TUNE_DEFAULT_OCTAVE;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ _tempo               = TUNE_DEFAULT_TEMPO;

	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ _duration  = 0;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ _frequency = 0;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ _silence   = 0;
	uint8_t      _volume    = 0;

	bool  using_custom_ਸੁਨੇਹਾ = false;

	bool _tunes_disabled{false};
};
