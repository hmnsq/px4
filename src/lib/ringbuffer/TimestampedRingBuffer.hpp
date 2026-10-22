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
	TimestampedRingBuffer(अचर TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &चालक=(अचर TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &चालक=(TimestampedRingBuffer &&) = delete;

	तार्किक allocate(माप_प्रकार size) { फर्काऊ size == SIZE; }

	तार्किक valid() अचर { फर्काऊ SIZE > 0; }

	शून्य थप(अचर data_type &sample)
	{
		_buffer[_head] = sample;
		_head = next(_head);

		यदि (_entries < kSize) {
			_entries++;
		}
	}

	uint8_t get_length() अचर { फर्काऊ kSize; }

	data_type &चालक[](अचर uint8_t index) { फर्काऊ _buffer[index]; }

	अचर data_type &get_newest() अचर { फर्काऊ _buffer[get_newest_index()]; }
	अचर data_type &get_oldest() अचर { फर्काऊ _buffer[get_oldest_index()]; }

	uint8_t get_newest_index() अचर { फर्काऊ (_entries > 0) ? prev(_head) : 0; }
	uint8_t get_oldest_index() अचर
	{
		यदि (_entries == 0) {
			फर्काऊ 0;
		}

		अचर uint16_t idx = static_cast<uint16_t>(_head) + static_cast<uint16_t>(kSize) - static_cast<uint16_t>(_entries);
		फर्काऊ (idx >= kSize) ? static_cast<uint8_t>(idx - kSize) : static_cast<uint8_t>(idx);
	}

	uint8_t next(uint8_t i) अचर { फर्काऊ (i + 1 >= kSize) ? 0 : i + 1; }
	uint8_t prev(uint8_t i) अचर { फर्काऊ (i == 0) ? kSize - 1 : i - 1; }

	तार्किक झिक_first_older_than(अचर uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		अचर uint8_t newest_idx = get_newest_index();
		अचर uint8_t oldest_idx = get_oldest_index();

		लागि (uint8_t i = 0; i < _entries; i++) {
			पूर्णाङ्क index = (newest_idx - i);
			index = index < 0 ? kSize + index : index;

			यदि (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				अचर uint8_t index_u = static_cast<uint8_t>(index);
				अचर uint8_t distance = (index_u >= oldest_idx) ? (index_u - oldest_idx) : (index_u + kSize - oldest_idx);
				अचर uint8_t discard_गन्ती = distance + 1;
				_entries = (_entries > discard_गन्ती) ? (_entries - discard_गन्ती) : 0;
				_buffer[index].time_us = 0;

				फर्काऊ सत्य;
			}
		}

		फर्काऊ असत्य;
	}

	पूर्णाङ्क get_total_size() अचर
	{
		static_assert(परिमाण(TimestampedRingBuffer<data_type, SIZE>) <= static_cast<माप_प्रकार>(पूर्णांक_उच्च),
			      "static ring buffer size must fit in int");
		फर्काऊ static_cast<पूर्णाङ्क>(परिमाण(TimestampedRingBuffer<data_type, SIZE>));
	}

	पूर्णाङ्क entries() अचर { फर्काऊ _entries; }
	तार्किक empty() अचर { फर्काऊ _entries == 0; }

	शून्य reset()
	{
		लागि (uint8_t i = 0; i < kSize; i++) {
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
	TimestampedRingBuffer(अचर TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &चालक=(अचर TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &चालक=(TimestampedRingBuffer &&) = delete;

	तार्किक allocate(माप_प्रकार size)
	{
		यदि (valid() && (size == _size)) {
			// no change
			फर्काऊ सत्य;
		}

		यदि (size == 0 || size > UINT8_MAX) {
			फर्काऊ असत्य;
		}

		delete[] _buffer;

		_buffer = नूतन data_type[size] {};

		यदि (_buffer == nullptr) {
			फर्काऊ असत्य;
		}

		_size = static_cast<uint8_t>(size);

		// new[]{} already zeroed time_us, only reset the indices
		_head = 0;
		_tail = 0;
		_first_write = सत्य;

		फर्काऊ सत्य;
	}

	तार्किक valid() अचर { फर्काऊ (_buffer != nullptr) && (_size > 0); }

	शून्य थप(अचर data_type &sample)
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

	uint8_t get_length() अचर { फर्काऊ _size; }

	data_type &चालक[](अचर uint8_t index) { फर्काऊ _buffer[index]; }

	अचर data_type &get_newest() अचर { फर्काऊ _buffer[_head]; }
	अचर data_type &get_oldest() अचर { फर्काऊ _buffer[_tail]; }

	uint8_t get_newest_index() अचर { फर्काऊ _head; }
	uint8_t get_oldest_index() अचर { फर्काऊ _tail; }

	uint8_t next(uint8_t i) अचर { फर्काऊ (_size > 0) ? ((i + 1 >= _size) ? 0 : i + 1) : 0; }
	uint8_t prev(uint8_t i) अचर { फर्काऊ (_size > 0) ? ((i == 0) ? (_size - 1) : (i - 1)) : 0; }

	तार्किक झिक_first_older_than(अचर uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		लागि (uint8_t i = 0; i < _size; i++) {
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

				फर्काऊ सत्य;
			}

			यदि (index == _tail) {
				// we have reached the tail and haven't got a
				// match
				फर्काऊ असत्य;
			}
		}

		फर्काऊ असत्य;
	}

	पूर्णाङ्क get_used_size() अचर { फर्काऊ परिमाण(*एतत्) + परिमाण(data_type) * entries(); }
	पूर्णाङ्क get_total_size() अचर { फर्काऊ परिमाण(*एतत्) + परिमाण(data_type) * _size; }

	पूर्णाङ्क entries() अचर
	{
		पूर्णाङ्क गन्ती = 0;

		लागि (uint8_t i = 0; i < _size; i++) {
			यदि (_buffer[i].time_us != 0) {
				गन्ती++;
			}
		}

		फर्काऊ गन्ती;
	}

	तार्किक empty() अचर { फर्काऊ entries() == 0; }

	शून्य reset()
	{
		यदि (_buffer) {
			लागि (uint8_t i = 0; i < _size; i++) {
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
