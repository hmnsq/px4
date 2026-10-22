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




#ਅਨ੍ਤਰ੍ਭਾਵ "VariableLengthRingbuffer.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>


Variable_ਲੰਮਾਈ_Ringbuffer::~Variable_ਲੰਮਾਈ_Ringbuffer()
{
	deallocate();
}

ਤਾਰਕਿਕ Variable_ਲੰਮਾਈ_Ringbuffer::allocate(size_t ਬਫਰ_ਮਾਪ)
{
	ਵਾਪਸ _ringbuffer.allocate(ਬਫਰ_ਮਾਪ);
}

ਖਾਲੀ Variable_ਲੰਮਾਈ_Ringbuffer::deallocate()
{
	_ringbuffer.deallocate();
}

ਤਾਰਕਿਕ Variable_ਲੰਮਾਈ_Ringbuffer::push_back(ਅਚਲ uint8_t *packet, size_t packet_len)
{
	ਜੇ (packet_len == 0 || packet == ਖਾਲੀ_ਸੂਚਕ) {
		// Nothing to add, we better don't try.
		ਵਾਪਸ ਝੂਠ;
	}

	size_t space_required = packet_len + ਆਕਾਰ(ਸਿਰਲੇਖ_2);

	ਜੇ (space_required > _ringbuffer.space_ਉਪਲਬਧ()) {
		ਵਾਪਸ ਝੂਠ;
	}

	ਸਿਰਲੇਖ_2 ਸਿਰਲੇਖ{static_cast<uint32_t>(packet_len)};
	ਤਾਰਕਿਕ ਨਤੀਜਾ = _ringbuffer.push_back(reinterpret_cast<ਅਚਲ uint8_t * >(&ਸਿਰਲੇਖ), ਆਕਾਰ(ਸਿਰਲੇਖ));
	ਪ੍ਰਤਿਜ੍ਞਾ(ਨਤੀਜਾ);

	ਨਤੀਜਾ = _ringbuffer.push_back(packet, packet_len);
	ਪ੍ਰਤਿਜ੍ਞਾ(ਨਤੀਜਾ);

	// In case asserts are commented out to prevent unused warnings.
	(ਖਾਲੀ)ਨਤੀਜਾ;

	ਵਾਪਸ ਸੱਚ;
}

size_t Variable_ਲੰਮਾਈ_Ringbuffer::ਕੱਢੋ_front(uint8_t *buf, size_t buf_max_len)
{
	ਜੇ (buf == ਖਾਲੀ_ਸੂਚਕ) {
		// User needs to supply a valid pointer.
		ਵਾਪਸ 0;
	}

	// Check next header
	ਸਿਰਲੇਖ_2 ਸਿਰਲੇਖ;

	ਜੇ (_ringbuffer.ਕੱਢੋ_front(reinterpret_cast<uint8_t *>(&ਸਿਰਲੇਖ), ਆਕਾਰ(ਸਿਰਲੇਖ)) < ਆਕਾਰ(ਸਿਰਲੇਖ)) {
		ਵਾਪਸ 0;
	}

	// We can't fit the packet into the user supplied buffer.
	// This should never happen as the user has to supply a big // enough buffer.
	ਪ੍ਰਤਿਜ੍ਞਾ(static_cast<uint32_t>(ਸਿਰਲੇਖ.len) <= buf_max_len);

	size_t ਬਾਈਟਾਂ_read = _ringbuffer.ਕੱਢੋ_front(buf, ਸਿਰਲੇਖ.len);
	ਪ੍ਰਤਿਜ੍ਞਾ(ਬਾਈਟਾਂ_read == ਸਿਰਲੇਖ.len);

	ਵਾਪਸ ਬਾਈਟਾਂ_read;
}
