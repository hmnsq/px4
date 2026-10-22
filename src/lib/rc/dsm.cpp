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

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/px4_config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <board_config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/log.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <termios.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "dsm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "spektrum_rssi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "common_rc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_hrt.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <include/containers/Bitset.hpp>

#ਜੇ defined(__PX4_NUTTX)
#ਅਨ੍ਤਰ੍ਭਾਵ <nuttx/arch.h>
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ dsm_udelay(arg)    ਉੱਪਰ_udelay(arg)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ dsm_udelay(arg) px4_usleep(arg)
#ਜੇ_ਅੰਤ

// #define DSM_DEBUG

ਸਥਿਰ ਗਿਣਤੀ DSM_DECODE_STATE {
	DSM_DECODE_STATE_DESYNC = 0,
	DSM_DECODE_STATE_SYNC
} dsm_decode_state = DSM_DECODE_STATE_DESYNC;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
	hrt_abstime ਆਖਰੀ_ਵੇਖਿਆ;
	uint16_t value;
} dsm_channel_t;

ਸਥਿਰ dsm_channel_t channel_ਬਫਰ[DSM_MAX_CHANNEL_ਗਿਣਤੀ];

ਸਥਿਰ ਪੂਰਨ dsm_fd = -1;						/**< File handle to the DSM UART */
ਸਥਿਰ hrt_abstime dsm_ਆਖਰੀ_rx_ਸਮਾਂ;            /**< Timestamp when we last received data */
ਸਥਿਰ hrt_abstime dsm_ਆਖਰੀ_ਚੌਖਟਾ_ਸਮਾਂ;		/**< Timestamp for start of last valid dsm frame */
ਸਥਿਰ dsm_ਚੌਖਟਾ_t &dsm_ਚੌਖਟਾ = rc_decode_buf.dsm.ਚੌਖਟਾ;	/**< DSM_BUFFER_SIZE DSM dsm frame receive buffer */
ਸਥਿਰ dsm_buf_t &dsm_buf = rc_decode_buf.dsm.buf;	/**< DSM_BUFFER_SIZE DSM dsm frame receive buffer */

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ dsm_partial_ਚੌਖਟਾ_ਗਿਣਤੀ;	/**< Count of bytes received for current dsm frame */
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ dsm_channel_ਖਿਸਕਾਓ = 0;			/**< Channel resolution, 0=unknown, 10=10 bit (1024), 11=11 bit (2048) */
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ dsm_ਚੌਖਟਾ_drops = 0;			/**< Count of incomplete DSM frames */
ਸਥਿਰ uint16_t dsm_chan_ਗਿਣਤੀ = 0;         /**< DSM channel count */

ਸਥਿਰ constexpr uint8_t CHANNEL_ਅਣਵਰਤਿਆ = 0xFF;

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
ਸਥਿਰ ਤਾਰਕਿਕ dsm_decode_channel(uint16_t raw, ਬਿਨਾਨਿਸ਼ਾਨੀ ਖਿਸਕਾਓ, uint8_t &channel, uint16_t &value)
{
	ਜੇ (ਖਿਸਕਾਓ == 10) {
		// 1024 Mode: This format is used only by DSM2/22ms mode. All other modes use 2048 data.
		//  Bits 15-10 Channel ID
		//  Bits 9-0   Servo Position
		ਸਥਿਰ constexpr uint16_t ਨਕਾਬ_1024_CHANID = 0xFC00;
		ਸਥਿਰ constexpr uint16_t ਨਕਾਬ_1024_SXPOS = 0x03FF;

		channel = (raw & ਨਕਾਬ_1024_CHANID) >> 10; // 6 bits

		ਅਚਲ uint16_t servo_position = (raw & ਨਕਾਬ_1024_SXPOS); // 10 bits

		ਜੇ (channel >= DSM_MAX_CHANNEL_ਗਿਣਤੀ) {
			PX4_DEBUG("invalid channel: %d\n", channel);
			ਵਾਪਸ ਝੂਠ;
		}

		// PWM_OUT = (ServoPosition x 1.166μs) + Offset
		ਸਥਿਰ constexpr uint16_t ਵਿੱਥ = 903; // microseconds
		value = roundf(servo_position * 1.166f) + ਵਿੱਥ;

		ਵਾਪਸ ਸੱਚ;

	} ਨਹੀਂਤਾਂ ਜੇ (ਖਿਸਕਾਓ == 11) {
		// 2048 Mode
		//  Bits 15    Servo Phase
		//  Bits 14-11 Channel ID
		//  Bits 10-0  Servo Position

		uint16_t servo_position = 0;

		// from Spektrum Remote Receiver Interfacing Rev G Page 6
		ਅਚਲ ਤਾਰਕਿਕ phase = (raw >> 15) & 0x01; // the phase is part of the X-Plus address (bit 15)
		uint8_t chan = (raw >> 11) & 0x0F;

		ਜੇ (chan < 12) {
			// Normal channels
			ਸਥਿਰ constexpr uint16_t ਨਕਾਬ_2048_SXPOS = 0x07FF;
			servo_position = (raw & ਨਕਾਬ_2048_SXPOS);

		} ਨਹੀਂਤਾਂ ਜੇ (chan == 12) {
			// XPlus channels
			chan += ((raw >> 9) & 0x03);

			ਜੇ (phase) {
				chan += 4;
			}

			ਜੇ (chan >= DSM_MAX_CHANNEL_ਗਿਣਤੀ) {
				ਵਾਪਸ ਝੂਠ;
			}

			servo_position = (raw & 0x01FF) << 2;

			channel = chan;

		} ਨਹੀਂਤਾਂ ਜੇ (chan == 0x0F && phase) {
			// When the channel content is all bits set, and the phase is also set, this is the blank channel
			channel = CHANNEL_ਅਣਵਰਤਿਆ;
			ਵਾਪਸ ਸੱਚ;

		} ਨਹੀਂਤਾਂ {
			// This will be the case for an unused channel (raw 16bit content will be 0xffff)
			PX4_DEBUG("invalid: %d %d\n", chan, phase);
			ਵਾਪਸ ਝੂਠ;
		}

		channel = chan;

		// PWM_OUT = (ServoPosition x 0.583μs) + Offset
		ਸਥਿਰ constexpr uint16_t ਵਿੱਥ = 903; // microseconds
		value = roundf(servo_position * 0.583f) + ਵਿੱਥ;

		PX4_DEBUG(stderr, "CH%d=%d(0x%02x), ", channel, value, raw);

		ਵਾਪਸ ਸੱਚ;
	}

	ਵਾਪਸ ਝੂਠ;
}

/**
 * Attempt to guess if receiving 10 or 11 bit channel values
 *
 * @param[in] reset true=reset the 10/11 bit state to unknown
 */
ਸਥਿਰ ਤਾਰਕਿਕ dsm_guess_format(ਤਾਰਕਿਕ reset)
{
	ਸਥਿਰ uint32_t cs10 = 0;
	ਸਥਿਰ uint32_t cs11 = 0;
	ਸਥਿਰ uint16_t good_cs10_ਚੌਖਟਾ_ਗਿਣਤੀ = 0;
	ਸਥਿਰ uint16_t good_cs11_ਚੌਖਟਾ_ਗਿਣਤੀ = 0;
	ਸਥਿਰ uint16_t samples = 0;
	ਸਥਿਰ uint16_t ਵੇਖਿਆ_channels_ਗਿਣਤੀ_cs11[DSM_MAX_CHANNEL_ਗਿਣਤੀ] {};
	ਸਥਿਰ uint16_t ਵੇਖਿਆ_channels_ਗਿਣਤੀ_cs10[DSM_MAX_CHANNEL_ਗਿਣਤੀ] {};

	/* reset the 10/11 bit sniffed channel masks */
	ਜੇ (reset) {
		PX4_DEBUG("dsm_guess_format reset");
		cs10 = 0;
		cs11 = 0;
		samples = 0;

		dsm_channel_ਖਿਸਕਾਓ = 0;
		good_cs10_ਚੌਖਟਾ_ਗਿਣਤੀ = 0;
		good_cs11_ਚੌਖਟਾ_ਗਿਣਤੀ = 0;

		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < DSM_MAX_CHANNEL_ਗਿਣਤੀ; i++) {
			ਵੇਖਿਆ_channels_ਗਿਣਤੀ_cs10[i] = 0;
			ਵੇਖਿਆ_channels_ਗਿਣਤੀ_cs11[i] = 0;
		}

		ਵਾਪਸ ਝੂਠ;
	}

	px4::Bitset<DSM_MAX_CHANNEL_ਗਿਣਤੀ> channels_ਮਿਲਿਆ_10;
	px4::Bitset<DSM_MAX_CHANNEL_ਗਿਣਤੀ> channels_ਮਿਲਿਆ_11;

	ਤਾਰਕਿਕ cs10_ਚੌਖਟਾ_valid = ਸੱਚ;
	ਤਾਰਕਿਕ cs11_ਚੌਖਟਾ_valid = ਸੱਚ;

	/* scan the channels in the current dsm_frame in both 10- and 11-bit mode */
	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < DSM_ਚੌਖਟਾ_CHANNELS; i++) {

		uint8_t *dp = &dsm_ਚੌਖਟਾ[2 + (2 * i)];
		uint16_t raw = (dp[0] << 8) | dp[1];

		uint8_t channel = 0;
		uint16_t value = 0;

		/* if the channel decodes, remember the assigned number */
		ਜੇ (dsm_decode_channel(raw, 10, channel, value)) {
			ਜੇ (channel == CHANNEL_ਅਣਵਰਤਿਆ) {
				ਜਾਰੀ;
			}

			// invalidate entire frame (for 1024) if channel already found, no duplicate channels per DSM frame
			ਜੇ (channels_ਮਿਲਿਆ_10[channel]) {
				cs10_ਚੌਖਟਾ_valid = ਝੂਠ;

			} ਨਹੀਂਤਾਂ {
				ਵੇਖਿਆ_channels_ਗਿਣਤੀ_cs10[channel]++;
				channels_ਮਿਲਿਆ_10.ਸੈੱਟ(channel);
			}
		}

		ਜੇ (dsm_decode_channel(raw, 11, channel, value)) {
			ਜੇ (channel == CHANNEL_ਅਣਵਰਤਿਆ) {
				ਜਾਰੀ;
			}

			// invalidate entire frame (for 2048) if channel already found, no duplicate channels per DSM frame
			ਜੇ (channels_ਮਿਲਿਆ_11[channel]) {
				cs11_ਚੌਖਟਾ_valid = ਝੂਠ;

			} ਨਹੀਂਤਾਂ {
				ਵੇਖਿਆ_channels_ਗਿਣਤੀ_cs11[channel]++;

				channels_ਮਿਲਿਆ_11.ਸੈੱਟ(channel);
			}
		}
	}

	// add valid cs10 channels
	ਜੇ (cs10_ਚੌਖਟਾ_valid) {
		good_cs10_ਚੌਖਟਾ_ਗਿਣਤੀ++;

		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ channel = 0; channel < DSM_MAX_CHANNEL_ਗਿਣਤੀ; channel++) {
			ਜੇ (channels_ਮਿਲਿਆ_10[channel]) {
				cs10 |= 1 << channel;
			}
		}
	}

	// add valid cs11 channels
	ਜੇ (cs11_ਚੌਖਟਾ_valid) {
		good_cs11_ਚੌਖਟਾ_ਗਿਣਤੀ++;

		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ channel = 0; channel < DSM_MAX_CHANNEL_ਗਿਣਤੀ; channel++) {
			ਜੇ (channels_ਮਿਲਿਆ_11[channel]) {
				cs11 |= 1 << channel;
			}
		}
	}

	samples++;

	/* wait until we have seen plenty of frames */
	ਜੇ (samples < 15) {
		ਵਾਪਸ ਝੂਠ;
	}

	/*
	* 10 or 11 bit decoding guess requirements
	*  For CS10 or CS11...
	*  At least `samples - bad_samples_allowance` must decode correctly (no duplicates, valid channel ranges for CS10)
	*  Even distribution of all found channels
	*  Channels must begin at 0 and no channel gaps after that (ie, the last channel is the one before the first unseen channel starting from zero)
	 */

	ਤਾਰਕਿਕ cs10_channel_gap_ਮਿਲਿਆ = ਝੂਠ;
	ਤਾਰਕਿਕ cs11_channel_gap_ਮਿਲਿਆ = ਝੂਠ;
	ਤਾਰਕਿਕ valid_channel_counts_cs10 = ਝੂਠ;
	ਤਾਰਕਿਕ valid_channel_counts_cs11 = ਝੂਠ;
	uint32_t cs10_channel_ਗਿਣਤੀ = 0;
	uint32_t cs11_channel_ਗਿਣਤੀ = 0;
	ਤਾਰਕਿਕ ਮਿਲਿਆ_channels_ਸਿਰਾ = ਝੂਠ;

	// Count of allowed bad frames
	ਸਥਿਰ constexpr uint16_t bad_samples_allowance = 5;
	ਸਥਿਰ constexpr uint16_t minimum_channel_ਗਿਣਤੀ = 5;
	ਸਥਿਰ constexpr uint16_t minimum_channel_ਵੇਖਿਆ_ਗਿਣਤੀ = 4;

	// Check for continous channels in 10bit decoding
	ਮਿਲਿਆ_channels_ਸਿਰਾ = ਝੂਠ;

	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < DSM_MAX_CHANNEL_ਗਿਣਤੀ; i++) {
		// This bit is set
		ਜੇ (cs10 & (1 << i)) {
			ਜੇ (ਮਿਲਿਆ_channels_ਸਿਰਾ) {
				// Channel gap found
				cs10_channel_gap_ਮਿਲਿਆ = ਸੱਚ;
				ਰੋਕੋ;
			}

		} ਨਹੀਂਤਾਂ {
			ਜੇ (!ਮਿਲਿਆ_channels_ਸਿਰਾ) {
				cs10_channel_ਗਿਣਤੀ = i;
			}

			ਮਿਲਿਆ_channels_ਸਿਰਾ = ਸੱਚ;
		}
	}

	// Check for continous channels in 11bit decoding
	ਮਿਲਿਆ_channels_ਸਿਰਾ = ਝੂਠ;

	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < DSM_MAX_CHANNEL_ਗਿਣਤੀ; i++) {
		// This bit is set
		ਜੇ (cs11 & (1 << i)) {
			ਜੇ (ਮਿਲਿਆ_channels_ਸਿਰਾ) {
				// Channel gap found
				cs11_channel_gap_ਮਿਲਿਆ = ਸੱਚ;
				ਰੋਕੋ;
			}

		} ਨਹੀਂਤਾਂ {
			ਜੇ (!ਮਿਲਿਆ_channels_ਸਿਰਾ) {
				cs11_channel_ਗਿਣਤੀ = i;
			}

			ਮਿਲਿਆ_channels_ਸਿਰਾ = ਸੱਚ;
		}
	}

	// Check channel seen counts C10
	ਜੇ (cs10_channel_ਗਿਣਤੀ && !cs10_channel_gap_ਮਿਲਿਆ) {
		valid_channel_counts_cs10 = ਸੱਚ;

		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < ((cs10_channel_ਗਿਣਤੀ > 12) ? 12 : cs10_channel_ਗਿਣਤੀ); i++) {
			ਜੇ (ਵੇਖਿਆ_channels_ਗਿਣਤੀ_cs10[i] < minimum_channel_ਵੇਖਿਆ_ਗਿਣਤੀ) {
				valid_channel_counts_cs10 = ਝੂਠ;
				ਰੋਕੋ;
			}

		}
	}

	// Check channel seen counts C11
	ਜੇ (cs11_channel_ਗਿਣਤੀ && !cs11_channel_gap_ਮਿਲਿਆ) {
		valid_channel_counts_cs11 = ਸੱਚ;

		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < ((cs11_channel_ਗਿਣਤੀ > 12) ? 12 : cs11_channel_ਗਿਣਤੀ); i++) {
			ਜੇ (ਵੇਖਿਆ_channels_ਗਿਣਤੀ_cs11[i] < minimum_channel_ਵੇਖਿਆ_ਗਿਣਤੀ) {
				valid_channel_counts_cs11 = ਝੂਠ;
				ਰੋਕੋ;
			}

		}
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DSM_DEBUG
	ਲਿਖੋ("DSM guess: CS10 (%li good frames, %i gap found, %li channel count, %i dist)\r\n", good_cs10_ਚੌਖਟਾ_ਗਿਣਤੀ,
	       cs10_channel_gap_ਮਿਲਿਆ, cs10_channel_ਗਿਣਤੀ, valid_channel_counts_cs10);
	ਲਿਖੋ("DSM guess: CS11 (%li good frames, %i gap found, %li channel count, %i dist)\r\n", good_cs11_ਚੌਖਟਾ_ਗਿਣਤੀ,
	       cs11_channel_gap_ਮਿਲਿਆ, cs11_channel_ਗਿਣਤੀ, valid_channel_counts_cs11);
#ਜੇ_ਅੰਤ

	ਜੇ (good_cs11_ਚੌਖਟਾ_ਗਿਣਤੀ > samples - bad_samples_allowance && !cs11_channel_gap_ਮਿਲਿਆ
	    && cs11_channel_ਗਿਣਤੀ >= minimum_channel_ਗਿਣਤੀ && valid_channel_counts_cs11) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DSM_DEBUG
		ਲਿਖੋ("DSM guess: CS11 guessed!\n");
#ਜੇ_ਅੰਤ
		dsm_chan_ਗਿਣਤੀ = cs11_channel_ਗਿਣਤੀ;
		dsm_channel_ਖਿਸਕਾਓ = 11;
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (good_cs10_ਚੌਖਟਾ_ਗਿਣਤੀ > samples - bad_samples_allowance && !cs10_channel_gap_ਮਿਲਿਆ
	    && cs10_channel_ਗਿਣਤੀ >= minimum_channel_ਗਿਣਤੀ && valid_channel_counts_cs10) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DSM_DEBUG
		ਲਿਖੋ("DSM guess: CS10 guessed!\n");
#ਜੇ_ਅੰਤ
		dsm_chan_ਗਿਣਤੀ = cs10_channel_ਗਿਣਤੀ;
		dsm_channel_ਖਿਸਕਾਓ = 10;
		ਵਾਪਸ ਸੱਚ;
	}

	/* call ourselves to reset our state ... we have to try again */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DSM_DEBUG
	ਲਿਖੋ("DSM: format detect fail\n");
#ਜੇ_ਅੰਤ
	dsm_guess_format(ਸੱਚ);
	ਵਾਪਸ ਝੂਠ;
}

ਪੂਰਨ dsm_config(ਪੂਰਨ fd)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SPEKTRUM_POWER_CONFIG
	// Enable power controls for Spektrum receiver
	SPEKTRUM_POWER_CONFIG();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SPEKTRUM_POWER
	// enable power on DSM connector
	SPEKTRUM_POWER(ਸੱਚ);
#ਜੇ_ਅੰਤ

	ਪੂਰਨ ret = -1;

	ਜੇ (fd >= 0) {

		ਬਣਤਰ termios t;

		/* 115200bps, no parity, one stop bit */
		tcgetattr(fd, &t);
		cfsetspeed(&t, 115200);
		t.c_cflag &= ~(CSTOPB | PARENB);
		tcsetattr(fd, TCSANOW, &t);

		/* initialise the decoder */
		dsm_partial_ਚੌਖਟਾ_ਗਿਣਤੀ = 0;
		dsm_ਆਖਰੀ_rx_ਸਮਾਂ = hrt_absolute_ਸਮਾਂ();

		/* reset the format detector */
		dsm_guess_format(ਸੱਚ);

		ret = 0;
	}

	ਵਾਪਸ ret;
}

ਖਾਲੀ dsm_proto_init()
{
	dsm_channel_ਖਿਸਕਾਓ = 0;
	dsm_ਚੌਖਟਾ_drops = 0;
	dsm_chan_ਗਿਣਤੀ = 0;

	dsm_decode_state = DSM_DECODE_STATE_DESYNC;

	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < DSM_MAX_CHANNEL_ਗਿਣਤੀ; i++) {
		channel_ਬਫਰ[i].ਆਖਰੀ_ਵੇਖਿਆ = 0;
		channel_ਬਫਰ[i].value = 0;
	}

	/* reset the format detector */
	dsm_guess_format(ਸੱਚ);
}

/**
 * Initialize the DSM receive functionality
 *
 * Open the UART for receiving DSM frames and configure it appropriately
 *
 * @param[in] device Device name of DSM UART
 */
ਪੂਰਨ dsm_init(ਅਚਲ ਅੱਖਰ *device)
{
	ਜੇ (dsm_fd < 0) {
		dsm_fd = open(device, O_RDWR | O_NONBLOCK);
	}

	dsm_proto_init();

	ਪੂਰਨ ret = dsm_config(dsm_fd);

	ਜੇ (!ret) {
		ਵਾਪਸ dsm_fd;

	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ -1;
	}
}

ਖਾਲੀ dsm_deinit()
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SPEKTRUM_POWER_PASSIVE
	// Turn power controls to passive
	SPEKTRUM_POWER_PASSIVE();
#ਜੇ_ਅੰਤ

	ਜੇ (dsm_fd >= 0) {
		close(dsm_fd);
	}

	dsm_fd = -1;
}

#ਜੇ defined(SPEKTRUM_POWER)
/**
 * Handle DSM satellite receiver bind mode handler
 *
 * @param[in] cmd commands - dsm_bind_power_down, dsm_bind_power_up, dsm_bind_set_rx_out, dsm_bind_send_pulses, dsm_bind_reinit_uart
 * @param[in] pulses Number of pulses for dsm_bind_send_pulses command
 */
ਖਾਲੀ dsm_bind(uint16_t cmd, ਪੂਰਨ pulses)
{
	ਜੇ (dsm_fd < 0) {
		ਵਾਪਸ;
	}

	ਚੋਣ (cmd) {
	ਹਾਲਤ DSM_CMD_BIND_POWER_ਥੱਲੇ:
		// power down DSM satellite
#ਜੇ defined(DSM_DEBUG)
		ਲਿਖੋ("DSM: DSM_CMD_BIND_POWER_DOWN\n");
#ਜੇ_ਅੰਤ
		SPEKTRUM_POWER(ਝੂਠ);
		ਰੋਕੋ;

	ਹਾਲਤ DSM_CMD_BIND_POWER_ਉੱਪਰ:
		// power up DSM satellite
#ਜੇ defined(DSM_DEBUG)
		ਲਿਖੋ("DSM: DSM_CMD_BIND_POWER_UP\n");
#ਜੇ_ਅੰਤ
		SPEKTRUM_POWER(ਸੱਚ);
		dsm_guess_format(ਸੱਚ);
		ਰੋਕੋ;

	ਹਾਲਤ DSM_CMD_BIND_ਸੈੱਟ_RX_ਬਾਹਰਲਾ:
		// Set UART RX pin to active output mode
#ਜੇ defined(DSM_DEBUG)
		ਲਿਖੋ("DSM: DSM_CMD_BIND_SET_RX_OUT\n");
#ਜੇ_ਅੰਤ
		SPEKTRUM_RX_AS_GPIO_OUTPUT();
		ਰੋਕੋ;

	ਹਾਲਤ DSM_CMD_BIND_SEND_PULSES:
		// Pulse RX pin a number of times
#ਜੇ defined(DSM_DEBUG)
		ਲਿਖੋ("DSM: DSM_CMD_BIND_SEND_PULSES\n");
#ਜੇ_ਅੰਤ

		ਲਈ (ਪੂਰਨ i = 0; i < pulses; i++) {
			dsm_udelay(120);
			SPEKTRUM_ਬਾਹਰਲਾ(ਝੂਠ);
			dsm_udelay(120);
			SPEKTRUM_ਬਾਹਰਲਾ(ਸੱਚ);
		}

		ਰੋਕੋ;

	ਹਾਲਤ DSM_CMD_BIND_REINIT_UART:
		// Restore USART RX pin to RS232 receive mode
#ਜੇ defined(DSM_DEBUG)
		ਲਿਖੋ("DSM: DSM_CMD_BIND_REINIT_UART\n");
#ਜੇ_ਅੰਤ
		SPEKTRUM_RX_AS_UART();
		ਰੋਕੋ;

	}
}
#ਜੇ_ਅੰਤ

/**
 * Decode the entire dsm frame (all contained channels)
 *
 * @param[in] frame_time timestamp when this dsm frame was received. Used to detect RX loss in order to reset 10/11 bit guess.
 * @param[out] num_values pointer to number of raw channel values returned
 * @return true=DSM frame successfully decoded, false=no update
 */
ਤਾਰਕਿਕ dsm_decode(hrt_abstime ਚੌਖਟਾ_ਸਮਾਂ, ਤਾਰਕਿਕ *dsm_11_ਦੋਆਧਾਰੀ_ਅੰਕ, ਬਿਨਾਨਿਸ਼ਾਨੀ channel_ਗਿਣਤੀ,
		int8_t *rssi_percent)
{
	// If we haven't seen a new frame recently or haven't guessed the DSM encoding, try to guess
	ਜੇ (((ਚੌਖਟਾ_ਸਮਾਂ - dsm_ਆਖਰੀ_ਚੌਖਟਾ_ਸਮਾਂ) > 1000000) && (dsm_channel_ਖਿਸਕਾਓ != 0)) {
		dsm_guess_format(ਸੱਚ);
	}

	/* if we don't know the dsm_frame format, update the guessing state machine */
	ਜੇ (dsm_channel_ਖਿਸਕਾਓ == 0) {
		ਜੇ (!dsm_guess_format(ਝੂਠ)) {
			ਵਾਪਸ ਝੂਠ;
		}

		// We have our first packet so we're in sync
		dsm_ਆਖਰੀ_rx_ਸਮਾਂ = hrt_absolute_ਸਮਾਂ();
	}

	/*
	 * The first byte represents the rssi in dBm on telemetry receivers with updated
	 * firmware, or fades on others. If the value is less than zero, it's rssi.
	 * We have other ways to detect bad link metrics, so ignore positive values,
	 * but rssi dBm is a useful value.
	 */

	// The SPM4649T with firmware version 1.1RC9 or later will have RSSI in place of fades
	ਜੇ (rssi_percent) {
		ਜੇ (((int8_t *)dsm_ਚੌਖਟਾ)[0] < 0) {
			/*
			 * RSSI is a signed integer between -42dBm and -92dBm
			 * If signal is lost, the value is -128
			 */
			int8_t dbm = (int8_t)dsm_ਚੌਖਟਾ[0];

			ਜੇ (dbm == -128) {
				*rssi_percent = 0;

			} ਨਹੀਂਤਾਂ {
				*rssi_percent = spek_dbm_ਮੰਜ਼ਿਲ_percent(dbm);
			}

		} ਨਹੀਂਤਾਂ {
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

	px4::Bitset<DSM_MAX_CHANNEL_ਗਿਣਤੀ> channels_ਮਿਲਿਆ;

	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < DSM_ਚੌਖਟਾ_CHANNELS; i++) {

		uint8_t *dp = &dsm_ਚੌਖਟਾ[2 + (2 * i)];
		uint16_t raw = (dp[0] << 8) | dp[1];

		uint8_t channel = 0;
		uint16_t value = 0;

		ਜੇ (!dsm_decode_channel(raw, dsm_channel_ਖਿਸਕਾਓ, channel, value)) {
			dsm_ਚੌਖਟਾ_drops++;
			ਵਾਪਸ ਝੂਠ;
		}

		ਜੇ (channel == CHANNEL_ਅਣਵਰਤਿਆ) {
			ਜਾਰੀ;
		}

		// abort if channel already found, no duplicate channels per DSM frame
		ਜੇ (channels_ਮਿਲਿਆ[channel]) {
			PX4_DEBUG("duplicate channel %d\n\n", channel);
			dsm_guess_format(ਸੱਚ);
			dsm_ਚੌਖਟਾ_drops++;
			ਵਾਪਸ ਝੂਠ;

		} ਨਹੀਂਤਾਂ {
			channels_ਮਿਲਿਆ.ਸੈੱਟ(channel);
		}

		/* reset bit guessing state machine if the channel index is out of bounds */
		ਜੇ (channel >= DSM_MAX_CHANNEL_ਗਿਣਤੀ || channel >= channel_ਗਿਣਤੀ) {
			PX4_DEBUG("channel %d > %d (DSM_MAX_CHANNEL_COUNT)", channel, DSM_MAX_CHANNEL_ਗਿਣਤੀ);
			dsm_guess_format(ਸੱਚ);
			dsm_ਚੌਖਟਾ_drops++;
			ਵਾਪਸ ਝੂਠ;
		}

		/*
		 * Store the decoded channel into the R/C input buffer, taking into
		 * account the different ideas about channel assignement that we have.
		 *
		 * Specifically, the first four channels in rc_channel_data are roll, pitch, thrust, yaw,
		 * but the first four channels from the DSM receiver are thrust, roll, pitch, yaw.
		 */
		ਚੋਣ (channel) {
		ਹਾਲਤ 0:
			channel = 2; // Spektrum Throttle (0) -> 2
			ਰੋਕੋ;

		ਹਾਲਤ 1:
			channel = 0; // Spektrum Aileron (1) -> 0
			ਰੋਕੋ;

		ਹਾਲਤ 2:
			channel = 1; // Spektrum Elevator (2) -> 1

		ਮੂਲ:
			ਰੋਕੋ;
		}

		channel_ਬਫਰ[channel].value = value;
		channel_ਬਫਰ[channel].ਆਖਰੀ_ਵੇਖਿਆ = ਚੌਖਟਾ_ਸਮਾਂ;
	}

	/* Set the 11-bit data indicator */
	*dsm_11_ਦੋਆਧਾਰੀ_ਅੰਕ = (dsm_channel_ਖਿਸਕਾਓ == 11);

	/* we have received something we think is a dsm_frame */
	dsm_ਆਖਰੀ_ਚੌਖਟਾ_ਸਮਾਂ = ਚੌਖਟਾ_ਸਮਾਂ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DSM_DEBUG
	ਲਿਖੋ("PARSED PACKET\n");
#ਜੇ_ਅੰਤ

	ਵਾਪਸ ਸੱਚ;
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
ਤਾਰਕਿਕ dsm_ਨਿਵੇਸ਼(ਪੂਰਨ fd, uint16_t *ਮੁੱਲਾਂ_2, uint16_t *ਅੰਕ_ਮੁੱਲਾਂ, ਤਾਰਕਿਕ *dsm_11_ਦੋਆਧਾਰੀ_ਅੰਕ, uint8_t *n_ਬਾਈਟਾਂ, uint8_t **ਬਾਈਟਾਂ,
	       int8_t *rssi, ਬਿਨਾਨਿਸ਼ਾਨੀ *ਚੌਖਟਾ_drops, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮੁੱਲਾਂ)
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
	ਅਚਲ hrt_abstime now = hrt_absolute_ਸਮਾਂ();

	/*
	 * Fetch bytes, but no more than we would need to complete
	 * a complete frame.
	 */

	ਪੂਰਨ ret = ਪਠ(fd, &dsm_buf[0], ਆਕਾਰ(dsm_buf) / ਆਕਾਰ(dsm_buf[0]));

	/* if the read failed for any reason, just give up here */
	ਜੇ (ret < 1) {
		ਵਾਪਸ ਝੂਠ;

	} ਨਹੀਂਤਾਂ {
		*n_ਬਾਈਟਾਂ = ret;
		*ਬਾਈਟਾਂ = &dsm_buf[0];
	}

	/*
	 * Try to decode something with what we got
	 */
	ਵਾਪਸ dsm_parse(now, &dsm_buf[0], ret, ਮੁੱਲਾਂ_2, ਅੰਕ_ਮੁੱਲਾਂ, dsm_11_ਦੋਆਧਾਰੀ_ਅੰਕ, &dsm_ਚੌਖਟਾ_drops, rssi, max_ਮੁੱਲਾਂ);
}

ਤਾਰਕਿਕ dsm_parse(ਅਚਲ uint64_t now, ਅਚਲ uint8_t *ਚੌਖਟਾ, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ len, uint16_t *ਮੁੱਲਾਂ_2,
	       uint16_t *ਅੰਕ_ਮੁੱਲਾਂ, ਤਾਰਕਿਕ *dsm_11_ਦੋਆਧਾਰੀ_ਅੰਕ, ਬਿਨਾਨਿਸ਼ਾਨੀ *ਚੌਖਟਾ_drops, int8_t *rssi_percent, uint16_t max_channels)
{
	/* this is set by the decoding state machine and will default to false
	 * once everything that was decodable has been decoded.
	 */
	ਤਾਰਕਿਕ channel_ਅੰਕੜੇ_ਉਪਲਬਧ = ਝੂਠ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ channel_output_ਗਿਣਤੀ;

	/* keep decoding until we have consumed the buffer */
	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ d = 0; d < len; d++) {

		/* overflow check */
		ਜੇ (dsm_partial_ਚੌਖਟਾ_ਗਿਣਤੀ == ਆਕਾਰ(dsm_ਚੌਖਟਾ) / ਆਕਾਰ(dsm_ਚੌਖਟਾ[0])) {
			dsm_partial_ਚੌਖਟਾ_ਗਿਣਤੀ = 0;
			dsm_decode_state = DSM_DECODE_STATE_DESYNC;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DSM_DEBUG
			ਲਿਖੋ("DSM: RESET (BUF LIM)\n");
#ਜੇ_ਅੰਤ
		}

		ਜੇ (dsm_partial_ਚੌਖਟਾ_ਗਿਣਤੀ == DSM_ਚੌਖਟਾ_ਮਾਪ) {
			dsm_partial_ਚੌਖਟਾ_ਗਿਣਤੀ = 0;
			dsm_decode_state = DSM_DECODE_STATE_DESYNC;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DSM_DEBUG
			ਲਿਖੋ("DSM: RESET (PACKET LIM)\n");
#ਜੇ_ਅੰਤ
		}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DSM_DEBUG
#ਜੇ 0
		ਲਿਖੋ("dsm state: %s%s, count: %d, val: %02x\n",
		       (dsm_decode_state == DSM_DECODE_STATE_DESYNC) ? "DSM_DECODE_STATE_DESYNC" : "",
		       (dsm_decode_state == DSM_DECODE_STATE_SYNC) ? "DSM_DECODE_STATE_SYNC" : "",
		       dsm_partial_ਚੌਖਟਾ_ਗਿਣਤੀ,
		       (ਬਿਨਾਨਿਸ਼ਾਨੀ)ਚੌਖਟਾ[d]);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

		ਚੋਣ (dsm_decode_state) {
		ਹਾਲਤ DSM_DECODE_STATE_DESYNC:

			/* we are de-synced and only interested in the frame marker */
			ਜੇ ((now - dsm_ਆਖਰੀ_rx_ਸਮਾਂ) > 5000) {
				dsm_decode_state = DSM_DECODE_STATE_SYNC;
				dsm_partial_ਚੌਖਟਾ_ਗਿਣਤੀ = 0;
				dsm_ਚੌਖਟਾ[dsm_partial_ਚੌਖਟਾ_ਗਿਣਤੀ++] = ਚੌਖਟਾ[d];
			}

			ਰੋਕੋ;

		ਹਾਲਤ DSM_DECODE_STATE_SYNC: {
				dsm_ਚੌਖਟਾ[dsm_partial_ਚੌਖਟਾ_ਗਿਣਤੀ++] = ਚੌਖਟਾ[d];

				/* decode whatever we got and expect */
				ਜੇ (dsm_partial_ਚੌਖਟਾ_ਗਿਣਤੀ < DSM_ਚੌਖਟਾ_ਮਾਪ) {
					ਰੋਕੋ;
				}

				// We've collected a full frame, parse it
				channel_ਅੰਕੜੇ_ਉਪਲਬਧ = dsm_decode(now, dsm_11_ਦੋਆਧਾਰੀ_ਅੰਕ, dsm_chan_ਗਿਣਤੀ, rssi_percent);

				// Frame consumed, reset the buffer count
				dsm_partial_ਚੌਖਟਾ_ਗਿਣਤੀ = 0;

				/* if decoding failed, set proto to desync */
				ਜੇ (!channel_ਅੰਕੜੇ_ਉਪਲਬਧ) {
					dsm_decode_state = DSM_DECODE_STATE_DESYNC;
				}
			}
			ਰੋਕੋ;

		ਮੂਲ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DSM_DEBUG
			ਲਿਖੋ("UNKNOWN PROTO STATE");
#ਜੇ_ਅੰਤ
			channel_ਅੰਕੜੇ_ਉਪਲਬਧ = ਝੂਠ;
		}
	}

	ਜੇ (ਚੌਖਟਾ_drops) {
		*ਚੌਖਟਾ_drops = dsm_ਚੌਖਟਾ_drops;
	}

	ਜੇ (channel_ਅੰਕੜੇ_ਉਪਲਬਧ) {
		// Check if any channels have been absent for too long
		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < dsm_chan_ਗਿਣਤੀ; i++) {
			ਜੇ (now - channel_ਬਫਰ[i].ਆਖਰੀ_ਵੇਖਿਆ > 50 * 1000) {
				ਵਾਪਸ ਝੂਠ;
			}
		}

		// Overflow protect
		ਜੇ (max_channels >= DSM_MAX_CHANNEL_ਗਿਣਤੀ) {
			max_channels = DSM_MAX_CHANNEL_ਗਿਣਤੀ;
		}

		// Cap returned channels to max_channels if we've seen more
		ਜੇ (dsm_chan_ਗਿਣਤੀ > max_channels) {
			channel_output_ਗਿਣਤੀ = max_channels;

		} ਨਹੀਂਤਾਂ {
			channel_output_ਗਿਣਤੀ = dsm_chan_ਗਿਣਤੀ;
		}

		*ਅੰਕ_ਮੁੱਲਾਂ = channel_output_ਗਿਣਤੀ;

		// Copy the full list of collected channels
		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < channel_output_ਗਿਣਤੀ; i++) {
			// Channels which do not have a value yet (because their frame has not been seen yet)
			// will have a zero. If we come across a blank channel, keep storing values, but we
			// won't return that we have a valid frame just yet.
			ਜੇ (channel_ਬਫਰ[i].value) {
				ਮੁੱਲਾਂ_2[i] = channel_ਬਫਰ[i].value;

			} ਨਹੀਂਤਾਂ {
				channel_ਅੰਕੜੇ_ਉਪਲਬਧ = ਝੂਠ;
			}
		}
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DSM_DEBUG

	ਜੇ (channel_ਅੰਕੜੇ_ਉਪਲਬਧ) {
		ਲਿਖੋ("frame drops: %u, chan #: %u\n", dsm_ਚੌਖਟਾ_drops, dsm_chan_ਗਿਣਤੀ);

		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < dsm_chan_ਗਿਣਤੀ; i++) {
			ਲਿਖੋ("dsm_decode: #CH %02u: %u\n", i + 1, ਮੁੱਲਾਂ_2[i]);
		}
	}

#ਜੇ_ਅੰਤ

	dsm_ਆਖਰੀ_rx_ਸਮਾਂ = now;

	/* return false as default */
	ਵਾਪਸ channel_ਅੰਕੜੇ_ਉਪਲਬਧ;
}
