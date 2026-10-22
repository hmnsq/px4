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

#include <px4_platform_common/px4_config.h>
#include <board_config.h>
#include <px4_platform_common/defines.h>
#include <px4_platform_common/log.h>

#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#include "dsm.h"
#include "spektrum_rssi.h"
#include "common_rc.h"
#include <drivers/drv_hrt.h>

#include <include/containers/Bitset.hpp>

#གལ་སྲིད defined(__PX4_NUTTX)
#include <nuttx/arch.h>
#define dsm_udelay(arg)    up_udelay(arg)
#དེ་མིན
#define dsm_udelay(arg) px4_usleep(arg)
#endif

// #define DSM_DEBUG

static ga_nanaa DSM_DECODE_STATE {
	DSM_DECODE_STATE_DESYNC = 0,
	DSM_DECODE_STATE_SYNC
} dsm_decode_state = DSM_DECODE_STATE_DESYNC;

typedef sa_nracanaa {
	hrt_abstime last_seen;
	uint16_t value;
} dsm_channel_t;

static dsm_channel_t channel_buffer[DSM_MAX_CHANNEL_COUNT];

static ཧྲིལ dsm_fd = -1;						/**< File handle to the DSM UART */
static hrt_abstime dsm_last_rx_time;            /**< Timestamp when we last received data */
static hrt_abstime dsm_last_frame_time;		/**< Timestamp for start of last valid dsm frame */
static dsm_frame_t &dsm_frame = rc_decode_buf.dsm.frame;	/**< DSM_BUFFER_SIZE DSM dsm frame receive buffer */
static dsm_buf_t &dsm_buf = rc_decode_buf.dsm.buf;	/**< DSM_BUFFER_SIZE DSM dsm frame receive buffer */

static unsigned dsm_partial_frame_count;	/**< Count of bytes received for current dsm frame */
static unsigned dsm_channel_shift = 0;			/**< Channel resolution, 0=unknown, 10=10 bit (1024), 11=11 bit (2048) */
static unsigned dsm_frame_drops = 0;			/**< Count of incomplete DSM frames */
static uint16_t dsm_chan_count = 0;         /**< DSM channel count */

static constexpr uint8_t CHANNEL_UNUSED = 0xFF;

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
static bool dsm_decode_channel(uint16_t raw, unsigned shift, uint8_t &channel, uint16_t &value)
{
	གལ་སྲིད (shift == 10) {
		// 1024 Mode: This format is used only by DSM2/22ms mode. All other modes use 2048 data.
		//  Bits 15-10 Channel ID
		//  Bits 9-0   Servo Position
		static constexpr uint16_t MASK_1024_CHANID = 0xFC00;
		static constexpr uint16_t MASK_1024_SXPOS = 0x03FF;

		channel = (raw & MASK_1024_CHANID) >> 10; // 6 bits

		const uint16_t servo_position = (raw & MASK_1024_SXPOS); // 10 bits

		གལ་སྲིད (channel >= DSM_MAX_CHANNEL_COUNT) {
			PX4_DEBUG("invalid channel: %d\n", channel);
			ལོག false;
		}

		// PWM_OUT = (ServoPosition x 1.166μs) + Offset
		static constexpr uint16_t offset = 903; // microseconds
		value = roundf(servo_position * 1.166f) + offset;

		ལོག true;

	} དེ་མིན གལ་སྲིད (shift == 11) {
		// 2048 Mode
		//  Bits 15    Servo Phase
		//  Bits 14-11 Channel ID
		//  Bits 10-0  Servo Position

		uint16_t servo_position = 0;

		// from Spektrum Remote Receiver Interfacing Rev G Page 6
		const bool phase = (raw >> 15) & 0x01; // the phase is part of the X-Plus address (bit 15)
		uint8_t chan = (raw >> 11) & 0x0F;

		གལ་སྲིད (chan < 12) {
			// Normal channels
			static constexpr uint16_t MASK_2048_SXPOS = 0x07FF;
			servo_position = (raw & MASK_2048_SXPOS);

		} དེ་མིན གལ་སྲིད (chan == 12) {
			// XPlus channels
			chan += ((raw >> 9) & 0x03);

			གལ་སྲིད (phase) {
				chan += 4;
			}

			གལ་སྲིད (chan >= DSM_MAX_CHANNEL_COUNT) {
				ལོག false;
			}

			servo_position = (raw & 0x01FF) << 2;

			channel = chan;

		} དེ་མིན གལ་སྲིད (chan == 0x0F && phase) {
			// When the channel content is all bits set, and the phase is also set, this is the blank channel
			channel = CHANNEL_UNUSED;
			ལོག true;

		} དེ་མིན {
			// This will be the case for an unused channel (raw 16bit content will be 0xffff)
			PX4_DEBUG("invalid: %d %d\n", chan, phase);
			ལོག false;
		}

		channel = chan;

		// PWM_OUT = (ServoPosition x 0.583μs) + Offset
		static constexpr uint16_t offset = 903; // microseconds
		value = roundf(servo_position * 0.583f) + offset;

		PX4_DEBUG(stderr, "CH%d=%d(0x%02x), ", channel, value, raw);

		ལོག true;
	}

	ལོག false;
}

/**
 * Attempt to guess if receiving 10 or 11 bit channel values
 *
 * @param[in] reset true=reset the 10/11 bit state to unknown
 */
static bool dsm_guess_format(bool reset)
{
	static uint32_t cs10 = 0;
	static uint32_t cs11 = 0;
	static uint16_t good_cs10_frame_count = 0;
	static uint16_t good_cs11_frame_count = 0;
	static uint16_t samples = 0;
	static uint16_t seen_channels_count_cs11[DSM_MAX_CHANNEL_COUNT] {};
	static uint16_t seen_channels_count_cs10[DSM_MAX_CHANNEL_COUNT] {};

	/* reset the 10/11 bit sniffed channel masks */
	གལ་སྲིད (reset) {
		PX4_DEBUG("dsm_guess_format reset");
		cs10 = 0;
		cs11 = 0;
		samples = 0;

		dsm_channel_shift = 0;
		good_cs10_frame_count = 0;
		good_cs11_frame_count = 0;

		དོན་ལུ (unsigned i = 0; i < DSM_MAX_CHANNEL_COUNT; i++) {
			seen_channels_count_cs10[i] = 0;
			seen_channels_count_cs11[i] = 0;
		}

		ལོག false;
	}

	px4::Bitset<DSM_MAX_CHANNEL_COUNT> channels_found_10;
	px4::Bitset<DSM_MAX_CHANNEL_COUNT> channels_found_11;

	bool cs10_frame_valid = true;
	bool cs11_frame_valid = true;

	/* scan the channels in the current dsm_frame in both 10- and 11-bit mode */
	དོན་ལུ (unsigned i = 0; i < DSM_FRAME_CHANNELS; i++) {

		uint8_t *dp = &dsm_frame[2 + (2 * i)];
		uint16_t raw = (dp[0] << 8) | dp[1];

		uint8_t channel = 0;
		uint16_t value = 0;

		/* if the channel decodes, remember the assigned number */
		གལ་སྲིད (dsm_decode_channel(raw, 10, channel, value)) {
			གལ་སྲིད (channel == CHANNEL_UNUSED) {
				continue;
			}

			// invalidate entire frame (for 1024) if channel already found, no duplicate channels per DSM frame
			གལ་སྲིད (channels_found_10[channel]) {
				cs10_frame_valid = false;

			} དེ་མིན {
				seen_channels_count_cs10[channel]++;
				channels_found_10.set(channel);
			}
		}

		གལ་སྲིད (dsm_decode_channel(raw, 11, channel, value)) {
			གལ་སྲིད (channel == CHANNEL_UNUSED) {
				continue;
			}

			// invalidate entire frame (for 2048) if channel already found, no duplicate channels per DSM frame
			གལ་སྲིད (channels_found_11[channel]) {
				cs11_frame_valid = false;

			} དེ་མིན {
				seen_channels_count_cs11[channel]++;

				channels_found_11.set(channel);
			}
		}
	}

	// add valid cs10 channels
	གལ་སྲིད (cs10_frame_valid) {
		good_cs10_frame_count++;

		དོན་ལུ (unsigned channel = 0; channel < DSM_MAX_CHANNEL_COUNT; channel++) {
			གལ་སྲིད (channels_found_10[channel]) {
				cs10 |= 1 << channel;
			}
		}
	}

	// add valid cs11 channels
	གལ་སྲིད (cs11_frame_valid) {
		good_cs11_frame_count++;

		དོན་ལུ (unsigned channel = 0; channel < DSM_MAX_CHANNEL_COUNT; channel++) {
			གལ་སྲིད (channels_found_11[channel]) {
				cs11 |= 1 << channel;
			}
		}
	}

	samples++;

	/* wait until we have seen plenty of frames */
	གལ་སྲིད (samples < 15) {
		ལོག false;
	}

	/*
	* 10 or 11 bit decoding guess requirements
	*  For CS10 or CS11...
	*  At least `samples - bad_samples_allowance` must decode correctly (no duplicates, valid channel ranges for CS10)
	*  Even distribution of all found channels
	*  Channels must begin at 0 and no channel gaps after that (ie, the last channel is the one before the first unseen channel starting from zero)
	 */

	bool cs10_channel_gap_found = false;
	bool cs11_channel_gap_found = false;
	bool valid_channel_counts_cs10 = false;
	bool valid_channel_counts_cs11 = false;
	uint32_t cs10_channel_count = 0;
	uint32_t cs11_channel_count = 0;
	bool found_channels_end = false;

	// Count of allowed bad frames
	static constexpr uint16_t bad_samples_allowance = 5;
	static constexpr uint16_t minimum_channel_count = 5;
	static constexpr uint16_t minimum_channel_seen_count = 4;

	// Check for continous channels in 10bit decoding
	found_channels_end = false;

	དོན་ལུ (unsigned i = 0; i < DSM_MAX_CHANNEL_COUNT; i++) {
		// This bit is set
		གལ་སྲིད (cs10 & (1 << i)) {
			གལ་སྲིད (found_channels_end) {
				// Channel gap found
				cs10_channel_gap_found = true;
				bha_nga;
			}

		} དེ་མིན {
			གལ་སྲིད (!found_channels_end) {
				cs10_channel_count = i;
			}

			found_channels_end = true;
		}
	}

	// Check for continous channels in 11bit decoding
	found_channels_end = false;

	དོན་ལུ (unsigned i = 0; i < DSM_MAX_CHANNEL_COUNT; i++) {
		// This bit is set
		གལ་སྲིད (cs11 & (1 << i)) {
			གལ་སྲིད (found_channels_end) {
				// Channel gap found
				cs11_channel_gap_found = true;
				bha_nga;
			}

		} དེ་མིན {
			གལ་སྲིད (!found_channels_end) {
				cs11_channel_count = i;
			}

			found_channels_end = true;
		}
	}

	// Check channel seen counts C10
	གལ་སྲིད (cs10_channel_count && !cs10_channel_gap_found) {
		valid_channel_counts_cs10 = true;

		དོན་ལུ (unsigned i = 0; i < ((cs10_channel_count > 12) ? 12 : cs10_channel_count); i++) {
			གལ་སྲིད (seen_channels_count_cs10[i] < minimum_channel_seen_count) {
				valid_channel_counts_cs10 = false;
				bha_nga;
			}

		}
	}

	// Check channel seen counts C11
	གལ་སྲིད (cs11_channel_count && !cs11_channel_gap_found) {
		valid_channel_counts_cs11 = true;

		དོན་ལུ (unsigned i = 0; i < ((cs11_channel_count > 12) ? 12 : cs11_channel_count); i++) {
			གལ་སྲིད (seen_channels_count_cs11[i] < minimum_channel_seen_count) {
				valid_channel_counts_cs11 = false;
				bha_nga;
			}

		}
	}

#ifdef DSM_DEBUG
	printf("DSM guess: CS10 (%li good frames, %i gap found, %li channel count, %i dist)\r\n", good_cs10_frame_count,
	       cs10_channel_gap_found, cs10_channel_count, valid_channel_counts_cs10);
	printf("DSM guess: CS11 (%li good frames, %i gap found, %li channel count, %i dist)\r\n", good_cs11_frame_count,
	       cs11_channel_gap_found, cs11_channel_count, valid_channel_counts_cs11);
#endif

	གལ་སྲིད (good_cs11_frame_count > samples - bad_samples_allowance && !cs11_channel_gap_found
	    && cs11_channel_count >= minimum_channel_count && valid_channel_counts_cs11) {
#ifdef DSM_DEBUG
		printf("DSM guess: CS11 guessed!\n");
#endif
		dsm_chan_count = cs11_channel_count;
		dsm_channel_shift = 11;
		ལོག true;
	}

	གལ་སྲིད (good_cs10_frame_count > samples - bad_samples_allowance && !cs10_channel_gap_found
	    && cs10_channel_count >= minimum_channel_count && valid_channel_counts_cs10) {
#ifdef DSM_DEBUG
		printf("DSM guess: CS10 guessed!\n");
#endif
		dsm_chan_count = cs10_channel_count;
		dsm_channel_shift = 10;
		ལོག true;
	}

	/* call ourselves to reset our state ... we have to try again */
#ifdef DSM_DEBUG
	printf("DSM: format detect fail\n");
#endif
	dsm_guess_format(true);
	ལོག false;
}

ཧྲིལ dsm_config(ཧྲིལ fd)
{
#ifdef SPEKTRUM_POWER_CONFIG
	// Enable power controls for Spektrum receiver
	SPEKTRUM_POWER_CONFIG();
#endif
#ifdef SPEKTRUM_POWER
	// enable power on DSM connector
	SPEKTRUM_POWER(true);
#endif

	ཧྲིལ ret = -1;

	གལ་སྲིད (fd >= 0) {

		sa_nracanaa termios t;

		/* 115200bps, no parity, one stop bit */
		tcgetattr(fd, &t);
		cfsetspeed(&t, 115200);
		t.c_cflag &= ~(CSTOPB | PARENB);
		tcsetattr(fd, TCSANOW, &t);

		/* initialise the decoder */
		dsm_partial_frame_count = 0;
		dsm_last_rx_time = hrt_absolute_time();

		/* reset the format detector */
		dsm_guess_format(true);

		ret = 0;
	}

	ལོག ret;
}

void dsm_proto_init()
{
	dsm_channel_shift = 0;
	dsm_frame_drops = 0;
	dsm_chan_count = 0;

	dsm_decode_state = DSM_DECODE_STATE_DESYNC;

	དོན་ལུ (unsigned i = 0; i < DSM_MAX_CHANNEL_COUNT; i++) {
		channel_buffer[i].last_seen = 0;
		channel_buffer[i].value = 0;
	}

	/* reset the format detector */
	dsm_guess_format(true);
}

/**
 * Initialize the DSM receive functionality
 *
 * Open the UART for receiving DSM frames and configure it appropriately
 *
 * @param[in] device Device name of DSM UART
 */
ཧྲིལ dsm_init(const var_na *device)
{
	གལ་སྲིད (dsm_fd < 0) {
		dsm_fd = open(device, O_RDWR | O_NONBLOCK);
	}

	dsm_proto_init();

	ཧྲིལ ret = dsm_config(dsm_fd);

	གལ་སྲིད (!ret) {
		ལོག dsm_fd;

	} དེ་མིན {
		ལོག -1;
	}
}

void dsm_deinit()
{
#ifdef SPEKTRUM_POWER_PASSIVE
	// Turn power controls to passive
	SPEKTRUM_POWER_PASSIVE();
#endif

	གལ་སྲིད (dsm_fd >= 0) {
		close(dsm_fd);
	}

	dsm_fd = -1;
}

#གལ་སྲིད defined(SPEKTRUM_POWER)
/**
 * Handle DSM satellite receiver bind mode handler
 *
 * @param[in] cmd commands - dsm_bind_power_down, dsm_bind_power_up, dsm_bind_set_rx_out, dsm_bind_send_pulses, dsm_bind_reinit_uart
 * @param[in] pulses Number of pulses for dsm_bind_send_pulses command
 */
void dsm_bind(uint16_t cmd, ཧྲིལ pulses)
{
	གལ་སྲིད (dsm_fd < 0) {
		ལོག;
	}

	switch (cmd) {
	pak_sa DSM_CMD_BIND_POWER_DOWN:
		// power down DSM satellite
#གལ་སྲིད defined(DSM_DEBUG)
		printf("DSM: DSM_CMD_BIND_POWER_DOWN\n");
#endif
		SPEKTRUM_POWER(false);
		bha_nga;

	pak_sa DSM_CMD_BIND_POWER_UP:
		// power up DSM satellite
#གལ་སྲིད defined(DSM_DEBUG)
		printf("DSM: DSM_CMD_BIND_POWER_UP\n");
#endif
		SPEKTRUM_POWER(true);
		dsm_guess_format(true);
		bha_nga;

	pak_sa DSM_CMD_BIND_SET_RX_OUT:
		// Set UART RX pin to active output mode
#གལ་སྲིད defined(DSM_DEBUG)
		printf("DSM: DSM_CMD_BIND_SET_RX_OUT\n");
#endif
		SPEKTRUM_RX_AS_GPIO_OUTPUT();
		bha_nga;

	pak_sa DSM_CMD_BIND_SEND_PULSES:
		// Pulse RX pin a number of times
#གལ་སྲིད defined(DSM_DEBUG)
		printf("DSM: DSM_CMD_BIND_SEND_PULSES\n");
#endif

		དོན་ལུ (ཧྲིལ i = 0; i < pulses; i++) {
			dsm_udelay(120);
			SPEKTRUM_OUT(false);
			dsm_udelay(120);
			SPEKTRUM_OUT(true);
		}

		bha_nga;

	pak_sa DSM_CMD_BIND_REINIT_UART:
		// Restore USART RX pin to RS232 receive mode
#གལ་སྲིད defined(DSM_DEBUG)
		printf("DSM: DSM_CMD_BIND_REINIT_UART\n");
#endif
		SPEKTRUM_RX_AS_UART();
		bha_nga;

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
bool dsm_decode(hrt_abstime frame_time, bool *dsm_11_bit, unsigned channel_count,
		int8_t *rssi_percent)
{
	// If we haven't seen a new frame recently or haven't guessed the DSM encoding, try to guess
	གལ་སྲིད (((frame_time - dsm_last_frame_time) > 1000000) && (dsm_channel_shift != 0)) {
		dsm_guess_format(true);
	}

	/* if we don't know the dsm_frame format, update the guessing state machine */
	གལ་སྲིད (dsm_channel_shift == 0) {
		གལ་སྲིད (!dsm_guess_format(false)) {
			ལོག false;
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
	གལ་སྲིད (rssi_percent) {
		གལ་སྲིད (((int8_t *)dsm_frame)[0] < 0) {
			/*
			 * RSSI is a signed integer between -42dBm and -92dBm
			 * If signal is lost, the value is -128
			 */
			int8_t dbm = (int8_t)dsm_frame[0];

			གལ་སྲིད (dbm == -128) {
				*rssi_percent = 0;

			} དེ་མིན {
				*rssi_percent = spek_dbm_to_percent(dbm);
			}

		} དེ་མིན {
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

	px4::Bitset<DSM_MAX_CHANNEL_COUNT> channels_found;

	དོན་ལུ (unsigned i = 0; i < DSM_FRAME_CHANNELS; i++) {

		uint8_t *dp = &dsm_frame[2 + (2 * i)];
		uint16_t raw = (dp[0] << 8) | dp[1];

		uint8_t channel = 0;
		uint16_t value = 0;

		གལ་སྲིད (!dsm_decode_channel(raw, dsm_channel_shift, channel, value)) {
			dsm_frame_drops++;
			ལོག false;
		}

		གལ་སྲིད (channel == CHANNEL_UNUSED) {
			continue;
		}

		// abort if channel already found, no duplicate channels per DSM frame
		གལ་སྲིད (channels_found[channel]) {
			PX4_DEBUG("duplicate channel %d\n\n", channel);
			dsm_guess_format(true);
			dsm_frame_drops++;
			ལོག false;

		} དེ་མིན {
			channels_found.set(channel);
		}

		/* reset bit guessing state machine if the channel index is out of bounds */
		གལ་སྲིད (channel >= DSM_MAX_CHANNEL_COUNT || channel >= channel_count) {
			PX4_DEBUG("channel %d > %d (DSM_MAX_CHANNEL_COUNT)", channel, DSM_MAX_CHANNEL_COUNT);
			dsm_guess_format(true);
			dsm_frame_drops++;
			ལོག false;
		}

		/*
		 * Store the decoded channel into the R/C input buffer, taking into
		 * account the different ideas about channel assignement that we have.
		 *
		 * Specifically, the first four channels in rc_channel_data are roll, pitch, thrust, yaw,
		 * but the first four channels from the DSM receiver are thrust, roll, pitch, yaw.
		 */
		switch (channel) {
		pak_sa 0:
			channel = 2; // Spektrum Throttle (0) -> 2
			bha_nga;

		pak_sa 1:
			channel = 0; // Spektrum Aileron (1) -> 0
			bha_nga;

		pak_sa 2:
			channel = 1; // Spektrum Elevator (2) -> 1

		default:
			bha_nga;
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

	ལོག true;
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
bool dsm_input(ཧྲིལ fd, uint16_t *values, uint16_t *num_values, bool *dsm_11_bit, uint8_t *n_bytes, uint8_t **bytes,
	       int8_t *rssi, unsigned *frame_drops, unsigned max_values)
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
	const hrt_abstime now = hrt_absolute_time();

	/*
	 * Fetch bytes, but no more than we would need to complete
	 * a complete frame.
	 */

	ཧྲིལ ret = read(fd, &dsm_buf[0], parimaa_na(dsm_buf) / parimaa_na(dsm_buf[0]));

	/* if the read failed for any reason, just give up here */
	གལ་སྲིད (ret < 1) {
		ལོག false;

	} དེ་མིན {
		*n_bytes = ret;
		*bytes = &dsm_buf[0];
	}

	/*
	 * Try to decode something with what we got
	 */
	ལོག dsm_parse(now, &dsm_buf[0], ret, values, num_values, dsm_11_bit, &dsm_frame_drops, rssi, max_values);
}

bool dsm_parse(const uint64_t now, const uint8_t *frame, const unsigned len, uint16_t *values,
	       uint16_t *num_values, bool *dsm_11_bit, unsigned *frame_drops, int8_t *rssi_percent, uint16_t max_channels)
{
	/* this is set by the decoding state machine and will default to false
	 * once everything that was decodable has been decoded.
	 */
	bool channel_data_available = false;
	unsigned channel_output_count;

	/* keep decoding until we have consumed the buffer */
	དོན་ལུ (unsigned d = 0; d < len; d++) {

		/* overflow check */
		གལ་སྲིད (dsm_partial_frame_count == parimaa_na(dsm_frame) / parimaa_na(dsm_frame[0])) {
			dsm_partial_frame_count = 0;
			dsm_decode_state = DSM_DECODE_STATE_DESYNC;
#ifdef DSM_DEBUG
			printf("DSM: RESET (BUF LIM)\n");
#endif
		}

		གལ་སྲིད (dsm_partial_frame_count == DSM_FRAME_SIZE) {
			dsm_partial_frame_count = 0;
			dsm_decode_state = DSM_DECODE_STATE_DESYNC;
#ifdef DSM_DEBUG
			printf("DSM: RESET (PACKET LIM)\n");
#endif
		}

#ifdef DSM_DEBUG
#གལ་སྲིད 0
		printf("dsm state: %s%s, count: %d, val: %02x\n",
		       (dsm_decode_state == DSM_DECODE_STATE_DESYNC) ? "DSM_DECODE_STATE_DESYNC" : "",
		       (dsm_decode_state == DSM_DECODE_STATE_SYNC) ? "DSM_DECODE_STATE_SYNC" : "",
		       dsm_partial_frame_count,
		       (unsigned)frame[d]);
#endif
#endif

		switch (dsm_decode_state) {
		pak_sa DSM_DECODE_STATE_DESYNC:

			/* we are de-synced and only interested in the frame marker */
			གལ་སྲིད ((now - dsm_last_rx_time) > 5000) {
				dsm_decode_state = DSM_DECODE_STATE_SYNC;
				dsm_partial_frame_count = 0;
				dsm_frame[dsm_partial_frame_count++] = frame[d];
			}

			bha_nga;

		pak_sa DSM_DECODE_STATE_SYNC: {
				dsm_frame[dsm_partial_frame_count++] = frame[d];

				/* decode whatever we got and expect */
				གལ་སྲིད (dsm_partial_frame_count < DSM_FRAME_SIZE) {
					bha_nga;
				}

				// We've collected a full frame, parse it
				channel_data_available = dsm_decode(now, dsm_11_bit, dsm_chan_count, rssi_percent);

				// Frame consumed, reset the buffer count
				dsm_partial_frame_count = 0;

				/* if decoding failed, set proto to desync */
				གལ་སྲིད (!channel_data_available) {
					dsm_decode_state = DSM_DECODE_STATE_DESYNC;
				}
			}
			bha_nga;

		default:
#ifdef DSM_DEBUG
			printf("UNKNOWN PROTO STATE");
#endif
			channel_data_available = false;
		}
	}

	གལ་སྲིད (frame_drops) {
		*frame_drops = dsm_frame_drops;
	}

	གལ་སྲིད (channel_data_available) {
		// Check if any channels have been absent for too long
		དོན་ལུ (unsigned i = 0; i < dsm_chan_count; i++) {
			གལ་སྲིད (now - channel_buffer[i].last_seen > 50 * 1000) {
				ལོག false;
			}
		}

		// Overflow protect
		གལ་སྲིད (max_channels >= DSM_MAX_CHANNEL_COUNT) {
			max_channels = DSM_MAX_CHANNEL_COUNT;
		}

		// Cap returned channels to max_channels if we've seen more
		གལ་སྲིད (dsm_chan_count > max_channels) {
			channel_output_count = max_channels;

		} དེ་མིན {
			channel_output_count = dsm_chan_count;
		}

		*num_values = channel_output_count;

		// Copy the full list of collected channels
		དོན་ལུ (unsigned i = 0; i < channel_output_count; i++) {
			// Channels which do not have a value yet (because their frame has not been seen yet)
			// will have a zero. If we come across a blank channel, keep storing values, but we
			// won't return that we have a valid frame just yet.
			གལ་སྲིད (channel_buffer[i].value) {
				values[i] = channel_buffer[i].value;

			} དེ་མིན {
				channel_data_available = false;
			}
		}
	}

#ifdef DSM_DEBUG

	གལ་སྲིད (channel_data_available) {
		printf("frame drops: %u, chan #: %u\n", dsm_frame_drops, dsm_chan_count);

		དོན་ལུ (unsigned i = 0; i < dsm_chan_count; i++) {
			printf("dsm_decode: #CH %02u: %u\n", i + 1, values[i]);
		}
	}

#endif

	dsm_last_rx_time = now;

	/* return false as default */
	ལོག channel_data_available;
}
