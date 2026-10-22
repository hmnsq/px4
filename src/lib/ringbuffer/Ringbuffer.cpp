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




#అన్తర్భావ "Ringbuffer.hpp"

#అన్తర్భావ <mathlib/mathlib.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <string.h>


Ringbuffer::~Ringbuffer()
{
	deallocate();
}

తార్కిక Ringbuffer::allocate(size_t బఫరు_కొలత)
{
	ప్రతిజ్ఞా(_ringbuffer == శూన్యసూచిక);

	కొలత_2 = బఫరు_కొలత;
	_ringbuffer = కొత్త uint8_t[కొలత_2];
	తిరిగి _ringbuffer != శూన్యసూచిక;
}

శూన్య Ringbuffer::deallocate()
{
	తొలగించు[] _ringbuffer;
	_ringbuffer = శూన్యసూచిక;
	కొలత_2 = 0;
	ప్రారంభం_3 = 0;
	చివర_2 = 0;
}

size_t Ringbuffer::space_అందుబాటు() స్థిరరాశి
{
	ఒకవేళ (ప్రారంభం_3 > చివర_2) {
		తిరిగి ప్రారంభం_3 - చివర_2 - 1;

	} లేకపోతే {
		తిరిగి ప్రారంభం_3 - చివర_2 - 1 + కొలత_2;
	}
}

size_t Ringbuffer::space_వాడిన() స్థిరరాశి
{
	ఒకవేళ (ప్రారంభం_3 <= చివర_2) {
		తిరిగి చివర_2 - ప్రారంభం_3;

	} లేకపోతే {
		// Potential wrap around.
		తిరిగి చివర_2 - ప్రారంభం_3 + కొలత_2;
	}
}


తార్కిక Ringbuffer::push_back(స్థిరరాశి uint8_t *buf, size_t buf_len)
{
	ఒకవేళ (buf_len == 0 || buf == శూన్యసూచిక) {
		// Nothing to add, we better don't try.
		తిరిగి అబద్ధం;
	}

	ఒకవేళ (ప్రారంభం_3 > చివర_2) {
		// Add after end up to start, no wrap around.

		// Leave one byte free so that start don't end up the same
		// which signals empty.
		స్థిరరాశి size_t అందుబాటు = ప్రారంభం_3 - చివర_2 - 1;

		ఒకవేళ (అందుబాటు < buf_len) {
			తిరిగి అబద్ధం;
		}

		జ్ఞాపకం_నకలు(&_ringbuffer[చివర_2], buf, buf_len);
		చివర_2 += buf_len;

	} లేకపోతే {
		// Add after end, maybe wrap around.
		స్థిరరాశి size_t అందుబాటు = ప్రారంభం_3 - చివర_2 - 1 + కొలత_2;

		ఒకవేళ (అందుబాటు < buf_len) {
			తిరిగి అబద్ధం;
		}

		స్థిరరాశి size_t మిగిలిన_packet_len = కొలత_2 - చివర_2;

		ఒకవేళ (buf_len > మిగిలిన_packet_len) {
			జ్ఞాపకం_నకలు(&_ringbuffer[చివర_2], buf, మిగిలిన_packet_len);
			చివర_2 = 0;

			జ్ఞాపకం_నకలు(&_ringbuffer[చివర_2], buf + మిగిలిన_packet_len, buf_len - మిగిలిన_packet_len);
			చివర_2 += buf_len - మిగిలిన_packet_len;

		} లేకపోతే {
			జ్ఞాపకం_నకలు(&_ringbuffer[చివర_2], buf, buf_len);
			చివర_2 += buf_len;
		}
	}

	తిరిగి నిజం;
}

size_t Ringbuffer::తీయి_front(uint8_t *buf, size_t buf_max_len)
{
	ఒకవేళ (buf == శూన్యసూచిక) {
		// User needs to supply a valid pointer.
		తిరిగి 0;
	}

	ఒకవేళ (ప్రారంభం_3 == చివర_2) {
		// Empty
		తిరిగి 0;
	}

	ఒకవేళ (ప్రారంభం_3 < చివర_2) {

		// No wrap around.
		size_t గమ్యం_copy_len = math::min(చివర_2 - ప్రారంభం_3, buf_max_len);

		జ్ఞాపకం_నకలు(buf, &_ringbuffer[ప్రారంభం_3], గమ్యం_copy_len);
		ప్రారంభం_3 += గమ్యం_copy_len;

		తిరిగి గమ్యం_copy_len;

	} లేకపోతే {
		// Potential wrap around.
		size_t గమ్యం_copy_len = చివర_2 - ప్రారంభం_3 + కొలత_2;

		ఒకవేళ (గమ్యం_copy_len > buf_max_len) {
			గమ్యం_copy_len = buf_max_len;
		}

		స్థిరరాశి size_t మిగిలిన_buf_len = కొలత_2 - ప్రారంభం_3;

		ఒకవేళ (గమ్యం_copy_len > మిగిలిన_buf_len) {

			జ్ఞాపకం_నకలు(buf, &_ringbuffer[ప్రారంభం_3], మిగిలిన_buf_len);
			ప్రారంభం_3 = 0;
			జ్ఞాపకం_నకలు(buf + మిగిలిన_buf_len, &_ringbuffer[ప్రారంభం_3], గమ్యం_copy_len - మిగిలిన_buf_len);
			ప్రారంభం_3 += గమ్యం_copy_len - మిగిలిన_buf_len;

		} లేకపోతే {
			జ్ఞాపకం_నకలు(buf, &_ringbuffer[ప్రారంభం_3], గమ్యం_copy_len);
			ప్రారంభం_3 += గమ్యం_copy_len;
		}

		తిరిగి గమ్యం_copy_len;
	}
}
