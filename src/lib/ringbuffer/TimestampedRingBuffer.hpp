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

#অন্তর্ভাৱ <climits>
#অন্তর্ভাৱ <cstddef>
#অন্তর্ভাৱ <inttypes.h>

template <typename data_type, size_t SIZE = 0>
শ্ৰেণী TimestampedRingBuffer;

/**
 * Static-size ring buffer specialization (SIZE > 0).
 */
template <typename data_type, size_t SIZE>
শ্ৰেণী TimestampedRingBuffer
{
সার্ৱজনিক:
	static_assert(SIZE > 0, "SIZE must be > 0");
	static_assert(SIZE <= UINT8_MAX, "SIZE must fit in uint8_t");
	স্থির constexpr uint8_t kSize = static_cast<uint8_t>(SIZE);

	TimestampedRingBuffer() = স্ৱভাৱ;
	~TimestampedRingBuffer() = স্ৱভাৱ;

	// no copy, assignment, move, move assignment
	TimestampedRingBuffer(ধ্রুৱ TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &operator=(ধ্রুৱ TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &operator=(TimestampedRingBuffer &&) = delete;

	তার্কিক allocate(size_t size) { ঘূৰাই size == SIZE; }

	তার্কিক valid() ধ্রুৱ { ঘূৰাই SIZE > 0; }

	শূন্য ঠেলো(ধ্রুৱ data_type &sample)
	{
		_buffer[_head] = sample;
		_head = next(_head);

		যদি (_entries < kSize) {
			_entries++;
		}
	}

	uint8_t get_length() ধ্রুৱ { ঘূৰাই kSize; }

	data_type &operator[](ধ্রুৱ uint8_t index) { ঘূৰাই _buffer[index]; }

	ধ্রুৱ data_type &get_newest() ধ্রুৱ { ঘূৰাই _buffer[get_newest_index()]; }
	ধ্রুৱ data_type &get_oldest() ধ্রুৱ { ঘূৰাই _buffer[get_oldest_index()]; }

	uint8_t get_newest_index() ধ্রুৱ { ঘূৰাই (_entries > 0) ? prev(_head) : 0; }
	uint8_t get_oldest_index() ধ্রুৱ
	{
		যদি (_entries == 0) {
			ঘূৰাই 0;
		}

		ধ্রুৱ uint16_t idx = static_cast<uint16_t>(_head) + static_cast<uint16_t>(kSize) - static_cast<uint16_t>(_entries);
		ঘূৰাই (idx >= kSize) ? static_cast<uint8_t>(idx - kSize) : static_cast<uint8_t>(idx);
	}

	uint8_t next(uint8_t i) ধ্রুৱ { ঘূৰাই (i + 1 >= kSize) ? 0 : i + 1; }
	uint8_t prev(uint8_t i) ধ্রুৱ { ঘূৰাই (i == 0) ? kSize - 1 : i - 1; }

	তার্কিক বেৰ_কৰো_first_older_than(ধ্রুৱ uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		ধ্রুৱ uint8_t newest_idx = get_newest_index();
		ধ্রুৱ uint8_t oldest_idx = get_oldest_index();

		বাবে (uint8_t i = 0; i < _entries; i++) {
			পূর্ণাঙ্ক index = (newest_idx - i);
			index = index < 0 ? kSize + index : index;

			যদি (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				ধ্রুৱ uint8_t index_u = static_cast<uint8_t>(index);
				ধ্রুৱ uint8_t distance = (index_u >= oldest_idx) ? (index_u - oldest_idx) : (index_u + kSize - oldest_idx);
				ধ্রুৱ uint8_t discard_গুনতি = distance + 1;
				_entries = (_entries > discard_গুনতি) ? (_entries - discard_গুনতি) : 0;
				_buffer[index].time_us = 0;

				ঘূৰাই সঁচা;
			}
		}

		ঘূৰাই মিছা;
	}

	পূর্ণাঙ্ক get_total_size() ধ্রুৱ
	{
		static_assert(পরিমাণ(TimestampedRingBuffer<data_type, SIZE>) <= static_cast<size_t>(INT_MAX),
			      "static ring buffer size must fit in int");
		ঘূৰাই static_cast<পূর্ণাঙ্ক>(পরিমাণ(TimestampedRingBuffer<data_type, SIZE>));
	}

	পূর্ণাঙ্ক entries() ধ্রুৱ { ঘূৰাই _entries; }
	তার্কিক empty() ধ্রুৱ { ঘূৰাই _entries == 0; }

	শূন্য reset()
	{
		বাবে (uint8_t i = 0; i < kSize; i++) {
			_buffer[i].time_us = 0;
		}

		_head = 0;
		_entries = 0;
	}

গুপ্ত:
	data_type _buffer[SIZE] {};

	uint8_t _head{0};
	uint8_t _entries{0};
};

/**
 * Dynamic-size ring buffer specialization (SIZE == 0).
 */
template <typename data_type>
শ্ৰেণী TimestampedRingBuffer<data_type, 0>
{
সার্ৱজনিক:
	explicit TimestampedRingBuffer(size_t size) { allocate(size); }
	TimestampedRingBuffer() = delete;
	~TimestampedRingBuffer() { delete[] _buffer; }

	// no copy, assignment, move, move assignment
	TimestampedRingBuffer(ধ্রুৱ TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &operator=(ধ্রুৱ TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &operator=(TimestampedRingBuffer &&) = delete;

	তার্কিক allocate(size_t size)
	{
		যদি (valid() && (size == _size)) {
			// no change
			ঘূৰাই সঁচা;
		}

		যদি (size == 0 || size > UINT8_MAX) {
			ঘূৰাই মিছা;
		}

		delete[] _buffer;

		_buffer = নূতন data_type[size] {};

		যদি (_buffer == nullptr) {
			ঘূৰাই মিছা;
		}

		_size = static_cast<uint8_t>(size);

		// new[]{} already zeroed time_us, only reset the indices
		_head = 0;
		_tail = 0;
		_first_write = সঁচা;

		ঘূৰাই সঁচা;
	}

	তার্কিক valid() ধ্রুৱ { ঘূৰাই (_buffer != nullptr) && (_size > 0); }

	শূন্য ঠেলো(ধ্রুৱ data_type &sample)
	{
		uint8_t head_new = _head;

		যদি (!_first_write) {
			head_new = next(_head);
		}

		_buffer[head_new] = sample;
		_head = head_new;

		// move tail if we overwrite it
		যদি (_head == _tail && !_first_write) {
			_tail = next(_tail);

		} অন্যথা {
			_first_write = মিছা;
		}
	}

	uint8_t get_length() ধ্রুৱ { ঘূৰাই _size; }

	data_type &operator[](ধ্রুৱ uint8_t index) { ঘূৰাই _buffer[index]; }

	ধ্রুৱ data_type &get_newest() ধ্রুৱ { ঘূৰাই _buffer[_head]; }
	ধ্রুৱ data_type &get_oldest() ধ্রুৱ { ঘূৰাই _buffer[_tail]; }

	uint8_t get_newest_index() ধ্রুৱ { ঘূৰাই _head; }
	uint8_t get_oldest_index() ধ্রুৱ { ঘূৰাই _tail; }

	uint8_t next(uint8_t i) ধ্রুৱ { ঘূৰাই (_size > 0) ? ((i + 1 >= _size) ? 0 : i + 1) : 0; }
	uint8_t prev(uint8_t i) ধ্রুৱ { ঘূৰাই (_size > 0) ? ((i == 0) ? (_size - 1) : (i - 1)) : 0; }

	তার্কিক বেৰ_কৰো_first_older_than(ধ্রুৱ uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		বাবে (uint8_t i = 0; i < _size; i++) {
			পূর্ণাঙ্ক index = (_head - i);
			index = index < 0 ? _size + index : index;

			যদি (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				যদি (index == _head) {
					_tail = _head;
					_first_write = সঁচা;

				} অন্যথা {
					_tail = next(static_cast<uint8_t>(index));
				}

				_buffer[index].time_us = 0;

				ঘূৰাই সঁচা;
			}

			যদি (index == _tail) {
				// we have reached the tail and haven't got a
				// match
				ঘূৰাই মিছা;
			}
		}

		ঘূৰাই মিছা;
	}

	পূর্ণাঙ্ক get_used_size() ধ্রুৱ { ঘূৰাই পরিমাণ(*এতত্) + পরিমাণ(data_type) * entries(); }
	পূর্ণাঙ্ক get_total_size() ধ্রুৱ { ঘূৰাই পরিমাণ(*এতত্) + পরিমাণ(data_type) * _size; }

	পূর্ণাঙ্ক entries() ধ্রুৱ
	{
		পূর্ণাঙ্ক গুনতি = 0;

		বাবে (uint8_t i = 0; i < _size; i++) {
			যদি (_buffer[i].time_us != 0) {
				গুনতি++;
			}
		}

		ঘূৰাই গুনতি;
	}

	তার্কিক empty() ধ্রুৱ { ঘূৰাই entries() == 0; }

	শূন্য reset()
	{
		যদি (_buffer) {
			বাবে (uint8_t i = 0; i < _size; i++) {
				_buffer[i].time_us = 0;
			}

			_head = 0;
			_tail = 0;
			_first_write = সঁচা;
		}
	}

গুপ্ত:
	data_type *_buffer{nullptr};

	uint8_t _head{0};
	uint8_t _tail{0};
	uint8_t _size{0};

	তার্কিক _first_write{সঁচা};
};
