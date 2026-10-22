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

#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_hrt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/posix.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdarg.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/mavlink_log.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "mavlink_log.h"

__EXPORT ਖਾਲੀ mavlink_vasprintf(ਪੂਰਨ severity, orb_advert_t *mavlink_log_pub, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
	// TODO: add compile check for maxlen

	ਜੇ (!fmt) {
		ਵਾਪਸ;
	}

	ਜੇ (mavlink_log_pub == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ;
	}

	mavlink_log_s log_ਸੁਨੇਹਾ;
	log_ਸੁਨੇਹਾ.severity = severity;
	log_ਸੁਨੇਹਾ.timestamp = hrt_absolute_ਸਮਾਂ();

	va_list ap;
	va_ਸ਼ੁਰੂ(ap, fmt);
	vsnprintf((ਅੱਖਰ *)log_ਸੁਨੇਹਾ.ਪਾਠ, ਆਕਾਰ(log_ਸੁਨੇਹਾ.ਪਾਠ), fmt, ap);
	va_ਸਿਰਾ(ap);

	ਜੇ (*mavlink_log_pub != ਖਾਲੀ_ਸੂਚਕ) {
		orb_publish(ORB_ID(mavlink_log), *mavlink_log_pub, &log_ਸੁਨੇਹਾ);

	} ਨਹੀਂਤਾਂ {
		*mavlink_log_pub = orb_advertise(ORB_ID(mavlink_log), &log_ਸੁਨੇਹਾ);
	}
}
