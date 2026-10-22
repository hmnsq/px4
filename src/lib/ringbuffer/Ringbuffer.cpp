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




#ضمن "Ringbuffer.hpp"

#ضمن <mathlib/mathlib.h>
#ضمن <assert.h>
#ضمن <string.h>


Ringbuffer::~Ringbuffer()
{
	deallocate();
}

منطقي Ringbuffer::allocate(size_t بفر_ناپ)
{
	أكد(_ringbuffer == خالی_سوچک);

	ناپ_2 = بفر_ناپ;
	_ringbuffer = جديد uint8_t[ناپ_2];
	أرجع _ringbuffer != خالی_سوچک;
}

فراغ Ringbuffer::deallocate()
{
	مٹاؤ[] _ringbuffer;
	_ringbuffer = خالی_سوچک;
	ناپ_2 = 0;
	شروع_3 = 0;
	سرا_2 = 0;
}

size_t Ringbuffer::space_دستیاب() ثابت
{
	إذا (شروع_3 > سرا_2) {
		أرجع شروع_3 - سرا_2 - 1;

	} وإلا {
		أرجع شروع_3 - سرا_2 - 1 + ناپ_2;
	}
}

size_t Ringbuffer::space_ورتیا() ثابت
{
	إذا (شروع_3 <= سرا_2) {
		أرجع سرا_2 - شروع_3;

	} وإلا {
		// Potential wrap around.
		أرجع سرا_2 - شروع_3 + ناپ_2;
	}
}


منطقي Ringbuffer::push_back(ثابت uint8_t *buf, size_t buf_len)
{
	إذا (buf_len == 0 || buf == خالی_سوچک) {
		// Nothing to add, we better don't try.
		أرجع خطأ;
	}

	إذا (شروع_3 > سرا_2) {
		// Add after end up to start, no wrap around.

		// Leave one byte free so that start don't end up the same
		// which signals empty.
		ثابت size_t دستیاب = شروع_3 - سرا_2 - 1;

		إذا (دستیاب < buf_len) {
			أرجع خطأ;
		}

		حافظہ_نقل(&_ringbuffer[سرا_2], buf, buf_len);
		سرا_2 += buf_len;

	} وإلا {
		// Add after end, maybe wrap around.
		ثابت size_t دستیاب = شروع_3 - سرا_2 - 1 + ناپ_2;

		إذا (دستیاب < buf_len) {
			أرجع خطأ;
		}

		ثابت size_t بچیا_packet_len = ناپ_2 - سرا_2;

		إذا (buf_len > بچیا_packet_len) {
			حافظہ_نقل(&_ringbuffer[سرا_2], buf, بچیا_packet_len);
			سرا_2 = 0;

			حافظہ_نقل(&_ringbuffer[سرا_2], buf + بچیا_packet_len, buf_len - بچیا_packet_len);
			سرا_2 += buf_len - بچیا_packet_len;

		} وإلا {
			حافظہ_نقل(&_ringbuffer[سرا_2], buf, buf_len);
			سرا_2 += buf_len;
		}
	}

	أرجع سچ;
}

size_t Ringbuffer::نکالو_front(uint8_t *buf, size_t buf_max_len)
{
	إذا (buf == خالی_سوچک) {
		// User needs to supply a valid pointer.
		أرجع 0;
	}

	إذا (شروع_3 == سرا_2) {
		// Empty
		أرجع 0;
	}

	إذا (شروع_3 < سرا_2) {

		// No wrap around.
		size_t منزل_copy_len = math::min(سرا_2 - شروع_3, buf_max_len);

		حافظہ_نقل(buf, &_ringbuffer[شروع_3], منزل_copy_len);
		شروع_3 += منزل_copy_len;

		أرجع منزل_copy_len;

	} وإلا {
		// Potential wrap around.
		size_t منزل_copy_len = سرا_2 - شروع_3 + ناپ_2;

		إذا (منزل_copy_len > buf_max_len) {
			منزل_copy_len = buf_max_len;
		}

		ثابت size_t بچیا_buf_len = ناپ_2 - شروع_3;

		إذا (منزل_copy_len > بچیا_buf_len) {

			حافظہ_نقل(buf, &_ringbuffer[شروع_3], بچیا_buf_len);
			شروع_3 = 0;
			حافظہ_نقل(buf + بچیا_buf_len, &_ringbuffer[شروع_3], منزل_copy_len - بچیا_buf_len);
			شروع_3 += منزل_copy_len - بچیا_buf_len;

		} وإلا {
			حافظہ_نقل(buf, &_ringbuffer[شروع_3], منزل_copy_len);
			شروع_3 += منزل_copy_len;
		}

		أرجع منزل_copy_len;
	}
}
