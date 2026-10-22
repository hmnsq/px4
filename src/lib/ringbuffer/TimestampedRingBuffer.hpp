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

#شامل <climits>
#شامل <cstddef>
#شامل <inttypes.h>

template <typename data_type, size_t SIZE = 0>
رده TimestampedRingBuffer;

/**
 * Static-size ring buffer specialization (SIZE > 0).
 */
template <typename data_type, size_t SIZE>
رده TimestampedRingBuffer
{
عمومی:
	static_assert(SIZE > 0, "SIZE must be > 0");
	static_assert(SIZE <= UINT8_MAX, "SIZE must fit in uint8_t");
	ایستا constexpr uint8_t kSize = static_cast<uint8_t>(SIZE);

	TimestampedRingBuffer() = پیشفرض;
	~TimestampedRingBuffer() = پیشفرض;

	// no copy, assignment, move, move assignment
	TimestampedRingBuffer(ثابت TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &operator=(ثابت TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &operator=(TimestampedRingBuffer &&) = delete;

	منطقی allocate(size_t size) { بازگشت size == SIZE; }

	منطقی valid() ثابت { بازگشت SIZE > 0; }

	تهی افزودن(ثابت data_type &sample)
	{
		_buffer[_head] = sample;
		_head = next(_head);

		اگر (_entries < kSize) {
			_entries++;
		}
	}

	uint8_t get_length() ثابت { بازگشت kSize; }

	data_type &operator[](ثابت uint8_t index) { بازگشت _buffer[index]; }

	ثابت data_type &get_newest() ثابت { بازگشت _buffer[get_newest_index()]; }
	ثابت data_type &get_oldest() ثابت { بازگشت _buffer[get_oldest_index()]; }

	uint8_t get_newest_index() ثابت { بازگشت (_entries > 0) ? prev(_head) : 0; }
	uint8_t get_oldest_index() ثابت
	{
		اگر (_entries == 0) {
			بازگشت 0;
		}

		ثابت uint16_t idx = static_cast<uint16_t>(_head) + static_cast<uint16_t>(kSize) - static_cast<uint16_t>(_entries);
		بازگشت (idx >= kSize) ? static_cast<uint8_t>(idx - kSize) : static_cast<uint8_t>(idx);
	}

	uint8_t next(uint8_t i) ثابت { بازگشت (i + 1 >= kSize) ? 0 : i + 1; }
	uint8_t prev(uint8_t i) ثابت { بازگشت (i == 0) ? kSize - 1 : i - 1; }

	منطقی برداشت_first_older_than(ثابت uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		ثابت uint8_t newest_idx = get_newest_index();
		ثابت uint8_t oldest_idx = get_oldest_index();

		برای (uint8_t i = 0; i < _entries; i++) {
			صحیح index = (newest_idx - i);
			index = index < 0 ? kSize + index : index;

			اگر (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				ثابت uint8_t index_u = static_cast<uint8_t>(index);
				ثابت uint8_t distance = (index_u >= oldest_idx) ? (index_u - oldest_idx) : (index_u + kSize - oldest_idx);
				ثابت uint8_t discard_شمار = distance + 1;
				_entries = (_entries > discard_شمار) ? (_entries - discard_شمار) : 0;
				_buffer[index].time_us = 0;

				بازگشت درست;
			}
		}

		بازگشت نادرست;
	}

	صحیح get_total_size() ثابت
	{
		static_assert(اندازه(TimestampedRingBuffer<data_type, SIZE>) <= static_cast<size_t>(INT_MAX),
			      "static ring buffer size must fit in int");
		بازگشت static_cast<صحیح>(اندازه(TimestampedRingBuffer<data_type, SIZE>));
	}

	صحیح entries() ثابت { بازگشت _entries; }
	منطقی empty() ثابت { بازگشت _entries == 0; }

	تهی reset()
	{
		برای (uint8_t i = 0; i < kSize; i++) {
			_buffer[i].time_us = 0;
		}

		_head = 0;
		_entries = 0;
	}

خصوصی:
	data_type _buffer[SIZE] {};

	uint8_t _head{0};
	uint8_t _entries{0};
};

/**
 * Dynamic-size ring buffer specialization (SIZE == 0).
 */
template <typename data_type>
رده TimestampedRingBuffer<data_type, 0>
{
عمومی:
	explicit TimestampedRingBuffer(size_t size) { allocate(size); }
	TimestampedRingBuffer() = delete;
	~TimestampedRingBuffer() { delete[] _buffer; }

	// no copy, assignment, move, move assignment
	TimestampedRingBuffer(ثابت TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &operator=(ثابت TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &operator=(TimestampedRingBuffer &&) = delete;

	منطقی allocate(size_t size)
	{
		اگر (valid() && (size == _size)) {
			// no change
			بازگشت درست;
		}

		اگر (size == 0 || size > UINT8_MAX) {
			بازگشت نادرست;
		}

		delete[] _buffer;

		_buffer = جدید data_type[size] {};

		اگر (_buffer == nullptr) {
			بازگشت نادرست;
		}

		_size = static_cast<uint8_t>(size);

		// new[]{} already zeroed time_us, only reset the indices
		_head = 0;
		_tail = 0;
		_first_write = درست;

		بازگشت درست;
	}

	منطقی valid() ثابت { بازگشت (_buffer != nullptr) && (_size > 0); }

	تهی افزودن(ثابت data_type &sample)
	{
		uint8_t head_new = _head;

		اگر (!_first_write) {
			head_new = next(_head);
		}

		_buffer[head_new] = sample;
		_head = head_new;

		// move tail if we overwrite it
		اگر (_head == _tail && !_first_write) {
			_tail = next(_tail);

		} وگرنه {
			_first_write = نادرست;
		}
	}

	uint8_t get_length() ثابت { بازگشت _size; }

	data_type &operator[](ثابت uint8_t index) { بازگشت _buffer[index]; }

	ثابت data_type &get_newest() ثابت { بازگشت _buffer[_head]; }
	ثابت data_type &get_oldest() ثابت { بازگشت _buffer[_tail]; }

	uint8_t get_newest_index() ثابت { بازگشت _head; }
	uint8_t get_oldest_index() ثابت { بازگشت _tail; }

	uint8_t next(uint8_t i) ثابت { بازگشت (_size > 0) ? ((i + 1 >= _size) ? 0 : i + 1) : 0; }
	uint8_t prev(uint8_t i) ثابت { بازگشت (_size > 0) ? ((i == 0) ? (_size - 1) : (i - 1)) : 0; }

	منطقی برداشت_first_older_than(ثابت uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		برای (uint8_t i = 0; i < _size; i++) {
			صحیح index = (_head - i);
			index = index < 0 ? _size + index : index;

			اگر (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				اگر (index == _head) {
					_tail = _head;
					_first_write = درست;

				} وگرنه {
					_tail = next(static_cast<uint8_t>(index));
				}

				_buffer[index].time_us = 0;

				بازگشت درست;
			}

			اگر (index == _tail) {
				// we have reached the tail and haven't got a
				// match
				بازگشت نادرست;
			}
		}

		بازگشت نادرست;
	}

	صحیح get_used_size() ثابت { بازگشت اندازه(*این) + اندازه(data_type) * entries(); }
	صحیح get_total_size() ثابت { بازگشت اندازه(*این) + اندازه(data_type) * _size; }

	صحیح entries() ثابت
	{
		صحیح شمار = 0;

		برای (uint8_t i = 0; i < _size; i++) {
			اگر (_buffer[i].time_us != 0) {
				شمار++;
			}
		}

		بازگشت شمار;
	}

	منطقی empty() ثابت { بازگشت entries() == 0; }

	تهی reset()
	{
		اگر (_buffer) {
			برای (uint8_t i = 0; i < _size; i++) {
				_buffer[i].time_us = 0;
			}

			_head = 0;
			_tail = 0;
			_first_write = درست;
		}
	}

خصوصی:
	data_type *_buffer{nullptr};

	uint8_t _head{0};
	uint8_t _tail{0};
	uint8_t _size{0};

	منطقی _first_write{درست};
};
