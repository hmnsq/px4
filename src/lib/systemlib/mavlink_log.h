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
 * @file mavlink_log.h
 * MAVLink text logging.
 *
 * @author Lorenz Meier <lorenz@px4.io>
 * @author Julian Oes <julian@oes.ch>
 */

#సూచన once

#అన్తర్భావ <px4_log.h>
#అన్తర్భావ <uORB/uORB.h>

/**
 * The maximum string length supported.
 */
#నిర్వచించు MAVLINK_LOG_MAXLEN			50

#నిర్వచితమైతే __cplusplus
బాహ్య "C" {
#అయితే_ముగింపు
__EXPORT శూన్య mavlink_vasprintf(పూర్ణాంకం severity, orb_advert_t *mavlink_log_pub, స్థిరరాశి అక్షరం *fmt, ...);
#నిర్వచితమైతే __cplusplus
}
#అయితే_ముగింపు

#నిర్వచించు సందేశం_PRIO_DEBUG		7
#నిర్వచించు సందేశం_PRIO_INFO		6
#నిర్వచించు సందేశం_PRIO_NOTICE	5
#నిర్వచించు సందేశం_PRIO_WARNING	4
#నిర్వచించు సందేశం_PRIO_ERROR		3
#నిర్వచించు సందేశం_PRIO_CRITICAL	2
#నిర్వచించు సందేశం_PRIO_ALERT		1
#నిర్వచించు సందేశం_PRIO_EMERGENCY	0

/*
 * The va_args implementation here is not beautiful, but obviously we run into the same issues
 * the GCC devs saw, and are using their solution:
 *
 * http://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
 */

/**
 * Send a mavlink info message (also printed to console).
 *
 * @param _pub		Pointer to the uORB advert;
 * @param _text		The text to log;
 */
#నిర్వచించు mavlink_log_info(_pub, పాఠ్యం_2, ...)			\
	చేయి { \
		mavlink_vasprintf(సందేశం_PRIO_INFO, _pub, పాఠ్యం_2, ##__VA_ARGS__); \
		PX4_INFO(పాఠ్యం_2, ##__VA_ARGS__); \
	} అయితే(0);

/**
 * Send a mavlink warning message and print to console.
 *
 * @param _pub		Pointer to the uORB advert;
 * @param _text		The text to log;
 */
#నిర్వచించు mavlink_log_warning(_pub, పాఠ్యం_2, ...) \
	చేయి { \
		mavlink_vasprintf(సందేశం_PRIO_WARNING, _pub, పాఠ్యం_2, ##__VA_ARGS__); \
		PX4_WARN(పాఠ్యం_2, ##__VA_ARGS__); \
	} అయితే(0);

/**
 * Send a mavlink emergency message and print to console.
 *
 * @param _pub		Pointer to the uORB advert;
 * @param _text		The text to log;
 */
#నిర్వచించు mavlink_log_emergency(_pub, పాఠ్యం_2, ...) \
	చేయి { \
		mavlink_vasprintf(సందేశం_PRIO_EMERGENCY, _pub, పాఠ్యం_2, ##__VA_ARGS__); \
		PX4_ERR(పాఠ్యం_2, ##__VA_ARGS__); \
	} అయితే(0);

/**
 * Send a mavlink critical message and print to console.
 *
 * @param _pub		Pointer to the uORB advert;
 * @param _text		The text to log;
 */
#నిర్వచించు mavlink_log_critical(_pub, పాఠ్యం_2, ...) \
	చేయి { \
		mavlink_vasprintf(సందేశం_PRIO_CRITICAL, _pub, పాఠ్యం_2, ##__VA_ARGS__); \
		PX4_WARN(పాఠ్యం_2, ##__VA_ARGS__); \
	} అయితే(0);
