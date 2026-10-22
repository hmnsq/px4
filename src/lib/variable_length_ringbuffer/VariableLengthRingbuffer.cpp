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


Variable_اوږدتیا_Ringbuffer::~Variable_اوږدتیا_Ringbuffer()
{
	deallocate();
}

منطقي Variable_اوږدتیا_Ringbuffer::allocate(size_t بفر_اندازه)
{
	أرجع _ringbuffer.allocate(بفر_اندازه);
}

فراغ Variable_اوږدتیا_Ringbuffer::deallocate()
{
	_ringbuffer.deallocate();
}

منطقي Variable_اوږدتیا_Ringbuffer::push_back(ثابت uint8_t *packet, size_t packet_len)
{
	إذا (packet_len == 0 || packet == تش_اشاره) {
		// Nothing to add, we better don't try.
		أرجع خطأ;
	}

	size_t space_required = packet_len + حجم(سرلیک_2);

	إذا (space_required > _ringbuffer.space_شته()) {
		أرجع خطأ;
	}

	سرلیک_2 سرلیک{static_cast<uint32_t>(packet_len)};
	منطقي پایله = _ringbuffer.push_back(reinterpret_cast<ثابت uint8_t * >(&سرلیک), حجم(سرلیک));
	أكد(پایله);

	پایله = _ringbuffer.push_back(packet, packet_len);
	أكد(پایله);

	// In case asserts are commented out to prevent unused warnings.
	(فراغ)پایله;

	أرجع ریښتیا;
}

size_t Variable_اوږدتیا_Ringbuffer::راوباسه_front(uint8_t *buf, size_t buf_max_len)
{
	إذا (buf == تش_اشاره) {
		// User needs to supply a valid pointer.
		أرجع 0;
	}

	// Check next header
	سرلیک_2 سرلیک;

	إذا (_ringbuffer.راوباسه_front(reinterpret_cast<uint8_t *>(&سرلیک), حجم(سرلیک)) < حجم(سرلیک)) {
		أرجع 0;
	}

	// We can't fit the packet into the user supplied buffer.
	// This should never happen as the user has to supply a big // enough buffer.
	أكد(static_cast<uint32_t>(سرلیک.len) <= buf_max_len);

	size_t بایټونه_read = _ringbuffer.راوباسه_front(buf, سرلیک.len);
	أكد(بایټونه_read == سرلیک.len);

	أرجع بایټونه_read;
}
