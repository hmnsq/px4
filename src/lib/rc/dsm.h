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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/px4_config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>

__BEGIN_DECLS

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DSM_ਚੌਖਟਾ_ਮਾਪ		16		/**< DSM frame size in bytes */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DSM_ਚੌਖਟਾ_CHANNELS	7		/**< Max supported DSM channels per frame */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DSM_MAX_CHANNEL_ਗਿਣਤੀ   20  /**< Max channel count of any DSM RC */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DSM_ਬਫਰ_ਮਾਪ		(DSM_ਚੌਖਟਾ_ਮਾਪ + DSM_ਚੌਖਟਾ_ਮਾਪ / 2)


#ਹਦਾਇਤ pack(ਧੱਕੋ, 1)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ   uint8_t dsm_ਚੌਖਟਾ_t[DSM_ਬਫਰ_ਮਾਪ]; /**< DSM dsm frame receive buffer */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ   uint8_t dsm_buf_t[DSM_ਚੌਖਟਾ_ਮਾਪ * 2]; // Define working buffer

ਕਿਸਮਪਰਿਭਾਸ਼ਾ  ਬਣਤਰ dsm_decode_t {
	dsm_ਚੌਖਟਾ_t ਚੌਖਟਾ;
	dsm_buf_t buf;
} dsm_decode_t;

#ਹਦਾਇਤ pack(ਕੱਢੋ)

__EXPORT ਪੂਰਨ	dsm_init(ਅਚਲ ਅੱਖਰ *device);
__EXPORT ਖਾਲੀ	dsm_deinit(ਖਾਲੀ);
__EXPORT ਖਾਲੀ	dsm_proto_init(ਖਾਲੀ);
__EXPORT ਪੂਰਨ	dsm_config(ਪੂਰਨ dsm_fd);
__EXPORT bool	dsm_ਨਿਵੇਸ਼(ਪੂਰਨ dsm_fd, uint16_t *ਮੁੱਲਾਂ_2, uint16_t *ਅੰਕ_ਮੁੱਲਾਂ, bool *dsm_11_ਦੋਆਧਾਰੀ_ਅੰਕ, uint8_t *n_ਬਾਈਟਾਂ,
			  uint8_t **ਬਾਈਟਾਂ, int8_t *rssi, ਬਿਨਾਨਿਸ਼ਾਨੀ *ਚੌਖਟਾ_drops, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮੁੱਲਾਂ);

__EXPORT bool	dsm_parse(ਅਚਲ uint64_t now, ਅਚਲ uint8_t *ਚੌਖਟਾ, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ len, uint16_t *ਮੁੱਲਾਂ_2,
			  uint16_t *ਅੰਕ_ਮੁੱਲਾਂ, bool *dsm_11_ਦੋਆਧਾਰੀ_ਅੰਕ, ਬਿਨਾਨਿਸ਼ਾਨੀ *ਚੌਖਟਾ_drops, int8_t *rssi_percent, uint16_t max_channels);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SPEKTRUM_POWER
__EXPORT ਖਾਲੀ	dsm_bind(uint16_t cmd, ਪੂਰਨ pulses);
#ਜੇ_ਅੰਤ

ਗਿਣਤੀ DSM_CMD {							/* DSM bind states */
	DSM_CMD_BIND_POWER_ਥੱਲੇ = 0,
	DSM_CMD_BIND_POWER_ਉੱਪਰ,
	DSM_CMD_BIND_ਸੈੱਟ_RX_ਬਾਹਰਲਾ,
	DSM_CMD_BIND_SEND_PULSES,
	DSM_CMD_BIND_REINIT_UART
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DSM2_BIND_PULSES 3	/* DSM_BIND_START parameter, pulses required to start pairing DSM2 22ms */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DSMX_BIND_PULSES 7	/* DSM_BIND_START parameter, pulses required to start pairing DSMx 22ms */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DSMX8_BIND_PULSES 9     /* DSM_BIND_START parameter, pulses required to start pairing DSMx 11ms */

__END_DECLS
