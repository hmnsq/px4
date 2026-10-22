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

#அந்தர்பாவ <px4_platform_common/px4_config.h>
#அந்தர்பாவ <board_config.h>
#அந்தர்பாவ <px4_platform_common/defines.h>
#அந்தர்பாவ <px4_platform_common/log.h>

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <math.h>
#அந்தர்பாவ <unistd.h>
#அந்தர்பாவ <termios.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "dsm.h"
#அந்தர்பாவ "spektrum_rssi.h"
#அந்தர்பாவ "common_rc.h"
#அந்தர்பாவ <drivers/drv_hrt.h>

#அந்தர்பாவ <include/containers/Bitset.hpp>

#எனில் defined(__PX4_NUTTX)
#அந்தர்பாவ <nuttx/arch.h>
#define dsm_udelay(arg)    up_udelay(arg)
#இல்லையெனில்
#define dsm_udelay(arg) px4_usleep(arg)
#endif

// #define DSM_DEBUG

நிலையான எண்ணிக்கை DSM_DECODE_STATE {
	DSM_DECODE_STATE_DESYNC = 0,
	DSM_DECODE_STATE_SYNC
} dsm_decode_state = DSM_DECODE_STATE_DESYNC;

வகைவரை கட்டமைப்பு {
	hrt_abstime last_seen;
	uint16_t value;
} dsm_channel_t;

நிலையான dsm_channel_t channel_buffer[DSM_MAX_CHANNEL_மொத்தம்];

நிலையான முழு dsm_fd = -1;						/**< File handle to the DSM UART */
நிலையான hrt_abstime dsm_last_rx_time;            /**< Timestamp when we last received data */
நிலையான hrt_abstime dsm_last_frame_time;		/**< Timestamp for start of last valid dsm frame */
நிலையான dsm_frame_t &dsm_frame = rc_decode_buf.dsm.frame;	/**< DSM_BUFFER_SIZE DSM dsm frame receive buffer */
நிலையான dsm_buf_t &dsm_buf = rc_decode_buf.dsm.buf;	/**< DSM_BUFFER_SIZE DSM dsm frame receive buffer */

நிலையான unsigned dsm_partial_frame_மொத்தம்;	/**< Count of bytes received for current dsm frame */
நிலையான unsigned dsm_channel_shift = 0;			/**< Channel resolution, 0=unknown, 10=10 bit (1024), 11=11 bit (2048) */
நிலையான unsigned dsm_frame_drops = 0;			/**< Count of incomplete DSM frames */
நிலையான uint16_t dsm_chan_மொத்தம் = 0;         /**< DSM channel count */

நிலையான constexpr uint8_t CHANNEL_UNUSED = 0xFF;

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
நிலையான தருக்க dsm_decode_channel(uint16_t raw, unsigned shift, uint8_t &channel, uint16_t &value)
{
	எனில் (shift == 10) {
		// 1024 Mode: This format is used only by DSM2/22ms mode. All other modes use 2048 data.
		//  Bits 15-10 Channel ID
		//  Bits 9-0   Servo Position
		நிலையான constexpr uint16_t MASK_1024_CHANID = 0xFC00;
		நிலையான constexpr uint16_t MASK_1024_SXPOS = 0x03FF;

		channel = (raw & MASK_1024_CHANID) >> 10; // 6 bits

		மாறிலி uint16_t servo_position = (raw & MASK_1024_SXPOS); // 10 bits

		எனில் (channel >= DSM_MAX_CHANNEL_மொத்தம்) {
			PX4_DEBUG("invalid channel: %d\n", channel);
			திரும்பு பொய்;
		}

		// PWM_OUT = (ServoPosition x 1.166μs) + Offset
		நிலையான constexpr uint16_t offset = 903; // microseconds
		value = roundf(servo_position * 1.166f) + offset;

		திரும்பு உண்மை;

	} இல்லையெனில் எனில் (shift == 11) {
		// 2048 Mode
		//  Bits 15    Servo Phase
		//  Bits 14-11 Channel ID
		//  Bits 10-0  Servo Position

		uint16_t servo_position = 0;

		// from Spektrum Remote Receiver Interfacing Rev G Page 6
		மாறிலி தருக்க phase = (raw >> 15) & 0x01; // the phase is part of the X-Plus address (bit 15)
		uint8_t chan = (raw >> 11) & 0x0F;

		எனில் (chan < 12) {
			// Normal channels
			நிலையான constexpr uint16_t MASK_2048_SXPOS = 0x07FF;
			servo_position = (raw & MASK_2048_SXPOS);

		} இல்லையெனில் எனில் (chan == 12) {
			// XPlus channels
			chan += ((raw >> 9) & 0x03);

			எனில் (phase) {
				chan += 4;
			}

			எனில் (chan >= DSM_MAX_CHANNEL_மொத்தம்) {
				திரும்பு பொய்;
			}

			servo_position = (raw & 0x01FF) << 2;

			channel = chan;

		} இல்லையெனில் எனில் (chan == 0x0F && phase) {
			// When the channel content is all bits set, and the phase is also set, this is the blank channel
			channel = CHANNEL_UNUSED;
			திரும்பு உண்மை;

		} இல்லையெனில் {
			// This will be the case for an unused channel (raw 16bit content will be 0xffff)
			PX4_DEBUG("invalid: %d %d\n", chan, phase);
			திரும்பு பொய்;
		}

		channel = chan;

		// PWM_OUT = (ServoPosition x 0.583μs) + Offset
		நிலையான constexpr uint16_t offset = 903; // microseconds
		value = roundf(servo_position * 0.583f) + offset;

		PX4_DEBUG(stderr, "CH%d=%d(0x%02x), ", channel, value, raw);

		திரும்பு உண்மை;
	}

	திரும்பு பொய்;
}

/**
 * Attempt to guess if receiving 10 or 11 bit channel values
 *
 * @param[in] reset true=reset the 10/11 bit state to unknown
 */
நிலையான தருக்க dsm_guess_format(தருக்க reset)
{
	நிலையான uint32_t cs10 = 0;
	நிலையான uint32_t cs11 = 0;
	நிலையான uint16_t good_cs10_frame_மொத்தம் = 0;
	நிலையான uint16_t good_cs11_frame_மொத்தம் = 0;
	நிலையான uint16_t samples = 0;
	நிலையான uint16_t seen_channels_மொத்தம்_cs11[DSM_MAX_CHANNEL_மொத்தம்] {};
	நிலையான uint16_t seen_channels_மொத்தம்_cs10[DSM_MAX_CHANNEL_மொத்தம்] {};

	/* reset the 10/11 bit sniffed channel masks */
	எனில் (reset) {
		PX4_DEBUG("dsm_guess_format reset");
		cs10 = 0;
		cs11 = 0;
		samples = 0;

		dsm_channel_shift = 0;
		good_cs10_frame_மொத்தம் = 0;
		good_cs11_frame_மொத்தம் = 0;

		க்கு (unsigned i = 0; i < DSM_MAX_CHANNEL_மொத்தம்; i++) {
			seen_channels_மொத்தம்_cs10[i] = 0;
			seen_channels_மொத்தம்_cs11[i] = 0;
		}

		திரும்பு பொய்;
	}

	px4::Bitset<DSM_MAX_CHANNEL_மொத்தம்> channels_found_10;
	px4::Bitset<DSM_MAX_CHANNEL_மொத்தம்> channels_found_11;

	தருக்க cs10_frame_valid = உண்மை;
	தருக்க cs11_frame_valid = உண்மை;

	/* scan the channels in the current dsm_frame in both 10- and 11-bit mode */
	க்கு (unsigned i = 0; i < DSM_FRAME_CHANNELS; i++) {

		uint8_t *dp = &dsm_frame[2 + (2 * i)];
		uint16_t raw = (dp[0] << 8) | dp[1];

		uint8_t channel = 0;
		uint16_t value = 0;

		/* if the channel decodes, remember the assigned number */
		எனில் (dsm_decode_channel(raw, 10, channel, value)) {
			எனில் (channel == CHANNEL_UNUSED) {
				தொடர்;
			}

			// invalidate entire frame (for 1024) if channel already found, no duplicate channels per DSM frame
			எனில் (channels_found_10[channel]) {
				cs10_frame_valid = பொய்;

			} இல்லையெனில் {
				seen_channels_மொத்தம்_cs10[channel]++;
				channels_found_10.set(channel);
			}
		}

		எனில் (dsm_decode_channel(raw, 11, channel, value)) {
			எனில் (channel == CHANNEL_UNUSED) {
				தொடர்;
			}

			// invalidate entire frame (for 2048) if channel already found, no duplicate channels per DSM frame
			எனில் (channels_found_11[channel]) {
				cs11_frame_valid = பொய்;

			} இல்லையெனில் {
				seen_channels_மொத்தம்_cs11[channel]++;

				channels_found_11.set(channel);
			}
		}
	}

	// add valid cs10 channels
	எனில் (cs10_frame_valid) {
		good_cs10_frame_மொத்தம்++;

		க்கு (unsigned channel = 0; channel < DSM_MAX_CHANNEL_மொத்தம்; channel++) {
			எனில் (channels_found_10[channel]) {
				cs10 |= 1 << channel;
			}
		}
	}

	// add valid cs11 channels
	எனில் (cs11_frame_valid) {
		good_cs11_frame_மொத்தம்++;

		க்கு (unsigned channel = 0; channel < DSM_MAX_CHANNEL_மொத்தம்; channel++) {
			எனில் (channels_found_11[channel]) {
				cs11 |= 1 << channel;
			}
		}
	}

	samples++;

	/* wait until we have seen plenty of frames */
	எனில் (samples < 15) {
		திரும்பு பொய்;
	}

	/*
	* 10 or 11 bit decoding guess requirements
	*  For CS10 or CS11...
	*  At least `samples - bad_samples_allowance` must decode correctly (no duplicates, valid channel ranges for CS10)
	*  Even distribution of all found channels
	*  Channels must begin at 0 and no channel gaps after that (ie, the last channel is the one before the first unseen channel starting from zero)
	 */

	தருக்க cs10_channel_gap_found = பொய்;
	தருக்க cs11_channel_gap_found = பொய்;
	தருக்க valid_channel_counts_cs10 = பொய்;
	தருக்க valid_channel_counts_cs11 = பொய்;
	uint32_t cs10_channel_மொத்தம் = 0;
	uint32_t cs11_channel_மொத்தம் = 0;
	தருக்க found_channels_end = பொய்;

	// Count of allowed bad frames
	நிலையான constexpr uint16_t bad_samples_allowance = 5;
	நிலையான constexpr uint16_t minimum_channel_மொத்தம் = 5;
	நிலையான constexpr uint16_t minimum_channel_seen_மொத்தம் = 4;

	// Check for continous channels in 10bit decoding
	found_channels_end = பொய்;

	க்கு (unsigned i = 0; i < DSM_MAX_CHANNEL_மொத்தம்; i++) {
		// This bit is set
		எனில் (cs10 & (1 << i)) {
			எனில் (found_channels_end) {
				// Channel gap found
				cs10_channel_gap_found = உண்மை;
				நிறுத்து;
			}

		} இல்லையெனில் {
			எனில் (!found_channels_end) {
				cs10_channel_மொத்தம் = i;
			}

			found_channels_end = உண்மை;
		}
	}

	// Check for continous channels in 11bit decoding
	found_channels_end = பொய்;

	க்கு (unsigned i = 0; i < DSM_MAX_CHANNEL_மொத்தம்; i++) {
		// This bit is set
		எனில் (cs11 & (1 << i)) {
			எனில் (found_channels_end) {
				// Channel gap found
				cs11_channel_gap_found = உண்மை;
				நிறுத்து;
			}

		} இல்லையெனில் {
			எனில் (!found_channels_end) {
				cs11_channel_மொத்தம் = i;
			}

			found_channels_end = உண்மை;
		}
	}

	// Check channel seen counts C10
	எனில் (cs10_channel_மொத்தம் && !cs10_channel_gap_found) {
		valid_channel_counts_cs10 = உண்மை;

		க்கு (unsigned i = 0; i < ((cs10_channel_மொத்தம் > 12) ? 12 : cs10_channel_மொத்தம்); i++) {
			எனில் (seen_channels_மொத்தம்_cs10[i] < minimum_channel_seen_மொத்தம்) {
				valid_channel_counts_cs10 = பொய்;
				நிறுத்து;
			}

		}
	}

	// Check channel seen counts C11
	எனில் (cs11_channel_மொத்தம் && !cs11_channel_gap_found) {
		valid_channel_counts_cs11 = உண்மை;

		க்கு (unsigned i = 0; i < ((cs11_channel_மொத்தம் > 12) ? 12 : cs11_channel_மொத்தம்); i++) {
			எனில் (seen_channels_மொத்தம்_cs11[i] < minimum_channel_seen_மொத்தம்) {
				valid_channel_counts_cs11 = பொய்;
				நிறுத்து;
			}

		}
	}

#ifdef DSM_DEBUG
	printf("DSM guess: CS10 (%li good frames, %i gap found, %li channel count, %i dist)\r\n", good_cs10_frame_மொத்தம்,
	       cs10_channel_gap_found, cs10_channel_மொத்தம், valid_channel_counts_cs10);
	printf("DSM guess: CS11 (%li good frames, %i gap found, %li channel count, %i dist)\r\n", good_cs11_frame_மொத்தம்,
	       cs11_channel_gap_found, cs11_channel_மொத்தம், valid_channel_counts_cs11);
#endif

	எனில் (good_cs11_frame_மொத்தம் > samples - bad_samples_allowance && !cs11_channel_gap_found
	    && cs11_channel_மொத்தம் >= minimum_channel_மொத்தம் && valid_channel_counts_cs11) {
#ifdef DSM_DEBUG
		printf("DSM guess: CS11 guessed!\n");
#endif
		dsm_chan_மொத்தம் = cs11_channel_மொத்தம்;
		dsm_channel_shift = 11;
		திரும்பு உண்மை;
	}

	எனில் (good_cs10_frame_மொத்தம் > samples - bad_samples_allowance && !cs10_channel_gap_found
	    && cs10_channel_மொத்தம் >= minimum_channel_மொத்தம் && valid_channel_counts_cs10) {
#ifdef DSM_DEBUG
		printf("DSM guess: CS10 guessed!\n");
#endif
		dsm_chan_மொத்தம் = cs10_channel_மொத்தம்;
		dsm_channel_shift = 10;
		திரும்பு உண்மை;
	}

	/* call ourselves to reset our state ... we have to try again */
#ifdef DSM_DEBUG
	printf("DSM: format detect fail\n");
#endif
	dsm_guess_format(உண்மை);
	திரும்பு பொய்;
}

முழு dsm_config(முழு fd)
{
#ifdef SPEKTRUM_POWER_CONFIG
	// Enable power controls for Spektrum receiver
	SPEKTRUM_POWER_CONFIG();
#endif
#ifdef SPEKTRUM_POWER
	// enable power on DSM connector
	SPEKTRUM_POWER(உண்மை);
#endif

	முழு ret = -1;

	எனில் (fd >= 0) {

		கட்டமைப்பு termios t;

		/* 115200bps, no parity, one stop bit */
		tcgetattr(fd, &t);
		cfsetspeed(&t, 115200);
		t.c_cflag &= ~(CSTOPB | PARENB);
		tcsetattr(fd, TCSANOW, &t);

		/* initialise the decoder */
		dsm_partial_frame_மொத்தம் = 0;
		dsm_last_rx_time = hrt_absolute_time();

		/* reset the format detector */
		dsm_guess_format(உண்மை);

		ret = 0;
	}

	திரும்பு ret;
}

வெறுமை dsm_proto_init()
{
	dsm_channel_shift = 0;
	dsm_frame_drops = 0;
	dsm_chan_மொத்தம் = 0;

	dsm_decode_state = DSM_DECODE_STATE_DESYNC;

	க்கு (unsigned i = 0; i < DSM_MAX_CHANNEL_மொத்தம்; i++) {
		channel_buffer[i].last_seen = 0;
		channel_buffer[i].value = 0;
	}

	/* reset the format detector */
	dsm_guess_format(உண்மை);
}

/**
 * Initialize the DSM receive functionality
 *
 * Open the UART for receiving DSM frames and configure it appropriately
 *
 * @param[in] device Device name of DSM UART
 */
முழு dsm_init(மாறிலி எழுத்து *device)
{
	எனில் (dsm_fd < 0) {
		dsm_fd = open(device, O_RDWR | O_NONBLOCK);
	}

	dsm_proto_init();

	முழு ret = dsm_config(dsm_fd);

	எனில் (!ret) {
		திரும்பு dsm_fd;

	} இல்லையெனில் {
		திரும்பு -1;
	}
}

வெறுமை dsm_deinit()
{
#ifdef SPEKTRUM_POWER_PASSIVE
	// Turn power controls to passive
	SPEKTRUM_POWER_PASSIVE();
#endif

	எனில் (dsm_fd >= 0) {
		close(dsm_fd);
	}

	dsm_fd = -1;
}

#எனில் defined(SPEKTRUM_POWER)
/**
 * Handle DSM satellite receiver bind mode handler
 *
 * @param[in] cmd commands - dsm_bind_power_down, dsm_bind_power_up, dsm_bind_set_rx_out, dsm_bind_send_pulses, dsm_bind_reinit_uart
 * @param[in] pulses Number of pulses for dsm_bind_send_pulses command
 */
வெறுமை dsm_bind(uint16_t cmd, முழு pulses)
{
	எனில் (dsm_fd < 0) {
		திரும்பு;
	}

	தேர்வு (cmd) {
	நிலை DSM_CMD_BIND_POWER_DOWN:
		// power down DSM satellite
#எனில் defined(DSM_DEBUG)
		printf("DSM: DSM_CMD_BIND_POWER_DOWN\n");
#endif
		SPEKTRUM_POWER(பொய்);
		நிறுத்து;

	நிலை DSM_CMD_BIND_POWER_UP:
		// power up DSM satellite
#எனில் defined(DSM_DEBUG)
		printf("DSM: DSM_CMD_BIND_POWER_UP\n");
#endif
		SPEKTRUM_POWER(உண்மை);
		dsm_guess_format(உண்மை);
		நிறுத்து;

	நிலை DSM_CMD_BIND_SET_RX_OUT:
		// Set UART RX pin to active output mode
#எனில் defined(DSM_DEBUG)
		printf("DSM: DSM_CMD_BIND_SET_RX_OUT\n");
#endif
		SPEKTRUM_RX_AS_GPIO_OUTPUT();
		நிறுத்து;

	நிலை DSM_CMD_BIND_SEND_PULSES:
		// Pulse RX pin a number of times
#எனில் defined(DSM_DEBUG)
		printf("DSM: DSM_CMD_BIND_SEND_PULSES\n");
#endif

		க்கு (முழு i = 0; i < pulses; i++) {
			dsm_udelay(120);
			SPEKTRUM_OUT(பொய்);
			dsm_udelay(120);
			SPEKTRUM_OUT(உண்மை);
		}

		நிறுத்து;

	நிலை DSM_CMD_BIND_REINIT_UART:
		// Restore USART RX pin to RS232 receive mode
#எனில் defined(DSM_DEBUG)
		printf("DSM: DSM_CMD_BIND_REINIT_UART\n");
#endif
		SPEKTRUM_RX_AS_UART();
		நிறுத்து;

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
தருக்க dsm_decode(hrt_abstime frame_time, தருக்க *dsm_11_bit, unsigned channel_மொத்தம்,
		int8_t *rssi_percent)
{
	// If we haven't seen a new frame recently or haven't guessed the DSM encoding, try to guess
	எனில் (((frame_time - dsm_last_frame_time) > 1000000) && (dsm_channel_shift != 0)) {
		dsm_guess_format(உண்மை);
	}

	/* if we don't know the dsm_frame format, update the guessing state machine */
	எனில் (dsm_channel_shift == 0) {
		எனில் (!dsm_guess_format(பொய்)) {
			திரும்பு பொய்;
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
	எனில் (rssi_percent) {
		எனில் (((int8_t *)dsm_frame)[0] < 0) {
			/*
			 * RSSI is a signed integer between -42dBm and -92dBm
			 * If signal is lost, the value is -128
			 */
			int8_t dbm = (int8_t)dsm_frame[0];

			எனில் (dbm == -128) {
				*rssi_percent = 0;

			} இல்லையெனில் {
				*rssi_percent = spek_dbm_to_percent(dbm);
			}

		} இல்லையெனில் {
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

	px4::Bitset<DSM_MAX_CHANNEL_மொத்தம்> channels_found;

	க்கு (unsigned i = 0; i < DSM_FRAME_CHANNELS; i++) {

		uint8_t *dp = &dsm_frame[2 + (2 * i)];
		uint16_t raw = (dp[0] << 8) | dp[1];

		uint8_t channel = 0;
		uint16_t value = 0;

		எனில் (!dsm_decode_channel(raw, dsm_channel_shift, channel, value)) {
			dsm_frame_drops++;
			திரும்பு பொய்;
		}

		எனில் (channel == CHANNEL_UNUSED) {
			தொடர்;
		}

		// abort if channel already found, no duplicate channels per DSM frame
		எனில் (channels_found[channel]) {
			PX4_DEBUG("duplicate channel %d\n\n", channel);
			dsm_guess_format(உண்மை);
			dsm_frame_drops++;
			திரும்பு பொய்;

		} இல்லையெனில் {
			channels_found.set(channel);
		}

		/* reset bit guessing state machine if the channel index is out of bounds */
		எனில் (channel >= DSM_MAX_CHANNEL_மொத்தம் || channel >= channel_மொத்தம்) {
			PX4_DEBUG("channel %d > %d (DSM_MAX_CHANNEL_COUNT)", channel, DSM_MAX_CHANNEL_மொத்தம்);
			dsm_guess_format(உண்மை);
			dsm_frame_drops++;
			திரும்பு பொய்;
		}

		/*
		 * Store the decoded channel into the R/C input buffer, taking into
		 * account the different ideas about channel assignement that we have.
		 *
		 * Specifically, the first four channels in rc_channel_data are roll, pitch, thrust, yaw,
		 * but the first four channels from the DSM receiver are thrust, roll, pitch, yaw.
		 */
		தேர்வு (channel) {
		நிலை 0:
			channel = 2; // Spektrum Throttle (0) -> 2
			நிறுத்து;

		நிலை 1:
			channel = 0; // Spektrum Aileron (1) -> 0
			நிறுத்து;

		நிலை 2:
			channel = 1; // Spektrum Elevator (2) -> 1

		இயல்பு:
			நிறுத்து;
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

	திரும்பு உண்மை;
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
தருக்க dsm_உள்ளீடு(முழு fd, uint16_t *மதிப்புகள்_2, uint16_t *இலக்கம்_மதிப்புகள், தருக்க *dsm_11_bit, uint8_t *n_bytes, uint8_t **bytes,
	       int8_t *rssi, unsigned *frame_drops, unsigned max_மதிப்புகள்)
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
	மாறிலி hrt_abstime now = hrt_absolute_time();

	/*
	 * Fetch bytes, but no more than we would need to complete
	 * a complete frame.
	 */

	முழு ret = பட(fd, &dsm_buf[0], அளவு(dsm_buf) / அளவு(dsm_buf[0]));

	/* if the read failed for any reason, just give up here */
	எனில் (ret < 1) {
		திரும்பு பொய்;

	} இல்லையெனில் {
		*n_bytes = ret;
		*bytes = &dsm_buf[0];
	}

	/*
	 * Try to decode something with what we got
	 */
	திரும்பு dsm_parse(now, &dsm_buf[0], ret, மதிப்புகள்_2, இலக்கம்_மதிப்புகள், dsm_11_bit, &dsm_frame_drops, rssi, max_மதிப்புகள்);
}

தருக்க dsm_parse(மாறிலி uint64_t now, மாறிலி uint8_t *frame, மாறிலி unsigned len, uint16_t *மதிப்புகள்_2,
	       uint16_t *இலக்கம்_மதிப்புகள், தருக்க *dsm_11_bit, unsigned *frame_drops, int8_t *rssi_percent, uint16_t max_channels)
{
	/* this is set by the decoding state machine and will default to false
	 * once everything that was decodable has been decoded.
	 */
	தருக்க channel_data_available = பொய்;
	unsigned channel_output_மொத்தம்;

	/* keep decoding until we have consumed the buffer */
	க்கு (unsigned d = 0; d < len; d++) {

		/* overflow check */
		எனில் (dsm_partial_frame_மொத்தம் == அளவு(dsm_frame) / அளவு(dsm_frame[0])) {
			dsm_partial_frame_மொத்தம் = 0;
			dsm_decode_state = DSM_DECODE_STATE_DESYNC;
#ifdef DSM_DEBUG
			printf("DSM: RESET (BUF LIM)\n");
#endif
		}

		எனில் (dsm_partial_frame_மொத்தம் == DSM_FRAME_SIZE) {
			dsm_partial_frame_மொத்தம் = 0;
			dsm_decode_state = DSM_DECODE_STATE_DESYNC;
#ifdef DSM_DEBUG
			printf("DSM: RESET (PACKET LIM)\n");
#endif
		}

#ifdef DSM_DEBUG
#எனில் 0
		printf("dsm state: %s%s, count: %d, val: %02x\n",
		       (dsm_decode_state == DSM_DECODE_STATE_DESYNC) ? "DSM_DECODE_STATE_DESYNC" : "",
		       (dsm_decode_state == DSM_DECODE_STATE_SYNC) ? "DSM_DECODE_STATE_SYNC" : "",
		       dsm_partial_frame_மொத்தம்,
		       (unsigned)frame[d]);
#endif
#endif

		தேர்வு (dsm_decode_state) {
		நிலை DSM_DECODE_STATE_DESYNC:

			/* we are de-synced and only interested in the frame marker */
			எனில் ((now - dsm_last_rx_time) > 5000) {
				dsm_decode_state = DSM_DECODE_STATE_SYNC;
				dsm_partial_frame_மொத்தம் = 0;
				dsm_frame[dsm_partial_frame_மொத்தம்++] = frame[d];
			}

			நிறுத்து;

		நிலை DSM_DECODE_STATE_SYNC: {
				dsm_frame[dsm_partial_frame_மொத்தம்++] = frame[d];

				/* decode whatever we got and expect */
				எனில் (dsm_partial_frame_மொத்தம் < DSM_FRAME_SIZE) {
					நிறுத்து;
				}

				// We've collected a full frame, parse it
				channel_data_available = dsm_decode(now, dsm_11_bit, dsm_chan_மொத்தம், rssi_percent);

				// Frame consumed, reset the buffer count
				dsm_partial_frame_மொத்தம் = 0;

				/* if decoding failed, set proto to desync */
				எனில் (!channel_data_available) {
					dsm_decode_state = DSM_DECODE_STATE_DESYNC;
				}
			}
			நிறுத்து;

		இயல்பு:
#ifdef DSM_DEBUG
			printf("UNKNOWN PROTO STATE");
#endif
			channel_data_available = பொய்;
		}
	}

	எனில் (frame_drops) {
		*frame_drops = dsm_frame_drops;
	}

	எனில் (channel_data_available) {
		// Check if any channels have been absent for too long
		க்கு (unsigned i = 0; i < dsm_chan_மொத்தம்; i++) {
			எனில் (now - channel_buffer[i].last_seen > 50 * 1000) {
				திரும்பு பொய்;
			}
		}

		// Overflow protect
		எனில் (max_channels >= DSM_MAX_CHANNEL_மொத்தம்) {
			max_channels = DSM_MAX_CHANNEL_மொத்தம்;
		}

		// Cap returned channels to max_channels if we've seen more
		எனில் (dsm_chan_மொத்தம் > max_channels) {
			channel_output_மொத்தம் = max_channels;

		} இல்லையெனில் {
			channel_output_மொத்தம் = dsm_chan_மொத்தம்;
		}

		*இலக்கம்_மதிப்புகள் = channel_output_மொத்தம்;

		// Copy the full list of collected channels
		க்கு (unsigned i = 0; i < channel_output_மொத்தம்; i++) {
			// Channels which do not have a value yet (because their frame has not been seen yet)
			// will have a zero. If we come across a blank channel, keep storing values, but we
			// won't return that we have a valid frame just yet.
			எனில் (channel_buffer[i].value) {
				மதிப்புகள்_2[i] = channel_buffer[i].value;

			} இல்லையெனில் {
				channel_data_available = பொய்;
			}
		}
	}

#ifdef DSM_DEBUG

	எனில் (channel_data_available) {
		printf("frame drops: %u, chan #: %u\n", dsm_frame_drops, dsm_chan_மொத்தம்);

		க்கு (unsigned i = 0; i < dsm_chan_மொத்தம்; i++) {
			printf("dsm_decode: #CH %02u: %u\n", i + 1, மதிப்புகள்_2[i]);
		}
	}

#endif

	dsm_last_rx_time = now;

	/* return false as default */
	திரும்பு channel_data_available;
}
