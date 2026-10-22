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




#അന്തര്ഭാവ "VariableLengthRingbuffer.hpp"

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <string.h>


VariableLengthRingbuffer::~VariableLengthRingbuffer()
{
	deallocate();
}

തർക്കം VariableLengthRingbuffer::allocate(size_t buffer_size)
{
	തിരികെ _ringbuffer.allocate(buffer_size);
}

ശൂന്യം VariableLengthRingbuffer::deallocate()
{
	_ringbuffer.deallocate();
}

തർക്കം VariableLengthRingbuffer::push_back(സ്ഥിരം uint8_t *packet, size_t packet_len)
{
	എങ്കിൽ (packet_len == 0 || packet == nullptr) {
		// Nothing to add, we better don't try.
		തിരികെ അസത്യം;
	}

	size_t space_required = packet_len + വലുപ്പം(Header);

	എങ്കിൽ (space_required > _ringbuffer.space_available()) {
		തിരികെ അസത്യം;
	}

	Header header{static_cast<uint32_t>(packet_len)};
	തർക്കം result = _ringbuffer.push_back(reinterpret_cast<സ്ഥിരം uint8_t * >(&header), വലുപ്പം(header));
	പ്രതിജ്ഞാ(result);

	result = _ringbuffer.push_back(packet, packet_len);
	പ്രതിജ്ഞാ(result);

	// In case asserts are commented out to prevent unused warnings.
	(ശൂന്യം)result;

	തിരികെ സത്യം;
}

size_t VariableLengthRingbuffer::എടുക്കുക_front(uint8_t *buf, size_t buf_max_len)
{
	എങ്കിൽ (buf == nullptr) {
		// User needs to supply a valid pointer.
		തിരികെ 0;
	}

	// Check next header
	Header header;

	എങ്കിൽ (_ringbuffer.എടുക്കുക_front(reinterpret_cast<uint8_t *>(&header), വലുപ്പം(header)) < വലുപ്പം(header)) {
		തിരികെ 0;
	}

	// We can't fit the packet into the user supplied buffer.
	// This should never happen as the user has to supply a big // enough buffer.
	പ്രതിജ്ഞാ(static_cast<uint32_t>(header.len) <= buf_max_len);

	size_t bytes_read = _ringbuffer.എടുക്കുക_front(buf, header.len);
	പ്രതിജ്ഞാ(bytes_read == header.len);

	തിരികെ bytes_read;
}
