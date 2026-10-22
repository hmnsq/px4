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




#अन्तर्भाव "VariableLengthRingbuffer.hpp"

#अन्तर्भाव <assert.h>
#अन्तर्भाव <string.h>


VariableLengthRingbuffer::~VariableLengthRingbuffer()
{
	deallocate();
}

तार्किक VariableLengthRingbuffer::allocate(माप_प्रकार buffer_size)
{
	फर्काऊ _ringbuffer.allocate(buffer_size);
}

शून्य VariableLengthRingbuffer::deallocate()
{
	_ringbuffer.deallocate();
}

तार्किक VariableLengthRingbuffer::push_back(अचर uint8_t *packet, माप_प्रकार packet_len)
{
	यदि (packet_len == 0 || packet == nullptr) {
		// Nothing to add, we better don't try.
		फर्काऊ असत्य;
	}

	माप_प्रकार space_required = packet_len + परिमाण(Header);

	यदि (space_required > _ringbuffer.space_available()) {
		फर्काऊ असत्य;
	}

	Header header{static_cast<uint32_t>(packet_len)};
	तार्किक result = _ringbuffer.push_back(reinterpret_cast<अचर uint8_t * >(&header), परिमाण(header));
	प्रतिज्ञा(result);

	result = _ringbuffer.push_back(packet, packet_len);
	प्रतिज्ञा(result);

	// In case asserts are commented out to prevent unused warnings.
	(शून्य)result;

	फर्काऊ सत्य;
}

माप_प्रकार VariableLengthRingbuffer::झिक_front(uint8_t *buf, माप_प्रकार buf_max_len)
{
	यदि (buf == nullptr) {
		// User needs to supply a valid pointer.
		फर्काऊ 0;
	}

	// Check next header
	Header header;

	यदि (_ringbuffer.झिक_front(reinterpret_cast<uint8_t *>(&header), परिमाण(header)) < परिमाण(header)) {
		फर्काऊ 0;
	}

	// We can't fit the packet into the user supplied buffer.
	// This should never happen as the user has to supply a big // enough buffer.
	प्रतिज्ञा(static_cast<uint32_t>(header.len) <= buf_max_len);

	माप_प्रकार bytes_read = _ringbuffer.झिक_front(buf, header.len);
	प्रतिज्ञा(bytes_read == header.len);

	फर्काऊ bytes_read;
}
