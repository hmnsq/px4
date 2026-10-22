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

#ضمن <stdbool.h>
#ضمن <stdio.h>
#ضمن <cmath>
#ضمن "st24.h"
#ضمن "common_rc.h"

ثابت حرف *decode_حالتون[] = {"UNSYNCED",
			       "GOT_STX1",
			       "GOT_STX2",
			       "GOT_LEN",
			       "GOT_TYPE",
			       "GOT_DATA"
			      };

/* define range mapping here, -+100% -> 1000..2000 */
#وصف_ڪريو ST24_RANGE_MIN 500.0f
#وصف_ڪريو ST24_RANGE_MAX 3500.0f

#وصف_ڪريو ST24_هدف_MIN 1000.0f
#وصف_ڪريو ST24_هدف_MAX 2000.0f

/* pre-calculate the floating point stuff as far as possible at compile time */
#وصف_ڪريو ST24_SCALE_ضرب_ڪندڙ ((ST24_هدف_MAX - ST24_هدف_MIN) / (ST24_RANGE_MAX - ST24_RANGE_MIN))
#وصف_ڪريو ST24_SCALE_وٿي (صحيح)(ST24_هدف_MIN - (ST24_SCALE_ضرب_ڪندڙ * ST24_RANGE_MIN + 0.5f))

ساكن تعداد ST24_DECODE_STATE _decode_state = ST24_DECODE_STATE_UNSYNCED;
ساكن uint8_t _rxlen;

ساكن ReceiverFcPacket &_rxpacket = rc_decode_buf._strxpacket;

uint8_t st24_common_crc8(uint8_t *ptr, uint8_t len)
{
	uint8_t i, چڪري_جاچ ;
	چڪري_جاچ = 0;

	طالما (len--) {
		لكل (i = 0x80; i != 0; i >>= 1) {
			إذا ((چڪري_جاچ & 0x80) != 0) {
				چڪري_جاچ <<= 1;
				چڪري_جاچ ^= 0x07;

			} وإلا {
				چڪري_جاچ <<= 1;
			}

			إذا ((*ptr & i) != 0) {
				چڪري_جاچ ^= 0x07;
			}
		}

		ptr++;
	}

	أرجع (چڪري_جاچ);
}


صحيح st24_decode(uint8_t byte, uint8_t *rssi, uint8_t *گم_ڳڻپ, uint16_t *channel_ڳڻپ, uint16_t *channels,
		uint16_t max_chan_ڳڻپ)
{
	صحيح ret = 1;

	بدل (_decode_state) {
	حالة ST24_DECODE_STATE_UNSYNCED:
		إذا (byte == ST24_STX1) {
			_decode_state = ST24_DECODE_STATE_GOT_STX1;

		} وإلا {
			ret = 3;
		}

		اكسر;

	حالة ST24_DECODE_STATE_GOT_STX1:
		إذا (byte == ST24_STX2) {
			_decode_state = ST24_DECODE_STATE_GOT_STX2;

		} وإلا {
			_decode_state = ST24_DECODE_STATE_UNSYNCED;
		}

		اكسر;

	حالة ST24_DECODE_STATE_GOT_STX2:

		/* ensure no data overflow failure or hack is possible */
		إذا ((بنانشان)byte >= حجم(_rxpacket.type) + حجم(_rxpacket.crc8)
		    && (بنانشان)byte <= حجم(_rxpacket.type) + حجم(_rxpacket.st24_معلومات) + حجم(_rxpacket.crc8)) {
			_rxpacket.ڊيگهائي = byte;
			_rxlen = 0;
			_decode_state = ST24_DECODE_STATE_GOT_LEN;

		} وإلا {
			_decode_state = ST24_DECODE_STATE_UNSYNCED;
		}

		اكسر;

	حالة ST24_DECODE_STATE_GOT_LEN:
		_rxpacket.type = byte;
		_rxlen++;
		_decode_state = (_rxlen == (_rxpacket.ڊيگهائي - 1)) ? ST24_DECODE_STATE_GOT_معلومات : ST24_DECODE_STATE_GOT_TYPE;
		اكسر;

	حالة ST24_DECODE_STATE_GOT_TYPE:
		_rxpacket.st24_معلومات[_rxlen - 1] = byte;
		_rxlen++;

		إذا (_rxlen == (_rxpacket.ڊيگهائي - 1)) {
			_decode_state = ST24_DECODE_STATE_GOT_معلومات;
		}

		اكسر;

	حالة ST24_DECODE_STATE_GOT_معلومات:
		_rxpacket.crc8 = byte;
		_rxlen++;

		إذا (st24_common_crc8((uint8_t *) & (_rxpacket.ڊيگهائي), _rxlen) == _rxpacket.crc8) {

			ret = 0;

			/* decode the actual packet */

			بدل (_rxpacket.type) {

			حالة ST24_PACKET_TYPE_CHANNELDATA12: {
					ChannelData12 *d = (ChannelData12 *)_rxpacket.st24_معلومات;

					// Scale from 0..255 to 100%.
					*rssi = d->rssi * (100.0f / 255.0f);
					*گم_ڳڻپ = d->گم_ڳڻپ;

					/* this can lead to rounding of the strides */
					*channel_ڳڻپ = (max_chan_ڳڻپ < 12) ? max_chan_ڳڻپ : 12;

					بنانشان stride_ڳڻپ = (*channel_ڳڻپ * 3) / 2;
					بنانشان chan_اشاريو = 0;

					لكل (بنانشان i = 0; i < stride_ڳڻپ; i += 3) {
						channels[chan_اشاريو] = ((uint16_t)d->channel[i] << 4);
						channels[chan_اشاريو] |= ((uint16_t)(0xF0 & d->channel[i + 1]) >> 4);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_اشاريو] = (uint16_t)lroundf(channels[chan_اشاريو] * ST24_SCALE_ضرب_ڪندڙ) + ST24_SCALE_وٿي;
						chan_اشاريو++;

						channels[chan_اشاريو] = ((uint16_t)d->channel[i + 2]);
						channels[chan_اشاريو] |= (((uint16_t)(0x0F & d->channel[i + 1])) << 8);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_اشاريو] = (uint16_t)lroundf(channels[chan_اشاريو] * ST24_SCALE_ضرب_ڪندڙ) + ST24_SCALE_وٿي;
						chan_اشاريو++;
					}
				}
				اكسر;

			حالة ST24_PACKET_TYPE_CHANNELDATA24: {
					ChannelData24 *d = (ChannelData24 *)&_rxpacket.st24_معلومات;

					// Scale from 0..255 to 100%.
					*rssi = d->rssi * (100.0f / 255.0f);
					*گم_ڳڻپ = d->گم_ڳڻپ;

					/* this can lead to rounding of the strides */
					*channel_ڳڻپ = (max_chan_ڳڻپ < 24) ? max_chan_ڳڻپ : 24;

					بنانشان stride_ڳڻپ = (*channel_ڳڻپ * 3) / 2;
					بنانشان chan_اشاريو = 0;

					لكل (بنانشان i = 0; i < stride_ڳڻپ; i += 3) {
						channels[chan_اشاريو] = ((uint16_t)d->channel[i] << 4);
						channels[chan_اشاريو] |= ((uint16_t)(0xF0 & d->channel[i + 1]) >> 4);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_اشاريو] = static_cast<uint16_t>(lroundf(channels[chan_اشاريو] * ST24_SCALE_ضرب_ڪندڙ) + ST24_SCALE_وٿي);
						chan_اشاريو++;

						channels[chan_اشاريو] = ((uint16_t)d->channel[i + 2]);
						channels[chan_اشاريو] |= (((uint16_t)(0x0F & d->channel[i + 1])) << 8);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_اشاريو] = static_cast<uint16_t>(lroundf(channels[chan_اشاريو] * ST24_SCALE_ضرب_ڪندڙ) + ST24_SCALE_وٿي);
						chan_اشاريو++;
					}
				}
				اكسر;

			حالة ST24_PACKET_TYPE_TRANSMITTERGPSDATA: {

					// ReceiverFcPacket* d = (ReceiverFcPacket*)&_rxpacket.st24_data;
					/* we silently ignore this data for now, as it is unused */
					ret = 5;
				}
				اكسر;

			افتراضي:
				ret = 2;
				اكسر;
			}

		} وإلا {
			/* decoding failed */
			ret = 4;
		}

		_decode_state = ST24_DECODE_STATE_UNSYNCED;
		اكسر;
	}

	أرجع ret;
}
