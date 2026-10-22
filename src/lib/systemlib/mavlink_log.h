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

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <px4_log.h>
#ಅನ್ತರ್ಭಾವ <uORB/uORB.h>

/**
 * The maximum string length supported.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು MAVLINK_LOG_MAXLEN			50

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
ಬಾಹ್ಯ "C" {
#ಆದರೆ_ಅಂತ್ಯ
__EXPORT ಶೂನ್ಯ mavlink_vasprintf(ಪೂರ್ಣಾಂಕ severity, orb_advert_t *mavlink_log_pub, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂದೇಶ_PRIO_DEBUG		7
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂದೇಶ_PRIO_INFO		6
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂದೇಶ_PRIO_NOTICE	5
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂದೇಶ_PRIO_WARNING	4
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂದೇಶ_PRIO_ERROR		3
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂದೇಶ_PRIO_CRITICAL	2
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂದೇಶ_PRIO_ALERT		1
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂದೇಶ_PRIO_EMERGENCY	0

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
#ವ್ಯಾಖ್ಯಾನಿಸು mavlink_log_info(_pub, ಪಠ್ಯ_2, ...)			\
	ಮಾಡು { \
		mavlink_vasprintf(ಸಂದೇಶ_PRIO_INFO, _pub, ಪಠ್ಯ_2, ##__VA_ARGS__); \
		PX4_INFO(ಪಠ್ಯ_2, ##__VA_ARGS__); \
	} ಆಗ(0);

/**
 * Send a mavlink warning message and print to console.
 *
 * @param _pub		Pointer to the uORB advert;
 * @param _text		The text to log;
 */
#ವ್ಯಾಖ್ಯಾನಿಸು mavlink_log_warning(_pub, ಪಠ್ಯ_2, ...) \
	ಮಾಡು { \
		mavlink_vasprintf(ಸಂದೇಶ_PRIO_WARNING, _pub, ಪಠ್ಯ_2, ##__VA_ARGS__); \
		PX4_WARN(ಪಠ್ಯ_2, ##__VA_ARGS__); \
	} ಆಗ(0);

/**
 * Send a mavlink emergency message and print to console.
 *
 * @param _pub		Pointer to the uORB advert;
 * @param _text		The text to log;
 */
#ವ್ಯಾಖ್ಯಾನಿಸು mavlink_log_emergency(_pub, ಪಠ್ಯ_2, ...) \
	ಮಾಡು { \
		mavlink_vasprintf(ಸಂದೇಶ_PRIO_EMERGENCY, _pub, ಪಠ್ಯ_2, ##__VA_ARGS__); \
		PX4_ERR(ಪಠ್ಯ_2, ##__VA_ARGS__); \
	} ಆಗ(0);

/**
 * Send a mavlink critical message and print to console.
 *
 * @param _pub		Pointer to the uORB advert;
 * @param _text		The text to log;
 */
#ವ್ಯಾಖ್ಯಾನಿಸು mavlink_log_critical(_pub, ಪಠ್ಯ_2, ...) \
	ಮಾಡು { \
		mavlink_vasprintf(ಸಂದೇಶ_PRIO_CRITICAL, _pub, ಪಠ್ಯ_2, ##__VA_ARGS__); \
		PX4_WARN(ಪಠ್ಯ_2, ##__VA_ARGS__); \
	} ಆಗ(0);
