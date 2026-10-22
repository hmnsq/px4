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




#ଅନ୍ତର୍ଭାଵ "VariableLengthRingbuffer.hpp"

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <string.h>


VariableLengthRingbuffer::~VariableLengthRingbuffer()
{
	deallocate();
}

ତାର୍କିକ VariableLengthRingbuffer::allocate(size_t buffer_size)
{
	ଫେରନ୍ତୁ _ringbuffer.allocate(buffer_size);
}

ଶୂନ୍ୟ VariableLengthRingbuffer::deallocate()
{
	_ringbuffer.deallocate();
}

ତାର୍କିକ VariableLengthRingbuffer::push_back(ଅଚଳ uint8_t *packet, size_t packet_len)
{
	ଯଦି (packet_len == 0 || packet == nullptr) {
		// Nothing to add, we better don't try.
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	size_t space_required = packet_len + ଆକାର(Header);

	ଯଦି (space_required > _ringbuffer.space_available()) {
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	Header header{static_cast<uint32_t>(packet_len)};
	ତାର୍କିକ result = _ringbuffer.push_back(reinterpret_cast<ଅଚଳ uint8_t * >(&header), ଆକାର(header));
	ପ୍ରତିଜ୍ଞା(result);

	result = _ringbuffer.push_back(packet, packet_len);
	ପ୍ରତିଜ୍ଞା(result);

	// In case asserts are commented out to prevent unused warnings.
	(ଶୂନ୍ୟ)result;

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

size_t VariableLengthRingbuffer::କାଢ଼_front(uint8_t *buf, size_t buf_max_len)
{
	ଯଦି (buf == nullptr) {
		// User needs to supply a valid pointer.
		ଫେରନ୍ତୁ 0;
	}

	// Check next header
	Header header;

	ଯଦି (_ringbuffer.କାଢ଼_front(reinterpret_cast<uint8_t *>(&header), ଆକାର(header)) < ଆକାର(header)) {
		ଫେରନ୍ତୁ 0;
	}

	// We can't fit the packet into the user supplied buffer.
	// This should never happen as the user has to supply a big // enough buffer.
	ପ୍ରତିଜ୍ଞା(static_cast<uint32_t>(header.len) <= buf_max_len);

	size_t bytes_read = _ringbuffer.କାଢ଼_front(buf, header.len);
	ପ୍ରତିଜ୍ଞା(bytes_read == header.len);

	ଫେରନ୍ତୁ bytes_read;
}
