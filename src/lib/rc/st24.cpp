/****************************************************************************
 *
 *   Copyright (c) 2014 PX4 Development Team. All rights reserved.
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

/*
 * @file st24.cpp
 *
 * RC protocol implementation for Yuneec ST24 transmitter.
 *
 * @author Lorenz Meier <lm@inf.ethz.ch>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdbool.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <cmath>
#ਅਨ੍ਤਰ੍ਭਾਵ "st24.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "common_rc.h"

ਅਚਲ ਅੱਖਰ *decode_ਹਾਲਤਾਂ[] = {"UNSYNCED",
			       "GOT_STX1",
			       "GOT_STX2",
			       "GOT_LEN",
			       "GOT_TYPE",
			       "GOT_DATA"
			      };

/* define range mapping here, -+100% -> 1000..2000 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ST24_RANGE_MIN 500.0f
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ST24_RANGE_MAX 3500.0f

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ST24_ਨਿਸ਼ਾਨਾ_MIN 1000.0f
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ST24_ਨਿਸ਼ਾਨਾ_MAX 2000.0f

/* pre-calculate the floating point stuff as far as possible at compile time */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ST24_SCALE_ਗੁਣਕ ((ST24_ਨਿਸ਼ਾਨਾ_MAX - ST24_ਨਿਸ਼ਾਨਾ_MIN) / (ST24_RANGE_MAX - ST24_RANGE_MIN))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ST24_SCALE_ਵਿੱਥ (ਪੂਰਨ)(ST24_ਨਿਸ਼ਾਨਾ_MIN - (ST24_SCALE_ਗੁਣਕ * ST24_RANGE_MIN + 0.5f))

ਸਥਿਰ ਗਿਣਤੀ ST24_DECODE_STATE _decode_state = ST24_DECODE_STATE_UNSYNCED;
ਸਥਿਰ uint8_t _rxlen;

ਸਥਿਰ ReceiverFcPacket &_rxpacket = rc_decode_buf._strxpacket;

uint8_t st24_common_crc8(uint8_t *ptr, uint8_t len)
{
	uint8_t i, ਚੱਕਰੀ_ਜਾਂਚ ;
	ਚੱਕਰੀ_ਜਾਂਚ = 0;

	ਜਦੋਂ (len--) {
		ਲਈ (i = 0x80; i != 0; i >>= 1) {
			ਜੇ ((ਚੱਕਰੀ_ਜਾਂਚ & 0x80) != 0) {
				ਚੱਕਰੀ_ਜਾਂਚ <<= 1;
				ਚੱਕਰੀ_ਜਾਂਚ ^= 0x07;

			} ਨਹੀਂਤਾਂ {
				ਚੱਕਰੀ_ਜਾਂਚ <<= 1;
			}

			ਜੇ ((*ptr & i) != 0) {
				ਚੱਕਰੀ_ਜਾਂਚ ^= 0x07;
			}
		}

		ptr++;
	}

	ਵਾਪਸ (ਚੱਕਰੀ_ਜਾਂਚ);
}


ਪੂਰਨ st24_decode(uint8_t byte, uint8_t *rssi, uint8_t *ਗੁਆਚਾ_ਗਿਣਤੀ, uint16_t *channel_ਗਿਣਤੀ, uint16_t *channels,
		uint16_t max_chan_ਗਿਣਤੀ)
{
	ਪੂਰਨ ret = 1;

	ਚੋਣ (_decode_state) {
	ਹਾਲਤ ST24_DECODE_STATE_UNSYNCED:
		ਜੇ (byte == ST24_STX1) {
			_decode_state = ST24_DECODE_STATE_GOT_STX1;

		} ਨਹੀਂਤਾਂ {
			ret = 3;
		}

		ਰੋਕੋ;

	ਹਾਲਤ ST24_DECODE_STATE_GOT_STX1:
		ਜੇ (byte == ST24_STX2) {
			_decode_state = ST24_DECODE_STATE_GOT_STX2;

		} ਨਹੀਂਤਾਂ {
			_decode_state = ST24_DECODE_STATE_UNSYNCED;
		}

		ਰੋਕੋ;

	ਹਾਲਤ ST24_DECODE_STATE_GOT_STX2:

		/* ensure no data overflow failure or hack is possible */
		ਜੇ ((ਬਿਨਾਨਿਸ਼ਾਨੀ)byte >= ਆਕਾਰ(_rxpacket.type) + ਆਕਾਰ(_rxpacket.crc8)
		    && (ਬਿਨਾਨਿਸ਼ਾਨੀ)byte <= ਆਕਾਰ(_rxpacket.type) + ਆਕਾਰ(_rxpacket.st24_ਅੰਕੜੇ) + ਆਕਾਰ(_rxpacket.crc8)) {
			_rxpacket.ਲੰਮਾਈ = byte;
			_rxlen = 0;
			_decode_state = ST24_DECODE_STATE_GOT_LEN;

		} ਨਹੀਂਤਾਂ {
			_decode_state = ST24_DECODE_STATE_UNSYNCED;
		}

		ਰੋਕੋ;

	ਹਾਲਤ ST24_DECODE_STATE_GOT_LEN:
		_rxpacket.type = byte;
		_rxlen++;
		_decode_state = (_rxlen == (_rxpacket.ਲੰਮਾਈ - 1)) ? ST24_DECODE_STATE_GOT_ਅੰਕੜੇ : ST24_DECODE_STATE_GOT_TYPE;
		ਰੋਕੋ;

	ਹਾਲਤ ST24_DECODE_STATE_GOT_TYPE:
		_rxpacket.st24_ਅੰਕੜੇ[_rxlen - 1] = byte;
		_rxlen++;

		ਜੇ (_rxlen == (_rxpacket.ਲੰਮਾਈ - 1)) {
			_decode_state = ST24_DECODE_STATE_GOT_ਅੰਕੜੇ;
		}

		ਰੋਕੋ;

	ਹਾਲਤ ST24_DECODE_STATE_GOT_ਅੰਕੜੇ:
		_rxpacket.crc8 = byte;
		_rxlen++;

		ਜੇ (st24_common_crc8((uint8_t *) & (_rxpacket.ਲੰਮਾਈ), _rxlen) == _rxpacket.crc8) {

			ret = 0;

			/* decode the actual packet */

			ਚੋਣ (_rxpacket.type) {

			ਹਾਲਤ ST24_PACKET_TYPE_CHANNELDATA12: {
					ChannelData12 *d = (ChannelData12 *)_rxpacket.st24_ਅੰਕੜੇ;

					// Scale from 0..255 to 100%.
					*rssi = d->rssi * (100.0f / 255.0f);
					*ਗੁਆਚਾ_ਗਿਣਤੀ = d->ਗੁਆਚਾ_ਗਿਣਤੀ;

					/* this can lead to rounding of the strides */
					*channel_ਗਿਣਤੀ = (max_chan_ਗਿਣਤੀ < 12) ? max_chan_ਗਿਣਤੀ : 12;

					ਬਿਨਾਨਿਸ਼ਾਨੀ stride_ਗਿਣਤੀ = (*channel_ਗਿਣਤੀ * 3) / 2;
					ਬਿਨਾਨਿਸ਼ਾਨੀ chan_ਸੂਚਕਾਂਕ = 0;

					ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < stride_ਗਿਣਤੀ; i += 3) {
						channels[chan_ਸੂਚਕਾਂਕ] = ((uint16_t)d->channel[i] << 4);
						channels[chan_ਸੂਚਕਾਂਕ] |= ((uint16_t)(0xF0 & d->channel[i + 1]) >> 4);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_ਸੂਚਕਾਂਕ] = (uint16_t)lroundf(channels[chan_ਸੂਚਕਾਂਕ] * ST24_SCALE_ਗੁਣਕ) + ST24_SCALE_ਵਿੱਥ;
						chan_ਸੂਚਕਾਂਕ++;

						channels[chan_ਸੂਚਕਾਂਕ] = ((uint16_t)d->channel[i + 2]);
						channels[chan_ਸੂਚਕਾਂਕ] |= (((uint16_t)(0x0F & d->channel[i + 1])) << 8);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_ਸੂਚਕਾਂਕ] = (uint16_t)lroundf(channels[chan_ਸੂਚਕਾਂਕ] * ST24_SCALE_ਗੁਣਕ) + ST24_SCALE_ਵਿੱਥ;
						chan_ਸੂਚਕਾਂਕ++;
					}
				}
				ਰੋਕੋ;

			ਹਾਲਤ ST24_PACKET_TYPE_CHANNELDATA24: {
					ChannelData24 *d = (ChannelData24 *)&_rxpacket.st24_ਅੰਕੜੇ;

					// Scale from 0..255 to 100%.
					*rssi = d->rssi * (100.0f / 255.0f);
					*ਗੁਆਚਾ_ਗਿਣਤੀ = d->ਗੁਆਚਾ_ਗਿਣਤੀ;

					/* this can lead to rounding of the strides */
					*channel_ਗਿਣਤੀ = (max_chan_ਗਿਣਤੀ < 24) ? max_chan_ਗਿਣਤੀ : 24;

					ਬਿਨਾਨਿਸ਼ਾਨੀ stride_ਗਿਣਤੀ = (*channel_ਗਿਣਤੀ * 3) / 2;
					ਬਿਨਾਨਿਸ਼ਾਨੀ chan_ਸੂਚਕਾਂਕ = 0;

					ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < stride_ਗਿਣਤੀ; i += 3) {
						channels[chan_ਸੂਚਕਾਂਕ] = ((uint16_t)d->channel[i] << 4);
						channels[chan_ਸੂਚਕਾਂਕ] |= ((uint16_t)(0xF0 & d->channel[i + 1]) >> 4);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_ਸੂਚਕਾਂਕ] = static_cast<uint16_t>(lroundf(channels[chan_ਸੂਚਕਾਂਕ] * ST24_SCALE_ਗੁਣਕ) + ST24_SCALE_ਵਿੱਥ);
						chan_ਸੂਚਕਾਂਕ++;

						channels[chan_ਸੂਚਕਾਂਕ] = ((uint16_t)d->channel[i + 2]);
						channels[chan_ਸੂਚਕਾਂਕ] |= (((uint16_t)(0x0F & d->channel[i + 1])) << 8);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_ਸੂਚਕਾਂਕ] = static_cast<uint16_t>(lroundf(channels[chan_ਸੂਚਕਾਂਕ] * ST24_SCALE_ਗੁਣਕ) + ST24_SCALE_ਵਿੱਥ);
						chan_ਸੂਚਕਾਂਕ++;
					}
				}
				ਰੋਕੋ;

			ਹਾਲਤ ST24_PACKET_TYPE_TRANSMITTERGPSDATA: {

					// ReceiverFcPacket* d = (ReceiverFcPacket*)&_rxpacket.st24_data;
					/* we silently ignore this data for now, as it is unused */
					ret = 5;
				}
				ਰੋਕੋ;

			ਮੂਲ:
				ret = 2;
				ਰੋਕੋ;
			}

		} ਨਹੀਂਤਾਂ {
			/* decoding failed */
			ret = 4;
		}

		_decode_state = ST24_DECODE_STATE_UNSYNCED;
		ਰੋਕੋ;
	}

	ਵਾਪਸ ret;
}
