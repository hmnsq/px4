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

#ଅନ୍ତର୍ଭାଵ <climits>
#ଅନ୍ତର୍ଭାଵ <cstddef>
#ଅନ୍ତର୍ଭାଵ <inttypes.h>

template <typename data_type, size_t SIZE = 0>
ଶ୍ରେଣୀ TimestampedRingBuffer;

/**
 * Static-size ring buffer specialization (SIZE > 0).
 */
template <typename data_type, size_t SIZE>
ଶ୍ରେଣୀ TimestampedRingBuffer
{
ସର୍ବସାଧାରଣ:
	static_assert(SIZE > 0, "SIZE must be > 0");
	static_assert(SIZE <= UINT8_MAX, "SIZE must fit in uint8_t");
	ସ୍ଥିର constexpr uint8_t kSize = static_cast<uint8_t>(SIZE);

	TimestampedRingBuffer() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	~TimestampedRingBuffer() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	// no copy, assignment, move, move assignment
	TimestampedRingBuffer(ଅଚଳ TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &operator=(ଅଚଳ TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &operator=(TimestampedRingBuffer &&) = delete;

	ତାର୍କିକ allocate(size_t size) { ଫେରନ୍ତୁ size == SIZE; }

	ତାର୍କିକ valid() ଅଚଳ { ଫେରନ୍ତୁ SIZE > 0; }

	ଶୂନ୍ୟ ଠେଲ(ଅଚଳ data_type &sample)
	{
		_buffer[_head] = sample;
		_head = next(_head);

		ଯଦି (_entries < kSize) {
			_entries++;
		}
	}

	uint8_t get_length() ଅଚଳ { ଫେରନ୍ତୁ kSize; }

	data_type &operator[](ଅଚଳ uint8_t index) { ଫେରନ୍ତୁ _buffer[index]; }

	ଅଚଳ data_type &get_newest() ଅଚଳ { ଫେରନ୍ତୁ _buffer[get_newest_index()]; }
	ଅଚଳ data_type &get_oldest() ଅଚଳ { ଫେରନ୍ତୁ _buffer[get_oldest_index()]; }

	uint8_t get_newest_index() ଅଚଳ { ଫେରନ୍ତୁ (_entries > 0) ? prev(_head) : 0; }
	uint8_t get_oldest_index() ଅଚଳ
	{
		ଯଦି (_entries == 0) {
			ଫେରନ୍ତୁ 0;
		}

		ଅଚଳ uint16_t idx = static_cast<uint16_t>(_head) + static_cast<uint16_t>(kSize) - static_cast<uint16_t>(_entries);
		ଫେରନ୍ତୁ (idx >= kSize) ? static_cast<uint8_t>(idx - kSize) : static_cast<uint8_t>(idx);
	}

	uint8_t next(uint8_t i) ଅଚଳ { ଫେରନ୍ତୁ (i + 1 >= kSize) ? 0 : i + 1; }
	uint8_t prev(uint8_t i) ଅଚଳ { ଫେରନ୍ତୁ (i == 0) ? kSize - 1 : i - 1; }

	ତାର୍କିକ କାଢ଼_first_older_than(ଅଚଳ uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		ଅଚଳ uint8_t newest_idx = get_newest_index();
		ଅଚଳ uint8_t oldest_idx = get_oldest_index();

		ପାଇଁ (uint8_t i = 0; i < _entries; i++) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = (newest_idx - i);
			index = index < 0 ? kSize + index : index;

			ଯଦି (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				ଅଚଳ uint8_t index_u = static_cast<uint8_t>(index);
				ଅଚଳ uint8_t distance = (index_u >= oldest_idx) ? (index_u - oldest_idx) : (index_u + kSize - oldest_idx);
				ଅଚଳ uint8_t discard_ଗଣନା = distance + 1;
				_entries = (_entries > discard_ଗଣନା) ? (_entries - discard_ଗଣନା) : 0;
				_buffer[index].time_us = 0;

				ଫେରନ୍ତୁ ସତ୍ୟ;
			}
		}

		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_total_size() ଅଚଳ
	{
		static_assert(ଆକାର(TimestampedRingBuffer<data_type, SIZE>) <= static_cast<size_t>(INT_MAX),
			      "static ring buffer size must fit in int");
		ଫେରନ୍ତୁ static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(ଆକାର(TimestampedRingBuffer<data_type, SIZE>));
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା entries() ଅଚଳ { ଫେରନ୍ତୁ _entries; }
	ତାର୍କିକ empty() ଅଚଳ { ଫେରନ୍ତୁ _entries == 0; }

	ଶୂନ୍ୟ reset()
	{
		ପାଇଁ (uint8_t i = 0; i < kSize; i++) {
			_buffer[i].time_us = 0;
		}

		_head = 0;
		_entries = 0;
	}

ବ୍ୟକ୍ତିଗତ:
	data_type _buffer[SIZE] {};

	uint8_t _head{0};
	uint8_t _entries{0};
};

/**
 * Dynamic-size ring buffer specialization (SIZE == 0).
 */
template <typename data_type>
ଶ୍ରେଣୀ TimestampedRingBuffer<data_type, 0>
{
ସର୍ବସାଧାରଣ:
	explicit TimestampedRingBuffer(size_t size) { allocate(size); }
	TimestampedRingBuffer() = delete;
	~TimestampedRingBuffer() { delete[] _buffer; }

	// no copy, assignment, move, move assignment
	TimestampedRingBuffer(ଅଚଳ TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &operator=(ଅଚଳ TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &operator=(TimestampedRingBuffer &&) = delete;

	ତାର୍କିକ allocate(size_t size)
	{
		ଯଦି (valid() && (size == _size)) {
			// no change
			ଫେରନ୍ତୁ ସତ୍ୟ;
		}

		ଯଦି (size == 0 || size > UINT8_MAX) {
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		}

		delete[] _buffer;

		_buffer = ନୂତନ data_type[size] {};

		ଯଦି (_buffer == nullptr) {
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		}

		_size = static_cast<uint8_t>(size);

		// new[]{} already zeroed time_us, only reset the indices
		_head = 0;
		_tail = 0;
		_first_write = ସତ୍ୟ;

		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ତାର୍କିକ valid() ଅଚଳ { ଫେରନ୍ତୁ (_buffer != nullptr) && (_size > 0); }

	ଶୂନ୍ୟ ଠେଲ(ଅଚଳ data_type &sample)
	{
		uint8_t head_new = _head;

		ଯଦି (!_first_write) {
			head_new = next(_head);
		}

		_buffer[head_new] = sample;
		_head = head_new;

		// move tail if we overwrite it
		ଯଦି (_head == _tail && !_first_write) {
			_tail = next(_tail);

		} ଅନ୍ୟଥା {
			_first_write = ମିଥ୍ୟା;
		}
	}

	uint8_t get_length() ଅଚଳ { ଫେରନ୍ତୁ _size; }

	data_type &operator[](ଅଚଳ uint8_t index) { ଫେରନ୍ତୁ _buffer[index]; }

	ଅଚଳ data_type &get_newest() ଅଚଳ { ଫେରନ୍ତୁ _buffer[_head]; }
	ଅଚଳ data_type &get_oldest() ଅଚଳ { ଫେରନ୍ତୁ _buffer[_tail]; }

	uint8_t get_newest_index() ଅଚଳ { ଫେରନ୍ତୁ _head; }
	uint8_t get_oldest_index() ଅଚଳ { ଫେରନ୍ତୁ _tail; }

	uint8_t next(uint8_t i) ଅଚଳ { ଫେରନ୍ତୁ (_size > 0) ? ((i + 1 >= _size) ? 0 : i + 1) : 0; }
	uint8_t prev(uint8_t i) ଅଚଳ { ଫେରନ୍ତୁ (_size > 0) ? ((i == 0) ? (_size - 1) : (i - 1)) : 0; }

	ତାର୍କିକ କାଢ଼_first_older_than(ଅଚଳ uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		ପାଇଁ (uint8_t i = 0; i < _size; i++) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = (_head - i);
			index = index < 0 ? _size + index : index;

			ଯଦି (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				ଯଦି (index == _head) {
					_tail = _head;
					_first_write = ସତ୍ୟ;

				} ଅନ୍ୟଥା {
					_tail = next(static_cast<uint8_t>(index));
				}

				_buffer[index].time_us = 0;

				ଫେରନ୍ତୁ ସତ୍ୟ;
			}

			ଯଦି (index == _tail) {
				// we have reached the tail and haven't got a
				// match
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
			}
		}

		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_used_size() ଅଚଳ { ଫେରନ୍ତୁ ଆକାର(*ଏହି) + ଆକାର(data_type) * entries(); }
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_total_size() ଅଚଳ { ଫେରନ୍ତୁ ଆକାର(*ଏହି) + ଆକାର(data_type) * _size; }

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା entries() ଅଚଳ
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = 0;

		ପାଇଁ (uint8_t i = 0; i < _size; i++) {
			ଯଦି (_buffer[i].time_us != 0) {
				ଗଣନା_2++;
			}
		}

		ଫେରନ୍ତୁ ଗଣନା_2;
	}

	ତାର୍କିକ empty() ଅଚଳ { ଫେରନ୍ତୁ entries() == 0; }

	ଶୂନ୍ୟ reset()
	{
		ଯଦି (_buffer) {
			ପାଇଁ (uint8_t i = 0; i < _size; i++) {
				_buffer[i].time_us = 0;
			}

			_head = 0;
			_tail = 0;
			_first_write = ସତ୍ୟ;
		}
	}

ବ୍ୟକ୍ତିଗତ:
	data_type *_buffer{nullptr};

	uint8_t _head{0};
	uint8_t _tail{0};
	uint8_t _size{0};

	ତାର୍କିକ _first_write{ସତ୍ୟ};
};
