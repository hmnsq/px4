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

#సూచన once

#అన్తర్భావ <stdint.h>

#అన్తర్భావ <px4_platform_common/px4_config.h>
#అన్తర్భావ <px4_platform_common/defines.h>

__BEGIN_DECLS

#నిర్వచించు DSM_చట్రం_కొలత		16		/**< DSM frame size in bytes */
#నిర్వచించు DSM_చట్రం_CHANNELS	7		/**< Max supported DSM channels per frame */
#నిర్వచించు DSM_MAX_CHANNEL_లెక్క   20  /**< Max channel count of any DSM RC */
#నిర్వచించు DSM_బఫరు_కొలత		(DSM_చట్రం_కొలత + DSM_చట్రం_కొలత / 2)


#సూచన pack(నెట్టు, 1)
రకనిర్వచనం   uint8_t dsm_చట్రం_t[DSM_బఫరు_కొలత]; /**< DSM dsm frame receive buffer */
రకనిర్వచనం   uint8_t dsm_buf_t[DSM_చట్రం_కొలత * 2]; // Define working buffer

రకనిర్వచనం  నిర్మాణం dsm_decode_t {
	dsm_చట్రం_t చట్రం;
	dsm_buf_t buf;
} dsm_decode_t;

#సూచన pack(తీయి)

__EXPORT పూర్ణాంకం	dsm_init(స్థిరరాశి అక్షరం *device);
__EXPORT శూన్య	dsm_deinit(శూన్య);
__EXPORT శూన్య	dsm_proto_init(శూన్య);
__EXPORT పూర్ణాంకం	dsm_config(పూర్ణాంకం dsm_fd);
__EXPORT bool	dsm_ఆదానం(పూర్ణాంకం dsm_fd, uint16_t *విలువలు_2, uint16_t *అంకె_విలువలు, bool *dsm_11_బిట్టు, uint8_t *n_బైట్లు,
			  uint8_t **బైట్లు, int8_t *rssi, గుర్తులేని *చట్రం_drops, గుర్తులేని max_విలువలు);

__EXPORT bool	dsm_parse(స్థిరరాశి uint64_t now, స్థిరరాశి uint8_t *చట్రం, స్థిరరాశి గుర్తులేని len, uint16_t *విలువలు_2,
			  uint16_t *అంకె_విలువలు, bool *dsm_11_బిట్టు, గుర్తులేని *చట్రం_drops, int8_t *rssi_percent, uint16_t max_channels);

#నిర్వచితమైతే SPEKTRUM_POWER
__EXPORT శూన్య	dsm_bind(uint16_t cmd, పూర్ణాంకం pulses);
#అయితే_ముగింపు

గణన DSM_CMD {							/* DSM bind states */
	DSM_CMD_BIND_POWER_కిందికి = 0,
	DSM_CMD_BIND_POWER_పైకి,
	DSM_CMD_BIND_సమితి_RX_బహిర్గతం,
	DSM_CMD_BIND_SEND_PULSES,
	DSM_CMD_BIND_REINIT_UART
};

#నిర్వచించు DSM2_BIND_PULSES 3	/* DSM_BIND_START parameter, pulses required to start pairing DSM2 22ms */
#నిర్వచించు DSMX_BIND_PULSES 7	/* DSM_BIND_START parameter, pulses required to start pairing DSMx 22ms */
#నిర్వచించు DSMX8_BIND_PULSES 9     /* DSM_BIND_START parameter, pulses required to start pairing DSMx 11ms */

__END_DECLS
