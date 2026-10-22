/****************************************************************************
 *
 *   Copyright (c) 2023 Technology Innovation Institute. All rights reserved.
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

/**
 * @file events_ioctl.cpp
 *
 * Interface to send events from user space.
 */

#அந்தர்பாவ <px4_platform/board_ctrl.h>

#அந்தர்பாவ <errno.h>

#அந்தர்பாவ "events_ioctl.h"

நிலையான முழு events_ioctl(unsigned முழு cmd, unsigned long arg)
{
	முழு ret = OK;

	தேர்வு (cmd) {
	நிலை EVENTSIOCSEND: {
			eventiocsend_t *data = (eventiocsend_t *)arg;
			events::send(data->event);
		}
		நிறுத்து;

	இயல்பு:
		ret = -ENOTTY;
		நிறுத்து;
	}

	திரும்பு ret;
}

வெறுமை events_ioctl_init(வெறுமை)
{
	px4_register_boardct_ioctl(_EVENTSIOCBASE, events_ioctl);
}
