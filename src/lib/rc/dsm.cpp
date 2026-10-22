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

#شامل <px4_platform_common/px4_config.h>
#شامل <board_config.h>
#شامل <px4_platform_common/defines.h>
#شامل <px4_platform_common/log.h>

#شامل <fcntl.h>
#شامل <math.h>
#شامل <unistd.h>
#شامل <termios.h>
#شامل <string.h>

#شامل "dsm.h"
#شامل "spektrum_rssi.h"
#شامل "common_rc.h"
#شامل <drivers/drv_hrt.h>

#شامل <include/containers/Bitset.hpp>

#اگر defined(__PX4_NUTTX)
#شامل <nuttx/arch.h>
#تعریف_کرو dsm_udelay(arg)    اوپر_udelay(arg)
#ورنہ
#تعریف_کرو dsm_udelay(arg) px4_usleep(arg)
#اگر_ختم

// #define DSM_DEBUG

ساکن شمار DSM_DECODE_STATE {
	DSM_DECODE_STATE_DESYNC = 0,
	DSM_DECODE_STATE_SYNC
} dsm_decode_state = DSM_DECODE_STATE_DESYNC;

قسمتعریف ساخت {
	hrt_abstime آخری_نظر_آیا;
	uint16_t value;
} dsm_channel_t;

ساکن dsm_channel_t channel_بفر[DSM_MAX_CHANNEL_گنتی];

ساکن صحیح dsm_fd = -1;						/**< File handle to the DSM UART */
ساکن hrt_abstime dsm_آخری_rx_وقت;            /**< Timestamp when we last received data */
ساکن hrt_abstime dsm_آخری_چوکھٹا_وقت;		/**< Timestamp for start of last valid dsm frame */
ساکن dsm_چوکھٹا_t &dsm_چوکھٹا = rc_decode_buf.dsm.چوکھٹا;	/**< DSM_BUFFER_SIZE DSM dsm frame receive buffer */
ساکن dsm_buf_t &dsm_buf = rc_decode_buf.dsm.buf;	/**< DSM_BUFFER_SIZE DSM dsm frame receive buffer */

ساکن غیرعلامتی dsm_partial_چوکھٹا_گنتی;	/**< Count of bytes received for current dsm frame */
ساکن غیرعلامتی dsm_channel_سرکاؤ = 0;			/**< Channel resolution, 0=unknown, 10=10 bit (1024), 11=11 bit (2048) */
ساکن غیرعلامتی dsm_چوکھٹا_drops = 0;			/**< Count of incomplete DSM frames */
ساکن uint16_t dsm_chan_گنتی = 0;         /**< DSM channel count */

ساکن constexpr uint8_t CHANNEL_غیر_مستعمل = 0xFF;

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
ساکن منطقی dsm_decode_channel(uint16_t raw, غیرعلامتی سرکاؤ, uint8_t &channel, uint16_t &value)
{
	اگر (سرکاؤ == 10) {
		// 1024 Mode: This format is used only by DSM2/22ms mode. All other modes use 2048 data.
		//  Bits 15-10 Channel ID
		//  Bits 9-0   Servo Position
		ساکن constexpr uint16_t نقاب_1024_CHANID = 0xFC00;
		ساکن constexpr uint16_t نقاب_1024_SXPOS = 0x03FF;

		channel = (raw & نقاب_1024_CHANID) >> 10; // 6 bits

		ثابت uint16_t servo_position = (raw & نقاب_1024_SXPOS); // 10 bits

		اگر (channel >= DSM_MAX_CHANNEL_گنتی) {
			PX4_DEBUG("invalid channel: %d\n", channel);
			واپس جھوٹ;
		}

		// PWM_OUT = (ServoPosition x 1.166μs) + Offset
		ساکن constexpr uint16_t انحراف = 903; // microseconds
		value = roundf(servo_position * 1.166f) + انحراف;

		واپس سچ;

	} ورنہ اگر (سرکاؤ == 11) {
		// 2048 Mode
		//  Bits 15    Servo Phase
		//  Bits 14-11 Channel ID
		//  Bits 10-0  Servo Position

		uint16_t servo_position = 0;

		// from Spektrum Remote Receiver Interfacing Rev G Page 6
		ثابت منطقی phase = (raw >> 15) & 0x01; // the phase is part of the X-Plus address (bit 15)
		uint8_t chan = (raw >> 11) & 0x0F;

		اگر (chan < 12) {
			// Normal channels
			ساکن constexpr uint16_t نقاب_2048_SXPOS = 0x07FF;
			servo_position = (raw & نقاب_2048_SXPOS);

		} ورنہ اگر (chan == 12) {
			// XPlus channels
			chan += ((raw >> 9) & 0x03);

			اگر (phase) {
				chan += 4;
			}

			اگر (chan >= DSM_MAX_CHANNEL_گنتی) {
				واپس جھوٹ;
			}

			servo_position = (raw & 0x01FF) << 2;

			channel = chan;

		} ورنہ اگر (chan == 0x0F && phase) {
			// When the channel content is all bits set, and the phase is also set, this is the blank channel
			channel = CHANNEL_غیر_مستعمل;
			واپس سچ;

		} ورنہ {
			// This will be the case for an unused channel (raw 16bit content will be 0xffff)
			PX4_DEBUG("invalid: %d %d\n", chan, phase);
			واپس جھوٹ;
		}

		channel = chan;

		// PWM_OUT = (ServoPosition x 0.583μs) + Offset
		ساکن constexpr uint16_t انحراف = 903; // microseconds
		value = roundf(servo_position * 0.583f) + انحراف;

		PX4_DEBUG(stderr, "CH%d=%d(0x%02x), ", channel, value, raw);

		واپس سچ;
	}

	واپس جھوٹ;
}

/**
 * Attempt to guess if receiving 10 or 11 bit channel values
 *
 * @param[in] reset true=reset the 10/11 bit state to unknown
 */
ساکن منطقی dsm_guess_format(منطقی reset)
{
	ساکن uint32_t cs10 = 0;
	ساکن uint32_t cs11 = 0;
	ساکن uint16_t good_cs10_چوکھٹا_گنتی = 0;
	ساکن uint16_t good_cs11_چوکھٹا_گنتی = 0;
	ساکن uint16_t samples = 0;
	ساکن uint16_t نظر_آیا_channels_گنتی_cs11[DSM_MAX_CHANNEL_گنتی] {};
	ساکن uint16_t نظر_آیا_channels_گنتی_cs10[DSM_MAX_CHANNEL_گنتی] {};

	/* reset the 10/11 bit sniffed channel masks */
	اگر (reset) {
		PX4_DEBUG("dsm_guess_format reset");
		cs10 = 0;
		cs11 = 0;
		samples = 0;

		dsm_channel_سرکاؤ = 0;
		good_cs10_چوکھٹا_گنتی = 0;
		good_cs11_چوکھٹا_گنتی = 0;

		برائے (غیرعلامتی i = 0; i < DSM_MAX_CHANNEL_گنتی; i++) {
			نظر_آیا_channels_گنتی_cs10[i] = 0;
			نظر_آیا_channels_گنتی_cs11[i] = 0;
		}

		واپس جھوٹ;
	}

	px4::Bitset<DSM_MAX_CHANNEL_گنتی> channels_ملا_10;
	px4::Bitset<DSM_MAX_CHANNEL_گنتی> channels_ملا_11;

	منطقی cs10_چوکھٹا_valid = سچ;
	منطقی cs11_چوکھٹا_valid = سچ;

	/* scan the channels in the current dsm_frame in both 10- and 11-bit mode */
	برائے (غیرعلامتی i = 0; i < DSM_چوکھٹا_CHANNELS; i++) {

		uint8_t *dp = &dsm_چوکھٹا[2 + (2 * i)];
		uint16_t raw = (dp[0] << 8) | dp[1];

		uint8_t channel = 0;
		uint16_t value = 0;

		/* if the channel decodes, remember the assigned number */
		اگر (dsm_decode_channel(raw, 10, channel, value)) {
			اگر (channel == CHANNEL_غیر_مستعمل) {
				جاری;
			}

			// invalidate entire frame (for 1024) if channel already found, no duplicate channels per DSM frame
			اگر (channels_ملا_10[channel]) {
				cs10_چوکھٹا_valid = جھوٹ;

			} ورنہ {
				نظر_آیا_channels_گنتی_cs10[channel]++;
				channels_ملا_10.سیٹ(channel);
			}
		}

		اگر (dsm_decode_channel(raw, 11, channel, value)) {
			اگر (channel == CHANNEL_غیر_مستعمل) {
				جاری;
			}

			// invalidate entire frame (for 2048) if channel already found, no duplicate channels per DSM frame
			اگر (channels_ملا_11[channel]) {
				cs11_چوکھٹا_valid = جھوٹ;

			} ورنہ {
				نظر_آیا_channels_گنتی_cs11[channel]++;

				channels_ملا_11.سیٹ(channel);
			}
		}
	}

	// add valid cs10 channels
	اگر (cs10_چوکھٹا_valid) {
		good_cs10_چوکھٹا_گنتی++;

		برائے (غیرعلامتی channel = 0; channel < DSM_MAX_CHANNEL_گنتی; channel++) {
			اگر (channels_ملا_10[channel]) {
				cs10 |= 1 << channel;
			}
		}
	}

	// add valid cs11 channels
	اگر (cs11_چوکھٹا_valid) {
		good_cs11_چوکھٹا_گنتی++;

		برائے (غیرعلامتی channel = 0; channel < DSM_MAX_CHANNEL_گنتی; channel++) {
			اگر (channels_ملا_11[channel]) {
				cs11 |= 1 << channel;
			}
		}
	}

	samples++;

	/* wait until we have seen plenty of frames */
	اگر (samples < 15) {
		واپس جھوٹ;
	}

	/*
	* 10 or 11 bit decoding guess requirements
	*  For CS10 or CS11...
	*  At least `samples - bad_samples_allowance` must decode correctly (no duplicates, valid channel ranges for CS10)
	*  Even distribution of all found channels
	*  Channels must begin at 0 and no channel gaps after that (ie, the last channel is the one before the first unseen channel starting from zero)
	 */

	منطقی cs10_channel_gap_ملا = جھوٹ;
	منطقی cs11_channel_gap_ملا = جھوٹ;
	منطقی valid_channel_counts_cs10 = جھوٹ;
	منطقی valid_channel_counts_cs11 = جھوٹ;
	uint32_t cs10_channel_گنتی = 0;
	uint32_t cs11_channel_گنتی = 0;
	منطقی ملا_channels_انتہا = جھوٹ;

	// Count of allowed bad frames
	ساکن constexpr uint16_t bad_samples_allowance = 5;
	ساکن constexpr uint16_t minimum_channel_گنتی = 5;
	ساکن constexpr uint16_t minimum_channel_نظر_آیا_گنتی = 4;

	// Check for continous channels in 10bit decoding
	ملا_channels_انتہا = جھوٹ;

	برائے (غیرعلامتی i = 0; i < DSM_MAX_CHANNEL_گنتی; i++) {
		// This bit is set
		اگر (cs10 & (1 << i)) {
			اگر (ملا_channels_انتہا) {
				// Channel gap found
				cs10_channel_gap_ملا = سچ;
				توڑو;
			}

		} ورنہ {
			اگر (!ملا_channels_انتہا) {
				cs10_channel_گنتی = i;
			}

			ملا_channels_انتہا = سچ;
		}
	}

	// Check for continous channels in 11bit decoding
	ملا_channels_انتہا = جھوٹ;

	برائے (غیرعلامتی i = 0; i < DSM_MAX_CHANNEL_گنتی; i++) {
		// This bit is set
		اگر (cs11 & (1 << i)) {
			اگر (ملا_channels_انتہا) {
				// Channel gap found
				cs11_channel_gap_ملا = سچ;
				توڑو;
			}

		} ورنہ {
			اگر (!ملا_channels_انتہا) {
				cs11_channel_گنتی = i;
			}

			ملا_channels_انتہا = سچ;
		}
	}

	// Check channel seen counts C10
	اگر (cs10_channel_گنتی && !cs10_channel_gap_ملا) {
		valid_channel_counts_cs10 = سچ;

		برائے (غیرعلامتی i = 0; i < ((cs10_channel_گنتی > 12) ? 12 : cs10_channel_گنتی); i++) {
			اگر (نظر_آیا_channels_گنتی_cs10[i] < minimum_channel_نظر_آیا_گنتی) {
				valid_channel_counts_cs10 = جھوٹ;
				توڑو;
			}

		}
	}

	// Check channel seen counts C11
	اگر (cs11_channel_گنتی && !cs11_channel_gap_ملا) {
		valid_channel_counts_cs11 = سچ;

		برائے (غیرعلامتی i = 0; i < ((cs11_channel_گنتی > 12) ? 12 : cs11_channel_گنتی); i++) {
			اگر (نظر_آیا_channels_گنتی_cs11[i] < minimum_channel_نظر_آیا_گنتی) {
				valid_channel_counts_cs11 = جھوٹ;
				توڑو;
			}

		}
	}

#اگر_تعریف_شدہ DSM_DEBUG
	لکھو("DSM guess: CS10 (%li good frames, %i gap found, %li channel count, %i dist)\r\n", good_cs10_چوکھٹا_گنتی,
	       cs10_channel_gap_ملا, cs10_channel_گنتی, valid_channel_counts_cs10);
	لکھو("DSM guess: CS11 (%li good frames, %i gap found, %li channel count, %i dist)\r\n", good_cs11_چوکھٹا_گنتی,
	       cs11_channel_gap_ملا, cs11_channel_گنتی, valid_channel_counts_cs11);
#اگر_ختم

	اگر (good_cs11_چوکھٹا_گنتی > samples - bad_samples_allowance && !cs11_channel_gap_ملا
	    && cs11_channel_گنتی >= minimum_channel_گنتی && valid_channel_counts_cs11) {
#اگر_تعریف_شدہ DSM_DEBUG
		لکھو("DSM guess: CS11 guessed!\n");
#اگر_ختم
		dsm_chan_گنتی = cs11_channel_گنتی;
		dsm_channel_سرکاؤ = 11;
		واپس سچ;
	}

	اگر (good_cs10_چوکھٹا_گنتی > samples - bad_samples_allowance && !cs10_channel_gap_ملا
	    && cs10_channel_گنتی >= minimum_channel_گنتی && valid_channel_counts_cs10) {
#اگر_تعریف_شدہ DSM_DEBUG
		لکھو("DSM guess: CS10 guessed!\n");
#اگر_ختم
		dsm_chan_گنتی = cs10_channel_گنتی;
		dsm_channel_سرکاؤ = 10;
		واپس سچ;
	}

	/* call ourselves to reset our state ... we have to try again */
#اگر_تعریف_شدہ DSM_DEBUG
	لکھو("DSM: format detect fail\n");
#اگر_ختم
	dsm_guess_format(سچ);
	واپس جھوٹ;
}

صحیح dsm_config(صحیح fd)
{
#اگر_تعریف_شدہ SPEKTRUM_POWER_CONFIG
	// Enable power controls for Spektrum receiver
	SPEKTRUM_POWER_CONFIG();
#اگر_ختم
#اگر_تعریف_شدہ SPEKTRUM_POWER
	// enable power on DSM connector
	SPEKTRUM_POWER(سچ);
#اگر_ختم

	صحیح ret = -1;

	اگر (fd >= 0) {

		ساخت termios t;

		/* 115200bps, no parity, one stop bit */
		tcgetattr(fd, &t);
		cfsetspeed(&t, 115200);
		t.c_cflag &= ~(CSTOPB | PARENB);
		tcsetattr(fd, TCSANOW, &t);

		/* initialise the decoder */
		dsm_partial_چوکھٹا_گنتی = 0;
		dsm_آخری_rx_وقت = hrt_absolute_وقت();

		/* reset the format detector */
		dsm_guess_format(سچ);

		ret = 0;
	}

	واپس ret;
}

خالی dsm_proto_init()
{
	dsm_channel_سرکاؤ = 0;
	dsm_چوکھٹا_drops = 0;
	dsm_chan_گنتی = 0;

	dsm_decode_state = DSM_DECODE_STATE_DESYNC;

	برائے (غیرعلامتی i = 0; i < DSM_MAX_CHANNEL_گنتی; i++) {
		channel_بفر[i].آخری_نظر_آیا = 0;
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
صحیح dsm_init(ثابت حرف *device)
{
	اگر (dsm_fd < 0) {
		dsm_fd = open(device, O_RDWR | O_NONBLOCK);
	}

	dsm_proto_init();

	صحیح ret = dsm_config(dsm_fd);

	اگر (!ret) {
		واپس dsm_fd;

	} ورنہ {
		واپس -1;
	}
}

خالی dsm_deinit()
{
#اگر_تعریف_شدہ SPEKTRUM_POWER_PASSIVE
	// Turn power controls to passive
	SPEKTRUM_POWER_PASSIVE();
#اگر_ختم

	اگر (dsm_fd >= 0) {
		close(dsm_fd);
	}

	dsm_fd = -1;
}

#اگر defined(SPEKTRUM_POWER)
/**
 * Handle DSM satellite receiver bind mode handler
 *
 * @param[in] cmd commands - dsm_bind_power_down, dsm_bind_power_up, dsm_bind_set_rx_out, dsm_bind_send_pulses, dsm_bind_reinit_uart
 * @param[in] pulses Number of pulses for dsm_bind_send_pulses command
 */
خالی dsm_bind(uint16_t cmd, صحیح pulses)
{
	اگر (dsm_fd < 0) {
		واپس;
	}

	انتخاب (cmd) {
	حالت DSM_CMD_BIND_POWER_نیچے_کو:
		// power down DSM satellite
#اگر defined(DSM_DEBUG)
		لکھو("DSM: DSM_CMD_BIND_POWER_DOWN\n");
#اگر_ختم
		SPEKTRUM_POWER(جھوٹ);
		توڑو;

	حالت DSM_CMD_BIND_POWER_اوپر:
		// power up DSM satellite
#اگر defined(DSM_DEBUG)
		لکھو("DSM: DSM_CMD_BIND_POWER_UP\n");
#اگر_ختم
		SPEKTRUM_POWER(سچ);
		dsm_guess_format(سچ);
		توڑو;

	حالت DSM_CMD_BIND_سیٹ_RX_برآمد:
		// Set UART RX pin to active output mode
#اگر defined(DSM_DEBUG)
		لکھو("DSM: DSM_CMD_BIND_SET_RX_OUT\n");
#اگر_ختم
		SPEKTRUM_RX_AS_GPIO_OUTPUT();
		توڑو;

	حالت DSM_CMD_BIND_SEND_PULSES:
		// Pulse RX pin a number of times
#اگر defined(DSM_DEBUG)
		لکھو("DSM: DSM_CMD_BIND_SEND_PULSES\n");
#اگر_ختم

		برائے (صحیح i = 0; i < pulses; i++) {
			dsm_udelay(120);
			SPEKTRUM_برآمد(جھوٹ);
			dsm_udelay(120);
			SPEKTRUM_برآمد(سچ);
		}

		توڑو;

	حالت DSM_CMD_BIND_REINIT_UART:
		// Restore USART RX pin to RS232 receive mode
#اگر defined(DSM_DEBUG)
		لکھو("DSM: DSM_CMD_BIND_REINIT_UART\n");
#اگر_ختم
		SPEKTRUM_RX_AS_UART();
		توڑو;

	}
}
#اگر_ختم

/**
 * Decode the entire dsm frame (all contained channels)
 *
 * @param[in] frame_time timestamp when this dsm frame was received. Used to detect RX loss in order to reset 10/11 bit guess.
 * @param[out] num_values pointer to number of raw channel values returned
 * @return true=DSM frame successfully decoded, false=no update
 */
منطقی dsm_decode(hrt_abstime چوکھٹا_وقت, منطقی *dsm_11_ثنائی_ہندسہ, غیرعلامتی channel_گنتی,
		int8_t *rssi_percent)
{
	// If we haven't seen a new frame recently or haven't guessed the DSM encoding, try to guess
	اگر (((چوکھٹا_وقت - dsm_آخری_چوکھٹا_وقت) > 1000000) && (dsm_channel_سرکاؤ != 0)) {
		dsm_guess_format(سچ);
	}

	/* if we don't know the dsm_frame format, update the guessing state machine */
	اگر (dsm_channel_سرکاؤ == 0) {
		اگر (!dsm_guess_format(جھوٹ)) {
			واپس جھوٹ;
		}

		// We have our first packet so we're in sync
		dsm_آخری_rx_وقت = hrt_absolute_وقت();
	}

	/*
	 * The first byte represents the rssi in dBm on telemetry receivers with updated
	 * firmware, or fades on others. If the value is less than zero, it's rssi.
	 * We have other ways to detect bad link metrics, so ignore positive values,
	 * but rssi dBm is a useful value.
	 */

	// The SPM4649T with firmware version 1.1RC9 or later will have RSSI in place of fades
	اگر (rssi_percent) {
		اگر (((int8_t *)dsm_چوکھٹا)[0] < 0) {
			/*
			 * RSSI is a signed integer between -42dBm and -92dBm
			 * If signal is lost, the value is -128
			 */
			int8_t dbm = (int8_t)dsm_چوکھٹا[0];

			اگر (dbm == -128) {
				*rssi_percent = 0;

			} ورنہ {
				*rssi_percent = spek_dbm_مقصد_percent(dbm);
			}

		} ورنہ {
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

	px4::Bitset<DSM_MAX_CHANNEL_گنتی> channels_ملا;

	برائے (غیرعلامتی i = 0; i < DSM_چوکھٹا_CHANNELS; i++) {

		uint8_t *dp = &dsm_چوکھٹا[2 + (2 * i)];
		uint16_t raw = (dp[0] << 8) | dp[1];

		uint8_t channel = 0;
		uint16_t value = 0;

		اگر (!dsm_decode_channel(raw, dsm_channel_سرکاؤ, channel, value)) {
			dsm_چوکھٹا_drops++;
			واپس جھوٹ;
		}

		اگر (channel == CHANNEL_غیر_مستعمل) {
			جاری;
		}

		// abort if channel already found, no duplicate channels per DSM frame
		اگر (channels_ملا[channel]) {
			PX4_DEBUG("duplicate channel %d\n\n", channel);
			dsm_guess_format(سچ);
			dsm_چوکھٹا_drops++;
			واپس جھوٹ;

		} ورنہ {
			channels_ملا.سیٹ(channel);
		}

		/* reset bit guessing state machine if the channel index is out of bounds */
		اگر (channel >= DSM_MAX_CHANNEL_گنتی || channel >= channel_گنتی) {
			PX4_DEBUG("channel %d > %d (DSM_MAX_CHANNEL_COUNT)", channel, DSM_MAX_CHANNEL_گنتی);
			dsm_guess_format(سچ);
			dsm_چوکھٹا_drops++;
			واپس جھوٹ;
		}

		/*
		 * Store the decoded channel into the R/C input buffer, taking into
		 * account the different ideas about channel assignement that we have.
		 *
		 * Specifically, the first four channels in rc_channel_data are roll, pitch, thrust, yaw,
		 * but the first four channels from the DSM receiver are thrust, roll, pitch, yaw.
		 */
		انتخاب (channel) {
		حالت 0:
			channel = 2; // Spektrum Throttle (0) -> 2
			توڑو;

		حالت 1:
			channel = 0; // Spektrum Aileron (1) -> 0
			توڑو;

		حالت 2:
			channel = 1; // Spektrum Elevator (2) -> 1

		طےشدہ:
			توڑو;
		}

		channel_بفر[channel].value = value;
		channel_بفر[channel].آخری_نظر_آیا = چوکھٹا_وقت;
	}

	/* Set the 11-bit data indicator */
	*dsm_11_ثنائی_ہندسہ = (dsm_channel_سرکاؤ == 11);

	/* we have received something we think is a dsm_frame */
	dsm_آخری_چوکھٹا_وقت = چوکھٹا_وقت;

#اگر_تعریف_شدہ DSM_DEBUG
	لکھو("PARSED PACKET\n");
#اگر_ختم

	واپس سچ;
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
منطقی dsm_داخلہ(صحیح fd, uint16_t *اقدار_2, uint16_t *عدد_اقدار, منطقی *dsm_11_ثنائی_ہندسہ, uint8_t *n_بائٹیں, uint8_t **بائٹیں,
	       int8_t *rssi, غیرعلامتی *چوکھٹا_drops, غیرعلامتی max_اقدار)
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

	صحیح ret = اقرأ(fd, &dsm_buf[0], حجم(dsm_buf) / حجم(dsm_buf[0]));

	/* if the read failed for any reason, just give up here */
	اگر (ret < 1) {
		واپس جھوٹ;

	} ورنہ {
		*n_بائٹیں = ret;
		*بائٹیں = &dsm_buf[0];
	}

	/*
	 * Try to decode something with what we got
	 */
	واپس dsm_parse(now, &dsm_buf[0], ret, اقدار_2, عدد_اقدار, dsm_11_ثنائی_ہندسہ, &dsm_چوکھٹا_drops, rssi, max_اقدار);
}

منطقی dsm_parse(ثابت uint64_t now, ثابت uint8_t *چوکھٹا, ثابت غیرعلامتی len, uint16_t *اقدار_2,
	       uint16_t *عدد_اقدار, منطقی *dsm_11_ثنائی_ہندسہ, غیرعلامتی *چوکھٹا_drops, int8_t *rssi_percent, uint16_t max_channels)
{
	/* this is set by the decoding state machine and will default to false
	 * once everything that was decodable has been decoded.
	 */
	منطقی channel_ڈیٹا_دستیاب = جھوٹ;
	غیرعلامتی channel_output_گنتی;

	/* keep decoding until we have consumed the buffer */
	برائے (غیرعلامتی d = 0; d < len; d++) {

		/* overflow check */
		اگر (dsm_partial_چوکھٹا_گنتی == حجم(dsm_چوکھٹا) / حجم(dsm_چوکھٹا[0])) {
			dsm_partial_چوکھٹا_گنتی = 0;
			dsm_decode_state = DSM_DECODE_STATE_DESYNC;
#اگر_تعریف_شدہ DSM_DEBUG
			لکھو("DSM: RESET (BUF LIM)\n");
#اگر_ختم
		}

		اگر (dsm_partial_چوکھٹا_گنتی == DSM_چوکھٹا_سائز) {
			dsm_partial_چوکھٹا_گنتی = 0;
			dsm_decode_state = DSM_DECODE_STATE_DESYNC;
#اگر_تعریف_شدہ DSM_DEBUG
			لکھو("DSM: RESET (PACKET LIM)\n");
#اگر_ختم
		}

#اگر_تعریف_شدہ DSM_DEBUG
#اگر 0
		لکھو("dsm state: %s%s, count: %d, val: %02x\n",
		       (dsm_decode_state == DSM_DECODE_STATE_DESYNC) ? "DSM_DECODE_STATE_DESYNC" : "",
		       (dsm_decode_state == DSM_DECODE_STATE_SYNC) ? "DSM_DECODE_STATE_SYNC" : "",
		       dsm_partial_چوکھٹا_گنتی,
		       (غیرعلامتی)چوکھٹا[d]);
#اگر_ختم
#اگر_ختم

		انتخاب (dsm_decode_state) {
		حالت DSM_DECODE_STATE_DESYNC:

			/* we are de-synced and only interested in the frame marker */
			اگر ((now - dsm_آخری_rx_وقت) > 5000) {
				dsm_decode_state = DSM_DECODE_STATE_SYNC;
				dsm_partial_چوکھٹا_گنتی = 0;
				dsm_چوکھٹا[dsm_partial_چوکھٹا_گنتی++] = چوکھٹا[d];
			}

			توڑو;

		حالت DSM_DECODE_STATE_SYNC: {
				dsm_چوکھٹا[dsm_partial_چوکھٹا_گنتی++] = چوکھٹا[d];

				/* decode whatever we got and expect */
				اگر (dsm_partial_چوکھٹا_گنتی < DSM_چوکھٹا_سائز) {
					توڑو;
				}

				// We've collected a full frame, parse it
				channel_ڈیٹا_دستیاب = dsm_decode(now, dsm_11_ثنائی_ہندسہ, dsm_chan_گنتی, rssi_percent);

				// Frame consumed, reset the buffer count
				dsm_partial_چوکھٹا_گنتی = 0;

				/* if decoding failed, set proto to desync */
				اگر (!channel_ڈیٹا_دستیاب) {
					dsm_decode_state = DSM_DECODE_STATE_DESYNC;
				}
			}
			توڑو;

		طےشدہ:
#اگر_تعریف_شدہ DSM_DEBUG
			لکھو("UNKNOWN PROTO STATE");
#اگر_ختم
			channel_ڈیٹا_دستیاب = جھوٹ;
		}
	}

	اگر (چوکھٹا_drops) {
		*چوکھٹا_drops = dsm_چوکھٹا_drops;
	}

	اگر (channel_ڈیٹا_دستیاب) {
		// Check if any channels have been absent for too long
		برائے (غیرعلامتی i = 0; i < dsm_chan_گنتی; i++) {
			اگر (now - channel_بفر[i].آخری_نظر_آیا > 50 * 1000) {
				واپس جھوٹ;
			}
		}

		// Overflow protect
		اگر (max_channels >= DSM_MAX_CHANNEL_گنتی) {
			max_channels = DSM_MAX_CHANNEL_گنتی;
		}

		// Cap returned channels to max_channels if we've seen more
		اگر (dsm_chan_گنتی > max_channels) {
			channel_output_گنتی = max_channels;

		} ورنہ {
			channel_output_گنتی = dsm_chan_گنتی;
		}

		*عدد_اقدار = channel_output_گنتی;

		// Copy the full list of collected channels
		برائے (غیرعلامتی i = 0; i < channel_output_گنتی; i++) {
			// Channels which do not have a value yet (because their frame has not been seen yet)
			// will have a zero. If we come across a blank channel, keep storing values, but we
			// won't return that we have a valid frame just yet.
			اگر (channel_بفر[i].value) {
				اقدار_2[i] = channel_بفر[i].value;

			} ورنہ {
				channel_ڈیٹا_دستیاب = جھوٹ;
			}
		}
	}

#اگر_تعریف_شدہ DSM_DEBUG

	اگر (channel_ڈیٹا_دستیاب) {
		لکھو("frame drops: %u, chan #: %u\n", dsm_چوکھٹا_drops, dsm_chan_گنتی);

		برائے (غیرعلامتی i = 0; i < dsm_chan_گنتی; i++) {
			لکھو("dsm_decode: #CH %02u: %u\n", i + 1, اقدار_2[i]);
		}
	}

#اگر_ختم

	dsm_آخری_rx_وقت = now;

	/* return false as default */
	واپس channel_ڈیٹا_دستیاب;
}
