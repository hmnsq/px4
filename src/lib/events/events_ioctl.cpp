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

#अन्तर्भाव <px4_platform/board_ctrl.h>

#अन्तर्भाव <errno.h>

#अन्तर्भाव "events_ioctl.h"

स्थिर पूर्णाङ्क events_ioctl(अचिह्नित पूर्णाङ्क cmd, अचिह्नित दीर्घ arg)
{
	पूर्णाङ्क ret = OK;

	विकल्प (cmd) {
	पक्ष EVENTSIOCSEND: {
			eventiocsend_t *दत्त = (eventiocsend_t *)arg;
			events::send(दत्त->event);
		}
		भङ्ग;

	स्वभाव:
		ret = -ENOTTY;
		भङ्ग;
	}

	प्रत्यागम ret;
}

शून्य events_ioctl_init(शून्य)
{
	px4_पञ्जिकाकोशः_boardct_ioctl(_EVENTSIOCBASE, events_ioctl);
}
