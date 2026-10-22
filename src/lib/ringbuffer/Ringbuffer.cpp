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




#ಅನ್ತರ್ಭಾವ "Ringbuffer.hpp"

#ಅನ್ತರ್ಭಾವ <mathlib/mathlib.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <string.h>


Ringbuffer::~Ringbuffer()
{
	deallocate();
}

ತಾರ್ಕಿಕ Ringbuffer::allocate(size_t buffer_size)
{
	ಪ್ರತಿಜ್ಞಾ(_ringbuffer == nullptr);

	_size = buffer_size;
	_ringbuffer = ನೂತನ uint8_t[_size];
	ಪ್ರತ್ಯಾಗಮ _ringbuffer != nullptr;
}

ಶೂನ್ಯ Ringbuffer::deallocate()
{
	delete[] _ringbuffer;
	_ringbuffer = nullptr;
	_size = 0;
	_start = 0;
	_end = 0;
}

size_t Ringbuffer::space_available() ಧ್ರುವ
{
	ಯದಿ (_start > _end) {
		ಪ್ರತ್ಯಾಗಮ _start - _end - 1;

	} ಅನ್ಯಥಾ {
		ಪ್ರತ್ಯಾಗಮ _start - _end - 1 + _size;
	}
}

size_t Ringbuffer::space_used() ಧ್ರುವ
{
	ಯದಿ (_start <= _end) {
		ಪ್ರತ್ಯಾಗಮ _end - _start;

	} ಅನ್ಯಥಾ {
		// Potential wrap around.
		ಪ್ರತ್ಯಾಗಮ _end - _start + _size;
	}
}


ತಾರ್ಕಿಕ Ringbuffer::push_back(ಧ್ರುವ uint8_t *buf, size_t buf_len)
{
	ಯದಿ (buf_len == 0 || buf == nullptr) {
		// Nothing to add, we better don't try.
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	ಯದಿ (_start > _end) {
		// Add after end up to start, no wrap around.

		// Leave one byte free so that start don't end up the same
		// which signals empty.
		ಧ್ರುವ size_t available = _start - _end - 1;

		ಯದಿ (available < buf_len) {
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		}

		memcpy(&_ringbuffer[_end], buf, buf_len);
		_end += buf_len;

	} ಅನ್ಯಥಾ {
		// Add after end, maybe wrap around.
		ಧ್ರುವ size_t available = _start - _end - 1 + _size;

		ಯದಿ (available < buf_len) {
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		}

		ಧ್ರುವ size_t remaining_packet_len = _size - _end;

		ಯದಿ (buf_len > remaining_packet_len) {
			memcpy(&_ringbuffer[_end], buf, remaining_packet_len);
			_end = 0;

			memcpy(&_ringbuffer[_end], buf + remaining_packet_len, buf_len - remaining_packet_len);
			_end += buf_len - remaining_packet_len;

		} ಅನ್ಯಥಾ {
			memcpy(&_ringbuffer[_end], buf, buf_len);
			_end += buf_len;
		}
	}

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

size_t Ringbuffer::ತೆಗೆ_front(uint8_t *buf, size_t buf_max_len)
{
	ಯದಿ (buf == nullptr) {
		// User needs to supply a valid pointer.
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	ಯದಿ (_start == _end) {
		// Empty
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	ಯದಿ (_start < _end) {

		// No wrap around.
		size_t to_copy_len = math::min(_end - _start, buf_max_len);

		memcpy(buf, &_ringbuffer[_start], to_copy_len);
		_start += to_copy_len;

		ಪ್ರತ್ಯಾಗಮ to_copy_len;

	} ಅನ್ಯಥಾ {
		// Potential wrap around.
		size_t to_copy_len = _end - _start + _size;

		ಯದಿ (to_copy_len > buf_max_len) {
			to_copy_len = buf_max_len;
		}

		ಧ್ರುವ size_t remaining_buf_len = _size - _start;

		ಯದಿ (to_copy_len > remaining_buf_len) {

			memcpy(buf, &_ringbuffer[_start], remaining_buf_len);
			_start = 0;
			memcpy(buf + remaining_buf_len, &_ringbuffer[_start], to_copy_len - remaining_buf_len);
			_start += to_copy_len - remaining_buf_len;

		} ಅನ್ಯಥಾ {
			memcpy(buf, &_ringbuffer[_start], to_copy_len);
			_start += to_copy_len;
		}

		ಪ್ರತ್ಯಾಗಮ to_copy_len;
	}
}
