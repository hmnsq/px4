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

तार्किक Ringbuffer::allocate(size_t बफर_आकार)
{
	प्रतिज्ञा(_ringbuffer == शून्यसूचकः);

	आकार_2 = बफर_आकार;
	_ringbuffer = नूतन uint8_t[आकार_2];
	प्रत्यागम _ringbuffer != शून्यसूचकः;
}

शून्य Ringbuffer::deallocate()
{
	विलोपय[] _ringbuffer;
	_ringbuffer = शून्यसूचकः;
	आकार_2 = 0;
	_start = 0;
	अन्तः_3 = 0;
}

size_t Ringbuffer::space_उपलब्धम्() ध्रुव
{
	यदि (_start > अन्तः_3) {
		प्रत्यागम _start - अन्तः_3 - 1;

	} अन्यथा {
		प्रत्यागम _start - अन्तः_3 - 1 + आकार_2;
	}
}

size_t Ringbuffer::space_प्रयुक्तम्() ध्रुव
{
	यदि (_start <= अन्तः_3) {
		प्रत्यागम अन्तः_3 - _start;

	} अन्यथा {
		// Potential wrap around.
		प्रत्यागम अन्तः_3 - _start + आकार_2;
	}
}


तार्किक Ringbuffer::push_back(ध्रुव uint8_t *buf, size_t buf_len)
{
	यदि (buf_len == 0 || buf == शून्यसूचकः) {
		// Nothing to add, we better don't try.
		प्रत्यागम असत्य;
	}

	यदि (_start > अन्तः_3) {
		// Add after end up to start, no wrap around.

		// Leave one byte free so that start don't end up the same
		// which signals empty.
		ध्रुव size_t उपलब्धम् = _start - अन्तः_3 - 1;

		यदि (उपलब्धम् < buf_len) {
			प्रत्यागम असत्य;
		}

		स्मृतिं_प्रतिलिख(&_ringbuffer[अन्तः_3], buf, buf_len);
		अन्तः_3 += buf_len;

	} अन्यथा {
		// Add after end, maybe wrap around.
		ध्रुव size_t उपलब्धम् = _start - अन्तः_3 - 1 + आकार_2;

		यदि (उपलब्धम् < buf_len) {
			प्रत्यागम असत्य;
		}

		ध्रुव size_t remaining_packet_len = आकार_2 - अन्तः_3;

		यदि (buf_len > remaining_packet_len) {
			स्मृतिं_प्रतिलिख(&_ringbuffer[अन्तः_3], buf, remaining_packet_len);
			अन्तः_3 = 0;

			स्मृतिं_प्रतिलिख(&_ringbuffer[अन्तः_3], buf + remaining_packet_len, buf_len - remaining_packet_len);
			अन्तः_3 += buf_len - remaining_packet_len;

		} अन्यथा {
			स्मृतिं_प्रतिलिख(&_ringbuffer[अन्तः_3], buf, buf_len);
			अन्तः_3 += buf_len;
		}
	}

	प्रत्यागम सत्य;
}

size_t Ringbuffer::निष्कासय_front(uint8_t *buf, size_t buf_max_len)
{
	यदि (buf == शून्यसूचकः) {
		// User needs to supply a valid pointer.
		प्रत्यागम 0;
	}

	यदि (_start == अन्तः_3) {
		// Empty
		प्रत्यागम 0;
	}

	यदि (_start < अन्तः_3) {

		// No wrap around.
		size_t गन्तव्यम्_copy_len = math::min(अन्तः_3 - _start, buf_max_len);

		स्मृतिं_प्रतिलिख(buf, &_ringbuffer[_start], गन्तव्यम्_copy_len);
		_start += गन्तव्यम्_copy_len;

		प्रत्यागम गन्तव्यम्_copy_len;

	} अन्यथा {
		// Potential wrap around.
		size_t गन्तव्यम्_copy_len = अन्तः_3 - _start + आकार_2;

		यदि (गन्तव्यम्_copy_len > buf_max_len) {
			गन्तव्यम्_copy_len = buf_max_len;
		}

		ध्रुव size_t remaining_buf_len = आकार_2 - _start;

		यदि (गन्तव्यम्_copy_len > remaining_buf_len) {

			स्मृतिं_प्रतिलिख(buf, &_ringbuffer[_start], remaining_buf_len);
			_start = 0;
			स्मृतिं_प्रतिलिख(buf + remaining_buf_len, &_ringbuffer[_start], गन्तव्यम्_copy_len - remaining_buf_len);
			_start += गन्तव्यम्_copy_len - remaining_buf_len;

		} अन्यथा {
			स्मृतिं_प्रतिलिख(buf, &_ringbuffer[_start], गन्तव्यम्_copy_len);
			_start += गन्तव्यम्_copy_len;
		}

		प्रत्यागम गन्तव्यम्_copy_len;
	}
}
