/****************************************************************************
 *
 *   Copyright (c) 2012-2021 PX4 Development Team. All rights reserved.
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
 * @file dsm.cpp
 *
 * Serial protocol decoder for the Spektrum DSM* family of protocols.
 *
 * Decodes into the global PPM buffer and updates accordingly.
 */

#అన్తర్భావ <px4_platform_common/px4_config.h>
#అన్తర్భావ <board_config.h>
#అన్తర్భావ <px4_platform_common/defines.h>
#అన్తర్భావ <px4_platform_common/log.h>

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <math.h>
#అన్తర్భావ <unistd.h>
#అన్తర్భావ <termios.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "dsm.h"
#అన్తర్భావ "spektrum_rssi.h"
#అన్తర్భావ "common_rc.h"
#అన్తర్భావ <drivers/drv_hrt.h>

#అన్తర్భావ <include/containers/Bitset.hpp>

#ఒకవేళ defined(__PX4_NUTTX)
#అన్తర్భావ <nuttx/arch.h>
#నిర్వచించు dsm_udelay(arg)    పైకి_udelay(arg)
#లేకపోతే
#నిర్వచించు dsm_udelay(arg) px4_usleep(arg)
#అయితే_ముగింపు

// #define DSM_DEBUG

స్థిర గణన DSM_DECODE_STATE {
	DSM_DECODE_STATE_DESYNC = 0,
	DSM_DECODE_STATE_SYNC
} dsm_decode_state = DSM_DECODE_STATE_DESYNC;

రకనిర్వచనం నిర్మాణం {
	hrt_abstime చివరి_చూసిన;
	uint16_t value;
} dsm_channel_t;

స్థిర dsm_channel_t channel_బఫరు[DSM_MAX_CHANNEL_లెక్క];

స్థిర పూర్ణాంకం dsm_fd = -1;						/**< File handle to the DSM UART */
స్థిర hrt_abstime dsm_చివరి_rx_సమయం;            /**< Timestamp when we last received data */
స్థిర hrt_abstime dsm_చివరి_చట్రం_సమయం;		/**< Timestamp for start of last valid dsm frame */
స్థిర dsm_చట్రం_t &dsm_చట్రం = rc_decode_buf.dsm.చట్రం;	/**< DSM_BUFFER_SIZE DSM dsm frame receive buffer */
స్థిర dsm_buf_t &dsm_buf = rc_decode_buf.dsm.buf;	/**< DSM_BUFFER_SIZE DSM dsm frame receive buffer */

స్థిర గుర్తులేని dsm_partial_చట్రం_లెక్క;	/**< Count of bytes received for current dsm frame */
స్థిర గుర్తులేని dsm_channel_జరుపు = 0;			/**< Channel resolution, 0=unknown, 10=10 bit (1024), 11=11 bit (2048) */
స్థిర గుర్తులేని dsm_చట్రం_drops = 0;			/**< Count of incomplete DSM frames */
స్థిర uint16_t dsm_chan_లెక్క = 0;         /**< DSM channel count */

స్థిర constexpr uint8_t CHANNEL_వాడని = 0xFF;

/**
 * Attempt to decode a single channel raw channel datum
 *
 * The DSM* protocol doesn't provide any explicit framing,
 * so we detect dsm frame boundaries by the inter-dsm frame delay.
 *
 * The minimum dsm frame spacing is 11ms; with 16 bytes at 115200bps
 * dsm frame transmission time is ~1.4ms.
 *
 * We expect to only be called when bytes arrive for processing,
 * and if an interval of more than 5ms passes between calls,
 * the first byte we read will be the first byte of a dsm frame.
 *
 * In the case where byte(s) are dropped from a dsm frame, this also
 * provides a degree of protection. Of course, it would be better
 * if we didn't drop bytes...
 *
 * Upon receiving a full dsm frame we attempt to decode it
 *
 * @param[in] raw 16 bit raw channel value from dsm frame
 * @param[in] shift position of channel number in raw data
 * @param[out] channel pointer to returned channel number
 * @param[out] value pointer to returned channel value
 * @return true=raw value successfully decoded
 */
స్థిర తార్కిక dsm_decode_channel(uint16_t raw, గుర్తులేని జరుపు, uint8_t &channel, uint16_t &value)
{
	ఒకవేళ (జరుపు == 10) {
		// 1024 Mode: This format is used only by DSM2/22ms mode. All other modes use 2048 data.
		//  Bits 15-10 Channel ID
		//  Bits 9-0   Servo Position
		స్థిర constexpr uint16_t ముసుగు_1024_CHANID = 0xFC00;
		స్థిర constexpr uint16_t ముసుగు_1024_SXPOS = 0x03FF;

		channel = (raw & ముసుగు_1024_CHANID) >> 10; // 6 bits

		స్థిరరాశి uint16_t servo_position = (raw & ముసుగు_1024_SXPOS); // 10 bits

		ఒకవేళ (channel >= DSM_MAX_CHANNEL_లెక్క) {
			PX4_DEBUG("invalid channel: %d\n", channel);
			తిరిగి అబద్ధం;
		}

		// PWM_OUT = (ServoPosition x 1.166μs) + Offset
		స్థిర constexpr uint16_t స్థానభ్రంశం = 903; // microseconds
		value = roundf(servo_position * 1.166f) + స్థానభ్రంశం;

		తిరిగి నిజం;

	} లేకపోతే ఒకవేళ (జరుపు == 11) {
		// 2048 Mode
		//  Bits 15    Servo Phase
		//  Bits 14-11 Channel ID
		//  Bits 10-0  Servo Position

		uint16_t servo_position = 0;

		// from Spektrum Remote Receiver Interfacing Rev G Page 6
		స్థిరరాశి తార్కిక phase = (raw >> 15) & 0x01; // the phase is part of the X-Plus address (bit 15)
		uint8_t chan = (raw >> 11) & 0x0F;

		ఒకవేళ (chan < 12) {
			// Normal channels
			స్థిర constexpr uint16_t ముసుగు_2048_SXPOS = 0x07FF;
			servo_position = (raw & ముసుగు_2048_SXPOS);

		} లేకపోతే ఒకవేళ (chan == 12) {
			// XPlus channels
			chan += ((raw >> 9) & 0x03);

			ఒకవేళ (phase) {
				chan += 4;
			}

			ఒకవేళ (chan >= DSM_MAX_CHANNEL_లెక్క) {
				తిరిగి అబద్ధం;
			}

			servo_position = (raw & 0x01FF) << 2;

			channel = chan;

		} లేకపోతే ఒకవేళ (chan == 0x0F && phase) {
			// When the channel content is all bits set, and the phase is also set, this is the blank channel
			channel = CHANNEL_వాడని;
			తిరిగి నిజం;

		} లేకపోతే {
			// This will be the case for an unused channel (raw 16bit content will be 0xffff)
			PX4_DEBUG("invalid: %d %d\n", chan, phase);
			తిరిగి అబద్ధం;
		}

		channel = chan;

		// PWM_OUT = (ServoPosition x 0.583μs) + Offset
		స్థిర constexpr uint16_t స్థానభ్రంశం = 903; // microseconds
		value = roundf(servo_position * 0.583f) + స్థానభ్రంశం;

		PX4_DEBUG(stderr, "CH%d=%d(0x%02x), ", channel, value, raw);

		తిరిగి నిజం;
	}

	తిరిగి అబద్ధం;
}

/**
 * Attempt to guess if receiving 10 or 11 bit channel values
 *
 * @param[in] reset true=reset the 10/11 bit state to unknown
 */
స్థిర తార్కిక dsm_guess_format(తార్కిక reset)
{
	స్థిర uint32_t cs10 = 0;
	స్థిర uint32_t cs11 = 0;
	స్థిర uint16_t good_cs10_చట్రం_లెక్క = 0;
	స్థిర uint16_t good_cs11_చట్రం_లెక్క = 0;
	స్థిర uint16_t samples = 0;
	స్థిర uint16_t చూసిన_channels_లెక్క_cs11[DSM_MAX_CHANNEL_లెక్క] {};
	స్థిర uint16_t చూసిన_channels_లెక్క_cs10[DSM_MAX_CHANNEL_లెక్క] {};

	/* reset the 10/11 bit sniffed channel masks */
	ఒకవేళ (reset) {
		PX4_DEBUG("dsm_guess_format reset");
		cs10 = 0;
		cs11 = 0;
		samples = 0;

		dsm_channel_జరుపు = 0;
		good_cs10_చట్రం_లెక్క = 0;
		good_cs11_చట్రం_లెక్క = 0;

		కొరకు (గుర్తులేని i = 0; i < DSM_MAX_CHANNEL_లెక్క; i++) {
			చూసిన_channels_లెక్క_cs10[i] = 0;
			చూసిన_channels_లెక్క_cs11[i] = 0;
		}

		తిరిగి అబద్ధం;
	}

	px4::Bitset<DSM_MAX_CHANNEL_లెక్క> channels_దొరికింది_10;
	px4::Bitset<DSM_MAX_CHANNEL_లెక్క> channels_దొరికింది_11;

	తార్కిక cs10_చట్రం_valid = నిజం;
	తార్కిక cs11_చట్రం_valid = నిజం;

	/* scan the channels in the current dsm_frame in both 10- and 11-bit mode */
	కొరకు (గుర్తులేని i = 0; i < DSM_చట్రం_CHANNELS; i++) {

		uint8_t *dp = &dsm_చట్రం[2 + (2 * i)];
		uint16_t raw = (dp[0] << 8) | dp[1];

		uint8_t channel = 0;
		uint16_t value = 0;

		/* if the channel decodes, remember the assigned number */
		ఒకవేళ (dsm_decode_channel(raw, 10, channel, value)) {
			ఒకవేళ (channel == CHANNEL_వాడని) {
				కొనసాగు;
			}

			// invalidate entire frame (for 1024) if channel already found, no duplicate channels per DSM frame
			ఒకవేళ (channels_దొరికింది_10[channel]) {
				cs10_చట్రం_valid = అబద్ధం;

			} లేకపోతే {
				చూసిన_channels_లెక్క_cs10[channel]++;
				channels_దొరికింది_10.సమితి(channel);
			}
		}

		ఒకవేళ (dsm_decode_channel(raw, 11, channel, value)) {
			ఒకవేళ (channel == CHANNEL_వాడని) {
				కొనసాగు;
			}

			// invalidate entire frame (for 2048) if channel already found, no duplicate channels per DSM frame
			ఒకవేళ (channels_దొరికింది_11[channel]) {
				cs11_చట్రం_valid = అబద్ధం;

			} లేకపోతే {
				చూసిన_channels_లెక్క_cs11[channel]++;

				channels_దొరికింది_11.సమితి(channel);
			}
		}
	}

	// add valid cs10 channels
	ఒకవేళ (cs10_చట్రం_valid) {
		good_cs10_చట్రం_లెక్క++;

		కొరకు (గుర్తులేని channel = 0; channel < DSM_MAX_CHANNEL_లెక్క; channel++) {
			ఒకవేళ (channels_దొరికింది_10[channel]) {
				cs10 |= 1 << channel;
			}
		}
	}

	// add valid cs11 channels
	ఒకవేళ (cs11_చట్రం_valid) {
		good_cs11_చట్రం_లెక్క++;

		కొరకు (గుర్తులేని channel = 0; channel < DSM_MAX_CHANNEL_లెక్క; channel++) {
			ఒకవేళ (channels_దొరికింది_11[channel]) {
				cs11 |= 1 << channel;
			}
		}
	}

	samples++;

	/* wait until we have seen plenty of frames */
	ఒకవేళ (samples < 15) {
		తిరిగి అబద్ధం;
	}

	/*
	* 10 or 11 bit decoding guess requirements
	*  For CS10 or CS11...
	*  At least `samples - bad_samples_allowance` must decode correctly (no duplicates, valid channel ranges for CS10)
	*  Even distribution of all found channels
	*  Channels must begin at 0 and no channel gaps after that (ie, the last channel is the one before the first unseen channel starting from zero)
	 */

	తార్కిక cs10_channel_gap_దొరికింది = అబద్ధం;
	తార్కిక cs11_channel_gap_దొరికింది = అబద్ధం;
	తార్కిక valid_channel_counts_cs10 = అబద్ధం;
	తార్కిక valid_channel_counts_cs11 = అబద్ధం;
	uint32_t cs10_channel_లెక్క = 0;
	uint32_t cs11_channel_లెక్క = 0;
	తార్కిక దొరికింది_channels_చివర = అబద్ధం;

	// Count of allowed bad frames
	స్థిర constexpr uint16_t bad_samples_allowance = 5;
	స్థిర constexpr uint16_t minimum_channel_లెక్క = 5;
	స్థిర constexpr uint16_t minimum_channel_చూసిన_లెక్క = 4;

	// Check for continous channels in 10bit decoding
	దొరికింది_channels_చివర = అబద్ధం;

	కొరకు (గుర్తులేని i = 0; i < DSM_MAX_CHANNEL_లెక్క; i++) {
		// This bit is set
		ఒకవేళ (cs10 & (1 << i)) {
			ఒకవేళ (దొరికింది_channels_చివర) {
				// Channel gap found
				cs10_channel_gap_దొరికింది = నిజం;
				ఆపు;
			}

		} లేకపోతే {
			ఒకవేళ (!దొరికింది_channels_చివర) {
				cs10_channel_లెక్క = i;
			}

			దొరికింది_channels_చివర = నిజం;
		}
	}

	// Check for continous channels in 11bit decoding
	దొరికింది_channels_చివర = అబద్ధం;

	కొరకు (గుర్తులేని i = 0; i < DSM_MAX_CHANNEL_లెక్క; i++) {
		// This bit is set
		ఒకవేళ (cs11 & (1 << i)) {
			ఒకవేళ (దొరికింది_channels_చివర) {
				// Channel gap found
				cs11_channel_gap_దొరికింది = నిజం;
				ఆపు;
			}

		} లేకపోతే {
			ఒకవేళ (!దొరికింది_channels_చివర) {
				cs11_channel_లెక్క = i;
			}

			దొరికింది_channels_చివర = నిజం;
		}
	}

	// Check channel seen counts C10
	ఒకవేళ (cs10_channel_లెక్క && !cs10_channel_gap_దొరికింది) {
		valid_channel_counts_cs10 = నిజం;

		కొరకు (గుర్తులేని i = 0; i < ((cs10_channel_లెక్క > 12) ? 12 : cs10_channel_లెక్క); i++) {
			ఒకవేళ (చూసిన_channels_లెక్క_cs10[i] < minimum_channel_చూసిన_లెక్క) {
				valid_channel_counts_cs10 = అబద్ధం;
				ఆపు;
			}

		}
	}

	// Check channel seen counts C11
	ఒకవేళ (cs11_channel_లెక్క && !cs11_channel_gap_దొరికింది) {
		valid_channel_counts_cs11 = నిజం;

		కొరకు (గుర్తులేని i = 0; i < ((cs11_channel_లెక్క > 12) ? 12 : cs11_channel_లెక్క); i++) {
			ఒకవేళ (చూసిన_channels_లెక్క_cs11[i] < minimum_channel_చూసిన_లెక్క) {
				valid_channel_counts_cs11 = అబద్ధం;
				ఆపు;
			}

		}
	}

#నిర్వచితమైతే DSM_DEBUG
	రాయి("DSM guess: CS10 (%li good frames, %i gap found, %li channel count, %i dist)\r\n", good_cs10_చట్రం_లెక్క,
	       cs10_channel_gap_దొరికింది, cs10_channel_లెక్క, valid_channel_counts_cs10);
	రాయి("DSM guess: CS11 (%li good frames, %i gap found, %li channel count, %i dist)\r\n", good_cs11_చట్రం_లెక్క,
	       cs11_channel_gap_దొరికింది, cs11_channel_లెక్క, valid_channel_counts_cs11);
#అయితే_ముగింపు

	ఒకవేళ (good_cs11_చట్రం_లెక్క > samples - bad_samples_allowance && !cs11_channel_gap_దొరికింది
	    && cs11_channel_లెక్క >= minimum_channel_లెక్క && valid_channel_counts_cs11) {
#నిర్వచితమైతే DSM_DEBUG
		రాయి("DSM guess: CS11 guessed!\n");
#అయితే_ముగింపు
		dsm_chan_లెక్క = cs11_channel_లెక్క;
		dsm_channel_జరుపు = 11;
		తిరిగి నిజం;
	}

	ఒకవేళ (good_cs10_చట్రం_లెక్క > samples - bad_samples_allowance && !cs10_channel_gap_దొరికింది
	    && cs10_channel_లెక్క >= minimum_channel_లెక్క && valid_channel_counts_cs10) {
#నిర్వచితమైతే DSM_DEBUG
		రాయి("DSM guess: CS10 guessed!\n");
#అయితే_ముగింపు
		dsm_chan_లెక్క = cs10_channel_లెక్క;
		dsm_channel_జరుపు = 10;
		తిరిగి నిజం;
	}

	/* call ourselves to reset our state ... we have to try again */
#నిర్వచితమైతే DSM_DEBUG
	రాయి("DSM: format detect fail\n");
#అయితే_ముగింపు
	dsm_guess_format(నిజం);
	తిరిగి అబద్ధం;
}

పూర్ణాంకం dsm_config(పూర్ణాంకం fd)
{
#నిర్వచితమైతే SPEKTRUM_POWER_CONFIG
	// Enable power controls for Spektrum receiver
	SPEKTRUM_POWER_CONFIG();
#అయితే_ముగింపు
#నిర్వచితమైతే SPEKTRUM_POWER
	// enable power on DSM connector
	SPEKTRUM_POWER(నిజం);
#అయితే_ముగింపు

	పూర్ణాంకం ret = -1;

	ఒకవేళ (fd >= 0) {

		నిర్మాణం termios t;

		/* 115200bps, no parity, one stop bit */
		tcgetattr(fd, &t);
		cfsetspeed(&t, 115200);
		t.c_cflag &= ~(CSTOPB | PARENB);
		tcsetattr(fd, TCSANOW, &t);

		/* initialise the decoder */
		dsm_partial_చట్రం_లెక్క = 0;
		dsm_చివరి_rx_సమయం = hrt_absolute_సమయం();

		/* reset the format detector */
		dsm_guess_format(నిజం);

		ret = 0;
	}

	తిరిగి ret;
}

శూన్య dsm_proto_init()
{
	dsm_channel_జరుపు = 0;
	dsm_చట్రం_drops = 0;
	dsm_chan_లెక్క = 0;

	dsm_decode_state = DSM_DECODE_STATE_DESYNC;

	కొరకు (గుర్తులేని i = 0; i < DSM_MAX_CHANNEL_లెక్క; i++) {
		channel_బఫరు[i].చివరి_చూసిన = 0;
		channel_బఫరు[i].value = 0;
	}

	/* reset the format detector */
	dsm_guess_format(నిజం);
}

/**
 * Initialize the DSM receive functionality
 *
 * Open the UART for receiving DSM frames and configure it appropriately
 *
 * @param[in] device Device name of DSM UART
 */
పూర్ణాంకం dsm_init(స్థిరరాశి అక్షరం *device)
{
	ఒకవేళ (dsm_fd < 0) {
		dsm_fd = open(device, O_RDWR | O_NONBLOCK);
	}

	dsm_proto_init();

	పూర్ణాంకం ret = dsm_config(dsm_fd);

	ఒకవేళ (!ret) {
		తిరిగి dsm_fd;

	} లేకపోతే {
		తిరిగి -1;
	}
}

శూన్య dsm_deinit()
{
#నిర్వచితమైతే SPEKTRUM_POWER_PASSIVE
	// Turn power controls to passive
	SPEKTRUM_POWER_PASSIVE();
#అయితే_ముగింపు

	ఒకవేళ (dsm_fd >= 0) {
		close(dsm_fd);
	}

	dsm_fd = -1;
}

#ఒకవేళ defined(SPEKTRUM_POWER)
/**
 * Handle DSM satellite receiver bind mode handler
 *
 * @param[in] cmd commands - dsm_bind_power_down, dsm_bind_power_up, dsm_bind_set_rx_out, dsm_bind_send_pulses, dsm_bind_reinit_uart
 * @param[in] pulses Number of pulses for dsm_bind_send_pulses command
 */
శూన్య dsm_bind(uint16_t cmd, పూర్ణాంకం pulses)
{
	ఒకవేళ (dsm_fd < 0) {
		తిరిగి;
	}

	ఎంపిక (cmd) {
	స్థితి DSM_CMD_BIND_POWER_కిందికి:
		// power down DSM satellite
#ఒకవేళ defined(DSM_DEBUG)
		రాయి("DSM: DSM_CMD_BIND_POWER_DOWN\n");
#అయితే_ముగింపు
		SPEKTRUM_POWER(అబద్ధం);
		ఆపు;

	స్థితి DSM_CMD_BIND_POWER_పైకి:
		// power up DSM satellite
#ఒకవేళ defined(DSM_DEBUG)
		రాయి("DSM: DSM_CMD_BIND_POWER_UP\n");
#అయితే_ముగింపు
		SPEKTRUM_POWER(నిజం);
		dsm_guess_format(నిజం);
		ఆపు;

	స్థితి DSM_CMD_BIND_సమితి_RX_బహిర్గతం:
		// Set UART RX pin to active output mode
#ఒకవేళ defined(DSM_DEBUG)
		రాయి("DSM: DSM_CMD_BIND_SET_RX_OUT\n");
#అయితే_ముగింపు
		SPEKTRUM_RX_AS_GPIO_OUTPUT();
		ఆపు;

	స్థితి DSM_CMD_BIND_SEND_PULSES:
		// Pulse RX pin a number of times
#ఒకవేళ defined(DSM_DEBUG)
		రాయి("DSM: DSM_CMD_BIND_SEND_PULSES\n");
#అయితే_ముగింపు

		కొరకు (పూర్ణాంకం i = 0; i < pulses; i++) {
			dsm_udelay(120);
			SPEKTRUM_బహిర్గతం(అబద్ధం);
			dsm_udelay(120);
			SPEKTRUM_బహిర్గతం(నిజం);
		}

		ఆపు;

	స్థితి DSM_CMD_BIND_REINIT_UART:
		// Restore USART RX pin to RS232 receive mode
#ఒకవేళ defined(DSM_DEBUG)
		రాయి("DSM: DSM_CMD_BIND_REINIT_UART\n");
#అయితే_ముగింపు
		SPEKTRUM_RX_AS_UART();
		ఆపు;

	}
}
#అయితే_ముగింపు

/**
 * Decode the entire dsm frame (all contained channels)
 *
 * @param[in] frame_time timestamp when this dsm frame was received. Used to detect RX loss in order to reset 10/11 bit guess.
 * @param[out] num_values pointer to number of raw channel values returned
 * @return true=DSM frame successfully decoded, false=no update
 */
తార్కిక dsm_decode(hrt_abstime చట్రం_సమయం, తార్కిక *dsm_11_బిట్టు, గుర్తులేని channel_లెక్క,
		int8_t *rssi_percent)
{
	// If we haven't seen a new frame recently or haven't guessed the DSM encoding, try to guess
	ఒకవేళ (((చట్రం_సమయం - dsm_చివరి_చట్రం_సమయం) > 1000000) && (dsm_channel_జరుపు != 0)) {
		dsm_guess_format(నిజం);
	}

	/* if we don't know the dsm_frame format, update the guessing state machine */
	ఒకవేళ (dsm_channel_జరుపు == 0) {
		ఒకవేళ (!dsm_guess_format(అబద్ధం)) {
			తిరిగి అబద్ధం;
		}

		// We have our first packet so we're in sync
		dsm_చివరి_rx_సమయం = hrt_absolute_సమయం();
	}

	/*
	 * The first byte represents the rssi in dBm on telemetry receivers with updated
	 * firmware, or fades on others. If the value is less than zero, it's rssi.
	 * We have other ways to detect bad link metrics, so ignore positive values,
	 * but rssi dBm is a useful value.
	 */

	// The SPM4649T with firmware version 1.1RC9 or later will have RSSI in place of fades
	ఒకవేళ (rssi_percent) {
		ఒకవేళ (((int8_t *)dsm_చట్రం)[0] < 0) {
			/*
			 * RSSI is a signed integer between -42dBm and -92dBm
			 * If signal is lost, the value is -128
			 */
			int8_t dbm = (int8_t)dsm_చట్రం[0];

			ఒకవేళ (dbm == -128) {
				*rssi_percent = 0;

			} లేకపోతే {
				*rssi_percent = spek_dbm_గమ్యం_percent(dbm);
			}

		} లేకపోతే {
			/* if we don't know the rssi, anything over 100 will invalidate it */
			*rssi_percent = 127;
		}
	}

	/*
	 * Each channel is a 16-bit unsigned value containing either a 10-
	 * or 11-bit channel value and a 4-bit channel number, shifted
	 * either 10 or 11 bits. The MSB may also be set to indicate the
	 * second dsm_frame in variants of the protocol where more than
	 * seven channels are being transmitted.
	 */

	px4::Bitset<DSM_MAX_CHANNEL_లెక్క> channels_దొరికింది;

	కొరకు (గుర్తులేని i = 0; i < DSM_చట్రం_CHANNELS; i++) {

		uint8_t *dp = &dsm_చట్రం[2 + (2 * i)];
		uint16_t raw = (dp[0] << 8) | dp[1];

		uint8_t channel = 0;
		uint16_t value = 0;

		ఒకవేళ (!dsm_decode_channel(raw, dsm_channel_జరుపు, channel, value)) {
			dsm_చట్రం_drops++;
			తిరిగి అబద్ధం;
		}

		ఒకవేళ (channel == CHANNEL_వాడని) {
			కొనసాగు;
		}

		// abort if channel already found, no duplicate channels per DSM frame
		ఒకవేళ (channels_దొరికింది[channel]) {
			PX4_DEBUG("duplicate channel %d\n\n", channel);
			dsm_guess_format(నిజం);
			dsm_చట్రం_drops++;
			తిరిగి అబద్ధం;

		} లేకపోతే {
			channels_దొరికింది.సమితి(channel);
		}

		/* reset bit guessing state machine if the channel index is out of bounds */
		ఒకవేళ (channel >= DSM_MAX_CHANNEL_లెక్క || channel >= channel_లెక్క) {
			PX4_DEBUG("channel %d > %d (DSM_MAX_CHANNEL_COUNT)", channel, DSM_MAX_CHANNEL_లెక్క);
			dsm_guess_format(నిజం);
			dsm_చట్రం_drops++;
			తిరిగి అబద్ధం;
		}

		/*
		 * Store the decoded channel into the R/C input buffer, taking into
		 * account the different ideas about channel assignement that we have.
		 *
		 * Specifically, the first four channels in rc_channel_data are roll, pitch, thrust, yaw,
		 * but the first four channels from the DSM receiver are thrust, roll, pitch, yaw.
		 */
		ఎంపిక (channel) {
		స్థితి 0:
			channel = 2; // Spektrum Throttle (0) -> 2
			ఆపు;

		స్థితి 1:
			channel = 0; // Spektrum Aileron (1) -> 0
			ఆపు;

		స్థితి 2:
			channel = 1; // Spektrum Elevator (2) -> 1

		అప్రమేయ:
			ఆపు;
		}

		channel_బఫరు[channel].value = value;
		channel_బఫరు[channel].చివరి_చూసిన = చట్రం_సమయం;
	}

	/* Set the 11-bit data indicator */
	*dsm_11_బిట్టు = (dsm_channel_జరుపు == 11);

	/* we have received something we think is a dsm_frame */
	dsm_చివరి_చట్రం_సమయం = చట్రం_సమయం;

#నిర్వచితమైతే DSM_DEBUG
	రాయి("PARSED PACKET\n");
#అయితే_ముగింపు

	తిరిగి నిజం;
}

/**
 * Called periodically to check for input data from the DSM UART
 *
 * The DSM* protocol doesn't provide any explicit framing,
 * so we detect dsm frame boundaries by the inter-dsm frame delay.
 * The minimum dsm frame spacing is 11ms; with 16 bytes at 115200bps
 * dsm frame transmission time is ~1.4ms.
 * We expect to only be called when bytes arrive for processing,
 * and if an interval of more than 5ms passes between calls,
 * the first byte we read will be the first byte of a dsm frame.
 * In the case where byte(s) are dropped from a dsm frame, this also
 * provides a degree of protection. Of course, it would be better
 * if we didn't drop bytes...
 * Upon receiving a full dsm frame we attempt to decode it.
 *
 * @param[out] values pointer to per channel array of decoded values
 * @param[out] num_values pointer to number of raw channel values returned, high order bit 0:10 bit data, 1:11 bit data
 * @param[out] n_butes number of bytes read
 * @param[out] bytes pointer to the buffer of read bytes
 * @param[out] rssi value in percent, if supported, or 127
 * @param[out] frame_drops dropped frames (indication of an unstable link)
 * @param[in] max_values maximum number of channels the receiver can process
 * @return true=decoded raw channel values updated, false=no update
 */
తార్కిక dsm_ఆదానం(పూర్ణాంకం fd, uint16_t *విలువలు_2, uint16_t *అంకె_విలువలు, తార్కిక *dsm_11_బిట్టు, uint8_t *n_బైట్లు, uint8_t **బైట్లు,
	       int8_t *rssi, గుర్తులేని *చట్రం_drops, గుర్తులేని max_విలువలు)
{
	/*
	 * The DSMX/2 protocol doesn't provide reliable framing,
	 * so we detect frame boundaries by the inter-frame delay.
	 *
	 * We expect to only be called when bytes arrive for processing,
	 * and if an interval of more than 3ms passes between calls,
	 * the first byte we read will be the first byte of a frame.
	 *
	 * In the case where byte(s) are dropped from a frame, this also
	 * provides a degree of protection. Of course, it would be better
	 * if we didn't drop bytes...
	 */
	స్థిరరాశి hrt_abstime now = hrt_absolute_సమయం();

	/*
	 * Fetch bytes, but no more than we would need to complete
	 * a complete frame.
	 */

	పూర్ణాంకం ret = పఠ(fd, &dsm_buf[0], పరిమాణం(dsm_buf) / పరిమాణం(dsm_buf[0]));

	/* if the read failed for any reason, just give up here */
	ఒకవేళ (ret < 1) {
		తిరిగి అబద్ధం;

	} లేకపోతే {
		*n_బైట్లు = ret;
		*బైట్లు = &dsm_buf[0];
	}

	/*
	 * Try to decode something with what we got
	 */
	తిరిగి dsm_parse(now, &dsm_buf[0], ret, విలువలు_2, అంకె_విలువలు, dsm_11_బిట్టు, &dsm_చట్రం_drops, rssi, max_విలువలు);
}

తార్కిక dsm_parse(స్థిరరాశి uint64_t now, స్థిరరాశి uint8_t *చట్రం, స్థిరరాశి గుర్తులేని len, uint16_t *విలువలు_2,
	       uint16_t *అంకె_విలువలు, తార్కిక *dsm_11_బిట్టు, గుర్తులేని *చట్రం_drops, int8_t *rssi_percent, uint16_t max_channels)
{
	/* this is set by the decoding state machine and will default to false
	 * once everything that was decodable has been decoded.
	 */
	తార్కిక channel_దత్తాంశం_అందుబాటు = అబద్ధం;
	గుర్తులేని channel_output_లెక్క;

	/* keep decoding until we have consumed the buffer */
	కొరకు (గుర్తులేని d = 0; d < len; d++) {

		/* overflow check */
		ఒకవేళ (dsm_partial_చట్రం_లెక్క == పరిమాణం(dsm_చట్రం) / పరిమాణం(dsm_చట్రం[0])) {
			dsm_partial_చట్రం_లెక్క = 0;
			dsm_decode_state = DSM_DECODE_STATE_DESYNC;
#నిర్వచితమైతే DSM_DEBUG
			రాయి("DSM: RESET (BUF LIM)\n");
#అయితే_ముగింపు
		}

		ఒకవేళ (dsm_partial_చట్రం_లెక్క == DSM_చట్రం_కొలత) {
			dsm_partial_చట్రం_లెక్క = 0;
			dsm_decode_state = DSM_DECODE_STATE_DESYNC;
#నిర్వచితమైతే DSM_DEBUG
			రాయి("DSM: RESET (PACKET LIM)\n");
#అయితే_ముగింపు
		}

#నిర్వచితమైతే DSM_DEBUG
#ఒకవేళ 0
		రాయి("dsm state: %s%s, count: %d, val: %02x\n",
		       (dsm_decode_state == DSM_DECODE_STATE_DESYNC) ? "DSM_DECODE_STATE_DESYNC" : "",
		       (dsm_decode_state == DSM_DECODE_STATE_SYNC) ? "DSM_DECODE_STATE_SYNC" : "",
		       dsm_partial_చట్రం_లెక్క,
		       (గుర్తులేని)చట్రం[d]);
#అయితే_ముగింపు
#అయితే_ముగింపు

		ఎంపిక (dsm_decode_state) {
		స్థితి DSM_DECODE_STATE_DESYNC:

			/* we are de-synced and only interested in the frame marker */
			ఒకవేళ ((now - dsm_చివరి_rx_సమయం) > 5000) {
				dsm_decode_state = DSM_DECODE_STATE_SYNC;
				dsm_partial_చట్రం_లెక్క = 0;
				dsm_చట్రం[dsm_partial_చట్రం_లెక్క++] = చట్రం[d];
			}

			ఆపు;

		స్థితి DSM_DECODE_STATE_SYNC: {
				dsm_చట్రం[dsm_partial_చట్రం_లెక్క++] = చట్రం[d];

				/* decode whatever we got and expect */
				ఒకవేళ (dsm_partial_చట్రం_లెక్క < DSM_చట్రం_కొలత) {
					ఆపు;
				}

				// We've collected a full frame, parse it
				channel_దత్తాంశం_అందుబాటు = dsm_decode(now, dsm_11_బిట్టు, dsm_chan_లెక్క, rssi_percent);

				// Frame consumed, reset the buffer count
				dsm_partial_చట్రం_లెక్క = 0;

				/* if decoding failed, set proto to desync */
				ఒకవేళ (!channel_దత్తాంశం_అందుబాటు) {
					dsm_decode_state = DSM_DECODE_STATE_DESYNC;
				}
			}
			ఆపు;

		అప్రమేయ:
#నిర్వచితమైతే DSM_DEBUG
			రాయి("UNKNOWN PROTO STATE");
#అయితే_ముగింపు
			channel_దత్తాంశం_అందుబాటు = అబద్ధం;
		}
	}

	ఒకవేళ (చట్రం_drops) {
		*చట్రం_drops = dsm_చట్రం_drops;
	}

	ఒకవేళ (channel_దత్తాంశం_అందుబాటు) {
		// Check if any channels have been absent for too long
		కొరకు (గుర్తులేని i = 0; i < dsm_chan_లెక్క; i++) {
			ఒకవేళ (now - channel_బఫరు[i].చివరి_చూసిన > 50 * 1000) {
				తిరిగి అబద్ధం;
			}
		}

		// Overflow protect
		ఒకవేళ (max_channels >= DSM_MAX_CHANNEL_లెక్క) {
			max_channels = DSM_MAX_CHANNEL_లెక్క;
		}

		// Cap returned channels to max_channels if we've seen more
		ఒకవేళ (dsm_chan_లెక్క > max_channels) {
			channel_output_లెక్క = max_channels;

		} లేకపోతే {
			channel_output_లెక్క = dsm_chan_లెక్క;
		}

		*అంకె_విలువలు = channel_output_లెక్క;

		// Copy the full list of collected channels
		కొరకు (గుర్తులేని i = 0; i < channel_output_లెక్క; i++) {
			// Channels which do not have a value yet (because their frame has not been seen yet)
			// will have a zero. If we come across a blank channel, keep storing values, but we
			// won't return that we have a valid frame just yet.
			ఒకవేళ (channel_బఫరు[i].value) {
				విలువలు_2[i] = channel_బఫరు[i].value;

			} లేకపోతే {
				channel_దత్తాంశం_అందుబాటు = అబద్ధం;
			}
		}
	}

#నిర్వచితమైతే DSM_DEBUG

	ఒకవేళ (channel_దత్తాంశం_అందుబాటు) {
		రాయి("frame drops: %u, chan #: %u\n", dsm_చట్రం_drops, dsm_chan_లెక్క);

		కొరకు (గుర్తులేని i = 0; i < dsm_chan_లెక్క; i++) {
			రాయి("dsm_decode: #CH %02u: %u\n", i + 1, విలువలు_2[i]);
		}
	}

#అయితే_ముగింపు

	dsm_చివరి_rx_సమయం = now;

	/* return false as default */
	తిరిగి channel_దత్తాంశం_అందుబాటు;
}
