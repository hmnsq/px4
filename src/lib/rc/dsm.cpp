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

#ଅନ୍ତର୍ଭାଵ <px4_platform_common/px4_config.h>
#ଅନ୍ତର୍ଭାଵ <board_config.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/defines.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/log.h>

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <math.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଅନ୍ତର୍ଭାଵ <termios.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "dsm.h"
#ଅନ୍ତର୍ଭାଵ "spektrum_rssi.h"
#ଅନ୍ତର୍ଭାଵ "common_rc.h"
#ଅନ୍ତର୍ଭାଵ <drivers/drv_hrt.h>

#ଅନ୍ତର୍ଭାଵ <include/containers/Bitset.hpp>

#ଯଦି defined(__PX4_NUTTX)
#ଅନ୍ତର୍ଭାଵ <nuttx/arch.h>
#define dsm_udelay(arg)    up_udelay(arg)
#ଅନ୍ୟଥା
#define dsm_udelay(arg) px4_usleep(arg)
#endif

// #define DSM_DEBUG

ସ୍ଥିର ଗଣନା DSM_DECODE_STATE {
	DSM_DECODE_STATE_DESYNC = 0,
	DSM_DECODE_STATE_SYNC
} dsm_decode_state = DSM_DECODE_STATE_DESYNC;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
	hrt_abstime last_seen;
	uint16_t value;
} dsm_channel_t;

ସ୍ଥିର dsm_channel_t channel_buffer[DSM_MAX_CHANNEL_ଗଣନା];

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା dsm_fd = -1;						/**< File handle to the DSM UART */
ସ୍ଥିର hrt_abstime dsm_last_rx_time;            /**< Timestamp when we last received data */
ସ୍ଥିର hrt_abstime dsm_last_frame_time;		/**< Timestamp for start of last valid dsm frame */
ସ୍ଥିର dsm_frame_t &dsm_frame = rc_decode_buf.dsm.frame;	/**< DSM_BUFFER_SIZE DSM dsm frame receive buffer */
ସ୍ଥିର dsm_buf_t &dsm_buf = rc_decode_buf.dsm.buf;	/**< DSM_BUFFER_SIZE DSM dsm frame receive buffer */

ସ୍ଥିର unsigned dsm_partial_frame_ଗଣନା;	/**< Count of bytes received for current dsm frame */
ସ୍ଥିର unsigned dsm_channel_shift = 0;			/**< Channel resolution, 0=unknown, 10=10 bit (1024), 11=11 bit (2048) */
ସ୍ଥିର unsigned dsm_frame_drops = 0;			/**< Count of incomplete DSM frames */
ସ୍ଥିର uint16_t dsm_chan_ଗଣନା = 0;         /**< DSM channel count */

ସ୍ଥିର constexpr uint8_t CHANNEL_UNUSED = 0xFF;

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
ସ୍ଥିର ତାର୍କିକ dsm_decode_channel(uint16_t raw, unsigned shift, uint8_t &channel, uint16_t &value)
{
	ଯଦି (shift == 10) {
		// 1024 Mode: This format is used only by DSM2/22ms mode. All other modes use 2048 data.
		//  Bits 15-10 Channel ID
		//  Bits 9-0   Servo Position
		ସ୍ଥିର constexpr uint16_t MASK_1024_CHANID = 0xFC00;
		ସ୍ଥିର constexpr uint16_t MASK_1024_SXPOS = 0x03FF;

		channel = (raw & MASK_1024_CHANID) >> 10; // 6 bits

		ଅଚଳ uint16_t servo_position = (raw & MASK_1024_SXPOS); // 10 bits

		ଯଦି (channel >= DSM_MAX_CHANNEL_ଗଣନା) {
			PX4_DEBUG("invalid channel: %d\n", channel);
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		}

		// PWM_OUT = (ServoPosition x 1.166μs) + Offset
		ସ୍ଥିର constexpr uint16_t offset = 903; // microseconds
		value = roundf(servo_position * 1.166f) + offset;

		ଫେରନ୍ତୁ ସତ୍ୟ;

	} ଅନ୍ୟଥା ଯଦି (shift == 11) {
		// 2048 Mode
		//  Bits 15    Servo Phase
		//  Bits 14-11 Channel ID
		//  Bits 10-0  Servo Position

		uint16_t servo_position = 0;

		// from Spektrum Remote Receiver Interfacing Rev G Page 6
		ଅଚଳ ତାର୍କିକ phase = (raw >> 15) & 0x01; // the phase is part of the X-Plus address (bit 15)
		uint8_t chan = (raw >> 11) & 0x0F;

		ଯଦି (chan < 12) {
			// Normal channels
			ସ୍ଥିର constexpr uint16_t MASK_2048_SXPOS = 0x07FF;
			servo_position = (raw & MASK_2048_SXPOS);

		} ଅନ୍ୟଥା ଯଦି (chan == 12) {
			// XPlus channels
			chan += ((raw >> 9) & 0x03);

			ଯଦି (phase) {
				chan += 4;
			}

			ଯଦି (chan >= DSM_MAX_CHANNEL_ଗଣନା) {
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
			}

			servo_position = (raw & 0x01FF) << 2;

			channel = chan;

		} ଅନ୍ୟଥା ଯଦି (chan == 0x0F && phase) {
			// When the channel content is all bits set, and the phase is also set, this is the blank channel
			channel = CHANNEL_UNUSED;
			ଫେରନ୍ତୁ ସତ୍ୟ;

		} ଅନ୍ୟଥା {
			// This will be the case for an unused channel (raw 16bit content will be 0xffff)
			PX4_DEBUG("invalid: %d %d\n", chan, phase);
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		}

		channel = chan;

		// PWM_OUT = (ServoPosition x 0.583μs) + Offset
		ସ୍ଥିର constexpr uint16_t offset = 903; // microseconds
		value = roundf(servo_position * 0.583f) + offset;

		PX4_DEBUG(stderr, "CH%d=%d(0x%02x), ", channel, value, raw);

		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

/**
 * Attempt to guess if receiving 10 or 11 bit channel values
 *
 * @param[in] reset true=reset the 10/11 bit state to unknown
 */
ସ୍ଥିର ତାର୍କିକ dsm_guess_format(ତାର୍କିକ reset)
{
	ସ୍ଥିର uint32_t cs10 = 0;
	ସ୍ଥିର uint32_t cs11 = 0;
	ସ୍ଥିର uint16_t good_cs10_frame_ଗଣନା = 0;
	ସ୍ଥିର uint16_t good_cs11_frame_ଗଣନା = 0;
	ସ୍ଥିର uint16_t samples = 0;
	ସ୍ଥିର uint16_t seen_channels_ଗଣନା_cs11[DSM_MAX_CHANNEL_ଗଣନା] {};
	ସ୍ଥିର uint16_t seen_channels_ଗଣନା_cs10[DSM_MAX_CHANNEL_ଗଣନା] {};

	/* reset the 10/11 bit sniffed channel masks */
	ଯଦି (reset) {
		PX4_DEBUG("dsm_guess_format reset");
		cs10 = 0;
		cs11 = 0;
		samples = 0;

		dsm_channel_shift = 0;
		good_cs10_frame_ଗଣନା = 0;
		good_cs11_frame_ଗଣନା = 0;

		ପାଇଁ (unsigned i = 0; i < DSM_MAX_CHANNEL_ଗଣନା; i++) {
			seen_channels_ଗଣନା_cs10[i] = 0;
			seen_channels_ଗଣନା_cs11[i] = 0;
		}

		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	px4::Bitset<DSM_MAX_CHANNEL_ଗଣନା> channels_found_10;
	px4::Bitset<DSM_MAX_CHANNEL_ଗଣନା> channels_found_11;

	ତାର୍କିକ cs10_frame_valid = ସତ୍ୟ;
	ତାର୍କିକ cs11_frame_valid = ସତ୍ୟ;

	/* scan the channels in the current dsm_frame in both 10- and 11-bit mode */
	ପାଇଁ (unsigned i = 0; i < DSM_FRAME_CHANNELS; i++) {

		uint8_t *dp = &dsm_frame[2 + (2 * i)];
		uint16_t raw = (dp[0] << 8) | dp[1];

		uint8_t channel = 0;
		uint16_t value = 0;

		/* if the channel decodes, remember the assigned number */
		ଯଦି (dsm_decode_channel(raw, 10, channel, value)) {
			ଯଦି (channel == CHANNEL_UNUSED) {
				ଜାରି;
			}

			// invalidate entire frame (for 1024) if channel already found, no duplicate channels per DSM frame
			ଯଦି (channels_found_10[channel]) {
				cs10_frame_valid = ମିଥ୍ୟା;

			} ଅନ୍ୟଥା {
				seen_channels_ଗଣନା_cs10[channel]++;
				channels_found_10.set(channel);
			}
		}

		ଯଦି (dsm_decode_channel(raw, 11, channel, value)) {
			ଯଦି (channel == CHANNEL_UNUSED) {
				ଜାରି;
			}

			// invalidate entire frame (for 2048) if channel already found, no duplicate channels per DSM frame
			ଯଦି (channels_found_11[channel]) {
				cs11_frame_valid = ମିଥ୍ୟା;

			} ଅନ୍ୟଥା {
				seen_channels_ଗଣନା_cs11[channel]++;

				channels_found_11.set(channel);
			}
		}
	}

	// add valid cs10 channels
	ଯଦି (cs10_frame_valid) {
		good_cs10_frame_ଗଣନା++;

		ପାଇଁ (unsigned channel = 0; channel < DSM_MAX_CHANNEL_ଗଣନା; channel++) {
			ଯଦି (channels_found_10[channel]) {
				cs10 |= 1 << channel;
			}
		}
	}

	// add valid cs11 channels
	ଯଦି (cs11_frame_valid) {
		good_cs11_frame_ଗଣନା++;

		ପାଇଁ (unsigned channel = 0; channel < DSM_MAX_CHANNEL_ଗଣନା; channel++) {
			ଯଦି (channels_found_11[channel]) {
				cs11 |= 1 << channel;
			}
		}
	}

	samples++;

	/* wait until we have seen plenty of frames */
	ଯଦି (samples < 15) {
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	/*
	* 10 or 11 bit decoding guess requirements
	*  For CS10 or CS11...
	*  At least `samples - bad_samples_allowance` must decode correctly (no duplicates, valid channel ranges for CS10)
	*  Even distribution of all found channels
	*  Channels must begin at 0 and no channel gaps after that (ie, the last channel is the one before the first unseen channel starting from zero)
	 */

	ତାର୍କିକ cs10_channel_gap_found = ମିଥ୍ୟା;
	ତାର୍କିକ cs11_channel_gap_found = ମିଥ୍ୟା;
	ତାର୍କିକ valid_channel_counts_cs10 = ମିଥ୍ୟା;
	ତାର୍କିକ valid_channel_counts_cs11 = ମିଥ୍ୟା;
	uint32_t cs10_channel_ଗଣନା = 0;
	uint32_t cs11_channel_ଗଣନା = 0;
	ତାର୍କିକ found_channels_end = ମିଥ୍ୟା;

	// Count of allowed bad frames
	ସ୍ଥିର constexpr uint16_t bad_samples_allowance = 5;
	ସ୍ଥିର constexpr uint16_t minimum_channel_ଗଣନା = 5;
	ସ୍ଥିର constexpr uint16_t minimum_channel_seen_ଗଣନା = 4;

	// Check for continous channels in 10bit decoding
	found_channels_end = ମିଥ୍ୟା;

	ପାଇଁ (unsigned i = 0; i < DSM_MAX_CHANNEL_ଗଣନା; i++) {
		// This bit is set
		ଯଦି (cs10 & (1 << i)) {
			ଯଦି (found_channels_end) {
				// Channel gap found
				cs10_channel_gap_found = ସତ୍ୟ;
				ଭାଙ୍ଗ;
			}

		} ଅନ୍ୟଥା {
			ଯଦି (!found_channels_end) {
				cs10_channel_ଗଣନା = i;
			}

			found_channels_end = ସତ୍ୟ;
		}
	}

	// Check for continous channels in 11bit decoding
	found_channels_end = ମିଥ୍ୟା;

	ପାଇଁ (unsigned i = 0; i < DSM_MAX_CHANNEL_ଗଣନା; i++) {
		// This bit is set
		ଯଦି (cs11 & (1 << i)) {
			ଯଦି (found_channels_end) {
				// Channel gap found
				cs11_channel_gap_found = ସତ୍ୟ;
				ଭାଙ୍ଗ;
			}

		} ଅନ୍ୟଥା {
			ଯଦି (!found_channels_end) {
				cs11_channel_ଗଣନା = i;
			}

			found_channels_end = ସତ୍ୟ;
		}
	}

	// Check channel seen counts C10
	ଯଦି (cs10_channel_ଗଣନା && !cs10_channel_gap_found) {
		valid_channel_counts_cs10 = ସତ୍ୟ;

		ପାଇଁ (unsigned i = 0; i < ((cs10_channel_ଗଣନା > 12) ? 12 : cs10_channel_ଗଣନା); i++) {
			ଯଦି (seen_channels_ଗଣନା_cs10[i] < minimum_channel_seen_ଗଣନା) {
				valid_channel_counts_cs10 = ମିଥ୍ୟା;
				ଭାଙ୍ଗ;
			}

		}
	}

	// Check channel seen counts C11
	ଯଦି (cs11_channel_ଗଣନା && !cs11_channel_gap_found) {
		valid_channel_counts_cs11 = ସତ୍ୟ;

		ପାଇଁ (unsigned i = 0; i < ((cs11_channel_ଗଣନା > 12) ? 12 : cs11_channel_ଗଣନା); i++) {
			ଯଦି (seen_channels_ଗଣନା_cs11[i] < minimum_channel_seen_ଗଣନା) {
				valid_channel_counts_cs11 = ମିଥ୍ୟା;
				ଭାଙ୍ଗ;
			}

		}
	}

#ifdef DSM_DEBUG
	printf("DSM guess: CS10 (%li good frames, %i gap found, %li channel count, %i dist)\r\n", good_cs10_frame_ଗଣନା,
	       cs10_channel_gap_found, cs10_channel_ଗଣନା, valid_channel_counts_cs10);
	printf("DSM guess: CS11 (%li good frames, %i gap found, %li channel count, %i dist)\r\n", good_cs11_frame_ଗଣନା,
	       cs11_channel_gap_found, cs11_channel_ଗଣନା, valid_channel_counts_cs11);
#endif

	ଯଦି (good_cs11_frame_ଗଣନା > samples - bad_samples_allowance && !cs11_channel_gap_found
	    && cs11_channel_ଗଣନା >= minimum_channel_ଗଣନା && valid_channel_counts_cs11) {
#ifdef DSM_DEBUG
		printf("DSM guess: CS11 guessed!\n");
#endif
		dsm_chan_ଗଣନା = cs11_channel_ଗଣନା;
		dsm_channel_shift = 11;
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (good_cs10_frame_ଗଣନା > samples - bad_samples_allowance && !cs10_channel_gap_found
	    && cs10_channel_ଗଣନା >= minimum_channel_ଗଣନା && valid_channel_counts_cs10) {
#ifdef DSM_DEBUG
		printf("DSM guess: CS10 guessed!\n");
#endif
		dsm_chan_ଗଣନା = cs10_channel_ଗଣନା;
		dsm_channel_shift = 10;
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	/* call ourselves to reset our state ... we have to try again */
#ifdef DSM_DEBUG
	printf("DSM: format detect fail\n");
#endif
	dsm_guess_format(ସତ୍ୟ);
	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା dsm_config(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd)
{
#ifdef SPEKTRUM_POWER_CONFIG
	// Enable power controls for Spektrum receiver
	SPEKTRUM_POWER_CONFIG();
#endif
#ifdef SPEKTRUM_POWER
	// enable power on DSM connector
	SPEKTRUM_POWER(ସତ୍ୟ);
#endif

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = -1;

	ଯଦି (fd >= 0) {

		ସଂରଚନା termios t;

		/* 115200bps, no parity, one stop bit */
		tcgetattr(fd, &t);
		cfsetspeed(&t, 115200);
		t.c_cflag &= ~(CSTOPB | PARENB);
		tcsetattr(fd, TCSANOW, &t);

		/* initialise the decoder */
		dsm_partial_frame_ଗଣନା = 0;
		dsm_last_rx_time = hrt_absolute_time();

		/* reset the format detector */
		dsm_guess_format(ସତ୍ୟ);

		ret = 0;
	}

	ଫେରନ୍ତୁ ret;
}

ଶୂନ୍ୟ dsm_proto_init()
{
	dsm_channel_shift = 0;
	dsm_frame_drops = 0;
	dsm_chan_ଗଣନା = 0;

	dsm_decode_state = DSM_DECODE_STATE_DESYNC;

	ପାଇଁ (unsigned i = 0; i < DSM_MAX_CHANNEL_ଗଣନା; i++) {
		channel_buffer[i].last_seen = 0;
		channel_buffer[i].value = 0;
	}

	/* reset the format detector */
	dsm_guess_format(ସତ୍ୟ);
}

/**
 * Initialize the DSM receive functionality
 *
 * Open the UART for receiving DSM frames and configure it appropriately
 *
 * @param[in] device Device name of DSM UART
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା dsm_init(ଅଚଳ ଅକ୍ଷର *device)
{
	ଯଦି (dsm_fd < 0) {
		dsm_fd = open(device, O_RDWR | O_NONBLOCK);
	}

	dsm_proto_init();

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = dsm_config(dsm_fd);

	ଯଦି (!ret) {
		ଫେରନ୍ତୁ dsm_fd;

	} ଅନ୍ୟଥା {
		ଫେରନ୍ତୁ -1;
	}
}

ଶୂନ୍ୟ dsm_deinit()
{
#ifdef SPEKTRUM_POWER_PASSIVE
	// Turn power controls to passive
	SPEKTRUM_POWER_PASSIVE();
#endif

	ଯଦି (dsm_fd >= 0) {
		close(dsm_fd);
	}

	dsm_fd = -1;
}

#ଯଦି defined(SPEKTRUM_POWER)
/**
 * Handle DSM satellite receiver bind mode handler
 *
 * @param[in] cmd commands - dsm_bind_power_down, dsm_bind_power_up, dsm_bind_set_rx_out, dsm_bind_send_pulses, dsm_bind_reinit_uart
 * @param[in] pulses Number of pulses for dsm_bind_send_pulses command
 */
ଶୂନ୍ୟ dsm_bind(uint16_t cmd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା pulses)
{
	ଯଦି (dsm_fd < 0) {
		ଫେରନ୍ତୁ;
	}

	ବାଛ (cmd) {
	ସ୍ଥିତି DSM_CMD_BIND_POWER_DOWN:
		// power down DSM satellite
#ଯଦି defined(DSM_DEBUG)
		printf("DSM: DSM_CMD_BIND_POWER_DOWN\n");
#endif
		SPEKTRUM_POWER(ମିଥ୍ୟା);
		ଭାଙ୍ଗ;

	ସ୍ଥିତି DSM_CMD_BIND_POWER_UP:
		// power up DSM satellite
#ଯଦି defined(DSM_DEBUG)
		printf("DSM: DSM_CMD_BIND_POWER_UP\n");
#endif
		SPEKTRUM_POWER(ସତ୍ୟ);
		dsm_guess_format(ସତ୍ୟ);
		ଭାଙ୍ଗ;

	ସ୍ଥିତି DSM_CMD_BIND_SET_RX_OUT:
		// Set UART RX pin to active output mode
#ଯଦି defined(DSM_DEBUG)
		printf("DSM: DSM_CMD_BIND_SET_RX_OUT\n");
#endif
		SPEKTRUM_RX_AS_GPIO_OUTPUT();
		ଭାଙ୍ଗ;

	ସ୍ଥିତି DSM_CMD_BIND_SEND_PULSES:
		// Pulse RX pin a number of times
#ଯଦି defined(DSM_DEBUG)
		printf("DSM: DSM_CMD_BIND_SEND_PULSES\n");
#endif

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < pulses; i++) {
			dsm_udelay(120);
			SPEKTRUM_OUT(ମିଥ୍ୟା);
			dsm_udelay(120);
			SPEKTRUM_OUT(ସତ୍ୟ);
		}

		ଭାଙ୍ଗ;

	ସ୍ଥିତି DSM_CMD_BIND_REINIT_UART:
		// Restore USART RX pin to RS232 receive mode
#ଯଦି defined(DSM_DEBUG)
		printf("DSM: DSM_CMD_BIND_REINIT_UART\n");
#endif
		SPEKTRUM_RX_AS_UART();
		ଭାଙ୍ଗ;

	}
}
#endif

/**
 * Decode the entire dsm frame (all contained channels)
 *
 * @param[in] frame_time timestamp when this dsm frame was received. Used to detect RX loss in order to reset 10/11 bit guess.
 * @param[out] num_values pointer to number of raw channel values returned
 * @return true=DSM frame successfully decoded, false=no update
 */
ତାର୍କିକ dsm_decode(hrt_abstime frame_time, ତାର୍କିକ *dsm_11_bit, unsigned channel_ଗଣନା,
		int8_t *rssi_percent)
{
	// If we haven't seen a new frame recently or haven't guessed the DSM encoding, try to guess
	ଯଦି (((frame_time - dsm_last_frame_time) > 1000000) && (dsm_channel_shift != 0)) {
		dsm_guess_format(ସତ୍ୟ);
	}

	/* if we don't know the dsm_frame format, update the guessing state machine */
	ଯଦି (dsm_channel_shift == 0) {
		ଯଦି (!dsm_guess_format(ମିଥ୍ୟା)) {
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		}

		// We have our first packet so we're in sync
		dsm_last_rx_time = hrt_absolute_time();
	}

	/*
	 * The first byte represents the rssi in dBm on telemetry receivers with updated
	 * firmware, or fades on others. If the value is less than zero, it's rssi.
	 * We have other ways to detect bad link metrics, so ignore positive values,
	 * but rssi dBm is a useful value.
	 */

	// The SPM4649T with firmware version 1.1RC9 or later will have RSSI in place of fades
	ଯଦି (rssi_percent) {
		ଯଦି (((int8_t *)dsm_frame)[0] < 0) {
			/*
			 * RSSI is a signed integer between -42dBm and -92dBm
			 * If signal is lost, the value is -128
			 */
			int8_t dbm = (int8_t)dsm_frame[0];

			ଯଦି (dbm == -128) {
				*rssi_percent = 0;

			} ଅନ୍ୟଥା {
				*rssi_percent = spek_dbm_to_percent(dbm);
			}

		} ଅନ୍ୟଥା {
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

	px4::Bitset<DSM_MAX_CHANNEL_ଗଣନା> channels_found;

	ପାଇଁ (unsigned i = 0; i < DSM_FRAME_CHANNELS; i++) {

		uint8_t *dp = &dsm_frame[2 + (2 * i)];
		uint16_t raw = (dp[0] << 8) | dp[1];

		uint8_t channel = 0;
		uint16_t value = 0;

		ଯଦି (!dsm_decode_channel(raw, dsm_channel_shift, channel, value)) {
			dsm_frame_drops++;
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		}

		ଯଦି (channel == CHANNEL_UNUSED) {
			ଜାରି;
		}

		// abort if channel already found, no duplicate channels per DSM frame
		ଯଦି (channels_found[channel]) {
			PX4_DEBUG("duplicate channel %d\n\n", channel);
			dsm_guess_format(ସତ୍ୟ);
			dsm_frame_drops++;
			ଫେରନ୍ତୁ ମିଥ୍ୟା;

		} ଅନ୍ୟଥା {
			channels_found.set(channel);
		}

		/* reset bit guessing state machine if the channel index is out of bounds */
		ଯଦି (channel >= DSM_MAX_CHANNEL_ଗଣନା || channel >= channel_ଗଣନା) {
			PX4_DEBUG("channel %d > %d (DSM_MAX_CHANNEL_COUNT)", channel, DSM_MAX_CHANNEL_ଗଣନା);
			dsm_guess_format(ସତ୍ୟ);
			dsm_frame_drops++;
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		}

		/*
		 * Store the decoded channel into the R/C input buffer, taking into
		 * account the different ideas about channel assignement that we have.
		 *
		 * Specifically, the first four channels in rc_channel_data are roll, pitch, thrust, yaw,
		 * but the first four channels from the DSM receiver are thrust, roll, pitch, yaw.
		 */
		ବାଛ (channel) {
		ସ୍ଥିତି 0:
			channel = 2; // Spektrum Throttle (0) -> 2
			ଭାଙ୍ଗ;

		ସ୍ଥିତି 1:
			channel = 0; // Spektrum Aileron (1) -> 0
			ଭାଙ୍ଗ;

		ସ୍ଥିତି 2:
			channel = 1; // Spektrum Elevator (2) -> 1

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଭାଙ୍ଗ;
		}

		channel_buffer[channel].value = value;
		channel_buffer[channel].last_seen = frame_time;
	}

	/* Set the 11-bit data indicator */
	*dsm_11_bit = (dsm_channel_shift == 11);

	/* we have received something we think is a dsm_frame */
	dsm_last_frame_time = frame_time;

#ifdef DSM_DEBUG
	printf("PARSED PACKET\n");
#endif

	ଫେରନ୍ତୁ ସତ୍ୟ;
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
ତାର୍କିକ dsm_ନିବେଶ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint16_t *ମୂଲ୍ୟଗୁଡ଼ିକ_2, uint16_t *ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ, ତାର୍କିକ *dsm_11_bit, uint8_t *n_bytes, uint8_t **bytes,
	       int8_t *rssi, unsigned *frame_drops, unsigned max_ମୂଲ୍ୟଗୁଡ଼ିକ)
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
	ଅଚଳ hrt_abstime now = hrt_absolute_time();

	/*
	 * Fetch bytes, but no more than we would need to complete
	 * a complete frame.
	 */

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = ପଠ(fd, &dsm_buf[0], ଆକାର(dsm_buf) / ଆକାର(dsm_buf[0]));

	/* if the read failed for any reason, just give up here */
	ଯଦି (ret < 1) {
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	} ଅନ୍ୟଥା {
		*n_bytes = ret;
		*bytes = &dsm_buf[0];
	}

	/*
	 * Try to decode something with what we got
	 */
	ଫେରନ୍ତୁ dsm_parse(now, &dsm_buf[0], ret, ମୂଲ୍ୟଗୁଡ଼ିକ_2, ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ, dsm_11_bit, &dsm_frame_drops, rssi, max_ମୂଲ୍ୟଗୁଡ଼ିକ);
}

ତାର୍କିକ dsm_parse(ଅଚଳ uint64_t now, ଅଚଳ uint8_t *frame, ଅଚଳ unsigned len, uint16_t *ମୂଲ୍ୟଗୁଡ଼ିକ_2,
	       uint16_t *ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ, ତାର୍କିକ *dsm_11_bit, unsigned *frame_drops, int8_t *rssi_percent, uint16_t max_channels)
{
	/* this is set by the decoding state machine and will default to false
	 * once everything that was decodable has been decoded.
	 */
	ତାର୍କିକ channel_data_available = ମିଥ୍ୟା;
	unsigned channel_output_ଗଣନା;

	/* keep decoding until we have consumed the buffer */
	ପାଇଁ (unsigned d = 0; d < len; d++) {

		/* overflow check */
		ଯଦି (dsm_partial_frame_ଗଣନା == ଆକାର(dsm_frame) / ଆକାର(dsm_frame[0])) {
			dsm_partial_frame_ଗଣନା = 0;
			dsm_decode_state = DSM_DECODE_STATE_DESYNC;
#ifdef DSM_DEBUG
			printf("DSM: RESET (BUF LIM)\n");
#endif
		}

		ଯଦି (dsm_partial_frame_ଗଣନା == DSM_FRAME_SIZE) {
			dsm_partial_frame_ଗଣନା = 0;
			dsm_decode_state = DSM_DECODE_STATE_DESYNC;
#ifdef DSM_DEBUG
			printf("DSM: RESET (PACKET LIM)\n");
#endif
		}

#ifdef DSM_DEBUG
#ଯଦି 0
		printf("dsm state: %s%s, count: %d, val: %02x\n",
		       (dsm_decode_state == DSM_DECODE_STATE_DESYNC) ? "DSM_DECODE_STATE_DESYNC" : "",
		       (dsm_decode_state == DSM_DECODE_STATE_SYNC) ? "DSM_DECODE_STATE_SYNC" : "",
		       dsm_partial_frame_ଗଣନା,
		       (unsigned)frame[d]);
#endif
#endif

		ବାଛ (dsm_decode_state) {
		ସ୍ଥିତି DSM_DECODE_STATE_DESYNC:

			/* we are de-synced and only interested in the frame marker */
			ଯଦି ((now - dsm_last_rx_time) > 5000) {
				dsm_decode_state = DSM_DECODE_STATE_SYNC;
				dsm_partial_frame_ଗଣନା = 0;
				dsm_frame[dsm_partial_frame_ଗଣନା++] = frame[d];
			}

			ଭାଙ୍ଗ;

		ସ୍ଥିତି DSM_DECODE_STATE_SYNC: {
				dsm_frame[dsm_partial_frame_ଗଣନା++] = frame[d];

				/* decode whatever we got and expect */
				ଯଦି (dsm_partial_frame_ଗଣନା < DSM_FRAME_SIZE) {
					ଭାଙ୍ଗ;
				}

				// We've collected a full frame, parse it
				channel_data_available = dsm_decode(now, dsm_11_bit, dsm_chan_ଗଣନା, rssi_percent);

				// Frame consumed, reset the buffer count
				dsm_partial_frame_ଗଣନା = 0;

				/* if decoding failed, set proto to desync */
				ଯଦି (!channel_data_available) {
					dsm_decode_state = DSM_DECODE_STATE_DESYNC;
				}
			}
			ଭାଙ୍ଗ;

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
#ifdef DSM_DEBUG
			printf("UNKNOWN PROTO STATE");
#endif
			channel_data_available = ମିଥ୍ୟା;
		}
	}

	ଯଦି (frame_drops) {
		*frame_drops = dsm_frame_drops;
	}

	ଯଦି (channel_data_available) {
		// Check if any channels have been absent for too long
		ପାଇଁ (unsigned i = 0; i < dsm_chan_ଗଣନା; i++) {
			ଯଦି (now - channel_buffer[i].last_seen > 50 * 1000) {
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
			}
		}

		// Overflow protect
		ଯଦି (max_channels >= DSM_MAX_CHANNEL_ଗଣନା) {
			max_channels = DSM_MAX_CHANNEL_ଗଣନା;
		}

		// Cap returned channels to max_channels if we've seen more
		ଯଦି (dsm_chan_ଗଣନା > max_channels) {
			channel_output_ଗଣନା = max_channels;

		} ଅନ୍ୟଥା {
			channel_output_ଗଣନା = dsm_chan_ଗଣନା;
		}

		*ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ = channel_output_ଗଣନା;

		// Copy the full list of collected channels
		ପାଇଁ (unsigned i = 0; i < channel_output_ଗଣନା; i++) {
			// Channels which do not have a value yet (because their frame has not been seen yet)
			// will have a zero. If we come across a blank channel, keep storing values, but we
			// won't return that we have a valid frame just yet.
			ଯଦି (channel_buffer[i].value) {
				ମୂଲ୍ୟଗୁଡ଼ିକ_2[i] = channel_buffer[i].value;

			} ଅନ୍ୟଥା {
				channel_data_available = ମିଥ୍ୟା;
			}
		}
	}

#ifdef DSM_DEBUG

	ଯଦି (channel_data_available) {
		printf("frame drops: %u, chan #: %u\n", dsm_frame_drops, dsm_chan_ଗଣନା);

		ପାଇଁ (unsigned i = 0; i < dsm_chan_ଗଣନା; i++) {
			printf("dsm_decode: #CH %02u: %u\n", i + 1, ମୂଲ୍ୟଗୁଡ଼ିକ_2[i]);
		}
	}

#endif

	dsm_last_rx_time = now;

	/* return false as default */
	ଫେରନ୍ତୁ channel_data_available;
}
