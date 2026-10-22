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

#અન્તર્ભાવ <climits>
#અન્તર્ભાવ <cstddef>
#અન્તર્ભાવ <inttypes.h>

template <typename data_type, size_t SIZE = 0>
વર્ગ TimestampedRingBuffer;

/**
 * Static-size ring buffer specialization (SIZE > 0).
 */
template <typename data_type, size_t SIZE>
વર્ગ TimestampedRingBuffer
{
જાહેર:
	static_assert(SIZE > 0, "SIZE must be > 0");
	static_assert(SIZE <= UINT8_MAX, "SIZE must fit in uint8_t");
	સ્થિર constexpr uint8_t kSize = static_cast<uint8_t>(SIZE);

	TimestampedRingBuffer() = મૂળભૂત;
	~TimestampedRingBuffer() = મૂળભૂત;

	// no copy, assignment, move, move assignment
	TimestampedRingBuffer(અચલ TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &operator=(અચલ TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &operator=(TimestampedRingBuffer &&) = delete;

	તાર્કિક allocate(size_t size) { પાછા size == SIZE; }

	તાર્કિક valid() અચલ { પાછા SIZE > 0; }

	શૂન્ય ધકેલો(અચલ data_type &sample)
	{
		_buffer[_head] = sample;
		_head = next(_head);

		જો (_entries < kSize) {
			_entries++;
		}
	}

	uint8_t get_length() અચલ { પાછા kSize; }

	data_type &operator[](અચલ uint8_t index) { પાછા _buffer[index]; }

	અચલ data_type &get_newest() અચલ { પાછા _buffer[get_newest_index()]; }
	અચલ data_type &get_oldest() અચલ { પાછા _buffer[get_oldest_index()]; }

	uint8_t get_newest_index() અચલ { પાછા (_entries > 0) ? prev(_head) : 0; }
	uint8_t get_oldest_index() અચલ
	{
		જો (_entries == 0) {
			પાછા 0;
		}

		અચલ uint16_t idx = static_cast<uint16_t>(_head) + static_cast<uint16_t>(kSize) - static_cast<uint16_t>(_entries);
		પાછા (idx >= kSize) ? static_cast<uint8_t>(idx - kSize) : static_cast<uint8_t>(idx);
	}

	uint8_t next(uint8_t i) અચલ { પાછા (i + 1 >= kSize) ? 0 : i + 1; }
	uint8_t prev(uint8_t i) અચલ { પાછા (i == 0) ? kSize - 1 : i - 1; }

	તાર્કિક કાઢો_first_older_than(અચલ uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		અચલ uint8_t newest_idx = get_newest_index();
		અચલ uint8_t oldest_idx = get_oldest_index();

		માટે (uint8_t i = 0; i < _entries; i++) {
			પૂર્ણાંક index = (newest_idx - i);
			index = index < 0 ? kSize + index : index;

			જો (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				અચલ uint8_t index_u = static_cast<uint8_t>(index);
				અચલ uint8_t distance = (index_u >= oldest_idx) ? (index_u - oldest_idx) : (index_u + kSize - oldest_idx);
				અચલ uint8_t discard_ગણતરી = distance + 1;
				_entries = (_entries > discard_ગણતરી) ? (_entries - discard_ગણતરી) : 0;
				_buffer[index].time_us = 0;

				પાછા સાચું;
			}
		}

		પાછા ખોટું;
	}

	પૂર્ણાંક get_total_size() અચલ
	{
		static_assert(કદ(TimestampedRingBuffer<data_type, SIZE>) <= static_cast<size_t>(INT_MAX),
			      "static ring buffer size must fit in int");
		પાછા static_cast<પૂર્ણાંક>(કદ(TimestampedRingBuffer<data_type, SIZE>));
	}

	પૂર્ણાંક entries() અચલ { પાછા _entries; }
	તાર્કિક empty() અચલ { પાછા _entries == 0; }

	શૂન્ય reset()
	{
		માટે (uint8_t i = 0; i < kSize; i++) {
			_buffer[i].time_us = 0;
		}

		_head = 0;
		_entries = 0;
	}

ખાનગી:
	data_type _buffer[SIZE] {};

	uint8_t _head{0};
	uint8_t _entries{0};
};

/**
 * Dynamic-size ring buffer specialization (SIZE == 0).
 */
template <typename data_type>
વર્ગ TimestampedRingBuffer<data_type, 0>
{
જાહેર:
	explicit TimestampedRingBuffer(size_t size) { allocate(size); }
	TimestampedRingBuffer() = delete;
	~TimestampedRingBuffer() { delete[] _buffer; }

	// no copy, assignment, move, move assignment
	TimestampedRingBuffer(અચલ TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &operator=(અચલ TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &operator=(TimestampedRingBuffer &&) = delete;

	તાર્કિક allocate(size_t size)
	{
		જો (valid() && (size == _size)) {
			// no change
			પાછા સાચું;
		}

		જો (size == 0 || size > UINT8_MAX) {
			પાછા ખોટું;
		}

		delete[] _buffer;

		_buffer = નવું data_type[size] {};

		જો (_buffer == nullptr) {
			પાછા ખોટું;
		}

		_size = static_cast<uint8_t>(size);

		// new[]{} already zeroed time_us, only reset the indices
		_head = 0;
		_tail = 0;
		_first_write = સાચું;

		પાછા સાચું;
	}

	તાર્કિક valid() અચલ { પાછા (_buffer != nullptr) && (_size > 0); }

	શૂન્ય ધકેલો(અચલ data_type &sample)
	{
		uint8_t head_new = _head;

		જો (!_first_write) {
			head_new = next(_head);
		}

		_buffer[head_new] = sample;
		_head = head_new;

		// move tail if we overwrite it
		જો (_head == _tail && !_first_write) {
			_tail = next(_tail);

		} નહીંતર {
			_first_write = ખોટું;
		}
	}

	uint8_t get_length() અચલ { પાછા _size; }

	data_type &operator[](અચલ uint8_t index) { પાછા _buffer[index]; }

	અચલ data_type &get_newest() અચલ { પાછા _buffer[_head]; }
	અચલ data_type &get_oldest() અચલ { પાછા _buffer[_tail]; }

	uint8_t get_newest_index() અચલ { પાછા _head; }
	uint8_t get_oldest_index() અચલ { પાછા _tail; }

	uint8_t next(uint8_t i) અચલ { પાછા (_size > 0) ? ((i + 1 >= _size) ? 0 : i + 1) : 0; }
	uint8_t prev(uint8_t i) અચલ { પાછા (_size > 0) ? ((i == 0) ? (_size - 1) : (i - 1)) : 0; }

	તાર્કિક કાઢો_first_older_than(અચલ uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		માટે (uint8_t i = 0; i < _size; i++) {
			પૂર્ણાંક index = (_head - i);
			index = index < 0 ? _size + index : index;

			જો (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				જો (index == _head) {
					_tail = _head;
					_first_write = સાચું;

				} નહીંતર {
					_tail = next(static_cast<uint8_t>(index));
				}

				_buffer[index].time_us = 0;

				પાછા સાચું;
			}

			જો (index == _tail) {
				// we have reached the tail and haven't got a
				// match
				પાછા ખોટું;
			}
		}

		પાછા ખોટું;
	}

	પૂર્ણાંક get_used_size() અચલ { પાછા કદ(*આ) + કદ(data_type) * entries(); }
	પૂર્ણાંક get_total_size() અચલ { પાછા કદ(*આ) + કદ(data_type) * _size; }

	પૂર્ણાંક entries() અચલ
	{
		પૂર્ણાંક ગણતરી = 0;

		માટે (uint8_t i = 0; i < _size; i++) {
			જો (_buffer[i].time_us != 0) {
				ગણતરી++;
			}
		}

		પાછા ગણતરી;
	}

	તાર્કિક empty() અચલ { પાછા entries() == 0; }

	શૂન્ય reset()
	{
		જો (_buffer) {
			માટે (uint8_t i = 0; i < _size; i++) {
				_buffer[i].time_us = 0;
			}

			_head = 0;
			_tail = 0;
			_first_write = સાચું;
		}
	}

ખાનગી:
	data_type *_buffer{nullptr};

	uint8_t _head{0};
	uint8_t _tail{0};
	uint8_t _size{0};

	તાર્કિક _first_write{સાચું};
};
