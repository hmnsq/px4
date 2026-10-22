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




#అన్తర్భావ "VariableLengthRingbuffer.hpp"

#అన్తర్భావ <assert.h>
#అన్తర్భావ <string.h>


Variable_నిడివి_Ringbuffer::~Variable_నిడివి_Ringbuffer()
{
	deallocate();
}

తార్కిక Variable_నిడివి_Ringbuffer::allocate(size_t బఫరు_కొలత)
{
	తిరిగి _ringbuffer.allocate(బఫరు_కొలత);
}

శూన్య Variable_నిడివి_Ringbuffer::deallocate()
{
	_ringbuffer.deallocate();
}

తార్కిక Variable_నిడివి_Ringbuffer::push_back(స్థిరరాశి uint8_t *packet, size_t packet_len)
{
	ఒకవేళ (packet_len == 0 || packet == శూన్యసూచిక) {
		// Nothing to add, we better don't try.
		తిరిగి అబద్ధం;
	}

	size_t space_required = packet_len + పరిమాణం(శీర్షిక_2);

	ఒకవేళ (space_required > _ringbuffer.space_అందుబాటు()) {
		తిరిగి అబద్ధం;
	}

	శీర్షిక_2 శీర్షిక{static_cast<uint32_t>(packet_len)};
	తార్కిక ఫలితం = _ringbuffer.push_back(reinterpret_cast<స్థిరరాశి uint8_t * >(&శీర్షిక), పరిమాణం(శీర్షిక));
	ప్రతిజ్ఞా(ఫలితం);

	ఫలితం = _ringbuffer.push_back(packet, packet_len);
	ప్రతిజ్ఞా(ఫలితం);

	// In case asserts are commented out to prevent unused warnings.
	(శూన్య)ఫలితం;

	తిరిగి నిజం;
}

size_t Variable_నిడివి_Ringbuffer::తీయి_front(uint8_t *buf, size_t buf_max_len)
{
	ఒకవేళ (buf == శూన్యసూచిక) {
		// User needs to supply a valid pointer.
		తిరిగి 0;
	}

	// Check next header
	శీర్షిక_2 శీర్షిక;

	ఒకవేళ (_ringbuffer.తీయి_front(reinterpret_cast<uint8_t *>(&శీర్షిక), పరిమాణం(శీర్షిక)) < పరిమాణం(శీర్షిక)) {
		తిరిగి 0;
	}

	// We can't fit the packet into the user supplied buffer.
	// This should never happen as the user has to supply a big // enough buffer.
	ప్రతిజ్ఞా(static_cast<uint32_t>(శీర్షిక.len) <= buf_max_len);

	size_t బైట్లు_read = _ringbuffer.తీయి_front(buf, శీర్షిక.len);
	ప్రతిజ్ఞా(బైట్లు_read == శీర్షిక.len);

	తిరిగి బైట్లు_read;
}
