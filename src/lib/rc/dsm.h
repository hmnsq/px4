/****************************************************************************
 *
 *	Copyright (c) 2012-2021 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in
 *	the documentation and/or other materials provided with the
 *	distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *	used to endorse or promote products derived from this software
 *	without specific prior written permission.
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
 * @file dsm.h
 *
 * RC protocol definition for Spektrum RC
 *
 * @author Lorenz Meier <lorenz@px4.io>
 */

#pragma once

#ಅನ್ತರ್ಭಾವ <stdint.h>

#ಅನ್ತರ್ಭಾವ <px4_platform_common/px4_config.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>

__BEGIN_DECLS

#define DSM_FRAME_SIZE		16		/**< DSM frame size in bytes */
#define DSM_FRAME_CHANNELS	7		/**< Max supported DSM channels per frame */
#define DSM_MAX_CHANNEL_ಎಣಿಕೆ   20  /**< Max channel count of any DSM RC */
#define DSM_BUFFER_SIZE		(DSM_FRAME_SIZE + DSM_FRAME_SIZE / 2)


#pragma pack(ತಳ್ಳು, 1)
ಪ್ರಕಾರನಿರ್ದೇಶ   uint8_t dsm_frame_t[DSM_BUFFER_SIZE]; /**< DSM dsm frame receive buffer */
ಪ್ರಕಾರನಿರ್ದೇಶ   uint8_t dsm_buf_t[DSM_FRAME_SIZE * 2]; // Define working buffer

ಪ್ರಕಾರನಿರ್ದೇಶ  ಸಂರಚನಾ dsm_decode_t {
	dsm_frame_t frame;
	dsm_buf_t buf;
} dsm_decode_t;

#pragma pack(ತೆಗೆ)

__EXPORT ಪೂರ್ಣಾಙ್ಕ	dsm_init(ಧ್ರುವ ವರ್ಣ *device);
__EXPORT ಶೂನ್ಯ	dsm_deinit(ಶೂನ್ಯ);
__EXPORT ಶೂನ್ಯ	dsm_proto_init(ಶೂನ್ಯ);
__EXPORT ಪೂರ್ಣಾಙ್ಕ	dsm_config(ಪೂರ್ಣಾಙ್ಕ dsm_fd);
__EXPORT bool	dsm_ಒಳಹರಿವು(ಪೂರ್ಣಾಙ್ಕ dsm_fd, uint16_t *ಮೌಲ್ಯಗಳು_2, uint16_t *ಅಂಕೆ_ಮೌಲ್ಯಗಳು, bool *dsm_11_bit, uint8_t *n_bytes,
			  uint8_t **bytes, int8_t *rssi, unsigned *frame_drops, unsigned max_ಮೌಲ್ಯಗಳು);

__EXPORT bool	dsm_parse(ಧ್ರುವ uint64_t now, ಧ್ರುವ uint8_t *frame, ಧ್ರುವ unsigned len, uint16_t *ಮೌಲ್ಯಗಳು_2,
			  uint16_t *ಅಂಕೆ_ಮೌಲ್ಯಗಳು, bool *dsm_11_bit, unsigned *frame_drops, int8_t *rssi_percent, uint16_t max_channels);

#ifdef SPEKTRUM_POWER
__EXPORT ಶೂನ್ಯ	dsm_bind(uint16_t cmd, ಪೂರ್ಣಾಙ್ಕ pulses);
#endif

ಗಣನಾ DSM_CMD {							/* DSM bind states */
	DSM_CMD_BIND_POWER_DOWN = 0,
	DSM_CMD_BIND_POWER_UP,
	DSM_CMD_BIND_SET_RX_OUT,
	DSM_CMD_BIND_SEND_PULSES,
	DSM_CMD_BIND_REINIT_UART
};

#define DSM2_BIND_PULSES 3	/* DSM_BIND_START parameter, pulses required to start pairing DSM2 22ms */
#define DSMX_BIND_PULSES 7	/* DSM_BIND_START parameter, pulses required to start pairing DSMx 22ms */
#define DSMX8_BIND_PULSES 9     /* DSM_BIND_START parameter, pulses required to start pairing DSMx 11ms */

__END_DECLS
