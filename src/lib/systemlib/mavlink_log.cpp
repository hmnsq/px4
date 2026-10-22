/****************************************************************************
 *
 *   Copyright (c) 2012-2016 PX4 Development Team. All rights reserved.
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
 * @file mavlink_log.cpp
 * MAVLink text logging.
 *
 * @author Lorenz Meier <lorenz@px4.io>
 */

#شامل <drivers/drv_hrt.h>
#شامل <px4_platform_common/posix.h>
#شامل <string.h>
#شامل <stdlib.h>
#شامل <stdio.h>
#شامل <stdarg.h>

#شامل <uORB/topics/mavlink_log.h>
#شامل "mavlink_log.h"

__EXPORT خالی mavlink_vasprintf(صحیح severity, orb_advert_t *mavlink_log_pub, ثابت حرف *fmt, ...)
{
	// TODO: add compile check for maxlen

	اگر (!fmt) {
		واپس;
	}

	اگر (mavlink_log_pub == خالی_اشاریہ) {
		واپس;
	}

	mavlink_log_s log_پیغام;
	log_پیغام.severity = severity;
	log_پیغام.timestamp = hrt_absolute_وقت();

	va_list ap;
	va_آغاز(ap, fmt);
	vsnprintf((حرف *)log_پیغام.متن, حجم(log_پیغام.متن), fmt, ap);
	va_انتہا(ap);

	اگر (*mavlink_log_pub != خالی_اشاریہ) {
		orb_publish(ORB_ID(mavlink_log), *mavlink_log_pub, &log_پیغام);

	} ورنہ {
		*mavlink_log_pub = orb_advertise(ORB_ID(mavlink_log), &log_پیغام);
	}
}
