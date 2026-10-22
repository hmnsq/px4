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

#ضمن <px4_platform_common/px4_config.h>
#ضمن <board_config.h>
#ضمن <px4_platform_common/defines.h>
#ضمن <px4_platform_common/log.h>

#ضمن <fcntl.h>
#ضمن <math.h>
#ضمن <unistd.h>
#ضمن <termios.h>
#ضمن <string.h>

#ضمن "dsm.h"
#ضمن "spektrum_rssi.h"
#ضمن "common_rc.h"
#ضمن <drivers/drv_hrt.h>

#ضمن <include/containers/Bitset.hpp>

#إذا defined(__PX4_NUTTX)
#ضمن <nuttx/arch.h>
#وصف_ڪريو dsm_udelay(arg)    مٿي_udelay(arg)
#وإلا
#وصف_ڪريو dsm_udelay(arg) px4_usleep(arg)
#جيڪڏهن_پورو

// #define DSM_DEBUG

ساكن تعداد DSM_DECODE_STATE {
	DSM_DECODE_STATE_DESYNC = 0,
	DSM_DECODE_STATE_SYNC
} dsm_decode_state = DSM_DECODE_STATE_DESYNC;

تعريفنوع بنية {
	hrt_abstime آخري_ڏٺل;
	uint16_t value;
} dsm_channel_t;

ساكن dsm_channel_t channel_بفر[DSM_MAX_CHANNEL_ڳڻپ];

ساكن صحيح dsm_fd = -1;						/**< File handle to the DSM UART */
ساكن hrt_abstime dsm_آخري_rx_وقت;            /**< Timestamp when we last received data */
ساكن hrt_abstime dsm_آخري_چوکٺو_وقت;		/**< Timestamp for start of last valid dsm frame */
ساكن dsm_چوکٺو_t &dsm_چوکٺو = rc_decode_buf.dsm.چوکٺو;	/**< DSM_BUFFER_SIZE DSM dsm frame receive buffer */
ساكن dsm_buf_t &dsm_buf = rc_decode_buf.dsm.buf;	/**< DSM_BUFFER_SIZE DSM dsm frame receive buffer */

ساكن بنانشان dsm_partial_چوکٺو_ڳڻپ;	/**< Count of bytes received for current dsm frame */
ساكن بنانشان dsm_channel_سرڪايو = 0;			/**< Channel resolution, 0=unknown, 10=10 bit (1024), 11=11 bit (2048) */
ساكن بنانشان dsm_چوکٺو_drops = 0;			/**< Count of incomplete DSM frames */
ساكن uint16_t dsm_chan_ڳڻپ = 0;         /**< DSM channel count */

ساكن constexpr uint8_t CHANNEL_اڻ_استعمال_ٿيل = 0xFF;

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
ساكن منطقي dsm_decode_channel(uint16_t raw, بنانشان سرڪايو, uint8_t &channel, uint16_t &value)
{
	إذا (سرڪايو == 10) {
		// 1024 Mode: This format is used only by DSM2/22ms mode. All other modes use 2048 data.
		//  Bits 15-10 Channel ID
		//  Bits 9-0   Servo Position
		ساكن constexpr uint16_t نقاب_1024_CHANID = 0xFC00;
		ساكن constexpr uint16_t نقاب_1024_SXPOS = 0x03FF;

		channel = (raw & نقاب_1024_CHANID) >> 10; // 6 bits

		ثابت uint16_t servo_position = (raw & نقاب_1024_SXPOS); // 10 bits

		إذا (channel >= DSM_MAX_CHANNEL_ڳڻپ) {
			PX4_DEBUG("invalid channel: %d\n", channel);
			أرجع خطأ;
		}

		// PWM_OUT = (ServoPosition x 1.166μs) + Offset
		ساكن constexpr uint16_t وٿي = 903; // microseconds
		value = roundf(servo_position * 1.166f) + وٿي;

		أرجع سچ;

	} وإلا إذا (سرڪايو == 11) {
		// 2048 Mode
		//  Bits 15    Servo Phase
		//  Bits 14-11 Channel ID
		//  Bits 10-0  Servo Position

		uint16_t servo_position = 0;

		// from Spektrum Remote Receiver Interfacing Rev G Page 6
		ثابت منطقي phase = (raw >> 15) & 0x01; // the phase is part of the X-Plus address (bit 15)
		uint8_t chan = (raw >> 11) & 0x0F;

		إذا (chan < 12) {
			// Normal channels
			ساكن constexpr uint16_t نقاب_2048_SXPOS = 0x07FF;
			servo_position = (raw & نقاب_2048_SXPOS);

		} وإلا إذا (chan == 12) {
			// XPlus channels
			chan += ((raw >> 9) & 0x03);

			إذا (phase) {
				chan += 4;
			}

			إذا (chan >= DSM_MAX_CHANNEL_ڳڻپ) {
				أرجع خطأ;
			}

			servo_position = (raw & 0x01FF) << 2;

			channel = chan;

		} وإلا إذا (chan == 0x0F && phase) {
			// When the channel content is all bits set, and the phase is also set, this is the blank channel
			channel = CHANNEL_اڻ_استعمال_ٿيل;
			أرجع سچ;

		} وإلا {
			// This will be the case for an unused channel (raw 16bit content will be 0xffff)
			PX4_DEBUG("invalid: %d %d\n", chan, phase);
			أرجع خطأ;
		}

		channel = chan;

		// PWM_OUT = (ServoPosition x 0.583μs) + Offset
		ساكن constexpr uint16_t وٿي = 903; // microseconds
		value = roundf(servo_position * 0.583f) + وٿي;

		PX4_DEBUG(stderr, "CH%d=%d(0x%02x), ", channel, value, raw);

		أرجع سچ;
	}

	أرجع خطأ;
}

/**
 * Attempt to guess if receiving 10 or 11 bit channel values
 *
 * @param[in] reset true=reset the 10/11 bit state to unknown
 */
ساكن منطقي dsm_guess_format(منطقي reset)
{
	ساكن uint32_t cs10 = 0;
	ساكن uint32_t cs11 = 0;
	ساكن uint16_t good_cs10_چوکٺو_ڳڻپ = 0;
	ساكن uint16_t good_cs11_چوکٺو_ڳڻپ = 0;
	ساكن uint16_t samples = 0;
	ساكن uint16_t ڏٺل_channels_ڳڻپ_cs11[DSM_MAX_CHANNEL_ڳڻپ] {};
	ساكن uint16_t ڏٺل_channels_ڳڻپ_cs10[DSM_MAX_CHANNEL_ڳڻپ] {};

	/* reset the 10/11 bit sniffed channel masks */
	إذا (reset) {
		PX4_DEBUG("dsm_guess_format reset");
		cs10 = 0;
		cs11 = 0;
		samples = 0;

		dsm_channel_سرڪايو = 0;
		good_cs10_چوکٺو_ڳڻپ = 0;
		good_cs11_چوکٺو_ڳڻپ = 0;

		لكل (بنانشان i = 0; i < DSM_MAX_CHANNEL_ڳڻپ; i++) {
			ڏٺل_channels_ڳڻپ_cs10[i] = 0;
			ڏٺل_channels_ڳڻپ_cs11[i] = 0;
		}

		أرجع خطأ;
	}

	px4::Bitset<DSM_MAX_CHANNEL_ڳڻپ> channels_مليو_10;
	px4::Bitset<DSM_MAX_CHANNEL_ڳڻپ> channels_مليو_11;

	منطقي cs10_چوکٺو_valid = سچ;
	منطقي cs11_چوکٺو_valid = سچ;

	/* scan the channels in the current dsm_frame in both 10- and 11-bit mode */
	لكل (بنانشان i = 0; i < DSM_چوکٺو_CHANNELS; i++) {

		uint8_t *dp = &dsm_چوکٺو[2 + (2 * i)];
		uint16_t raw = (dp[0] << 8) | dp[1];

		uint8_t channel = 0;
		uint16_t value = 0;

		/* if the channel decodes, remember the assigned number */
		إذا (dsm_decode_channel(raw, 10, channel, value)) {
			إذا (channel == CHANNEL_اڻ_استعمال_ٿيل) {
				استمر;
			}

			// invalidate entire frame (for 1024) if channel already found, no duplicate channels per DSM frame
			إذا (channels_مليو_10[channel]) {
				cs10_چوکٺو_valid = خطأ;

			} وإلا {
				ڏٺل_channels_ڳڻپ_cs10[channel]++;
				channels_مليو_10.سيٽ(channel);
			}
		}

		إذا (dsm_decode_channel(raw, 11, channel, value)) {
			إذا (channel == CHANNEL_اڻ_استعمال_ٿيل) {
				استمر;
			}

			// invalidate entire frame (for 2048) if channel already found, no duplicate channels per DSM frame
			إذا (channels_مليو_11[channel]) {
				cs11_چوکٺو_valid = خطأ;

			} وإلا {
				ڏٺل_channels_ڳڻپ_cs11[channel]++;

				channels_مليو_11.سيٽ(channel);
			}
		}
	}

	// add valid cs10 channels
	إذا (cs10_چوکٺو_valid) {
		good_cs10_چوکٺو_ڳڻپ++;

		لكل (بنانشان channel = 0; channel < DSM_MAX_CHANNEL_ڳڻپ; channel++) {
			إذا (channels_مليو_10[channel]) {
				cs10 |= 1 << channel;
			}
		}
	}

	// add valid cs11 channels
	إذا (cs11_چوکٺو_valid) {
		good_cs11_چوکٺو_ڳڻپ++;

		لكل (بنانشان channel = 0; channel < DSM_MAX_CHANNEL_ڳڻپ; channel++) {
			إذا (channels_مليو_11[channel]) {
				cs11 |= 1 << channel;
			}
		}
	}

	samples++;

	/* wait until we have seen plenty of frames */
	إذا (samples < 15) {
		أرجع خطأ;
	}

	/*
	* 10 or 11 bit decoding guess requirements
	*  For CS10 or CS11...
	*  At least `samples - bad_samples_allowance` must decode correctly (no duplicates, valid channel ranges for CS10)
	*  Even distribution of all found channels
	*  Channels must begin at 0 and no channel gaps after that (ie, the last channel is the one before the first unseen channel starting from zero)
	 */

	منطقي cs10_channel_gap_مليو = خطأ;
	منطقي cs11_channel_gap_مليو = خطأ;
	منطقي valid_channel_counts_cs10 = خطأ;
	منطقي valid_channel_counts_cs11 = خطأ;
	uint32_t cs10_channel_ڳڻپ = 0;
	uint32_t cs11_channel_ڳڻپ = 0;
	منطقي مليو_channels_پڇاڙي = خطأ;

	// Count of allowed bad frames
	ساكن constexpr uint16_t bad_samples_allowance = 5;
	ساكن constexpr uint16_t minimum_channel_ڳڻپ = 5;
	ساكن constexpr uint16_t minimum_channel_ڏٺل_ڳڻپ = 4;

	// Check for continous channels in 10bit decoding
	مليو_channels_پڇاڙي = خطأ;

	لكل (بنانشان i = 0; i < DSM_MAX_CHANNEL_ڳڻپ; i++) {
		// This bit is set
		إذا (cs10 & (1 << i)) {
			إذا (مليو_channels_پڇاڙي) {
				// Channel gap found
				cs10_channel_gap_مليو = سچ;
				اكسر;
			}

		} وإلا {
			إذا (!مليو_channels_پڇاڙي) {
				cs10_channel_ڳڻپ = i;
			}

			مليو_channels_پڇاڙي = سچ;
		}
	}

	// Check for continous channels in 11bit decoding
	مليو_channels_پڇاڙي = خطأ;

	لكل (بنانشان i = 0; i < DSM_MAX_CHANNEL_ڳڻپ; i++) {
		// This bit is set
		إذا (cs11 & (1 << i)) {
			إذا (مليو_channels_پڇاڙي) {
				// Channel gap found
				cs11_channel_gap_مليو = سچ;
				اكسر;
			}

		} وإلا {
			إذا (!مليو_channels_پڇاڙي) {
				cs11_channel_ڳڻپ = i;
			}

			مليو_channels_پڇاڙي = سچ;
		}
	}

	// Check channel seen counts C10
	إذا (cs10_channel_ڳڻپ && !cs10_channel_gap_مليو) {
		valid_channel_counts_cs10 = سچ;

		لكل (بنانشان i = 0; i < ((cs10_channel_ڳڻپ > 12) ? 12 : cs10_channel_ڳڻپ); i++) {
			إذا (ڏٺل_channels_ڳڻپ_cs10[i] < minimum_channel_ڏٺل_ڳڻپ) {
				valid_channel_counts_cs10 = خطأ;
				اكسر;
			}

		}
	}

	// Check channel seen counts C11
	إذا (cs11_channel_ڳڻپ && !cs11_channel_gap_مليو) {
		valid_channel_counts_cs11 = سچ;

		لكل (بنانشان i = 0; i < ((cs11_channel_ڳڻپ > 12) ? 12 : cs11_channel_ڳڻپ); i++) {
			إذا (ڏٺل_channels_ڳڻپ_cs11[i] < minimum_channel_ڏٺل_ڳڻپ) {
				valid_channel_counts_cs11 = خطأ;
				اكسر;
			}

		}
	}

#جيڪڏهن_وصف_ٿيل DSM_DEBUG
	لکو("DSM guess: CS10 (%li good frames, %i gap found, %li channel count, %i dist)\r\n", good_cs10_چوکٺو_ڳڻپ,
	       cs10_channel_gap_مليو, cs10_channel_ڳڻپ, valid_channel_counts_cs10);
	لکو("DSM guess: CS11 (%li good frames, %i gap found, %li channel count, %i dist)\r\n", good_cs11_چوکٺو_ڳڻپ,
	       cs11_channel_gap_مليو, cs11_channel_ڳڻپ, valid_channel_counts_cs11);
#جيڪڏهن_پورو

	إذا (good_cs11_چوکٺو_ڳڻپ > samples - bad_samples_allowance && !cs11_channel_gap_مليو
	    && cs11_channel_ڳڻپ >= minimum_channel_ڳڻپ && valid_channel_counts_cs11) {
#جيڪڏهن_وصف_ٿيل DSM_DEBUG
		لکو("DSM guess: CS11 guessed!\n");
#جيڪڏهن_پورو
		dsm_chan_ڳڻپ = cs11_channel_ڳڻپ;
		dsm_channel_سرڪايو = 11;
		أرجع سچ;
	}

	إذا (good_cs10_چوکٺو_ڳڻپ > samples - bad_samples_allowance && !cs10_channel_gap_مليو
	    && cs10_channel_ڳڻپ >= minimum_channel_ڳڻپ && valid_channel_counts_cs10) {
#جيڪڏهن_وصف_ٿيل DSM_DEBUG
		لکو("DSM guess: CS10 guessed!\n");
#جيڪڏهن_پورو
		dsm_chan_ڳڻپ = cs10_channel_ڳڻپ;
		dsm_channel_سرڪايو = 10;
		أرجع سچ;
	}

	/* call ourselves to reset our state ... we have to try again */
#جيڪڏهن_وصف_ٿيل DSM_DEBUG
	لکو("DSM: format detect fail\n");
#جيڪڏهن_پورو
	dsm_guess_format(سچ);
	أرجع خطأ;
}

صحيح dsm_config(صحيح fd)
{
#جيڪڏهن_وصف_ٿيل SPEKTRUM_POWER_CONFIG
	// Enable power controls for Spektrum receiver
	SPEKTRUM_POWER_CONFIG();
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SPEKTRUM_POWER
	// enable power on DSM connector
	SPEKTRUM_POWER(سچ);
#جيڪڏهن_پورو

	صحيح ret = -1;

	إذا (fd >= 0) {

		بنية termios t;

		/* 115200bps, no parity, one stop bit */
		tcgetattr(fd, &t);
		cfsetspeed(&t, 115200);
		t.c_cflag &= ~(CSTOPB | PARENB);
		tcsetattr(fd, TCSANOW, &t);

		/* initialise the decoder */
		dsm_partial_چوکٺو_ڳڻپ = 0;
		dsm_آخري_rx_وقت = hrt_absolute_وقت();

		/* reset the format detector */
		dsm_guess_format(سچ);

		ret = 0;
	}

	أرجع ret;
}

فراغ dsm_proto_init()
{
	dsm_channel_سرڪايو = 0;
	dsm_چوکٺو_drops = 0;
	dsm_chan_ڳڻپ = 0;

	dsm_decode_state = DSM_DECODE_STATE_DESYNC;

	لكل (بنانشان i = 0; i < DSM_MAX_CHANNEL_ڳڻپ; i++) {
		channel_بفر[i].آخري_ڏٺل = 0;
		channel_بفر[i].value = 0;
	}

	/* reset the format detector */
	dsm_guess_format(سچ);
}

/**
 * Initialize the DSM receive functionality
 *
 * Open the UART for receiving DSM frames and configure it appropriately
 *
 * @param[in] device Device name of DSM UART
 */
صحيح dsm_init(ثابت حرف *device)
{
	إذا (dsm_fd < 0) {
		dsm_fd = open(device, O_RDWR | O_NONBLOCK);
	}

	dsm_proto_init();

	صحيح ret = dsm_config(dsm_fd);

	إذا (!ret) {
		أرجع dsm_fd;

	} وإلا {
		أرجع -1;
	}
}

فراغ dsm_deinit()
{
#جيڪڏهن_وصف_ٿيل SPEKTRUM_POWER_PASSIVE
	// Turn power controls to passive
	SPEKTRUM_POWER_PASSIVE();
#جيڪڏهن_پورو

	إذا (dsm_fd >= 0) {
		close(dsm_fd);
	}

	dsm_fd = -1;
}

#إذا defined(SPEKTRUM_POWER)
/**
 * Handle DSM satellite receiver bind mode handler
 *
 * @param[in] cmd commands - dsm_bind_power_down, dsm_bind_power_up, dsm_bind_set_rx_out, dsm_bind_send_pulses, dsm_bind_reinit_uart
 * @param[in] pulses Number of pulses for dsm_bind_send_pulses command
 */
فراغ dsm_bind(uint16_t cmd, صحيح pulses)
{
	إذا (dsm_fd < 0) {
		أرجع;
	}

	بدل (cmd) {
	حالة DSM_CMD_BIND_POWER_هيٺ_ڏانهن:
		// power down DSM satellite
#إذا defined(DSM_DEBUG)
		لکو("DSM: DSM_CMD_BIND_POWER_DOWN\n");
#جيڪڏهن_پورو
		SPEKTRUM_POWER(خطأ);
		اكسر;

	حالة DSM_CMD_BIND_POWER_مٿي:
		// power up DSM satellite
#إذا defined(DSM_DEBUG)
		لکو("DSM: DSM_CMD_BIND_POWER_UP\n");
#جيڪڏهن_پورو
		SPEKTRUM_POWER(سچ);
		dsm_guess_format(سچ);
		اكسر;

	حالة DSM_CMD_BIND_سيٽ_RX_ٻاهريون:
		// Set UART RX pin to active output mode
#إذا defined(DSM_DEBUG)
		لکو("DSM: DSM_CMD_BIND_SET_RX_OUT\n");
#جيڪڏهن_پورو
		SPEKTRUM_RX_AS_GPIO_OUTPUT();
		اكسر;

	حالة DSM_CMD_BIND_SEND_PULSES:
		// Pulse RX pin a number of times
#إذا defined(DSM_DEBUG)
		لکو("DSM: DSM_CMD_BIND_SEND_PULSES\n");
#جيڪڏهن_پورو

		لكل (صحيح i = 0; i < pulses; i++) {
			dsm_udelay(120);
			SPEKTRUM_ٻاهريون(خطأ);
			dsm_udelay(120);
			SPEKTRUM_ٻاهريون(سچ);
		}

		اكسر;

	حالة DSM_CMD_BIND_REINIT_UART:
		// Restore USART RX pin to RS232 receive mode
#إذا defined(DSM_DEBUG)
		لکو("DSM: DSM_CMD_BIND_REINIT_UART\n");
#جيڪڏهن_پورو
		SPEKTRUM_RX_AS_UART();
		اكسر;

	}
}
#جيڪڏهن_پورو

/**
 * Decode the entire dsm frame (all contained channels)
 *
 * @param[in] frame_time timestamp when this dsm frame was received. Used to detect RX loss in order to reset 10/11 bit guess.
 * @param[out] num_values pointer to number of raw channel values returned
 * @return true=DSM frame successfully decoded, false=no update
 */
منطقي dsm_decode(hrt_abstime چوکٺو_وقت, منطقي *dsm_11_ٻٽو_انگ, بنانشان channel_ڳڻپ,
		int8_t *rssi_percent)
{
	// If we haven't seen a new frame recently or haven't guessed the DSM encoding, try to guess
	إذا (((چوکٺو_وقت - dsm_آخري_چوکٺو_وقت) > 1000000) && (dsm_channel_سرڪايو != 0)) {
		dsm_guess_format(سچ);
	}

	/* if we don't know the dsm_frame format, update the guessing state machine */
	إذا (dsm_channel_سرڪايو == 0) {
		إذا (!dsm_guess_format(خطأ)) {
			أرجع خطأ;
		}

		// We have our first packet so we're in sync
		dsm_آخري_rx_وقت = hrt_absolute_وقت();
	}

	/*
	 * The first byte represents the rssi in dBm on telemetry receivers with updated
	 * firmware, or fades on others. If the value is less than zero, it's rssi.
	 * We have other ways to detect bad link metrics, so ignore positive values,
	 * but rssi dBm is a useful value.
	 */

	// The SPM4649T with firmware version 1.1RC9 or later will have RSSI in place of fades
	إذا (rssi_percent) {
		إذا (((int8_t *)dsm_چوکٺو)[0] < 0) {
			/*
			 * RSSI is a signed integer between -42dBm and -92dBm
			 * If signal is lost, the value is -128
			 */
			int8_t dbm = (int8_t)dsm_چوکٺو[0];

			إذا (dbm == -128) {
				*rssi_percent = 0;

			} وإلا {
				*rssi_percent = spek_dbm_منزل_percent(dbm);
			}

		} وإلا {
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

	px4::Bitset<DSM_MAX_CHANNEL_ڳڻپ> channels_مليو;

	لكل (بنانشان i = 0; i < DSM_چوکٺو_CHANNELS; i++) {

		uint8_t *dp = &dsm_چوکٺو[2 + (2 * i)];
		uint16_t raw = (dp[0] << 8) | dp[1];

		uint8_t channel = 0;
		uint16_t value = 0;

		إذا (!dsm_decode_channel(raw, dsm_channel_سرڪايو, channel, value)) {
			dsm_چوکٺو_drops++;
			أرجع خطأ;
		}

		إذا (channel == CHANNEL_اڻ_استعمال_ٿيل) {
			استمر;
		}

		// abort if channel already found, no duplicate channels per DSM frame
		إذا (channels_مليو[channel]) {
			PX4_DEBUG("duplicate channel %d\n\n", channel);
			dsm_guess_format(سچ);
			dsm_چوکٺو_drops++;
			أرجع خطأ;

		} وإلا {
			channels_مليو.سيٽ(channel);
		}

		/* reset bit guessing state machine if the channel index is out of bounds */
		إذا (channel >= DSM_MAX_CHANNEL_ڳڻپ || channel >= channel_ڳڻپ) {
			PX4_DEBUG("channel %d > %d (DSM_MAX_CHANNEL_COUNT)", channel, DSM_MAX_CHANNEL_ڳڻپ);
			dsm_guess_format(سچ);
			dsm_چوکٺو_drops++;
			أرجع خطأ;
		}

		/*
		 * Store the decoded channel into the R/C input buffer, taking into
		 * account the different ideas about channel assignement that we have.
		 *
		 * Specifically, the first four channels in rc_channel_data are roll, pitch, thrust, yaw,
		 * but the first four channels from the DSM receiver are thrust, roll, pitch, yaw.
		 */
		بدل (channel) {
		حالة 0:
			channel = 2; // Spektrum Throttle (0) -> 2
			اكسر;

		حالة 1:
			channel = 0; // Spektrum Aileron (1) -> 0
			اكسر;

		حالة 2:
			channel = 1; // Spektrum Elevator (2) -> 1

		افتراضي:
			اكسر;
		}

		channel_بفر[channel].value = value;
		channel_بفر[channel].آخري_ڏٺل = چوکٺو_وقت;
	}

	/* Set the 11-bit data indicator */
	*dsm_11_ٻٽو_انگ = (dsm_channel_سرڪايو == 11);

	/* we have received something we think is a dsm_frame */
	dsm_آخري_چوکٺو_وقت = چوکٺو_وقت;

#جيڪڏهن_وصف_ٿيل DSM_DEBUG
	لکو("PARSED PACKET\n");
#جيڪڏهن_پورو

	أرجع سچ;
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
منطقي dsm_داخلا(صحيح fd, uint16_t *قدرون_2, uint16_t *انگ_قدرون, منطقي *dsm_11_ٻٽو_انگ, uint8_t *n_بائيٽون, uint8_t **بائيٽون,
	       int8_t *rssi, بنانشان *چوکٺو_drops, بنانشان max_قدرون)
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
	ثابت hrt_abstime now = hrt_absolute_وقت();

	/*
	 * Fetch bytes, but no more than we would need to complete
	 * a complete frame.
	 */

	صحيح ret = اقرأ(fd, &dsm_buf[0], حجم(dsm_buf) / حجم(dsm_buf[0]));

	/* if the read failed for any reason, just give up here */
	إذا (ret < 1) {
		أرجع خطأ;

	} وإلا {
		*n_بائيٽون = ret;
		*بائيٽون = &dsm_buf[0];
	}

	/*
	 * Try to decode something with what we got
	 */
	أرجع dsm_parse(now, &dsm_buf[0], ret, قدرون_2, انگ_قدرون, dsm_11_ٻٽو_انگ, &dsm_چوکٺو_drops, rssi, max_قدرون);
}

منطقي dsm_parse(ثابت uint64_t now, ثابت uint8_t *چوکٺو, ثابت بنانشان len, uint16_t *قدرون_2,
	       uint16_t *انگ_قدرون, منطقي *dsm_11_ٻٽو_انگ, بنانشان *چوکٺو_drops, int8_t *rssi_percent, uint16_t max_channels)
{
	/* this is set by the decoding state machine and will default to false
	 * once everything that was decodable has been decoded.
	 */
	منطقي channel_معلومات_موجود = خطأ;
	بنانشان channel_output_ڳڻپ;

	/* keep decoding until we have consumed the buffer */
	لكل (بنانشان d = 0; d < len; d++) {

		/* overflow check */
		إذا (dsm_partial_چوکٺو_ڳڻپ == حجم(dsm_چوکٺو) / حجم(dsm_چوکٺو[0])) {
			dsm_partial_چوکٺو_ڳڻپ = 0;
			dsm_decode_state = DSM_DECODE_STATE_DESYNC;
#جيڪڏهن_وصف_ٿيل DSM_DEBUG
			لکو("DSM: RESET (BUF LIM)\n");
#جيڪڏهن_پورو
		}

		إذا (dsm_partial_چوکٺو_ڳڻپ == DSM_چوکٺو_سائيز) {
			dsm_partial_چوکٺو_ڳڻپ = 0;
			dsm_decode_state = DSM_DECODE_STATE_DESYNC;
#جيڪڏهن_وصف_ٿيل DSM_DEBUG
			لکو("DSM: RESET (PACKET LIM)\n");
#جيڪڏهن_پورو
		}

#جيڪڏهن_وصف_ٿيل DSM_DEBUG
#إذا 0
		لکو("dsm state: %s%s, count: %d, val: %02x\n",
		       (dsm_decode_state == DSM_DECODE_STATE_DESYNC) ? "DSM_DECODE_STATE_DESYNC" : "",
		       (dsm_decode_state == DSM_DECODE_STATE_SYNC) ? "DSM_DECODE_STATE_SYNC" : "",
		       dsm_partial_چوکٺو_ڳڻپ,
		       (بنانشان)چوکٺو[d]);
#جيڪڏهن_پورو
#جيڪڏهن_پورو

		بدل (dsm_decode_state) {
		حالة DSM_DECODE_STATE_DESYNC:

			/* we are de-synced and only interested in the frame marker */
			إذا ((now - dsm_آخري_rx_وقت) > 5000) {
				dsm_decode_state = DSM_DECODE_STATE_SYNC;
				dsm_partial_چوکٺو_ڳڻپ = 0;
				dsm_چوکٺو[dsm_partial_چوکٺو_ڳڻپ++] = چوکٺو[d];
			}

			اكسر;

		حالة DSM_DECODE_STATE_SYNC: {
				dsm_چوکٺو[dsm_partial_چوکٺو_ڳڻپ++] = چوکٺو[d];

				/* decode whatever we got and expect */
				إذا (dsm_partial_چوکٺو_ڳڻپ < DSM_چوکٺو_سائيز) {
					اكسر;
				}

				// We've collected a full frame, parse it
				channel_معلومات_موجود = dsm_decode(now, dsm_11_ٻٽو_انگ, dsm_chan_ڳڻپ, rssi_percent);

				// Frame consumed, reset the buffer count
				dsm_partial_چوکٺو_ڳڻپ = 0;

				/* if decoding failed, set proto to desync */
				إذا (!channel_معلومات_موجود) {
					dsm_decode_state = DSM_DECODE_STATE_DESYNC;
				}
			}
			اكسر;

		افتراضي:
#جيڪڏهن_وصف_ٿيل DSM_DEBUG
			لکو("UNKNOWN PROTO STATE");
#جيڪڏهن_پورو
			channel_معلومات_موجود = خطأ;
		}
	}

	إذا (چوکٺو_drops) {
		*چوکٺو_drops = dsm_چوکٺو_drops;
	}

	إذا (channel_معلومات_موجود) {
		// Check if any channels have been absent for too long
		لكل (بنانشان i = 0; i < dsm_chan_ڳڻپ; i++) {
			إذا (now - channel_بفر[i].آخري_ڏٺل > 50 * 1000) {
				أرجع خطأ;
			}
		}

		// Overflow protect
		إذا (max_channels >= DSM_MAX_CHANNEL_ڳڻپ) {
			max_channels = DSM_MAX_CHANNEL_ڳڻپ;
		}

		// Cap returned channels to max_channels if we've seen more
		إذا (dsm_chan_ڳڻپ > max_channels) {
			channel_output_ڳڻپ = max_channels;

		} وإلا {
			channel_output_ڳڻپ = dsm_chan_ڳڻپ;
		}

		*انگ_قدرون = channel_output_ڳڻپ;

		// Copy the full list of collected channels
		لكل (بنانشان i = 0; i < channel_output_ڳڻپ; i++) {
			// Channels which do not have a value yet (because their frame has not been seen yet)
			// will have a zero. If we come across a blank channel, keep storing values, but we
			// won't return that we have a valid frame just yet.
			إذا (channel_بفر[i].value) {
				قدرون_2[i] = channel_بفر[i].value;

			} وإلا {
				channel_معلومات_موجود = خطأ;
			}
		}
	}

#جيڪڏهن_وصف_ٿيل DSM_DEBUG

	إذا (channel_معلومات_موجود) {
		لکو("frame drops: %u, chan #: %u\n", dsm_چوکٺو_drops, dsm_chan_ڳڻپ);

		لكل (بنانشان i = 0; i < dsm_chan_ڳڻپ; i++) {
			لکو("dsm_decode: #CH %02u: %u\n", i + 1, قدرون_2[i]);
		}
	}

#جيڪڏهن_پورو

	dsm_آخري_rx_وقت = now;

	/* return false as default */
	أرجع channel_معلومات_موجود;
}
