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

#include <climits>
#include <cstddef>
#include <inttypes.h>

template <typename data_type, size_t SIZE = 0>
class TimestampedRingBuffer;

/**
 * Static-size ring buffer specialization (SIZE > 0).
 */
template <typename data_type, size_t SIZE>
class TimestampedRingBuffer
{
public:
	static_assert(SIZE > 0, "SIZE must be > 0");
	static_assert(SIZE <= UINT8_MAX, "SIZE must fit in uint8_t");
	static constexpr uint8_t kSize = static_cast<uint8_t>(SIZE);

	TimestampedRingBuffer() = default;
	~TimestampedRingBuffer() = default;

	// no copy, assignment, move, move assignment
	TimestampedRingBuffer(const TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &operator=(const TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &operator=(TimestampedRingBuffer &&) = delete;

	bool allocate(size_t size) { ආපසු size == SIZE; }

	bool valid() const { ආපසු SIZE > 0; }

	void push(const data_type &sample)
	{
		_buffer[_head] = sample;
		_head = next(_head);

		නම් (_entries < kSize) {
			_entries++;
		}
	}

	uint8_t get_length() const { ආපසු kSize; }

	data_type &operator[](const uint8_t index) { ආපසු _buffer[index]; }

	const data_type &get_newest() const { ආපසු _buffer[get_newest_index()]; }
	const data_type &get_oldest() const { ආපසු _buffer[get_oldest_index()]; }

	uint8_t get_newest_index() const { ආපසු (_entries > 0) ? prev(_head) : 0; }
	uint8_t get_oldest_index() const
	{
		නම් (_entries == 0) {
			ආපසු 0;
		}

		const uint16_t idx = static_cast<uint16_t>(_head) + static_cast<uint16_t>(kSize) - static_cast<uint16_t>(_entries);
		ආපසු (idx >= kSize) ? static_cast<uint8_t>(idx - kSize) : static_cast<uint8_t>(idx);
	}

	uint8_t next(uint8_t i) const { ආපසු (i + 1 >= kSize) ? 0 : i + 1; }
	uint8_t prev(uint8_t i) const { ආපසු (i == 0) ? kSize - 1 : i - 1; }

	bool pop_first_older_than(const uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		const uint8_t newest_idx = get_newest_index();
		const uint8_t oldest_idx = get_oldest_index();

		සඳහා (uint8_t i = 0; i < _entries; i++) {
			පූර්ණ_සංඛ්‍යා index = (newest_idx - i);
			index = index < 0 ? kSize + index : index;

			නම් (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				const uint8_t index_u = static_cast<uint8_t>(index);
				const uint8_t distance = (index_u >= oldest_idx) ? (index_u - oldest_idx) : (index_u + kSize - oldest_idx);
				const uint8_t discard_count = distance + 1;
				_entries = (_entries > discard_count) ? (_entries - discard_count) : 0;
				_buffer[index].time_us = 0;

				ආපසු true;
			}
		}

		ආපසු false;
	}

	පූර්ණ_සංඛ්‍යා get_total_size() const
	{
		static_assert(parimaa_na(TimestampedRingBuffer<data_type, SIZE>) <= static_cast<size_t>(INT_MAX),
			      "static ring buffer size must fit in int");
		ආපසු static_cast<පූර්ණ_සංඛ්‍යා>(parimaa_na(TimestampedRingBuffer<data_type, SIZE>));
	}

	පූර්ණ_සංඛ්‍යා entries() const { ආපසු _entries; }
	bool empty() const { ආපසු _entries == 0; }

	void reset()
	{
		සඳහා (uint8_t i = 0; i < kSize; i++) {
			_buffer[i].time_us = 0;
		}

		_head = 0;
		_entries = 0;
	}

private:
	data_type _buffer[SIZE] {};

	uint8_t _head{0};
	uint8_t _entries{0};
};

/**
 * Dynamic-size ring buffer specialization (SIZE == 0).
 */
template <typename data_type>
class TimestampedRingBuffer<data_type, 0>
{
public:
	explicit TimestampedRingBuffer(size_t size) { allocate(size); }
	TimestampedRingBuffer() = delete;
	~TimestampedRingBuffer() { delete[] _buffer; }

	// no copy, assignment, move, move assignment
	TimestampedRingBuffer(const TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &operator=(const TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &operator=(TimestampedRingBuffer &&) = delete;

	bool allocate(size_t size)
	{
		නම් (valid() && (size == _size)) {
			// no change
			ආපසු true;
		}

		නම් (size == 0 || size > UINT8_MAX) {
			ආපසු false;
		}

		delete[] _buffer;

		_buffer = new data_type[size] {};

		නම් (_buffer == nullptr) {
			ආපසු false;
		}

		_size = static_cast<uint8_t>(size);

		// new[]{} already zeroed time_us, only reset the indices
		_head = 0;
		_tail = 0;
		_first_write = true;

		ආපසු true;
	}

	bool valid() const { ආපසු (_buffer != nullptr) && (_size > 0); }

	void push(const data_type &sample)
	{
		uint8_t head_new = _head;

		නම් (!_first_write) {
			head_new = next(_head);
		}

		_buffer[head_new] = sample;
		_head = head_new;

		// move tail if we overwrite it
		නම් (_head == _tail && !_first_write) {
			_tail = next(_tail);

		} නැතහොත් {
			_first_write = false;
		}
	}

	uint8_t get_length() const { ආපසු _size; }

	data_type &operator[](const uint8_t index) { ආපසු _buffer[index]; }

	const data_type &get_newest() const { ආපසු _buffer[_head]; }
	const data_type &get_oldest() const { ආපසු _buffer[_tail]; }

	uint8_t get_newest_index() const { ආපසු _head; }
	uint8_t get_oldest_index() const { ආපසු _tail; }

	uint8_t next(uint8_t i) const { ආපසු (_size > 0) ? ((i + 1 >= _size) ? 0 : i + 1) : 0; }
	uint8_t prev(uint8_t i) const { ආපසු (_size > 0) ? ((i == 0) ? (_size - 1) : (i - 1)) : 0; }

	bool pop_first_older_than(const uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		සඳහා (uint8_t i = 0; i < _size; i++) {
			පූර්ණ_සංඛ්‍යා index = (_head - i);
			index = index < 0 ? _size + index : index;

			නම් (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				නම් (index == _head) {
					_tail = _head;
					_first_write = true;

				} නැතහොත් {
					_tail = next(static_cast<uint8_t>(index));
				}

				_buffer[index].time_us = 0;

				ආපසු true;
			}

			නම් (index == _tail) {
				// we have reached the tail and haven't got a
				// match
				ආපසු false;
			}
		}

		ආපසු false;
	}

	පූර්ණ_සංඛ්‍යා get_used_size() const { ආපසු parimaa_na(*this) + parimaa_na(data_type) * entries(); }
	පූර්ණ_සංඛ්‍යා get_total_size() const { ආපසු parimaa_na(*this) + parimaa_na(data_type) * _size; }

	පූර්ණ_සංඛ්‍යා entries() const
	{
		පූර්ණ_සංඛ්‍යා count = 0;

		සඳහා (uint8_t i = 0; i < _size; i++) {
			නම් (_buffer[i].time_us != 0) {
				count++;
			}
		}

		ආපසු count;
	}

	bool empty() const { ආපසු entries() == 0; }

	void reset()
	{
		නම් (_buffer) {
			සඳහා (uint8_t i = 0; i < _size; i++) {
				_buffer[i].time_us = 0;
			}

			_head = 0;
			_tail = 0;
			_first_write = true;
		}
	}

private:
	data_type *_buffer{nullptr};

	uint8_t _head{0};
	uint8_t _tail{0};
	uint8_t _size{0};

	bool _first_write{true};
};
