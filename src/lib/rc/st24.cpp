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

#ଅନ୍ତର୍ଭାଵ <stdbool.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <cmath>
#ଅନ୍ତର୍ଭାଵ "st24.h"
#ଅନ୍ତର୍ଭାଵ "common_rc.h"

ଅଚଳ ଅକ୍ଷର *decode_states[] = {"UNSYNCED",
			       "GOT_STX1",
			       "GOT_STX2",
			       "GOT_LEN",
			       "GOT_TYPE",
			       "GOT_DATA"
			      };

/* define range mapping here, -+100% -> 1000..2000 */
#define ST24_RANGE_MIN 500.0f
#define ST24_RANGE_MAX 3500.0f

#define ST24_TARGET_MIN 1000.0f
#define ST24_TARGET_MAX 2000.0f

/* pre-calculate the floating point stuff as far as possible at compile time */
#define ST24_SCALE_FACTOR ((ST24_TARGET_MAX - ST24_TARGET_MIN) / (ST24_RANGE_MAX - ST24_RANGE_MIN))
#define ST24_SCALE_OFFSET (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ST24_TARGET_MIN - (ST24_SCALE_FACTOR * ST24_RANGE_MIN + 0.5f))

ସ୍ଥିର ଗଣନା ST24_DECODE_STATE _decode_state = ST24_DECODE_STATE_UNSYNCED;
ସ୍ଥିର uint8_t _rxlen;

ସ୍ଥିର ReceiverFcPacket &_rxpacket = rc_decode_buf._strxpacket;

uint8_t st24_common_crc8(uint8_t *ptr, uint8_t len)
{
	uint8_t i, crc ;
	crc = 0;

	ଯେତେବେଳେ (len--) {
		ପାଇଁ (i = 0x80; i != 0; i >>= 1) {
			ଯଦି ((crc & 0x80) != 0) {
				crc <<= 1;
				crc ^= 0x07;

			} ଅନ୍ୟଥା {
				crc <<= 1;
			}

			ଯଦି ((*ptr & i) != 0) {
				crc ^= 0x07;
			}
		}

		ptr++;
	}

	ଫେରନ୍ତୁ (crc);
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା st24_decode(uint8_t byte, uint8_t *rssi, uint8_t *lost_ଗଣନା, uint16_t *channel_ଗଣନା, uint16_t *channels,
		uint16_t max_chan_ଗଣନା)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 1;

	ବାଛ (_decode_state) {
	ସ୍ଥିତି ST24_DECODE_STATE_UNSYNCED:
		ଯଦି (byte == ST24_STX1) {
			_decode_state = ST24_DECODE_STATE_GOT_STX1;

		} ଅନ୍ୟଥା {
			ret = 3;
		}

		ଭାଙ୍ଗ;

	ସ୍ଥିତି ST24_DECODE_STATE_GOT_STX1:
		ଯଦି (byte == ST24_STX2) {
			_decode_state = ST24_DECODE_STATE_GOT_STX2;

		} ଅନ୍ୟଥା {
			_decode_state = ST24_DECODE_STATE_UNSYNCED;
		}

		ଭାଙ୍ଗ;

	ସ୍ଥିତି ST24_DECODE_STATE_GOT_STX2:

		/* ensure no data overflow failure or hack is possible */
		ଯଦି ((unsigned)byte >= ଆକାର(_rxpacket.type) + ଆକାର(_rxpacket.crc8)
		    && (unsigned)byte <= ଆକାର(_rxpacket.type) + ଆକାର(_rxpacket.st24_data) + ଆକାର(_rxpacket.crc8)) {
			_rxpacket.length = byte;
			_rxlen = 0;
			_decode_state = ST24_DECODE_STATE_GOT_LEN;

		} ଅନ୍ୟଥା {
			_decode_state = ST24_DECODE_STATE_UNSYNCED;
		}

		ଭାଙ୍ଗ;

	ସ୍ଥିତି ST24_DECODE_STATE_GOT_LEN:
		_rxpacket.type = byte;
		_rxlen++;
		_decode_state = (_rxlen == (_rxpacket.length - 1)) ? ST24_DECODE_STATE_GOT_DATA : ST24_DECODE_STATE_GOT_TYPE;
		ଭାଙ୍ଗ;

	ସ୍ଥିତି ST24_DECODE_STATE_GOT_TYPE:
		_rxpacket.st24_data[_rxlen - 1] = byte;
		_rxlen++;

		ଯଦି (_rxlen == (_rxpacket.length - 1)) {
			_decode_state = ST24_DECODE_STATE_GOT_DATA;
		}

		ଭାଙ୍ଗ;

	ସ୍ଥିତି ST24_DECODE_STATE_GOT_DATA:
		_rxpacket.crc8 = byte;
		_rxlen++;

		ଯଦି (st24_common_crc8((uint8_t *) & (_rxpacket.length), _rxlen) == _rxpacket.crc8) {

			ret = 0;

			/* decode the actual packet */

			ବାଛ (_rxpacket.type) {

			ସ୍ଥିତି ST24_PACKET_TYPE_CHANNELDATA12: {
					ChannelData12 *d = (ChannelData12 *)_rxpacket.st24_data;

					// Scale from 0..255 to 100%.
					*rssi = d->rssi * (100.0f / 255.0f);
					*lost_ଗଣନା = d->lost_ଗଣନା;

					/* this can lead to rounding of the strides */
					*channel_ଗଣନା = (max_chan_ଗଣନା < 12) ? max_chan_ଗଣନା : 12;

					unsigned stride_ଗଣନା = (*channel_ଗଣନା * 3) / 2;
					unsigned chan_index = 0;

					ପାଇଁ (unsigned i = 0; i < stride_ଗଣନା; i += 3) {
						channels[chan_index] = ((uint16_t)d->channel[i] << 4);
						channels[chan_index] |= ((uint16_t)(0xF0 & d->channel[i + 1]) >> 4);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_index] = (uint16_t)lroundf(channels[chan_index] * ST24_SCALE_FACTOR) + ST24_SCALE_OFFSET;
						chan_index++;

						channels[chan_index] = ((uint16_t)d->channel[i + 2]);
						channels[chan_index] |= (((uint16_t)(0x0F & d->channel[i + 1])) << 8);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_index] = (uint16_t)lroundf(channels[chan_index] * ST24_SCALE_FACTOR) + ST24_SCALE_OFFSET;
						chan_index++;
					}
				}
				ଭାଙ୍ଗ;

			ସ୍ଥିତି ST24_PACKET_TYPE_CHANNELDATA24: {
					ChannelData24 *d = (ChannelData24 *)&_rxpacket.st24_data;

					// Scale from 0..255 to 100%.
					*rssi = d->rssi * (100.0f / 255.0f);
					*lost_ଗଣନା = d->lost_ଗଣନା;

					/* this can lead to rounding of the strides */
					*channel_ଗଣନା = (max_chan_ଗଣନା < 24) ? max_chan_ଗଣନା : 24;

					unsigned stride_ଗଣନା = (*channel_ଗଣନା * 3) / 2;
					unsigned chan_index = 0;

					ପାଇଁ (unsigned i = 0; i < stride_ଗଣନା; i += 3) {
						channels[chan_index] = ((uint16_t)d->channel[i] << 4);
						channels[chan_index] |= ((uint16_t)(0xF0 & d->channel[i + 1]) >> 4);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_index] = static_cast<uint16_t>(lroundf(channels[chan_index] * ST24_SCALE_FACTOR) + ST24_SCALE_OFFSET);
						chan_index++;

						channels[chan_index] = ((uint16_t)d->channel[i + 2]);
						channels[chan_index] |= (((uint16_t)(0x0F & d->channel[i + 1])) << 8);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_index] = static_cast<uint16_t>(lroundf(channels[chan_index] * ST24_SCALE_FACTOR) + ST24_SCALE_OFFSET);
						chan_index++;
					}
				}
				ଭାଙ୍ଗ;

			ସ୍ଥିତି ST24_PACKET_TYPE_TRANSMITTERGPSDATA: {

					// ReceiverFcPacket* d = (ReceiverFcPacket*)&_rxpacket.st24_data;
					/* we silently ignore this data for now, as it is unused */
					ret = 5;
				}
				ଭାଙ୍ଗ;

			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
				ret = 2;
				ଭାଙ୍ଗ;
			}

		} ଅନ୍ୟଥା {
			/* decoding failed */
			ret = 4;
		}

		_decode_state = ST24_DECODE_STATE_UNSYNCED;
		ଭାଙ୍ଗ;
	}

	ଫେରନ୍ତୁ ret;
}
