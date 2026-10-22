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

#pragma once

#अन्तर्भाव <climits>
#अन्तर्भाव <cstddef>
#अन्तर्भाव <inttypes.h>

ढाँचा <typename data_type, माप_प्रकार SIZE = 0>
वर्ग TimestampedRingBuffer;

/**
 * Static-size ring buffer specialization (SIZE > 0).
 */
ढाँचा <typename data_type, माप_प्रकार SIZE>
वर्ग TimestampedRingBuffer
{
सार्वजनिक:
	static_assert(SIZE > 0, "SIZE must be > 0");
	static_assert(SIZE <= UINT8_MAX, "SIZE must fit in uint8_t");
	स्थिर constexpr uint8_t kSize = static_cast<uint8_t>(SIZE);

	TimestampedRingBuffer() = स्वभाव;
	~TimestampedRingBuffer() = स्वभाव;

	// no copy, assignment, move, move assignment
	TimestampedRingBuffer(ध्रुव TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &चालक=(ध्रुव TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &चालक=(TimestampedRingBuffer &&) = delete;

	तार्किक allocate(माप_प्रकार size) { प्रत्यागम size == SIZE; }

	तार्किक valid() ध्रुव { प्रत्यागम SIZE > 0; }

	शून्य धकेलो(ध्रुव data_type &sample)
	{
		_buffer[_head] = sample;
		_head = next(_head);

		यदि (_entries < kSize) {
			_entries++;
		}
	}

	uint8_t get_length() ध्रुव { प्रत्यागम kSize; }

	data_type &चालक[](ध्रुव uint8_t index) { प्रत्यागम _buffer[index]; }

	ध्रुव data_type &get_newest() ध्रुव { प्रत्यागम _buffer[get_newest_index()]; }
	ध्रुव data_type &get_oldest() ध्रुव { प्रत्यागम _buffer[get_oldest_index()]; }

	uint8_t get_newest_index() ध्रुव { प्रत्यागम (_entries > 0) ? prev(_head) : 0; }
	uint8_t get_oldest_index() ध्रुव
	{
		यदि (_entries == 0) {
			प्रत्यागम 0;
		}

		ध्रुव uint16_t idx = static_cast<uint16_t>(_head) + static_cast<uint16_t>(kSize) - static_cast<uint16_t>(_entries);
		प्रत्यागम (idx >= kSize) ? static_cast<uint8_t>(idx - kSize) : static_cast<uint8_t>(idx);
	}

	uint8_t next(uint8_t i) ध्रुव { प्रत्यागम (i + 1 >= kSize) ? 0 : i + 1; }
	uint8_t prev(uint8_t i) ध्रुव { प्रत्यागम (i == 0) ? kSize - 1 : i - 1; }

	तार्किक निकालो_first_older_than(ध्रुव uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		ध्रुव uint8_t newest_idx = get_newest_index();
		ध्रुव uint8_t oldest_idx = get_oldest_index();

		प्रति (uint8_t i = 0; i < _entries; i++) {
			पूर्णाङ्क index = (newest_idx - i);
			index = index < 0 ? kSize + index : index;

			यदि (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				ध्रुव uint8_t index_u = static_cast<uint8_t>(index);
				ध्रुव uint8_t distance = (index_u >= oldest_idx) ? (index_u - oldest_idx) : (index_u + kSize - oldest_idx);
				ध्रुव uint8_t discard_गिनती = distance + 1;
				_entries = (_entries > discard_गिनती) ? (_entries - discard_गिनती) : 0;
				_buffer[index].time_us = 0;

				प्रत्यागम सत्य;
			}
		}

		प्रत्यागम असत्य;
	}

	पूर्णाङ्क get_total_size() ध्रुव
	{
		static_assert(परिमाण(TimestampedRingBuffer<data_type, SIZE>) <= static_cast<माप_प्रकार>(पूर्णांक_उच्च),
			      "static ring buffer size must fit in int");
		प्रत्यागम static_cast<पूर्णाङ्क>(परिमाण(TimestampedRingBuffer<data_type, SIZE>));
	}

	पूर्णाङ्क entries() ध्रुव { प्रत्यागम _entries; }
	तार्किक empty() ध्रुव { प्रत्यागम _entries == 0; }

	शून्य reset()
	{
		प्रति (uint8_t i = 0; i < kSize; i++) {
			_buffer[i].time_us = 0;
		}

		_head = 0;
		_entries = 0;
	}

गुप्त:
	data_type _buffer[SIZE] {};

	uint8_t _head{0};
	uint8_t _entries{0};
};

/**
 * Dynamic-size ring buffer specialization (SIZE == 0).
 */
ढाँचा <typename data_type>
वर्ग TimestampedRingBuffer<data_type, 0>
{
सार्वजनिक:
	explicit TimestampedRingBuffer(माप_प्रकार size) { allocate(size); }
	TimestampedRingBuffer() = delete;
	~TimestampedRingBuffer() { delete[] _buffer; }

	// no copy, assignment, move, move assignment
	TimestampedRingBuffer(ध्रुव TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &चालक=(ध्रुव TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &चालक=(TimestampedRingBuffer &&) = delete;

	तार्किक allocate(माप_प्रकार size)
	{
		यदि (valid() && (size == _size)) {
			// no change
			प्रत्यागम सत्य;
		}

		यदि (size == 0 || size > UINT8_MAX) {
			प्रत्यागम असत्य;
		}

		delete[] _buffer;

		_buffer = नूतन data_type[size] {};

		यदि (_buffer == nullptr) {
			प्रत्यागम असत्य;
		}

		_size = static_cast<uint8_t>(size);

		// new[]{} already zeroed time_us, only reset the indices
		_head = 0;
		_tail = 0;
		_first_write = सत्य;

		प्रत्यागम सत्य;
	}

	तार्किक valid() ध्रुव { प्रत्यागम (_buffer != nullptr) && (_size > 0); }

	शून्य धकेलो(ध्रुव data_type &sample)
	{
		uint8_t head_new = _head;

		यदि (!_first_write) {
			head_new = next(_head);
		}

		_buffer[head_new] = sample;
		_head = head_new;

		// move tail if we overwrite it
		यदि (_head == _tail && !_first_write) {
			_tail = next(_tail);

		} अन्यथा {
			_first_write = असत्य;
		}
	}

	uint8_t get_length() ध्रुव { प्रत्यागम _size; }

	data_type &चालक[](ध्रुव uint8_t index) { प्रत्यागम _buffer[index]; }

	ध्रुव data_type &get_newest() ध्रुव { प्रत्यागम _buffer[_head]; }
	ध्रुव data_type &get_oldest() ध्रुव { प्रत्यागम _buffer[_tail]; }

	uint8_t get_newest_index() ध्रुव { प्रत्यागम _head; }
	uint8_t get_oldest_index() ध्रुव { प्रत्यागम _tail; }

	uint8_t next(uint8_t i) ध्रुव { प्रत्यागम (_size > 0) ? ((i + 1 >= _size) ? 0 : i + 1) : 0; }
	uint8_t prev(uint8_t i) ध्रुव { प्रत्यागम (_size > 0) ? ((i == 0) ? (_size - 1) : (i - 1)) : 0; }

	तार्किक निकालो_first_older_than(ध्रुव uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		प्रति (uint8_t i = 0; i < _size; i++) {
			पूर्णाङ्क index = (_head - i);
			index = index < 0 ? _size + index : index;

			यदि (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				यदि (index == _head) {
					_tail = _head;
					_first_write = सत्य;

				} अन्यथा {
					_tail = next(static_cast<uint8_t>(index));
				}

				_buffer[index].time_us = 0;

				प्रत्यागम सत्य;
			}

			यदि (index == _tail) {
				// we have reached the tail and haven't got a
				// match
				प्रत्यागम असत्य;
			}
		}

		प्रत्यागम असत्य;
	}

	पूर्णाङ्क get_used_size() ध्रुव { प्रत्यागम परिमाण(*एतत्) + परिमाण(data_type) * entries(); }
	पूर्णाङ्क get_total_size() ध्रुव { प्रत्यागम परिमाण(*एतत्) + परिमाण(data_type) * _size; }

	पूर्णाङ्क entries() ध्रुव
	{
		पूर्णाङ्क गिनती = 0;

		प्रति (uint8_t i = 0; i < _size; i++) {
			यदि (_buffer[i].time_us != 0) {
				गिनती++;
			}
		}

		प्रत्यागम गिनती;
	}

	तार्किक empty() ध्रुव { प्रत्यागम entries() == 0; }

	शून्य reset()
	{
		यदि (_buffer) {
			प्रति (uint8_t i = 0; i < _size; i++) {
				_buffer[i].time_us = 0;
			}

			_head = 0;
			_tail = 0;
			_first_write = सत्य;
		}
	}

गुप्त:
	data_type *_buffer{nullptr};

	uint8_t _head{0};
	uint8_t _tail{0};
	uint8_t _size{0};

	तार्किक _first_write{सत्य};
};
