/****************************************************************************
 *
 *   Copyright (C) 2023 PX4 Development Team. All rights reserved.
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




#அந்தர்பாவ "Ringbuffer.hpp"

#அந்தர்பாவ <mathlib/mathlib.h>
#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <string.h>


Ringbuffer::~Ringbuffer()
{
	deallocate();
}

தருக்க Ringbuffer::allocate(size_t buffer_size)
{
	ப்ரதிஜ்ஞா(_ringbuffer == nullptr);

	_size = buffer_size;
	_ringbuffer = புதிய uint8_t[_size];
	திரும்பு _ringbuffer != nullptr;
}

வெறுமை Ringbuffer::deallocate()
{
	delete[] _ringbuffer;
	_ringbuffer = nullptr;
	_size = 0;
	_start = 0;
	_end = 0;
}

size_t Ringbuffer::space_available() மாறிலி
{
	எனில் (_start > _end) {
		திரும்பு _start - _end - 1;

	} இல்லையெனில் {
		திரும்பு _start - _end - 1 + _size;
	}
}

size_t Ringbuffer::space_used() மாறிலி
{
	எனில் (_start <= _end) {
		திரும்பு _end - _start;

	} இல்லையெனில் {
		// Potential wrap around.
		திரும்பு _end - _start + _size;
	}
}


தருக்க Ringbuffer::push_back(மாறிலி uint8_t *buf, size_t buf_len)
{
	எனில் (buf_len == 0 || buf == nullptr) {
		// Nothing to add, we better don't try.
		திரும்பு பொய்;
	}

	எனில் (_start > _end) {
		// Add after end up to start, no wrap around.

		// Leave one byte free so that start don't end up the same
		// which signals empty.
		மாறிலி size_t available = _start - _end - 1;

		எனில் (available < buf_len) {
			திரும்பு பொய்;
		}

		memcpy(&_ringbuffer[_end], buf, buf_len);
		_end += buf_len;

	} இல்லையெனில் {
		// Add after end, maybe wrap around.
		மாறிலி size_t available = _start - _end - 1 + _size;

		எனில் (available < buf_len) {
			திரும்பு பொய்;
		}

		மாறிலி size_t remaining_packet_len = _size - _end;

		எனில் (buf_len > remaining_packet_len) {
			memcpy(&_ringbuffer[_end], buf, remaining_packet_len);
			_end = 0;

			memcpy(&_ringbuffer[_end], buf + remaining_packet_len, buf_len - remaining_packet_len);
			_end += buf_len - remaining_packet_len;

		} இல்லையெனில் {
			memcpy(&_ringbuffer[_end], buf, buf_len);
			_end += buf_len;
		}
	}

	திரும்பு உண்மை;
}

size_t Ringbuffer::எடு_front(uint8_t *buf, size_t buf_max_len)
{
	எனில் (buf == nullptr) {
		// User needs to supply a valid pointer.
		திரும்பு 0;
	}

	எனில் (_start == _end) {
		// Empty
		திரும்பு 0;
	}

	எனில் (_start < _end) {

		// No wrap around.
		size_t to_copy_len = math::min(_end - _start, buf_max_len);

		memcpy(buf, &_ringbuffer[_start], to_copy_len);
		_start += to_copy_len;

		திரும்பு to_copy_len;

	} இல்லையெனில் {
		// Potential wrap around.
		size_t to_copy_len = _end - _start + _size;

		எனில் (to_copy_len > buf_max_len) {
			to_copy_len = buf_max_len;
		}

		மாறிலி size_t remaining_buf_len = _size - _start;

		எனில் (to_copy_len > remaining_buf_len) {

			memcpy(buf, &_ringbuffer[_start], remaining_buf_len);
			_start = 0;
			memcpy(buf + remaining_buf_len, &_ringbuffer[_start], to_copy_len - remaining_buf_len);
			_start += to_copy_len - remaining_buf_len;

		} இல்லையெனில் {
			memcpy(buf, &_ringbuffer[_start], to_copy_len);
			_start += to_copy_len;
		}

		திரும்பு to_copy_len;
	}
}
