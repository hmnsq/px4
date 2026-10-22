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




#અન્તર્ભાવ "Ringbuffer.hpp"

#અન્તર્ભાવ <mathlib/mathlib.h>
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <string.h>


Ringbuffer::~Ringbuffer()
{
	deallocate();
}

તાર્કિક Ringbuffer::allocate(size_t buffer_size)
{
	પ્રતિજ્ઞા(_ringbuffer == nullptr);

	_size = buffer_size;
	_ringbuffer = નવું uint8_t[_size];
	પાછા _ringbuffer != nullptr;
}

શૂન્ય Ringbuffer::deallocate()
{
	delete[] _ringbuffer;
	_ringbuffer = nullptr;
	_size = 0;
	_start = 0;
	_end = 0;
}

size_t Ringbuffer::space_available() અચલ
{
	જો (_start > _end) {
		પાછા _start - _end - 1;

	} નહીંતર {
		પાછા _start - _end - 1 + _size;
	}
}

size_t Ringbuffer::space_used() અચલ
{
	જો (_start <= _end) {
		પાછા _end - _start;

	} નહીંતર {
		// Potential wrap around.
		પાછા _end - _start + _size;
	}
}


તાર્કિક Ringbuffer::push_back(અચલ uint8_t *buf, size_t buf_len)
{
	જો (buf_len == 0 || buf == nullptr) {
		// Nothing to add, we better don't try.
		પાછા ખોટું;
	}

	જો (_start > _end) {
		// Add after end up to start, no wrap around.

		// Leave one byte free so that start don't end up the same
		// which signals empty.
		અચલ size_t available = _start - _end - 1;

		જો (available < buf_len) {
			પાછા ખોટું;
		}

		memcpy(&_ringbuffer[_end], buf, buf_len);
		_end += buf_len;

	} નહીંતર {
		// Add after end, maybe wrap around.
		અચલ size_t available = _start - _end - 1 + _size;

		જો (available < buf_len) {
			પાછા ખોટું;
		}

		અચલ size_t remaining_packet_len = _size - _end;

		જો (buf_len > remaining_packet_len) {
			memcpy(&_ringbuffer[_end], buf, remaining_packet_len);
			_end = 0;

			memcpy(&_ringbuffer[_end], buf + remaining_packet_len, buf_len - remaining_packet_len);
			_end += buf_len - remaining_packet_len;

		} નહીંતર {
			memcpy(&_ringbuffer[_end], buf, buf_len);
			_end += buf_len;
		}
	}

	પાછા સાચું;
}

size_t Ringbuffer::કાઢો_front(uint8_t *buf, size_t buf_max_len)
{
	જો (buf == nullptr) {
		// User needs to supply a valid pointer.
		પાછા 0;
	}

	જો (_start == _end) {
		// Empty
		પાછા 0;
	}

	જો (_start < _end) {

		// No wrap around.
		size_t to_copy_len = math::min(_end - _start, buf_max_len);

		memcpy(buf, &_ringbuffer[_start], to_copy_len);
		_start += to_copy_len;

		પાછા to_copy_len;

	} નહીંતર {
		// Potential wrap around.
		size_t to_copy_len = _end - _start + _size;

		જો (to_copy_len > buf_max_len) {
			to_copy_len = buf_max_len;
		}

		અચલ size_t remaining_buf_len = _size - _start;

		જો (to_copy_len > remaining_buf_len) {

			memcpy(buf, &_ringbuffer[_start], remaining_buf_len);
			_start = 0;
			memcpy(buf + remaining_buf_len, &_ringbuffer[_start], to_copy_len - remaining_buf_len);
			_start += to_copy_len - remaining_buf_len;

		} નહીંતર {
			memcpy(buf, &_ringbuffer[_start], to_copy_len);
			_start += to_copy_len;
		}

		પાછા to_copy_len;
	}
}
