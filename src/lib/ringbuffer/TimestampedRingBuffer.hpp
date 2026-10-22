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

#അന്തര്ഭാവ <climits>
#അന്തര്ഭാവ <cstddef>
#അന്തര്ഭാവ <inttypes.h>

template <typename data_type, size_t SIZE = 0>
വർഗ്ഗം TimestampedRingBuffer;

/**
 * Static-size ring buffer specialization (SIZE > 0).
 */
template <typename data_type, size_t SIZE>
വർഗ്ഗം TimestampedRingBuffer
{
പൊതു:
	static_assert(SIZE > 0, "SIZE must be > 0");
	static_assert(SIZE <= UINT8_MAX, "SIZE must fit in uint8_t");
	സ്ഥിതം constexpr uint8_t kSize = static_cast<uint8_t>(SIZE);

	TimestampedRingBuffer() = സ്ഥിരസ്ഥിതി;
	~TimestampedRingBuffer() = സ്ഥിരസ്ഥിതി;

	// no copy, assignment, move, move assignment
	TimestampedRingBuffer(സ്ഥിരം TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &operator=(സ്ഥിരം TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &operator=(TimestampedRingBuffer &&) = delete;

	തർക്കം allocate(size_t size) { തിരികെ size == SIZE; }

	തർക്കം valid() സ്ഥിരം { തിരികെ SIZE > 0; }

	ശൂന്യം തള്ളുക(സ്ഥിരം data_type &sample)
	{
		_buffer[_head] = sample;
		_head = next(_head);

		എങ്കിൽ (_entries < kSize) {
			_entries++;
		}
	}

	uint8_t get_length() സ്ഥിരം { തിരികെ kSize; }

	data_type &operator[](സ്ഥിരം uint8_t index) { തിരികെ _buffer[index]; }

	സ്ഥിരം data_type &get_newest() സ്ഥിരം { തിരികെ _buffer[get_newest_index()]; }
	സ്ഥിരം data_type &get_oldest() സ്ഥിരം { തിരികെ _buffer[get_oldest_index()]; }

	uint8_t get_newest_index() സ്ഥിരം { തിരികെ (_entries > 0) ? prev(_head) : 0; }
	uint8_t get_oldest_index() സ്ഥിരം
	{
		എങ്കിൽ (_entries == 0) {
			തിരികെ 0;
		}

		സ്ഥിരം uint16_t idx = static_cast<uint16_t>(_head) + static_cast<uint16_t>(kSize) - static_cast<uint16_t>(_entries);
		തിരികെ (idx >= kSize) ? static_cast<uint8_t>(idx - kSize) : static_cast<uint8_t>(idx);
	}

	uint8_t next(uint8_t i) സ്ഥിരം { തിരികെ (i + 1 >= kSize) ? 0 : i + 1; }
	uint8_t prev(uint8_t i) സ്ഥിരം { തിരികെ (i == 0) ? kSize - 1 : i - 1; }

	തർക്കം എടുക്കുക_first_older_than(സ്ഥിരം uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		സ്ഥിരം uint8_t newest_idx = get_newest_index();
		സ്ഥിരം uint8_t oldest_idx = get_oldest_index();

		വേണ്ടി (uint8_t i = 0; i < _entries; i++) {
			പൂർണ്ണസംഖ്യ index = (newest_idx - i);
			index = index < 0 ? kSize + index : index;

			എങ്കിൽ (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				സ്ഥിരം uint8_t index_u = static_cast<uint8_t>(index);
				സ്ഥിരം uint8_t distance = (index_u >= oldest_idx) ? (index_u - oldest_idx) : (index_u + kSize - oldest_idx);
				സ്ഥിരം uint8_t discard_എണ്ണം = distance + 1;
				_entries = (_entries > discard_എണ്ണം) ? (_entries - discard_എണ്ണം) : 0;
				_buffer[index].time_us = 0;

				തിരികെ സത്യം;
			}
		}

		തിരികെ അസത്യം;
	}

	പൂർണ്ണസംഖ്യ get_total_size() സ്ഥിരം
	{
		static_assert(വലുപ്പം(TimestampedRingBuffer<data_type, SIZE>) <= static_cast<size_t>(INT_MAX),
			      "static ring buffer size must fit in int");
		തിരികെ static_cast<പൂർണ്ണസംഖ്യ>(വലുപ്പം(TimestampedRingBuffer<data_type, SIZE>));
	}

	പൂർണ്ണസംഖ്യ entries() സ്ഥിരം { തിരികെ _entries; }
	തർക്കം empty() സ്ഥിരം { തിരികെ _entries == 0; }

	ശൂന്യം reset()
	{
		വേണ്ടി (uint8_t i = 0; i < kSize; i++) {
			_buffer[i].time_us = 0;
		}

		_head = 0;
		_entries = 0;
	}

സ്വകാര്യ:
	data_type _buffer[SIZE] {};

	uint8_t _head{0};
	uint8_t _entries{0};
};

/**
 * Dynamic-size ring buffer specialization (SIZE == 0).
 */
template <typename data_type>
വർഗ്ഗം TimestampedRingBuffer<data_type, 0>
{
പൊതു:
	explicit TimestampedRingBuffer(size_t size) { allocate(size); }
	TimestampedRingBuffer() = delete;
	~TimestampedRingBuffer() { delete[] _buffer; }

	// no copy, assignment, move, move assignment
	TimestampedRingBuffer(സ്ഥിരം TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer &operator=(സ്ഥിരം TimestampedRingBuffer &) = delete;
	TimestampedRingBuffer(TimestampedRingBuffer &&) = delete;
	TimestampedRingBuffer &operator=(TimestampedRingBuffer &&) = delete;

	തർക്കം allocate(size_t size)
	{
		എങ്കിൽ (valid() && (size == _size)) {
			// no change
			തിരികെ സത്യം;
		}

		എങ്കിൽ (size == 0 || size > UINT8_MAX) {
			തിരികെ അസത്യം;
		}

		delete[] _buffer;

		_buffer = പുതിയ data_type[size] {};

		എങ്കിൽ (_buffer == nullptr) {
			തിരികെ അസത്യം;
		}

		_size = static_cast<uint8_t>(size);

		// new[]{} already zeroed time_us, only reset the indices
		_head = 0;
		_tail = 0;
		_first_write = സത്യം;

		തിരികെ സത്യം;
	}

	തർക്കം valid() സ്ഥിരം { തിരികെ (_buffer != nullptr) && (_size > 0); }

	ശൂന്യം തള്ളുക(സ്ഥിരം data_type &sample)
	{
		uint8_t head_new = _head;

		എങ്കിൽ (!_first_write) {
			head_new = next(_head);
		}

		_buffer[head_new] = sample;
		_head = head_new;

		// move tail if we overwrite it
		എങ്കിൽ (_head == _tail && !_first_write) {
			_tail = next(_tail);

		} അല്ലെങ്കിൽ {
			_first_write = അസത്യം;
		}
	}

	uint8_t get_length() സ്ഥിരം { തിരികെ _size; }

	data_type &operator[](സ്ഥിരം uint8_t index) { തിരികെ _buffer[index]; }

	സ്ഥിരം data_type &get_newest() സ്ഥിരം { തിരികെ _buffer[_head]; }
	സ്ഥിരം data_type &get_oldest() സ്ഥിരം { തിരികെ _buffer[_tail]; }

	uint8_t get_newest_index() സ്ഥിരം { തിരികെ _head; }
	uint8_t get_oldest_index() സ്ഥിരം { തിരികെ _tail; }

	uint8_t next(uint8_t i) സ്ഥിരം { തിരികെ (_size > 0) ? ((i + 1 >= _size) ? 0 : i + 1) : 0; }
	uint8_t prev(uint8_t i) സ്ഥിരം { തിരികെ (_size > 0) ? ((i == 0) ? (_size - 1) : (i - 1)) : 0; }

	തർക്കം എടുക്കുക_first_older_than(സ്ഥിരം uint64_t &timestamp, data_type *sample)
	{
		// start looking from newest observation data
		വേണ്ടി (uint8_t i = 0; i < _size; i++) {
			പൂർണ്ണസംഖ്യ index = (_head - i);
			index = index < 0 ? _size + index : index;

			എങ്കിൽ (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
				*sample = _buffer[index];

				// Now we can set the tail to the item which
				// comes after the one we removed since we don't
				// want to have any older data in the buffer
				എങ്കിൽ (index == _head) {
					_tail = _head;
					_first_write = സത്യം;

				} അല്ലെങ്കിൽ {
					_tail = next(static_cast<uint8_t>(index));
				}

				_buffer[index].time_us = 0;

				തിരികെ സത്യം;
			}

			എങ്കിൽ (index == _tail) {
				// we have reached the tail and haven't got a
				// match
				തിരികെ അസത്യം;
			}
		}

		തിരികെ അസത്യം;
	}

	പൂർണ്ണസംഖ്യ get_used_size() സ്ഥിരം { തിരികെ വലുപ്പം(*ഇത്) + വലുപ്പം(data_type) * entries(); }
	പൂർണ്ണസംഖ്യ get_total_size() സ്ഥിരം { തിരികെ വലുപ്പം(*ഇത്) + വലുപ്പം(data_type) * _size; }

	പൂർണ്ണസംഖ്യ entries() സ്ഥിരം
	{
		പൂർണ്ണസംഖ്യ എണ്ണം = 0;

		വേണ്ടി (uint8_t i = 0; i < _size; i++) {
			എങ്കിൽ (_buffer[i].time_us != 0) {
				എണ്ണം++;
			}
		}

		തിരികെ എണ്ണം;
	}

	തർക്കം empty() സ്ഥിരം { തിരികെ entries() == 0; }

	ശൂന്യം reset()
	{
		എങ്കിൽ (_buffer) {
			വേണ്ടി (uint8_t i = 0; i < _size; i++) {
				_buffer[i].time_us = 0;
			}

			_head = 0;
			_tail = 0;
			_first_write = സത്യം;
		}
	}

സ്വകാര്യ:
	data_type *_buffer{nullptr};

	uint8_t _head{0};
	uint8_t _tail{0};
	uint8_t _size{0};

	തർക്കം _first_write{സത്യം};
};
