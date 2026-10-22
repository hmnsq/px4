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




#অন্তর্ভাৱ "VariableLengthRingbuffer.hpp"

#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <string.h>


VariableLengthRingbuffer::~VariableLengthRingbuffer()
{
	deallocate();
}

তার্কিক VariableLengthRingbuffer::allocate(size_t buffer_size)
{
	ঘূৰাই _ringbuffer.allocate(buffer_size);
}

শূন্য VariableLengthRingbuffer::deallocate()
{
	_ringbuffer.deallocate();
}

তার্কিক VariableLengthRingbuffer::push_back(ধ্রুৱ uint8_t *packet, size_t packet_len)
{
	যদি (packet_len == 0 || packet == nullptr) {
		// Nothing to add, we better don't try.
		ঘূৰাই মিছা;
	}

	size_t space_required = packet_len + পরিমাণ(Header);

	যদি (space_required > _ringbuffer.space_available()) {
		ঘূৰাই মিছা;
	}

	Header header{static_cast<uint32_t>(packet_len)};
	তার্কিক result = _ringbuffer.push_back(reinterpret_cast<ধ্রুৱ uint8_t * >(&header), পরিমাণ(header));
	প্রতিজ্ঞা(result);

	result = _ringbuffer.push_back(packet, packet_len);
	প্রতিজ্ঞা(result);

	// In case asserts are commented out to prevent unused warnings.
	(শূন্য)result;

	ঘূৰাই সঁচা;
}

size_t VariableLengthRingbuffer::বেৰ_কৰো_front(uint8_t *buf, size_t buf_max_len)
{
	যদি (buf == nullptr) {
		// User needs to supply a valid pointer.
		ঘূৰাই 0;
	}

	// Check next header
	Header header;

	যদি (_ringbuffer.বেৰ_কৰো_front(reinterpret_cast<uint8_t *>(&header), পরিমাণ(header)) < পরিমাণ(header)) {
		ঘূৰাই 0;
	}

	// We can't fit the packet into the user supplied buffer.
	// This should never happen as the user has to supply a big // enough buffer.
	প্রতিজ্ঞা(static_cast<uint32_t>(header.len) <= buf_max_len);

	size_t bytes_read = _ringbuffer.বেৰ_কৰো_front(buf, header.len);
	প্রতিজ্ঞা(bytes_read == header.len);

	ঘূৰাই bytes_read;
}
