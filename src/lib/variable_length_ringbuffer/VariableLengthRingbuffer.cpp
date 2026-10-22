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




#ضمن "VariableLengthRingbuffer.hpp"

#ضمن <assert.h>
#ضمن <string.h>


Variable_درازی_Ringbuffer::~Variable_درازی_Ringbuffer()
{
	deallocate();
}

منطقي Variable_درازی_Ringbuffer::allocate(size_t میانگیر_اندازه)
{
	أرجع _ringbuffer.allocate(میانگیر_اندازه);
}

فراغ Variable_درازی_Ringbuffer::deallocate()
{
	_ringbuffer.deallocate();
}

منطقي Variable_درازی_Ringbuffer::push_back(ثابت uint8_t *packet, size_t packet_len)
{
	إذا (packet_len == 0 || packet == اشاره_تهی) {
		// Nothing to add, we better don't try.
		أرجع خطأ;
	}

	size_t space_required = packet_len + حجم(سرآیند_2);

	إذا (space_required > _ringbuffer.space_موجود()) {
		أرجع خطأ;
	}

	سرآیند_2 سرآیند{static_cast<uint32_t>(packet_len)};
	منطقي نتیجه = _ringbuffer.push_back(reinterpret_cast<ثابت uint8_t * >(&سرآیند), حجم(سرآیند));
	أكد(نتیجه);

	نتیجه = _ringbuffer.push_back(packet, packet_len);
	أكد(نتیجه);

	// In case asserts are commented out to prevent unused warnings.
	(فراغ)نتیجه;

	أرجع درست;
}

size_t Variable_درازی_Ringbuffer::برداشت_front(uint8_t *buf, size_t buf_max_len)
{
	إذا (buf == اشاره_تهی) {
		// User needs to supply a valid pointer.
		أرجع 0;
	}

	// Check next header
	سرآیند_2 سرآیند;

	إذا (_ringbuffer.برداشت_front(reinterpret_cast<uint8_t *>(&سرآیند), حجم(سرآیند)) < حجم(سرآیند)) {
		أرجع 0;
	}

	// We can't fit the packet into the user supplied buffer.
	// This should never happen as the user has to supply a big // enough buffer.
	أكد(static_cast<uint32_t>(سرآیند.len) <= buf_max_len);

	size_t بایتها_read = _ringbuffer.برداشت_front(buf, سرآیند.len);
	أكد(بایتها_read == سرآیند.len);

	أرجع بایتها_read;
}
