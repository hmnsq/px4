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

منطقي Ringbuffer::allocate(size_t بفر_اندازه)
{
	أكد(_ringbuffer == تش_اشاره);

	اندازه_2 = بفر_اندازه;
	_ringbuffer = جديد uint8_t[اندازه_2];
	أرجع _ringbuffer != تش_اشاره;
}

فراغ Ringbuffer::deallocate()
{
	ړنګول[] _ringbuffer;
	_ringbuffer = تش_اشاره;
	اندازه_2 = 0;
	پیل_3 = 0;
	پای_ټکی_2 = 0;
}

size_t Ringbuffer::space_شته() ثابت
{
	إذا (پیل_3 > پای_ټکی_2) {
		أرجع پیل_3 - پای_ټکی_2 - 1;

	} وإلا {
		أرجع پیل_3 - پای_ټکی_2 - 1 + اندازه_2;
	}
}

size_t Ringbuffer::space_کارول_شوی() ثابت
{
	إذا (پیل_3 <= پای_ټکی_2) {
		أرجع پای_ټکی_2 - پیل_3;

	} وإلا {
		// Potential wrap around.
		أرجع پای_ټکی_2 - پیل_3 + اندازه_2;
	}
}


منطقي Ringbuffer::push_back(ثابت uint8_t *buf, size_t buf_len)
{
	إذا (buf_len == 0 || buf == تش_اشاره) {
		// Nothing to add, we better don't try.
		أرجع خطأ;
	}

	إذا (پیل_3 > پای_ټکی_2) {
		// Add after end up to start, no wrap around.

		// Leave one byte free so that start don't end up the same
		// which signals empty.
		ثابت size_t شته = پیل_3 - پای_ټکی_2 - 1;

		إذا (شته < buf_len) {
			أرجع خطأ;
		}

		حافظه_نقل(&_ringbuffer[پای_ټکی_2], buf, buf_len);
		پای_ټکی_2 += buf_len;

	} وإلا {
		// Add after end, maybe wrap around.
		ثابت size_t شته = پیل_3 - پای_ټکی_2 - 1 + اندازه_2;

		إذا (شته < buf_len) {
			أرجع خطأ;
		}

		ثابت size_t پاتې_شوی_packet_len = اندازه_2 - پای_ټکی_2;

		إذا (buf_len > پاتې_شوی_packet_len) {
			حافظه_نقل(&_ringbuffer[پای_ټکی_2], buf, پاتې_شوی_packet_len);
			پای_ټکی_2 = 0;

			حافظه_نقل(&_ringbuffer[پای_ټکی_2], buf + پاتې_شوی_packet_len, buf_len - پاتې_شوی_packet_len);
			پای_ټکی_2 += buf_len - پاتې_شوی_packet_len;

		} وإلا {
			حافظه_نقل(&_ringbuffer[پای_ټکی_2], buf, buf_len);
			پای_ټکی_2 += buf_len;
		}
	}

	أرجع ریښتیا;
}

size_t Ringbuffer::راوباسه_front(uint8_t *buf, size_t buf_max_len)
{
	إذا (buf == تش_اشاره) {
		// User needs to supply a valid pointer.
		أرجع 0;
	}

	إذا (پیل_3 == پای_ټکی_2) {
		// Empty
		أرجع 0;
	}

	إذا (پیل_3 < پای_ټکی_2) {

		// No wrap around.
		size_t منزل_copy_len = math::min(پای_ټکی_2 - پیل_3, buf_max_len);

		حافظه_نقل(buf, &_ringbuffer[پیل_3], منزل_copy_len);
		پیل_3 += منزل_copy_len;

		أرجع منزل_copy_len;

	} وإلا {
		// Potential wrap around.
		size_t منزل_copy_len = پای_ټکی_2 - پیل_3 + اندازه_2;

		إذا (منزل_copy_len > buf_max_len) {
			منزل_copy_len = buf_max_len;
		}

		ثابت size_t پاتې_شوی_buf_len = اندازه_2 - پیل_3;

		إذا (منزل_copy_len > پاتې_شوی_buf_len) {

			حافظه_نقل(buf, &_ringbuffer[پیل_3], پاتې_شوی_buf_len);
			پیل_3 = 0;
			حافظه_نقل(buf + پاتې_شوی_buf_len, &_ringbuffer[پیل_3], منزل_copy_len - پاتې_شوی_buf_len);
			پیل_3 += منزل_copy_len - پاتې_شوی_buf_len;

		} وإلا {
			حافظه_نقل(buf, &_ringbuffer[پیل_3], منزل_copy_len);
			پیل_3 += منزل_copy_len;
		}

		أرجع منزل_copy_len;
	}
}
