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




#شامل "Ringbuffer.hpp"

#شامل <mathlib/mathlib.h>
#شامل <assert.h>
#شامل <string.h>


Ringbuffer::~Ringbuffer()
{
	deallocate();
}

منطقی Ringbuffer::allocate(size_t بفر_سائز)
{
	أكد(_ringbuffer == خالی_اشاریہ);

	سائز_2 = بفر_سائز;
	_ringbuffer = نیا uint8_t[سائز_2];
	واپس _ringbuffer != خالی_اشاریہ;
}

خالی Ringbuffer::deallocate()
{
	حذف[] _ringbuffer;
	_ringbuffer = خالی_اشاریہ;
	سائز_2 = 0;
	آغاز_3 = 0;
	انتہا_2 = 0;
}

size_t Ringbuffer::space_دستیاب() ثابت
{
	اگر (آغاز_3 > انتہا_2) {
		واپس آغاز_3 - انتہا_2 - 1;

	} ورنہ {
		واپس آغاز_3 - انتہا_2 - 1 + سائز_2;
	}
}

size_t Ringbuffer::space_مستعمل() ثابت
{
	اگر (آغاز_3 <= انتہا_2) {
		واپس انتہا_2 - آغاز_3;

	} ورنہ {
		// Potential wrap around.
		واپس انتہا_2 - آغاز_3 + سائز_2;
	}
}


منطقی Ringbuffer::push_back(ثابت uint8_t *buf, size_t buf_len)
{
	اگر (buf_len == 0 || buf == خالی_اشاریہ) {
		// Nothing to add, we better don't try.
		واپس جھوٹ;
	}

	اگر (آغاز_3 > انتہا_2) {
		// Add after end up to start, no wrap around.

		// Leave one byte free so that start don't end up the same
		// which signals empty.
		ثابت size_t دستیاب = آغاز_3 - انتہا_2 - 1;

		اگر (دستیاب < buf_len) {
			واپس جھوٹ;
		}

		حافظہ_نقل(&_ringbuffer[انتہا_2], buf, buf_len);
		انتہا_2 += buf_len;

	} ورنہ {
		// Add after end, maybe wrap around.
		ثابت size_t دستیاب = آغاز_3 - انتہا_2 - 1 + سائز_2;

		اگر (دستیاب < buf_len) {
			واپس جھوٹ;
		}

		ثابت size_t بقایا_packet_len = سائز_2 - انتہا_2;

		اگر (buf_len > بقایا_packet_len) {
			حافظہ_نقل(&_ringbuffer[انتہا_2], buf, بقایا_packet_len);
			انتہا_2 = 0;

			حافظہ_نقل(&_ringbuffer[انتہا_2], buf + بقایا_packet_len, buf_len - بقایا_packet_len);
			انتہا_2 += buf_len - بقایا_packet_len;

		} ورنہ {
			حافظہ_نقل(&_ringbuffer[انتہا_2], buf, buf_len);
			انتہا_2 += buf_len;
		}
	}

	واپس سچ;
}

size_t Ringbuffer::نکالو_front(uint8_t *buf, size_t buf_max_len)
{
	اگر (buf == خالی_اشاریہ) {
		// User needs to supply a valid pointer.
		واپس 0;
	}

	اگر (آغاز_3 == انتہا_2) {
		// Empty
		واپس 0;
	}

	اگر (آغاز_3 < انتہا_2) {

		// No wrap around.
		size_t مقصد_copy_len = math::min(انتہا_2 - آغاز_3, buf_max_len);

		حافظہ_نقل(buf, &_ringbuffer[آغاز_3], مقصد_copy_len);
		آغاز_3 += مقصد_copy_len;

		واپس مقصد_copy_len;

	} ورنہ {
		// Potential wrap around.
		size_t مقصد_copy_len = انتہا_2 - آغاز_3 + سائز_2;

		اگر (مقصد_copy_len > buf_max_len) {
			مقصد_copy_len = buf_max_len;
		}

		ثابت size_t بقایا_buf_len = سائز_2 - آغاز_3;

		اگر (مقصد_copy_len > بقایا_buf_len) {

			حافظہ_نقل(buf, &_ringbuffer[آغاز_3], بقایا_buf_len);
			آغاز_3 = 0;
			حافظہ_نقل(buf + بقایا_buf_len, &_ringbuffer[آغاز_3], مقصد_copy_len - بقایا_buf_len);
			آغاز_3 += مقصد_copy_len - بقایا_buf_len;

		} ورنہ {
			حافظہ_نقل(buf, &_ringbuffer[آغاز_3], مقصد_copy_len);
			آغاز_3 += مقصد_copy_len;
		}

		واپس مقصد_copy_len;
	}
}
