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




#अन्तर्भाव "Ringbuffer.hpp"

#अन्तर्भाव <mathlib/mathlib.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <string.h>


Ringbuffer::~Ringbuffer()
{
	deallocate();
}

तार्किक Ringbuffer::allocate(माप_प्रकार buffer_size)
{
	प्रतिज्ञा(_ringbuffer == nullptr);

	_size = buffer_size;
	_ringbuffer = नवीन uint8_t[_size];
	परत _ringbuffer != nullptr;
}

शून्य Ringbuffer::deallocate()
{
	delete[] _ringbuffer;
	_ringbuffer = nullptr;
	_size = 0;
	_start = 0;
	_end = 0;
}

माप_प्रकार Ringbuffer::space_available() स्थिरांक
{
	जर (_start > _end) {
		परत _start - _end - 1;

	} अन्यथा {
		परत _start - _end - 1 + _size;
	}
}

माप_प्रकार Ringbuffer::space_used() स्थिरांक
{
	जर (_start <= _end) {
		परत _end - _start;

	} अन्यथा {
		// Potential wrap around.
		परत _end - _start + _size;
	}
}


तार्किक Ringbuffer::push_back(स्थिरांक uint8_t *buf, माप_प्रकार buf_len)
{
	जर (buf_len == 0 || buf == nullptr) {
		// Nothing to add, we better don't try.
		परत असत्य;
	}

	जर (_start > _end) {
		// Add after end up to start, no wrap around.

		// Leave one byte free so that start don't end up the same
		// which signals empty.
		स्थिरांक माप_प्रकार available = _start - _end - 1;

		जर (available < buf_len) {
			परत असत्य;
		}

		स_नकल(&_ringbuffer[_end], buf, buf_len);
		_end += buf_len;

	} अन्यथा {
		// Add after end, maybe wrap around.
		स्थिरांक माप_प्रकार available = _start - _end - 1 + _size;

		जर (available < buf_len) {
			परत असत्य;
		}

		स्थिरांक माप_प्रकार remaining_packet_len = _size - _end;

		जर (buf_len > remaining_packet_len) {
			स_नकल(&_ringbuffer[_end], buf, remaining_packet_len);
			_end = 0;

			स_नकल(&_ringbuffer[_end], buf + remaining_packet_len, buf_len - remaining_packet_len);
			_end += buf_len - remaining_packet_len;

		} अन्यथा {
			स_नकल(&_ringbuffer[_end], buf, buf_len);
			_end += buf_len;
		}
	}

	परत सत्य;
}

माप_प्रकार Ringbuffer::काढा_front(uint8_t *buf, माप_प्रकार buf_max_len)
{
	जर (buf == nullptr) {
		// User needs to supply a valid pointer.
		परत 0;
	}

	जर (_start == _end) {
		// Empty
		परत 0;
	}

	जर (_start < _end) {

		// No wrap around.
		माप_प्रकार to_copy_len = math::min(_end - _start, buf_max_len);

		स_नकल(buf, &_ringbuffer[_start], to_copy_len);
		_start += to_copy_len;

		परत to_copy_len;

	} अन्यथा {
		// Potential wrap around.
		माप_प्रकार to_copy_len = _end - _start + _size;

		जर (to_copy_len > buf_max_len) {
			to_copy_len = buf_max_len;
		}

		स्थिरांक माप_प्रकार remaining_buf_len = _size - _start;

		जर (to_copy_len > remaining_buf_len) {

			स_नकल(buf, &_ringbuffer[_start], remaining_buf_len);
			_start = 0;
			स_नकल(buf + remaining_buf_len, &_ringbuffer[_start], to_copy_len - remaining_buf_len);
			_start += to_copy_len - remaining_buf_len;

		} अन्यथा {
			स_नकल(buf, &_ringbuffer[_start], to_copy_len);
			_start += to_copy_len;
		}

		परत to_copy_len;
	}
}
