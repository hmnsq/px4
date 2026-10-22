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




#ಅನ್ತರ್ಭಾವ "VariableLengthRingbuffer.hpp"

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <string.h>


Variable_ಉದ್ದಳತೆ_Ringbuffer::~Variable_ಉದ್ದಳತೆ_Ringbuffer()
{
	deallocate();
}

ತಾರ್ಕಿಕ Variable_ಉದ್ದಳತೆ_Ringbuffer::allocate(size_t ಬಫರ್_ಗಾತ್ರ)
{
	ಹಿಂತಿರುಗಿ _ringbuffer.allocate(ಬಫರ್_ಗಾತ್ರ);
}

ಶೂನ್ಯ Variable_ಉದ್ದಳತೆ_Ringbuffer::deallocate()
{
	_ringbuffer.deallocate();
}

ತಾರ್ಕಿಕ Variable_ಉದ್ದಳತೆ_Ringbuffer::push_back(ಸ್ಥಿರಾಂಕ uint8_t *packet, size_t packet_len)
{
	ಆದರೆ (packet_len == 0 || packet == ಶೂನ್ಯಸೂಚಕ) {
		// Nothing to add, we better don't try.
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	size_t space_required = packet_len + ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ_2);

	ಆದರೆ (space_required > _ringbuffer.space_ಲಭ್ಯ()) {
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಶೀರ್ಷಿಕೆ_2 ಶೀರ್ಷಿಕೆ{static_cast<uint32_t>(packet_len)};
	ತಾರ್ಕಿಕ ಫಲಿತಾಂಶ = _ringbuffer.push_back(reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t * >(&ಶೀರ್ಷಿಕೆ), ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ));
	ಪ್ರತಿಜ್ಞಾ(ಫಲಿತಾಂಶ);

	ಫಲಿತಾಂಶ = _ringbuffer.push_back(packet, packet_len);
	ಪ್ರತಿಜ್ಞಾ(ಫಲಿತಾಂಶ);

	// In case asserts are commented out to prevent unused warnings.
	(ಶೂನ್ಯ)ಫಲಿತಾಂಶ;

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

size_t Variable_ಉದ್ದಳತೆ_Ringbuffer::ತೆಗೆ_front(uint8_t *buf, size_t buf_max_len)
{
	ಆದರೆ (buf == ಶೂನ್ಯಸೂಚಕ) {
		// User needs to supply a valid pointer.
		ಹಿಂತಿರುಗಿ 0;
	}

	// Check next header
	ಶೀರ್ಷಿಕೆ_2 ಶೀರ್ಷಿಕೆ;

	ಆದರೆ (_ringbuffer.ತೆಗೆ_front(reinterpret_cast<uint8_t *>(&ಶೀರ್ಷಿಕೆ), ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ)) < ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ)) {
		ಹಿಂತಿರುಗಿ 0;
	}

	// We can't fit the packet into the user supplied buffer.
	// This should never happen as the user has to supply a big // enough buffer.
	ಪ್ರತಿಜ್ಞಾ(static_cast<uint32_t>(ಶೀರ್ಷಿಕೆ.len) <= buf_max_len);

	size_t ಬೈಟುಗಳು_read = _ringbuffer.ತೆಗೆ_front(buf, ಶೀರ್ಷಿಕೆ.len);
	ಪ್ರತಿಜ್ಞಾ(ಬೈಟುಗಳು_read == ಶೀರ್ಷಿಕೆ.len);

	ಹಿಂತಿರುಗಿ ಬೈಟುಗಳು_read;
}
