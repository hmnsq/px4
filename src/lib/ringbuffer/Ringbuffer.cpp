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




#അന്തര്ഭാവ "Ringbuffer.hpp"

#അന്തര്ഭാവ <mathlib/mathlib.h>
#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <string.h>


Ringbuffer::~Ringbuffer()
{
	deallocate();
}

തർക്കം Ringbuffer::allocate(size_t buffer_size)
{
	പ്രതിജ്ഞാ(_ringbuffer == nullptr);

	_size = buffer_size;
	_ringbuffer = പുതിയ uint8_t[_size];
	തിരികെ _ringbuffer != nullptr;
}

ശൂന്യം Ringbuffer::deallocate()
{
	delete[] _ringbuffer;
	_ringbuffer = nullptr;
	_size = 0;
	_start = 0;
	_end = 0;
}

size_t Ringbuffer::space_available() സ്ഥിരം
{
	എങ്കിൽ (_start > _end) {
		തിരികെ _start - _end - 1;

	} അല്ലെങ്കിൽ {
		തിരികെ _start - _end - 1 + _size;
	}
}

size_t Ringbuffer::space_used() സ്ഥിരം
{
	എങ്കിൽ (_start <= _end) {
		തിരികെ _end - _start;

	} അല്ലെങ്കിൽ {
		// Potential wrap around.
		തിരികെ _end - _start + _size;
	}
}


തർക്കം Ringbuffer::push_back(സ്ഥിരം uint8_t *buf, size_t buf_len)
{
	എങ്കിൽ (buf_len == 0 || buf == nullptr) {
		// Nothing to add, we better don't try.
		തിരികെ അസത്യം;
	}

	എങ്കിൽ (_start > _end) {
		// Add after end up to start, no wrap around.

		// Leave one byte free so that start don't end up the same
		// which signals empty.
		സ്ഥിരം size_t available = _start - _end - 1;

		എങ്കിൽ (available < buf_len) {
			തിരികെ അസത്യം;
		}

		memcpy(&_ringbuffer[_end], buf, buf_len);
		_end += buf_len;

	} അല്ലെങ്കിൽ {
		// Add after end, maybe wrap around.
		സ്ഥിരം size_t available = _start - _end - 1 + _size;

		എങ്കിൽ (available < buf_len) {
			തിരികെ അസത്യം;
		}

		സ്ഥിരം size_t remaining_packet_len = _size - _end;

		എങ്കിൽ (buf_len > remaining_packet_len) {
			memcpy(&_ringbuffer[_end], buf, remaining_packet_len);
			_end = 0;

			memcpy(&_ringbuffer[_end], buf + remaining_packet_len, buf_len - remaining_packet_len);
			_end += buf_len - remaining_packet_len;

		} അല്ലെങ്കിൽ {
			memcpy(&_ringbuffer[_end], buf, buf_len);
			_end += buf_len;
		}
	}

	തിരികെ സത്യം;
}

size_t Ringbuffer::എടുക്കുക_front(uint8_t *buf, size_t buf_max_len)
{
	എങ്കിൽ (buf == nullptr) {
		// User needs to supply a valid pointer.
		തിരികെ 0;
	}

	എങ്കിൽ (_start == _end) {
		// Empty
		തിരികെ 0;
	}

	എങ്കിൽ (_start < _end) {

		// No wrap around.
		size_t to_copy_len = math::min(_end - _start, buf_max_len);

		memcpy(buf, &_ringbuffer[_start], to_copy_len);
		_start += to_copy_len;

		തിരികെ to_copy_len;

	} അല്ലെങ്കിൽ {
		// Potential wrap around.
		size_t to_copy_len = _end - _start + _size;

		എങ്കിൽ (to_copy_len > buf_max_len) {
			to_copy_len = buf_max_len;
		}

		സ്ഥിരം size_t remaining_buf_len = _size - _start;

		എങ്കിൽ (to_copy_len > remaining_buf_len) {

			memcpy(buf, &_ringbuffer[_start], remaining_buf_len);
			_start = 0;
			memcpy(buf + remaining_buf_len, &_ringbuffer[_start], to_copy_len - remaining_buf_len);
			_start += to_copy_len - remaining_buf_len;

		} അല്ലെങ്കിൽ {
			memcpy(buf, &_ringbuffer[_start], to_copy_len);
			_start += to_copy_len;
		}

		തിരികെ to_copy_len;
	}
}
