/****************************************************************************
 *
 *   Copyright (C) 2015-2026 PX4 Development Team. All rights reserved.
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
* @file TimestampedRingBuffer.hpp
* @author Roman Bapst <bapstroman@gmail.com>
* @brief Template ring buffer for timestamped samples (requires data_type::time_us).
*
* Note: This is not the same as `Ringbuffer` (byte FIFO) in `src/lib/ringbuffer/Ringbuffer.hpp`.
*/

#निर्देशः once

#अन्तर्भाव <climits>
#अन्तर्भाव <cstddef>
#अन्तर्भाव <inttypes.h>

प्रतिरूपम् <प्रकारनाम दत्त_type, size_t आकार_3 = 0>
वर्ग Timestamped_Ring_बफर;

/**
 * Static-size ring buffer specialization (SIZE > 0).
 */
प्रतिरूपम् <प्रकारनाम दत्त_type, size_t आकार_3>
वर्ग Timestamped_Ring_बफर
{
सार्वजनिक:
	static_assert(आकार_3 > 0, "SIZE must be > 0");
	static_assert(आकार_3 <= UINT8_MAX, "SIZE must fit in uint8_t");
	स्थिर constexpr uint8_t k_आकार = static_cast<uint8_t>(आकार_3);

	Timestamped_Ring_बफर() = स्वभाव;
	~Timestamped_Ring_बफर() = स्वभाव;

	// no copy, assignment, move, move assignment
	Timestamped_Ring_बफर(ध्रुव Timestamped_Ring_बफर &) = विलोपय;
	Timestamped_Ring_बफर &संक्रियकः=(ध्रुव Timestamped_Ring_बफर &) = विलोपय;
	Timestamped_Ring_बफर(Timestamped_Ring_बफर &&) = विलोपय;
	Timestamped_Ring_बफर &संक्रियकः=(Timestamped_Ring_बफर &&) = विलोपय;

	तार्किक allocate(size_t आकार) { प्रत्यागम आकार == आकार_3; }

	तार्किक valid() ध्रुव { प्रत्यागम आकार_3 > 0; }

	शून्य प्रक्षिप(ध्रुव दत्त_type &sample)
	{
		बफर[शीर्ष] = sample;
		शीर्ष = अग्र(शीर्ष);

		यदि (_entries < k_आकार) {
			_entries++;
		}
	}

	uint8_t get_दीर्घता() ध्रुव { प्रत्यागम k_आकार; }

	दत्त_type &संक्रियकः[](ध्रुव uint8_t सूचकांक) { प्रत्यागम बफर[सूचकांक]; }

	ध्रुव दत्त_type &get_newest() ध्रुव { प्रत्यागम बफर[get_newest_सूचकांक()]; }
	ध्रुव दत्त_type &get_oldest() ध्रुव { प्रत्यागम बफर[get_oldest_सूचकांक()]; }

	uint8_t get_newest_सूचकांक() ध्रुव { प्रत्यागम (_entries > 0) ? पूर्व(शीर्ष) : 0; }
	uint8_t get_oldest_सूचकांक() ध्रुव
	{
		यदि (_entries == 0) {
			प्रत्यागम 0;
		}

		ध्रुव uint16_t idx = static_cast<uint16_t>(शीर्ष) + static_cast<uint16_t>(k_आकार) - static_cast<uint16_t>(_entries);
		प्रत्यागम (idx >= k_आकार) ? static_cast<uint8_t>(idx - k_आकार) : static_cast<uint8_t>(idx);
	}

	uint8_t अग्र(uint8_t i) ध्रुव { प्रत्यागम (i + 1 >= k_आकार) ? 0 : i + 1; }
	uint8_t पूर्व(uint8_t i) ध्रुव { प्रत्यागम (i == 0) ? k_आकार - 1 : i - 1; }

	तार्किक निष्कासय_first_older_than(ध्रुव uint64_t &timestamp, दत्त_type *sample)
	{
		// start looking from newest observation data
		ध्रुव uint8_t newest_idx = get_newest_सूचकांक();
		ध्रुव uint8_t oldest_idx = get_oldest_सूचकांक();

		प्रति (uint8_t i = 0; i < _entries; i++) {
			पूर्णाङ्क सूचकांक = (newest_idx - i);
			सूचकांक = सूचकांक < 0 ? k_आकार + सूचकांक : सूचकांक;

			यदि (timestamp >= बफर[सूचकांक].time_us && timestamp < बफर[सूचकांक].time_us + (uint64_t)1e5) {
				*sample = बफर[सूचकांक];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				ध्रुव uint8_t सूचकांक_u = static_cast<uint8_t>(सूचकांक);
				ध्रुव uint8_t distance = (सूचकांक_u >= oldest_idx) ? (सूचकांक_u - oldest_idx) : (सूचकांक_u + k_आकार - oldest_idx);
				ध्रुव uint8_t discard_गणनम् = distance + 1;
				_entries = (_entries > discard_गणनम्) ? (_entries - discard_गणनम्) : 0;
				बफर[सूचकांक].time_us = 0;

				प्रत्यागम सत्य;
			}
		}

		प्रत्यागम असत्य;
	}

	पूर्णाङ्क get_योग_आकार() ध्रुव
	{
		static_assert(परिमाण(Timestamped_Ring_बफर<दत्त_type, आकार_3>) <= static_cast<size_t>(अधिकतम_पूर्णाङ्कः),
			      "static ring buffer size must fit in int");
		प्रत्यागम static_cast<पूर्णाङ्क>(परिमाण(Timestamped_Ring_बफर<दत्त_type, आकार_3>));
	}

	पूर्णाङ्क entries() ध्रुव { प्रत्यागम _entries; }
	तार्किक रिक्तानि() ध्रुव { प्रत्यागम _entries == 0; }

	शून्य reset()
	{
		प्रति (uint8_t i = 0; i < k_आकार; i++) {
			बफर[i].time_us = 0;
		}

		शीर्ष = 0;
		_entries = 0;
	}

गुप्त:
	दत्त_type बफर[आकार_3] {};

	uint8_t शीर्ष{0};
	uint8_t _entries{0};
};

/**
 * Dynamic-size ring buffer specialization (SIZE == 0).
 */
प्रतिरूपम् <प्रकारनाम दत्त_type>
वर्ग Timestamped_Ring_बफर<दत्त_type, 0>
{
सार्वजनिक:
	स्पष्ट Timestamped_Ring_बफर(size_t आकार) { allocate(आकार); }
	Timestamped_Ring_बफर() = विलोपय;
	~Timestamped_Ring_बफर() { विलोपय[] बफर; }

	// no copy, assignment, move, move assignment
	Timestamped_Ring_बफर(ध्रुव Timestamped_Ring_बफर &) = विलोपय;
	Timestamped_Ring_बफर &संक्रियकः=(ध्रुव Timestamped_Ring_बफर &) = विलोपय;
	Timestamped_Ring_बफर(Timestamped_Ring_बफर &&) = विलोपय;
	Timestamped_Ring_बफर &संक्रियकः=(Timestamped_Ring_बफर &&) = विलोपय;

	तार्किक allocate(size_t आकार)
	{
		यदि (valid() && (आकार == आकार_2)) {
			// no change
			प्रत्यागम सत्य;
		}

		यदि (आकार == 0 || आकार > UINT8_MAX) {
			प्रत्यागम असत्य;
		}

		विलोपय[] बफर;

		बफर = नूतन दत्त_type[आकार] {};

		यदि (बफर == शून्यसूचकः) {
			प्रत्यागम असत्य;
		}

		आकार_2 = static_cast<uint8_t>(आकार);

		// new[]{} already zeroed time_us, only reset the indices
		शीर्ष = 0;
		पुच्छ_2 = 0;
		_first_write = सत्य;

		प्रत्यागम सत्य;
	}

	तार्किक valid() ध्रुव { प्रत्यागम (बफर != शून्यसूचकः) && (आकार_2 > 0); }

	शून्य प्रक्षिप(ध्रुव दत्त_type &sample)
	{
		uint8_t शीर्ष_new = शीर्ष;

		यदि (!_first_write) {
			शीर्ष_new = अग्र(शीर्ष);
		}

		बफर[शीर्ष_new] = sample;
		शीर्ष = शीर्ष_new;

		// move tail if we overwrite it
		यदि (शीर्ष == पुच्छ_2 && !_first_write) {
			पुच्छ_2 = अग्र(पुच्छ_2);

		} अन्यथा {
			_first_write = असत्य;
		}
	}

	uint8_t get_दीर्घता() ध्रुव { प्रत्यागम आकार_2; }

	दत्त_type &संक्रियकः[](ध्रुव uint8_t सूचकांक) { प्रत्यागम बफर[सूचकांक]; }

	ध्रुव दत्त_type &get_newest() ध्रुव { प्रत्यागम बफर[शीर्ष]; }
	ध्रुव दत्त_type &get_oldest() ध्रुव { प्रत्यागम बफर[पुच्छ_2]; }

	uint8_t get_newest_सूचकांक() ध्रुव { प्रत्यागम शीर्ष; }
	uint8_t get_oldest_सूचकांक() ध्रुव { प्रत्यागम पुच्छ_2; }

	uint8_t अग्र(uint8_t i) ध्रुव { प्रत्यागम (आकार_2 > 0) ? ((i + 1 >= आकार_2) ? 0 : i + 1) : 0; }
	uint8_t पूर्व(uint8_t i) ध्रुव { प्रत्यागम (आकार_2 > 0) ? ((i == 0) ? (आकार_2 - 1) : (i - 1)) : 0; }

	तार्किक निष्कासय_first_older_than(ध्रुव uint64_t &timestamp, दत्त_type *sample)
	{
		// start looking from newest observation data
		प्रति (uint8_t i = 0; i < आकार_2; i++) {
			पूर्णाङ्क सूचकांक = (शीर्ष - i);
			सूचकांक = सूचकांक < 0 ? आकार_2 + सूचकांक : सूचकांक;

			यदि (timestamp >= बफर[सूचकांक].time_us && timestamp < बफर[सूचकांक].time_us + (uint64_t)1e5) {
				*sample = बफर[सूचकांक];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				यदि (सूचकांक == शीर्ष) {
					पुच्छ_2 = शीर्ष;
					_first_write = सत्य;

				} अन्यथा {
					पुच्छ_2 = अग्र(static_cast<uint8_t>(सूचकांक));
				}

				बफर[सूचकांक].time_us = 0;

				प्रत्यागम सत्य;
			}

			यदि (सूचकांक == पुच्छ_2) {
				// we have reached the tail and haven't got a
				//मेलनम्
				प्रत्यागम असत्य;
			}
		}

		प्रत्यागम असत्य;
	}

	पूर्णाङ्क get_प्रयुक्तम्_आकार() ध्रुव { प्रत्यागम परिमाण(*एतत्) + परिमाण(दत्त_type) * entries(); }
	पूर्णाङ्क get_योग_आकार() ध्रुव { प्रत्यागम परिमाण(*एतत्) + परिमाण(दत्त_type) * आकार_2; }

	पूर्णाङ्क entries() ध्रुव
	{
		पूर्णाङ्क गणनम् = 0;

		प्रति (uint8_t i = 0; i < आकार_2; i++) {
			यदि (बफर[i].time_us != 0) {
				गणनम्++;
			}
		}

		प्रत्यागम गणनम्;
	}

	तार्किक रिक्तानि() ध्रुव { प्रत्यागम entries() == 0; }

	शून्य reset()
	{
		यदि (बफर) {
			प्रति (uint8_t i = 0; i < आकार_2; i++) {
				बफर[i].time_us = 0;
			}

			शीर्ष = 0;
			पुच्छ_2 = 0;
			_first_write = सत्य;
		}
	}

गुप्त:
	दत्त_type *बफर{शून्यसूचकः};

	uint8_t शीर्ष{0};
	uint8_t पुच्छ_2{0};
	uint8_t आकार_2{0};

	तार्किक _first_write{सत्य};
};
