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

#निर्देशः once

#अन्तर्भाव <stdint.h>

#अन्तर्भाव <px4_platform_common/px4_config.h>
#अन्तर्भाव <px4_platform_common/defines.h>

__BEGIN_DECLS

#परिभाषय DSM_FRAME_आकार		16		/**< DSM frame size in bytes */
#परिभाषय DSM_FRAME_CHANNELS	7		/**< Max supported DSM channels per frame */
#परिभाषय DSM_MAX_CHANNEL_गणनम्   20  /**< Max channel count of any DSM RC */
#परिभाषय DSM_बफर_आकार		(DSM_FRAME_आकार + DSM_FRAME_आकार / 2)


#निर्देशः pack(प्रक्षिप, 1)
प्रकारनिर्देश   uint8_t dsm_frame_t[DSM_बफर_आकार]; /**< DSM dsm frame receive buffer */
प्रकारनिर्देश   uint8_t dsm_buf_t[DSM_FRAME_आकार * 2]; // Define working buffer

प्रकारनिर्देश  संरचना dsm_decode_t {
	dsm_frame_t frame;
	dsm_buf_t buf;
} dsm_decode_t;

#निर्देशः pack(निष्कासय)

__EXPORT पूर्णाङ्क	dsm_init(ध्रुव वर्ण *device);
__EXPORT शून्य	dsm_deinit(शून्य);
__EXPORT शून्य	dsm_proto_init(शून्य);
__EXPORT पूर्णाङ्क	dsm_config(पूर्णाङ्क dsm_fd);
__EXPORT bool	dsm_निवेशः(पूर्णाङ्क dsm_fd, uint16_t *मानानि_2, uint16_t *अङ्कः_मानानि, bool *dsm_11_bit, uint8_t *n_अष्टकानि,
			  uint8_t **अष्टकानि, int8_t *rssi, अचिह्नित *frame_drops, अचिह्नित max_मानानि);

__EXPORT bool	dsm_parse(ध्रुव uint64_t now, ध्रुव uint8_t *frame, ध्रुव अचिह्नित len, uint16_t *मानानि_2,
			  uint16_t *अङ्कः_मानानि, bool *dsm_11_bit, अचिह्नित *frame_drops, int8_t *rssi_percent, uint16_t max_channels);

#यदि_परिभाषितम् SPEKTRUM_POWER
__EXPORT शून्य	dsm_bind(uint16_t cmd, पूर्णाङ्क pulses);
#यदि_अन्तः

गणना DSM_CMD {							/* DSM bind states */
	DSM_CMD_BIND_POWER_अधस्तात् = 0,
	DSM_CMD_BIND_POWER_ऊर्ध्वम्,
	DSM_CMD_BIND_समुच्चयः_RX_बहिर्गमनम्,
	DSM_CMD_BIND_SEND_PULSES,
	DSM_CMD_BIND_REINIT_UART
};

#परिभाषय DSM2_BIND_PULSES 3	/* DSM_BIND_START parameter, pulses required to start pairing DSM2 22ms */
#परिभाषय DSMX_BIND_PULSES 7	/* DSM_BIND_START parameter, pulses required to start pairing DSMx 22ms */
#परिभाषय DSMX8_BIND_PULSES 9     /* DSM_BIND_START parameter, pulses required to start pairing DSMx 11ms */

__END_DECLS
