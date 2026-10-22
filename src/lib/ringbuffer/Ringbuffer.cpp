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




#ਅਨ੍ਤਰ੍ਭਾਵ "Ringbuffer.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ <mathlib/mathlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>


Ringbuffer::~Ringbuffer()
{
	deallocate();
}

ਤਾਰਕਿਕ Ringbuffer::allocate(size_t ਬਫਰ_ਮਾਪ)
{
	ਪ੍ਰਤਿਜ੍ਞਾ(_ringbuffer == ਖਾਲੀ_ਸੂਚਕ);

	ਮਾਪ_2 = ਬਫਰ_ਮਾਪ;
	_ringbuffer = ਨਵਾਂ uint8_t[ਮਾਪ_2];
	ਵਾਪਸ _ringbuffer != ਖਾਲੀ_ਸੂਚਕ;
}

ਖਾਲੀ Ringbuffer::deallocate()
{
	ਮਿਟਾਓ[] _ringbuffer;
	_ringbuffer = ਖਾਲੀ_ਸੂਚਕ;
	ਮਾਪ_2 = 0;
	ਸ਼ੁਰੂ_3 = 0;
	ਸਿਰਾ_2 = 0;
}

size_t Ringbuffer::space_ਉਪਲਬਧ() ਅਚਲ
{
	ਜੇ (ਸ਼ੁਰੂ_3 > ਸਿਰਾ_2) {
		ਵਾਪਸ ਸ਼ੁਰੂ_3 - ਸਿਰਾ_2 - 1;

	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ ਸ਼ੁਰੂ_3 - ਸਿਰਾ_2 - 1 + ਮਾਪ_2;
	}
}

size_t Ringbuffer::space_ਵਰਤਿਆ() ਅਚਲ
{
	ਜੇ (ਸ਼ੁਰੂ_3 <= ਸਿਰਾ_2) {
		ਵਾਪਸ ਸਿਰਾ_2 - ਸ਼ੁਰੂ_3;

	} ਨਹੀਂਤਾਂ {
		// Potential wrap around.
		ਵਾਪਸ ਸਿਰਾ_2 - ਸ਼ੁਰੂ_3 + ਮਾਪ_2;
	}
}


ਤਾਰਕਿਕ Ringbuffer::push_back(ਅਚਲ uint8_t *buf, size_t buf_len)
{
	ਜੇ (buf_len == 0 || buf == ਖਾਲੀ_ਸੂਚਕ) {
		// Nothing to add, we better don't try.
		ਵਾਪਸ ਝੂਠ;
	}

	ਜੇ (ਸ਼ੁਰੂ_3 > ਸਿਰਾ_2) {
		// Add after end up to start, no wrap around.

		// Leave one byte free so that start don't end up the same
		// which signals empty.
		ਅਚਲ size_t ਉਪਲਬਧ = ਸ਼ੁਰੂ_3 - ਸਿਰਾ_2 - 1;

		ਜੇ (ਉਪਲਬਧ < buf_len) {
			ਵਾਪਸ ਝੂਠ;
		}

		ਸਮ੍ਰਿਤੀ_ਨਕਲ(&_ringbuffer[ਸਿਰਾ_2], buf, buf_len);
		ਸਿਰਾ_2 += buf_len;

	} ਨਹੀਂਤਾਂ {
		// Add after end, maybe wrap around.
		ਅਚਲ size_t ਉਪਲਬਧ = ਸ਼ੁਰੂ_3 - ਸਿਰਾ_2 - 1 + ਮਾਪ_2;

		ਜੇ (ਉਪਲਬਧ < buf_len) {
			ਵਾਪਸ ਝੂਠ;
		}

		ਅਚਲ size_t ਬਚਿਆ_packet_len = ਮਾਪ_2 - ਸਿਰਾ_2;

		ਜੇ (buf_len > ਬਚਿਆ_packet_len) {
			ਸਮ੍ਰਿਤੀ_ਨਕਲ(&_ringbuffer[ਸਿਰਾ_2], buf, ਬਚਿਆ_packet_len);
			ਸਿਰਾ_2 = 0;

			ਸਮ੍ਰਿਤੀ_ਨਕਲ(&_ringbuffer[ਸਿਰਾ_2], buf + ਬਚਿਆ_packet_len, buf_len - ਬਚਿਆ_packet_len);
			ਸਿਰਾ_2 += buf_len - ਬਚਿਆ_packet_len;

		} ਨਹੀਂਤਾਂ {
			ਸਮ੍ਰਿਤੀ_ਨਕਲ(&_ringbuffer[ਸਿਰਾ_2], buf, buf_len);
			ਸਿਰਾ_2 += buf_len;
		}
	}

	ਵਾਪਸ ਸੱਚ;
}

size_t Ringbuffer::ਕੱਢੋ_front(uint8_t *buf, size_t buf_max_len)
{
	ਜੇ (buf == ਖਾਲੀ_ਸੂਚਕ) {
		// User needs to supply a valid pointer.
		ਵਾਪਸ 0;
	}

	ਜੇ (ਸ਼ੁਰੂ_3 == ਸਿਰਾ_2) {
		// Empty
		ਵਾਪਸ 0;
	}

	ਜੇ (ਸ਼ੁਰੂ_3 < ਸਿਰਾ_2) {

		// No wrap around.
		size_t ਮੰਜ਼ਿਲ_copy_len = math::min(ਸਿਰਾ_2 - ਸ਼ੁਰੂ_3, buf_max_len);

		ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf, &_ringbuffer[ਸ਼ੁਰੂ_3], ਮੰਜ਼ਿਲ_copy_len);
		ਸ਼ੁਰੂ_3 += ਮੰਜ਼ਿਲ_copy_len;

		ਵਾਪਸ ਮੰਜ਼ਿਲ_copy_len;

	} ਨਹੀਂਤਾਂ {
		// Potential wrap around.
		size_t ਮੰਜ਼ਿਲ_copy_len = ਸਿਰਾ_2 - ਸ਼ੁਰੂ_3 + ਮਾਪ_2;

		ਜੇ (ਮੰਜ਼ਿਲ_copy_len > buf_max_len) {
			ਮੰਜ਼ਿਲ_copy_len = buf_max_len;
		}

		ਅਚਲ size_t ਬਚਿਆ_buf_len = ਮਾਪ_2 - ਸ਼ੁਰੂ_3;

		ਜੇ (ਮੰਜ਼ਿਲ_copy_len > ਬਚਿਆ_buf_len) {

			ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf, &_ringbuffer[ਸ਼ੁਰੂ_3], ਬਚਿਆ_buf_len);
			ਸ਼ੁਰੂ_3 = 0;
			ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf + ਬਚਿਆ_buf_len, &_ringbuffer[ਸ਼ੁਰੂ_3], ਮੰਜ਼ਿਲ_copy_len - ਬਚਿਆ_buf_len);
			ਸ਼ੁਰੂ_3 += ਮੰਜ਼ਿਲ_copy_len - ਬਚਿਆ_buf_len;

		} ਨਹੀਂਤਾਂ {
			ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf, &_ringbuffer[ਸ਼ੁਰੂ_3], ਮੰਜ਼ਿਲ_copy_len);
			ਸ਼ੁਰੂ_3 += ਮੰਜ਼ਿਲ_copy_len;
		}

		ਵਾਪਸ ਮੰਜ਼ਿਲ_copy_len;
	}
}
