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

#ہدایت once

#ضمن <stdint.h>

#ضمن <px4_platform_common/px4_config.h>
#ضمن <px4_platform_common/defines.h>

__BEGIN_DECLS

#تعریف_کرو DSM_چوکھٹا_ناپ		16		/**< DSM frame size in bytes */
#تعریف_کرو DSM_چوکھٹا_CHANNELS	7		/**< Max supported DSM channels per frame */
#تعریف_کرو DSM_MAX_CHANNEL_گنتی   20  /**< Max channel count of any DSM RC */
#تعریف_کرو DSM_بفر_ناپ		(DSM_چوکھٹا_ناپ + DSM_چوکھٹا_ناپ / 2)


#ہدایت pack(ڈالو, 1)
تعريفنوع   uint8_t dsm_چوکھٹا_t[DSM_بفر_ناپ]; /**< DSM dsm frame receive buffer */
تعريفنوع   uint8_t dsm_buf_t[DSM_چوکھٹا_ناپ * 2]; // Define working buffer

تعريفنوع  بنية dsm_decode_t {
	dsm_چوکھٹا_t چوکھٹا;
	dsm_buf_t buf;
} dsm_decode_t;

#ہدایت pack(نکالو)

__EXPORT صحيح	dsm_init(ثابت حرف *device);
__EXPORT فراغ	dsm_deinit(فراغ);
__EXPORT فراغ	dsm_proto_init(فراغ);
__EXPORT صحيح	dsm_config(صحيح dsm_fd);
__EXPORT bool	dsm_داخلہ(صحيح dsm_fd, uint16_t *اقدار_2, uint16_t *عدد_اقدار, bool *dsm_11_ثنائی_ہندسہ, uint8_t *n_بائٹاں,
			  uint8_t **بائٹاں, int8_t *rssi, بنانشانی *چوکھٹا_drops, بنانشانی max_اقدار);

__EXPORT bool	dsm_parse(ثابت uint64_t now, ثابت uint8_t *چوکھٹا, ثابت بنانشانی len, uint16_t *اقدار_2,
			  uint16_t *عدد_اقدار, bool *dsm_11_ثنائی_ہندسہ, بنانشانی *چوکھٹا_drops, int8_t *rssi_percent, uint16_t max_channels);

#جے_تعریف_شدہ SPEKTRUM_POWER
__EXPORT فراغ	dsm_bind(uint16_t cmd, صحيح pulses);
#جے_انت

تعداد DSM_CMD {							/* DSM bind states */
	DSM_CMD_BIND_POWER_تھلے = 0,
	DSM_CMD_BIND_POWER_اتے,
	DSM_CMD_BIND_سیٹ_RX_باہرلا,
	DSM_CMD_BIND_SEND_PULSES,
	DSM_CMD_BIND_REINIT_UART
};

#تعریف_کرو DSM2_BIND_PULSES 3	/* DSM_BIND_START parameter, pulses required to start pairing DSM2 22ms */
#تعریف_کرو DSMX_BIND_PULSES 7	/* DSM_BIND_START parameter, pulses required to start pairing DSMx 22ms */
#تعریف_کرو DSMX8_BIND_PULSES 9     /* DSM_BIND_START parameter, pulses required to start pairing DSMx 11ms */

__END_DECLS
