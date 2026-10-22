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

#شامل <stdbool.h>
#شامل <stdio.h>
#شامل <cmath>
#شامل "st24.h"
#شامل "common_rc.h"

ثابت حرف *decode_حالتیں[] = {"UNSYNCED",
			       "GOT_STX1",
			       "GOT_STX2",
			       "GOT_LEN",
			       "GOT_TYPE",
			       "GOT_DATA"
			      };

/* define range mapping here, -+100% -> 1000..2000 */
#تعریف_کرو ST24_RANGE_MIN 500.0f
#تعریف_کرو ST24_RANGE_MAX 3500.0f

#تعریف_کرو ST24_ہدف_MIN 1000.0f
#تعریف_کرو ST24_ہدف_MAX 2000.0f

/* pre-calculate the floating point stuff as far as possible at compile time */
#تعریف_کرو ST24_SCALE_ضربی_عدد ((ST24_ہدف_MAX - ST24_ہدف_MIN) / (ST24_RANGE_MAX - ST24_RANGE_MIN))
#تعریف_کرو ST24_SCALE_انحراف (صحیح)(ST24_ہدف_MIN - (ST24_SCALE_ضربی_عدد * ST24_RANGE_MIN + 0.5f))

ساکن شمار ST24_DECODE_STATE _decode_state = ST24_DECODE_STATE_UNSYNCED;
ساکن uint8_t _rxlen;

ساکن ReceiverFcPacket &_rxpacket = rc_decode_buf._strxpacket;

uint8_t st24_common_crc8(uint8_t *ptr, uint8_t len)
{
	uint8_t i, چکری_جانچ ;
	چکری_جانچ = 0;

	جب (len--) {
		برائے (i = 0x80; i != 0; i >>= 1) {
			اگر ((چکری_جانچ & 0x80) != 0) {
				چکری_جانچ <<= 1;
				چکری_جانچ ^= 0x07;

			} ورنہ {
				چکری_جانچ <<= 1;
			}

			اگر ((*ptr & i) != 0) {
				چکری_جانچ ^= 0x07;
			}
		}

		ptr++;
	}

	واپس (چکری_جانچ);
}


صحیح st24_decode(uint8_t byte, uint8_t *rssi, uint8_t *گمشدہ_گنتی, uint16_t *channel_گنتی, uint16_t *channels,
		uint16_t max_chan_گنتی)
{
	صحیح ret = 1;

	انتخاب (_decode_state) {
	حالت ST24_DECODE_STATE_UNSYNCED:
		اگر (byte == ST24_STX1) {
			_decode_state = ST24_DECODE_STATE_GOT_STX1;

		} ورنہ {
			ret = 3;
		}

		توڑو;

	حالت ST24_DECODE_STATE_GOT_STX1:
		اگر (byte == ST24_STX2) {
			_decode_state = ST24_DECODE_STATE_GOT_STX2;

		} ورنہ {
			_decode_state = ST24_DECODE_STATE_UNSYNCED;
		}

		توڑو;

	حالت ST24_DECODE_STATE_GOT_STX2:

		/* ensure no data overflow failure or hack is possible */
		اگر ((غیرعلامتی)byte >= حجم(_rxpacket.type) + حجم(_rxpacket.crc8)
		    && (غیرعلامتی)byte <= حجم(_rxpacket.type) + حجم(_rxpacket.st24_ڈیٹا) + حجم(_rxpacket.crc8)) {
			_rxpacket.لمبائی_2 = byte;
			_rxlen = 0;
			_decode_state = ST24_DECODE_STATE_GOT_LEN;

		} ورنہ {
			_decode_state = ST24_DECODE_STATE_UNSYNCED;
		}

		توڑو;

	حالت ST24_DECODE_STATE_GOT_LEN:
		_rxpacket.type = byte;
		_rxlen++;
		_decode_state = (_rxlen == (_rxpacket.لمبائی_2 - 1)) ? ST24_DECODE_STATE_GOT_ڈیٹا : ST24_DECODE_STATE_GOT_TYPE;
		توڑو;

	حالت ST24_DECODE_STATE_GOT_TYPE:
		_rxpacket.st24_ڈیٹا[_rxlen - 1] = byte;
		_rxlen++;

		اگر (_rxlen == (_rxpacket.لمبائی_2 - 1)) {
			_decode_state = ST24_DECODE_STATE_GOT_ڈیٹا;
		}

		توڑو;

	حالت ST24_DECODE_STATE_GOT_ڈیٹا:
		_rxpacket.crc8 = byte;
		_rxlen++;

		اگر (st24_common_crc8((uint8_t *) & (_rxpacket.لمبائی_2), _rxlen) == _rxpacket.crc8) {

			ret = 0;

			/* decode the actual packet */

			انتخاب (_rxpacket.type) {

			حالت ST24_PACKET_TYPE_CHANNELDATA12: {
					ChannelData12 *d = (ChannelData12 *)_rxpacket.st24_ڈیٹا;

					// Scale from 0..255 to 100%.
					*rssi = d->rssi * (100.0f / 255.0f);
					*گمشدہ_گنتی = d->گمشدہ_گنتی;

					/* this can lead to rounding of the strides */
					*channel_گنتی = (max_chan_گنتی < 12) ? max_chan_گنتی : 12;

					غیرعلامتی stride_گنتی = (*channel_گنتی * 3) / 2;
					غیرعلامتی chan_اشاریہ = 0;

					برائے (غیرعلامتی i = 0; i < stride_گنتی; i += 3) {
						channels[chan_اشاریہ] = ((uint16_t)d->channel[i] << 4);
						channels[chan_اشاریہ] |= ((uint16_t)(0xF0 & d->channel[i + 1]) >> 4);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_اشاریہ] = (uint16_t)lroundf(channels[chan_اشاریہ] * ST24_SCALE_ضربی_عدد) + ST24_SCALE_انحراف;
						chan_اشاریہ++;

						channels[chan_اشاریہ] = ((uint16_t)d->channel[i + 2]);
						channels[chan_اشاریہ] |= (((uint16_t)(0x0F & d->channel[i + 1])) << 8);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_اشاریہ] = (uint16_t)lroundf(channels[chan_اشاریہ] * ST24_SCALE_ضربی_عدد) + ST24_SCALE_انحراف;
						chan_اشاریہ++;
					}
				}
				توڑو;

			حالت ST24_PACKET_TYPE_CHANNELDATA24: {
					ChannelData24 *d = (ChannelData24 *)&_rxpacket.st24_ڈیٹا;

					// Scale from 0..255 to 100%.
					*rssi = d->rssi * (100.0f / 255.0f);
					*گمشدہ_گنتی = d->گمشدہ_گنتی;

					/* this can lead to rounding of the strides */
					*channel_گنتی = (max_chan_گنتی < 24) ? max_chan_گنتی : 24;

					غیرعلامتی stride_گنتی = (*channel_گنتی * 3) / 2;
					غیرعلامتی chan_اشاریہ = 0;

					برائے (غیرعلامتی i = 0; i < stride_گنتی; i += 3) {
						channels[chan_اشاریہ] = ((uint16_t)d->channel[i] << 4);
						channels[chan_اشاریہ] |= ((uint16_t)(0xF0 & d->channel[i + 1]) >> 4);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_اشاریہ] = static_cast<uint16_t>(lroundf(channels[chan_اشاریہ] * ST24_SCALE_ضربی_عدد) + ST24_SCALE_انحراف);
						chan_اشاریہ++;

						channels[chan_اشاریہ] = ((uint16_t)d->channel[i + 2]);
						channels[chan_اشاریہ] |= (((uint16_t)(0x0F & d->channel[i + 1])) << 8);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_اشاریہ] = static_cast<uint16_t>(lroundf(channels[chan_اشاریہ] * ST24_SCALE_ضربی_عدد) + ST24_SCALE_انحراف);
						chan_اشاریہ++;
					}
				}
				توڑو;

			حالت ST24_PACKET_TYPE_TRANSMITTERGPSDATA: {

					// ReceiverFcPacket* d = (ReceiverFcPacket*)&_rxpacket.st24_data;
					/* we silently ignore this data for now, as it is unused */
					ret = 5;
				}
				توڑو;

			طےشدہ:
				ret = 2;
				توڑو;
			}

		} ورنہ {
			/* decoding failed */
			ret = 4;
		}

		_decode_state = ST24_DECODE_STATE_UNSYNCED;
		توڑو;
	}

	واپس ret;
}
