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

#अन्तर्भाव <px4_platform_common/px4_config.h>
#अन्तर्भाव <board_config.h>
#अन्तर्भाव <px4_platform_common/defines.h>
#अन्तर्भाव <px4_platform_common/log.h>

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <math.h>
#अन्तर्भाव <unistd.h>
#अन्तर्भाव <termios.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "dsm.h"
#अन्तर्भाव "spektrum_rssi.h"
#अन्तर्भाव "common_rc.h"
#अन्तर्भाव <drivers/drv_hrt.h>

#अन्तर्भाव <include/containers/Bitset.hpp>

#यदि defined(__PX4_NUTTX)
#अन्तर्भाव <nuttx/arch.h>
#define dsm_udelay(arg)    up_udelay(arg)
#अन्यथा
#define dsm_udelay(arg) px4_usleep(arg)
#endif

// #define DSM_DEBUG

स्थिर गणना DSM_DECODE_STATE {
	DSM_DECODE_STATE_DESYNC = 0,
	DSM_DECODE_STATE_SYNC
} dsm_decode_state = DSM_DECODE_STATE_DESYNC;

प्रकारनिर्देश संरचना {
	hrt_abstime last_seen;
	uint16_t value;
} dsm_channel_t;

स्थिर dsm_channel_t channel_buffer[DSM_MAX_CHANNEL_गन्ती];

स्थिर पूर्णाङ्क dsm_fd = -1;						/**< File handle to the DSM UART */
स्थिर hrt_abstime dsm_last_rx_time;            /**< Timestamp when we last received data */
स्थिर hrt_abstime dsm_last_frame_time;		/**< Timestamp for start of last valid dsm frame */
स्थिर dsm_frame_t &dsm_frame = rc_decode_buf.dsm.frame;	/**< DSM_BUFFER_SIZE DSM dsm frame receive buffer */
स्थिर dsm_buf_t &dsm_buf = rc_decode_buf.dsm.buf;	/**< DSM_BUFFER_SIZE DSM dsm frame receive buffer */

स्थिर अचिन्हित dsm_partial_frame_गन्ती;	/**< Count of bytes received for current dsm frame */
स्थिर अचिन्हित dsm_channel_shift = 0;			/**< Channel resolution, 0=unknown, 10=10 bit (1024), 11=11 bit (2048) */
स्थिर अचिन्हित dsm_frame_drops = 0;			/**< Count of incomplete DSM frames */
स्थिर uint16_t dsm_chan_गन्ती = 0;         /**< DSM channel count */

स्थिर constexpr uint8_t CHANNEL_UNUSED = 0xFF;

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
स्थिर तार्किक dsm_decode_channel(uint16_t raw, अचिन्हित shift, uint8_t &channel, uint16_t &value)
{
	यदि (shift == 10) {
		// 1024 Mode: This format is used only by DSM2/22ms mode. All other modes use 2048 data.
		//  Bits 15-10 Channel ID
		//  Bits 9-0   Servo Position
		स्थिर constexpr uint16_t MASK_1024_CHANID = 0xFC00;
		स्थिर constexpr uint16_t MASK_1024_SXPOS = 0x03FF;

		channel = (raw & MASK_1024_CHANID) >> 10; // 6 bits

		अचर uint16_t servo_position = (raw & MASK_1024_SXPOS); // 10 bits

		यदि (channel >= DSM_MAX_CHANNEL_गन्ती) {
			PX4_DEBUG("invalid channel: %d\n", channel);
			फर्काऊ असत्य;
		}

		// PWM_OUT = (ServoPosition x 1.166μs) + Offset
		स्थिर constexpr uint16_t offset = 903; // microseconds
		value = roundf(servo_position * 1.166f) + offset;

		फर्काऊ सत्य;

	} अन्यथा यदि (shift == 11) {
		// 2048 Mode
		//  Bits 15    Servo Phase
		//  Bits 14-11 Channel ID
		//  Bits 10-0  Servo Position

		uint16_t servo_position = 0;

		// from Spektrum Remote Receiver Interfacing Rev G Page 6
		अचर तार्किक phase = (raw >> 15) & 0x01; // the phase is part of the X-Plus address (bit 15)
		uint8_t chan = (raw >> 11) & 0x0F;

		यदि (chan < 12) {
			// Normal channels
			स्थिर constexpr uint16_t MASK_2048_SXPOS = 0x07FF;
			servo_position = (raw & MASK_2048_SXPOS);

		} अन्यथा यदि (chan == 12) {
			// XPlus channels
			chan += ((raw >> 9) & 0x03);

			यदि (phase) {
				chan += 4;
			}

			यदि (chan >= DSM_MAX_CHANNEL_गन्ती) {
				फर्काऊ असत्य;
			}

			servo_position = (raw & 0x01FF) << 2;

			channel = chan;

		} अन्यथा यदि (chan == 0x0F && phase) {
			// When the channel content is all bits set, and the phase is also set, this is the blank channel
			channel = CHANNEL_UNUSED;
			फर्काऊ सत्य;

		} अन्यथा {
			// This will be the case for an unused channel (raw 16bit content will be 0xffff)
			PX4_DEBUG("invalid: %d %d\n", chan, phase);
			फर्काऊ असत्य;
		}

		channel = chan;

		// PWM_OUT = (ServoPosition x 0.583μs) + Offset
		स्थिर constexpr uint16_t offset = 903; // microseconds
		value = roundf(servo_position * 0.583f) + offset;

		PX4_DEBUG(मानक_त्रुटि, "CH%d=%d(0x%02x), ", channel, value, raw);

		फर्काऊ सत्य;
	}

	फर्काऊ असत्य;
}

/**
 * Attempt to guess if receiving 10 or 11 bit channel values
 *
 * @param[in] reset true=reset the 10/11 bit state to unknown
 */
स्थिर तार्किक dsm_guess_format(तार्किक reset)
{
	स्थिर uint32_t cs10 = 0;
	स्थिर uint32_t cs11 = 0;
	स्थिर uint16_t good_cs10_frame_गन्ती = 0;
	स्थिर uint16_t good_cs11_frame_गन्ती = 0;
	स्थिर uint16_t samples = 0;
	स्थिर uint16_t seen_channels_गन्ती_cs11[DSM_MAX_CHANNEL_गन्ती] {};
	स्थिर uint16_t seen_channels_गन्ती_cs10[DSM_MAX_CHANNEL_गन्ती] {};

	/* reset the 10/11 bit sniffed channel masks */
	यदि (reset) {
		PX4_DEBUG("dsm_guess_format reset");
		cs10 = 0;
		cs11 = 0;
		samples = 0;

		dsm_channel_shift = 0;
		good_cs10_frame_गन्ती = 0;
		good_cs11_frame_गन्ती = 0;

		लागि (अचिन्हित i = 0; i < DSM_MAX_CHANNEL_गन्ती; i++) {
			seen_channels_गन्ती_cs10[i] = 0;
			seen_channels_गन्ती_cs11[i] = 0;
		}

		फर्काऊ असत्य;
	}

	px4::Bitset<DSM_MAX_CHANNEL_गन्ती> channels_found_10;
	px4::Bitset<DSM_MAX_CHANNEL_गन्ती> channels_found_11;

	तार्किक cs10_frame_valid = सत्य;
	तार्किक cs11_frame_valid = सत्य;

	/* scan the channels in the current dsm_frame in both 10- and 11-bit mode */
	लागि (अचिन्हित i = 0; i < DSM_FRAME_CHANNELS; i++) {

		uint8_t *dp = &dsm_frame[2 + (2 * i)];
		uint16_t raw = (dp[0] << 8) | dp[1];

		uint8_t channel = 0;
		uint16_t value = 0;

		/* if the channel decodes, remember the assigned number */
		यदि (dsm_decode_channel(raw, 10, channel, value)) {
			यदि (channel == CHANNEL_UNUSED) {
				जारी;
			}

			// invalidate entire frame (for 1024) if channel already found, no duplicate channels per DSM frame
			यदि (channels_found_10[channel]) {
				cs10_frame_valid = असत्य;

			} अन्यथा {
				seen_channels_गन्ती_cs10[channel]++;
				channels_found_10.set(channel);
			}
		}

		यदि (dsm_decode_channel(raw, 11, channel, value)) {
			यदि (channel == CHANNEL_UNUSED) {
				जारी;
			}

			// invalidate entire frame (for 2048) if channel already found, no duplicate channels per DSM frame
			यदि (channels_found_11[channel]) {
				cs11_frame_valid = असत्य;

			} अन्यथा {
				seen_channels_गन्ती_cs11[channel]++;

				channels_found_11.set(channel);
			}
		}
	}

	// add valid cs10 channels
	यदि (cs10_frame_valid) {
		good_cs10_frame_गन्ती++;

		लागि (अचिन्हित channel = 0; channel < DSM_MAX_CHANNEL_गन्ती; channel++) {
			यदि (channels_found_10[channel]) {
				cs10 |= 1 << channel;
			}
		}
	}

	// add valid cs11 channels
	यदि (cs11_frame_valid) {
		good_cs11_frame_गन्ती++;

		लागि (अचिन्हित channel = 0; channel < DSM_MAX_CHANNEL_गन्ती; channel++) {
			यदि (channels_found_11[channel]) {
				cs11 |= 1 << channel;
			}
		}
	}

	samples++;

	/* wait until we have seen plenty of frames */
	यदि (samples < 15) {
		फर्काऊ असत्य;
	}

	/*
	* 10 or 11 bit decoding guess requirements
	*  For CS10 or CS11...
	*  At least `samples - bad_samples_allowance` must decode correctly (no duplicates, valid channel ranges for CS10)
	*  Even distribution of all found channels
	*  Channels must begin at 0 and no channel gaps after that (ie, the last channel is the one before the first unseen channel starting from zero)
	 */

	तार्किक cs10_channel_gap_found = असत्य;
	तार्किक cs11_channel_gap_found = असत्य;
	तार्किक valid_channel_counts_cs10 = असत्य;
	तार्किक valid_channel_counts_cs11 = असत्य;
	uint32_t cs10_channel_गन्ती = 0;
	uint32_t cs11_channel_गन्ती = 0;
	तार्किक found_channels_end = असत्य;

	// Count of allowed bad frames
	स्थिर constexpr uint16_t bad_samples_allowance = 5;
	स्थिर constexpr uint16_t minimum_channel_गन्ती = 5;
	स्थिर constexpr uint16_t minimum_channel_seen_गन्ती = 4;

	// Check for continous channels in 10bit decoding
	found_channels_end = असत्य;

	लागि (अचिन्हित i = 0; i < DSM_MAX_CHANNEL_गन्ती; i++) {
		// This bit is set
		यदि (cs10 & (1 << i)) {
			यदि (found_channels_end) {
				// Channel gap found
				cs10_channel_gap_found = सत्य;
				रोक;
			}

		} अन्यथा {
			यदि (!found_channels_end) {
				cs10_channel_गन्ती = i;
			}

			found_channels_end = सत्य;
		}
	}

	// Check for continous channels in 11bit decoding
	found_channels_end = असत्य;

	लागि (अचिन्हित i = 0; i < DSM_MAX_CHANNEL_गन्ती; i++) {
		// This bit is set
		यदि (cs11 & (1 << i)) {
			यदि (found_channels_end) {
				// Channel gap found
				cs11_channel_gap_found = सत्य;
				रोक;
			}

		} अन्यथा {
			यदि (!found_channels_end) {
				cs11_channel_गन्ती = i;
			}

			found_channels_end = सत्य;
		}
	}

	// Check channel seen counts C10
	यदि (cs10_channel_गन्ती && !cs10_channel_gap_found) {
		valid_channel_counts_cs10 = सत्य;

		लागि (अचिन्हित i = 0; i < ((cs10_channel_गन्ती > 12) ? 12 : cs10_channel_गन्ती); i++) {
			यदि (seen_channels_गन्ती_cs10[i] < minimum_channel_seen_गन्ती) {
				valid_channel_counts_cs10 = असत्य;
				रोक;
			}

		}
	}

	// Check channel seen counts C11
	यदि (cs11_channel_गन्ती && !cs11_channel_gap_found) {
		valid_channel_counts_cs11 = सत्य;

		लागि (अचिन्हित i = 0; i < ((cs11_channel_गन्ती > 12) ? 12 : cs11_channel_गन्ती); i++) {
			यदि (seen_channels_गन्ती_cs11[i] < minimum_channel_seen_गन्ती) {
				valid_channel_counts_cs11 = असत्य;
				रोक;
			}

		}
	}

#ifdef DSM_DEBUG
	म_लिखो("DSM guess: CS10 (%li good frames, %i gap found, %li channel count, %i dist)\r\n", good_cs10_frame_गन्ती,
	       cs10_channel_gap_found, cs10_channel_गन्ती, valid_channel_counts_cs10);
	म_लिखो("DSM guess: CS11 (%li good frames, %i gap found, %li channel count, %i dist)\r\n", good_cs11_frame_गन्ती,
	       cs11_channel_gap_found, cs11_channel_गन्ती, valid_channel_counts_cs11);
#endif

	यदि (good_cs11_frame_गन्ती > samples - bad_samples_allowance && !cs11_channel_gap_found
	    && cs11_channel_गन्ती >= minimum_channel_गन्ती && valid_channel_counts_cs11) {
#ifdef DSM_DEBUG
		म_लिखो("DSM guess: CS11 guessed!\n");
#endif
		dsm_chan_गन्ती = cs11_channel_गन्ती;
		dsm_channel_shift = 11;
		फर्काऊ सत्य;
	}

	यदि (good_cs10_frame_गन्ती > samples - bad_samples_allowance && !cs10_channel_gap_found
	    && cs10_channel_गन्ती >= minimum_channel_गन्ती && valid_channel_counts_cs10) {
#ifdef DSM_DEBUG
		म_लिखो("DSM guess: CS10 guessed!\n");
#endif
		dsm_chan_गन्ती = cs10_channel_गन्ती;
		dsm_channel_shift = 10;
		फर्काऊ सत्य;
	}

	/* call ourselves to reset our state ... we have to try again */
#ifdef DSM_DEBUG
	म_लिखो("DSM: format detect fail\n");
#endif
	dsm_guess_format(सत्य);
	फर्काऊ असत्य;
}

पूर्णाङ्क dsm_config(पूर्णाङ्क fd)
{
#ifdef SPEKTRUM_POWER_CONFIG
	// Enable power controls for Spektrum receiver
	SPEKTRUM_POWER_CONFIG();
#endif
#ifdef SPEKTRUM_POWER
	// enable power on DSM connector
	SPEKTRUM_POWER(सत्य);
#endif

	पूर्णाङ्क ret = -1;

	यदि (fd >= 0) {

		संरचना termios t;

		/* 115200bps, no parity, one stop bit */
		tcgetattr(fd, &t);
		cfsetspeed(&t, 115200);
		t.c_cflag &= ~(CSTOPB | PARENB);
		tcsetattr(fd, TCSANOW, &t);

		/* initialise the decoder */
		dsm_partial_frame_गन्ती = 0;
		dsm_last_rx_time = hrt_absolute_time();

		/* reset the format detector */
		dsm_guess_format(सत्य);

		ret = 0;
	}

	फर्काऊ ret;
}

शून्य dsm_proto_init()
{
	dsm_channel_shift = 0;
	dsm_frame_drops = 0;
	dsm_chan_गन्ती = 0;

	dsm_decode_state = DSM_DECODE_STATE_DESYNC;

	लागि (अचिन्हित i = 0; i < DSM_MAX_CHANNEL_गन्ती; i++) {
		channel_buffer[i].last_seen = 0;
		channel_buffer[i].value = 0;
	}

	/* reset the format detector */
	dsm_guess_format(सत्य);
}

/**
 * Initialize the DSM receive functionality
 *
 * Open the UART for receiving DSM frames and configure it appropriately
 *
 * @param[in] device Device name of DSM UART
 */
पूर्णाङ्क dsm_init(अचर वर्ण *device)
{
	यदि (dsm_fd < 0) {
		dsm_fd = खोलो(device, O_RDWR | O_NONBLOCK);
	}

	dsm_proto_init();

	पूर्णाङ्क ret = dsm_config(dsm_fd);

	यदि (!ret) {
		फर्काऊ dsm_fd;

	} अन्यथा {
		फर्काऊ -1;
	}
}

शून्य dsm_deinit()
{
#ifdef SPEKTRUM_POWER_PASSIVE
	// Turn power controls to passive
	SPEKTRUM_POWER_PASSIVE();
#endif

	यदि (dsm_fd >= 0) {
		बंद(dsm_fd);
	}

	dsm_fd = -1;
}

#यदि defined(SPEKTRUM_POWER)
/**
 * Handle DSM satellite receiver bind mode handler
 *
 * @param[in] cmd commands - dsm_bind_power_down, dsm_bind_power_up, dsm_bind_set_rx_out, dsm_bind_send_pulses, dsm_bind_reinit_uart
 * @param[in] pulses Number of pulses for dsm_bind_send_pulses command
 */
शून्य dsm_bind(uint16_t cmd, पूर्णाङ्क pulses)
{
	यदि (dsm_fd < 0) {
		फर्काऊ;
	}

	विकल्प (cmd) {
	पक्ष DSM_CMD_BIND_POWER_DOWN:
		// power down DSM satellite
#यदि defined(DSM_DEBUG)
		म_लिखो("DSM: DSM_CMD_BIND_POWER_DOWN\n");
#endif
		SPEKTRUM_POWER(असत्य);
		रोक;

	पक्ष DSM_CMD_BIND_POWER_UP:
		// power up DSM satellite
#यदि defined(DSM_DEBUG)
		म_लिखो("DSM: DSM_CMD_BIND_POWER_UP\n");
#endif
		SPEKTRUM_POWER(सत्य);
		dsm_guess_format(सत्य);
		रोक;

	पक्ष DSM_CMD_BIND_SET_RX_OUT:
		// Set UART RX pin to active output mode
#यदि defined(DSM_DEBUG)
		म_लिखो("DSM: DSM_CMD_BIND_SET_RX_OUT\n");
#endif
		SPEKTRUM_RX_AS_GPIO_OUTPUT();
		रोक;

	पक्ष DSM_CMD_BIND_SEND_PULSES:
		// Pulse RX pin a number of times
#यदि defined(DSM_DEBUG)
		म_लिखो("DSM: DSM_CMD_BIND_SEND_PULSES\n");
#endif

		लागि (पूर्णाङ्क i = 0; i < pulses; i++) {
			dsm_udelay(120);
			SPEKTRUM_OUT(असत्य);
			dsm_udelay(120);
			SPEKTRUM_OUT(सत्य);
		}

		रोक;

	पक्ष DSM_CMD_BIND_REINIT_UART:
		// Restore USART RX pin to RS232 receive mode
#यदि defined(DSM_DEBUG)
		म_लिखो("DSM: DSM_CMD_BIND_REINIT_UART\n");
#endif
		SPEKTRUM_RX_AS_UART();
		रोक;

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
तार्किक dsm_decode(hrt_abstime frame_time, तार्किक *dsm_11_bit, अचिन्हित channel_गन्ती,
		int8_t *rssi_percent)
{
	// If we haven't seen a new frame recently or haven't guessed the DSM encoding, try to guess
	यदि (((frame_time - dsm_last_frame_time) > 1000000) && (dsm_channel_shift != 0)) {
		dsm_guess_format(सत्य);
	}

	/* if we don't know the dsm_frame format, update the guessing state machine */
	यदि (dsm_channel_shift == 0) {
		यदि (!dsm_guess_format(असत्य)) {
			फर्काऊ असत्य;
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
	यदि (rssi_percent) {
		यदि (((int8_t *)dsm_frame)[0] < 0) {
			/*
			 * RSSI is a signed integer between -42dBm and -92dBm
			 * If signal is lost, the value is -128
			 */
			int8_t dbm = (int8_t)dsm_frame[0];

			यदि (dbm == -128) {
				*rssi_percent = 0;

			} अन्यथा {
				*rssi_percent = spek_dbm_to_percent(dbm);
			}

		} अन्यथा {
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

	px4::Bitset<DSM_MAX_CHANNEL_गन्ती> channels_found;

	लागि (अचिन्हित i = 0; i < DSM_FRAME_CHANNELS; i++) {

		uint8_t *dp = &dsm_frame[2 + (2 * i)];
		uint16_t raw = (dp[0] << 8) | dp[1];

		uint8_t channel = 0;
		uint16_t value = 0;

		यदि (!dsm_decode_channel(raw, dsm_channel_shift, channel, value)) {
			dsm_frame_drops++;
			फर्काऊ असत्य;
		}

		यदि (channel == CHANNEL_UNUSED) {
			जारी;
		}

		// abort if channel already found, no duplicate channels per DSM frame
		यदि (channels_found[channel]) {
			PX4_DEBUG("duplicate channel %d\n\n", channel);
			dsm_guess_format(सत्य);
			dsm_frame_drops++;
			फर्काऊ असत्य;

		} अन्यथा {
			channels_found.set(channel);
		}

		/* reset bit guessing state machine if the channel index is out of bounds */
		यदि (channel >= DSM_MAX_CHANNEL_गन्ती || channel >= channel_गन्ती) {
			PX4_DEBUG("channel %d > %d (DSM_MAX_CHANNEL_COUNT)", channel, DSM_MAX_CHANNEL_गन्ती);
			dsm_guess_format(सत्य);
			dsm_frame_drops++;
			फर्काऊ असत्य;
		}

		/*
		 * Store the decoded channel into the R/C input buffer, taking into
		 * account the different ideas about channel assignement that we have.
		 *
		 * Specifically, the first four channels in rc_channel_data are roll, pitch, thrust, yaw,
		 * but the first four channels from the DSM receiver are thrust, roll, pitch, yaw.
		 */
		विकल्प (channel) {
		पक्ष 0:
			channel = 2; // Spektrum Throttle (0) -> 2
			रोक;

		पक्ष 1:
			channel = 0; // Spektrum Aileron (1) -> 0
			रोक;

		पक्ष 2:
			channel = 1; // Spektrum Elevator (2) -> 1

		स्वभाव:
			रोक;
		}

		channel_buffer[channel].value = value;
		channel_buffer[channel].last_seen = frame_time;
	}

	/* Set the 11-bit data indicator */
	*dsm_11_bit = (dsm_channel_shift == 11);

	/* we have received something we think is a dsm_frame */
	dsm_last_frame_time = frame_time;

#ifdef DSM_DEBUG
	म_लिखो("PARSED PACKET\n");
#endif

	फर्काऊ सत्य;
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
तार्किक dsm_आगत(पूर्णाङ्क fd, uint16_t *मानहरू_2, uint16_t *अङ्क_मानहरू, तार्किक *dsm_11_bit, uint8_t *n_bytes, uint8_t **bytes,
	       int8_t *rssi, अचिन्हित *frame_drops, अचिन्हित max_मानहरू)
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
	अचर hrt_abstime now = hrt_absolute_time();

	/*
	 * Fetch bytes, but no more than we would need to complete
	 * a complete frame.
	 */

	पूर्णाङ्क ret = पठ(fd, &dsm_buf[0], परिमाण(dsm_buf) / परिमाण(dsm_buf[0]));

	/* if the read failed for any reason, just give up here */
	यदि (ret < 1) {
		फर्काऊ असत्य;

	} अन्यथा {
		*n_bytes = ret;
		*bytes = &dsm_buf[0];
	}

	/*
	 * Try to decode something with what we got
	 */
	फर्काऊ dsm_parse(now, &dsm_buf[0], ret, मानहरू_2, अङ्क_मानहरू, dsm_11_bit, &dsm_frame_drops, rssi, max_मानहरू);
}

तार्किक dsm_parse(अचर uint64_t now, अचर uint8_t *frame, अचर अचिन्हित len, uint16_t *मानहरू_2,
	       uint16_t *अङ्क_मानहरू, तार्किक *dsm_11_bit, अचिन्हित *frame_drops, int8_t *rssi_percent, uint16_t max_channels)
{
	/* this is set by the decoding state machine and will default to false
	 * once everything that was decodable has been decoded.
	 */
	तार्किक channel_data_available = असत्य;
	अचिन्हित channel_output_गन्ती;

	/* keep decoding until we have consumed the buffer */
	लागि (अचिन्हित d = 0; d < len; d++) {

		/* overflow check */
		यदि (dsm_partial_frame_गन्ती == परिमाण(dsm_frame) / परिमाण(dsm_frame[0])) {
			dsm_partial_frame_गन्ती = 0;
			dsm_decode_state = DSM_DECODE_STATE_DESYNC;
#ifdef DSM_DEBUG
			म_लिखो("DSM: RESET (BUF LIM)\n");
#endif
		}

		यदि (dsm_partial_frame_गन्ती == DSM_FRAME_SIZE) {
			dsm_partial_frame_गन्ती = 0;
			dsm_decode_state = DSM_DECODE_STATE_DESYNC;
#ifdef DSM_DEBUG
			म_लिखो("DSM: RESET (PACKET LIM)\n");
#endif
		}

#ifdef DSM_DEBUG
#यदि 0
		म_लिखो("dsm state: %s%s, count: %d, val: %02x\n",
		       (dsm_decode_state == DSM_DECODE_STATE_DESYNC) ? "DSM_DECODE_STATE_DESYNC" : "",
		       (dsm_decode_state == DSM_DECODE_STATE_SYNC) ? "DSM_DECODE_STATE_SYNC" : "",
		       dsm_partial_frame_गन्ती,
		       (अचिन्हित)frame[d]);
#endif
#endif

		विकल्प (dsm_decode_state) {
		पक्ष DSM_DECODE_STATE_DESYNC:

			/* we are de-synced and only interested in the frame marker */
			यदि ((now - dsm_last_rx_time) > 5000) {
				dsm_decode_state = DSM_DECODE_STATE_SYNC;
				dsm_partial_frame_गन्ती = 0;
				dsm_frame[dsm_partial_frame_गन्ती++] = frame[d];
			}

			रोक;

		पक्ष DSM_DECODE_STATE_SYNC: {
				dsm_frame[dsm_partial_frame_गन्ती++] = frame[d];

				/* decode whatever we got and expect */
				यदि (dsm_partial_frame_गन्ती < DSM_FRAME_SIZE) {
					रोक;
				}

				// We've collected a full frame, parse it
				channel_data_available = dsm_decode(now, dsm_11_bit, dsm_chan_गन्ती, rssi_percent);

				// Frame consumed, reset the buffer count
				dsm_partial_frame_गन्ती = 0;

				/* if decoding failed, set proto to desync */
				यदि (!channel_data_available) {
					dsm_decode_state = DSM_DECODE_STATE_DESYNC;
				}
			}
			रोक;

		स्वभाव:
#ifdef DSM_DEBUG
			म_लिखो("UNKNOWN PROTO STATE");
#endif
			channel_data_available = असत्य;
		}
	}

	यदि (frame_drops) {
		*frame_drops = dsm_frame_drops;
	}

	यदि (channel_data_available) {
		// Check if any channels have been absent for too long
		लागि (अचिन्हित i = 0; i < dsm_chan_गन्ती; i++) {
			यदि (now - channel_buffer[i].last_seen > 50 * 1000) {
				फर्काऊ असत्य;
			}
		}

		// Overflow protect
		यदि (max_channels >= DSM_MAX_CHANNEL_गन्ती) {
			max_channels = DSM_MAX_CHANNEL_गन्ती;
		}

		// Cap returned channels to max_channels if we've seen more
		यदि (dsm_chan_गन्ती > max_channels) {
			channel_output_गन्ती = max_channels;

		} अन्यथा {
			channel_output_गन्ती = dsm_chan_गन्ती;
		}

		*अङ्क_मानहरू = channel_output_गन्ती;

		// Copy the full list of collected channels
		लागि (अचिन्हित i = 0; i < channel_output_गन्ती; i++) {
			// Channels which do not have a value yet (because their frame has not been seen yet)
			// will have a zero. If we come across a blank channel, keep storing values, but we
			// won't return that we have a valid frame just yet.
			यदि (channel_buffer[i].value) {
				मानहरू_2[i] = channel_buffer[i].value;

			} अन्यथा {
				channel_data_available = असत्य;
			}
		}
	}

#ifdef DSM_DEBUG

	यदि (channel_data_available) {
		म_लिखो("frame drops: %u, chan #: %u\n", dsm_frame_drops, dsm_chan_गन्ती);

		लागि (अचिन्हित i = 0; i < dsm_chan_गन्ती; i++) {
			म_लिखो("dsm_decode: #CH %02u: %u\n", i + 1, मानहरू_2[i]);
		}
	}

#endif

	dsm_last_rx_time = now;

	/* return false as default */
	फर्काऊ channel_data_available;
}
