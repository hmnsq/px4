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

#அந்தர்பாவ <climits>
#அந்தர்பாவ <cstddef>
#அந்தர்பாவ <inttypes.h>

template <typename data_type, size_t SIZE = 0>
வகுப்பு TimestampedRingBuffer;

/**
 * Static-size ring buffer specialization (SIZE > 0).
 */
template <typename data_type, size_t SIZE>
வகுப்பு TimestampedRingBuffer
{
பொது:
	static_assert(SIZE > 0, "SIZE must be > 0");
	static_assert(SIZE <= UINT8_MAX, "SIZE must fit in uint8_t");
	நிலையான constexpr uint8_t kSize = static_cast<uint8_t>(SIZE);

	TimestampedRingBuffer() = இயல்பு;
	~TimestampedRingBuffer() = இயல்பு;

	// no copy, assignment, move, move assignment
	TimestampedRingBuffer(மாறிலி TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &operator=(மாறிலி TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &operator=(TimestampedRingBuffer &&) = delete;

	தருக்க allocate(size_t size) { திரும்பு size == SIZE; }

	தருக்க valid() மாறிலி { திரும்பு SIZE > 0; }

	வெறுமை தள்ளு(மாறிலி data_type &sample)
	{
		_buffer[_head] = sample;
		_head = next(_head);

		எனில் (_entries < kSize) {
			_entries++;
		}
	}

	uint8_t get_length() மாறிலி { திரும்பு kSize; }

	data_type &operator[](மாறிலி uint8_t index) { திரும்பு _buffer[index]; }

	மாறிலி data_type &get_newest() மாறிலி { திரும்பு _buffer[get_newest_index()]; }
	மாறிலி data_type &get_oldest() மாறிலி { திரும்பு _buffer[get_oldest_index()]; }

	uint8_t get_newest_index() மாறிலி { திரும்பு (_entries > 0) ? prev(_head) : 0; }
	uint8_t get_oldest_index() மாறிலி
	{
		எனில் (_entries == 0) {
			திரும்பு 0;
		}

		மாறிலி uint16_t idx = static_cast<uint16_t>(_head) + static_cast<uint16_t>(kSize) - static_cast<uint16_t>(_entries);
		திரும்பு (idx >= kSize) ? static_cast<uint8_t>(idx - kSize) : static_cast<uint8_t>(idx);
	}

	uint8_t next(uint8_t i) மாறிலி { திரும்பு (i + 1 >= kSize) ? 0 : i + 1; }
	uint8_t prev(uint8_t i) மாறிலி { திரும்பு (i == 0) ? kSize - 1 : i - 1; }

	தருக்க எடு_first_older_than(மாறிலி uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		மாறிலி uint8_t newest_idx = get_newest_index();
		மாறிலி uint8_t oldest_idx = get_oldest_index();

		க்கு (uint8_t i = 0; i < _entries; i++) {
			முழு index = (newest_idx - i);
			index = index < 0 ? kSize + index : index;

			எனில் (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				மாறிலி uint8_t index_u = static_cast<uint8_t>(index);
				மாறிலி uint8_t distance = (index_u >= oldest_idx) ? (index_u - oldest_idx) : (index_u + kSize - oldest_idx);
				மாறிலி uint8_t discard_மொத்தம் = distance + 1;
				_entries = (_entries > discard_மொத்தம்) ? (_entries - discard_மொத்தம்) : 0;
				_buffer[index].time_us = 0;

				திரும்பு உண்மை;
			}
		}

		திரும்பு பொய்;
	}

	முழு get_total_size() மாறிலி
	{
		static_assert(அளவு(TimestampedRingBuffer<data_type, SIZE>) <= static_cast<size_t>(INT_MAX),
			      "static ring buffer size must fit in int");
		திரும்பு static_cast<முழு>(அளவு(TimestampedRingBuffer<data_type, SIZE>));
	}

	முழு entries() மாறிலி { திரும்பு _entries; }
	தருக்க empty() மாறிலி { திரும்பு _entries == 0; }

	வெறுமை reset()
	{
		க்கு (uint8_t i = 0; i < kSize; i++) {
			_buffer[i].time_us = 0;
		}

		_head = 0;
		_entries = 0;
	}

தனிப்பட்ட:
	data_type _buffer[SIZE] {};

	uint8_t _head{0};
	uint8_t _entries{0};
};

/**
 * Dynamic-size ring buffer specialization (SIZE == 0).
 */
template <typename data_type>
வகுப்பு TimestampedRingBuffer<data_type, 0>
{
பொது:
	explicit TimestampedRingBuffer(size_t size) { allocate(size); }
	TimestampedRingBuffer() = delete;
	~TimestampedRingBuffer() { delete[] _buffer; }

	// no copy, assignment, move, move assignment
	TimestampedRingBuffer(மாறிலி TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &operator=(மாறிலி TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &operator=(TimestampedRingBuffer &&) = delete;

	தருக்க allocate(size_t size)
	{
		எனில் (valid() && (size == _size)) {
			// no change
			திரும்பு உண்மை;
		}

		எனில் (size == 0 || size > UINT8_MAX) {
			திரும்பு பொய்;
		}

		delete[] _buffer;

		_buffer = புதிய data_type[size] {};

		எனில் (_buffer == nullptr) {
			திரும்பு பொய்;
		}

		_size = static_cast<uint8_t>(size);

		// new[]{} already zeroed time_us, only reset the indices
		_head = 0;
		_tail = 0;
		_first_write = உண்மை;

		திரும்பு உண்மை;
	}

	தருக்க valid() மாறிலி { திரும்பு (_buffer != nullptr) && (_size > 0); }

	வெறுமை தள்ளு(மாறிலி data_type &sample)
	{
		uint8_t head_new = _head;

		எனில் (!_first_write) {
			head_new = next(_head);
		}

		_buffer[head_new] = sample;
		_head = head_new;

		// move tail if we overwrite it
		எனில் (_head == _tail && !_first_write) {
			_tail = next(_tail);

		} இல்லையெனில் {
			_first_write = பொய்;
		}
	}

	uint8_t get_length() மாறிலி { திரும்பு _size; }

	data_type &operator[](மாறிலி uint8_t index) { திரும்பு _buffer[index]; }

	மாறிலி data_type &get_newest() மாறிலி { திரும்பு _buffer[_head]; }
	மாறிலி data_type &get_oldest() மாறிலி { திரும்பு _buffer[_tail]; }

	uint8_t get_newest_index() மாறிலி { திரும்பு _head; }
	uint8_t get_oldest_index() மாறிலி { திரும்பு _tail; }

	uint8_t next(uint8_t i) மாறிலி { திரும்பு (_size > 0) ? ((i + 1 >= _size) ? 0 : i + 1) : 0; }
	uint8_t prev(uint8_t i) மாறிலி { திரும்பு (_size > 0) ? ((i == 0) ? (_size - 1) : (i - 1)) : 0; }

	தருக்க எடு_first_older_than(மாறிலி uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		க்கு (uint8_t i = 0; i < _size; i++) {
			முழு index = (_head - i);
			index = index < 0 ? _size + index : index;

			எனில் (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				எனில் (index == _head) {
					_tail = _head;
					_first_write = உண்மை;

				} இல்லையெனில் {
					_tail = next(static_cast<uint8_t>(index));
				}

				_buffer[index].time_us = 0;

				திரும்பு உண்மை;
			}

			எனில் (index == _tail) {
				// we have reached the tail and haven't got a
				// match
				திரும்பு பொய்;
			}
		}

		திரும்பு பொய்;
	}

	முழு get_used_size() மாறிலி { திரும்பு அளவு(*இது) + அளவு(data_type) * entries(); }
	முழு get_total_size() மாறிலி { திரும்பு அளவு(*இது) + அளவு(data_type) * _size; }

	முழு entries() மாறிலி
	{
		முழு மொத்தம் = 0;

		க்கு (uint8_t i = 0; i < _size; i++) {
			எனில் (_buffer[i].time_us != 0) {
				மொத்தம்++;
			}
		}

		திரும்பு மொத்தம்;
	}

	தருக்க empty() மாறிலி { திரும்பு entries() == 0; }

	வெறுமை reset()
	{
		எனில் (_buffer) {
			க்கு (uint8_t i = 0; i < _size; i++) {
				_buffer[i].time_us = 0;
			}

			_head = 0;
			_tail = 0;
			_first_write = உண்மை;
		}
	}

தனிப்பட்ட:
	data_type *_buffer{nullptr};

	uint8_t _head{0};
	uint8_t _tail{0};
	uint8_t _size{0};

	தருக்க _first_write{உண்மை};
};
