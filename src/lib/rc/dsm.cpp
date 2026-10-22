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

#અન્તર્ભાવ <px4_platform_common/px4_config.h>
#અન્તર્ભાવ <board_config.h>
#અન્તર્ભાવ <px4_platform_common/defines.h>
#અન્તર્ભાવ <px4_platform_common/log.h>

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <math.h>
#અન્તર્ભાવ <unistd.h>
#અન્તર્ભાવ <termios.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "dsm.h"
#અન્તર્ભાવ "spektrum_rssi.h"
#અન્તર્ભાવ "common_rc.h"
#અન્તર્ભાવ <drivers/drv_hrt.h>

#અન્તર્ભાવ <include/containers/Bitset.hpp>

#જો defined(__PX4_NUTTX)
#અન્તર્ભાવ <nuttx/arch.h>
#define dsm_udelay(arg)    up_udelay(arg)
#નહીંતર
#define dsm_udelay(arg) px4_usleep(arg)
#endif

// #define DSM_DEBUG

સ્થિર ગણના DSM_DECODE_STATE {
	DSM_DECODE_STATE_DESYNC = 0,
	DSM_DECODE_STATE_SYNC
} dsm_decode_state = DSM_DECODE_STATE_DESYNC;

પ્રકારવ્યાખ્યા સંરચના {
	hrt_abstime last_seen;
	uint16_t value;
} dsm_channel_t;

સ્થિર dsm_channel_t channel_buffer[DSM_MAX_CHANNEL_ગણતરી];

સ્થિર પૂર્ણાંક dsm_fd = -1;						/**< File handle to the DSM UART */
સ્થિર hrt_abstime dsm_last_rx_time;            /**< Timestamp when we last received data */
સ્થિર hrt_abstime dsm_last_frame_time;		/**< Timestamp for start of last valid dsm frame */
સ્થિર dsm_frame_t &dsm_frame = rc_decode_buf.dsm.frame;	/**< DSM_BUFFER_SIZE DSM dsm frame receive buffer */
સ્થિર dsm_buf_t &dsm_buf = rc_decode_buf.dsm.buf;	/**< DSM_BUFFER_SIZE DSM dsm frame receive buffer */

સ્થિર unsigned dsm_partial_frame_ગણતરી;	/**< Count of bytes received for current dsm frame */
સ્થિર unsigned dsm_channel_shift = 0;			/**< Channel resolution, 0=unknown, 10=10 bit (1024), 11=11 bit (2048) */
સ્થિર unsigned dsm_frame_drops = 0;			/**< Count of incomplete DSM frames */
સ્થિર uint16_t dsm_chan_ગણતરી = 0;         /**< DSM channel count */

સ્થિર constexpr uint8_t CHANNEL_UNUSED = 0xFF;

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
સ્થિર તાર્કિક dsm_decode_channel(uint16_t raw, unsigned shift, uint8_t &channel, uint16_t &value)
{
	જો (shift == 10) {
		// 1024 Mode: This format is used only by DSM2/22ms mode. All other modes use 2048 data.
		//  Bits 15-10 Channel ID
		//  Bits 9-0   Servo Position
		સ્થિર constexpr uint16_t MASK_1024_CHANID = 0xFC00;
		સ્થિર constexpr uint16_t MASK_1024_SXPOS = 0x03FF;

		channel = (raw & MASK_1024_CHANID) >> 10; // 6 bits

		અચલ uint16_t servo_position = (raw & MASK_1024_SXPOS); // 10 bits

		જો (channel >= DSM_MAX_CHANNEL_ગણતરી) {
			PX4_DEBUG("invalid channel: %d\n", channel);
			પાછા ખોટું;
		}

		// PWM_OUT = (ServoPosition x 1.166μs) + Offset
		સ્થિર constexpr uint16_t offset = 903; // microseconds
		value = roundf(servo_position * 1.166f) + offset;

		પાછા સાચું;

	} નહીંતર જો (shift == 11) {
		// 2048 Mode
		//  Bits 15    Servo Phase
		//  Bits 14-11 Channel ID
		//  Bits 10-0  Servo Position

		uint16_t servo_position = 0;

		// from Spektrum Remote Receiver Interfacing Rev G Page 6
		અચલ તાર્કિક phase = (raw >> 15) & 0x01; // the phase is part of the X-Plus address (bit 15)
		uint8_t chan = (raw >> 11) & 0x0F;

		જો (chan < 12) {
			// Normal channels
			સ્થિર constexpr uint16_t MASK_2048_SXPOS = 0x07FF;
			servo_position = (raw & MASK_2048_SXPOS);

		} નહીંતર જો (chan == 12) {
			// XPlus channels
			chan += ((raw >> 9) & 0x03);

			જો (phase) {
				chan += 4;
			}

			જો (chan >= DSM_MAX_CHANNEL_ગણતરી) {
				પાછા ખોટું;
			}

			servo_position = (raw & 0x01FF) << 2;

			channel = chan;

		} નહીંતર જો (chan == 0x0F && phase) {
			// When the channel content is all bits set, and the phase is also set, this is the blank channel
			channel = CHANNEL_UNUSED;
			પાછા સાચું;

		} નહીંતર {
			// This will be the case for an unused channel (raw 16bit content will be 0xffff)
			PX4_DEBUG("invalid: %d %d\n", chan, phase);
			પાછા ખોટું;
		}

		channel = chan;

		// PWM_OUT = (ServoPosition x 0.583μs) + Offset
		સ્થિર constexpr uint16_t offset = 903; // microseconds
		value = roundf(servo_position * 0.583f) + offset;

		PX4_DEBUG(stderr, "CH%d=%d(0x%02x), ", channel, value, raw);

		પાછા સાચું;
	}

	પાછા ખોટું;
}

/**
 * Attempt to guess if receiving 10 or 11 bit channel values
 *
 * @param[in] reset true=reset the 10/11 bit state to unknown
 */
સ્થિર તાર્કિક dsm_guess_format(તાર્કિક reset)
{
	સ્થિર uint32_t cs10 = 0;
	સ્થિર uint32_t cs11 = 0;
	સ્થિર uint16_t good_cs10_frame_ગણતરી = 0;
	સ્થિર uint16_t good_cs11_frame_ગણતરી = 0;
	સ્થિર uint16_t samples = 0;
	સ્થિર uint16_t seen_channels_ગણતરી_cs11[DSM_MAX_CHANNEL_ગણતરી] {};
	સ્થિર uint16_t seen_channels_ગણતરી_cs10[DSM_MAX_CHANNEL_ગણતરી] {};

	/* reset the 10/11 bit sniffed channel masks */
	જો (reset) {
		PX4_DEBUG("dsm_guess_format reset");
		cs10 = 0;
		cs11 = 0;
		samples = 0;

		dsm_channel_shift = 0;
		good_cs10_frame_ગણતરી = 0;
		good_cs11_frame_ગણતરી = 0;

		માટે (unsigned i = 0; i < DSM_MAX_CHANNEL_ગણતરી; i++) {
			seen_channels_ગણતરી_cs10[i] = 0;
			seen_channels_ગણતરી_cs11[i] = 0;
		}

		પાછા ખોટું;
	}

	px4::Bitset<DSM_MAX_CHANNEL_ગણતરી> channels_found_10;
	px4::Bitset<DSM_MAX_CHANNEL_ગણતરી> channels_found_11;

	તાર્કિક cs10_frame_valid = સાચું;
	તાર્કિક cs11_frame_valid = સાચું;

	/* scan the channels in the current dsm_frame in both 10- and 11-bit mode */
	માટે (unsigned i = 0; i < DSM_FRAME_CHANNELS; i++) {

		uint8_t *dp = &dsm_frame[2 + (2 * i)];
		uint16_t raw = (dp[0] << 8) | dp[1];

		uint8_t channel = 0;
		uint16_t value = 0;

		/* if the channel decodes, remember the assigned number */
		જો (dsm_decode_channel(raw, 10, channel, value)) {
			જો (channel == CHANNEL_UNUSED) {
				ચાલુ;
			}

			// invalidate entire frame (for 1024) if channel already found, no duplicate channels per DSM frame
			જો (channels_found_10[channel]) {
				cs10_frame_valid = ખોટું;

			} નહીંતર {
				seen_channels_ગણતરી_cs10[channel]++;
				channels_found_10.set(channel);
			}
		}

		જો (dsm_decode_channel(raw, 11, channel, value)) {
			જો (channel == CHANNEL_UNUSED) {
				ચાલુ;
			}

			// invalidate entire frame (for 2048) if channel already found, no duplicate channels per DSM frame
			જો (channels_found_11[channel]) {
				cs11_frame_valid = ખોટું;

			} નહીંતર {
				seen_channels_ગણતરી_cs11[channel]++;

				channels_found_11.set(channel);
			}
		}
	}

	// add valid cs10 channels
	જો (cs10_frame_valid) {
		good_cs10_frame_ગણતરી++;

		માટે (unsigned channel = 0; channel < DSM_MAX_CHANNEL_ગણતરી; channel++) {
			જો (channels_found_10[channel]) {
				cs10 |= 1 << channel;
			}
		}
	}

	// add valid cs11 channels
	જો (cs11_frame_valid) {
		good_cs11_frame_ગણતરી++;

		માટે (unsigned channel = 0; channel < DSM_MAX_CHANNEL_ગણતરી; channel++) {
			જો (channels_found_11[channel]) {
				cs11 |= 1 << channel;
			}
		}
	}

	samples++;

	/* wait until we have seen plenty of frames */
	જો (samples < 15) {
		પાછા ખોટું;
	}

	/*
	* 10 or 11 bit decoding guess requirements
	*  For CS10 or CS11...
	*  At least `samples - bad_samples_allowance` must decode correctly (no duplicates, valid channel ranges for CS10)
	*  Even distribution of all found channels
	*  Channels must begin at 0 and no channel gaps after that (ie, the last channel is the one before the first unseen channel starting from zero)
	 */

	તાર્કિક cs10_channel_gap_found = ખોટું;
	તાર્કિક cs11_channel_gap_found = ખોટું;
	તાર્કિક valid_channel_counts_cs10 = ખોટું;
	તાર્કિક valid_channel_counts_cs11 = ખોટું;
	uint32_t cs10_channel_ગણતરી = 0;
	uint32_t cs11_channel_ગણતરી = 0;
	તાર્કિક found_channels_end = ખોટું;

	// Count of allowed bad frames
	સ્થિર constexpr uint16_t bad_samples_allowance = 5;
	સ્થિર constexpr uint16_t minimum_channel_ગણતરી = 5;
	સ્થિર constexpr uint16_t minimum_channel_seen_ગણતરી = 4;

	// Check for continous channels in 10bit decoding
	found_channels_end = ખોટું;

	માટે (unsigned i = 0; i < DSM_MAX_CHANNEL_ગણતરી; i++) {
		// This bit is set
		જો (cs10 & (1 << i)) {
			જો (found_channels_end) {
				// Channel gap found
				cs10_channel_gap_found = સાચું;
				તોડો;
			}

		} નહીંતર {
			જો (!found_channels_end) {
				cs10_channel_ગણતરી = i;
			}

			found_channels_end = સાચું;
		}
	}

	// Check for continous channels in 11bit decoding
	found_channels_end = ખોટું;

	માટે (unsigned i = 0; i < DSM_MAX_CHANNEL_ગણતરી; i++) {
		// This bit is set
		જો (cs11 & (1 << i)) {
			જો (found_channels_end) {
				// Channel gap found
				cs11_channel_gap_found = સાચું;
				તોડો;
			}

		} નહીંતર {
			જો (!found_channels_end) {
				cs11_channel_ગણતરી = i;
			}

			found_channels_end = સાચું;
		}
	}

	// Check channel seen counts C10
	જો (cs10_channel_ગણતરી && !cs10_channel_gap_found) {
		valid_channel_counts_cs10 = સાચું;

		માટે (unsigned i = 0; i < ((cs10_channel_ગણતરી > 12) ? 12 : cs10_channel_ગણતરી); i++) {
			જો (seen_channels_ગણતરી_cs10[i] < minimum_channel_seen_ગણતરી) {
				valid_channel_counts_cs10 = ખોટું;
				તોડો;
			}

		}
	}

	// Check channel seen counts C11
	જો (cs11_channel_ગણતરી && !cs11_channel_gap_found) {
		valid_channel_counts_cs11 = સાચું;

		માટે (unsigned i = 0; i < ((cs11_channel_ગણતરી > 12) ? 12 : cs11_channel_ગણતરી); i++) {
			જો (seen_channels_ગણતરી_cs11[i] < minimum_channel_seen_ગણતરી) {
				valid_channel_counts_cs11 = ખોટું;
				તોડો;
			}

		}
	}

#ifdef DSM_DEBUG
	printf("DSM guess: CS10 (%li good frames, %i gap found, %li channel count, %i dist)\r\n", good_cs10_frame_ગણતરી,
	       cs10_channel_gap_found, cs10_channel_ગણતરી, valid_channel_counts_cs10);
	printf("DSM guess: CS11 (%li good frames, %i gap found, %li channel count, %i dist)\r\n", good_cs11_frame_ગણતરી,
	       cs11_channel_gap_found, cs11_channel_ગણતરી, valid_channel_counts_cs11);
#endif

	જો (good_cs11_frame_ગણતરી > samples - bad_samples_allowance && !cs11_channel_gap_found
	    && cs11_channel_ગણતરી >= minimum_channel_ગણતરી && valid_channel_counts_cs11) {
#ifdef DSM_DEBUG
		printf("DSM guess: CS11 guessed!\n");
#endif
		dsm_chan_ગણતરી = cs11_channel_ગણતરી;
		dsm_channel_shift = 11;
		પાછા સાચું;
	}

	જો (good_cs10_frame_ગણતરી > samples - bad_samples_allowance && !cs10_channel_gap_found
	    && cs10_channel_ગણતરી >= minimum_channel_ગણતરી && valid_channel_counts_cs10) {
#ifdef DSM_DEBUG
		printf("DSM guess: CS10 guessed!\n");
#endif
		dsm_chan_ગણતરી = cs10_channel_ગણતરી;
		dsm_channel_shift = 10;
		પાછા સાચું;
	}

	/* call ourselves to reset our state ... we have to try again */
#ifdef DSM_DEBUG
	printf("DSM: format detect fail\n");
#endif
	dsm_guess_format(સાચું);
	પાછા ખોટું;
}

પૂર્ણાંક dsm_config(પૂર્ણાંક fd)
{
#ifdef SPEKTRUM_POWER_CONFIG
	// Enable power controls for Spektrum receiver
	SPEKTRUM_POWER_CONFIG();
#endif
#ifdef SPEKTRUM_POWER
	// enable power on DSM connector
	SPEKTRUM_POWER(સાચું);
#endif

	પૂર્ણાંક ret = -1;

	જો (fd >= 0) {

		સંરચના termios t;

		/* 115200bps, no parity, one stop bit */
		tcgetattr(fd, &t);
		cfsetspeed(&t, 115200);
		t.c_cflag &= ~(CSTOPB | PARENB);
		tcsetattr(fd, TCSANOW, &t);

		/* initialise the decoder */
		dsm_partial_frame_ગણતરી = 0;
		dsm_last_rx_time = hrt_absolute_time();

		/* reset the format detector */
		dsm_guess_format(સાચું);

		ret = 0;
	}

	પાછા ret;
}

શૂન્ય dsm_proto_init()
{
	dsm_channel_shift = 0;
	dsm_frame_drops = 0;
	dsm_chan_ગણતરી = 0;

	dsm_decode_state = DSM_DECODE_STATE_DESYNC;

	માટે (unsigned i = 0; i < DSM_MAX_CHANNEL_ગણતરી; i++) {
		channel_buffer[i].last_seen = 0;
		channel_buffer[i].value = 0;
	}

	/* reset the format detector */
	dsm_guess_format(સાચું);
}

/**
 * Initialize the DSM receive functionality
 *
 * Open the UART for receiving DSM frames and configure it appropriately
 *
 * @param[in] device Device name of DSM UART
 */
પૂર્ણાંક dsm_init(અચલ અક્ષર *device)
{
	જો (dsm_fd < 0) {
		dsm_fd = open(device, O_RDWR | O_NONBLOCK);
	}

	dsm_proto_init();

	પૂર્ણાંક ret = dsm_config(dsm_fd);

	જો (!ret) {
		પાછા dsm_fd;

	} નહીંતર {
		પાછા -1;
	}
}

શૂન્ય dsm_deinit()
{
#ifdef SPEKTRUM_POWER_PASSIVE
	// Turn power controls to passive
	SPEKTRUM_POWER_PASSIVE();
#endif

	જો (dsm_fd >= 0) {
		close(dsm_fd);
	}

	dsm_fd = -1;
}

#જો defined(SPEKTRUM_POWER)
/**
 * Handle DSM satellite receiver bind mode handler
 *
 * @param[in] cmd commands - dsm_bind_power_down, dsm_bind_power_up, dsm_bind_set_rx_out, dsm_bind_send_pulses, dsm_bind_reinit_uart
 * @param[in] pulses Number of pulses for dsm_bind_send_pulses command
 */
શૂન્ય dsm_bind(uint16_t cmd, પૂર્ણાંક pulses)
{
	જો (dsm_fd < 0) {
		પાછા;
	}

	પસંદગી (cmd) {
	સ્થિતિ DSM_CMD_BIND_POWER_DOWN:
		// power down DSM satellite
#જો defined(DSM_DEBUG)
		printf("DSM: DSM_CMD_BIND_POWER_DOWN\n");
#endif
		SPEKTRUM_POWER(ખોટું);
		તોડો;

	સ્થિતિ DSM_CMD_BIND_POWER_UP:
		// power up DSM satellite
#જો defined(DSM_DEBUG)
		printf("DSM: DSM_CMD_BIND_POWER_UP\n");
#endif
		SPEKTRUM_POWER(સાચું);
		dsm_guess_format(સાચું);
		તોડો;

	સ્થિતિ DSM_CMD_BIND_SET_RX_OUT:
		// Set UART RX pin to active output mode
#જો defined(DSM_DEBUG)
		printf("DSM: DSM_CMD_BIND_SET_RX_OUT\n");
#endif
		SPEKTRUM_RX_AS_GPIO_OUTPUT();
		તોડો;

	સ્થિતિ DSM_CMD_BIND_SEND_PULSES:
		// Pulse RX pin a number of times
#જો defined(DSM_DEBUG)
		printf("DSM: DSM_CMD_BIND_SEND_PULSES\n");
#endif

		માટે (પૂર્ણાંક i = 0; i < pulses; i++) {
			dsm_udelay(120);
			SPEKTRUM_OUT(ખોટું);
			dsm_udelay(120);
			SPEKTRUM_OUT(સાચું);
		}

		તોડો;

	સ્થિતિ DSM_CMD_BIND_REINIT_UART:
		// Restore USART RX pin to RS232 receive mode
#જો defined(DSM_DEBUG)
		printf("DSM: DSM_CMD_BIND_REINIT_UART\n");
#endif
		SPEKTRUM_RX_AS_UART();
		તોડો;

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
તાર્કિક dsm_decode(hrt_abstime frame_time, તાર્કિક *dsm_11_bit, unsigned channel_ગણતરી,
		int8_t *rssi_percent)
{
	// If we haven't seen a new frame recently or haven't guessed the DSM encoding, try to guess
	જો (((frame_time - dsm_last_frame_time) > 1000000) && (dsm_channel_shift != 0)) {
		dsm_guess_format(સાચું);
	}

	/* if we don't know the dsm_frame format, update the guessing state machine */
	જો (dsm_channel_shift == 0) {
		જો (!dsm_guess_format(ખોટું)) {
			પાછા ખોટું;
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
	જો (rssi_percent) {
		જો (((int8_t *)dsm_frame)[0] < 0) {
			/*
			 * RSSI is a signed integer between -42dBm and -92dBm
			 * If signal is lost, the value is -128
			 */
			int8_t dbm = (int8_t)dsm_frame[0];

			જો (dbm == -128) {
				*rssi_percent = 0;

			} નહીંતર {
				*rssi_percent = spek_dbm_to_percent(dbm);
			}

		} નહીંતર {
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

	px4::Bitset<DSM_MAX_CHANNEL_ગણતરી> channels_found;

	માટે (unsigned i = 0; i < DSM_FRAME_CHANNELS; i++) {

		uint8_t *dp = &dsm_frame[2 + (2 * i)];
		uint16_t raw = (dp[0] << 8) | dp[1];

		uint8_t channel = 0;
		uint16_t value = 0;

		જો (!dsm_decode_channel(raw, dsm_channel_shift, channel, value)) {
			dsm_frame_drops++;
			પાછા ખોટું;
		}

		જો (channel == CHANNEL_UNUSED) {
			ચાલુ;
		}

		// abort if channel already found, no duplicate channels per DSM frame
		જો (channels_found[channel]) {
			PX4_DEBUG("duplicate channel %d\n\n", channel);
			dsm_guess_format(સાચું);
			dsm_frame_drops++;
			પાછા ખોટું;

		} નહીંતર {
			channels_found.set(channel);
		}

		/* reset bit guessing state machine if the channel index is out of bounds */
		જો (channel >= DSM_MAX_CHANNEL_ગણતરી || channel >= channel_ગણતરી) {
			PX4_DEBUG("channel %d > %d (DSM_MAX_CHANNEL_COUNT)", channel, DSM_MAX_CHANNEL_ગણતરી);
			dsm_guess_format(સાચું);
			dsm_frame_drops++;
			પાછા ખોટું;
		}

		/*
		 * Store the decoded channel into the R/C input buffer, taking into
		 * account the different ideas about channel assignement that we have.
		 *
		 * Specifically, the first four channels in rc_channel_data are roll, pitch, thrust, yaw,
		 * but the first four channels from the DSM receiver are thrust, roll, pitch, yaw.
		 */
		પસંદગી (channel) {
		સ્થિતિ 0:
			channel = 2; // Spektrum Throttle (0) -> 2
			તોડો;

		સ્થિતિ 1:
			channel = 0; // Spektrum Aileron (1) -> 0
			તોડો;

		સ્થિતિ 2:
			channel = 1; // Spektrum Elevator (2) -> 1

		મૂળભૂત:
			તોડો;
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

	પાછા સાચું;
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
તાર્કિક dsm_આગત(પૂર્ણાંક fd, uint16_t *કિંમતો_2, uint16_t *અંક_કિંમતો, તાર્કિક *dsm_11_bit, uint8_t *n_bytes, uint8_t **bytes,
	       int8_t *rssi, unsigned *frame_drops, unsigned max_કિંમતો)
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
	અચલ hrt_abstime now = hrt_absolute_time();

	/*
	 * Fetch bytes, but no more than we would need to complete
	 * a complete frame.
	 */

	પૂર્ણાંક ret = પઠ(fd, &dsm_buf[0], કદ(dsm_buf) / કદ(dsm_buf[0]));

	/* if the read failed for any reason, just give up here */
	જો (ret < 1) {
		પાછા ખોટું;

	} નહીંતર {
		*n_bytes = ret;
		*bytes = &dsm_buf[0];
	}

	/*
	 * Try to decode something with what we got
	 */
	પાછા dsm_parse(now, &dsm_buf[0], ret, કિંમતો_2, અંક_કિંમતો, dsm_11_bit, &dsm_frame_drops, rssi, max_કિંમતો);
}

તાર્કિક dsm_parse(અચલ uint64_t now, અચલ uint8_t *frame, અચલ unsigned len, uint16_t *કિંમતો_2,
	       uint16_t *અંક_કિંમતો, તાર્કિક *dsm_11_bit, unsigned *frame_drops, int8_t *rssi_percent, uint16_t max_channels)
{
	/* this is set by the decoding state machine and will default to false
	 * once everything that was decodable has been decoded.
	 */
	તાર્કિક channel_data_available = ખોટું;
	unsigned channel_output_ગણતરી;

	/* keep decoding until we have consumed the buffer */
	માટે (unsigned d = 0; d < len; d++) {

		/* overflow check */
		જો (dsm_partial_frame_ગણતરી == કદ(dsm_frame) / કદ(dsm_frame[0])) {
			dsm_partial_frame_ગણતરી = 0;
			dsm_decode_state = DSM_DECODE_STATE_DESYNC;
#ifdef DSM_DEBUG
			printf("DSM: RESET (BUF LIM)\n");
#endif
		}

		જો (dsm_partial_frame_ગણતરી == DSM_FRAME_SIZE) {
			dsm_partial_frame_ગણતરી = 0;
			dsm_decode_state = DSM_DECODE_STATE_DESYNC;
#ifdef DSM_DEBUG
			printf("DSM: RESET (PACKET LIM)\n");
#endif
		}

#ifdef DSM_DEBUG
#જો 0
		printf("dsm state: %s%s, count: %d, val: %02x\n",
		       (dsm_decode_state == DSM_DECODE_STATE_DESYNC) ? "DSM_DECODE_STATE_DESYNC" : "",
		       (dsm_decode_state == DSM_DECODE_STATE_SYNC) ? "DSM_DECODE_STATE_SYNC" : "",
		       dsm_partial_frame_ગણતરી,
		       (unsigned)frame[d]);
#endif
#endif

		પસંદગી (dsm_decode_state) {
		સ્થિતિ DSM_DECODE_STATE_DESYNC:

			/* we are de-synced and only interested in the frame marker */
			જો ((now - dsm_last_rx_time) > 5000) {
				dsm_decode_state = DSM_DECODE_STATE_SYNC;
				dsm_partial_frame_ગણતરી = 0;
				dsm_frame[dsm_partial_frame_ગણતરી++] = frame[d];
			}

			તોડો;

		સ્થિતિ DSM_DECODE_STATE_SYNC: {
				dsm_frame[dsm_partial_frame_ગણતરી++] = frame[d];

				/* decode whatever we got and expect */
				જો (dsm_partial_frame_ગણતરી < DSM_FRAME_SIZE) {
					તોડો;
				}

				// We've collected a full frame, parse it
				channel_data_available = dsm_decode(now, dsm_11_bit, dsm_chan_ગણતરી, rssi_percent);

				// Frame consumed, reset the buffer count
				dsm_partial_frame_ગણતરી = 0;

				/* if decoding failed, set proto to desync */
				જો (!channel_data_available) {
					dsm_decode_state = DSM_DECODE_STATE_DESYNC;
				}
			}
			તોડો;

		મૂળભૂત:
#ifdef DSM_DEBUG
			printf("UNKNOWN PROTO STATE");
#endif
			channel_data_available = ખોટું;
		}
	}

	જો (frame_drops) {
		*frame_drops = dsm_frame_drops;
	}

	જો (channel_data_available) {
		// Check if any channels have been absent for too long
		માટે (unsigned i = 0; i < dsm_chan_ગણતરી; i++) {
			જો (now - channel_buffer[i].last_seen > 50 * 1000) {
				પાછા ખોટું;
			}
		}

		// Overflow protect
		જો (max_channels >= DSM_MAX_CHANNEL_ગણતરી) {
			max_channels = DSM_MAX_CHANNEL_ગણતરી;
		}

		// Cap returned channels to max_channels if we've seen more
		જો (dsm_chan_ગણતરી > max_channels) {
			channel_output_ગણતરી = max_channels;

		} નહીંતર {
			channel_output_ગણતરી = dsm_chan_ગણતરી;
		}

		*અંક_કિંમતો = channel_output_ગણતરી;

		// Copy the full list of collected channels
		માટે (unsigned i = 0; i < channel_output_ગણતરી; i++) {
			// Channels which do not have a value yet (because their frame has not been seen yet)
			// will have a zero. If we come across a blank channel, keep storing values, but we
			// won't return that we have a valid frame just yet.
			જો (channel_buffer[i].value) {
				કિંમતો_2[i] = channel_buffer[i].value;

			} નહીંતર {
				channel_data_available = ખોટું;
			}
		}
	}

#ifdef DSM_DEBUG

	જો (channel_data_available) {
		printf("frame drops: %u, chan #: %u\n", dsm_frame_drops, dsm_chan_ગણતરી);

		માટે (unsigned i = 0; i < dsm_chan_ગણતરી; i++) {
			printf("dsm_decode: #CH %02u: %u\n", i + 1, કિંમતો_2[i]);
		}
	}

#endif

	dsm_last_rx_time = now;

	/* return false as default */
	પાછા channel_data_available;
}
