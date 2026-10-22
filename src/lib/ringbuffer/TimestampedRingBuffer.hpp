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

#ضمن <climits>
#ضمن <cstddef>
#ضمن <inttypes.h>

template <typename data_type, size_t SIZE = 0>
صنف TimestampedRingBuffer;

/**
 * Static-size ring buffer specialization (SIZE > 0).
 */
template <typename data_type, size_t SIZE>
صنف TimestampedRingBuffer
{
عام:
	static_assert(SIZE > 0, "SIZE must be > 0");
	static_assert(SIZE <= UINT8_MAX, "SIZE must fit in uint8_t");
	ساكن constexpr uint8_t kSize = static_cast<uint8_t>(SIZE);

	TimestampedRingBuffer() = افتراضي;
	~TimestampedRingBuffer() = افتراضي;

	// no copy, assignment, move, move assignment
	TimestampedRingBuffer(ثابت TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &operator=(ثابت TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &operator=(TimestampedRingBuffer &&) = delete;

	منطقي allocate(size_t size) { أرجع size == SIZE; }

	منطقي valid() ثابت { أرجع SIZE > 0; }

	فراغ push(ثابت data_type &sample)
	{
		_buffer[_head] = sample;
		_head = next(_head);

		𐡄𐡍 (_entries < kSize) {
			_entries++;
		}
	}

	uint8_t get_length() ثابت { أرجع kSize; }

	data_type &operator[](ثابت uint8_t index) { أرجع _buffer[index]; }

	ثابت data_type &get_newest() ثابت { أرجع _buffer[get_newest_index()]; }
	ثابت data_type &get_oldest() ثابت { أرجع _buffer[get_oldest_index()]; }

	uint8_t get_newest_index() ثابت { أرجع (_entries > 0) ? prev(_head) : 0; }
	uint8_t get_oldest_index() ثابت
	{
		𐡄𐡍 (_entries == 0) {
			أرجع 0;
		}

		ثابت uint16_t idx = static_cast<uint16_t>(_head) + static_cast<uint16_t>(kSize) - static_cast<uint16_t>(_entries);
		أرجع (idx >= kSize) ? static_cast<uint8_t>(idx - kSize) : static_cast<uint8_t>(idx);
	}

	uint8_t next(uint8_t i) ثابت { أرجع (i + 1 >= kSize) ? 0 : i + 1; }
	uint8_t prev(uint8_t i) ثابت { أرجع (i == 0) ? kSize - 1 : i - 1; }

	منطقي pop_first_older_than(ثابت uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		ثابت uint8_t newest_idx = get_newest_index();
		ثابت uint8_t oldest_idx = get_oldest_index();

		لكل (uint8_t i = 0; i < _entries; i++) {
			صحيح index = (newest_idx - i);
			index = index < 0 ? kSize + index : index;

			𐡄𐡍 (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				ثابت uint8_t index_u = static_cast<uint8_t>(index);
				ثابت uint8_t distance = (index_u >= oldest_idx) ? (index_u - oldest_idx) : (index_u + kSize - oldest_idx);
				ثابت uint8_t discard_count = distance + 1;
				_entries = (_entries > discard_count) ? (_entries - discard_count) : 0;
				_buffer[index].time_us = 0;

				أرجع 𐡒𐡔𐡈;
			}
		}

		أرجع خطأ;
	}

	صحيح get_total_size() ثابت
	{
		static_assert(حجم(TimestampedRingBuffer<data_type, SIZE>) <= static_cast<size_t>(INT_MAX),
			      "static ring buffer size must fit in int");
		أرجع static_cast<صحيح>(حجم(TimestampedRingBuffer<data_type, SIZE>));
	}

	صحيح entries() ثابت { أرجع _entries; }
	منطقي empty() ثابت { أرجع _entries == 0; }

	فراغ reset()
	{
		لكل (uint8_t i = 0; i < kSize; i++) {
			_buffer[i].time_us = 0;
		}

		_head = 0;
		_entries = 0;
	}

خاص:
	data_type _buffer[SIZE] {};

	uint8_t _head{0};
	uint8_t _entries{0};
};

/**
 * Dynamic-size ring buffer specialization (SIZE == 0).
 */
template <typename data_type>
صنف TimestampedRingBuffer<data_type, 0>
{
عام:
	explicit TimestampedRingBuffer(size_t size) { allocate(size); }
	TimestampedRingBuffer() = delete;
	~TimestampedRingBuffer() { delete[] _buffer; }

	// no copy, assignment, move, move assignment
	TimestampedRingBuffer(ثابت TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &operator=(ثابت TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &operator=(TimestampedRingBuffer &&) = delete;

	منطقي allocate(size_t size)
	{
		𐡄𐡍 (valid() && (size == _size)) {
			// no change
			أرجع 𐡒𐡔𐡈;
		}

		𐡄𐡍 (size == 0 || size > UINT8_MAX) {
			أرجع خطأ;
		}

		delete[] _buffer;

		_buffer = جديد data_type[size] {};

		𐡄𐡍 (_buffer == nullptr) {
			أرجع خطأ;
		}

		_size = static_cast<uint8_t>(size);

		// new[]{} already zeroed time_us, only reset the indices
		_head = 0;
		_tail = 0;
		_first_write = 𐡒𐡔𐡈;

		أرجع 𐡒𐡔𐡈;
	}

	منطقي valid() ثابت { أرجع (_buffer != nullptr) && (_size > 0); }

	فراغ push(ثابت data_type &sample)
	{
		uint8_t head_new = _head;

		𐡄𐡍 (!_first_write) {
			head_new = next(_head);
		}

		_buffer[head_new] = sample;
		_head = head_new;

		// move tail if we overwrite it
		𐡄𐡍 (_head == _tail && !_first_write) {
			_tail = next(_tail);

		} وإلا {
			_first_write = خطأ;
		}
	}

	uint8_t get_length() ثابت { أرجع _size; }

	data_type &operator[](ثابت uint8_t index) { أرجع _buffer[index]; }

	ثابت data_type &get_newest() ثابت { أرجع _buffer[_head]; }
	ثابت data_type &get_oldest() ثابت { أرجع _buffer[_tail]; }

	uint8_t get_newest_index() ثابت { أرجع _head; }
	uint8_t get_oldest_index() ثابت { أرجع _tail; }

	uint8_t next(uint8_t i) ثابت { أرجع (_size > 0) ? ((i + 1 >= _size) ? 0 : i + 1) : 0; }
	uint8_t prev(uint8_t i) ثابت { أرجع (_size > 0) ? ((i == 0) ? (_size - 1) : (i - 1)) : 0; }

	منطقي pop_first_older_than(ثابت uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		لكل (uint8_t i = 0; i < _size; i++) {
			صحيح index = (_head - i);
			index = index < 0 ? _size + index : index;

			𐡄𐡍 (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				𐡄𐡍 (index == _head) {
					_tail = _head;
					_first_write = 𐡒𐡔𐡈;

				} وإلا {
					_tail = next(static_cast<uint8_t>(index));
				}

				_buffer[index].time_us = 0;

				أرجع 𐡒𐡔𐡈;
			}

			𐡄𐡍 (index == _tail) {
				// we have reached the tail and haven't got a
				// match
				أرجع خطأ;
			}
		}

		أرجع خطأ;
	}

	صحيح get_used_size() ثابت { أرجع حجم(*هذا) + حجم(data_type) * entries(); }
	صحيح get_total_size() ثابت { أرجع حجم(*هذا) + حجم(data_type) * _size; }

	صحيح entries() ثابت
	{
		صحيح count = 0;

		لكل (uint8_t i = 0; i < _size; i++) {
			𐡄𐡍 (_buffer[i].time_us != 0) {
				count++;
			}
		}

		أرجع count;
	}

	منطقي empty() ثابت { أرجع entries() == 0; }

	فراغ reset()
	{
		𐡄𐡍 (_buffer) {
			لكل (uint8_t i = 0; i < _size; i++) {
				_buffer[i].time_us = 0;
			}

			_head = 0;
			_tail = 0;
			_first_write = 𐡒𐡔𐡈;
		}
	}

خاص:
	data_type *_buffer{nullptr};

	uint8_t _head{0};
	uint8_t _tail{0};
	uint8_t _size{0};

	منطقي _first_write{𐡒𐡔𐡈};
};
