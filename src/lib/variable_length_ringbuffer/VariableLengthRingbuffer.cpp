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




#અન્તર્ભાવ "VariableLengthRingbuffer.hpp"

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <string.h>


VariableLengthRingbuffer::~VariableLengthRingbuffer()
{
	deallocate();
}

તાર્કિક VariableLengthRingbuffer::allocate(size_t buffer_size)
{
	પાછા _ringbuffer.allocate(buffer_size);
}

શૂન્ય VariableLengthRingbuffer::deallocate()
{
	_ringbuffer.deallocate();
}

તાર્કિક VariableLengthRingbuffer::push_back(અચલ uint8_t *packet, size_t packet_len)
{
	જો (packet_len == 0 || packet == nullptr) {
		// Nothing to add, we better don't try.
		પાછા ખોટું;
	}

	size_t space_required = packet_len + કદ(Header);

	જો (space_required > _ringbuffer.space_available()) {
		પાછા ખોટું;
	}

	Header header{static_cast<uint32_t>(packet_len)};
	તાર્કિક result = _ringbuffer.push_back(reinterpret_cast<અચલ uint8_t * >(&header), કદ(header));
	પ્રતિજ્ઞા(result);

	result = _ringbuffer.push_back(packet, packet_len);
	પ્રતિજ્ઞા(result);

	// In case asserts are commented out to prevent unused warnings.
	(શૂન્ય)result;

	પાછા સાચું;
}

size_t VariableLengthRingbuffer::કાઢો_front(uint8_t *buf, size_t buf_max_len)
{
	જો (buf == nullptr) {
		// User needs to supply a valid pointer.
		પાછા 0;
	}

	// Check next header
	Header header;

	જો (_ringbuffer.કાઢો_front(reinterpret_cast<uint8_t *>(&header), કદ(header)) < કદ(header)) {
		પાછા 0;
	}

	// We can't fit the packet into the user supplied buffer.
	// This should never happen as the user has to supply a big // enough buffer.
	પ્રતિજ્ઞા(static_cast<uint32_t>(header.len) <= buf_max_len);

	size_t bytes_read = _ringbuffer.કાઢો_front(buf, header.len);
	પ્રતિજ્ઞા(bytes_read == header.len);

	પાછા bytes_read;
}
