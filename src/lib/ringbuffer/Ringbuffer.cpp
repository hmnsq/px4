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

منطقي Ringbuffer::allocate(size_t میانگیر_اندازه)
{
	أكد(_ringbuffer == اشاره_تهی);

	اندازه_2 = میانگیر_اندازه;
	_ringbuffer = جديد uint8_t[اندازه_2];
	أرجع _ringbuffer != اشاره_تهی;
}

فراغ Ringbuffer::deallocate()
{
	حذف[] _ringbuffer;
	_ringbuffer = اشاره_تهی;
	اندازه_2 = 0;
	شروع_3 = 0;
	انتها_2 = 0;
}

size_t Ringbuffer::space_موجود() ثابت
{
	إذا (شروع_3 > انتها_2) {
		أرجع شروع_3 - انتها_2 - 1;

	} وإلا {
		أرجع شروع_3 - انتها_2 - 1 + اندازه_2;
	}
}

size_t Ringbuffer::space_استفاده_شده() ثابت
{
	إذا (شروع_3 <= انتها_2) {
		أرجع انتها_2 - شروع_3;

	} وإلا {
		// Potential wrap around.
		أرجع انتها_2 - شروع_3 + اندازه_2;
	}
}


منطقي Ringbuffer::push_back(ثابت uint8_t *buf, size_t buf_len)
{
	إذا (buf_len == 0 || buf == اشاره_تهی) {
		// Nothing to add, we better don't try.
		أرجع خطأ;
	}

	إذا (شروع_3 > انتها_2) {
		// Add after end up to start, no wrap around.

		// Leave one byte free so that start don't end up the same
		// which signals empty.
		ثابت size_t موجود = شروع_3 - انتها_2 - 1;

		إذا (موجود < buf_len) {
			أرجع خطأ;
		}

		کپی_حافظه(&_ringbuffer[انتها_2], buf, buf_len);
		انتها_2 += buf_len;

	} وإلا {
		// Add after end, maybe wrap around.
		ثابت size_t موجود = شروع_3 - انتها_2 - 1 + اندازه_2;

		إذا (موجود < buf_len) {
			أرجع خطأ;
		}

		ثابت size_t مانده_packet_len = اندازه_2 - انتها_2;

		إذا (buf_len > مانده_packet_len) {
			کپی_حافظه(&_ringbuffer[انتها_2], buf, مانده_packet_len);
			انتها_2 = 0;

			کپی_حافظه(&_ringbuffer[انتها_2], buf + مانده_packet_len, buf_len - مانده_packet_len);
			انتها_2 += buf_len - مانده_packet_len;

		} وإلا {
			کپی_حافظه(&_ringbuffer[انتها_2], buf, buf_len);
			انتها_2 += buf_len;
		}
	}

	أرجع درست;
}

size_t Ringbuffer::برداشت_front(uint8_t *buf, size_t buf_max_len)
{
	إذا (buf == اشاره_تهی) {
		// User needs to supply a valid pointer.
		أرجع 0;
	}

	إذا (شروع_3 == انتها_2) {
		// Empty
		أرجع 0;
	}

	إذا (شروع_3 < انتها_2) {

		// No wrap around.
		size_t مقصد_copy_len = math::min(انتها_2 - شروع_3, buf_max_len);

		کپی_حافظه(buf, &_ringbuffer[شروع_3], مقصد_copy_len);
		شروع_3 += مقصد_copy_len;

		أرجع مقصد_copy_len;

	} وإلا {
		// Potential wrap around.
		size_t مقصد_copy_len = انتها_2 - شروع_3 + اندازه_2;

		إذا (مقصد_copy_len > buf_max_len) {
			مقصد_copy_len = buf_max_len;
		}

		ثابت size_t مانده_buf_len = اندازه_2 - شروع_3;

		إذا (مقصد_copy_len > مانده_buf_len) {

			کپی_حافظه(buf, &_ringbuffer[شروع_3], مانده_buf_len);
			شروع_3 = 0;
			کپی_حافظه(buf + مانده_buf_len, &_ringbuffer[شروع_3], مقصد_copy_len - مانده_buf_len);
			شروع_3 += مقصد_copy_len - مانده_buf_len;

		} وإلا {
			کپی_حافظه(buf, &_ringbuffer[شروع_3], مقصد_copy_len);
			شروع_3 += مقصد_copy_len;
		}

		أرجع مقصد_copy_len;
	}
}
