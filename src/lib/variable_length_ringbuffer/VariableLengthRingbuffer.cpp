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


Variable_दीर्घता_Ringbuffer::~Variable_दीर्घता_Ringbuffer()
{
	deallocate();
}

तार्किक Variable_दीर्घता_Ringbuffer::allocate(size_t बफर_आकार)
{
	प्रत्यागम _ringbuffer.allocate(बफर_आकार);
}

शून्य Variable_दीर्घता_Ringbuffer::deallocate()
{
	_ringbuffer.deallocate();
}

तार्किक Variable_दीर्घता_Ringbuffer::push_back(ध्रुव uint8_t *packet, size_t packet_len)
{
	यदि (packet_len == 0 || packet == शून्यसूचकः) {
		// Nothing to add, we better don't try.
		प्रत्यागम असत्य;
	}

	size_t space_required = packet_len + परिमाण(शीर्षकम्_2);

	यदि (space_required > _ringbuffer.space_उपलब्धम्()) {
		प्रत्यागम असत्य;
	}

	शीर्षकम्_2 शीर्षकम्{static_cast<uint32_t>(packet_len)};
	तार्किक परिणाम = _ringbuffer.push_back(reinterpret_cast<ध्रुव uint8_t * >(&शीर्षकम्), परिमाण(शीर्षकम्));
	प्रतिज्ञा(परिणाम);

	परिणाम = _ringbuffer.push_back(packet, packet_len);
	प्रतिज्ञा(परिणाम);

	// In case asserts are commented out to prevent unused warnings.
	(शून्य)परिणाम;

	प्रत्यागम सत्य;
}

size_t Variable_दीर्घता_Ringbuffer::निष्कासय_front(uint8_t *buf, size_t buf_max_len)
{
	यदि (buf == शून्यसूचकः) {
		// User needs to supply a valid pointer.
		प्रत्यागम 0;
	}

	// Check next header
	शीर्षकम्_2 शीर्षकम्;

	यदि (_ringbuffer.निष्कासय_front(reinterpret_cast<uint8_t *>(&शीर्षकम्), परिमाण(शीर्षकम्)) < परिमाण(शीर्षकम्)) {
		प्रत्यागम 0;
	}

	// We can't fit the packet into the user supplied buffer.
	// This should never happen as the user has to supply a big // enough buffer.
	प्रतिज्ञा(static_cast<uint32_t>(शीर्षकम्.len) <= buf_max_len);

	size_t अष्टकानि_read = _ringbuffer.निष्कासय_front(buf, शीर्षकम्.len);
	प्रतिज्ञा(अष्टकानि_read == शीर्षकम्.len);

	प्रत्यागम अष्टकानि_read;
}
