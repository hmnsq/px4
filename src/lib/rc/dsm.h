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

#هدايت once

#ضمن <stdint.h>

#ضمن <px4_platform_common/px4_config.h>
#ضمن <px4_platform_common/defines.h>

__BEGIN_DECLS

#وصف_ڪريو DSM_چوکٺو_سائيز		16		/**< DSM frame size in bytes */
#وصف_ڪريو DSM_چوکٺو_CHANNELS	7		/**< Max supported DSM channels per frame */
#وصف_ڪريو DSM_MAX_CHANNEL_ڳڻپ   20  /**< Max channel count of any DSM RC */
#وصف_ڪريو DSM_بفر_سائيز		(DSM_چوکٺو_سائيز + DSM_چوکٺو_سائيز / 2)


#هدايت pack(ڌڪو, 1)
تعريفنوع   uint8_t dsm_چوکٺو_t[DSM_بفر_سائيز]; /**< DSM dsm frame receive buffer */
تعريفنوع   uint8_t dsm_buf_t[DSM_چوکٺو_سائيز * 2]; // Define working buffer

تعريفنوع  بنية dsm_decode_t {
	dsm_چوکٺو_t چوکٺو;
	dsm_buf_t buf;
} dsm_decode_t;

#هدايت pack(ڪڍو)

__EXPORT صحيح	dsm_init(ثابت حرف *device);
__EXPORT فراغ	dsm_deinit(فراغ);
__EXPORT فراغ	dsm_proto_init(فراغ);
__EXPORT صحيح	dsm_config(صحيح dsm_fd);
__EXPORT bool	dsm_داخلا(صحيح dsm_fd, uint16_t *قدرون_2, uint16_t *انگ_قدرون, bool *dsm_11_ٻٽو_انگ, uint8_t *n_بائيٽون,
			  uint8_t **بائيٽون, int8_t *rssi, بنانشان *چوکٺو_drops, بنانشان max_قدرون);

__EXPORT bool	dsm_parse(ثابت uint64_t now, ثابت uint8_t *چوکٺو, ثابت بنانشان len, uint16_t *قدرون_2,
			  uint16_t *انگ_قدرون, bool *dsm_11_ٻٽو_انگ, بنانشان *چوکٺو_drops, int8_t *rssi_percent, uint16_t max_channels);

#جيڪڏهن_وصف_ٿيل SPEKTRUM_POWER
__EXPORT فراغ	dsm_bind(uint16_t cmd, صحيح pulses);
#جيڪڏهن_پورو

تعداد DSM_CMD {							/* DSM bind states */
	DSM_CMD_BIND_POWER_هيٺ_ڏانهن = 0,
	DSM_CMD_BIND_POWER_مٿي,
	DSM_CMD_BIND_سيٽ_RX_ٻاهريون,
	DSM_CMD_BIND_SEND_PULSES,
	DSM_CMD_BIND_REINIT_UART
};

#وصف_ڪريو DSM2_BIND_PULSES 3	/* DSM_BIND_START parameter, pulses required to start pairing DSM2 22ms */
#وصف_ڪريو DSMX_BIND_PULSES 7	/* DSM_BIND_START parameter, pulses required to start pairing DSMx 22ms */
#وصف_ڪريو DSMX8_BIND_PULSES 9     /* DSM_BIND_START parameter, pulses required to start pairing DSMx 11ms */

__END_DECLS
