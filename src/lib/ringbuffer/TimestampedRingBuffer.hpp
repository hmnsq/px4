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

#ಅನ್ತರ್ಭಾವ <climits>
#ಅನ್ತರ್ಭಾವ <cstddef>
#ಅನ್ತರ್ಭಾವ <inttypes.h>

template <typename data_type, size_t SIZE = 0>
ವರ್ಗ TimestampedRingBuffer;

/**
 * Static-size ring buffer specialization (SIZE > 0).
 */
template <typename data_type, size_t SIZE>
ವರ್ಗ TimestampedRingBuffer
{
ಸಾರ್ವಜನಿಕ:
	static_assert(SIZE > 0, "SIZE must be > 0");
	static_assert(SIZE <= UINT8_MAX, "SIZE must fit in uint8_t");
	ಸ್ಥಿರ constexpr uint8_t kSize = static_cast<uint8_t>(SIZE);

	TimestampedRingBuffer() = ಸ್ವಭಾವ;
	~TimestampedRingBuffer() = ಸ್ವಭಾವ;

	// no copy, assignment, move, move assignment
	TimestampedRingBuffer(ಧ್ರುವ TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &operator=(ಧ್ರುವ TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &operator=(TimestampedRingBuffer &&) = delete;

	ತಾರ್ಕಿಕ allocate(size_t size) { ಪ್ರತ್ಯಾಗಮ size == SIZE; }

	ತಾರ್ಕಿಕ valid() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ SIZE > 0; }

	ಶೂನ್ಯ ತಳ್ಳು(ಧ್ರುವ data_type &sample)
	{
		_buffer[_head] = sample;
		_head = next(_head);

		ಯದಿ (_entries < kSize) {
			_entries++;
		}
	}

	uint8_t get_length() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ kSize; }

	data_type &operator[](ಧ್ರುವ uint8_t index) { ಪ್ರತ್ಯಾಗಮ _buffer[index]; }

	ಧ್ರುವ data_type &get_newest() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _buffer[get_newest_index()]; }
	ಧ್ರುವ data_type &get_oldest() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _buffer[get_oldest_index()]; }

	uint8_t get_newest_index() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ (_entries > 0) ? prev(_head) : 0; }
	uint8_t get_oldest_index() ಧ್ರುವ
	{
		ಯದಿ (_entries == 0) {
			ಪ್ರತ್ಯಾಗಮ 0;
		}

		ಧ್ರುವ uint16_t idx = static_cast<uint16_t>(_head) + static_cast<uint16_t>(kSize) - static_cast<uint16_t>(_entries);
		ಪ್ರತ್ಯಾಗಮ (idx >= kSize) ? static_cast<uint8_t>(idx - kSize) : static_cast<uint8_t>(idx);
	}

	uint8_t next(uint8_t i) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ (i + 1 >= kSize) ? 0 : i + 1; }
	uint8_t prev(uint8_t i) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ (i == 0) ? kSize - 1 : i - 1; }

	ತಾರ್ಕಿಕ ತೆಗೆ_first_older_than(ಧ್ರುವ uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		ಧ್ರುವ uint8_t newest_idx = get_newest_index();
		ಧ್ರುವ uint8_t oldest_idx = get_oldest_index();

		ಪ್ರತಿ (uint8_t i = 0; i < _entries; i++) {
			ಪೂರ್ಣಾಙ್ಕ index = (newest_idx - i);
			index = index < 0 ? kSize + index : index;

			ಯದಿ (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				ಧ್ರುವ uint8_t index_u = static_cast<uint8_t>(index);
				ಧ್ರುವ uint8_t distance = (index_u >= oldest_idx) ? (index_u - oldest_idx) : (index_u + kSize - oldest_idx);
				ಧ್ರುವ uint8_t discard_ಎಣಿಕೆ = distance + 1;
				_entries = (_entries > discard_ಎಣಿಕೆ) ? (_entries - discard_ಎಣಿಕೆ) : 0;
				_buffer[index].time_us = 0;

				ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
			}
		}

		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	ಪೂರ್ಣಾಙ್ಕ get_total_size() ಧ್ರುವ
	{
		static_assert(ಪರಿಮಾಣ(TimestampedRingBuffer<data_type, SIZE>) <= static_cast<size_t>(INT_MAX),
			      "static ring buffer size must fit in int");
		ಪ್ರತ್ಯಾಗಮ static_cast<ಪೂರ್ಣಾಙ್ಕ>(ಪರಿಮಾಣ(TimestampedRingBuffer<data_type, SIZE>));
	}

	ಪೂರ್ಣಾಙ್ಕ entries() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _entries; }
	ತಾರ್ಕಿಕ empty() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _entries == 0; }

	ಶೂನ್ಯ reset()
	{
		ಪ್ರತಿ (uint8_t i = 0; i < kSize; i++) {
			_buffer[i].time_us = 0;
		}

		_head = 0;
		_entries = 0;
	}

ಗುಪ್ತ:
	data_type _buffer[SIZE] {};

	uint8_t _head{0};
	uint8_t _entries{0};
};

/**
 * Dynamic-size ring buffer specialization (SIZE == 0).
 */
template <typename data_type>
ವರ್ಗ TimestampedRingBuffer<data_type, 0>
{
ಸಾರ್ವಜನಿಕ:
	explicit TimestampedRingBuffer(size_t size) { allocate(size); }
	TimestampedRingBuffer() = delete;
	~TimestampedRingBuffer() { delete[] _buffer; }

	// no copy, assignment, move, move assignment
	TimestampedRingBuffer(ಧ್ರುವ TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &operator=(ಧ್ರುವ TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &operator=(TimestampedRingBuffer &&) = delete;

	ತಾರ್ಕಿಕ allocate(size_t size)
	{
		ಯದಿ (valid() && (size == _size)) {
			// no change
			ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
		}

		ಯದಿ (size == 0 || size > UINT8_MAX) {
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		}

		delete[] _buffer;

		_buffer = ನೂತನ data_type[size] {};

		ಯದಿ (_buffer == nullptr) {
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		}

		_size = static_cast<uint8_t>(size);

		// new[]{} already zeroed time_us, only reset the indices
		_head = 0;
		_tail = 0;
		_first_write = ಸತ್ಯ;

		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ತಾರ್ಕಿಕ valid() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ (_buffer != nullptr) && (_size > 0); }

	ಶೂನ್ಯ ತಳ್ಳು(ಧ್ರುವ data_type &sample)
	{
		uint8_t head_new = _head;

		ಯದಿ (!_first_write) {
			head_new = next(_head);
		}

		_buffer[head_new] = sample;
		_head = head_new;

		// move tail if we overwrite it
		ಯದಿ (_head == _tail && !_first_write) {
			_tail = next(_tail);

		} ಅನ್ಯಥಾ {
			_first_write = ಅಸತ್ಯ;
		}
	}

	uint8_t get_length() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _size; }

	data_type &operator[](ಧ್ರುವ uint8_t index) { ಪ್ರತ್ಯಾಗಮ _buffer[index]; }

	ಧ್ರುವ data_type &get_newest() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _buffer[_head]; }
	ಧ್ರುವ data_type &get_oldest() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _buffer[_tail]; }

	uint8_t get_newest_index() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _head; }
	uint8_t get_oldest_index() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _tail; }

	uint8_t next(uint8_t i) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ (_size > 0) ? ((i + 1 >= _size) ? 0 : i + 1) : 0; }
	uint8_t prev(uint8_t i) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ (_size > 0) ? ((i == 0) ? (_size - 1) : (i - 1)) : 0; }

	ತಾರ್ಕಿಕ ತೆಗೆ_first_older_than(ಧ್ರುವ uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		ಪ್ರತಿ (uint8_t i = 0; i < _size; i++) {
			ಪೂರ್ಣಾಙ್ಕ index = (_head - i);
			index = index < 0 ? _size + index : index;

			ಯದಿ (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				ಯದಿ (index == _head) {
					_tail = _head;
					_first_write = ಸತ್ಯ;

				} ಅನ್ಯಥಾ {
					_tail = next(static_cast<uint8_t>(index));
				}

				_buffer[index].time_us = 0;

				ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
			}

			ಯದಿ (index == _tail) {
				// we have reached the tail and haven't got a
				// match
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			}
		}

		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	ಪೂರ್ಣಾಙ್ಕ get_used_size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(*ಏತತ್) + ಪರಿಮಾಣ(data_type) * entries(); }
	ಪೂರ್ಣಾಙ್ಕ get_total_size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(*ಏತತ್) + ಪರಿಮಾಣ(data_type) * _size; }

	ಪೂರ್ಣಾಙ್ಕ entries() ಧ್ರುವ
	{
		ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ = 0;

		ಪ್ರತಿ (uint8_t i = 0; i < _size; i++) {
			ಯದಿ (_buffer[i].time_us != 0) {
				ಎಣಿಕೆ++;
			}
		}

		ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ;
	}

	ತಾರ್ಕಿಕ empty() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ entries() == 0; }

	ಶೂನ್ಯ reset()
	{
		ಯದಿ (_buffer) {
			ಪ್ರತಿ (uint8_t i = 0; i < _size; i++) {
				_buffer[i].time_us = 0;
			}

			_head = 0;
			_tail = 0;
			_first_write = ಸತ್ಯ;
		}
	}

ಗುಪ್ತ:
	data_type *_buffer{nullptr};

	uint8_t _head{0};
	uint8_t _tail{0};
	uint8_t _size{0};

	ತಾರ್ಕಿಕ _first_write{ಸತ್ಯ};
};
